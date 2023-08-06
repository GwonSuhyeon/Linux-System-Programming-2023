

#include "Monitor.h"


void Monitor(char *pwdPath, char *resolvedPath, int interval)
{
    FileNode *FileList;

    char monitorFilePath[MAX_BUFFER];

    pid_t daemonPid;

    int checkResult;
    int addResult;
    int makeResult;
    int runResult;


    daemonPid = getpid();

    // monitor_list.txt 파일 경로 생성
    Make_Monitor_File_Path(pwdPath, monitorFilePath);

    // monitor_list.txt 파일 검사
    checkResult = Check_File(monitorFilePath);
    if(checkResult == false)
    {
        // 예외처리 로그 기록

        exit(1);
    }

    // monitor_list.txt 파일에 모니터링 디렉토리 주소와 디몬 프로세스 pid 기록
    addResult = Add_Monitor_File(monitorFilePath, resolvedPath, daemonPid);
    if(addResult == false)
    {
        // 예외처리 로그 기록

        exit(1);
    }

    FileList = malloc(sizeof(FileNode));
    if(FileList == NULL)
    {
        exit(1);
    }

    memset(FileList->path, 0, MAX_BUFFER);
    FileList->modifyTime = 0;
    FileList->nextNode = NULL;

    // 모니터링 디렉토리의 파일 리스트 생성
    makeResult = Make_FileList(FileList, resolvedPath);
    if(makeResult == false)
    {
        // 예외처리 로그 기록

        exit(1);
    }

    // 모니터링 시작
    runResult = Run_Monitoring(FileList, resolvedPath, interval);
    if(runResult == false)
    {
        exit(1);
    }

    return;
}


int Add_Monitor_File(char *monitorFilePath, char *resolvedPath, int daemonPid)
{
    char buf[50];

    int fd;

    off_t offset;

    int writeResult;


    fd = open(monitorFilePath, O_WRONLY);
    if(fd == -1)
    {
        return false;
    }

    offset = lseek(fd, 0, SEEK_END);
    if(offset == -1)
    {
        return false;
    }

    writeResult = write(fd, resolvedPath, strlen(resolvedPath));
    if(writeResult == -1)
    {
        close(fd);

        return false;
    }

    writeResult = write(fd, " ", 1);
    if(writeResult == -1)
    {
        close(fd);
        
        return false;
    }

    snprintf(buf, 50, "%d\n", daemonPid);

    writeResult = write(fd, buf, strlen(buf));
    if(writeResult == -1)
    {
        close(fd);
        
        return false;
    }

    close(fd);

    return true;
}


int Run_Monitoring(FileNode *FileList, char *monitoringPath, int interval)
{
    FileNode *CompareList;

    char logFilePath[MAX_BUFFER];
    char timeFormat[TIME_FORMAT_BUFFER_SIZE];

    int fd;

    off_t offset;

    int makeResult;
    int getResult;
    int compareResult;


    if(FileList == NULL)
    {
        return false;
    }

    if((monitoringPath == NULL) || (strcmp(monitoringPath, "") == 0))
    {
        return false;
    }

    if(interval < 0)
    {
        return false;
    }

    memset(logFilePath, 0, MAX_BUFFER);
    
    strncpy(logFilePath, monitoringPath, strlen(monitoringPath));
    strncat(logFilePath, "/", 2);
    strncat(logFilePath, MONITOR_LOG_FILE_NAME, strlen(MONITOR_LOG_FILE_NAME) + 1);

    fd = open(logFilePath, O_WRONLY | O_CREAT, 0777);
    if(fd == -1)
    {
        return false;
    }

    offset = lseek(fd, 0L, SEEK_END);
    if(offset == -1)
    {
        return false;
    }

    while(1)
    {
        memset(timeFormat, 0, TIME_FORMAT_BUFFER_SIZE);

        CompareList = malloc(sizeof(FileNode));
        if(CompareList == NULL)
        {
            return false;
        }

        memset(CompareList->path, 0, MAX_BUFFER);
        CompareList->modifyTime = 0;
        CompareList->nextNode = NULL;

        // 과거의 디렉토리 파일 리스트와 비교할 현재의 디렉토리 파일 리스트를 생성
        makeResult = Make_FileList(CompareList, monitoringPath);
        if(makeResult == false)
        {
            return false;
        }

        // 모니터링 시각 획득
        getResult = Get_Current_Time(timeFormat, 0L);
        if(getResult == false)
        {
            return false;
        }

        // 디렉토리 변경 사항 비교
        compareResult = Compare_Directory(FileList, CompareList, logFilePath, timeFormat, fd);
        if(compareResult == false)
        {
            return false;
        }

        if(FileList != NULL)
        {
            // 과거의 디렉토리 파일 리스트 삭제
            // 새롭게 생성한 디렉토리 파일 리스트로 변경
            
            Delete_FileList(FileList);

            FileList = NULL;

            FileList = CompareList;

            CompareList = NULL;
        }

        if(interval > 0)
        {
            sleep(interval);
        }
    }

    if(fd >= 0)
    {
        close(fd);
    }

    return true;
}


