#include "fileProtocol.h"
#include "fileList.h"
#include "file.h"

FileList *g_pFileList = NULL;

int fileFace()
{
    printf("   ^_^\n");
    printf("1,浏览文件\n");
    printf("2,上传文件\n");
    printf("3,下载文件\n");
    printf("4,删除文件\n");
    printf("5,文件重命名\n");
    printf("0,退出\n");
    printf("请输入操作选项:\n");
    int num = -1;
    scanf("%d", &num);
    return num;
}

void scanFile(int sockfd)
{
    //生成一个浏览文件请求并发送给服务器
    PDU *pdu = makePDU(0, ENUM_PROTOCOL_TYPE_FILE_OPE);
    FilePDU *filepdu = (FilePDU *)pdu->data;
    filepdu->uiMsgType = ENUM_FILE_MSG_TYPE_SCAN_REQUEST;
    sendPDU(sockfd, pdu);
    free(pdu->data);
    free(pdu);
}

void uploadFile(int sockfd)
{
    printf("请输入要上传的文件的路径:\n");
    char caPathname[128] = {'\0'};
    scanf("%s", caPathname);

    //判断文件是否存在
    if (-1 == access(caPathname, F_OK))
    {
        perror("access");
        return;
    }

    //判断文件是否是常规文件
    //若输入的文件名是一个文件夹名，则不处理
    struct stat fstat;
    stat(caPathname, &fstat);
    if (!S_ISREG(fstat.st_mode))
    {
        printf("不是一个常规文件\n");
        return;
    }

    PDU *pdu = NULL;
    FilePDU *filepdu = NULL;

    //从路径中提取要上传的文件名
    char *p = strrchr(caPathname, '/');
    if (NULL == p)
    {
        p = caPathname;
    }
    else
    {
        p++;
    }

    int fd = openFile(caPathname, O_RDWR);
    //获得文件的大小
    off_t size = lseek(fd, 0, SEEK_END);

    //重新将文件的读写位置置为文件首部
    lseek(fd, 0, SEEK_SET);
    int ret = -1;
    //循环将文件的数据发送给服务器
    char caBuf[PER_MAX_IO_BYTES] = {'\0'};
    while (1)
    {
        memset(caBuf, 0, PER_MAX_IO_BYTES);
        ret = read(fd, caBuf, PER_MAX_IO_BYTES);
        if (-1 == ret || 0 == ret)
        {
            break;
        }
        pdu = makePDU(ret, ENUM_PROTOCOL_TYPE_FILE_OPE);
        filepdu = (FilePDU *)pdu->data;
        memcpy(filepdu->caMsg, caBuf, ret);
        filepdu->uiMsgType = ENUM_FILE_MSG_TYPE_UPLOAD_REQUEST;
        strcpy(filepdu->caFileName, p);
        filepdu->uiFileSize = size;
        sendPDU(sockfd, pdu);
        free(pdu->data);
        free(pdu);
    }

    close(fd);
}

void downloadFile(int sockfd)
{}

void deleteFile(int sockfd)
{
    //生成一个删除文件请求并发送给服务器
    PDU *pdu = makePDU(0, ENUM_PROTOCOL_TYPE_FILE_OPE);
    FilePDU *filepdu = (FilePDU *)pdu->data;
    filepdu->uiMsgType = ENUM_FILE_MSG_TYPE_DELETE_REQUEST;
    printf("请输入要删除的文件名:\n");
    scanf("%s", filepdu->caFileName);
    sendPDU(sockfd, pdu);
    free(pdu->data);
    free(pdu);
}

void fileOpe(int sockfd)
{
    int ret = -1;
    while (1)
    {
        ret = fileFace();
        switch (ret)
        {
        case 1:  //浏览文件
            scanFile(sockfd);
            break;
        case 2:  //上传文件
            uploadFile(sockfd);
            break;
        case 3:  //下载文件
            downloadFile(sockfd);
            break;
        case 4:  //删除文件
            deleteFile(sockfd);
            break;
        case 5:  //文件重命名
            renameFile(sockfd);
            break;
        case 0:  //退出
            return;
        default:
            printf("输入有误,请重新输入\n");
            break;
        }
    }
}

void renameFile(int sockfd)
{
    //生成一个重命名文件请求并发送给服务器
    PDU *pdu = makePDU(PATHNAME_LEN, ENUM_PROTOCOL_TYPE_FILE_OPE);
    FilePDU *filepdu = (FilePDU *)pdu->data;
    filepdu->uiMsgType = ENUM_FILE_MSG_TYPE_RENAME_REQUEST;
    printf("请输入要重命名的文件名:\n");
    scanf("%s", filepdu->caFileName);
    printf("请输入新的文件名:\n");
    scanf("%s", filepdu->caMsg);
    sendPDU(sockfd, pdu);
    free(pdu->data);
    free(pdu);
}

