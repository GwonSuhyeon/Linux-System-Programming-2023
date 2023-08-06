#ifndef __DAEMON_H__
#define __DAEMON_H__


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include "../Common/CommonDefine.h"
#include "../Common/GlobalValue.h"
#include "../Monitor/Monitor.h"


int Daemon(char *pwdPath, char *resolvedPath, int interval);

void Make_Daemon();

int Set_Signal();
void Handler_Delete_Signal();


#endif