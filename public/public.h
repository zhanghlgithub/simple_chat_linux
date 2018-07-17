#ifndef PUBLIC_H
#define PUBLIC_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PWD_LEN 32
#define PATHNAME_LEN 64
#define PER_MAX_IO_BYTES 4096
typedef unsigned int uint;

enum ENUM_PROTOCOL_TYPE
{
    ENUM_PROTOCOL_TYPE_MIN = 0,
    ENUM_PROTOCOL_TYPE_FILE_OPE,
    ENUM_PROTOCOL_TYPE_CHAT,
    ENUM_PROTOCOL_TYPE_MAX = 0x00ffffff
};

typedef struct PDU
{
    void *data;
    uint uiPDUType;
}PDU;

PDU *makePDU(uint uiMsgLen, uint uiProtocolType);
void sendPDU(int sockfd, PDU *pdu);
PDU *recvPDU(int sockfd);

#endif // PUBLIC_H

