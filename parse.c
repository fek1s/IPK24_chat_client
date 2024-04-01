/**
 * @file parse.c
 * @brief This file contains the implementation of the functions used to parse the input and received messages
 * @details The functions parse the input message and format it according to the protocol, and also parse the received
 * message and format it to be displayed to the user
 */

#include "ipkcpc_utils.h"

char DisplayName[MAX_DISPLAY_NAME];

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

char* parseInputMessage(char *message,ssize_t *messageSize){
    if (message[*messageSize-1] == '\n'){
        message[*messageSize-1] = '\0';
        *messageSize -= 1;
    }
    char formattedMessage[MAX_MESSAGE_SIZE];
    if (message[0] == '/'){

        char command[MAX_COMMAND_SIZE];
        getCommand(message,command);

        if (strcmp(command, "/auth") == 0){
            int count;
            char **tokens = split(message, " ", &count);

            if (count != 4){
                fprintf(stderr, "ERR: Invalid number of arguments\n");
                strcpy(message, "/CONTINUE");

                // Free the memory allocated for tokens
                for (int i = 0; i < count; i++) {
                    free(tokens[i]);
                }
                free(tokens);
                return message;
            }
            sprintf(formattedMessage, "AUTH %s AS %s USING %s\r\n", tokens[1], tokens[2], tokens[3]);
            memcpy(DisplayName,tokens[2],strlen(tokens[2])+1);


            strcpy(message, formattedMessage);

            // Free the memory allocated for tokens
            for (int i = 0; i < count; i++) {
                free(tokens[i]);
            }
            free(tokens);
            return message;
        }
        else if (strcmp(command, "/join") == 0){

            int count;
            char **tokens = split(message, " ", &count);

            if (count != 2){
                fprintf(stderr, "ERR: Invalid number of arguments\n");
                strcpy(message, "/CONTINUE");
                // Free the memory allocated for tokens
                for (int i = 0; i < count; i++) {
                    free(tokens[i]);
                }
                free(tokens);
                return message;
            }

            char formattedMessage[MAX_MESSAGE_SIZE];
            sprintf(formattedMessage, "JOIN %s AS %s\r\n", tokens[1],DisplayName);
            strcpy(message, formattedMessage);

            // Free the memory allocated for tokens
            for (int i = 0; i < count; i++) {
                free(tokens[i]);
            }
            free(tokens);
            return message;
        }
        else if (strcmp(command, "/rename") == 0){

            int count;
            char **tokens = split(message, " ", &count);

            // Invalid number of arguments
            if (count != 2){
                fprintf(stderr, "ERR: Invalid number of arguments\n");
                strcpy(message, "/CONTINUE");
                return message;
            }

            memcpy(DisplayName,tokens[1],strlen(tokens[1])+1);

            // Free the memory allocated for tokens
            for (int i = 0; i < count; i++) {
                free(tokens[i]);
            }
            free(tokens);
            strcpy(message, "/CONTINUE");
            return message;
        }
        else if (strcmp(command, "/help") == 0){
            printf("Available commands:\n");
            printf("/help - Display help\n");
            printf("/auth <username> <password> <secret> - To be able to chat\n");
            printf("/join <channel> - Join a channel\n");
            printf("/rename <new_name> - Rename yourself\n");
            printf("/exit - Exit the chat\n");
            strcpy(message, "/CONTINUE");
            return message;
        }
        else if (strcmp(command, "/bye") == 0){
            sprintf(formattedMessage, "BYE\r\n");
            strcpy(message, formattedMessage);
            return message;
        }
        else {
            fprintf(stderr, "ERR: Invalid command\n");
            strcpy(message, "/CONTINUE");
            return message;
        }
    }

    sprintf(formattedMessage, "MSG FROM %s IS %s\r\n", DisplayName, message);
    strcpy(message, formattedMessage);
    return message;
}

