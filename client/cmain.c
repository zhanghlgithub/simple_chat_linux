#include "client.h"

int main(void)
{
    int sockfd = makeSocket();
    connectToServer(sockfd);
    mainLoop(sockfd);

    return 0;
}
