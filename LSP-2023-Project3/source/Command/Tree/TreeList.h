#ifndef __TREELIST_H__
#define __TREELIST_H__


typedef struct TreeList
{
    char fileName[MAX_FILE_BUFFER_SIZE + 1];

    struct TreeList *nextNode;

}TreeList;


#endif