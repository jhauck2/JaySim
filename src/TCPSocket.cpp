#include "TCPSocket.hpp"
#include "shotData.hpp"
#include "shotParser.hpp"
#include <cstring>


int TCPSocket::init_socket() {
    // Linux Implementation
    // -----------------------------------------------------------------------

    #ifdef PLATFORM_LINUX

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
    #endif
    // End Linux implementation

    // Windows Implementation
    // ----------------------------------------------------------------------
    #ifdef PLATFORM_WINDOWS

    int result;
    WSADATA wsaData; // gets populated w/ info explaining this sockets implementation
    // load Winsock 2.0 DLL. initiates use of the Winsock DLL by a process
    if ((result = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
    {
        printf("WSAStartup() error %d\n", result);
        return result;
    }

    //atexit(finalWSACleanup); // add callback to trigger when program ends. cleans up sockets
    // create the main socket, either client or server
    socketfd = (int)socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if ((SOCKET)socketfd == INVALID_SOCKET)
    {
        printf("socket() error %d\n", WSAGetLastError());
        return EXIT_FAILURE;
    }
    // 1 to set non-blocking, 0 to set re-usable
    unsigned long argp = 1;
    // for complete info on this method, check out http://msdn.microsoft.com/en-us/library/ms740476(VS.85).aspx
    result = setsockopt((SOCKET)socketfd, SOL_SOCKET, SO_REUSEADDR, (char*)&argp, sizeof(argp));

    if (result != 0) {
        printf("setsockopt() error %d\n", result);
        return result;
    }
    // 1 to set non-blocking, 0 to set blocking
    argp = 1;
    // attempt to setup the socket as non-blocking
    if (ioctlsocket((SOCKET)socketfd, FIONBIO, &argp) == SOCKET_ERROR) {
        printf("ioctlsocket() error %d\n", WSAGetLastError());
        return EXIT_FAILURE;
    }

    // start listening on the server
    result = bind((SOCKET)socketfd, (sockaddr *)(&address), sizeof(address));
    if (result == SOCKET_ERROR)
    {
        printf("bind() error %d\n", WSAGetLastError());
        return EXIT_FAILURE;
    }
    result = listen((SOCKET)socketfd, /* size of connection queue */10);
    if (result == SOCKET_ERROR)
    {
        printf("listen() error %d\n", WSAGetLastError());
        return EXIT_FAILURE;
    }

    return 0;

    #endif
    // End Windows implementation
}

void TCPSocket::run_socket(t_ball_data *ball_data, bool *should_close, std::mutex *ball_mtx, std::mutex *close_mtx) {
    // Linux Implementation
    // -----------------------------------------------------------------------

    #ifdef PLATFORM_LINUX
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

        // We have a connection, keep reading from the same connection unti it closes
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

            // Need a way to detect closed client socket
            bool disconnected = false;

            printf("Waiting to read from connected client\n");
            // Keep trying to read from connection until we get something
            while(1) {
                // lock mutex for should close
                close_mtx->lock();
                if (*should_close) {
                    // unlock mutex for should close
                    close_mtx->unlock();
                    close(socketfd);
                    break;
                }
                close_mtx->unlock();

                

                // Read from the socket
                int valread = recv(this->newsocket, this->json_data, BUFFER_SIZE,0);
                if (valread < 0) {
                    // nothing to read
                    continue;
                }
                else if (valread == 0) {
                    printf("Connection broken\n");
                    disconnected = true;
                    break;
                }
                else break; // data read, lets parse it
            }
            if (disconnected) break;
            printf("Data read from socket\n");

            // Parse json_data into shot_data
            std::string shot_string = this->json_data;
            t_shot_data shot_data;
            try {
                shot_data = parse_json_shot_string(shot_string);
                shot_data.ball_data.status = VALID;
            }
            catch (...) {
                printf("No valid json data to read\n");
                shot_data.ball_data.status = INVALID;
                // respond with failure
                int valsend = send(newsocket, resp_501, strlen(resp_501)+1, MSG_NOSIGNAL);
                if (valsend < 0) { // Likely the socket is closed
                    break; // break and start accepting new connections
                }
                continue;
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
                        int valsend = send(newsocket, resp_200, strlen(resp_200)+1, MSG_NOSIGNAL);
                        if (valsend < 0) { // Likely the socket is closed
                            break; // break and start accepting new connections
                        }
                    }
                }
                ball_mtx->unlock();
            } 
            else printf("Unable to get lock on ball data\n");// Otherwise, the main thread is still consuming ball data. Ignore shot
        }
    }

    
    close(this->socketfd);
    return;
    #endif
    // End Linux implementation

    // Windows Implementation
    // ----------------------------------------------------------------------
    #ifdef PLATFORM_WINDOWS
    printf("Waiting for incoming connections\n");

    // Get incoming connections
    while (1) {
        // lock mutex for should close
        close_mtx->lock();
        if (*should_close) {
            // unlock mutex for should close
            close_mtx->unlock();
            closesocket((SOCKET)socketfd);
            break;
        }
        close_mtx->unlock();

        // Accept incoming connections
        // Non-blocking accept call
        newsocket = -1;
        newsocket = (int)accept((SOCKET)socketfd, (struct sockaddr *)&client_addr, &client_addrlen);

        if (newsocket < 0) {
            continue; // no connection detected
        }
        else {
            printf("Connection accepted\n");
        }

        // We have a connection, keep reading from the same connection unti it closes
        while (1) {
            // lock mutex for should close
            close_mtx->lock();
            if (*should_close) {
                // unlock mutex for should close
                close_mtx->unlock();
                closesocket((SOCKET)socketfd);
                break;
            }
            close_mtx->unlock();

            // Need a way to detect closed client socket
            bool disconnected = false;

            printf("Waiting to read from connected client\n");
            // Keep trying to read from connection until we get something
            while(1) {
                // lock mutex for should close
                close_mtx->lock();
                if (*should_close) {
                    // unlock mutex for should close
                    close_mtx->unlock();
                    closesocket((SOCKET)socketfd);
                    break;
                }
                close_mtx->unlock();

                

                // Read from the socket
                int valread = recv((SOCKET)newsocket, this->json_data, BUFFER_SIZE,0);
                if (valread == SOCKET_ERROR) {
                    if(WSAGetLastError() == WSAEWOULDBLOCK) continue; // Nothing to read
                    else ; // Some other error happend. Maybe we should close?
                }
                else if (valread == 0) {
                    printf("Connection broken\n");
                    disconnected = true;
                    break;
                }
                else break; // data read, lets parse it
            }
            if (disconnected) break;
            printf("Data read from socket\n");

            // Parse json_data into shot_data
            std::string shot_string = this->json_data;
            t_shot_data shot_data;
            try {
                shot_data = parse_json_shot_string(shot_string);
                shot_data.ball_data.status = VALID;
            }
            catch (...) {
                printf("No valid json data to read\n");
                shot_data.ball_data.status = INVALID;
                // respond with failure
                int valsend = send((SOCKET)newsocket, resp_501, strlen(resp_501)+1, 0);
                if (valsend < 0) { // Likely the socket is closed
                    break; // break and start accepting new connections
                }
                continue;
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
                        int valsend = send((SOCKET)newsocket, resp_200, strlen(resp_200)+1, 0);
                        if (valsend < 0) { // Likely the socket is closed
                            break; // break and start accepting new connections
                        }
                    }
                }
                ball_mtx->unlock();
            } 
            else printf("Unable to get lock on ball data\n");// Otherwise, the main thread is still consuming ball data. Ignore shot
        }
    }

    
    closesocket((SOCKET)socketfd);
    return;

    #endif
    // End Windows Implementation
}