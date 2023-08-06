#ifndef __MONITOR_H__
#define __MONITOR_H__


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "../Common/CommonDefine.h"
#include "../Common/GlobalValue.h"
#include "../Command/BaseCommand/BaseCommand.h"
#include "../FileListManager/FileListManager.h"
#include "../FileListManager/FileList.h"
#include "Monitor_Enum.h"


void Monitor(char *pwdPath, char *resolvedPath, int interval);

int Add_Monitor_File(char *monitorFilePath, char *resolvedPath, int daemonpid);

int Run_Monitoring(FileNode *FileList, char *monitoringPath, int interval);
int Compare_Directory(FileNode *FileList, FileNode *CompareList, char *logFilePath, char *timeFormat, int fd);

int Get_Current_Time(char *timeFormat, time_t modifyTime);

// int Test_Print_FileList(FileNode *FileList, char *path);


#endif