void handleScanRespond(PDU *pdu)
{
    if (NULL == pdu)
    {
        return;
    }

    FilePDU *filepdu = (FilePDU*)pdu->data;
    uint num = filepdu->uiMsgLen / PATHNAME_LEN;
    int i = 0;
    char caTmp[PATHNAME_LEN] = {'\0'};
    printf("服务器中的文件为:\n");
    for (; i < num; i++)
    {
        memset(caTmp, '\0', PATHNAME_LEN);
        memcpy(caTmp, filepdu->caMsg+i*PATHNAME_LEN, PATHNAME_LEN);
        printf("%s\t", caTmp);
    }
    putchar('\n');
}

void handleUploadRespond(PDU *pdu)
{
    if (NULL == pdu)
    {
        return;
    }
    FilePDU *filepdu = (FilePDU*)pdu->data;
    if (0 == strncmp(filepdu->caMsg, UPLOAD_OK, filepdu->uiMsgLen))
    {
        printf("上传文件成功\n");
    }
    else
    {
        printf("上传文件失败\n");
    }
}

void handleDownloadRespond(PDU *pdu)
{
    if (NULL == pdu)
    {
        return;
    }
}

void handleDeleteRespond(PDU *pdu)
{
    if (NULL == pdu)
    {
        return;
    }
    FilePDU *filepdu = (FilePDU*)pdu->data;
    if (0 == strncmp(filepdu->caMsg, DELETE_OK, filepdu->uiMsgLen))
    {
        printf("删除文件成功\n");
    }
    else
    {
        printf("删除文件失败\n");
    }
}

void handleRenameRespond(PDU *pdu)
{
    if (NULL == pdu)
    {
        return;
    }
    FilePDU *filepdu = (FilePDU*)pdu->data;
    if (0 == strncmp(filepdu->caMsg, RENAME_OK, filepdu->uiMsgLen))
    {
        printf("重命名文件成功\n");
    }
    else
    {
        printf("重命名文件失败\n");
    }
}


//将链表中保存的文件名全部遍历出来
//将文件名写入pdu中的caMsg中
//每64个字节保存一个文件名
void handleScanRequest(int sockfd, PDU *pdu)
{
    if (NULL == pdu || NULL == g_pFileList)
    {
        return;
    }
    PDU *respdu = NULL;
    FilePDU *filepdu = NULL;
    if (0 == g_pFileList->uiLen)
    {
        respdu = makePDU(PATHNAME_LEN, ENUM_PROTOCOL_TYPE_FILE_OPE);
        filepdu = (FilePDU *)respdu->data;
        strcpy(filepdu->caMsg, NO_FILE);
    }
    else if (g_pFileList->uiLen > 0)
    {
        //根据文件节点的个数计算出pdu->caMsg部分的大小
        uint uiMsgLen = g_pFileList->uiLen*PATHNAME_LEN;
        respdu = makePDU(uiMsgLen, ENUM_PROTOCOL_TYPE_FILE_OPE);
        filepdu = (FilePDU *)respdu->data;
        FileNode *node = g_pFileList->pFirstNode;
        int i = 0;
        //循环的将链表中的文件名写入pdu->caMsg中
        while (NULL != node)
        {
            memcpy(filepdu->caMsg+i*PATHNAME_LEN
                   , node->caPathname, PATHNAME_LEN);
            i++;
            node = node->pNext;
        }
    }
    filepdu->uiMsgType = ENUM_FILE_MSG_TYPE_SCAN_RESPOND;
    sendPDU(sockfd, respdu);
    free(respdu->data);
    free(respdu);
}

