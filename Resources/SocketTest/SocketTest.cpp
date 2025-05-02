#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fstream>
#include <chrono>
#include <thread>

#define PORT 49152
#define SERVER_ADDRESS "127.0.0.1"
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    const char *message = "{}";

    std::ifstream t;
    int length;
    t.open("test_shot_2.json");
    t.seekg(0, std::ios::end);
    length = t.tellg();
    t.seekg(0, std::ios::beg);
    char* json_str = new char[length+1];
    t.read(json_str, length);
    t.close();

    json_str[length] = '\0';
    
    // Create socket file descriptor
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set server address parameters
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, SERVER_ADDRESS, &serv_addr.sin_addr) <= 0) {
        perror("invalid address / address not supported");
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connection failed");
        exit(EXIT_FAILURE);
    }
    printf("Connected to server\n");
    std::this_thread::sleep_for(std::chrono::seconds(3));

    for (int i=0; i<3; i++) {
        // Send message to server
        send(sock, json_str, length+1, 0);
        printf("Message sent to server.\n");

        // Read response from server
        read(sock, buffer, BUFFER_SIZE);
        printf("Server response: %s\n", buffer);

        std::this_thread::sleep_for(std::chrono::seconds(12));
    }

    // Close socket
    close(sock);

    return 0;
}
