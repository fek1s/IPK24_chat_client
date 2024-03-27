/**
 * @file ipkcpc.c
 *
 * @brief IPK projekt 1
 *
 * @author Jakub Fukala (xfukal01)
 */

#include "ipkcpc_utils.h"
#include "ipkcpc.h"


int main(){
    // Define server address
    //char* ip = "142.251.36.142";
    //int16_t port = 80;

    // Loopback address for testing
    char* ip = "127.0.0.1";
    int16_t port = 6969;

    int socketFD = createSocket();
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

    char* mesage;
    mesage = "This is a test massage for netcat\n";
    send(socketFD,mesage, strlen(mesage), 0);
    char buffer[1024];
    recv(socketFD,buffer,1024,0 );
    printf("Received: %s\n", buffer);

    close(socketFD);

    return 0;
}