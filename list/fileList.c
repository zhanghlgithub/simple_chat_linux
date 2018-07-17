#include "fileList.h"

FileNode *makeFileNode()
{
    FileNode *node = (FileNode *)malloc(sizeof(FileNode));
    if (NULL == node)
    {
        printf("malloc file node failed\n");
        exit(EXIT_FAILURE);
    }
    memset(node, 0, sizeof(FileNode));
    return node;
}

FileList *makeFileList()
{
    FileList *list = (FileList *)malloc(sizeof(FileList));
    if (NULL == list)
    {
        printf("malloc file list failed\n");
        exit(EXIT_FAILURE);
    }
    memset(list, 0, sizeof(FileList));
    return list;
}

void insertFileNodeToList(FileList *list, FileNode *node)
{
    if (NULL != list && NULL != node)
    {
        node->pNext = list->pFirstNode;
        list->pFirstNode = node;
        list->uiLen++;
    }
}

void showFileNode(const FileNode *node)
{
    if (NULL != node)
    {
        printf("filename:%s, attach num = %u\n"
                   , node->caPathname, node->uiAttachNum);
    }
}

void showFileList(const FileList *list)
{
    if (NULL != list)
    {
        FileNode *node = list->pFirstNode;
        while (NULL != node)
        {
            showFileNode(node);
            node = node->pNext;
        }
    }
}

FileNode *findFileByName(const FileList *list
                         , const char *pathname)
{
    FileNode *node = NULL;
    if (NULL != list && NULL != pathname)
    {
        node = list->pFirstNode;
        while (NULL != node)
        {
            if (0 == strcmp(node->caPathname, pathname))
            {
                break;
            }
            node = node->pNext;
        }
    }
    return node;
}

int delFileNodeByName(FileList *list, const char *pathname)
{
    int sign = 0;
    if (NULL != list && NULL != pathname)
    {
        FileNode *cur = list->pFirstNode;
        FileNode *pre = cur;
        while (NULL != cur)
        {
            if (0 == strcmp(cur->caPathname, pathname))
            {
                if (pre == cur)
                {
                    list->pFirstNode = cur->pNext;
                }
                else
                {
                    pre->pNext = cur->pNext;
                }
                break;
            }
            pre = cur;
            cur = cur->pNext;
        }
        if (NULL != cur)
        {
            sign = 1;
            free(cur);
        }
    }
    return sign;
}
