#ifndef _TCPSOCKET_H_
#define _TCPSOCKET_H_
#include <mutex>

#ifdef PLATFORM_LINUX
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#ifdef PLATFORM_WINDOWS
#define NODRAWTEXT
#define NOGDI 
#define NOUSER
// Type required before windows.h inclusion
typedef struct tagMSG *LPMSG;
// Windows version that allows inet_ntop function to be used
#ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0600
#elif _WIN32_WINNT < 0x0600
    #undef _WIN32_WINNT
    #define _WIN32_WINNT 0x0600
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#endif

#include "shotData.hpp"

#define PORT 49152
#define BUFFER_SIZE 1024

class TCPSocket {
    char json_data[BUFFER_SIZE];
    struct sockaddr_in address;
    struct sockaddr_in client_addr;

    #ifdef PLATFORM_LINUX
    int socketfd, newsocket;
    #endif
    #ifdef PLATFORM_WINDOWS
    SOCKET socketfd, newsocket;
    #endif

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