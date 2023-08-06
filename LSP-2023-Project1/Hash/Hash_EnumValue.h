#ifndef __HASH_ENUMVALUE_H__
#define __HASH_ENUMVALUE_H__


typedef enum E_Hash_Type
{
    E_Invalid_Hash  = 0,
    E_MD5           = 1,
    E_SHA1          = 2

} E_Hash_Type;

typedef enum E_MD5_State
{
    E_MD5_Invalid       = 0,
    E_MD5_Same          = 1,
    E_MD5_Not_Same      = 2

} E_MD5_State;

typedef enum E_SHA1_State
{
    E_SHA1_Invalid      = 0,
    E_SHA1_Same         = 1,
    E_SHA1_Not_Same     = 2

} E_SHA1_State;


#endif