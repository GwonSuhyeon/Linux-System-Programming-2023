

#include "Add.h"


extern FileListNode     *FileList;
extern FileListNode     *BackupList;

// extern E_Hash_Type      HashType;


int main( int argc, char *argv[] )
{
    char    path[ MAX_PATH_BUFFER_SIZE ]    = { '\0' };
    
    char    *backupFile         = NULL;
    
    char    *optionList         = NULL;
    char    *memsetResult       = NULL;

    bool    checkResult         = false;
    bool    checkFileOrDir      = false;
    bool    checkPath           = false;

    int     optionCnt           = 0;
    bool    optionActivate      = false;

    bool    fileListResult      = false;
    bool    backupResult        = false;
    bool    getDirectoryResult  = false;


    // 디렉토리 경로 획득

    getDirectoryResult = Get_HomeDirectory();
    if( getDirectoryResult == false )
    {
        printf( "Fail to get directory path\n" );

        return false;
    }

    getDirectoryResult = Get_WorkDirectory();
    if( getDirectoryResult == false )
    {
        printf( "Fail to get directory path\n" );

        return false;
    }

    getDirectoryResult = Get_BackupDirectory();
    if( getDirectoryResult == false )
    {
        printf( "Fail to get directory path\n" );

        return false;
    }

    // 실행 인자 검사
    checkResult = Check_Command_Params( argv + 2 );
    if( checkResult == false )
    {
        Print_Usage();

        return -1;
    }

    if( *( *( argv + 2 ) + 0 ) == '/' )
    {
        strcpy( path, *( argv + 2 ) );
    }
    else if( *( *( argv + 2 ) + 0 ) == '~' )
    {
        strcpy( path, HomeDirPath );
        strcat( path, ( *( argv + 2 ) + 1 ) );
    }
    else
    {
        strcpy( path, Current_Work_Directory_Path );
        strcat( path, "/" );
        strcat( path, *( argv + 2 ) );
    }

    // 올바른 경로인지 검사
    checkPath = Check_Path_Info( path, E_Type_Add );
    if( checkPath == false )
    {
        return 0;
    }

    optionList = malloc( sizeof( char ) * MAX_ADD_OPTION_CNT );
    if( optionList == NULL )
    {
        return -1;
    }

    memsetResult = NULL;
    memsetResult = memset( optionList, '\0', sizeof( char ) * MAX_ADD_OPTION_CNT );
    if( memsetResult == NULL )
    {
        return -1;
    }

    // 옵션 검사
    optionCnt = Get_Option( optionList, argc, argv );
    if( optionCnt > 0 )
    {
        if( optionCnt > MAX_ADD_OPTION_CNT )
        {
            Print_Usage();
            
            return -1;
        }

        optionActivate = true;
    }
    else if( optionCnt == 0 )
    {
        optionActivate = false;
    }
    else
    {
        return -1;
    }

    // 정규파일 또는 디렉토리 여부와 옵션 활성화 여부 검사
    checkFileOrDir = Check_File_Or_Directory( path, optionActivate );
    if( checkFileOrDir == false )
    {
        return 0;
    }

    // 파일 관리 링크드리스트 획득
    fileListResult = Get_FileList( path );
    if( fileListResult == false )
    {
        return 0;
    }

    if( strcmp( *( argv + 0 ), "md5" ) == 0 )
    {
        HashType = E_MD5;
    }
    else if( strcmp( *( argv + 0 ), "sha1" ) == 0 )
    {
        HashType = E_SHA1;
    }

    // 백업 시작
    backupResult = Backup();
    if( backupResult == false )
    {
        return 0;
    }

    
    return 0;
}


