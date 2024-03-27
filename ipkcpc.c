/**
 * @file ipkcpc.c
 *
 * @brief IPK projekt 1
 *
 * @author Jakub Fukala (xfukal01)
 */

#include "ipkcpc_utils.h"
#include "ipkcpc.h"

void *receiveAndPrintIncomingData(void *socketFD){
    int *socketFDPtr = (int*)socketFD;
    int socket = *socketFDPtr;
    char buffer[1024];
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
            printf("Received %ld bytes: %s\n", recvAmount, buffer);
        }
    }
    return NULL;
}

int main(){
    // Define server address
    //char* ip = "142.251.36.142";
    //int16_t port = 80;

    // Loopback address for testing
    char* ip = "127.0.0.1";
    int16_t port = 1999;

    int socketFD = createTcpSocket();
    if (socketFD == -1){
        printf("Failed to create socket\n");
        return 1;
    }
    printf("Socket binded successfully\n");




    struct sockaddr_in serverAddress = createServerAddress(ip, port);


    if (connect(socketFD,(struct sockaddr*)&serverAddress,sizeof(serverAddress)) == -1){
        printf("Failed to connect to server\n");
        return 1;
    }
    printf("Connected to server\n");

    char *line = NULL;
    size_t lineSize = 0;
    printf("Enter message:\n");

    //Vytvoření vlákna
    pthread_t tid;
    pthread_create(&tid, NULL, receiveAndPrintIncomingData, (void*)&socketFD);


    while (1){
        ssize_t charCount = getline(&line, &lineSize, stdin);
        if (charCount > 0){
            if (strcmp(line, "exit\n") == 0){
                break;
            }
            ssize_t sendAmount = send(socketFD, line, charCount, 0);
            printf("Sent %ld bytes\n", sendAmount);
        }
    }

    close(socketFD);

    return 0;
}