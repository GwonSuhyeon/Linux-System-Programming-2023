#ifndef __FILELIST_ENUMVALUE_H__
#define __FILELIST_ENUMVALUE_H__


typedef enum E_Print_Type
{
    E_ALL       = 0,
    E_Dir       = 1,
    E_File      = 2,
    E_Remove    = 3,
    E_Recover   = 4

} E_Print_Type;

typedef enum E_Search
{
    E_Invalid   = 0,
    E_Stop      = 1,
    E_Continue  = 2

} E_Search;

typedef enum E_ListType
{
    E_FileList          = 0,
    E_BackupList        = 1,
    
    E_RemoveFileList    = 2,
    E_RecoverFileList   = 3,
    E_AddFileList       = 4

} E_ListType;


#endif