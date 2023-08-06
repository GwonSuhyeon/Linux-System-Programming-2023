#ifndef __MD5_H__
#define __MD5_H__


#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <openssl/md5.h>

#include "../Hash_EnumValue.h"
#include "../Hash_Header.h"


E_MD5_State   Hash_Compare_MD5( char *originFileName, char *compareFileName );


#endif