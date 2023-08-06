

#include "Remove.h"


extern FileListNode     *FileList;
extern FileListNode     *BackupList;


int main( int argc, char *argv[] )
{
    char    path[ MAX_PATH_BUFFER_SIZE ]    = { '\0' };

    char    *homeText           = "/home/";

    char    *optionList         = NULL;
    char    *memsetResult       = NULL;

    int     optionCnt           = 0;
    bool    optionActivate      = false;

    bool    checkResult         = false;
    bool    checkFileOrDir      = false;
    bool    checkPath           = false;
    bool    fileListResult      = false;
    bool    backupResult        = false;
    bool    getDirectoryResult  = false;
    bool    removeResult        = false;


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

    optionList = malloc( sizeof( char ) * MAX_REMOVE_OPTION_CNT );
    if( optionList == NULL )
    {
        return -1;
    }

    memsetResult = NULL;
    memsetResult = memset( optionList, '\0', sizeof( char ) * MAX_REMOVE_OPTION_CNT );
    if( memsetResult == NULL )
    {
        return -1;
    }

    // 옵션 검사
    optionCnt = Get_Option( optionList, argc, argv );
    if( optionCnt > 0 )
    {
        if( optionCnt > MAX_REMOVE_OPTION_CNT )
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

    if( Remove_Option == E_c )
    {
        if( strcmp( *( argv + 3 ), "" ) != 0 )
        {
            Print_Usage();

            return false;
        }
    }
    else
    {
        // 올바른 경로인지 검사
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

        for( int i = 0; i < ( strlen( homeText ) - 1 ); i++ )
        {
            if( *( path + i ) != *( homeText + i ) )
            {
                printf( "\"%s\" can't be backuped\n", path );
                
                return -1;
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
                
                return -1;
            }
        }

        // 정규파일 또는 디렉토리 여부와 옵션 활성화 여부 검사
        checkFileOrDir = Check_File_Or_Directory( argc, path, optionActivate );
        if( checkFileOrDir == false )
        {
            return -1;
        }
    }

    // 백업 디렉토리 링크드리스트 획득
    fileListResult = Get_FileList( Backup_Directory_Path );
    if( fileListResult == false )
    {
        return -1;
    }

    // 파일 삭제 시작
    removeResult = Remove( path );
    if( removeResult == false )
    {
        return -1;
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
        option = getopt( argc, argvCopy, REMOVE_OPTION );

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
            
            if( optionCnt >= MAX_REMOVE_OPTION_CNT )
            {
                return ++optionCnt;
            }

            *( optionList + optionCnt ) = option;

            optionCnt++;
        }
    }

    if( strcmp( ( optionList + 0 ), "a" ) == 0 )
    {
        Remove_Option = E_a;
    }
    else if( strcmp( ( optionList + 0 ), "c" ) == 0 )
    {
        Remove_Option = E_c;
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

    manager = FileListManager( path, E_BackupList );
    if( manager == false )
    {
        return false;
    }


    return true;
}


bool Check_File_Or_Directory( int argc, char *path, bool optionActivate )
{
    struct stat     fileStat;

    int             statResult  = -1;

    bool            checkPath   = false;
    bool            checkResult = false;


    // 옵션 검사
    if( Remove_Option == E_c )
    {
        if( strcmp( path, "-c" ) != 0 )
        {
            Print_Usage();

            return false;
        }

        return true;
    }

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
        Empty_In_Path = true;

        // printf( "Error : Invalid path\n" );

        return true;
    }

    // 접근 권한 검사
    if( access( path, R_OK ) == -1 )
    {
        printf( "Non Permission - Can't file access\n" );

        return false;
    }

    if( Empty_In_Path == false )
    {
        // 올바른 경로인지 검사
        checkPath = Check_Path_Info( path, E_Type_Remove );
        if( checkPath == false )
        {
            return false;
        }
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

            if( Remove_Option != E_a )
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


bool Remove( char *path )
{
    char    pathCopy[ MAX_PATH_BUFFER_SIZE ]        = { '\0' };
    char    convertPath[ MAX_PATH_BUFFER_SIZE ]     = { '\0' };
    char    resolvedPath[ MAX_PATH_BUFFER_SIZE ]    = { '\0' };

    bool    realPathResult      = false;

    bool    manager             = false;
    bool    searchResult        = false;


    if( path == NULL )
    {
        return false;
    }

    strcpy( pathCopy, path );

    // 사용자의 디렉토리 링크드리스트 생성
    manager = FileListManager( pathCopy, E_RemoveFileList );
    if( manager == false )
    {
        return false;
    }

    // 탐색 경로 변환
    if( *( path + 0 ) != '/' )
    {
        strcpy( convertPath, Backup_Directory_Path );
        strcat( convertPath, ( Current_Work_Directory_Path + strlen( HomeDirPath ) ) );
        strcat( convertPath, "/" );
        strcat( convertPath, path );
    }
    else
    {
        strcpy( convertPath, Backup_Directory_Path );
        strcat( convertPath, ( path + strlen( HomeDirPath ) ) );
    }

    realPathResult = Custom_RealPath( convertPath, resolvedPath );
    if( realPathResult == false )
    {
        printf( "Not found backup file\n" );

        return false;
    }

    // 링크드리스트 탐색
    searchResult = Search_FileList( resolvedPath );
    if( searchResult == false )
    {
        return false;
    }


    return true;
}


bool Search_FileList( char *path )
{
    DirectoryNode   *travelDirNode      = NULL;
    DirectoryNode   *travelSubDirNode   = NULL;
    FileNode        *travelFileNode     = NULL;

    struct stat     fileStat;

    char            resolvedPath[ MAX_PATH_BUFFER_SIZE ]    = { '\0' };
    char            backupPath[ MAX_PATH_BUFFER_SIZE ]      = { '\0' };

    char            *pathCopy           = NULL;
    char            *splitPath          = NULL;
    char            *splitFile          = NULL;

    char            *realpathResult     = NULL;

    int             searchCnt           = 0;
    E_Search        searchState         = E_Invalid;

    int             statResult          = -1;


    if( path == NULL )
    {
        return false;
    }

    // -c 옵션
    if( Remove_Option == E_c )
    {
        if( BackupList->currentDirNode->nextDirNode == NULL )
        {
            printf( "no file(s) in the backup\n" );
        }
        else
        {
            RemoveDirCnt        = 0;
            RemoveFileCnt       = 0;
            SearchedRemoveCnt   = 0;

            // 백업 링크드리스트 재귀 탐색
            searchState = Recursive_Search( BackupList->currentDirNode, Backup_Directory_Path, NULL );
            if( searchState == E_Invalid )
            {
                return false;
            }

            printf( "backup directory cleared(%d regular files and %d subdirectories totally.)\n", RemoveFileCnt, RemoveDirCnt );
        }

        Delete_Memory( FileList, BackupList, pathCopy, splitPath, splitFile );


        return true;
    }


    statResult = stat( path, &fileStat );
    if( statResult == -1 )
    {
        pathCopy = malloc( sizeof( char ) * ( strlen( path ) + 1 ) );
        if( pathCopy == NULL )
        {
            return false;
        }

        strcpy( pathCopy, path );

        SearchedRemoveCnt = 0;

        // 디렉토리 경로 부분과 파일 경로 부분을 분리
        for( int i = strlen( path ); i >= 0; i-- )
        {
            if( *( pathCopy + i ) == '/' )
            {
                splitPath = malloc( sizeof( char ) * i );
                if( splitPath == NULL )
                {
                    Delete_Memory( FileList, BackupList, pathCopy, NULL, NULL );

                    return false;
                }

                splitFile = malloc( sizeof( char ) * ( strlen( pathCopy ) - i + 1 ) );
                if( splitFile == NULL )
                {
                    Delete_Memory( FileList, BackupList, pathCopy, splitPath, NULL );

                    return false;
                }

                *( pathCopy + i ) = '\0';

                strcpy( splitPath, pathCopy );
                strcpy( splitFile, ( pathCopy + i + 1 ) );

                break;
            }
        }

        realpath( path, resolvedPath );
        strcpy( backupPath, Backup_Directory_Path );
        strcat( backupPath, &resolvedPath[ strlen( Backup_Directory_Path ) ] );

        if( splitFile != NULL )
        {
            strcat( backupPath, "/" );
            strcat( backupPath, splitFile );
        }

        if( splitPath == NULL )
        {
            realpath( path, resolvedPath );
        }
        else
        {
            realpath( splitPath, resolvedPath );
        }

        memset( backupPath, '\0', MAX_PATH_BUFFER_SIZE );
        strcpy( backupPath, Backup_Directory_Path );
        strcat( backupPath, &resolvedPath[ strlen( Backup_Directory_Path ) ] );

        SearchedRemoveCnt = 0;

        // 백업 링크드리스트 재귀 탐색
        searchState = Recursive_Search( BackupList->currentDirNode, backupPath, splitFile );
        if( searchState == E_Invalid )
        {
            return false;
        }
    }
    else
    {
        if( Remove_Option != E_a )
        {
            printf( "\"%s\" is a directory file\n", path );

            Delete_Memory( FileList, BackupList, pathCopy, splitPath, splitFile );

            return false;
        }

        pathCopy = malloc( sizeof( char ) * ( strlen( path ) + 1 ) );
        if( pathCopy == NULL )
        {
            Delete_Memory( FileList, BackupList, pathCopy, splitPath, splitFile );

            return false;
        }

        strcpy( pathCopy, path );

        splitPath = malloc( sizeof( char ) * ( strlen( pathCopy ) + 1 ) );
        if( splitPath == NULL )
        {
            Delete_Memory( FileList, BackupList, pathCopy, splitPath, splitFile );

            return false;
        }

        strcpy( splitPath, pathCopy );

        realpathResult = realpath( splitPath, resolvedPath );
        if( realpathResult == NULL )
        {
            realpath( path, resolvedPath );
        }

        strcpy( backupPath, Backup_Directory_Path );
        strcat( backupPath, &resolvedPath[ strlen( Backup_Directory_Path ) ] );

        // 백업 디렉토리 링크드리스트 재귀 탐색
        SearchedRemoveCnt = 0;
        searchState = Recursive_Search( BackupList->currentDirNode, backupPath, NULL );
        if( searchState == E_Invalid )
        {
            return false;
        }
    }

    if( ( RemoveFileCnt == 0 ) && ( RemoveDirCnt == 0 ) )
    {
        if( IsFind == false )
        {
            printf( "Not found backup file\n" );
        }
    }

    
    Delete_Memory( FileList, BackupList, pathCopy, splitPath, splitFile );


    return true;
}


E_Search Recursive_Search( DirectoryNode *travelDirNode, char *backupDirPath, char *backupFilePath )
{
    FileNode        *travelFileNode     = NULL;

    E_Search        searchState         = E_Invalid;

    char            firstFileTime[ BACKUP_TIME_FORMAT_SIZE ]    = { '\0' };
    char            firstFileSize[ MAX_FILE_SIZE_BUFFER ]       = { '\0' };

    char            backupTime[ BACKUP_TIME_FORMAT_SIZE ]   = { '\0' };
    char            fileName[ MAX_FILE_NAME_SIZE ]          = { '\0' };
    char            filePath[ MAX_PATH_BUFFER_SIZE ]        = { '\0' };
    char            fileSize[ MAX_FILE_SIZE_BUFFER ]        = { '\0' };

    int             backupTimeLen   = 0;
    int             backupFileLen   = 0;

    int             sizeIndex       = 0;

    bool            convertResult       = false;


    if( backupDirPath == NULL )
    {
        return E_Invalid;
    }

    while( travelDirNode != NULL )
    {
        if( strcmp( travelDirNode->dirPath, backupDirPath ) == 0 )
        {
            if( backupFilePath == NULL )
            {
                SearchedRemoveCnt++;

                // 하위 디렉토리 삭제를 위한 탐색
                if( Remove_Option == E_c )
                {
                    RM_Directory( travelDirNode, backupDirPath );
                }
                else
                {
                    RM_Directory( travelDirNode->subDirNode, backupDirPath );
                }
            }
            else
            {
                backupTimeLen   = 0;
                
                travelFileNode = travelDirNode->subDirNode->fileNode;
                if( strcmp( travelFileNode->fileName, "" ) == 0 )
                {
                    if( travelFileNode->nextFileNode != NULL )
                        travelFileNode = travelFileNode->nextFileNode;
                }
                
                backupFileLen   = 0;
                while( travelFileNode != NULL )
                {
                    // 삭제 대상 파일 경로 생성
                    memset( backupTime, '\0', BACKUP_TIME_FORMAT_SIZE );
                    memset( fileName, '\0', MAX_FILE_NAME_SIZE );
                    memset( filePath, '\0', MAX_PATH_BUFFER_SIZE );

                    backupFileLen = strlen( travelFileNode->fileName );

                    strcpy( backupTime, &( travelFileNode->fileName )[ backupFileLen - BACKUP_TIME_FORMAT_SIZE + 2 ] );
                    backupTime[ BACKUP_TIME_FORMAT_SIZE - 1 ] = '\0';
                    
                    strcpy( fileName, travelFileNode->fileName );
                    fileName[ backupFileLen - BACKUP_TIME_FORMAT_SIZE + 1 ] = '\0';

                    strcpy( filePath, backupDirPath );
                    strcat( filePath, "/" );
                    strcat( filePath, travelFileNode->fileName );

                    if( ( Remove_Option != E_a ) && ( strcmp( fileName, backupFilePath ) == 0 ) )
                    {
                        memset( fileSize, '\0', MAX_FILE_SIZE_BUFFER );

                        if( IsFind == false )
                        {
                            IsFind = true;
                        }

                        // 파일 크기 출력 형식 변환
                        convertResult = Convert_FileSize_Format( fileSize, filePath );
                        if( convertResult == false )
                        {
                            return E_Invalid;
                        }

                        SearchedRemoveCnt++;

                        // 파일 백업 상태에 따른 파일 삭제 방식
                        if( SearchedRemoveCnt == 1 )
                        {
                            strcpy( firstFileTime, backupTime );
                            strcpy( firstFileSize, fileSize );
                        }
                        else if( SearchedRemoveCnt == 2 )
                        {
                            char listPath[ MAX_PATH_BUFFER_SIZE ] = { '\0' };

                            strcpy( listPath, backupDirPath);
                            strcat( listPath, "/" );
                            strcat( listPath, fileName );

                            printf( "backup file list of \"%s\"\n", listPath );
                            printf( "0. exit\n" );
                            printf( "%d. %s \t%sbytes\n", 1, firstFileTime, firstFileSize );
                            printf( "%d. %s \t%sbytes\n", 2, backupTime, fileSize );
                        }
                        else if( SearchedRemoveCnt >= 3 )
                        {
                            printf( "%d. %s \t%sbytes\n", SearchedRemoveCnt, backupTime, fileSize );
                        }
                    }

                    travelFileNode = travelFileNode->nextFileNode;
                }

                RM_File( travelDirNode->subDirNode->fileNode, backupDirPath, backupFilePath );
            }


            return E_Stop;
        }

        
        if( travelDirNode != NULL & travelDirNode->subDirNode != NULL )
        {
            // 하위 디렉토리 재귀 탐색
            searchState = Recursive_Search( travelDirNode->subDirNode, backupDirPath, backupFilePath );
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


void RM_Directory( DirectoryNode *d_Node, char *backupDirPath )
{
    DirectoryNode   *travelDirNode      = NULL;
    FileNode        *travelFileNode     = NULL;

    char            removePath[ MAX_PATH_BUFFER_SIZE ]  = { '\0' };;

    int             removeResult        = -1;


    travelDirNode = d_Node;
    while( travelDirNode != NULL )
    {
        if( travelDirNode->subDirNode != NULL )
        {
            // 하위 디렉토리 탐색
            RM_Directory( travelDirNode->subDirNode, travelDirNode->subDirNode->dirPath );
        }

        travelDirNode = travelDirNode->nextDirNode;

        
    }

    travelFileNode = d_Node->fileNode;

    SearchedRemoveCnt = 0;

    while( travelFileNode != NULL )
    {
        if( ( travelFileNode->fileName != NULL ) && ( strcmp( travelFileNode->fileName, "" ) != 0 ) )
        {
            memset( removePath, '\0', MAX_PATH_BUFFER_SIZE );
        
            strcpy( removePath, d_Node->dirPath );
            strcat( removePath, "/" );
            strcat( removePath, travelFileNode->fileName );

            // 해당 디렉토리 내에 존재하는 파일 삭제
            removeResult = remove( removePath );
            if( removeResult == -1 )
            {
                printf( "Fail to remove \"%s\"\n", removePath );

                return;
            }

            if( ( Remove_Option == E_Remove_Non ) || ( Remove_Option == E_a ) )
            {
                printf( "\"%s\" backup file removed\n", removePath );
            }

            RemoveFileCnt++;
        }

        travelFileNode = travelFileNode->nextFileNode;

        if( travelFileNode == NULL )
        {
            if( strcmp( d_Node->dirPath, Backup_Directory_Path ) != 0 )
            {
                // 디렉토리 삭제
                removeResult = remove( d_Node->dirPath );
                if( removeResult == -1 )
                {
                    printf( "Fail to remove \"%s\"\n", d_Node->dirPath );

                    return;
                }

                RemoveDirCnt++;
            }
        }
    }


    return;
}


void RM_File( FileNode *travelFileNode, char *backupDirPath, char *backupFilePath )
{
    char    backupTime[ BACKUP_TIME_FORMAT_SIZE ]   = { '\0' };
    char    fileName[ MAX_FILE_NAME_SIZE ]          = { '\0' };
    char    filePath[ MAX_PATH_BUFFER_SIZE ]        = { '\0' };

    int     backupFileLen   = 0;
    
    int     selectNumber    = -1;

    int     fileCnt         = 0;

    int     removeResult    = -1;

    
    if( SearchedRemoveCnt > 1 )
    {
        if( Remove_Option != E_a )
        {
            // 삭제할 파일 백업 버전 선택
            selectNumber = Select_File( E_Type_Remove );
            if( ( selectNumber < 0 ) || ( selectNumber > SearchedRemoveCnt ) )
            {
                return;
            }
        }
    }
    else
    {
        selectNumber = 1;
    }

    while( travelFileNode != NULL )
    {
        // 삭제할 파일 경로 생성
        backupFileLen = strlen( travelFileNode->fileName );

        strcpy( backupTime, &( travelFileNode->fileName )[ backupFileLen - BACKUP_TIME_FORMAT_SIZE + 2 ] );
        backupTime[ BACKUP_TIME_FORMAT_SIZE - 1 ] = '\0';
                    
        strcpy( fileName, travelFileNode->fileName );
        fileName[ backupFileLen - BACKUP_TIME_FORMAT_SIZE + 1 ] = '\0';

        strcpy( filePath, backupDirPath );
        strcat( filePath, "/" );
        strcat( filePath, travelFileNode->fileName );

        if( strcmp( fileName, backupFilePath ) == 0 )
        {
            if( Remove_Option == E_a )
            {
                if( strcmp( filePath, Backup_Directory_Path ) != 0 )
                {
                    // 파일 삭제
                    removeResult = remove( filePath );
                    if( removeResult == -1 )
                    {
                        printf( "Fail to remove \"%s\"\n", filePath );

                        return;
                    }

                    if( ( Remove_Option == E_Remove_Non ) || ( Remove_Option == E_a ) )
                    {
                        printf( "\"%s\" backup file removed\n", filePath );
                    }

                    RemoveFileCnt++;
                }
            }
            else
            {
                fileCnt++;

                if( fileCnt == selectNumber )
                {
                    if( strcmp( filePath, Backup_Directory_Path ) != 0 )
                    {
                        // 파일 삭제
                        removeResult = remove( filePath );
                        if( removeResult == -1 )
                        {
                            printf( "Fail to remove \"%s\"\n", filePath );

                            return;
                        }

                        if( ( Remove_Option == E_Remove_Non ) || ( Remove_Option == E_a ) )
                        {
                            printf( "\"%s\" backup file removed\n", filePath );
                        }

                        RemoveFileCnt++;

                        break;
                    }
                }
            }
        }

        travelFileNode = travelFileNode->nextFileNode;
    }

    
    return;
}


void Print_Usage()
{
    printf( "Usage : remove <FILENAME> [OPTION]\n" );
    printf( "  -a : remove all file(recursive)\n" );
    printf( "  -c : clear backup directory\n" );


    return;
}