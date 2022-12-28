//
// Created by garret on 11/22/22.
//

#include "socketdatareceiver.h"

#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>

SocketBuffer initializeReceiver() {
    SocketBuffer output = {};

    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd = 0;
    int s = 0;
    struct sockaddr_storage peer_addr;
    socklen_t peer_addr_len;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
    hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
    hints.ai_protocol = 0;          /* Any protocol */
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    s = getaddrinfo(NULL, "8888", &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }

    /* getaddrinfo() returns a list of address structures.
       Try each address until we successfully bind(2).
       If socket(2) (or bind(2)) fails, we (close the socket
       and) try the next address. */

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1)
            continue;

        if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
            break;                  /* Success */

        close(sfd);
    }

    freeaddrinfo(result);           /* No longer needed */
    output.socketfd = sfd;
    if (sfd > 0) {
        output.buffer = (SocketData*) malloc(sizeof(SocketData)*1024);
        output.rowCount = 1024;
    }

    return output;
}

void receiveToBuffer(SocketBuffer buffer) {
    ssize_t bytes = recvfrom(buffer.socketfd, buffer.buffer, buffer.rowCount * sizeof(SocketData),
                             MSG_DONTWAIT, NULL, NULL);
    if (bytes > 0) {

    }
}

void freeReceiver(SocketBuffer buffer) {
    if (buffer.socketfd > 0) {
        close(buffer.socketfd);
    }
    free(buffer.buffer);
}

