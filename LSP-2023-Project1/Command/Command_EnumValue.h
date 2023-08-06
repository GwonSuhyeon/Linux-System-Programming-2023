#ifndef __COMMAND_ENUMVALUE_H__
#define __COMMAND_ENUMVALUE_H__


typedef enum E_Command
{
    E_invalid       = -1,

    E_add           = 0,
    E_remove        = 1,
    E_recover       = 2,
    E_ls            = 3,
    E_vi            = 4,
    E_vim           = 5,
    E_help          = 6,
    E_exit          = 7,

    E_CommandCnt    = 8

} E_Command;


#endif