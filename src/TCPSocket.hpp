#ifndef _TCPSOCKET_H_
#define _TCPSOCKET_H_

#include <mutex>
#include <netinet/in.h>
#include "shotData.hpp"
#include <sys/socket.h>
#include <unistd.h>

#define PORT 49152
#define BUFFER_SIZE 1024

class TCPSocket {
    char json_data[BUFFER_SIZE];
    int socketfd, newsocket;
    struct sockaddr_in address, client_addr;
    int addrlen, client_addrlen;

public:
    int init_socket();
    void run_socket(t_ball_data *ball_data, bool *should_close, std::mutex *ball_mtx, std::mutex *close_mtx);
    
};



#endif