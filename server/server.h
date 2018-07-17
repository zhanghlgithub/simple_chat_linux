#ifndef SERVER_H
#define SERVER_H

#include "public.h"

void setBaseId();
int makeSocket();
void makeBind(int sockfd);
void makeListen(int sockfd);
void acceptClient(int sockfd);


#endif // SERVER_H

