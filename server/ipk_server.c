//
// Created by fuki on 27.3.24.
//

#include "ipk_server.h"

struct AcceptedSocket aceeptedSockets[10];
int acceptedSocketsCount = 0;

struct sockaddr_in createServerAddress(char* ip, int16_t port){
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    // Convert port number to network byte order (big endian)
    serverAddress.sin_port = htons(port);
    // Convert IP address to binary form
    if (strlen(ip) == 0){
        serverAddress.sin_addr.s_addr = INADDR_ANY;
    }
    else {
        inet_pton(AF_INET, ip, &serverAddress.sin_addr.s_addr);
    }
    return serverAddress;
}

/**
 * Create a socket
 * @return socket file descriptor
 */
int createSocket(){
    return socket(AF_INET, SOCK_STREAM, 0);
}

/**
 * Accept client connection
 * @param serverSocketFD server socket file descriptor
 * @return accepted socket
 */
struct AcceptedSocket *acceptIncomingConnection(int serverSocketFD){
    struct sockaddr_in clientAddress;
    int clientAddressSize = sizeof(clientAddress);
    int clientSocketFD = accept(serverSocketFD, (struct sockaddr*)&clientAddress, (socklen_t*)&clientAddressSize);

    struct AcceptedSocket *acceptedSocket = malloc(sizeof(struct AcceptedSocket));
    acceptedSocket->socketFD = clientSocketFD;
    acceptedSocket->clientAddress = clientAddress;
    acceptedSocket->acepted = clientSocketFD > 0;

    if (!acceptedSocket->acepted){
        acceptedSocket->err = clientSocketFD;
    }
    return acceptedSocket;
}

void sendReceivedDataToClient(char *data, int socketFD){
    for (int i = 0; i < acceptedSocketsCount; i++) {
        if (aceeptedSockets[i].socketFD != socketFD){
            send(aceeptedSockets[i].socketFD, data, strlen(data), 0);
        }

    }
}


void receiveAndPrintIncomingData(int socketFD){
    char buffer[1024] = {0};
    while (true){
        memset(buffer, 0, 1024);
        ssize_t readAmount = recv(socketFD, buffer, 1024, 0);
        if (readAmount > 0){
            printf("Received %ld bytes: %s\n", readAmount, buffer);

            sendReceivedDataToClient(buffer, socketFD);
        }
        if (readAmount == 0){
            printf("Connection closed\n");
            break;
        }
    }
    close(socketFD);
}

void *accept_thread_forprint(void *arg){
    struct AcceptedSocket *acceptedSocket = (struct AcceptedSocket*)arg;
    printf("Client connected\n");
    receiveAndPrintIncomingData(acceptedSocket->socketFD);
    return NULL;
}



// Funkce pro nové vlákno
void *startAccteptingIncomingConnection(void *arg) {
    int serverSocketFD = *((int*)arg); // Přetypujeme argument na int (socket descriptor)

    while (1){
        struct AcceptedSocket *acceptedSocket = acceptIncomingConnection(serverSocketFD);

        // Vytvoříme vlákno
        pthread_t tid;
        pthread_create(&tid, NULL, accept_thread_forprint, (void*)acceptedSocket);

        //receiveAndPrintIncomingData(acceptedSocket->socketFD);
        // Zde můžete provést další operace s acceptedSocket

        //free(acceptedSocket); // Uvolníme paměť, když už nepotřebujeme strukturu AcceptedSocket
    }

    //free(acceptedSocket); // Uvolníme paměť, když už nepotřebujeme strukturu AcceptedSocket

    //pthread_exit(NULL); // Ukončíme vlákno
}


int main() {
    int serverSocket = createSocket();
    if (serverSocket == -1){
        printf("Failed to create server socket\n");
        return 1;
    }
    printf("Server socket created\n");

    struct sockaddr_in serverAddress = createServerAddress("", 1999);

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1){
        printf("Failed to bind server socket\n");
        return 1;
    }
    printf("Server socket binded\n");


    int result = listen(serverSocket, 5);
    if (result == -1){
        printf("Failed to listen on server socket\n");
        return 1;
    }


    // Accept client connection
    //startAcceptingIncomingConnections(serverSocket);
    // Vytvoříme vlákno
    //pthread_t tid;
    //pthread_create(&tid, NULL, startAccteptingIncomingConnection, (void*)&serverSocket);
    while (1){
        struct AcceptedSocket *acceptedSocket = acceptIncomingConnection(serverSocket);
        aceeptedSockets[acceptedSocketsCount++] = *acceptedSocket;
        // Vytvoříme vlákno
        pthread_t tid;
        pthread_create(&tid, NULL, accept_thread_forprint, (void*)acceptedSocket);


        //receiveAndPrintIncomingData(acceptedSocket->socketFD);
        // Zde můžete provést další operace s acceptedSocket

        //free(acceptedSocket); // Uvolníme paměť, když už nepotřebujeme strukturu AcceptedSocket
    }

    //printf("Client connected\n");

    //close(acceptedSocket->socketFD);
    shutdown(serverSocket, SHUT_RDWR);

    return 0;
}




