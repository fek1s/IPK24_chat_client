//
// Created by fuki on 22.3.24.
//


#include "ipkcpc_utils.h"

struct sockaddr_in createServerAddress(char* ip, int16_t port){
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    // Convert port number to network byte order (big endian)
    serverAddress.sin_port = htons(port);
    // Convert IP address to binary form
    if (strlen(ip) == 0){
        serverAddress.sin_addr.s_addr = INADDR_ANY;
    }
    else {
        inet_pton(AF_INET, ip, &serverAddress.sin_addr.s_addr);
    }
    return serverAddress;
}

int createTcpSocket(){
    return socket(AF_INET, SOCK_STREAM, 0);
}
