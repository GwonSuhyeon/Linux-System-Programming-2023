

#include "BaseCommand.h"


bool Get_HomeDirectory()
{
    char    resolvedPath[ MAX_PATH_BUFFER_SIZE ]    = { '\0' };
    char    homePath[ MAX_PATH_BUFFER_SIZE ]        = { '\0' };

    char    *realPathResult                         = NULL;

    char    *splitPtr                               = NULL;


    // 현재 작업 디렉토리 경로 획득
    realPathResult = realpath( ".", resolvedPath );
    if( realPathResult == NULL )
    {
        printf( "Error : Invalid path\n" );

        return false;
    }
    realPathResult = NULL;
    
    // 현재 작업 디렉토리 경로로부터 홈 디렉토리 경로 추출
    splitPtr = strtok( resolvedPath, "/" );
    for( int i = 0; i < HOME_DIRECTORY_INDEX; i++ )
    {
        if( splitPtr == NULL )
        {
            break;
        }

        strcat( homePath, "/" );
        strcat( homePath, splitPtr );

        splitPtr = strtok( NULL, "/" );
    }

    HomeDirPath = malloc( sizeof( char ) * ( strlen( homePath ) + 1 ) );
    if( HomeDirPath == NULL )
    {
        return false;
    }

    memset( HomeDirPath, '\0', ( strlen( homePath ) + 1 ) );
    strcpy( HomeDirPath, homePath );

    splitPtr = NULL;


    return true;
}


bool Get_WorkDirectory()
{
    char    resolvedPath[ MAX_PATH_BUFFER_SIZE ]    = { '\0' };

    char    *realPathResult                         = NULL;


    // 현재 작업 디렉토리 경로 획득
    realPathResult = realpath( ".", resolvedPath );
    if( realPathResult == NULL )
    {
        printf( "Error : Invalid path\n" );

        return false;
    }

    Current_Work_Directory_Path = malloc( sizeof( char ) * ( strlen( resolvedPath ) + 1 ) );
    if( Current_Work_Directory_Path == NULL )
    {
        return false;
    }

    strcpy( Current_Work_Directory_Path, resolvedPath );


    return true;
}


bool Get_BackupDirectory()
{
    // 백업 디렉토리 경로 획득
    Backup_Directory_Path = malloc( sizeof( char ) * ( strlen( HomeDirPath ) + strlen( BACKUP_DIRECTORY_NAME ) + 1 ) );
    if( Backup_Directory_Path == NULL )
    {
        return false;
    }

    strcpy( Backup_Directory_Path, HomeDirPath );
    strcat( Backup_Directory_Path, BACKUP_DIRECTORY_NAME );


    return true;
}


bool Check_Command_Params( char **params )
{
    // 실행 인자 비었는지 검사
    if( strcmp( *( params + 0 ), "" ) == 0 )
    {
        return false;
    }


    return true;
}


