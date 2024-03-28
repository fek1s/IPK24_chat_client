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

    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;

    getaddrinfo(args.server_ip, NULL, &hints, &res);

    struct sockaddr_in *addr = (struct sockaddr_in*)res->ai_addr;
    // @DEBUG
    printf("Server IP: %s\n", inet_ntoa(addr->sin_addr));
    addr->sin_port = htons(args.port);

    if (connect(socketFD, (struct sockaddr*)addr, sizeof(*addr)) == -1){
        printf("Failed to connect to server\n");
        return 1;
    }
    // @DEBUG
    printf("Connected to server\n");

    char *line = NULL;
    size_t lineSize = 0;
    // @DEBUG
    printf("Enter message:\n");

    //Create thread for receiving data
    pthread_t tid;
    pthread_create(&tid, NULL, receiveAndPrintIncomingData, (void*)&socketFD);


    while (1){
        ssize_t charCount = getline(&line, &lineSize, stdin);
        if (charCount > 0){
            if (strcmp(line, "exit\n") == 0){
                break;
            }
            ssize_t sendAmount = send(socketFD, line, charCount, 0);
            //printf("Sent %ld bytes\n", sendAmount);
            if (sendAmount == -1){
                printf("Failed to send data\n");
                break;
            }
        }
    }

    close(socketFD);

    return 0;
}