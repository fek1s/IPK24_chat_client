/**
 * @file ipkcpc_utils.h
 * @brief This is a header file containing function declarations and structures for IPK project 1
 * @author Jakub Fukala (xfukal01)
 */


#ifndef IPK_PROJ1_IPKCPC_UTILS_H
#define IPK_PROJ1_IPKCPC_UTILS_H

#include "stdint.h"
#include "parse.h"
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/time.h>


#define MAX_MESSAGE_SIZE 1500
#define MAX_COMMAND_SIZE 30
#define MESSAGE_CONTENT 1400
#define MAX_DATAGRAMS 100
#define MAX_DISPLAY_NAME 20

#define MAX_RETRIES 3
#define CONFIRM_TIMEOUT_MS 250

#define CONFIRMATION_MESSAGE 0x00
#define REPLY_MESSAGE 0x01
#define AUTH_MESSAGE 0x02
#define JOIN_MESSAGE 0x03
#define MSG_MESSAGE 0x04
#define ERROR_MESSAGE 0xFE


struct SendDatagram{
    bool confirmed;
    struct timeval sentTime;
    uint8_t*  message;
    ssize_t messageSize;
    int retransmissions;
    uint16_t sequenceNumber;
};

struct ReceivedDatagram{
    uint16_t sequenceNumber;
    ssize_t messageSize;
    bool processed;
};

struct ThreadArgs {
    int sockfd;
    struct sockaddr_in server_addr;
    struct SendDatagram *sent_datagrams;
    uint16_t *sequence_number;
};


/**
 * Resolve host
 * @brief Function resolves host by IP address and port number
 * @param ip IP address
 * @param port port number
 * @return resolved host
 */
struct sockaddr_in resolve_host(char *ip,u_int16_t port);

/**
 * Create a TCP socket
 * @return socket file descriptor
 */
int createTcpSocket();

/**
 * Create a UDP socket
 * @return socket file descriptor
 */
int createUdpSocket();


/**
 * Use TCP
 * @brief Used to communicate with server using TCP protocol and handle incoming data
 * @param args program arguments
 * @return 0 if successful, 1 otherwise
 */
int useTCP(ProgramArguments args);


/**
 * Use UDP
 * @brief Used to communicate with server using UDP protocol and handle incoming data
 * @param args program arguments
 * @return 0 if successful, 1 otherwise
 */
int useUDP(ProgramArguments args);


/**
 * Send message
 * @brief Takes received message, parses it and sends it to the server its made to be ran in a separate thread
 * @param socketFD socket file descriptor
 * @param message message
 * @param messageSize message size
 * @return 0 if successful, 1 otherwise
 */
void *receiveAndPrintIncomingData(void *socketFD);


/**
 * Make message
 * @brief Support function for UDP communication, makes message from user input
 * @param command command
 * @param message message
 * @param messageSize message size
 * @return message
 */
uint8_t *makeAuthMessage(char *username, char *password, char *displayName,uint16_t sequenceNumber,ssize_t *messageSize);

/**
 * Make message
 * @brief Support function for UDP communication, makes message from user input
 * @param sequenceNumber sequence number
 * @param displayName display name
 * @param message message
 * @param messageSize message size
 * @return message
 */
uint8_t *makeMsgMessage(uint16_t sequenceNumber,char *displayName ,char *message, ssize_t *messageSize);

/**
 * Make message
 * @brief Support function for UDP communication, makes message from user input
 * @param channel channel
 * @param sequenceNumber sequence number
 * @param messageSize message size
 * @param displayName display name
 * @return message
 */
uint8_t *makeJoinMessage(char *channel, uint16_t sequenceNumber, ssize_t *messageSize,char*displayName);

/**
 * Send Datagram
 * @brief Is used to send a datagram to the server and store the sendTime to the sructure
 * @param sequenceNumber sequence number
 * @param messageSize message size
 * @param SentDatagram is a structure that holds information about the sent datagram
 * @return message
 */
void sendDatagram(int socketFD,struct sockaddr_in *addr, struct SendDatagram *sentDatagram);

/**
 * Confirmation checker
 * @brief Function for checking if the message sent was confirmed by the server, if not it resends it
 * @param arg structure with arguments for the thread
 */
void *confirmation_checker(void *arg);

/**
 * Send confirmation
 * @brief Sends confirmation message to the server for specific sequence number
 * @param sockfd socket file descriptor
 * @param addr server address
 * @param sequenceNumber sequence number
 */
void sendConfirmation(int sockfd, struct sockaddr_in *addr, uint16_t sequenceNumber);

/**
 * Receives UDP messages from the server
 * @brief Listens for incoming UDP messages from the server and processes them
 * @param arg structure with arguments for the thread
 */
void *receiveAndPrintIncomingDataUDP(void *arg);

#endif //IPK_PROJ1_IPKCPC_UTILS_H