bool Check_Path_Info( char *path, E_CommandType commandType )
{
    char    resolvedPath[ MAX_PATH_BUFFER_SIZE ]    = { '\0' };
    char    homePath[ MAX_PATH_BUFFER_SIZE ]        = { '\0' };
    char    pwdPath[ MAX_PATH_BUFFER_SIZE ]         = { '\0' };
    char    backupPath[ MAX_PATH_BUFFER_SIZE ]      = { '\0' };

    char    *homeText           = "/home/";

    char    *realPathResult     = NULL;

    char    *splitPtr           = NULL;


    // 홈 디렉토리, 백업 디렉토리, /home 경로를 인자로 입력했는지 검사

    if( commandType == E_Type_Add )
    {
        for( int i = 0; i < ( strlen( homeText ) - 1 ); i++ )
        {
            if( *( path + i ) != *( homeText + i ) )
            {
                printf( "\"%s\" can't be backuped\n", path );

                return false;
            }
        }

        for( int i = 0; i < strlen( path ); i++ )
        {
            if( i == strlen( HomeDirPath ) )
            {
                break;
            }

            if( *( path + i ) != *( HomeDirPath + i ) )
            {
                printf( "\"%s\" can't be backuped\n", path );
                    
                return false;
            }
        }
    }
    else if( commandType == E_Type_Remove )
    {
        if( Remove_Option == E_c )
        {
            return true;
        }
    }

    memset( resolvedPath, '\0', MAX_PATH_BUFFER_SIZE );
    realPathResult = realpath( path, resolvedPath );
    if( realPathResult == NULL )
    {
        printf( "Error : Invalid path\n" );

        return false;
    }
    realPathResult = NULL;

    if( strcmp( HomeDirPath, resolvedPath ) == 0 )
    {
        printf( "\"%s\" can't be backuped\n", path );

        return false;
    }

    splitPtr = strtok( resolvedPath, "/" );
    for( int i = 0; i < 2; i++ )
    {
        if( splitPtr == NULL )
        {
            break;
        }

        strcat( pwdPath, "/" );
        strcat( pwdPath, splitPtr );

        splitPtr = strtok( NULL, "/" );
    }
    splitPtr = NULL;

    if( strcmp( HomeDirPath, pwdPath ) != 0 )
    {
        printf( "\"%s\" can't be backuped\n", path );

        return false;
    }

    memset( resolvedPath, '\0', MAX_PATH_BUFFER_SIZE );
    realPathResult = realpath( path, resolvedPath );
    if( realPathResult == NULL )
    {
        printf( "Error : Invalid path\n" );

        return false;
    }

    memset( pwdPath, '\0', MAX_PATH_BUFFER_SIZE );
    splitPtr = strtok( resolvedPath, "/" );
    for( int i = 0; i < 3; i++ )
    {
        if( splitPtr == NULL )
        {
            break;
        }

        strcat( pwdPath, "/" );
        strcat( pwdPath, splitPtr );

        splitPtr = strtok( NULL, "/" );
    }

    strcpy( backupPath, HomeDirPath );
    strcat( backupPath, "/backup" );

    if( strcmp( backupPath, pwdPath ) == 0 )
    {
        printf( "\"%s\" can't be backuped\n", path );

        return false;
    }


    return true;
}


E_MD5_State Check_MD5( char *originFilePath, char *compareFilePath )
{
    struct stat     fileStat;

    int             statResult  = -1;
    E_MD5_State     md5Result   = E_MD5_Invalid;


    if( originFilePath == NULL )
    {
        return E_MD5_Invalid;
    }

    if( compareFilePath == NULL )
    {
        return E_MD5_Invalid;
    }

    statResult = stat( compareFilePath, &fileStat );
    if( statResult == -1 )
    {
        return E_MD5_Not_Same;
    }

    // md5 해시 함수로 해시 값 비교
    md5Result = Hash_Compare_MD5( originFilePath, compareFilePath );
    if( md5Result == E_MD5_Invalid )
    {
        return E_MD5_Invalid;
    }

    if( md5Result == E_MD5_Same )
    {
        return E_MD5_Same;
    }
    else if( md5Result == E_MD5_Not_Same )
    {
        return E_MD5_Not_Same;
    }
    

    return E_MD5_Invalid;
}


E_SHA1_State Check_SHA1( char *originFilePath, char *compareFilePath )
{
    struct stat     fileStat;

    int             statResult  = -1;
    E_SHA1_State    sha1Result   = E_SHA1_Invalid;


    if( originFilePath == NULL )
    {
        return E_SHA1_Invalid;
    }

    if( compareFilePath == NULL )
    {
        return E_SHA1_Invalid;
    }

    statResult = stat( compareFilePath, &fileStat );
    if( statResult == -1 )
    {
        return E_SHA1_Not_Same;
    }

    // sha1 해시 함수로 해시 값 비교
    sha1Result = Hash_Compare_SHA1( originFilePath, compareFilePath );
    if( sha1Result == E_SHA1_Invalid )
    {
        return E_SHA1_Invalid;
    }

    if( sha1Result == E_SHA1_Same )
    {
        return E_SHA1_Same;
    }
    else if( sha1Result == E_SHA1_Not_Same )
    {
        return E_SHA1_Not_Same;
    }
    

    return E_SHA1_Invalid;
}


