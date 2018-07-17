#ifndef CHAT_PROTOCOL_H
#define CHAT_PROTOCOL_H

#include "public.h"

enum ENUM_CHAT_MSG_TYPE
{
    ENUM_CHAT_MSG_TYPE_MIN = 0,
    ENUM_CHAT_MSG_TYPE_REGIST_REQUEST = 20,       //注册请求
    ENUM_CHAT_MSG_TYPE_REGIST_RESPOND,       //注册回复
    ENUM_CHAT_MSG_TYPE_LOGIN_REQUEST,        //登录请求
    ENUM_CHAT_MSG_TYPE_LOGIN_RESPOND,        //登录回复
    ENUM_CHAT_MSG_TYPE_PRIVATE_CHAT_REQUEST, //私聊请求
    ENUM_CHAT_MSG_TYPE_PRIVATE_CHAT_RESPOND, //私聊回复
    ENUM_CHAT_MSG_TYPE_GROUP_CHAT_REQUEST,   //群聊请求
    ENUM_CHAT_MSG_TYPE_GROUP_CHAT_RESPOND,   //群聊回复
    ENUM_CHAT_MSG_TYPE_EXIT_REQUEST,         //退出请求
    ENUM_CHAT_MSG_TYPE_EXIT_RESPOND,         //退出回复
    ENUM_CHAT_MSG_TYPE_MAX = 0x00ffffff
};

#define LOGIN_OK "ok"
#define LOGIN_FAILED "failed"

typedef struct ChatPDU  //协议数据单元
{
	uint uiPDULen;        //消息的总的大小
    uint uiProtocolType;
    uint uiMsgLen;        //实际消息的大小
	uint uiMsgType;       //消息的类型
	uint uiFrom;          //消息发送者的id
	uint uiTo;            //消息接收者的id
	char caMsg[4];        //实际消息
}ChatPDU;

//客户端向服务器发送登录注册请求函数
int loginOrRegistFace();
void regist(int sockfd);
int login(int sockfd);
 void exitPrograms(int sockfd);
void loginOrRegist(int sockfd);

//客户端向服务器发送群聊私聊请求函数
int chatFace();
void privateChat(int sockfd);
void groupChat(int sockfd);
void chat(int sockfd);

//客户端处理服务器回复函数
void handleChatRespond(PDU *pdu);

//服务器处理客户端请求函数
void handleRegistRequest(int sockfd, PDU *pdu);
void handleLoginRequest(int sockfd, PDU *pdu);
void handlePrivateChatRequest(int sockfd, PDU *pdu);
void handleGroupChatRequest(int sockfd, PDU *pdu);
void handleExitRequest(int sockfd, PDU *pdu);

#endif
