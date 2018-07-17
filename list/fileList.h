#ifndef FILE_LIST_H
#define FILE_LIST_H

#include "public.h"

typedef struct FileNode
{
    char caPathname[PATHNAME_LEN]; //文件名
    uint uiAttachNum;  //正在访问该文件的客户端个数
    struct FileNode *pNext;
}FileNode;

typedef struct FileList
{
    uint uiLen;
    FileNode *pFirstNode;
}FileList;

FileNode *makeFileNode();
FileList *makeFileList();
void insertFileNodeToList(FileList *list, FileNode *node);
void showFileNode(const FileNode *node);
void showFileList(const FileList *list);
FileNode *findFileByName(const FileList *list, const char *pathname);
int delFileNodeByName(FileList *list, const char *pathname);

#endif // FILE_LIST_H

