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
            fprintf(stdout, "Server disconnected\n")
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
            if (strcmp(message, "/exit\n") == 0) {
                pthread_cancel(tid);
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
    int socketFD = createUdpSocket();
    struct SendDatagram sendDatagrams[MAX_DATAGRAMS];

    if (socketFD == -1) {
        fprintf(stderr, "Failed to create socket\n");
        return -1;
    }

    struct sockaddr_in addr = resolve_host(args.server_ip, args.port);
    printf("Connected to server\n");
    printf("Server address: %s\n", inet_ntoa(addr.sin_addr));

    // Create thread for receiving data
    uint16_t sequenceNumber = 0;
    uint16_t *seqNum = &sequenceNumber;
    pthread_t tid_receive;
    struct ThreadArgs thread_args_receive = {socketFD, addr, sendDatagrams, seqNum};
    pthread_create(&tid_receive, NULL, receiveAndPrintIncomingDataUDP, (void*)&thread_args_receive);

    // Create thread for checking confirmations
    pthread_t tid;
    struct ThreadArgs thread_args = {socketFD, addr, sendDatagrams,seqNum};
    pthread_create(&tid, NULL, confirmation_checker, (void*)&thread_args);

    while(1){
        char buffer[MAX_MESSAGE_SIZE];

        fgets(buffer, sizeof(buffer), stdin);
        ssize_t messageSize = strlen(buffer);

        // Parse the input message TODO REMOVE
        printf("Message: %s\n", buffer);
        printf("Size of buffer: %lo\n", strlen(buffer));
        if (strlen(buffer) == 1) {
            fprintf(stderr, "No message entered\n");
            continue;
        }

        if (strcmp(buffer, "/exit\n") == 0) {
            pthread_cancel(tid);
            pthread_cancel(tid_receive);
            break;

        }

        // Parse the input message
        uint8_t *byte = parseInputMessageUDP(buffer, &messageSize, sequenceNumber);

        // TODO REMOVE
        printf("Mesaage size: %lo\n", messageSize);
        if (byte == NULL) {
            fprintf(stderr, "Failed to parse input message\n");
            continue;
        }

        // Create a new datagram
        struct SendDatagram newDatagram;
        newDatagram.message = byte;
        newDatagram.messageSize = messageSize;
        newDatagram.confirmed = false;
        newDatagram.retransmissions = 0;
        newDatagram.sequenceNumber = sequenceNumber;

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
            sendDatagram(socketFD, &addr, &sendDatagrams[free_slot]);
        }
        else {
            fprintf(stderr, "No free slot for new datagram\n");
            return -1;
        }
        sequenceNumber++;
    }
    //Release memory
    for (uint16_t i = 0; i < sequenceNumber; i++) {
        if (sendDatagrams[i].message != NULL) {
            free(sendDatagrams[i].message);
        }
    }

    //free(buffer);
    close(socketFD);
    return 0;
}

uint8_t *makeMsgMessage(uint16_t sequenceNumber,char *displayName ,char *message, ssize_t *messageSize) {
    *messageSize = strlen(displayName) + strlen(message) + 5;
    uint8_t *msgMessage = (uint8_t*)malloc(*messageSize);
    if (msgMessage == NULL) {
        fprintf(stderr, "Memory allocation error.\n");
        exit(EXIT_FAILURE);
    }
    msgMessage[0] = MSG_MESSAGE;
    msgMessage[1] = (sequenceNumber >> 8) & 0xFF;
    msgMessage[2] = sequenceNumber & 0xFF;

    int N = strlen(displayName);
    for (int i = 0; i < N; i++){
        msgMessage[i + 3] = displayName[i];
    }
    msgMessage[strlen(displayName) + 3] = 0x00;

    N = strlen(message);
    for (int i = 0; i < N; i++){
        msgMessage[i + 4 + strlen(displayName)] = message[i];
    }
    msgMessage[strlen(message) + 4 + strlen(displayName)] = 0x00;
    return msgMessage;
}

uint8_t *makeAuthMessage(char *username, char *password, char *displayName,uint16_t sequenceNumber,ssize_t *messageSize) {
    *messageSize = strlen(username) + strlen(password) + strlen(displayName) + 6;
    uint8_t *message = (uint8_t*)malloc(*messageSize);
    if (message == NULL) {
        fprintf(stderr, "Memory allocation error.\n");
        exit(EXIT_FAILURE);
    }
    // Set the message type
    message[0] = AUTH_MESSAGE;
    message[1] = (sequenceNumber >> 8) & 0xFF;
    message[2] = sequenceNumber & 0xFF;

    // Set the result
    int N= strlen(username);
    for (int i = 0; i < N; i++){
        message[i + 3] = username[i];
    }
    // Set the null terminator
    message[strlen(username) + 3] = 0x00;

    N = strlen(displayName);
    for (int i = 0; i < N; i++){
        message[i + 4 + strlen(username)] = displayName[i];
    }
    message[strlen(displayName) + 4 + strlen(username)] = 0x00;

    N = strlen(password);
    for (int i = 0; i < N; i++){
        message[i + 5 + strlen(username) + strlen(displayName)] = password[i];
    }
    message[strlen(password) + 5 + strlen(username) + strlen(displayName)] = 0x00;

    return message;
}

