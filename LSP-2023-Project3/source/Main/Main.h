

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../Common/CommonDefine.h"
#include "../Common/CommonEnum.h"
#include "../Common/GlobalValue.h"
#include "../Command/Add/Add.h"
#include "../Command/Delete/Delete.h"
#include "../Command/Tree/Tree.h"
#include "../Command/Help/Help.h"


char arguments[MAX_ARG_CNT][MAX_BUFFER];


// int Get_Arguments(char *prompt);
int Get_Argument(char *prompt, char *argument);
int Get_Command(char *prompt);

void Run_Command(E_Command_List commandType, char *argument, char *prompt);