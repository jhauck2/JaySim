#include "TCPSocket.hpp"
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
            return;
        }
        close_mtx->unlock();

        // Accept incoming connections
        // Non-blocking accept call
        newsocket = accept(socketfd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addrlen);

        if (newsocket < 0) {
            // no new connections
        }
        else {
            printf("Connection accepted\n");
            break;
        }

        
    }

    // Get the client address
    int sockn = getsockname(newsocket, (struct sockaddr *)&client_addr, (socklen_t *)&client_addrlen);

    if (sockn < 0) {
        perror("webserver (getsockname)");
        close(socketfd);
        return;
    }

    // loop forever, listening for connections
    while(1) {
        // lock mutex for should close
        close_mtx->lock();
        if (*should_close) break;
        // unlock mutex for should close
        close_mtx->unlock();

        // Read from the socket
        int valread = read(newsocket, json_data, BUFFER_SIZE);
        if (valread < 0) {
            perror("webserver (read)");
            continue;
        }

        // Parse json_data into shot_data
        std::string shot_string = json_data;
        t_shot_data shot_data = parse_json_shot_string(shot_string);

        if (ball_mtx->try_lock()) { // try to get the lock
            if (shot_data.shot_options.containsBallData && ball_data->status == STALE) {
                *ball_data = shot_data.ball_data;
                ball_data->status = VALID;
            }
            ball_mtx->unlock();

            // Write to the socket
            int valwrite = write(newsocket, resp, strlen(resp));
            if (valwrite < 0 ) {
                perror("webserver (write");
                continue;
            }
        } // Otherwise, the main thread is still consuming ball data. Ignore shot
    }
    close(newsocket);
    close(socketfd);
}