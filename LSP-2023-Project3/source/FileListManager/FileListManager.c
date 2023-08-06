

#include "FileListManager.h"


FileNode *travelNode;


int Make_FileList(FileNode *FileList, char *monitoringPath)
{
    int scanResult;


    if(FileList == NULL)
    {
        return false;
    }

    travelNode = FileList;

    // 대상 디렉토리 탐색
    scanResult = Scan_Directory(FileList, monitoringPath);
    if(scanResult == false)
    {
        if(FileList != NULL)
        {
            free(FileList);
        }

        return false;
    }

    travelNode = NULL;

    return true;
}


FileNode* Make_FileNode()
{
    FileNode *newNode;


    newNode = malloc(sizeof(FileNode));
    if(newNode == NULL)
    {
        return NULL;
    }

    memset(newNode->path, 0, MAX_BUFFER);
    newNode->modifyTime = 0;
    newNode->nextNode = NULL;

    return newNode;
}


void Delete_FileList(FileNode *List)
{
    FileNode *travel;
    FileNode *next;


    if(List == NULL)
    {
        return;
    }
    
    travel = List->nextNode;

    while(travel != NULL)
    {
        next = travel->nextNode;

        travel->nextNode = NULL;

        free(travel);

        travel = next;
    }

    List->nextNode = NULL;
    
    free(List);

    return;
}


int Scan_Directory(FileNode *fileNode, char *path)
{
    struct dirent *dirEntry;
    struct stat statbuf;

    DIR *dirp;

    char fileName[MAX_FILE_BUFFER_SIZE + 1];
    char buf[MAX_BUFFER];

    int scanResult;


    dirp = opendir(path);
    if(dirp == NULL)
    {
        return false;
    }

    while(1)
    {
        memset(fileName, 0, MAX_FILE_BUFFER_SIZE);
        memset(buf, 0, MAX_BUFFER);

        dirEntry = readdir(dirp);
        if(dirEntry == NULL)
        {
            break;
        }

        if(dirEntry->d_ino == 0)
        {
            continue;
        }

        if(strcmp(dirEntry->d_name, ".") == 0 || strcmp(dirEntry->d_name, "..") == 0)
        {
            continue;
        }

        strncpy(fileName, dirEntry->d_name, strlen(dirEntry->d_name));

        strncpy(buf, path, strlen(path));
        strncat(buf, "/", 2);
        strncat(buf, fileName, strlen(fileName));

        if(stat(buf, &statbuf) == -1)
        {
            continue;
        }

        if((statbuf.st_mode & S_IFMT) == S_IFREG)
        {
            // 디렉토리 하위의 파일 정보를 저장할 노드 생성 및 파일 리스트에 추가

            travelNode->nextNode = Make_FileNode();
            if(travelNode->nextNode == NULL)
            {
                closedir(dirp);

                return false;
            }

            strncpy(travelNode->path, buf, strlen(buf));

            travelNode->modifyTime = statbuf.st_mtime;

            travelNode = travelNode->nextNode;
        }
        else if((statbuf.st_mode & S_IFMT) == S_IFDIR)
        {
            // 현재 디렉토리에 하위 디렉토리가 존재할 경우 재귀 탐색
            scanResult = Scan_Directory(travelNode, buf);
            if(scanResult == false)
            {
                closedir(dirp);

                return false;
            }
        }
    }

    closedir(dirp);

    return true;
}