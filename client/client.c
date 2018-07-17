#include "client.h"
#include "fileProtocol.h"
#include "chatProtocol.h"
#include "file.h"

int makeSocket()
{
    //AF_INET:ipv4
    //SOCK_STREAM:使用可靠传输-->tcp
    //SOCK_DGRAM:非可靠传输-->udp
    //0: 使用传输默认的协议
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sockfd)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    return sockfd;
}

void connectToServer(int sockfd)
{
    struct sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    //htons:表示将主机字节序转换为网络字节序
    //字节序：大端字节序，小端字节序
    //端口用来标识应用
    servAddr.sin_port = htons(8888);
    //设置要连接的服务器的ip地址
    //inet_addr:将点分十进制表示的ip转换成整数表示
    servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    //将指定地址的开始往后的多少个字节置为0
    bzero(servAddr.sin_zero, 8);

    int ret = -1;
    //连接服务器
    ret = connect(sockfd
                  , (struct sockaddr *)&servAddr
                  , sizeof(servAddr));
    if (-1 == ret)
    {
        perror("connect");
        exit(EXIT_FAILURE);
    }
    printf("connect to server success\n");
}

//循环接收服务器的信息并处理
void *handleServer(void *arg)
{
    int sockfd = (int)arg;
    PDU *pdu = NULL;
    uint uiMsgType = 0;
    while (1)
    {
        //接收服务器的消息
        pdu = recvPDU(sockfd);
        if (NULL == pdu)
        {
            printf("和服务器已断开\n");
            exit(0);
        }
        //根据消息的类型做出不同的处理
        memcpy(&uiMsgType
              , (char *)(pdu->data)+3*sizeof(uint)
              , sizeof(uint));
        printf("client-msgtype=%u\n", uiMsgType);
        switch (uiMsgType)
        {
        //处理服务器返回的回复信息
        case ENUM_FILE_MSG_TYPE_SCAN_RESPOND:
            handleScanRespond(pdu);
            break;
        case ENUM_FILE_MSG_TYPE_UPLOAD_RESPOND:
            handleUploadRespond(pdu);
            break;
        case ENUM_FILE_MSG_TYPE_DOWNLOAD_RESPOND:
            handleDownloadRespond(pdu);
            break;
        case ENUM_FILE_MSG_TYPE_DELETE_RESPOND:
            handleDeleteRespond(pdu);
            break;
        case ENUM_FILE_MSG_TYPE_RENAME_RESPOND:
            handleRenameRespond(pdu);
            break;
        case ENUM_CHAT_MSG_TYPE_PRIVATE_CHAT_REQUEST:
        case ENUM_CHAT_MSG_TYPE_GROUP_CHAT_REQUEST:
            handleChatRespond(pdu);
            break;
        default:
            break;
        }
        free(pdu->data);
        free(pdu);
    }
}

int chatAndFileOpeFace()
{
    printf("  ^_^\n");
    printf("1,聊天\n");
    printf("2,文件操作\n");
    printf("0,退出\n");
    printf("请输入操作选项:\n");
    int num = -1;
    scanf("%d", &num);
    return num;
}

static int chatAndFileOpe(int sockfd)
{
    pthread_t thread;
    pthread_create(&thread, NULL, handleServer
                   , (void *)sockfd);

    int num = -1;
    while (1)
    {
        num = chatAndFileOpeFace();
        switch (num)
        {
        case 1:  //聊天
            chat(sockfd);
            break;
        case 2:  //文件操作
            fileOpe(sockfd);
            break;
        case 0:  //退出
            pthread_cancel(thread);
            return;
        default:
            printf("输入有误,请重新输入\n");
            break;
        }
    }
}

void mainLoop(int sockfd)
{
    char sign = '\0';
    while (1)
    {
        //登录和注册
        loginOrRegist(sockfd);

        //聊天和文件操作
        chatAndFileOpe(sockfd);
        printf("退出程序？Y/y\n");
        getchar();
        sign = getchar();
        getchar();
        if ('y' == sign || 'Y' == sign)
        {
            exitPrograms(sockfd);
            break;
        }
    }
}
