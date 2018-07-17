#ifndef FILE_PROTOCOL_H
#define FILE_PROTOCOL_H

#include "public.h"

#define NO_FILE "no file"
#define UPLOAD_OK "upload ok"
#define DELETE_OK "delete ok"
#define DELETE_FAILED "delete failed"
#define RENAME_OK "rename ok"
#define RENAME_FAILED "rename failed"

enum ENUM_FILE_MSG_TYPE
{
    ENUM_FILE_MSG_TYPE_MIN = 0,
    ENUM_FILE_MSG_TYPE_SCAN_REQUEST,
    ENUM_FILE_MSG_TYPE_SCAN_RESPOND,
    ENUM_FILE_MSG_TYPE_UPLOAD_REQUEST,
    ENUM_FILE_MSG_TYPE_UPLOAD_RESPOND,
    ENUM_FILE_MSG_TYPE_DOWNLOAD_REQUEST,
    ENUM_FILE_MSG_TYPE_DOWNLOAD_RESPOND,
    ENUM_FILE_MSG_TYPE_RENAME_REQUEST,
    ENUM_FILE_MSG_TYPE_RENAME_RESPOND,
    ENUM_FILE_MSG_TYPE_DELETE_REQUEST,
    ENUM_FILE_MSG_TYPE_DELETE_RESPOND,
    ENUM_FILE_MSG_TYPE_EXIT_REQUEST,
    ENUM_FILE_MSG_TYPE_EXIT_RESPOND,
    ENUM_FILE_MSG_TYPE_MAX = 0x00ffffff
};

typedef struct FilePDU
{
    uint uiPDULen;
    uint uiProtocolType;
    uint uiMsgLen;
    uint uiMsgType;
    char caFileName[PATHNAME_LEN];
    uint uiFileSize;
    char caMsg[4];
}FilePDU;

//客户端向服务器发送各类请求函数
int fileFace();
void scanFile(int sockfd);
void uploadFile(int sockfd);
void downloadFile(int sockfd);
void deleteFile(int sockfd);
void renameFile(int sockfd);
void fileOpe(int sockfd);

//客户端处理服务器回复函数
void handleScanRespond(PDU *pdu);
void handleUploadRespond(PDU *pdu);
void handleDownloadRespond(PDU *pdu);
void handleDeleteRespond(PDU *pdu);
void handleRenameRespond(PDU *pdu);

//服务器处理客户端请求函数
void handleScanRequest(int sockfd, PDU *pdu);
void handleUploadRequest(int sockfd, PDU *pdu);
void handleDownloadRequest(int sockfd, PDU *pdu);
void handleDeleteRequest(int sockfd, PDU *pdu);
void handleRenameRequest(int sockfd, PDU *pdu);

#endif // FILE_PROTOCOL_H

