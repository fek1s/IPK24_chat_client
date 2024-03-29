//
// Created by fuki on 22.3.24.
//


#include "ipkcpc_utils.h"

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
    char buffer[MAX_MESSAGE_SIZE];
    while (1){
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
            printf("Received %ld bytes: %s", recvAmount, buffer);
        }
    }
    //free(socketFDPtr);

    return NULL;
}

int createTcpSocket(){
    return socket(AF_INET, SOCK_STREAM, 0);
}

int createUdpSocket(){
    return socket(AF_INET, SOCK_DGRAM, 0);
}

ProgramArguments parseArguments(int argc, char *argv[]){
    ProgramArguments args;
    int opt;
    args.transport_protocol = NULL;
    args.server_ip = NULL;
    args.port = 4567;
    args.udp_timeout = 250;
    args.max_retransmissions = 3;

    while ((opt = getopt(argc, argv, "t:s:p:d:r:h")) != -1){
        switch (opt){
            case 't':
                args.transport_protocol = optarg;
                break;
            case 's':
                args.server_ip = optarg;
                break;
            case 'p':
                args.port = atoi(optarg);
                break;
            case 'd':
                args.udp_timeout = atoi(optarg);
                break;
            case 'r':
                args.max_retransmissions = atoi(optarg);
                break;
            case 'h':
                printf("Usage: %s -t <transport_protocol> -s <server_ip> -p <port> -d <udp_timeout> -r <max_retransmissions>\n", argv[0]);
                exit(0);
            default:
                printf("Invalid argument\n");
                exit(1);
        }
    }

    if (args.transport_protocol == NULL || args.server_ip == NULL){
        printf("Missing mandatory arguments\n");
        exit(1);
    }

    return args;
}

char* parseMessage(char *message,ssize_t *messageSize){
    if (message[*messageSize-1] == '\n'){
        message[*messageSize-1] = '\0';
        *messageSize -= 1;
    }
    if (message[0] == '/'){

        char command[MAX_COMMAND_SIZE];
        getCommand(message,command);

        if (strcmp(command, "/auth") == 0){
            // @DEBUG
            printf("AUTH command\n");
            int count;
            char **tokens = split(message, " ", &count);
            for (int i = 0; i < count; i++){
                printf("Token %d: %s\n", i, tokens[i]);
            }
            char formattedMessage[MAX_MESSAGE_SIZE];
            sprintf(formattedMessage, "AUTH %s AS %s USING %s\r\n", tokens[1], tokens[2], tokens[3]);
            printf("Formatted message: %s", formattedMessage);
            strcpy(message, formattedMessage);

            // Free the memory allocated for tokens
            for (int i = 0; i < count; i++) {
                free(tokens[i]);
            }
            free(tokens);
        }
        else if (strcmp(command, "/join") == 0){
            // @DEBUG
            printf("JOIN command\n");
        }
        else if (strcmp(command, "/rename") == 0){
            // @DEBUG
            printf("RENAME command\n");
        }
        else if (strcmp(command, "/help") == 0){
            // @DEBUG
            printf("HELP command\n");
        }
        else if (strcmp(command, "/exit") == 0){
            // @DEBUG
            printf("EXIT command\n");
            strcpy(message, "exit\n");
        }
        else {
            printf("Invalid command\n");
        }
    }

    return message;
}

void getCommand(char *message,char* command){
    int i = 0;
    printf("Message: %s\n", message);
    while (message[i] != ' ' && message[i] != '\n' && message[i] != '\0'){
        command[i] = message[i];
        printf("Command: %c\n", command[i]);
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




