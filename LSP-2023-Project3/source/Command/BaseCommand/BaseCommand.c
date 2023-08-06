

#include "BaseCommand.h"


int Check_Directory(char *dirPath)
{
    struct stat statBuf;


    if(strcmp(dirPath, "") == 0)
    {
        return false;
    }

    if(access(dirPath, F_OK) == -1)
    {
        return false;
    }

    if(stat(dirPath, &statBuf) == -1)
    {
        return false;
    }

    if((statBuf.st_mode & S_IFMT) != S_IFDIR)
    {
        return false;
    }

    return true;
}


int Check_File(char *filePath)
{
    struct stat statBuf;

    int makeResult;


    if(strcmp(filePath, "") == 0)
    {
        return false;
    }

    if(access(filePath, F_OK) == -1)
    {
        makeResult = Make_File(filePath);
        if(makeResult == false)
        {
            return false;
        }
    }

    if(stat(filePath, &statBuf) == -1)
    {
        return false;
    }

    if((statBuf.st_mode & S_IFMT) != S_IFREG)
    {
        return false;
    }

    return true;
}


int Check_Monitor_List(char *dirPath, char *monitorFilePath)
{
    char buf[MAX_BUFFER];

    FILE *fp;

    int isDirFind;

    char *readResult;
    char *strPtr;


    fp = fopen(monitorFilePath, "r");
    if(fp == NULL)
    {
        return false;
    }

    isDirFind = false;
    while(1)
    {
        memset(buf, 0, MAX_BUFFER);

        readResult = fgets(buf, MAX_BUFFER, fp);
        if(readResult == NULL)
        {
            break;
        }

        buf[strlen(buf) - 1] = '\0';

        strPtr = strrchr(buf, ' ');
        if(strPtr == NULL)
        {
            continue;
        }

        *strPtr = '\0';

        if(strcmp(dirPath, buf) == 0)
        {
            isDirFind = true;

            break;
        }
    }

    if(isDirFind == false)
    {
        fclose(fp);

        return false;
    }

    fclose(fp);

    return true;
}


int Make_File(char *filePath)
{
    int fd;


    fd = open(filePath, O_RDWR | O_CREAT, 0777);
    if(fd == -1)
    {
        return false;
    }

    close(fd);

    return true;
}


void Make_Monitor_File_Path(char *pwdPath, char *monitorFilePath)
{
    memset(monitorFilePath, 0, MAX_BUFFER);

    strncpy(monitorFilePath, pwdPath, MAX_BUFFER);
    strncat(monitorFilePath, "/", 2);
    strncat(monitorFilePath, MONITOR_LIST_FILE_NAME, strlen(MONITOR_LIST_FILE_NAME) + 1);

    return;
}