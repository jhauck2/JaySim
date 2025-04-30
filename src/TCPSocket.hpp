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
    static constexpr char resp_501[] = "{\n    \"Code\": 501,\n    \"Message\": \"Malformed json data\"\n}";
    static constexpr char resp_200[] = "{\n    \"Code\": 200,\n    \"Message\": \"Shot recieved successfully\"\n}";
    static constexpr char resp_201[] = "{\n    \"Code\": 201,\n"
                                        "    \"Message\": \"Player Information\",\n"
                                        "    \"Player\": {\n"
                                        "        \"Handed\": \"RH\",\n"
                                        "        \"Club\": \"DR\"\n"
                                        "    }\n}";

public:
    int init_socket();
    void run_socket(t_ball_data *ball_data, bool *should_close, std::mutex *ball_mtx, std::mutex *close_mtx);
    
};



#endif