#ifndef __BASECOMMAND_H__
#define __BASECOMMAND_H__


#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

#include "../../Common/CommonDefine.h"
#include "../../Common/GlobalValue.h"


int Check_Directory(char *dirPath);
int Check_File(char *filePath);
int Check_Monitor_List(char *dirPath, char *monitorFilePath);

int Make_File(char *filePath);
void Make_Monitor_File_Path(char *pwdPath, char *monitorFilePath);


#endif