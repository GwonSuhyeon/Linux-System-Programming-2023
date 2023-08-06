#ifndef __FILELIST_H__
#define __FILELIST_H__


#include "../Common/CommonDefine.h"


// 디렉토리 내의 파일 정보를 가지는 노드
typedef struct FileNode
{
    char                fileName[ MAX_FILE_NAME_SIZE ];

    struct FileNode     *nextFileNode;
    // struct FileNode     *previousFileNode;

} FileNode;

// 디렉토리 정보를 가지는 노드
typedef struct DirectoryNode
{
    bool                    FLAG_NODE_IS_EMPTY;
    
    char                    dirPath[ MAX_PATH_BUFFER_SIZE ];

    int                     directoryCnt;
    
    struct DirectoryNode    *nextDirNode;
    // struct DirectoryNode    *previousDirNode;

    struct DirectoryNode    *subDirNode;

    struct FileNode         *fileNode;

} DirectoryNode;

// 파일을 관리하는 링크드리스트
typedef struct FileListNode
{   
    bool                    FLAG_NODE_IS_EMPTY;

    struct DirectoryNode    *currentDirNode;

} FileListNode;


#endif