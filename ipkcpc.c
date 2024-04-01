/**
 * @file ipkcpc.c
 *
 * @brief IPK projekt 1
 *
 * @author Jakub Fukala (xfukal01)
 */

#include "ipkcpc_utils.h"

int main(int argc, char* argv[]){

    ProgramArguments args = parseArguments(argc, argv);

    if (strcmp(args.transport_protocol, "tcp") == 0){
        if (useTCP(args) != 0){
            fprintf(stderr, "Failed to use TCP\n");
            exit(1);
        }
    }
    else if (strcmp(args.transport_protocol, "udp") == 0){
        if (useUDP(args) != 0){
            fprintf(stderr, "Failed to use UDP\n");
            exit(1);
        }
    }
    else {
        fprintf(stderr, "Invalid transport protocol\n");
        exit(1);
    }

    return 0;
}