#ifndef __SHA1_H__
#define __SHA1_H__


#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <openssl/sha.h>

#include "../Hash_EnumValue.h"
#include "../Hash_Header.h"


E_SHA1_State  Hash_Compare_SHA1( char *originFileName, char *compareFileName );


#endif