int Get_Option( char *optionList, int argc, char *argv[] )
{
    int     option      = -1;
    int     optionCnt   = 0;

    char    **argvCopy  = NULL;


    argvCopy = malloc( sizeof( char * ) * 10 );
    for( int i = 0; i < 10; i++ )
    {
        *( argvCopy + i ) = malloc( sizeof( char ) * ( strlen( argv[ i ] ) + 1 ) );
        if( *( argvCopy + i ) == NULL )
        {
            return -1;
        }

        strcpy( *( argvCopy + i ), argv[ i ] );
    }

    while( true )
    {
        option = getopt( argc, argvCopy, ADD_OPTION );

        if( option == -1 )
        {
            break;
        }

        if( option == '?' )
        {
            Print_Usage();

            return -1;
        }
        else if( ( optionCnt < argc  ) )
        {
            for( int i = 0; i < optionCnt; i++ )
            {
                if( *( optionList + i ) == option )
                {
                    printf( "Invalid option : Overlap option\n" );

                    return ++optionCnt;
                }
            }
            
            if( optionCnt >= MAX_ADD_OPTION_CNT )
            {
                return ++optionCnt;
            }

            *( optionList + optionCnt ) = option;

            optionCnt++;
        }
    }

    for( int i = 0; i < 10; i++ )
    {
        if( *( argvCopy + i ) == NULL )
        {
            return -1;
        }

        free( *( argvCopy + i ) );
        *( argvCopy + i ) = NULL;
    }

    if( argvCopy == NULL )
    {
        return -1;
    }

    free( argvCopy );
    argvCopy = NULL;


    return optionCnt;
}


bool Get_FileList( char *path )
{
    bool    manager = false;


    if( path == NULL )
    {
        return false;
    }

    // 사용자의 디렉토리 링크드리스트 생성
    manager = FileListManager( path, E_FileList );
    if( manager == false )
    {
        return false;
    }


    return true;
}


void Get_BackupTime( char backupTime[ BACKUP_TIME_FORMAT_SIZE] )
{
    time_t      timer;

    struct tm   *currentTime;

    char        year[ 5 ];
    char        month[ 3 ];
    char        day[ 3 ];
    char        hour[ 3 ];
    char        minute[ 3 ];
    char        second[ 3 ];


    timer           = time( NULL );
    currentTime     = localtime( &timer );

    sprintf( year, "%d", currentTime->tm_year + 1900 );
    
    if( ( currentTime->tm_mon + 1 ) < 10 )
    {
        month[ 0 ] = '0';
        sprintf( &month[ 1 ], "%d", ( currentTime->tm_mon + 1 ) );
    }
    else
    {
        sprintf( month, "%d", ( currentTime->tm_mon + 1 ) );
    }

    if( currentTime->tm_mday < 10 )
    {
        day[ 0 ] = '0';
        sprintf( &day[ 1 ], "%d", currentTime->tm_mday );
    }
    else
    {
        sprintf( day, "%d", currentTime->tm_mday );
    }

    if( currentTime->tm_hour < 10 )
    {
        hour[ 0 ] = '0';
        sprintf( &hour[ 1 ], "%d", currentTime->tm_hour );
    }
    else
    {
        sprintf( hour, "%d", currentTime->tm_hour );
    }

    if( currentTime->tm_min < 10 )
    {
        minute[ 0 ] = '0';
        sprintf( &minute[ 1 ], "%d", currentTime->tm_min );
    }
    else
    {
        sprintf( minute, "%d", currentTime->tm_min );
    }

    if( currentTime->tm_sec < 10 )
    {
        second[ 0 ] = '0';
        sprintf( &second[ 1 ], "%d", currentTime->tm_sec );
    }
    else
    {
        sprintf( second, "%d", currentTime->tm_sec );
    }
    
    strcpy( backupTime, &year[ 2 ] );
    strcat( backupTime, month );
    strcat( backupTime, day );
    strcat( backupTime, hour );
    strcat( backupTime, minute );
    strcat( backupTime, second );


    return;
}


bool Check_File_Or_Directory( char *path, bool optionActivate )
{
    struct stat     fileStat;

    int             statResult  = -1;

    bool            checkResult = false;


    // 경로 길이 검사
    if( strlen( path ) >= MAX_FILE_NAME_SIZE )
    {
        printf( "Too long path\n" );

        return false;
    }

    // 경로 존재 여부 검사
    statResult = stat( path, &fileStat );
    if( statResult == -1 )
    {
        Print_Usage();

        return false;
    }

    // 접근 권한 검사
    if( access( path, R_OK ) == -1 )
    {
        printf( "Non Permission - Can't file access\n" );

        return false;
    }

    // 파일 정보 검사
    checkResult = Check_File_Info( path, fileStat, optionActivate );
    if( checkResult == false )
    {
        return false;
    }
    

    return true;
}


