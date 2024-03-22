/**
 * @file ipkcpc.c
 *
 * @brief IPK projekt 1
 *
 * @author Jakub Fukala (xfukal01)
 */

#include <sys/socket.h>
#include <netinet/in.h>
//#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include "ipkcpc_utils.h"
#include "ipkcpc.h"

void hello(){
    printf("Hello");
}


int main(){
    // Define server address
    char* ip = "142.251.36.142";
    int16_t port = 80;

    //char* ip = "127.0.0.1";
    //int16_t port = 6969;


    int socketFD = createSocket();

    struct sockaddr_in serverAddress = createServerAddress(ip, port);




    if (connect(socketFD,(struct sockaddr*)&serverAddress,sizeof(serverAddress)) == -1){
        printf("Failed to connect to server\n");
        return 1;
    }
    printf("Connected to server\n");

    char* mesage;
    mesage = "GET \\ HTTP/1.1\r\nHost: www.google.com\r\n\r\n";
    send(socketFD,mesage, strlen(mesage), 0);
    char buffer[1024];
    recv(socketFD,buffer,1024,0 );
    printf("Received: %s\n", buffer);


    return 0;
}