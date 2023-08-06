

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include "../Command_Option.h"
#include "../BaseCommand/BaseCommand.h"
#include "../../Common/CommonDefine.h"
#include "../../Common/GlobalValue.h"
#include "../../FileListManager/FileListManager.h"
#include "../../Hash/Hash_EnumValue.h"
#include "../../Hash/md5/md5.h"
#include "../../Hash/sha1/sha1.h"


#define     RECOVER_OPTION              "dn"
#define     MAX_RECOVER_OPTION_CNT      2
#define     MAX_COMMAND_PARAMS_CNT      MAX_RECOVER_OPTION_CNT + 2


bool            Empty_In_Path       = false;
bool            Input_Type_Is_File  = false;

int             SearchedRecoverCnt  = 0;
bool            IsFind              = false;
bool            RecoverStop         = false;

bool            InvalidRecoverPath  = false;


int         Get_Option( char *optionList, int argc, char *argv[] );
bool        Get_FileList( char *path );

bool        Check_File_Or_Directory( int argc, char *path, bool isRecoverPath, bool optionActivate );
bool        Check_File_Info( char *path, struct stat fileStat, bool optionActivate );
bool        Check_NewPath_Info( char *path );

bool        Recover( char *path, char *newRecoverPath );
bool        Search_FileList( char *path, char *newRecoverPath );
E_Search    Recursive_Search( DirectoryNode *travelDirNode, char *backupDirPath, char *backupFilePath, char *newRecoverPath );
bool        Directory_Recover( DirectoryNode *d_Node, char *backupDirPath, char *newRecoverPath );
bool        File_Recover( char *backupDirpath, char *backupFilePath, char *newRecoverPath );

void        Print_Usage();