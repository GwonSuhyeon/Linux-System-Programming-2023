#ifndef __COMMANDHEADER_H__
#define __COMMANDHEADER_H__


#include "Command_EnumValue.h"
#include "Command_Option.h"


#define COMMAND_NAME_SIZE   10


static char     CommandList[ E_CommandCnt ][ COMMAND_NAME_SIZE ]    = { "add", "remove", "recover", "ls", "vi", "vim", "help", "exit" };

// E_Remove_Option     Remove_Option       = E_Remove_Non;
// E_Recover_Option    Recover_Option      = E_Recover_Non;

E_Remove_Option     Remove_Option;
E_Recover_Option    Recover_Option;


#endif