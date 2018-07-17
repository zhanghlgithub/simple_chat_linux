#ifndef CLIENT_H
#define CLIENT_H

#include "public.h"

int makeSocket();
void connectToServer(int sockfd);
void mainLoop(int sockfd);

#endif // CLIENT_H

