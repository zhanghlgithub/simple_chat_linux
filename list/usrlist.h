#ifndef USRLIST_H
#define USRLIST_H

#include "public.h"

typedef struct UsrNode
{
    uint uiId;
    char caPwd[PWD_LEN];
    int sockfd;
    struct UsrNode *pNext;
}UsrNode;

typedef struct UsrList
{
    int iLen;
    UsrNode *pFirstNode;
}UsrList;

UsrNode *makeUsrNode();
UsrList *makeUsrList();
void insertUsrList(UsrList *list, UsrNode *node);
void showUsrList(const UsrList *list);
UsrNode *findUsrNodeById(uint id, const UsrList *list);

#endif // USRLIST_H

