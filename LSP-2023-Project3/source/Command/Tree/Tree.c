

#include "Tree.h"


int Tree(char *argument)
{
    char pwdPath[MAX_BUFFER];
    char resolvedPath[MAX_BUFFER];
    char monitorFilePath[MAX_BUFFER];

    char *resolvedPtr;

    int checkResult;
    int showResult;


    if(argument == NULL)
    {
        Help();

        return false;
    }

    if(strcmp(argument, "") == 0)
    {
        Help();

        return false;
    }

    // 트리 출력할 디렉토리 검사
    checkResult = Check_Directory(argument);
    if(checkResult == false)
    {
        Help();

        return false;
    }

    memset(resolvedPath, 0, MAX_BUFFER);

    resolvedPtr = realpath(argument, resolvedPath);
    if(resolvedPtr == NULL)
    {
        Help();
        
        return false;
    }

    memset(pwdPath, 0, MAX_BUFFER);

    getcwd(pwdPath, MAX_BUFFER);

    // monitor_list.txt 파일 경로 생성
    Make_Monitor_File_Path(pwdPath, monitorFilePath);

    // monitor_list.txt 파일 검사
    checkResult = Check_File(monitorFilePath);
    if(checkResult == false)
    {
        return false;
    }

    // 트리 출력 디렉토리 모니터링 여부 검사
    checkResult = Check_Monitor_List(resolvedPath, monitorFilePath);
    if(checkResult == false)
    {
        Help();

        return false;
    }

    // 디렉토리 구조 트리 출력
    showResult = Show_Tree(resolvedPath, 0);
    if(showResult == false)
    {
        fprintf(stderr, "fail to show directory tree\n");

        return false;
    }

    return true;
}


int Show_Tree(char *dirPath, int depth)
{
    TreeList fileList;
    TreeList *travel;

    struct stat statbuf;

    struct dirent **dirEntry;

    char readPath[MAX_BUFFER];

    int scanResult;
    int showResult;


    memset(fileList.fileName, 0, MAX_FILE_BUFFER_SIZE + 1);
    fileList.nextNode = NULL;
    travel = NULL;

    scanResult = scandir(dirPath, &dirEntry, Directory_Filter, alphasort);
    if(scanResult == -1)
    {
        return false;
    }

    for(int i = 0; i < scanResult; i++)
    {
        memset(readPath, 0, MAX_BUFFER);

        strncpy(readPath, dirPath, strlen(dirPath));
        strncat(readPath, "/", 2);
        strncat(readPath, dirEntry[i]->d_name, strlen(dirEntry[i]->d_name));

        if(stat(readPath, &statbuf) == -1)
        {
            Delete_Directory_Entry(dirEntry, scanResult);

            if(fileList.nextNode != NULL)
            {
                Delete_TreeList(fileList.nextNode);
            }

            return false;
        }

        if((statbuf.st_mode & S_IFMT) == S_IFREG)
        {
            if(strcmp(fileList.fileName, "") == 0)
            {
                memset(fileList.fileName, 0, MAX_FILE_BUFFER_SIZE + 1);
                fileList.nextNode = NULL;

                travel = &fileList;
            }
            else
            {
                travel->nextNode = malloc(sizeof(TreeList));
                if(travel->nextNode == NULL)
                {
                    Delete_Directory_Entry(dirEntry, scanResult);

                    if(fileList.nextNode != NULL)
                    {
                        Delete_TreeList(fileList.nextNode);
                    }

                    return false;
                }

                memset(travel->nextNode->fileName, 0, MAX_FILE_BUFFER_SIZE + 1);
                travel->nextNode->nextNode = NULL;

                travel = travel->nextNode;
            }

            strncpy(travel->fileName, dirEntry[i]->d_name, strlen(dirEntry[i]->d_name));
        }
        else if((statbuf.st_mode & S_IFMT) == S_IFDIR)
        {
            if(depth > 0)
            {
                for(int k = 0; k < depth; k++)
                {
                    if(k == (depth - 1))
                    {
                        printf("|----");
                    }
                    else
                    {
                        if(k == 0)
                        {
                            printf("|    ");
                        }
                        else
                        {
                            printf("     ");
                        }
                    }
                }
            }

            printf("%s\n", dirEntry[i]->d_name);

            showResult = Show_Tree(readPath, depth + 1);
            if(showResult == false)
            {
                Delete_Directory_Entry(dirEntry, scanResult);

                if(fileList.nextNode != NULL)
                {
                    Delete_TreeList(fileList.nextNode);
                }

                return false;
            }
        }
    }
    
    travel = &fileList;
    while(travel != NULL)
    {
        if(depth > 0)
        {
            for(int k = 0; k < depth; k++)
            {
                if(k == (depth - 1))
                {
                    if(strcmp(travel->fileName, "") == 0)
                    {
                        printf("|");
                    }
                    else
                    {
                        printf("|----");
                    }
                }
                else
                {
                    if(k == 0)
                    {
                        printf("|    ");
                    }
                    else
                    {
                        printf("     ");
                    }
                }
            }
        }

        printf("%s\n", travel->fileName);

        travel = travel->nextNode;
    }

    Delete_Directory_Entry(dirEntry, scanResult);

    if(fileList.nextNode != NULL)
    {
            Delete_TreeList(fileList.nextNode);
    }

    return true;
}


int Directory_Filter(const struct dirent *item)
{
    if(strcmp("..", item->d_name) == 0)
    {
        return 0;
    }

    if(strcmp(".", item->d_name) == 0)
    {
        return 0;
    }

    return 1;
}


void Delete_Directory_Entry(struct dirent **dirEntry, int entryCnt)
{
    for(int i = 0; i < entryCnt; i++)
    {
        if(dirEntry[i] != NULL)
        {
            free(dirEntry[i]);
        }
    }

    if(dirEntry != NULL)
    {
        free(dirEntry);
    }

    return;
}


void Delete_TreeList(TreeList *list)
{
    TreeList *travel;
    TreeList *next;


    travel = list;
    while(travel != NULL)
    {
        next = travel->nextNode;

        free(travel);

        travel = next;
    }

    travel = NULL;
    next = NULL;
    list = NULL;

    return;
}