#include "TCPSocket.hpp"
#include "shotData.hpp"
#include "shotParser.hpp"
#include <cstring>
#include <sys/socket.h>

int TCPSocket::init_socket() {

	// Create a socket file descriptor and make it non blocking
	socketfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (socketfd == -1) {
		perror("webserver (socket)");
		return 1;
	}

    // Create the address to bind the socket to
    addrlen = sizeof(address);

    address.sin_family = AF_INET;
    address.sin_port = htons(PORT); // host to network endian conversion
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    // Create client address
    client_addrlen = sizeof(client_addr);

    // Bind the socket to the address
    if (bind(socketfd, (struct sockaddr *)&address, addrlen) != 0) {
            perror("Webserver (bind");
            return 1;
    }

    // Listen for incoming connections
    if (listen(socketfd, SOMAXCONN) != 0) {
        perror("webserver (listen)");
        return 1;
    }

    return 0;
}

void TCPSocket::run_socket(t_ball_data *ball_data, bool *should_close, std::mutex *ball_mtx, std::mutex *close_mtx) {
    char *resp = nullptr;
    printf("Waiting for incoming connections\n");

    // Get incoming connections
    while (1) {
        // lock mutex for should close
        close_mtx->lock();
        if (*should_close) {
            // unlock mutex for should close
            close_mtx->unlock();
            close(socketfd);
            break;
        }
        close_mtx->unlock();

        // Accept incoming connections
        // Non-blocking accept call
        this->newsocket = -1;
        this->newsocket = accept(socketfd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addrlen);

        if (this->newsocket < 0) {
            continue; // no connection detected
        }
        else {
            printf("Connection accepted\n");
        }

        // loop forever, waiting to read from socket
        while(1) {
            // lock mutex for should close
            close_mtx->lock();
            if (*should_close) break;
            // unlock mutex for should close
            close_mtx->unlock();

            // Read from the socket
            int valread = read(this->newsocket, this->json_data, BUFFER_SIZE);
            if (valread < 0) {
                printf("Nothing to read right now\n");
                continue;
            }
            else break;
        }
        printf("Data read from socket\n");

        // Parse json_data into shot_data
        std::string shot_string = this->json_data;
        t_shot_data shot_data;
        try {
            shot_data = parse_json_shot_string(shot_string);
            shot_data.ball_data.status = VALID;
        }
        catch (...) {
            printf("No valid json data to read");
            shot_data.ball_data.status = INVALID;
            // set response to failure
            resp = (char*)malloc((strlen(resp_501)+1)*sizeof(char));
            strncpy(resp, resp_501, strlen(resp_501)+1);
        }
        printf("Valid json parsed from data\n");

        // set json_data to empty to stop re-shooting
        this->json_data[0] = '\0';


        if (ball_mtx->try_lock()) { // try to get the lock
            printf("Able to get lock on ball data\n");
            if (shot_data.shot_options.containsBallData and shot_data.ball_data.status != INVALID) {
                if (ball_data->status == STALE) {
                    *ball_data = shot_data.ball_data;
                    ball_data->status = VALID;
                    printf("Ball data written\n");
                    // set response to success
                    resp = (char*)malloc((strlen(resp_200)+1)*sizeof(char));
                    strncpy(resp, resp_200, strlen(resp_200)+1);
                }
            }
            ball_mtx->unlock();
        } 
        else printf("Unable to get lock on ball data\n");// Otherwise, the main thread is still consuming ball data. Ignore shot

        // Send response
        if (resp != nullptr) {
            int valsend = send(newsocket, resp, strlen(resp)+1, 0);
        }

        free(resp);
        resp = nullptr;
        close(this->newsocket);
    }

    
    close(this->socketfd);
    return;
}