int Compare_Directory(FileNode *FileList, FileNode *CompareList, char *logFilePath, char *timeFormat, int fd)
{
    FileNode *oldNode;
    FileNode *newNode;
    FileNode *tempNode;

    char modifyTimeFormat[MAX_BUFFER];
    char buf[MAX_BUFFER];

    E_Monitor_State monitorState;

    int writeResult;
    int getResult;


    if(FileList == NULL)
    {
        return false;
    }

    if(CompareList == NULL)
    {
        return false;
    }

    oldNode = FileList;
    newNode = CompareList;

    while(oldNode != NULL)
    {
        memset(buf, 0, MAX_BUFFER);

        monitorState = E_Non;

        if(strcmp(oldNode->path, logFilePath) == 0)
        {
            oldNode = oldNode->nextNode;

            continue;
        }

        if(strcmp(newNode->path, logFilePath) == 0)
        {
            newNode = newNode->nextNode;

            continue;
        }

        if(strcmp(oldNode->path, newNode->path) == 0)
        {
            if(oldNode->modifyTime == newNode->modifyTime)
            {
                monitorState = E_Non;
            }
            else
            {
                monitorState = E_Modify;
            }
        }
        else
        {
            monitorState = E_Delete;

            // tempNode = newNode;
            tempNode = CompareList;

            while(tempNode != NULL)
            {
                if(strcmp(oldNode->path, tempNode->path) == 0)
                {
                    monitorState = E_Create;

                    break;
                }

                tempNode = tempNode->nextNode;
            }

            // newNode = tempNode;
        }

        if(monitorState == E_Create)
        {
            if(strcmp(newNode->path, logFilePath) == 0)
            {
                newNode = newNode->nextNode;

                continue;
            }

            snprintf(buf, TIME_FORMAT_BUFFER_SIZE + 2, "[%s]", timeFormat);
            
            strncat(buf, "[create]", 9);
            strncat(buf, "[", 2);
            strncat(buf, newNode->path, strlen(newNode->path));
            strncat(buf, "]\n", 3);
            
            writeResult = write(fd, buf, strlen(buf));
            if(writeResult == -1)
            {
                // 예외처리 로그 기록
            }

            newNode = newNode->nextNode;
        }
        else if(monitorState == E_Delete)
        {
            if(strcmp(oldNode->path, logFilePath) == 0)
            {
                oldNode = oldNode->nextNode;

                continue;
            }

            snprintf(buf, TIME_FORMAT_BUFFER_SIZE + 2, "[%s]", timeFormat);
            
            strncat(buf, "[remove]", 9);
            strncat(buf, "[", 2);
            strncat(buf, oldNode->path, strlen(oldNode->path));
            strncat(buf, "]\n", 3);
            
            writeResult = write(fd, buf, strlen(buf));
            if(writeResult == -1)
            {
                // 예외처리 로그 기록
            }

            oldNode = oldNode->nextNode;
        }
        else if(monitorState == E_Modify)
        {
            if(strcmp(newNode->path, logFilePath) == 0)
            {
                oldNode = oldNode->nextNode;
                newNode = newNode->nextNode;

                continue;
            }

            memset(modifyTimeFormat, 0, MAX_BUFFER);

            getResult = Get_Current_Time(modifyTimeFormat, newNode->modifyTime);
            if(getResult == false)
            {
                // 예외처리 로그 기록
            }
            else
            {
                snprintf(buf, TIME_FORMAT_BUFFER_SIZE + 2, "[%s]", timeFormat);

                strncat(buf, "[modify]", 9);
                strncat(buf, "[", 2);
                strncat(buf, newNode->path, strlen(newNode->path));
                strncat(buf, "]\n", 3);

                writeResult = write(fd, buf, strlen(buf));
                if(writeResult == -1)
                {
                    // 예외처리 로그 기록
                }

                oldNode = oldNode->nextNode;
                newNode = newNode->nextNode;
            }
        }
        else if(monitorState == E_Non)
        {
            oldNode = oldNode->nextNode;
            newNode = newNode->nextNode;
        }
    }

    return true;
}


