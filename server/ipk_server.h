

#ifndef SERVER_IPK_SERVER_H
#define SERVER_IPK_SERVER_H

#include "../ipkcpc_utils.h"
#include <stdbool.h>
#include <stdlib.h>
#include "pthread.h"

struct AcceptedSocket {
    int socketFD;
    struct sockaddr_in clientAddress;
    int err;
    bool acepted;
};


struct AcceptedSocket *acceptIncomingConnection(int serverSocketFD);


#endif //SERVER_IPK_SERVER_H
