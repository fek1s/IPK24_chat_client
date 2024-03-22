//
// Created by fuki on 22.3.24.
//

#ifndef IPK_PROJ1_IPKCPC_UTILS_H
#define IPK_PROJ1_IPKCPC_UTILS_H

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
int createSocket();

#endif //IPK_PROJ1_IPKCPC_UTILS_H