char *parseReceivedMessage(char *message, ssize_t *messageSize){
    if (message[*messageSize-1] == '\n'){
        message[*messageSize-1] = '\0';
        *messageSize -= 1;
    }
    int count;
    char **tokens = split(message, " ", &count);


    if (strcmp(tokens[0], "MSG") == 0){
        char formattedMessage[MAX_MESSAGE_SIZE];
        char messageCopy[MESSAGE_CONTENT];
        strcpy(messageCopy, tokens[4]);

        for (int i = 5; i < count; i++) {
            char* temp = tokens[i];
            strcat(messageCopy, " ");
            strcat(messageCopy, temp);
        }
        sprintf(formattedMessage, "%s: %s\n", tokens[2], messageCopy);
        strcpy(message, formattedMessage);
    }
    else if (strcmp(tokens[0], "ERR") == 0){
        char formattedMessage[MAX_MESSAGE_SIZE];
        sprintf(formattedMessage, "ERR FROM %s: %s\n", tokens[2],tokens[4]);
        strcpy(message, "0");
        fprintf(stderr, "%s",formattedMessage);
        return message;
    }
    else if (strcmp(tokens[0], "REPLY") == 0){
        char messageCopy[MESSAGE_CONTENT];
        strcpy(messageCopy, tokens[3]);
        for (int i = 4; i < count; ++i) {
            char* temp = tokens[i];
            strcat(messageCopy, " ");
            strcat(messageCopy, temp);
        }

        if (strcmp(tokens[1], "OK") == 0){
            printf("Success: %s\n", messageCopy);
            strcpy(message, "0");
            return message;
        }
        else {
            // Failure
            printf("Failure: %s\n", messageCopy);
            strcpy(message, "0");
            return message;
        }
    }
    else {
        char formattedMessage[MAX_MESSAGE_SIZE];
        sprintf(formattedMessage, "Unknown message: %s\n", message);
        strcpy(message, formattedMessage);
    }
    // Free the memory allocated for tokens
    for (int i = 0; i < count; i++) {
        free(tokens[i]);
    }
    free(tokens);

    return message;
}

uint8_t *parseInputMessageUDP(char *message, ssize_t *messageSize, uint16_t sequenceNumber) {
    if (message[*messageSize - 1] == '\n') {
        message[*messageSize - 1] = '\0';
        *messageSize -= 1;
    }
    if (message[0] == '/')
    {
        //char formattedMessage[MAX_MESSAGE_SIZE];
        char command[MAX_COMMAND_SIZE];
        getCommand(message, command);

        if (strcmp(command, "/auth") == 0) {
            int count;
            char **tokens = split(message, " ", &count);
            if (count != 4) {
                fprintf(stderr, "ERR: Invalid number of arguments\n");
                return NULL;
            }

            uint8_t *authMessage = makeAuthMessage(tokens[1], tokens[3], tokens[2], sequenceNumber, messageSize);
            memcpy(DisplayName, tokens[2], strlen(tokens[2]) + 1);
            for (int i = 0; i < count; i++) {
                free(tokens[i]);
            }
            free(tokens);
            return authMessage;

        } else if (strcmp(command, "/join") == 0) {
            int count;
            char **tokens = split(message, " ", &count);
            if (count != 2) {
                fprintf(stderr, "ERR: Invalid number of arguments\n");
            }
                uint8_t *joinMessage = makeJoinMessage(tokens[1], sequenceNumber, messageSize, DisplayName);
                for (int i = 0; i < count; i++) {
                    free(tokens[i]);
                }
                free(tokens);
                return joinMessage;



        } else if (strcmp(command, "/rename") == 0) {
            int count;
            char **tokens = split(message, " ", &count);
            if (count != 2) {
                fprintf(stderr, "ERR: Invalid number of arguments\n");
                return NULL;
            }
            memcpy(DisplayName, tokens[1], strlen(tokens[1]) + 1);

            for (int i = 0; i < count; i++) {
                free(tokens[i]);
            }
            free(tokens);
            return NULL;

        } else if (strcmp(command, "/help") == 0) {
            printf("Available commands:\n");
            printf("/help - Display help\n");
            printf("/auth <username> <password> <secret> - To be able to chat\n");
            printf("/join <channel> - Join a channel\n");
            printf("/rename <new_name> - Rename yourself\n");
            printf("/exit - Exit the chat\n");
            return NULL;
        } else if (strcmp(command, "/bye") == 0) {
            printf("BYE command\n");
            strcpy(message, "/bye\n");
        } else {
            fprintf(stderr, "ERR: Invalid command\n");
            return NULL;
        }
    }
    printf("MSG FROM %s IS %s \r\n", DisplayName, message);
    return makeMsgMessage(sequenceNumber, DisplayName, message, messageSize);
}

void getCommand(char *message,char* command){
    int i = 0;
    while (message[i] != ' ' && message[i] != '\n' && message[i] != '\0'){
        command[i] = message[i];
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
        fprintf(stderr, "ERR: Memory allocation error.\n");
        exit(EXIT_FAILURE);
    }

    // Allocate memory for the array of strings
    result = (char**)malloc(sizeof(char*));
    if (result == NULL) {
        fprintf(stderr, "ERR: Memory allocation error.\n");
        exit(EXIT_FAILURE);
    }

    // Tokenize the string using the delimiter
    token = strtok(str_copy, delimiter);
    while (token != NULL) {
        // Resize the array to accommodate the new token
        result = (char**)realloc(result, (i + 1) * sizeof(char*));
        if (result == NULL) {
            fprintf(stderr, "ERR: Memory allocation error.\n");
            exit(EXIT_FAILURE);
        }

        // Allocate memory for the token
        result[i] = strdup(token);
        if (result[i] == NULL) {
            fprintf(stderr, "ERR: Memory allocation error.\n");
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

