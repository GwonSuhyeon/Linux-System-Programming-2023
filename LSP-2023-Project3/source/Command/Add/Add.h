#ifndef __ADD_H__
#define __ADD_H__


#include <string.h>

#include "../../Common/CommonDefine.h"
#include "../../Common/GlobalValue.h"
#include "../BaseCommand/BaseCommand.h"
#include "../Help/Help.h"
#include "../../Daemon/Daemon.h"


int Add(char *argDirPath, char *prompt);

int Check_Option(char *prompt, int *interval);
int Check_Overlap_Monitor(char *dirPath, char *monitorFilePath);


#endif