bool Check_File_Info( char *path, struct stat fileStat, bool optionActivate )
{
    if( path == NULL )
    {
        return false;
    }

    switch( fileStat.st_mode & S_IFMT )
    {
        case S_IFREG:
        break;

        
        case S_IFDIR:
        {
            if( optionActivate == false )
            {
                printf( "\"%s\" is a directory file\n", path );

                return false;
            }
        }
        break;


        default:
        {
            printf( "Not regular file or directory\n" );

            return false;
        }
    }


    return true;
}


bool Backup()
{
    bool    manager         = false;
    bool    searchResult    = false;


    // 백업 디렉토리 링크드리스트 생성
    manager = FileListManager( Backup_Directory_Path, E_BackupList );
    if( manager == false )
    {
        return false;
    }

    // 링크드리스트 재귀 탐색
    Recursive_Search( FileList->currentDirNode );


    return true;
}


void Recursive_Search( DirectoryNode *dirNode )
{
    FileNode        *f_Node;
    DirectoryNode   *d_Node;

    char            dirPath[ MAX_PATH_BUFFER_SIZE ]         = { '\0' };
    char            filePath[ MAX_PATH_BUFFER_SIZE ]        = { '\0' };
    char            fileBackupPath[ MAX_PATH_BUFFER_SIZE ]  = { '\0' };
    char            splitPath[ MAX_PATH_BUFFER_SIZE ]       = { '\0' };

    char            backupTime[ BACKUP_TIME_FORMAT_SIZE ]   = { '\0' };

    int             mkdirResult;
    int             mkdirErrno;

    bool            backupResult    = false;


    if( dirNode == NULL )
    {
        return;
    }

    if( ( dirNode->subDirNode == NULL ) && ( dirNode->fileNode == NULL ) )
    {
        return;
    }

    d_Node = dirNode;
    while( d_Node != NULL )
    {
        if( access( d_Node->dirPath, R_OK ) == -1 )
        {
            printf( "Add : Non Permission - Can't \"%s\" access\n", d_Node->dirPath );
        }
        else
        {
            memset( dirPath, '\0', MAX_PATH_BUFFER_SIZE );
            memset( splitPath, '\0', MAX_PATH_BUFFER_SIZE );

            strcpy( dirPath, d_Node->dirPath );
            strcpy( splitPath, &dirPath[ strlen( HomeDirPath ) ] );
            
            memset( dirPath, '\0', MAX_PATH_BUFFER_SIZE );
            strcpy( dirPath, Backup_Directory_Path );
            strcat( dirPath, splitPath );

            // 백업 디렉토리 내 백업할 디렉토리 생성
            mkdirResult     = mkdir( dirPath, MKDIR_MODE );
            mkdirErrno      = errno;

            if( mkdirResult == -1 )
            {
                if( mkdirErrno != EEXIST )
                {
                    char    mkPath[ MAX_PATH_BUFFER_SIZE ]  = { '\0' };
                    char    *strPtr     = NULL;


                    strcpy( mkPath, Backup_Directory_Path );
                    strPtr = strtok( &dirPath[ strlen( Backup_Directory_Path ) ], "/" );
                    while( strPtr != NULL )
                    {
                        strcat( mkPath, "/" );
                        strcat( mkPath, strPtr );

                        // 경로상에 존재하지 않는 디렉토리가 다수일 경우 모두 생성
                        mkdirResult = mkdir( mkPath, MKDIR_MODE );
                        if( mkdirErrno == -1 )
                        {
                            printf( "Fail mkdir in backup directory\n" );
                            printf( "Error : %s\n", strerror( mkdirErrno ) );

                            return;
                        }

                        strPtr = strtok( NULL, "/" );
                    }
                }
            }
        }

        d_Node = d_Node->nextDirNode;
    }

    f_Node = dirNode->fileNode;
    if( f_Node != NULL )
    {
        if( strcmp( f_Node->fileName, "" ) == 0 )
        {
            f_Node = f_Node->nextFileNode;
        }
    }

    // 파일 노드 순회하면서 파일 백업 진행
    while( f_Node != NULL )
    {
        memset( filePath, '\0', MAX_PATH_BUFFER_SIZE );
        memset( splitPath, '\0', MAX_PATH_BUFFER_SIZE );
        memset( fileBackupPath, '\0', MAX_PATH_BUFFER_SIZE );
        memset( backupTime, '\0', BACKUP_TIME_FORMAT_SIZE );

        Get_BackupTime( backupTime );

        strcpy( filePath, dirNode->dirPath );
        strcat( filePath, "/" );
        strcat( filePath, f_Node->fileName );

        if( access( filePath, R_OK ) == -1 )
        {
            printf( "Add : Non Permission - Can't \"%s\" access\n", filePath );
        }
        else
        {
            strcpy( splitPath, &filePath[ strlen( HomeDirPath ) ] );

            strcpy( fileBackupPath, Backup_Directory_Path );
            strcat( fileBackupPath, splitPath );
            strcat( fileBackupPath, "_" );
            strcat( fileBackupPath, backupTime );

            // 파일 백업
            backupResult = File_Backup( filePath, fileBackupPath );
            if( backupResult == false )
            {
                printf( "Fail to backup \"%s\"", filePath );
            }
        }
        
        f_Node = f_Node->nextFileNode;
    }

    if( dirNode == NULL )
    {
        return;
    }

    d_Node = dirNode->nextDirNode;
    while( d_Node != NULL )
    {
        // 재귀 탐색
        Recursive_Search( d_Node->subDirNode );

        d_Node = d_Node->nextDirNode;
    }


    return;
}


