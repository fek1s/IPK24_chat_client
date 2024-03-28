//
// Created by fuki on 22.3.24.
//


#include "ipkcpc_utils.h"

struct sockaddr_in resolve_host(char *ip,u_int16_t port){
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;

    getaddrinfo(ip, NULL, &hints, &res);

    struct sockaddr_in *addr = (struct sockaddr_in*)res->ai_addr;
    // @DEBUG
    printf("Server IP: %s\n", inet_ntoa(addr->sin_addr));
    addr->sin_port = htons(port);
    return *addr;
}

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
            printf("Received %ld bytes: %s", recvAmount, buffer);
        }
    }
    return NULL;
}

int createTcpSocket(){
    return socket(AF_INET, SOCK_STREAM, 0);
}

int createUdpSocket(){
    return socket(AF_INET, SOCK_DGRAM, 0);
}

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

