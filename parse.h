/**
 * @file parse.h
 * @brief This file contains the declarations of the functions used for parsing messages
 * for both UPD and TCP communication in the IPK project 1
 * @author Jakub Fukala (xfukal01)
 */

#ifndef IPK_PROJEKT_1_PARSE_H
#define IPK_PROJEKT_1_PARSE_H

#include "ipkcpc_utils.h"

typedef struct {
    char *transport_protocol;
    char *server_ip;
    int16_t port;
    uint16_t udp_timeout;
    u_int8_t  max_retransmissions;
} ProgramArguments;

/**
 * Parse arguments
 * @brief Function parses arguments from command line, validates them and returns them in a structure
 * @param argc argument count
 * @param argv argument values
 * @return parsed arguments
 */
ProgramArguments parseArguments(int argc, char *argv[]);

/**
 * Parse input message
 * @brief Support function for TCP communication, parses user input message
 * @param message message
 * @param messageSize message size
 * @return parsed message
 */
char* parseInputMessage(char *message,ssize_t *messageSize);

/**
 * Send message
 * @brief Takes a user input message, parses it and sends it to the server its made to be ran in a separate thread
 * @param socketFD socket file descriptor
 * @param message message
 * @param messageSize message size
 * @return 0 if successful, 1 otherwise
 */
uint8_t *parseInputMessageUDP(char *message, ssize_t *messageSize, uint16_t sequenceNumber);

/**
 * Parse received message
 * @brief Support function for TCP communication, parses received message
 * @param message message
 * @param messageSize message size
 * @return parsed message
 */
char *parseReceivedMessage(char *message, ssize_t *messageSize);

/**
 * Get command
 * @brief Support function for TCP communication, extracts command from user input message
 * @param message string with user input message
 * @param command pointer to command
 */
void getCommand(char *message,char* command);

/**
 * Split string by delimiter
 * @brief Support function for parsing user input message, splits string by delimiter
 * @param str string
 * @param delimiter delimiter
 * @param count count
 * @return split string
 */
char** split(const char *str, const char *delimiter, int *count);


#endif //IPK_PROJEKT_1_PARSE_H
