#include "chatProtocol.h"
#include "usrlist.h"
#include "file.h"

UsrList *g_pUsrList = NULL;
uint g_uiBaseId = 1000;
uint g_uiId = 0;

int loginOrRegistFace()
{
    printf("  欢迎\n");
    printf("1,登录\n");
    printf("2,注册\n");
    printf("0,退出\n");
    printf("请输入选项:\n");
    int num = 0;
    scanf("%d", &num);
    return num;
}
void regist(int sockfd)
{
    PDU *pdu = makePDU(PWD_LEN, ENUM_PROTOCOL_TYPE_CHAT);
    ChatPDU *chatpdu = (ChatPDU *)pdu->data;
    chatpdu->uiMsgType = ENUM_CHAT_MSG_TYPE_REGIST_REQUEST;
    printf("请输入注册需要的密码：\n");
    scanf("%s", chatpdu->caMsg);

    printf("pdulen = %u, protype = %u, msglen = %u, msgtype = %u\n"
               , chatpdu->uiPDULen, chatpdu->uiProtocolType
               , chatpdu->uiMsgLen, chatpdu->uiMsgType);

    sendPDU(sockfd, pdu);
    free(pdu->data);
    free(pdu);

    pdu = recvPDU(sockfd);
    chatpdu = (ChatPDU *)pdu->data;
    if (ENUM_CHAT_MSG_TYPE_REGIST_RESPOND
         == chatpdu->uiMsgType)
    {
        g_uiId = chatpdu->uiTo;
        printf("获得注册的id: %u\n", g_uiId);
    }
    else
    {
        printf("注册失败\n");
    }
    free(pdu->data);
    free(pdu);
}
int login(int sockfd)
{
    int id = 0;
    PDU *pdu = makePDU(PWD_LEN, ENUM_PROTOCOL_TYPE_CHAT);
    ChatPDU *chatpdu = (ChatPDU *)pdu->data;
    printf("请输入id:\n");
    scanf("%u", &(chatpdu->uiFrom));
    id = chatpdu->uiFrom;
    printf("请输入密码:\n");
    scanf("%s", chatpdu->caMsg);
    chatpdu->uiMsgType = ENUM_CHAT_MSG_TYPE_LOGIN_REQUEST;
    sendPDU(sockfd, pdu);
    free(pdu->data);
    free(pdu);

    pdu = recvPDU(sockfd);
    chatpdu = (ChatPDU *)pdu->data;
    if (ENUM_CHAT_MSG_TYPE_LOGIN_RESPOND
        && 0 == strncmp(LOGIN_OK, chatpdu->caMsg
                        , chatpdu->uiMsgLen))
    {
        printf("登录成功\n");
        g_uiId = id;
        return 1;
    }
    printf("登录失败\n");
    write(STDOUT_FILENO, chatpdu->caMsg, chatpdu->uiMsgLen);
    putchar('\n');
    free(pdu->data);
    free(pdu);

    return -1;
}
void exitPrograms(int sockfd)
{
    //生成一个退出请求并发送给服务器
    PDU *pdu = makePDU(0, ENUM_PROTOCOL_TYPE_CHAT);
    ChatPDU *chatpdu = (ChatPDU *)pdu->data;
    chatpdu->uiMsgType = ENUM_CHAT_MSG_TYPE_EXIT_REQUEST;
    sendPDU(sockfd, pdu);
    free(pdu->data);
    free(pdu);
    printf("发送退出请求\n");
}
void loginOrRegist(int sockfd)
{
    int num = -1;
    int ret = -1;
    while (1)
    {
        num = loginOrRegistFace();
        switch (num)
        {
        case 1:
            ret = login(sockfd);
            break;
        case 2:
            regist(sockfd);
            break;
        case 0:
            exitPrograms(sockfd);
            exit(EXIT_SUCCESS);
        default:
            printf("输入有误!!!\n");
            break;
        }
        if (1 == ret)
        {
            break;
        }
    }
}

int chatFace()
{
    printf("  ^_^\n");
    printf("1,私聊\n");
    printf("2,群聊\n");
    printf("0,返回\n");
    printf("请输入选项:\n");
    int num = 0;
    scanf("%d", &num);
    return num;
}
void privateChat(int sockfd)
{
    printf("请输入聊天的对象:\n");
    uint perid = 0;
    scanf("%u", &perid);

    char caMsg[PER_MAX_IO_BYTES] = {'\0'};
    printf("请输入聊天信息:\n");
    read(STDIN_FILENO, caMsg
         , PER_MAX_IO_BYTES);
    PDU *pdu = makePDU(strlen(caMsg), ENUM_PROTOCOL_TYPE_CHAT);
    ChatPDU *chatpdu = (ChatPDU *)pdu->data;
    chatpdu->uiFrom = g_uiId;
    chatpdu->uiTo = perid;
    strncpy(chatpdu->caMsg, caMsg, strlen(caMsg));
    chatpdu->uiMsgType = ENUM_CHAT_MSG_TYPE_PRIVATE_CHAT_REQUEST;
    sendPDU(sockfd, pdu);
    free(pdu->data);
    free(pdu);
}
void groupChat(int sockfd)
{
    char caMsg[PER_MAX_IO_BYTES] = {'\0'};
    printf("请输入聊天信息:\n");
    read(STDIN_FILENO, caMsg
         , PER_MAX_IO_BYTES);
    PDU *pdu = makePDU(strlen(caMsg), ENUM_PROTOCOL_TYPE_CHAT);
    ChatPDU *chatpdu = (ChatPDU *)pdu->data;
    chatpdu->uiFrom = g_uiId;
    strncpy(chatpdu->caMsg, caMsg, strlen(caMsg));
    chatpdu->uiMsgType = ENUM_CHAT_MSG_TYPE_GROUP_CHAT_REQUEST;
    sendPDU(sockfd, pdu);
    free(pdu->data);
    free(pdu);
}
void chat(int sockfd)
{
    int num = -1;
    while (1)
    {
        num = chatFace();
        switch (num)
        {
        case 1:
            privateChat(sockfd);
            break;
        case 2:
            groupChat(sockfd);
            break;
        case 0:
            return;
        default:
            printf("输入有误,请重新输入\n");
            break;
        }
    }
}

