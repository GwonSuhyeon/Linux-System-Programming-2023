

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <time.h>

#include "../BaseCommand/BaseCommand.h"
#include "../../Common/CommonDefine.h"
#include "../../Common/GlobalValue.h"
#include "../../FileListManager/FileListManager.h"
#include "../../Hash/Hash_EnumValue.h"
#include "../../Hash/md5/md5.h"
#include "../../Hash/sha1/sha1.h"


#define     ADD_OPTION                  "d"
#define     MAX_ADD_OPTION_CNT          1
#define     MAX_COMMAND_PARAMS_CNT      MAX_ADD_OPTION_CNT + 1


int         Get_Option( char *optionList, int argc, char *argv[] );
bool        Get_FileList( char *path );
void        Get_BackupTime( char backupTime[ BACKUP_TIME_FORMAT_SIZE ] );

bool        Check_File_Or_Directory( char *path, bool optionActivate );
bool        Check_File_Info( char *path, struct stat fileStat, bool optionActivate );

bool        Backup();
void        Recursive_Search( DirectoryNode *dirNode);
bool        File_Backup( char filePath[ MAX_PATH_BUFFER_SIZE ], char fileBackupPath[ MAX_PATH_BUFFER_SIZE ] );
E_Search    Find_BackupDirNode( DirectoryNode *dirNode, char *backupDirPath, char *originFilePath );
bool        Find_BackupFileNode( DirectoryNode *dirNode, char *originFilePath );

void        Print_Usage();