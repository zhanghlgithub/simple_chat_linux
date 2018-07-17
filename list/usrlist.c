#include "usrlist.h"

UsrNode *makeUsrNode()
{
    UsrNode *node = (UsrNode *)malloc(sizeof(UsrNode));
    if (NULL == node)
    {
        printf("malloc usr node failed\n");
        exit(EXIT_FAILURE);
    }
    memset(node, 0, sizeof(UsrNode));
    return node;
}

UsrList *makeUsrList()
{
    UsrList *list = (UsrList *)malloc(sizeof(UsrList));
    if (NULL == list)
    {
        printf("malloc usr list failed\n");
        exit(EXIT_FAILURE);
    }
    memset(list, 0, sizeof(UsrList));
    return list;
}

void insertUsrList(UsrList *list, UsrNode *node)
{
    if (NULL != list && NULL != node)
    {
        node->pNext = list->pFirstNode;
        list->pFirstNode = node;
        list->iLen++;
    }
}

void showUsrList(const UsrList *list)
{
    if (NULL == list)
    {
        return;
    }
    UsrNode *node = list->pFirstNode;
    while (NULL != node)
    {
        printf("id:%u, pwd:%s, sockfd:%d\t"
               , node->uiId, node->caPwd
               , node->sockfd);
        node = node->pNext;
    }
    putchar('\n');
}

UsrNode *findUsrNodeById(uint id, const UsrList *list)
{
    UsrNode *node = NULL;
    if (NULL != list)
    {
        node = list->pFirstNode;
        while (NULL != node)
        {
            if (id == node->uiId)
            {
                break;
            }
            node = node->pNext;
        }
    }
    return node;
}
