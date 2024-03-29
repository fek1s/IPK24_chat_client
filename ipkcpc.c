/**
 * @file ipkcpc.c
 *
 * @brief IPK projekt 1
 *
 * @author Jakub Fukala (xfukal01)
 */

#include "ipkcpc_utils.h"
#include "ipkcpc.h"

int main(int argc, char* argv[]){

    ProgramArguments args = parseArguments(argc, argv);

    int socketFD = createTcpSocket();
    if (socketFD == -1){
        // @DEBUG
        printf("Failed to create socket\n");
        return 1;
    }
    // @DEBUG
    printf("Socket binded successfully\n");
    printf("Server IP: %s\n", args.server_ip);


    struct sockaddr_in addr = resolve_host(args.server_ip, args.port);

    if (connect(socketFD, (struct sockaddr*)&addr, sizeof(addr)) == -1){
        printf("Failed to connect to server\n");
        return 1;
    }
    // @DEBUG
    printf("Server address: %s\n", inet_ntoa(addr.sin_addr));
    printf("Connected to server\n");


    char *line = NULL;
    size_t lineSize = 0;
    //Create thread for receiving data
    pthread_t tid;
    pthread_create(&tid, NULL, receiveAndPrintIncomingData, (void*)&socketFD);

    while (1){
        ssize_t charCount = getline(&line, &lineSize, stdin);
        // print line
        printf("Line: %s\n", line);
        char* message = parseMessage(line, &charCount);
        printf("Message: %s\n", message);
        if (strlen(message) > 0){
            if (strcmp(message, "/exit\n") == 0){
                break;
            }
            ssize_t sendAmount = send(socketFD, message, strlen(message), 0);
            //printf("Sent %ld bytes\n", sendAmount);
            if (sendAmount == -1){
                printf("Failed to send data\n");
                break;
            }
        }
    }
    close(socketFD);
    //pthread_join(tid, NULL);

    return 0;
}