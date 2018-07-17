#include "server.h"
#include "fileList.h"
#include "file.h"

extern FileList *g_pFileList;
extern UsrList *g_pUsrList;

int main(void)
{
    //产生一个链表，用于保存文件的信息
    g_pFileList = makeFileList();
    //遍历目录获得目录下的文件信息
    scanFileInfoToFileList(SHARED_DIR, g_pFileList);
    showFileList(g_pFileList);

    //产生一个链表，用于保存客户端的信息
    g_pUsrList = makeUsrList();

    //从文件中获得之前注册的用户数据
    getUsrInfoFromFile(USRINFO_PATH, g_pUsrList);
    showUsrList(g_pUsrList);

    //设置新注册用户的起始id
    setBaseId();

    int sockfd = makeSocket();
    makeBind(sockfd);
    makeListen(sockfd);
    acceptClient(sockfd);

    return 0;
}
