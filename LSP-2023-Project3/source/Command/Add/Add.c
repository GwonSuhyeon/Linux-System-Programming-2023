

#include "Add.h"


int Add(char *argDirPath, char *prompt)
{
    char dirPath[MAX_BUFFER];
    // char option[MAX_BUFFER];
    char pwdPath[MAX_BUFFER];
    char resolvedPath[MAX_BUFFER];
    char monitorFilePath[MAX_BUFFER];

    char *resolvedPtr;

    pid_t pid;

    int interval;

    int checkResult;
    int daemonResult;


    memset(dirPath, 0, MAX_BUFFER);
    memset(pwdPath, 0, MAX_BUFFER);
    memset(resolvedPath, 0, MAX_BUFFER);

    strncpy(dirPath, argDirPath, MAX_BUFFER);
    
    resolvedPtr = realpath(dirPath, resolvedPath);
    // if(resolvedPtr == NULL)
    // {
    //     return false;
    // }

    // 모니터링 디렉토리 검사
    checkResult = Check_Directory(dirPath);
    if(checkResult == false)
    {
        Help();

        return false;
    }

    getcwd(pwdPath, MAX_BUFFER);

    // monitor_list.txt 파일 경로 생성
    Make_Monitor_File_Path(pwdPath, monitorFilePath);

    // monitor_list.txt 파일 검사
    checkResult = Check_File(monitorFilePath);
    if(checkResult == true)
    {
        // 모니터링 리스트 파일이 존재할 경우에만 모니터링 대상 디렉토리 중복 여부 검사 수행
        // 파일이 존재하지 않으면 디몬 프로세스 생성 후 파일 생성됨

        checkResult = Check_Overlap_Monitor(resolvedPath, monitorFilePath);
        if(checkResult == false)
        {
            Help();

            return false;
        }
    }

    // 옵션 검사
    checkResult = Check_Option(prompt, &interval);
    if(checkResult == false)
    {
        Help();

        return false;
    }

    pid = getpid();

    // 모니터링 디몬 프로세스 생성
    daemonResult = Daemon(pwdPath, resolvedPath, interval);
    if(daemonResult == false)
    {
        fprintf(stderr, "fail to monitoring\n");

        return false;
    }

    return true;
}


int Check_Option(char *prompt, int *interval)
{
    char buf[MAX_BUFFER];
    char temp[MAX_BUFFER];

    char *strPtr;


    memset(buf, 0, MAX_BUFFER);

    strcpy(buf, prompt);

    for(int i = 0; i < 3; i++)
    {
        if(i == 0)
            strPtr = strtok(buf, " ");
        else
            strPtr = strtok(NULL, " ");
    }

    if(strPtr == NULL)
    {
        // -t 옵션이 없는 경우에는 모니터링 간격을 1초로 기본 설정

        *interval = 1;
        
        return true;
    }

    if(strcmp(strPtr, "-t") != 0)
    {
        return false;
    }

    strPtr = strtok(NULL, " ");
    if(strPtr == NULL)
    {
        return false;
    }

    memset(temp, 0, MAX_BUFFER);

    strcpy(temp, strPtr);

    for(int i = 0; i < strlen(temp); i++)
    {
        if(temp[i] < '0' || temp[i] > '9')
        {
            return false;
        }
    }

    *interval = atoi(temp);

    strPtr = strtok(NULL, " ");
    if(strPtr != NULL)
    {
        return false;
    }

    return true;
}


int Check_Overlap_Monitor(char *dirPath, char *monitorFilePath)
{
    char buf[MAX_BUFFER];

    FILE *fp;

    int isNotOverlap;

    char *readResult;
    char *strPtr;


    fp = fopen(monitorFilePath, "r");
    if(fp == NULL)
    {
        return false;
    }

    isNotOverlap = true;
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
            isNotOverlap = false;

            break;
        }

        if(strstr(dirPath, buf) != NULL)
        {
            isNotOverlap = false;

            break;
        }

        if(strstr(buf, dirPath) != NULL)
        {
            isNotOverlap = false;

            break;
        }
    }

    if(isNotOverlap == false)
    {
        fclose(fp);

        return false;
    }

    fclose(fp);

    return true;
}