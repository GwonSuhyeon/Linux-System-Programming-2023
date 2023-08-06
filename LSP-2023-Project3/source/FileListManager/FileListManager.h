#ifndef __FILELISTMANAGER_H__
#define __FILELISTMANAGER_H__


#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../Common/CommonDefine.h"
#include "../Common/GlobalValue.h"
#include "FileList.h"


int Make_FileList(FileNode *FileList, char *monitoringPath);
FileNode* Make_FileNode();

void Delete_FileList(FileNode *List);

int Scan_Directory(FileNode *fileNode, char *path);


#endif