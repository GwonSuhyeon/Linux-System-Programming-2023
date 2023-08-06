#ifndef __FILELIST_H__
#define __FILELIST_H__


#include "../Common/CommonDefine.h"


typedef struct FileNode
{
    char path[MAX_BUFFER];
    
    // char modifyTime[MAX_TIME_BUFFER_SIZE];

    time_t modifyTime;
    
    struct FileNode *nextNode;

}FileNode;


#endif