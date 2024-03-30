/**
 * @file parse.c
 * @brief This file contains the implementation of the functions used to parse the input and received messages
 * @details The functions parse the input message and format it according to the protocol, and also parse the received
 * message and format it to be displayed to the user
 */


#include "ipkcpc_utils.h"
char DisplayName[20];

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

            // @DEBUG
            printf("AUTH command\n");

            int count;
            char **tokens = split(message, " ", &count);
//            for (int i = 0; i < count; i++){
//                printf("Token %d: %s\n", i, tokens[i]);
//            }
            sprintf(formattedMessage, "AUTH %s AS %s USING %s\r\n", tokens[1], tokens[2], tokens[3]);
            memcpy(DisplayName,tokens[2],strlen(tokens[2])+1);

            // @DEBUG
            printf("Formatted message: %s", formattedMessage);
            printf("Display name: %s\n", DisplayName);

            strcpy(message, formattedMessage);

            // Free the memory allocated for tokens
            for (int i = 0; i < count; i++) {
                free(tokens[i]);
            }
            free(tokens);
            return message;
        }
        else if (strcmp(command, "/join") == 0){
            // @DEBUG
            printf("JOIN command\n");
            int count;
            char **tokens = split(message, " ", &count);
//            for (int i = 0; i < count; i++){
//                printf("Token %d: %s\n", i, tokens[i]);
//            }
            char formattedMessage[MAX_MESSAGE_SIZE];
            sprintf(formattedMessage, "JOIN %s AS %s\r\n", tokens[1],DisplayName);
            printf("Formatted message: %s", formattedMessage);
            strcpy(message, formattedMessage);

            // Free the memory allocated for tokens
            for (int i = 0; i < count; i++) {
                free(tokens[i]);
            }
            free(tokens);
            return message;
        }
        else if (strcmp(command, "/rename") == 0){
            // @DEBUG
            printf("RENAME command\n");

            int count;
            char **tokens = split(message, " ", &count);

            // @DEBUG
//            for (int i = 0; i < count; i++){
//                printf("Token %d: %s\n", i, tokens[i]);
//            }
            memcpy(DisplayName,tokens[1],strlen(tokens[1])+1);
            printf("NEW Display name: %s\n", DisplayName);
            for (int i = 0; i < count; i++) {
                free(tokens[i]);
            }
            free(tokens);
            strcpy(message, "/CONTINUE");
            return message;
        }
        else if (strcmp(command, "/help") == 0){
            // @DEBUG
            printf("HELP command\n");
            printf("Available commands:\n");
            printf("/help - Display help\n");
            printf("/auth <username> <password> <secret> - To be able to chat\n");
            printf("/join <channel> - Join a channel\n");
            printf("/rename <new_name> - Rename yourself\n");
            printf("/exit - Exit the chat\n");
            strcpy(message, "/CONTINUE");
            return message;
        }
        else if (strcmp(command, "/exit") == 0){
            // @DEBUG
            printf("EXIT command\n");
            strcpy(message, "/exit\n");
            return message;
        }
        else {
            printf("Invalid command\n");
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
    // @DEBUG
//    for (int i = 0; i < count; i++){
//        printf("Token %d: %s\n", i, tokens[i]);
//    }

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
        sprintf(formattedMessage, "ERROR: %s\n", tokens[1]);
        strcpy(message, formattedMessage);

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