int Get_Current_Time(char *timeFormat, time_t modifyTime)
{
    time_t currentTime;

    struct tm *timer;

    char year[5];
    char month[3];
    char day[3];
    char hour[3];
    char minute[3];
    char second[3];


    if(modifyTime == (long)0)
    {
        currentTime = time(NULL);
    }
    else
    {
        currentTime = modifyTime;
    }

    timer = localtime(&currentTime);

    sprintf(year, "%d", timer->tm_year + 1900);
    
    if((timer->tm_mon + 1) < 10)
    {
        month[0] = '0';
        sprintf(&month[1], "%d", (timer->tm_mon + 1));
    }
    else
    {
        sprintf(month, "%d", (timer->tm_mon + 1));
    }

    if(timer->tm_mday < 10)
    {
        day[0] = '0';
        sprintf(&day[1], "%d", timer->tm_mday);
    }
    else
    {
        sprintf(day, "%d", timer->tm_mday);
    }

    if(timer->tm_hour < 10)
    {
        hour[0] = '0';
        sprintf(&hour[1], "%d", timer->tm_hour);
    }
    else
    {
        sprintf(hour, "%d", timer->tm_hour);
    }

    if(timer->tm_min < 10)
    {
        minute[0] = '0';
        sprintf(&minute[1], "%d", timer->tm_min);
    }
    else
    {
        sprintf(minute, "%d", timer->tm_min);
    }

    if(timer->tm_sec < 10)
    {
        second[0] = '0';
        sprintf(&second[1], "%d", timer->tm_sec);
    }
    else
    {
        sprintf(second, "%d", timer->tm_sec);
    }

    sprintf(timeFormat, "%s-%s-%s %s:%s:%s", year, month, day, hour, minute, second);
    
    return true;
}


// int Test_Print_FileList(FileNode *FileList, char *path)
// {
//     FileNode *travelNode;

//     char buf[50];

//     int fd;

//     off_t offset;

//     int writeResult;


//     fd = open(path, O_WRONLY);
//     if(fd == -1)
//     {
//         return false;
//     }

//     offset = lseek(fd, 0, SEEK_END);
//     if(offset == -1)
//     {
//         return false;
//     }

//     travelNode = FileList;
//     while(travelNode != NULL)
//     {
//         writeResult = write(fd, travelNode->path, strlen(travelNode->path));
//         if(writeResult == -1)
//         {
//             close(fd);

//             return false;
//         }

//         writeResult = write(fd, "\n", strlen("\n"));
//         if(writeResult == -1)
//         {
//             close(fd);

//             return false;
//         }
        
//         travelNode = travelNode->nextNode;
//     }

//     return true;
// }