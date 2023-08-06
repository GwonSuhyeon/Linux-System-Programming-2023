

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#include "../Command_Option.h"
#include "../BaseCommand/BaseCommand.h"
#include "../../Common/CommonDefine.h"
#include "../../Common/GlobalValue.h"
#include "../../FileListManager/FileListManager.h"


#define     REMOVE_OPTION               "ac"
#define     MAX_REMOVE_OPTION_CNT       1
#define     MAX_COMMAND_PARAMS_CNT      MAX_REMOVE_OPTION_CNT + 1


bool        Empty_In_Path       = false;

int         SearchedRemoveCnt   = 0;
bool        IsFind              = false;

int         RemoveDirCnt        = 0;
int         RemoveFileCnt       = 0;


int         Get_Option( char *optionList, int argc, char *argv[] );
bool        Get_FileList( char *path );

bool        Check_File_Or_Directory( int argc, char *path, bool optionActivate );
bool        Check_File_Info( char *path, struct stat fileStat, bool optionActivate );

bool        Remove( char *path );
bool        Search_FileList( char *path );
E_Search    Recursive_Search( DirectoryNode *travelDirNode, char *backupDirPath, char *backupFilePath );
void        RM_Directory( DirectoryNode *d_Node, char *backupDirPath );
void        RM_File( FileNode *travelFileNode, char *backupDirPath, char *backupFilePath );

void        Print_Usage();