#ifndef __BASECOMMAND_H__
#define __BASECOMMAND_H__


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "BaseCommand_EnumValue.h"
#include "../../Common/CommonDefine.h"
#include "../../Common/GlobalValue.h"
#include "../Command_Option.h"
#include "../Command_Header.h"
#include "../../FileListManager/FileListManager.h"
#include "../../Hash/Hash_EnumValue.h"
#include "../../Hash/md5/md5.h"
#include "../../Hash/sha1/sha1.h"


E_Hash_Type     HashType;


bool            Get_HomeDirectory();
bool            Get_WorkDirectory();
bool            Get_BackupDirectory();

bool            Check_Command_Params( char **params );
bool            Check_Path_Info( char *path, E_CommandType commandType );
E_MD5_State     Check_MD5( char *originFilePath, char *compareFilePath );
E_SHA1_State    Check_SHA1( char *originFilePath, char *compareFilePath );

bool            Custom_RealPath( char *path, char *resolvedPath );

int             Select_File( E_CommandType commandType );

bool            Convert_FileSize_Format( char *fileSize, char *filePath );

bool            Delete_Memory( FileListNode *FileList, FileListNode *BackupList, char *pathCopy, char *splitPath, char *splitFile );


#endif