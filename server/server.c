#include "server.h"
#include "fileProtocol.h"
#include "chatProtocol.h"
#include "fileList.h"
#include "usrlist.h"
#include "file.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

extern FileList *g_pFileList;
extern UsrList *g_pUsrList;
extern uint g_uiBaseId;


static void *handleClient(void *arg)
{
    int sockfd = (int)arg;
    PDU *pdu = NULL;
    uint uiMsgType = 0;
    while (1)
    {
        //接收客户端的数据
        pdu = recvPDU(sockfd);
        if (NULL == pdu)
        {
            pthread_exit(NULL);
        }
        //判断消息类型
        //根据消息类型的不同做出不同的处理
        memcpy(&uiMsgType
               , (char *)(pdu->data)+3*sizeof(uint)
               , sizeof(uint));
        printf("msg type = %u\n", uiMsgType);
        switch (uiMsgType)
        {
        //处理客户端的注册请求
        case ENUM_CHAT_MSG_TYPE_REGIST_REQUEST:
            handleRegistRequest(sockfd, pdu);
            break;
            //处理客户端的登录请求
        case ENUM_CHAT_MSG_TYPE_LOGIN_REQUEST:
            handleLoginRequest(sockfd, pdu);
            showUsrList(g_pUsrList);
            printf("baseid = %u\n", g_uiBaseId);
            break;
            //处理客户端的私聊请求
        case ENUM_CHAT_MSG_TYPE_PRIVATE_CHAT_REQUEST:
            handlePrivateChatRequest(sockfd, pdu);
            break;
            //处理客户端的群聊请求
        case ENUM_CHAT_MSG_TYPE_GROUP_CHAT_REQUEST:
            handleGroupChatRequest(sockfd, pdu);
            break;
            //处理客户端的退出请求
        case ENUM_CHAT_MSG_TYPE_EXIT_REQUEST:
            handleExitRequest(sockfd, pdu);
            printf("线程退出\n");
            pthread_exit(NULL);
            break;
            //处理客户端的浏览文件请求
        case ENUM_FILE_MSG_TYPE_SCAN_REQUEST:
            handleScanRequest(sockfd, pdu);
            break;
            //处理客户端的上传文件请求
        case ENUM_FILE_MSG_TYPE_UPLOAD_REQUEST:
            handleUploadRequest(sockfd, pdu);
            break;
            //处理客户端的下载文件请求
        case ENUM_FILE_MSG_TYPE_DOWNLOAD_REQUEST:
            handleDownloadRequest(sockfd, pdu);
            break;
            //处理客户端的删除文件请求
        case ENUM_FILE_MSG_TYPE_DELETE_REQUEST:
            handleDeleteRequest(sockfd, pdu);
            break;
            //处理客户端的文件重命名请求
        case ENUM_FILE_MSG_TYPE_RENAME_REQUEST:
            handleRenameRequest(sockfd, pdu);
            break;
        default:
            break;
        }
        free(pdu->data);
        free(pdu);
    }
}

void setBaseId()
{
    UsrNode *node = g_pUsrList->pFirstNode;
    int sign = 0;
    while (NULL != node)
    {
        if (g_uiBaseId < node->uiId)
        {
            g_uiBaseId = node->uiId;
            sign = 1;
        }
        node = node->pNext;
    }
    if (1 == sign)
    {
        g_uiBaseId++;
    }
}

int makeSocket()
{
    //产生socket用于监听客户端的连接
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sockfd)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    return sockfd;
}

void makeBind(int sockfd)
{
    struct sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(8888);
    servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    bzero(servAddr.sin_zero, 8);

    //将sockfd和特定的ip及端口绑定
    //表示通过该scokfd来监听从绑定的ip连接过来的
    //作用于指定端口的客户端
    int ret = -1;
    ret = bind(sockfd
               , (struct sockaddr *)&servAddr
               , sizeof(servAddr));
    if (-1 == ret)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }
}

void makeListen(int sockfd)
{
    //设置该sockfd每次能够处理的最大客户端数
    int ret = listen(sockfd, 10);
    if (-1 == ret)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
}

void acceptClient(int sockfd)
{
    struct sockaddr_in clientAddr;
    int clientSockfd = -1;
    int iLen = sizeof(clientAddr);
    pthread_t thread;
    while (1)
    {
        printf("等待客户端的连接...\n");
        //阻塞等待客户端的连接
        //若有客户端连接过来，
        //则会自动将客户端的相应信息存入clientAddr中
        //然后往下执行
        //若有客户端连接服务器成功
        //则产生一个新的socket
        //该新的socket用于服务器和客户端数据交换
        clientSockfd = accept(sockfd
                              , (struct sockaddr *)&clientAddr
                              , &iLen);
        if (-1 == clientSockfd)
        {
            perror("accept");
            break;
        }
        //inet_ntoa:将整形表示的ip
        //          转换成点分十进制表示的ip
        printf("ip为%s的客户端连接到服务器\n"
               , inet_ntoa(clientAddr.sin_addr));

        //每来一个客户端的连接
        //创建一个新的线程来专门处理该客户端
        pthread_create(&thread, NULL
                       , handleClient
                       , (void *)clientSockfd);
    }
}
