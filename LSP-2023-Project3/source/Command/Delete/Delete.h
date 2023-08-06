#ifndef __DELETE_H__
#define __DELETE_H__


#include <string.h>
#include <signal.h>

#include "../../Common/CommonDefine.h"
#include "../../Common/GlobalValue.h"
#include "../BaseCommand/BaseCommand.h"
#include "../Help/Help.h"
#include "../../Daemon/Daemon.h"


int Delete(char *argument);

int Check_Pid(pid_t pid, char *monitorFilePath);

int Kill_Deamon_Process(pid_t daemonPid);

int Remove_Monitor_List(pid_t pid, char *monitorFilePath);


#endif