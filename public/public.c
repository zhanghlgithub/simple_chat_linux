#include "public.h"
#include "fileProtocol.h"
#include "chatProtocol.h"


PDU *makePDU(uint uiMsgLen, uint uiProtocolType)
{
    uint uiPDULen = 0;
    if (ENUM_PROTOCOL_TYPE_CHAT == uiProtocolType)
    {
        uiPDULen = sizeof(ChatPDU)-4*sizeof(char)
                                     +uiMsgLen;
    }
    else if (ENUM_PROTOCOL_TYPE_FILE_OPE == uiProtocolType)
    {
        uiPDULen = sizeof(FilePDU)-4*sizeof(char)
                                     +uiMsgLen;
    }
    PDU *pdu = (PDU *)malloc(sizeof(PDU));
    if (NULL == pdu)
    {
        printf("malloc ChatPDU failed\n");
        exit(EXIT_FAILURE);
    }

    pdu->data = malloc(uiPDULen);
    if (NULL == pdu->data)
    {
        printf("malloc PDU dat failed\n");
        exit(EXIT_FAILURE);
    }
    memset(pdu->data, 0, uiPDULen);

    memcpy(pdu->data, &uiPDULen, sizeof(uint));
    memcpy((char*)(pdu->data)+sizeof(uint)
                    , &uiProtocolType, sizeof(uint));
    memcpy((char*)(pdu->data)+2*sizeof(uint)
                    , &uiMsgLen, sizeof(uint));

    return pdu;
}

void sendPDU(int sockfd, PDU *pdu)
{
        if (NULL == pdu)
        {
            return;
        }
        int iSended = 0;
        int iLeft = 0;
        memcpy(&iLeft, pdu->data, sizeof(uint));
        printf("iLeft = %d\n", iLeft);
        int ret = -1;
        while (iLeft)
        {
            if (iLeft > PER_MAX_IO_BYTES)
            {
                ret = write(sockfd, (char *)(pdu->data)+iSended
                            , PER_MAX_IO_BYTES);
            }
            else
            {
                ret = write(sockfd, (char *)(pdu->data)+iSended
                            , iLeft);
            }
            if (-1 == ret)
            {
                perror("send PDU write");
                break;
            }
            iSended += ret;
            iLeft -= ret;
        }
        printf("send = %d\n", iSended);
}

PDU *recvPDU(int sockfd)
{
    uint uiPDULen = 0;
    int ret = 0;
    ret = read(sockfd, &uiPDULen, sizeof(uint));
    if (-1 == ret || 0 == ret)
    {
        return NULL;
    }
    printf("pdulen = %u\n", uiPDULen);
    uint uiProtocolType = 0;
    read(sockfd, &uiProtocolType, sizeof(uint));
    uint uiMsgLen = 0;
    read(sockfd, &uiMsgLen, sizeof(uint));

    PDU *pdu  = makePDU(uiMsgLen, uiProtocolType);

    int iRecved = 3*sizeof(uint);
    int iLeft = uiPDULen-iRecved;
    while (iLeft)
    {
        if (PER_MAX_IO_BYTES < iLeft)
        {
            ret = read(sockfd, (char*)(pdu->data)+iRecved
                       , PER_MAX_IO_BYTES);
        }
        else
        {
            ret = read(sockfd, (char*)(pdu->data)+iRecved
                       , iLeft);
        }
        if (-1 == ret || 0 == ret)
        {
            break;
        }
        iRecved += ret;
        iLeft -= ret;
    }
    return pdu;
}
