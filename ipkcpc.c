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
    // Define server address
    //char* ip = "142.251.36.142";
    //int16_t port = 80;

    ProgramArguments args = parseArguments(argc, argv);

    int socketFD = createTcpSocket();
    if (socketFD == -1){
        printf("Failed to create socket\n");
        return 1;
    }
    printf("Socket binded successfully\n");

    struct sockaddr_in serverAddress = createServerAddress(args.server_ip, args.port);

    if (connect(socketFD,(struct sockaddr*)&serverAddress,sizeof(serverAddress)) == -1){
        printf("Failed to connect to server\n");
        return 1;
    }
    printf("Connected to server\n");

    char *line = NULL;
    size_t lineSize = 0;
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
            printf("Sent %ld bytes\n", sendAmount);
        }
    }

    close(socketFD);

    return 0;
}