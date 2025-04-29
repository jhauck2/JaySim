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
    char resp[BUFFER_SIZE] = { 0 };
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

        printf("Waiting for data to be read from connection\n");

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
        }
        catch (...) {
            printf("No valid json data to read");
            continue;
        }

        // set json_data to empty to stop re-shooting
        this->json_data[0] = '\0';


        if (ball_mtx->try_lock()) { // try to get the lock
            if (shot_data.shot_options.containsBallData) {
                if (ball_data->status == STALE) {
                    *ball_data = shot_data.ball_data;
                    ball_data->status = VALID;
                    printf("Ball data written\n");
                }
            }
            ball_mtx->unlock();
        } 
        else printf("Unable to get lock on ball data\n");// Otherwise, the main thread is still consuming ball data. Ignore shot

        close(this->newsocket);
    }

    
    close(this->socketfd);
    return;
}