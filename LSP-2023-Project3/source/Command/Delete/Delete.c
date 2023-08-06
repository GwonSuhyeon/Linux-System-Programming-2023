

#include "Delete.h"


int Delete(char *argument)
{
    char pwdPath[MAX_BUFFER];
    char monitorFilePath[MAX_BUFFER];

    pid_t daemonPid;

    int checkResult;
    int killResult;
    int removeResult;


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

    for(int i = 0; i < strlen(argument); i++)
    {
        if(*(argument + i) < '0' || *(argument + i) > '9')
        {
            Help();

            return false;
        }
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

    daemonPid = atoi(argument);

    // 종료시킬 디몬 프로세스 pid 검사
    checkResult = Check_Pid(daemonPid, monitorFilePath);
    if(checkResult == false)
    {
        fprintf(stderr, "Not found [PID %d]\n", daemonPid);

        return false;
    }

    // 디몬 프로세스 종료
    killResult = Kill_Deamon_Process(daemonPid);
    if(killResult == false)
    {
        return false;
    }

    // monitor_list.txt 수정
    removeResult = Remove_Monitor_List(daemonPid, monitorFilePath);
    if(removeResult == false)
    {
        return false;
    }

    return true;
}


int Check_Pid(pid_t pid, char *monitorFilePath)
{
    char buf[MAX_BUFFER];

    FILE *fp;

    int isPidFind;

    char *readResult;
    char *strPtr;


    fp = fopen(monitorFilePath, "r");
    if(fp == NULL)
    {
        return false;
    }

    isPidFind = false;
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

        if(pid == atoi(strPtr + 1))
        {
            isPidFind = true;

            break;
        }
    }

    if(isPidFind == false)
    {
        fclose(fp);

        return false;
    }

    fclose(fp);

    return true;
}


int Kill_Deamon_Process(pid_t daemonPid)
{
    int killResult;


    killResult = kill(daemonPid, SIGUSR1);
    if(killResult == -1)
    {
        fprintf(stderr, "fail to kill [PID %d]\n", daemonPid);

        return false;
    }

    return true;
}


int Remove_Monitor_List(pid_t pid, char *monitorFilePath)
{
    char newFilePath[MAX_BUFFER];
    char monitorDirPath[MAX_BUFFER];
    char buf[MAX_BUFFER];

    char *tempFileName = "monitor_temp.txt";

    FILE *old_fp;
    int new_fd;

    char *readResult;
    char *strPtr;

    int writeResult;
    int removeResult;
    int renameResult;


    memset(newFilePath, 0, MAX_BUFFER);
    memset(monitorDirPath, 0, MAX_BUFFER);
    
    strncpy(newFilePath, monitorFilePath, strlen(monitorFilePath));

    strPtr = strrchr(newFilePath, '/');
    if(strPtr == NULL)
    {
        return false;
    }

    *(strPtr + 1) = '\0';

    strncat(newFilePath, tempFileName, strlen(tempFileName));

    old_fp = fopen(monitorFilePath, "r");
    if(old_fp == NULL)
    {
        return false;
    }

    new_fd = open(newFilePath, O_WRONLY | O_CREAT, 0777);
    if(new_fd == -1)
    {
        return false;
    }

    while(1)
    {
        memset(buf, 0, MAX_BUFFER);

        readResult = fgets(buf, MAX_BUFFER, old_fp);
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

        if(pid != atoi(strPtr + 1))
        {
            buf[strlen(buf)] = '\n';

            writeResult = write(new_fd, buf, strlen(buf));
            if(writeResult == -1)
            {
                if(old_fp != NULL)
                    fclose(old_fp);
                
                if(new_fd >= 0)
                    close(new_fd);
                
                return false;
            }
        }
        else
        {
            *strPtr = '\0';

            strncpy(monitorDirPath, buf, strlen(buf));
        }
    }

    if(old_fp != NULL)
    {
        fclose(old_fp);
    }

    if(new_fd >= 0)
    {
        close(new_fd);
    }

    unlink(monitorFilePath);

    rename(newFilePath, monitorFilePath);

    printf("monitoring ended (%s)\n", monitorDirPath);

    return true;
}