bool Custom_RealPath( char *path, char *resolvedPath )
{
    int     dot             = 0;
    int     resolvedIndex   = 0;


    // .과 .. 디렉토리가 존재하지 않는 디렉토리 혹은 파일명과 결합된 경로를 절대경로로 변환

    if( path == NULL )
    {
        return false;
    }

    if( resolvedPath == NULL )
    {
        return false;
    }

    for( int i = 0; i < strlen( path ); i++ )
    {
        if( *( path + i ) == '.' )
        {
            dot++;
        }
        else
        {
            if( dot == 1 )
            {
                if( *( path + i ) != '/' )
                {
                    *( resolvedPath + resolvedIndex ) = '.';
                    resolvedIndex++;

                    *( resolvedPath + resolvedIndex ) = *( path + i );
                    resolvedIndex++;
                }

                dot = 0;
            }
            else if( dot == 2 )
            {
                *( resolvedPath + ( resolvedIndex - 1 ) ) = '\0';

                for( int k = ( resolvedIndex - 2 ); k >= 0; k-- )
                {
                    if( *( resolvedPath + k ) == '/' )
                    {
                        resolvedIndex = k + 1;

                        break;
                    }
                    
                    *( resolvedPath + k ) = '\0';
                }
            }
            else
            {
                *( resolvedPath + resolvedIndex ) = *( path + i );

                resolvedIndex++;
            }

            dot = 0;
        }
    }


    return true;
}


int Select_File( E_CommandType commandType )
{
    int     selectNumber    = -1;


    // 다수의 백업 버전이 존재할 경우 파일 선택

    if( commandType == E_Type_Remove )
    {
        printf( "Choose file to remove\n" );
    }
    else if( commandType == E_Type_Recover )
    {
        printf( "Choose file to recover\n" );
    }
    else if( commandType == E_Type_Add )
    {
        return 0;
    }
    
    printf( ">> " );

    scanf( "%d", &selectNumber );


    return selectNumber;
}


bool Convert_FileSize_Format( char *fileSize, char *filePath )
{
    struct stat     fileStat;

    char            tempBuffer[ MAX_FILE_SIZE_BUFFER ]  = { '\0' };

    int             statResult  = -1;

    int             sizeIndex   = 0;


    // 바이트 단위인 파일 크기를 세자리수마다 ,를 출력하기 위한 출력 형식 변환
    
    statResult = stat( filePath, &fileStat );
    if( statResult == -1 )
    {
        return false;
    }

    sprintf( tempBuffer, "%ld", fileStat.st_size );

    for( int index = 0; index < strlen( tempBuffer ); index++ )
    {
        if( index == 0 )
        {
            fileSize[ sizeIndex ] = tempBuffer[ index ];

            sizeIndex++;
        }
        else if( ( index % 3 ) == ( strlen( tempBuffer ) % 3 ) )
        {
            fileSize[ sizeIndex ]       = ',';
            fileSize[ sizeIndex + 1 ]   = tempBuffer[ index ];

            sizeIndex += 2;
        }
        else
        {
            fileSize[ sizeIndex ] = tempBuffer[ index ];

            sizeIndex++;
        }
    }


    return true;
}


bool Delete_Memory( FileListNode *FileList, FileListNode *BackupList, char *pathCopy, char *splitPath, char *splitFile )
{
    if( pathCopy != NULL )
    {
        free( pathCopy );
    }

    if( splitPath != NULL )
    {
        free( splitPath );
    }

    if( splitFile != NULL )
    {
        free( splitFile );
    }

    if( FileList != NULL )
    {
        if( FileList->currentDirNode != NULL )
        {
            Delete_FileList( FileList->currentDirNode );
        }
    }
    
    if( BackupList != NULL )
    {
        if( BackupList->currentDirNode != NULL )
        {
            Delete_FileList( BackupList->currentDirNode );
        }
    }


    return true;
}