void handleUploadRequest(int sockfd, PDU *pdu)
{
    if (NULL == pdu)
    {
        return;
    }
    FilePDU *filepdu = (FilePDU *)(pdu->data);
    //将服务器文件名和上传的文件名合成一个路径
    //让后根据该路径创建并打开该文件
    char caPathname[2*PATHNAME_LEN] = {'\0'};
    strcpy(caPathname, SHARED_DIR);
    strcat(caPathname, filepdu->caFileName);
    int fd = openFile(caPathname, O_RDWR);
    int iRecved = 0;

    PDU *recvpdu = pdu;
    filepdu = (FilePDU *)(recvpdu->data);
    //保留文件的名字
    char caFileName[PATHNAME_LEN] = {'\0'};
    strcpy(caFileName, filepdu->caFileName);
    //保留文件的大小
    uint uiFileSize = filepdu->uiFileSize;
    //循环的将接收的客户端的文件的数据写入新创建的文件中
    while (1)
    {
        filepdu = (FilePDU *)(recvpdu->data);
        writeDataToFile(fd, filepdu->caMsg, filepdu->uiMsgLen);
        //用来保存已接收的文件的大小
        iRecved += filepdu->uiMsgLen;
        if (pdu != recvpdu)
        {
            printf("aaa: %p, %p\n", recvpdu->data, recvpdu);
            free(recvpdu->data);
            recvpdu->data = NULL;
            free(recvpdu);
            recvpdu = NULL;
        }
        //如果已接受的文件大小和文件的总的大小一样
        //则表示上传完成，退出
        if (iRecved == uiFileSize)
        {
            break;
        }
        else
        {
            recvpdu = recvPDU(sockfd);
        }
    }
    close(fd);

    //将新上传的文件生成节点信息并插入聊表中
    FileNode *node = makeFileNode();
    strcpy(node->caPathname, caFileName);
    insertFileNodeToList(g_pFileList, node);

    //产生一个上传回复给客户端
    PDU *respdu = makePDU(strlen(UPLOAD_OK), ENUM_PROTOCOL_TYPE_FILE_OPE);
    filepdu = (FilePDU *)respdu->data;
    filepdu->uiMsgType = ENUM_FILE_MSG_TYPE_UPLOAD_RESPOND;
    memcpy(filepdu->caMsg, UPLOAD_OK, strlen(UPLOAD_OK));
    sendPDU(sockfd, respdu);
    free(respdu->data);
    free(respdu);
}

void handleDownloadRequest(int sockfd, PDU *pdu)
{
    if (NULL == pdu)
    {
        return;
    }

}

void handleDeleteRequest(int sockfd, PDU *pdu)
{
    if (NULL == pdu)
    {
        return;
    }
    FilePDU *filepdu = (FilePDU *)pdu->data;
    PDU *respdu = NULL;
    int ret = delFileNodeByName(g_pFileList, filepdu->caFileName);
    if (1 == ret)
    {
        respdu = makePDU(strlen(DELETE_OK), ENUM_PROTOCOL_TYPE_FILE_OPE);
        filepdu = (FilePDU *)respdu->data;
        memcpy(filepdu->caMsg, DELETE_OK, filepdu->uiMsgLen);

        char caPathname[2*PATHNAME_LEN] = {'\0'};
        strcpy(caPathname, SHARED_DIR);
        strcat(caPathname, filepdu->caFileName);
        remove(caPathname);
    }
    else
    {
        respdu = makePDU(strlen(DELETE_FAILED), ENUM_PROTOCOL_TYPE_FILE_OPE);
        filepdu = (FilePDU *)respdu->data;
        memcpy(filepdu->caMsg, DELETE_FAILED, filepdu->uiMsgLen);
    }
    filepdu->uiMsgType = ENUM_FILE_MSG_TYPE_DELETE_RESPOND;
    sendPDU(sockfd, respdu);

    free(respdu->data);
    free(respdu);
}

void handleRenameRequest(int sockfd, PDU *pdu)
{
    if (NULL == pdu)
    {
        return;
    }
    FilePDU *filepdu = (FilePDU *)pdu->data;
    PDU *respdu = NULL;
    FileNode *node = findFileByName(g_pFileList, filepdu->caFileName);
    if (NULL != node)
    {
        memset(node->caPathname, '\0', PATHNAME_LEN);
        strncpy(node->caPathname, filepdu->caMsg, filepdu->uiMsgLen);

        char caOldPathname[2*PATHNAME_LEN] = {'\0'};
        strcpy(caOldPathname, SHARED_DIR);
        strcat(caOldPathname, filepdu->caFileName);

        char caNewPathname[2*PATHNAME_LEN] = {'\0'};
        strcpy(caNewPathname, SHARED_DIR);
        strncat(caNewPathname, filepdu->caMsg, filepdu->uiMsgLen);
        rename(caOldPathname, caNewPathname);

        respdu = makePDU(strlen(RENAME_OK), ENUM_PROTOCOL_TYPE_FILE_OPE);
        filepdu = (FilePDU *)respdu->data;
        memcpy(filepdu->caMsg, RENAME_OK, filepdu->uiMsgLen);
    }
    else
    {
        respdu = makePDU(strlen(RENAME_FAILED), ENUM_PROTOCOL_TYPE_FILE_OPE);
        filepdu = (FilePDU *)respdu->data;
        memcpy(filepdu->caMsg, RENAME_FAILED, filepdu->uiMsgLen);
    }
    filepdu->uiMsgType = ENUM_FILE_MSG_TYPE_RENAME_RESPOND;
    sendPDU(sockfd, respdu);
    free(respdu);
}
