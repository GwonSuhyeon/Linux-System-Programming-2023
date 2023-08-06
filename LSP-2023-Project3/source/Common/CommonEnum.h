#ifndef __COMMONENUM_H__
#define __COMMONENUM_H__


typedef enum E_Command_List
{
    E_Invalid_Command = -1,

    E_add = 0,
    E_delete = 1,
    E_tree = 2,
    E_help = 3,
    E_exit = 4,

    E_Command_Cnt = 5

} E_Command_List;


#endif