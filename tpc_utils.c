/**
 * @file ipkcpc_utils.c
 * @brief This file contains the implementation of the utility functions used in the IPK Client-Server communication.
 */

#include "ipkcpc_utils.h"

int createTcpSocket(){
    return socket(AF_INET, SOCK_STREAM, 0);
}

void *receiveAndPrintIncomingData(void *socketFD){
    int *socketFDPtr = (int*)socketFD;
    int socket = *socketFDPtr;
    while (1){
        char buffer[MAX_MESSAGE_SIZE];
        ssize_t recvAmount = recv(socket, buffer, sizeof(buffer), 0);
        if (recvAmount == 0){
            fprintf(stdout, "Server disconnected\n");
            break;
        }
        else if (recvAmount == -1){
            fprintf(stderr, "Failed to receive data\n");
            break;
        }
        else {
            char *message = parseReceivedMessage(buffer, &recvAmount);
            if (strcmp(message, "0") == 0){
                continue;
            }
            fprintf(stdout, "%s", message);
        }
    }

    return NULL;
}

int useTCP(ProgramArguments args) {
    int socketFD = createTcpSocket();
    if (socketFD == -1) {
        fprintf(stderr, "Failed to create socket\n");
        return -1;
    }

    // Resolve the server address
    struct sockaddr_in addr = resolve_host(args.server_ip, args.port);

    // Connect to the server
    if (connect(socketFD, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        fprintf(stderr, "Failed to connect to server\n");
        return -1;
    }
    printf("Connected to server\n");
    printf("Server address: %s\n", inet_ntoa(addr.sin_addr));


    // Create thread for receiving data
    pthread_t tid;
    pthread_create(&tid, NULL, receiveAndPrintIncomingData, (void*)&socketFD);

    char *line = NULL;
    size_t lineSize = 0;
    // Read input from the user
    while (1) {
        ssize_t charCount = getline(&line, &lineSize, stdin);
        char* message = parseInputMessage(line, &charCount);
        if (strcmp(message, "/CONTINUE") == 0) {
            continue;
        }
        if (strlen(message) > 0) {
            send(socketFD, message, strlen(message), 0);
            if (strcmp(message, "BYE\r\n") == 0) {
                pthread_cancel(tid);
                break;
            }
        }
    }

    free(line);
    close(socketFD);
    return 0;
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

