void handleChatRespond(PDU *pdu)
{
    if (NULL == pdu)
    {
        return;
    }
    ChatPDU *chatpdu = (ChatPDU *)pdu->data;
    printf("%u says: \n", chatpdu->uiFrom);
    write(STDOUT_FILENO, chatpdu->caMsg, chatpdu->uiMsgLen);
}

void handleRegistRequest(int sockfd, PDU *pdu)
{
    ChatPDU *chatpdu = (ChatPDU *)pdu->data;
    //产生节点用于保留客户端的注册信息并存入链表
    //eg: id 和 密码
    UsrNode *node = makeUsrNode();
    node->sockfd = -1;
    node->uiId = g_uiBaseId;
    strncpy(node->caPwd, chatpdu->caMsg, PWD_LEN);
    insertUsrList(g_pUsrList, node);

    //将链表中的数据写入文件
    saveUsrInfoToFile(USRINFO_PATH, g_pUsrList);

    //给客户端产生一个注册回复
    PDU *respdu = makePDU(0, ENUM_PROTOCOL_TYPE_CHAT);
    chatpdu = (ChatPDU *)respdu->data;
    chatpdu->uiMsgType = ENUM_CHAT_MSG_TYPE_REGIST_RESPOND;
    chatpdu->uiTo = g_uiBaseId;
    g_uiBaseId++;
    sendPDU(sockfd, respdu);
    free(respdu->data);
    free(respdu);
}

void handleLoginRequest(int sockfd, PDU *pdu)
{
    if (NULL == pdu)
    {
        return;
    }
    ChatPDU *chatpdu = (ChatPDU *)pdu->data;
    UsrNode *node = g_pUsrList->pFirstNode;
    while (NULL != node)
    {
        if (node->uiId == chatpdu->uiFrom
            && 0 == strncmp(node->caPwd, chatpdu->caMsg, PWD_LEN))
        {
            if (-1 == node->sockfd)
            {
                node->sockfd = sockfd;
            }
            else
            {
                node = NULL;
            }
            break;
        }
        node = node->pNext;
    }

    PDU *respdu = NULL;
    if (NULL != node)
    {
        respdu = makePDU(strlen(LOGIN_OK), ENUM_PROTOCOL_TYPE_CHAT);
        chatpdu = (ChatPDU *)respdu->data;
        strncpy(chatpdu->caMsg, LOGIN_OK
                , strlen(LOGIN_OK));
    }
    else
    {
        respdu = makePDU(strlen(LOGIN_FAILED), ENUM_PROTOCOL_TYPE_CHAT);
        chatpdu = (ChatPDU *)respdu->data;
        strncpy(chatpdu->caMsg, LOGIN_FAILED
                , strlen(LOGIN_FAILED));
    }
    chatpdu->uiMsgType = ENUM_CHAT_MSG_TYPE_LOGIN_RESPOND;
    sendPDU(sockfd, respdu);
    free(respdu->data);
    free(respdu);
}

void handlePrivateChatRequest(int sockfd, PDU *pdu)
{
    if (NULL == pdu)
    {
        return;
    }
    ChatPDU *chatpdu = (ChatPDU *)pdu->data;

    UsrNode *node = g_pUsrList->pFirstNode;
    while (NULL != node)
    {
        if (node->uiId == chatpdu->uiTo)
        {
            printf("id=%u,sockfd=%u\n", node->uiId, node->sockfd);
            sendPDU(node->sockfd, pdu);
            break;
        }
        node = node->pNext;
    }
}

void handleGroupChatRequest(int sockfd, PDU *pdu)
{
    if (NULL == pdu)
    {
        return;
    }
    UsrNode *node = g_pUsrList->pFirstNode;
    while (NULL != node)
    {
        if (node->sockfd > 0)
        {
            sendPDU(node->sockfd, pdu);
        }
        node = node->pNext;
    }
}

void handleExitRequest(int sockfd, PDU *pdu)
{
    if (NULL == pdu)
    {
        return;
    }
    ChatPDU *chatpdu = (ChatPDU *)pdu->data;
    UsrNode *node = findUsrNodeById(chatpdu->uiFrom, g_pUsrList);
    if (NULL != node)
    {
        node->sockfd = -1;
    }
}
