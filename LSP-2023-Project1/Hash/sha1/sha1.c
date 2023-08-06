

#include "sha1.h"


E_SHA1_State Hash_Compare_SHA1( char *originFileName, char *compareFileName )
{
    FILE            *originFile     = NULL;
    FILE            *compareFile    = NULL;

    SHA_CTX         originContext;
    SHA_CTX         compareContext;

    unsigned char   originHashData[ READ_CNT ];
    unsigned char   compareHashData[ READ_CNT ];

    unsigned char   originSHA1[ SHA_DIGEST_LENGTH ];
    unsigned char   compareSHA1[ SHA_DIGEST_LENGTH ];

    int             readSize        = 0;

    int             initResult      = 0;
    int             updateResult    = 0;
    int             finalResult     = 0;


    if( originFileName == NULL )
    {
        return E_SHA1_Invalid;
    }

    if( compareFileName == NULL )
    {
        return E_SHA1_Invalid;
    }

    originFile = fopen( originFileName, "rb" );
    if( originFile == NULL )
    {
        printf( "File open fail \"%s\"\n", originFileName );

        return E_SHA1_Invalid;
    }

    compareFile = fopen( compareFileName, "rb" );
    if( compareFile == NULL )
    {
        printf( "File open fail \"%s\"\n", compareFileName );

        return E_SHA1_Invalid;
    }

    memset( originHashData, 0, READ_CNT );
    memset( compareHashData, 0, READ_CNT );
    memset( originSHA1, 0, SHA_DIGEST_LENGTH );
    memset( compareSHA1, 0, SHA_DIGEST_LENGTH );

    initResult = SHA1_Init( &originContext );
    if( initResult == 0 )
    {
        printf( "SHA1 init fail\n" );

        return E_SHA1_Invalid;
    }

    while( true )
    {
        readSize = fread( originHashData, READ_BYTE, READ_CNT, originFile );
        if( readSize == 0 )
        {
            break;
        }

        updateResult = SHA1_Update( &originContext, originHashData, readSize );
        if( updateResult == 0 )
        {
            printf( "SHA1 update fail\n" );

            return E_SHA1_Invalid;
        }
    }

    finalResult = SHA1_Final( originSHA1, &originContext );
    if( finalResult == 0 )
    {
        printf( "SHA1 final fail\n" );

        return E_SHA1_Invalid;
    }

    initResult = SHA1_Init( &compareContext );
    if( initResult == 0 )
    {
        printf( "SHA1 init fail\n" );

        return E_SHA1_Invalid;
    }

    while( true )
    {
        readSize = fread( compareHashData, READ_BYTE, READ_CNT, compareFile );
        if( readSize == 0 )
        {
            break;
        }

        updateResult = SHA1_Update( &compareContext, compareHashData, readSize );
        if( updateResult == 0 )
        {
            printf( "SHA1 update fail\n" );

            return E_SHA1_Invalid;
        }
    }

    finalResult = SHA1_Final( compareSHA1, &compareContext );
    if( finalResult == 0 )
    {
        printf( "SHA1 final fail\n" );

        return E_SHA1_Invalid;
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

    for( int i = 0; i < SHA_DIGEST_LENGTH; i++ )
    {
        if( originSHA1[ i ] != compareSHA1[ i ] )
        {
            return E_SHA1_Not_Same;
        }
    }


    return E_SHA1_Same;
}