bool File_Backup( char filePath[ MAX_PATH_BUFFER_SIZE ], char fileBackupPath[ MAX_PATH_BUFFER_SIZE ] )
{
    FILE            *originFile     = NULL;
    FILE            *backupFile     = NULL;

    char            backupDirPath[ MAX_PATH_BUFFER_SIZE ] = { '\0' };

    int             readCnt;
    int             writeCnt;

    int             closeResult;
    E_MD5_State     md5Result       = E_MD5_Invalid;
    E_SHA1_State    sha1Result      = E_SHA1_Invalid;
    E_Search        findResult      = false;


    if( filePath == NULL )
    {
        return false;
    }

    if( fileBackupPath == NULL )
    {
        return false;
    }

    strcpy( backupDirPath, fileBackupPath );
    
    for( int i = strlen( backupDirPath ); i >= 0; i-- )
    {
        if( backupDirPath[ i ] == '/' )
        {
            backupDirPath[ i ] = '\0';

            break;
        }
    }

    // 해시 값 비교를 위해 백업 리스트 탐색
    findResult = Find_BackupDirNode( BackupList->currentDirNode, backupDirPath, filePath );
    if( findResult == E_Invalid )
    {
        return true;
    }

    originFile = fopen( filePath, "rb" );
    if( originFile == NULL )
    {
        return false;
    }

    backupFile = fopen( fileBackupPath, "wb" );
    if( backupFile == NULL )
    {
        return false;
    }
    
    // 실질적인 파일 백업 진행
    while( feof( originFile ) == 0 )
    {
        char    fileData;


        readCnt = 0;
        readCnt = fread( &fileData, sizeof( char ), FILE_RD_WR_BUFFER_SIZE, originFile );
        if( readCnt != FILE_RD_WR_BUFFER_SIZE )
        {
            if( feof( originFile ) != 0 )
            {
                break;
            }

            if( originFile == NULL )
            {
                return false;
            }

            closeResult = fclose( originFile );
            if( closeResult != 0 )
            {
                return false;
            }

            if( backupFile == NULL )
            {
                return false;
            }

            closeResult = fclose( backupFile );
            if( closeResult != 0 )
            {
                return false;
            }

            return false;
        }

        writeCnt = fwrite( &fileData, sizeof( char ), FILE_RD_WR_BUFFER_SIZE, backupFile );
        if( writeCnt != FILE_RD_WR_BUFFER_SIZE )
        {
            if( originFile == NULL )
            {
                return false;
            }

            closeResult = fclose( originFile );
            if( closeResult != 0 )
            {
                return false;
            }

            if( backupFile == NULL )
            {
                return false;
            }

            closeResult = fclose( backupFile );
            if( closeResult != 0 )
            {
                return false;
            }

            return false;
        }
    }

    if( originFile == NULL )
    {
        return false;
    }

    closeResult = fclose( originFile );
    if( closeResult != 0 )
    {
        return false;
    }

    if( backupFile == NULL )
    {
        return false;
    }

    closeResult = fclose( backupFile );
    if( closeResult != 0 )
    {
        return false;
    }

    printf( "\"%s\" backuped\n", fileBackupPath );


    return true;
}


