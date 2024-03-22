//
// Created by fuki on 22.3.24.
//

#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "ipkcpc_utils.h"

struct sockaddr_in createServerAddress(char* ip, int16_t port){
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    // Convert port number to network byte order (big endian)
    serverAddress.sin_port = htons(port);
    // Convert IP address to binary form
    inet_pton(AF_INET, ip, &serverAddress.sin_addr.s_addr);
    return serverAddress;
}

int createSocket(){
    return socket(AF_INET, SOCK_STREAM, 0);
}
