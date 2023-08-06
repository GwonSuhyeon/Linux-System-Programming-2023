#ifndef __FILELISTMANAGER_H__
#define __FILELISTMANAGER_H__


#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include "FileList.h"
#include "FileList_EnumValue.h"


// #define     FILE_LIST_PRINT


extern char     *HomeDirPath;


bool    FileListManager( char *path, E_ListType listType );

bool    Generate_FileList( char *path, FileListNode *list, E_ListType listType );
void    Delete_FileList( DirectoryNode *dirNode );

bool    Scan_Directory( char *path, FileListNode *list, E_ListType listType );
bool    Recursive_Scan( DirectoryNode *dirNode, char *path, E_ListType listType );
int     Directory_Filter();

void    Print_FileList( DirectoryNode *dirNode, E_Print_Type printType );


#endif