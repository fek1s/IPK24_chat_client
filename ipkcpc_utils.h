//
// Created by fuki on 22.3.24.
//

#ifndef IPK_PROJ1_IPKCPC_UTILS_H
#define IPK_PROJ1_IPKCPC_UTILS_H

#include "stdint.h"
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

/**
 * Create server address
 * @param ip IP address
 * @param port port number
 * @return server address
 */
struct sockaddr_in createServerAddress(char* ip, int16_t port);

/**
 * Create a socket
 * @return socket file descriptor
 */
int createTcpSocket();

#endif //IPK_PROJ1_IPKCPC_UTILS_H
