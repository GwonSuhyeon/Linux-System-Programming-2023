

#include "md5.h"


E_MD5_State Hash_Compare_MD5( char *originFileName, char *compareFileName )
{
    FILE            *originFile     = NULL;
    FILE            *compareFile    = NULL;

    MD5_CTX         originContext;
    MD5_CTX         compareContext;

    unsigned char   originHashData[ READ_CNT ];
    unsigned char   compareHashData[ READ_CNT ];

    unsigned char   originMD5[ MD5_DIGEST_LENGTH ];
    unsigned char   compareMD5[ MD5_DIGEST_LENGTH ];

    int             readSize        = 0;

    int             initResult      = 0;
    int             updateResult    = 0;
    int             finalResult     = 0;


    if( originFileName == NULL )
    {
        return E_MD5_Invalid;
    }

    if( compareFileName == NULL )
    {
        return E_MD5_Invalid;
    }

    originFile = fopen( originFileName, "rb" );
    if( originFile == NULL )
    {
        printf( "File open fail \"%s\"\n", originFileName );

        return E_MD5_Invalid;
    }

    compareFile = fopen( compareFileName, "rb" );
    if( compareFile == NULL )
    {
        printf( "File open fail \"%s\"\n", compareFileName );

        return E_MD5_Invalid;
    }

    memset( originHashData, 0, READ_CNT );
    memset( compareHashData, 0, READ_CNT );
    memset( originMD5, 0, MD5_DIGEST_LENGTH );
    memset( compareMD5, 0, MD5_DIGEST_LENGTH );

    initResult = MD5_Init( &originContext );
    if( initResult == 0 )
    {
        printf( "MD5 init fail\n" );

        return E_MD5_Invalid;
    }

    while( true )
    {
        readSize = fread( originHashData, READ_BYTE, READ_CNT, originFile );
        if( readSize == 0 )
        {
            break;
        }

        updateResult = MD5_Update( &originContext, originHashData, readSize );
        if( updateResult == 0 )
        {
            printf( "MD5 update fail\n" );

            return E_MD5_Invalid;
        }
    }

    finalResult = MD5_Final( originMD5, &originContext );
    if( finalResult == 0 )
    {
        printf( "MD5 final fail\n" );

        return E_MD5_Invalid;
    }

    initResult = MD5_Init( &compareContext );
    if( initResult == 0 )
    {
        printf( "MD5 init fail\n" );

        return E_MD5_Invalid;
    }

    while( true )
    {
        readSize = fread( compareHashData, READ_BYTE, READ_CNT, compareFile );
        if( readSize == 0 )
        {
            break;
        }

        updateResult = MD5_Update( &compareContext, compareHashData, readSize );
        if( updateResult == 0 )
        {
            printf( "MD5 update fail\n" );

            return E_MD5_Invalid;
        }
    }

    finalResult = MD5_Final( compareMD5, &compareContext );
    if( finalResult == 0 )
    {
        printf( "MD5 final fail\n" );

        return E_MD5_Invalid;
    }

    if( originFile != NULL )
    {
        fclose( originFile );

        originFile = NULL;
    }

    if( compareFile != NULL )
    {
        fclose( compareFile );

        compareFile = NULL;
    }

    for( int i = 0; i < MD5_DIGEST_LENGTH; i++ )
    {
        if( originMD5[ i ] != compareMD5[ i ] )
        {
            return E_MD5_Not_Same;
        }
    }


    return E_MD5_Same;
}