uint8_t *makeJoinMessage(char *channel, uint16_t sequenceNumber, ssize_t *messageSize,char*displayName) {
    *messageSize = strlen(channel) + strlen(displayName) + 5;
    uint8_t *message = (uint8_t*)malloc(*messageSize);
    if (message == NULL) {
        fprintf(stderr, "Memory allocation error.\n");
        exit(EXIT_FAILURE);
    }
    // Set the message type
    message[0] = JOIN_MESSAGE;
    message[1] = (sequenceNumber >> 8) & 0xFF;
    message[2] = sequenceNumber & 0xFF;

    int N = strlen(channel);
    for (int i = 0; i < N; i++){
        message[i + 3] = channel[i];
    }
    // Set the null terminator
    message[strlen(channel) + 3] = 0x00;

    N = strlen(displayName);
    for (int i = 0; i < N; i++){
        message[i + 4 + strlen(channel)] = displayName[i];
    }
    message[strlen(displayName) + 4 + strlen(channel)] = 0x00;
    return message;
}

void sendDatagram(int socketFD,struct sockaddr_in *addr, struct SendDatagram *sentDatagram) {
    sendto(socketFD, sentDatagram->message, sentDatagram->messageSize, 0, (struct sockaddr*)addr, sizeof(*addr));
    gettimeofday(&sentDatagram->sentTime, NULL); // Record the time the datagram was sent
}

void *confirmation_checker(void *arg) {
    struct ThreadArgs *thread_args = (struct ThreadArgs *)arg;
    int sockfd = thread_args->sockfd;
    struct sockaddr_in server_addr = thread_args->server_addr;
    struct SendDatagram *sent_datagrams = thread_args->sent_datagrams;


    struct timeval current_time;

    while (1) {
        gettimeofday(&current_time, NULL);

        // TODO PASS TIMEOUT AS ARGUMENT TO THE FUNCTION && MAX_RETRIES

        // Check confirmations for each sent datagram
        for (uint16_t i = 0; i < *thread_args->sequence_number; ++i) {
            if (!sent_datagrams[i].confirmed && sent_datagrams[i].message != NULL) {
                long elapsed_time = (current_time.tv_sec - sent_datagrams[i].sentTime.tv_sec) * 1000 +
                                    (current_time.tv_usec - sent_datagrams[i].sentTime.tv_usec) / 1000;
                if (elapsed_time >= CONFIRM_TIMEOUT_MS) {
                    if (sent_datagrams[i].retransmissions < MAX_RETRIES) {
                        // Retransmit message
                        sendDatagram(sockfd, &server_addr, &sent_datagrams[i]);
                        printf("Resending message %d (retry %d)\n", i, sent_datagrams[i].retransmissions + 1);
                        sent_datagrams[i].retransmissions++;
                    }
                }
            }
        }

        usleep(100000); // Sleep to reduce CPU load
    }
}

