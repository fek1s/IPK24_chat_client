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
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>

typedef struct {
    char *transport_protocol;
    char *server_ip;
    int16_t port;
    uint16_t udp_timeout;
    u_int8_t  max_retransmissions;
} ProgramArguments;

/**
 * Parse program arguments
 * @param argc argument count
 * @param argv argument values
 * @return program arguments
 */
ProgramArguments parseArguments(int argc, char *argv[]);

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

/**
 * Create a UDP socket
 * @return socket file descriptor
 */
int createUdpSocket();

/**
 * Receive and print incoming data
 * @param socketFD socket file descriptor
 * @return NULL
 */
void *receiveAndPrintIncomingData(void *socketFD);

#endif //IPK_PROJ1_IPKCPC_UTILS_H
