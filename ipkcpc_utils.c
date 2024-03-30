/**
 * @file ipkcpc_utils.c
 * @brief This file contains the implementation of the utility functions used in the IPK Client-Server communication.
 */

#include "ipkcpc_utils.h"

int createTcpSocket(){
    return socket(AF_INET, SOCK_STREAM, 0);
}

int createUdpSocket(){
    return socket(AF_INET, SOCK_DGRAM, 0);
}

struct sockaddr_in resolve_host(char *ip,u_int16_t port){
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;

    // Resolve host
    getaddrinfo(ip, NULL, &hints, &res);
    struct sockaddr_in *addr = (struct sockaddr_in*)res->ai_addr;

    // Socket address structure
    struct sockaddr_in result;

    memcpy(&result, addr, sizeof(result));
    // Free the memory allocated for res
    freeaddrinfo(res);
    result.sin_port = htons(port);
    return result;
}

void *receiveAndPrintIncomingData(void *socketFD){
    int *socketFDPtr = (int*)socketFD;
    int socket = *socketFDPtr;
    while (1){
        char buffer[MAX_MESSAGE_SIZE];
        ssize_t recvAmount = recv(socket, buffer, sizeof(buffer), 0);
        if (recvAmount == 0){
            printf("Server disconnected\n");
            break;
        }
        else if (recvAmount == -1){
            printf("Failed to receive data\n");
            break;
        }
        else {
            // @DEBUG
            printf("BUFFER: %s", buffer);

            char *message = parseReceivedMessage(buffer, &recvAmount);
            if (strcmp(message, "0") == 0){
                continue;
            }
            fprintf(stdout, "%s", message);
        }
    }
    //free(socketFDPtr);

    return NULL;
}

void getCommand(char *message,char* command){
    int i = 0;
    // DEBUG
    //printf("Message: %s\n", message);
    while (message[i] != ' ' && message[i] != '\n' && message[i] != '\0'){
        command[i] = message[i];
        // Debug
        //printf("Command: %c\n", command[i]);
        i++;
    }
    command[i] = '\0';
}

char** split(const char *str, const char *delimiter, int *count) {
    char **result = NULL;
    char *token;
    int i = 0;

    // Copy the input string to avoid modifying the original string
    char *str_copy = strdup(str);
    if (str_copy == NULL) {
        fprintf(stderr, "Memory allocation error.\n");
        exit(EXIT_FAILURE);
    }

    // Allocate memory for the array of strings
    result = (char**)malloc(sizeof(char*));
    if (result == NULL) {
        fprintf(stderr, "Memory allocation error.\n");
        exit(EXIT_FAILURE);
    }

    // Tokenize the string using the delimiter
    token = strtok(str_copy, delimiter);
    while (token != NULL) {
        // Resize the array to accommodate the new token
        result = (char**)realloc(result, (i + 1) * sizeof(char*));
        if (result == NULL) {
            fprintf(stderr, "Memory allocation error.\n");
            exit(EXIT_FAILURE);
        }

        // Allocate memory for the token
        result[i] = strdup(token);
        if (result[i] == NULL) {
            fprintf(stderr, "Memory allocation error.\n");
            exit(EXIT_FAILURE);
        }

        i++;
        token = strtok(NULL, delimiter);
    }

    *count = i; // Set the count of tokens

    // Free the temporary copy of the string
    free(str_copy);

    return result;
}

int useTcp(ProgramArguments args) {
    int socketFD = createTcpSocket();
    if (socketFD == -1) {
        fprintf(stderr, "Failed to create socket\n");
        return -1;
    }

    struct sockaddr_in addr = resolve_host(args.server_ip, args.port);

    if (connect(socketFD, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        fprintf(stderr, "Failed to connect to server\n");
        return -1;
    }


    // Create thread for receiving data
    pthread_t tid;
    pthread_create(&tid, NULL, receiveAndPrintIncomingData, (void*)&socketFD);

    char *line = NULL;
    size_t lineSize = 0;
    while (1) {
        ssize_t charCount = getline(&line, &lineSize, stdin);
        char* message = parseInputMessage(line, &charCount);
        if (strcmp(message, "/CONTINUE") == 0) {
            continue;
        }
        if (strlen(message) > 0) {
            if (strcmp(message, "/exit\n") == 0) {
                break;
            }
            send(socketFD, message, strlen(message), 0);
        }
    }

    free(line);
    close(socketFD);
    return 0;
}

int useUDP(ProgramArguments args) {
    char buffer[MAX_MESSAGE_SIZE];
    int socketFD = createUdpSocket();
    struct SendDatagram sendDatagrams[MAX_DATAGRAMS];
    for (int i = 0; i < MAX_DATAGRAMS; i++) {
        sendDatagrams[i].confirmed = false;
        sendDatagrams[i].message = NULL;
    }

    if (socketFD == -1) {
        fprintf(stderr, "Failed to create socket\n");
        return -1;
    }

    struct sockaddr_in addr = resolve_host(args.server_ip, args.port);
    printf("Connected to server\n");
    printf("Server address: %s\n", inet_ntoa(addr.sin_addr));

    // Create thread for receiving data
    //TODO
    //pthread_t tid;
    //pthread_create(&tid, NULL, receiveAndPrintIncomingData, (void*)&socketFD);

    //char *line = NULL;
    //size_t lineSize = 0;
    while(1){
        //TODO
        //check_confirmations(socketFD, addr.sin_addr.s_addr, sendDatagrams);
        fgets(buffer, sizeof(buffer), stdin);
        ssize_t messageSize = strlen(buffer);
        // Parse the input message TODO

        struct SendDatagram newDatagram;
        newDatagram.message = strdup(buffer);
        newDatagram.messageSize = messageSize;
        newDatagram.confirmed = false;
        newDatagram.retransmissions = 0;

        // Find an empty slot in the array of datagrams
        int free_slot = -1;
        for (int i = 0; i < MAX_DATAGRAMS; i++) {
            if (sendDatagrams[i].message == NULL) {
                free_slot = i;
                break;
            }
        }
        if (free_slot != -1){
            sendDatagrams[free_slot] = newDatagram;
            //TODO
            //send_datagram(socketFD, addr.sin_addr.s_addr, &sendDatagrams[free_slot]);
            printf("Message sent: %s\n", sendDatagrams[free_slot].message);
        }
        else {
            fprintf(stderr, "No free slot for new datagram\n");
            return -1;
        }
        break;
    }
    //Release memory
    for (int i = 0; i < MAX_DATAGRAMS; i++) {
        if (sendDatagrams[i].message != NULL) {
            free(sendDatagrams[i].message);
        }
    }

    //free(buffer);
    close(socketFD);
    return 0;
}