void *receiveAndPrintIncomingDataUDP(void *arg){
   struct ThreadArgs *thread_args = (struct ThreadArgs *)arg;

    int sockfd = thread_args->sockfd;
    struct SendDatagram *sent_datagrams = thread_args->sent_datagrams;
    struct ReceivedDatagram received_datagrams[MAX_DATAGRAMS];
    uint16_t num_received_datagrams = 1;
    for (int i = 0; i < MAX_DATAGRAMS; i++) {
        received_datagrams[i].sequenceNumber = 0;
        received_datagrams[i].processed = false;
    }
    uint8_t buffer[MAX_MESSAGE_SIZE];

    while (1){
        struct sockaddr_in peer_addr;
        socklen_t peer_addr_len = sizeof(peer_addr);
        ssize_t recv_amount = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&peer_addr, &peer_addr_len);
        printf("Received %ld bytes\n", recv_amount);

        if (recv_amount > 0){
            uint8_t type = buffer[0];
            printf("Type: %d\n", type);
            uint16_t sequence_number = (buffer[1] << 8) | buffer[2];
            // TODO REMOVE
            //for (ssize_t i = 0; i < recv_amount; i++){
            //    printf("%c,[%lo],", buffer[i], i);
            //}
            if (type == CONFIRMATION_MESSAGE){
                // Confirmation message
                for (uint16_t i = 0; i < *thread_args->sequence_number; i++){
                    if (sent_datagrams[i].message != NULL && !sent_datagrams[i].confirmed){
                        if (sequence_number == sent_datagrams[i].sequenceNumber){
                            sent_datagrams[i].confirmed = true;
                            printf("Message %d confirmed\n", i);
                            free(sent_datagrams[i].message);
                            sent_datagrams[i].message = NULL;
                            break;
                        }
                    }
                }
            }
            else if (type == REPLY_MESSAGE){
                // Reply message
                printf("Reply message\n");
                for (uint16_t i = 0; i < num_received_datagrams; i++ ){
                    // Check if the message has already been processed
                    // If it has, send confirmation
                    if (received_datagrams[i].sequenceNumber == sequence_number && received_datagrams[i].processed){
                        sendConfirmation(sockfd, &peer_addr, sequence_number);
                        printf("Processed already\n");
                    }
                    // If it has not, process the message and send confirmation and add it to the list
                    else if (received_datagrams[i].sequenceNumber == 0 && !received_datagrams[i].processed){
                        received_datagrams[i].processed = true;
                        received_datagrams[i].sequenceNumber = sequence_number;

                        // Process the message
                        char mesageContent[recv_amount-6];
                        for (ssize_t i = 6; i < recv_amount; i++){
                            mesageContent[i-6] = buffer[i];
                        }
                        char message[recv_amount-6];
                        uint8_t result = buffer[3];
                        if (result == 1){
                            sprintf(message, "Success: %s", mesageContent);
                            printf("%s\n", message);

                        }
                        else { sprintf(message, "Failed: %s", mesageContent);
                            printf("%s\n", message);
                        }
                        sendConfirmation(sockfd, &peer_addr, sequence_number);
                        break;
                    }

                }
            }
            else if (type == MSG_MESSAGE){
                // MSG message
                for (uint16_t i = 0; i < num_received_datagrams; i++ ){
                    // Check if the message has already been processed
                    // If it has, send confirmation
                    if (received_datagrams[i].sequenceNumber == sequence_number && received_datagrams[i].processed){
                        sendConfirmation(sockfd, &peer_addr, sequence_number);
                    }
                    // If it has not, process the message and send confirmation and add it to the list
                    else if (received_datagrams[i].sequenceNumber == 0 && !received_datagrams[i].processed){
                        received_datagrams[i].processed = true;
                        received_datagrams[i].sequenceNumber = sequence_number;
                        // Process the Display name
                        int i = 3;
                        while (buffer[i]!= 0x00 && i < MAX_DISPLAY_NAME){
                            printf("%c", buffer[i]);
                            i++;
                        }
                        printf(": ");

                        // Process the message content
                        ssize_t j = i;
                        while (j < recv_amount-1){
                            printf("%c", buffer[j]);
                            j++;

                        }
                        printf("\n");

                        // Send confirmation
                        sendConfirmation(sockfd, &peer_addr, sequence_number);
                        break;
                    }

                }
            }
            else if (type == ERROR_MESSAGE){
                // Error message
                for (uint16_t i = 0; i < num_received_datagrams; i++ ){
                    // Check if the message has already been processed
                    // If it has, send confirmation
                    if (received_datagrams[i].sequenceNumber == sequence_number && received_datagrams[i].processed){
                        sendConfirmation(sockfd, &peer_addr, sequence_number);
                    }
                        // If it has not, process the message and send confirmation and add it to the list
                    else if (received_datagrams[i].sequenceNumber == 0 && !received_datagrams[i].processed){
                        received_datagrams[i].processed = true;
                        received_datagrams[i].sequenceNumber = sequence_number;
                        // Process the Display name
                        int i = 3;
                        while (buffer[i]!= 0x00 && i < MAX_DISPLAY_NAME){
                            printf("%c", buffer[i]);
                            i++;
                        }
                        printf(": ");

                        // Process the message content
                        ssize_t j = i;
                        while (j < recv_amount-1){
                            printf("%c", buffer[j]);
                            j++;

                        }
                        printf("\n");

                        // Send confirmation
                        sendConfirmation(sockfd, &peer_addr, sequence_number);
                        break;
                    }
                }
            }
        }
        num_received_datagrams++;
    }
}

void sendConfirmation(int sockfd, struct sockaddr_in *addr, uint16_t sequenceNumber) {
    uint8_t confirmationMessage[3];
    confirmationMessage[0] = CONFIRMATION_MESSAGE;
    // Split the sequence number into two bytes
    confirmationMessage[1] = (sequenceNumber >> 8) & 0xFF;
    confirmationMessage[2] = sequenceNumber & 0xFF;
    sendto(sockfd, confirmationMessage, sizeof(confirmationMessage), 0, (struct sockaddr*)addr, sizeof(*addr));
}





