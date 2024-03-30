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

#define MAX_MESSAGE_SIZE 1500
#define MAX_COMMAND_SIZE 30
#define MESSAGE_CONTENT 1400

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
 * Resolve host
 * @param ip IP address
 * @param port port number
 * @return resolved host
 */
struct sockaddr_in resolve_host(char *ip,u_int16_t port);
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

/**
 * Parse input message
 * @param message message
 * @param messageSize message size
 * @return parsed message
 */
char* parseInputMessage(char *message,ssize_t *messageSize);

/**
 * Parse received message
 * @param message message
 * @param messageSize message size
 * @return parsed message
 */
char *parseReceivedMessage(char *message, ssize_t *messageSize);

/**
 * Get command from message
 * @param message message
 * @param command command
 */
void getCommand(char *message,char* command);

/**
 * Split string by delimiter
 * @param str string
 * @param delimiter delimiter
 * @param count count
 * @return split string
 */
char** split(const char *str, const char *delimiter, int *count);

#endif //IPK_PROJ1_IPKCPC_UTILS_H