E_Search Find_BackupDirNode( DirectoryNode *dirNode, char *backupDirPath, char *originFilePath )
{
    DirectoryNode   *travelDirNode  = NULL;

    E_Search        searchState     = E_Invalid;

    bool            findResult      = false;


    travelDirNode = dirNode;

    while( travelDirNode != NULL )
    {
        if( strcmp( travelDirNode->dirPath, backupDirPath ) == 0 )
        {
            // 파일 노드 탐색
            findResult = Find_BackupFileNode( travelDirNode->subDirNode, originFilePath );
            if( findResult == false )
            {
                return E_Invalid;
            }

            return E_Stop;
        }

        if( travelDirNode != NULL & travelDirNode->subDirNode != NULL )
        {
            // 하위 디렉토리 탐색
            searchState = Find_BackupDirNode( travelDirNode->subDirNode, backupDirPath, originFilePath );
            if( searchState == E_Invalid )
            {
                return E_Invalid;
            }
            else if( searchState == E_Stop )
            {
                return E_Stop;
            }
        }

        travelDirNode = travelDirNode->nextDirNode;
    }


    return E_Continue;
}


bool Find_BackupFileNode( DirectoryNode *dirNode, char *originFilePath )
{
    FileNode        *travelFileNode     = NULL;

    char            compareFilePath[ MAX_PATH_BUFFER_SIZE ] = { '\0' };

    E_MD5_State     md5Result           = E_MD5_Invalid;
    E_SHA1_State    sha1Result          = E_SHA1_Invalid;


    if( dirNode == NULL )
    {
        return false;
    }

    travelFileNode = dirNode->fileNode;

    while( travelFileNode != NULL )
    {
        if( strcmp( travelFileNode->fileName, "" ) != 0 )
        {
            memset( compareFilePath, '\0', MAX_PATH_BUFFER_SIZE );

            strcpy( compareFilePath, dirNode->dirPath );
            strcat( compareFilePath, "/" );
            strcat( compareFilePath, travelFileNode->fileName );

            // md5 또는 sha1 해시 함수로 해시 값 비교
            if( HashType == E_MD5 )
            {
                md5Result = Check_MD5( originFilePath, compareFilePath );
                if( md5Result == E_MD5_Invalid )
                {
                    return false;
                }
                
                if( md5Result == E_MD5_Same )
                {
                    printf( "\"%s\" is already backuped\n", compareFilePath );

                    return false;
                }
            }
            else if( HashType == E_SHA1 )
            {
                sha1Result = Check_SHA1( originFilePath, compareFilePath );
                if( sha1Result == E_SHA1_Invalid )
                {
                    return false;
                }
                
                if( sha1Result == E_SHA1_Same )
                {
                    printf( "\"%s\" is already backuped\n", compareFilePath );

                    return false;
                }
            }
            else if( HashType == E_Invalid_Hash )
            {
                return false;
            }
        }

        travelFileNode = travelFileNode->nextFileNode;
    }


    return true;
}


void Print_Usage()
{
    printf( "Usage : add <FILENAME> [OPTION]\n" );
    printf( "  -d : add directory recursive\n" );


    return;
}