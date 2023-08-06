

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>

#include "../Command/Command_EnumValue.h"
#include "../Command/Command_Header.h"
#include "../Common/CommonDefine.h"
#include "../Common/GlobalValue.h"
#include "../Hash/Hash_EnumValue.h"


char            SSU_BACKUP_PATH[ MAX_PATH_BUFFER_SIZE ];

E_Hash_Type     HashType    = E_Invalid_Hash;

bool            ExitState   = false;


bool            Get_HomeDirectory();
bool            Get_WorkDirectory();

bool            Make_Backup_Directory();

E_Command       Get_Command( char *prompt );
bool            Split_Command_Params( char *prompt, char ***params );

bool            Run_Command( E_Command command, char **params );
bool            Execute_Child_Process( pid_t pid, char *commandPath, char **params );