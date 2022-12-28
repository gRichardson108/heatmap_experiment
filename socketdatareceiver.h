//
// Created by garret on 11/22/22.
//

#ifndef SOCKETHEATMAPDISPLAY_SOCKETDATARECEIVER_H
#define SOCKETHEATMAPDISPLAY_SOCKETDATARECEIVER_H

struct Sample
{
    short i;
    short q;
};

struct SocketData
{
    Sample samples[1024];
};

struct SocketBuffer
{
    int socketfd;
    unsigned int rowCount;
    SocketData* buffer;
};


SocketBuffer initializeReceiver();

void freeReceiver(SocketBuffer buffer);

void receiveToBuffer(SocketBuffer buffer);

#endif //SOCKETHEATMAPDISPLAY_SOCKETDATARECEIVER_H
