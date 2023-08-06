

#include "Daemon.h"


int Daemon(char *pwdPath, char *resolvedPath, int interval)
{
    pid_t pid;

    int status;

    int setResult;


    // 부모 프로세스에서 자식 프로세스 생성
    pid = fork();
    if(pid < 0)
    {
        fprintf(stderr, "fail to make child process\n");

        exit(1);
    }
    else if(pid == 0)
    {
        // 자식 프로세스인 경우

        // 디몬 프로세스 생성 시작
        Make_Daemon();

        // 생성된 디몬 프로세스의 시그널 등록
        setResult = Set_Signal();
        if(setResult == false)
        {
            exit(1);
        }

        // 디몬 프로세스에서 모니터링 시작
        Monitor(pwdPath, resolvedPath, interval);

        // while(1);
    }
    else if(pid != 0)
    {
        // 부모 프로세스인 경우

        waitpid(pid, &status, 0);

        if(status == 0)
        {
            // 디몬 프로세스 생성 성공

            printf("monitoring started (%s)\n", resolvedPath);
            
            return true;
        }
        else if(status == 1)
        {
            // 디몬 프로세스 생성 실패

            return false;
        }
    }

    return true;
}


void Make_Daemon()
{
    pid_t pid;

    int max_fd;
    int fd;


    // 자식 프로세스에서 디몬 프로세스 생성
    pid = fork();
    if(pid < 0)
    {
        exit(1);
    }
    else if(pid != 0)
    {
        // 디몬 프로세스를 생성시킨 부모 프로세스인 경우
        exit(0);
    }

    // 디몬 프로세스인 경우
    // 디몬 프로세스 생성 규칙에 따라 설정

    setsid();

    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);

    max_fd = getdtablesize();

    for(int i = 0; i < max_fd; i++)
    {
        close(i);
    }

    umask(0);

    chdir("/");

    fd = open("/dev/null", O_RDWR);

    dup(0);
    dup(0);

    return;
}


int Set_Signal()
{
    if(signal(SIGUSR1, Handler_Delete_Signal) == SIG_ERR)
    {
        fprintf(stderr, "fail to set signal\n");

        return false;
    }

    return true;
}


void Handler_Delete_Signal()
{
    int max_fd;


    max_fd = getdtablesize();

    for(int i = 0; i < max_fd; i++)
    {
        close(i);
    }

    exit(0);
}