#ifndef FILE_H
#define FILE_H

#include "fileList.h"
#include "usrlist.h"

#define SHARED_DIR "./SharedFile/"
#define USRINFO_PATH "./usr.info"

//打开文件
int openFile(const char *pathname, int flags);
//将数据写入文件
ssize_t writeDataToFile(int fd, void *buf, size_t count);
//将数据从文件中读取出来
ssize_t readDataFromFile(int fd, void *buf, size_t count);
//浏览文件夹，将文件夹中的文件信息写入链表中
void scanFileInfoToFileList(const char *dir, FileList *list);

void saveUsrInfoToFile(const char *pathname, const UsrList *list);
void getUsrInfoFromFile(const char *pathname, UsrList *list);

#endif // FILE_H
