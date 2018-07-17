#include "file.h"
#include <dirent.h>

int openFile(const char *pathname, int flags)
{
    int fd = open(pathname, flags|O_CREAT, 0664);
    if (-1 == fd)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }
    return fd;
}

ssize_t writeDataToFile(int fd, void *buf, size_t count)
{
    ssize_t iWrited = -1;
    if (NULL != buf && count > 0)
    {
        int ret = -1;
        while (count)
        {
            if (PER_MAX_IO_BYTES < count)
            {
                ret = write(fd, (char *)buf+iWrited, PER_MAX_IO_BYTES);
            }
            else
            {
                ret = write(fd, (char *)buf+iWrited, count);
            }
            if (-1 == ret || 0 == ret)
            {
                break;
            }
            iWrited += ret;
            count -= ret;
        }
    }

    return iWrited;
}

ssize_t readDataFromFile(int fd, void *buf, size_t count)
{
    ssize_t iReaded = -1;
    if (NULL != buf && count > 0)
    {
        int ret = -1;
        while (count)
        {
            if (PER_MAX_IO_BYTES < count)
            {
                ret = read(fd, (char *)buf+iReaded, PER_MAX_IO_BYTES);
            }
            else
            {
                ret = read(fd, (char *)buf+iReaded, count);
            }
            if (0 == ret || -1 == ret)
            {
                break;
            }
            iReaded += ret;
            count -= ret;
        }
    }

    return iReaded;
}

void scanFileInfoToFileList(const char *dir, FileList *list)
{
    if (NULL != dir && NULL != list)
    {
        DIR *pdir = opendir(dir);
        if (NULL == pdir)
        {
            perror("opendir");
            exit(EXIT_FAILURE);
        }
        struct dirent *pdirent = NULL;
        FileNode *node = NULL;
        while (1)
        {
            pdirent = readdir(pdir);
            if (NULL == pdirent)
            {
                break;
            }
            if (0 == strcmp(".", pdirent->d_name)
                 || 0 == strcmp("..", pdirent->d_name))
            {
                continue;
            }

            node = makeFileNode();
            strcpy(node->caPathname, pdirent->d_name);
            node->uiAttachNum = 0;
            insertFileNodeToList(list, node);
        }
    }
}

void saveUsrInfoToFile(const char *pathname, const UsrList *list)
{
    if (NULL == list)
    {
        return;
    }

    int fd = openFile(USRINFO_PATH, O_RDWR);

    UsrNode *node = list->pFirstNode;
    while (NULL != node)
    {
        write(fd, node, sizeof(UsrNode));
        node = node->pNext;
    }

    close(fd);
}
void getUsrInfoFromFile(const char *pathname, UsrList *list)
{
    if (NULL == list)
    {
        return;
    }

    int fd = openFile(USRINFO_PATH, O_RDWR);

    UsrNode *node = NULL;
    int ret = -1;
    while (1)
    {
        node = makeUsrNode();
        ret = read(fd, node, sizeof(UsrNode));
        if (-1 == ret || 0 == ret)
        {
            free(node);
            break;
        }
        node->sockfd = -1;
        node->pNext = NULL;
        insertUsrList(list, node);
    }
    close(fd);
}
