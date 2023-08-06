#ifndef __TREE_H__
#define __TREE_H__


#include <stdlib.h>
#include <dirent.h>

#include "../../Common/CommonDefine.h"
#include "../../Common/GlobalValue.h"
#include "../BaseCommand/BaseCommand.h"
#include "../Help/Help.h"
#include "TreeList.h"


int Tree(char *argument);

int Show_Tree(char *dirPath, int depth);

int Directory_Filter(const struct dirent *item);

void Delete_Directory_Entry(struct dirent **dirEntry, int entryCnt);
void Delete_TreeList(TreeList *list);


#endif