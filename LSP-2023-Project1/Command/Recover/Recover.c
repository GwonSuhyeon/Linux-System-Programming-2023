

#include "Recover.h"


extern FileListNode     *FileList;
extern FileListNode     *BackupList;

extern E_Hash_Type      HashType;


int main( int argc, char *argv[] )
{
    char    path[ MAX_PATH_BUFFER_SIZE ]    = { '\0' };

    char    *homeText               = "/home/";

    char    *optionList             = NULL;
    char    *memsetResult           = NULL;

    char    *newRecoverPath         = NULL;

    int     optionCnt               = 0;
    bool    optionActivate          = false;

    bool    checkResult             = false;
    bool    checkFileOrDir          = false;
    bool    checkPath               = false;
    bool    fileListResult          = false;
    bool    backupResult            = false;
    bool    getDirectoryResult      = false;
    bool    recoverResult           = false;


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

    optionList = malloc( sizeof( char ) * MAX_RECOVER_OPTION_CNT );
    if( optionList == NULL )
    {
        return -1;
    }

    memsetResult = NULL;
    memsetResult = memset( optionList, '\0', sizeof( char ) * MAX_RECOVER_OPTION_CNT );
    if( memsetResult == NULL )
    {
        return -1;
    }

    // 옵션 검사
    optionCnt = Get_Option( optionList, argc, argv );
    if( optionCnt > 0 )
    {
        if( optionCnt > MAX_RECOVER_OPTION_CNT )
        {
            Print_Usage();

            return -1;
        }

        if( Recover_Option == E_n )
        {
            if( strcmp( *( argv + 4 ), "" ) == 0 )
            {
                Print_Usage();

                return -1;
            }

            newRecoverPath = malloc( sizeof( char ) * MAX_PATH_BUFFER_SIZE );
            if( newRecoverPath == NULL )
            {
                return -1;
            }
            
            memset( newRecoverPath, '\0', MAX_PATH_BUFFER_SIZE );
            strcat( newRecoverPath, *( argv + 4 ) );
        }

        if( Recover_Option == E_dn )
        {
            if( ( strcmp( *( argv + 5 ), "" ) == 0 ) && ( strcmp( *( argv + 4 ), "" ) == 0 ) )
            {
                Print_Usage();

                return -1;
            }
            else
            {
                if( ( ( strcmp( *( argv + 3 ), "-dn" ) == 0 ) || ( strcmp( *( argv + 3 ), "-nb" ) == 0 ) ) && ( strcmp( *( argv + 4 ), "" ) == 0 ) )
                {
                    Print_Usage();

                    return -1;
                }
                else if( ( strcmp( *( argv + 4 ), "-n" ) == 0 ) && ( strcmp( *( argv + 5 ), "" ) == 0 ) )
                {
                    Print_Usage();

                    return -1;
                }
            }
            
            newRecoverPath = malloc( sizeof( char ) * MAX_PATH_BUFFER_SIZE );
            if( newRecoverPath == NULL )
            {
                return -1;
            }

            memset( newRecoverPath, '\0', MAX_PATH_BUFFER_SIZE );

            if( ( strcmp( *( argv + 3 ), "-dn" ) == 0 ) || ( strcmp( *( argv + 3 ), "-nd" ) == 0 ) )
            {
                strcat( newRecoverPath, *( argv + 4 ) );
            }
            else if( strcmp( *( argv + 4 ), "-n" ) == 0 )
            {
                strcat( newRecoverPath, *( argv + 5 ) );
            }
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
    checkFileOrDir = Check_File_Or_Directory( argc, path, false, optionActivate );
    if( checkFileOrDir == false )
    {
        return -1;
    }

    if( newRecoverPath != NULL )
    {
        // -n 옵션의 경로가 올바른지 검사
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

        // -n 옵션의 경로 검사
        checkPath = Check_NewPath_Info( newRecoverPath );
        if( checkPath == false )
        {
            return -1;
        }

        // -n 옵션의 경로가 정규파일 또는 디렉토리인지 여부와 옵션 활성화 여부 검사
        checkFileOrDir = Check_File_Or_Directory( argc, newRecoverPath, true, optionActivate );
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

    if( strcmp( *( argv + 0 ), "md5" ) == 0 )
    {
        HashType = E_MD5;
    }
    else if( strcmp( *( argv + 0 ), "sha1" ) == 0 )
    {
        HashType = E_SHA1;
    }

    // 파일 복구 시작
    recoverResult = Recover( path, newRecoverPath );
    if( recoverResult == false )
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
        option = getopt( argc, argvCopy, RECOVER_OPTION );

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
            
            if( optionCnt > MAX_RECOVER_OPTION_CNT )
            {
                return ++optionCnt;
            }

            *( optionList + optionCnt ) = option;

            optionCnt++;
        }
    }

    if( optionCnt == 1 )
    {
        if( strcmp( ( optionList + 0 ), "d" ) == 0 )
        {
            Recover_Option = E_d;
        }
        else if( strcmp( ( optionList + 0 ), "n" ) == 0 )
        {
            Recover_Option = E_n;
        }
    }
    else if( optionCnt == 2 )
    {
        Recover_Option = E_dn;
    }
    else
    {
        Recover_Option = E_Recover_Non;
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


bool Check_File_Or_Directory( int argc, char *path, bool isRecoverPath, bool optionActivate )
{
    struct stat     fileStat;

    int             statResult      = -1;

    bool            checkPath       = false;
    bool            checkResult     = false;


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

        if( isRecoverPath == false )
        {
            // printf( "Error : Invalid path\n" );

            InvalidRecoverPath = true;
        }

        return true;
    }

    // 접근 권한 검사
    if( access( path, R_OK ) == -1 )
    {
        printf( "Non Permission - Can't file access\n" );

        return false;
    }

    if( ( Empty_In_Path == false ) && ( isRecoverPath == false ) )
    {
        // 올바른 경로인지 검사
        checkPath = Check_Path_Info( path, E_Type_Recover );
        if( checkPath == false )
        {
            return -1;
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
        {
            Input_Type_Is_File = true;
        }
        break;

        
        case S_IFDIR:
        {
            if( optionActivate == false )
            {
                printf( "\"%s\" is a directory file\n", path );

                return false;
            }

            if( ( Recover_Option != E_d ) && ( Recover_Option != E_dn ) )
            {
                printf( "\"%s\" is a directory file\n", path );

                return false;
            }

            Input_Type_Is_File = false;
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


bool Check_NewPath_Info( char *path )
{
    char    resolvedPath[ MAX_PATH_BUFFER_SIZE ]    = { '\0' };
    char    homePath[ MAX_PATH_BUFFER_SIZE ]        = { '\0' };
    char    pwdPath[ MAX_PATH_BUFFER_SIZE ]         = { '\0' };
    char    backupPath[ MAX_PATH_BUFFER_SIZE ]      = { '\0' };

    char    *realPathResult                         = NULL;

    char    *splitPtr                               = NULL;

    char    convertPath[ MAX_PATH_BUFFER_SIZE ] = { '\0' };


    if( *( path + 0 ) != '/' )
    {
        strcpy( convertPath, Current_Work_Directory_Path );
        strcat( convertPath, "/" );
        strcat( convertPath, path );
    }
    else
    {
        strcpy( convertPath, path );
    }

    if( strcmp( HomeDirPath, convertPath ) == 0 )
    {
        printf( "\"%s\" can't be backuped\n", path );

        return false;
    }

    splitPtr = strtok( convertPath, "/" );
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

    memset( convertPath, '\0', MAX_PATH_BUFFER_SIZE );

    if( *( path + 0 ) != '/' )
    {
        strcpy( convertPath, Current_Work_Directory_Path );
        strcat( convertPath, "/" );
        strcat( convertPath, path );
    }
    else
    {
        strcpy( convertPath, path );
    }

    memset( pwdPath, '\0', MAX_PATH_BUFFER_SIZE );
    splitPtr = strtok( convertPath, "/" );
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


bool Recover( char *path, char *newRecoverPath )
{
    char    pathCopy[ MAX_PATH_BUFFER_SIZE ]            = { '\0' };
    char    convertPath[ MAX_PATH_BUFFER_SIZE ]         = { '\0' };
    char    resolvedPath[ MAX_PATH_BUFFER_SIZE ]        = { '\0' };
    char    newResolvedPath[ MAX_PATH_BUFFER_SIZE ]     = { '\0' };

    bool    realPathResult      = false;

    bool    manager             = false;
    bool    searchResult        = false;


    if( path == NULL )
    {
        return false;
    }

    strcpy( pathCopy, path );

    // 사용자의 디렉토리 링크드리스트 생성
    manager = FileListManager( pathCopy, E_RecoverFileList );
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

    memset( convertPath, '\0', MAX_PATH_BUFFER_SIZE );

    if( newRecoverPath != NULL )
    {
        if( *( newRecoverPath + 0 ) == '~' )
        {
            strcpy( convertPath, HomeDirPath );
            strcat( convertPath, ( newRecoverPath + 1 ) );
        }
        else if( *( newRecoverPath + 0 ) == '/' )
        {
            strcpy( convertPath, newRecoverPath );
        }
        else
        {
            strcpy( convertPath, Current_Work_Directory_Path );
            strcat( convertPath, "/" );
            strcat( convertPath, newRecoverPath );
        }

        realPathResult = Custom_RealPath( convertPath, newResolvedPath );
        if( realPathResult == false )
        {
            printf( "Not found backup file\n" );

            return false;
        }
    }

    // 링크드리스트 탐색
    searchResult = Search_FileList( resolvedPath, newResolvedPath );
    if( searchResult == false )
    {
        return false;
    }


    return true;
}


bool Search_FileList( char *path, char *newRecoverPath )
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

    statResult = stat( path, &fileStat );
    if( statResult == -1 )
    {
        pathCopy = malloc( sizeof( char ) * ( strlen( path ) + 1 ) );
        if( pathCopy == NULL )
        {
            return false;
        }

        strcpy( pathCopy, path );

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

        // 백업 링크드리스트 재귀 탐색
        SearchedRecoverCnt = 0;
        searchState = Recursive_Search( BackupList->currentDirNode, backupPath, splitFile, newRecoverPath );
        if( searchState == E_Invalid )
        {
            Delete_Memory( FileList, BackupList, pathCopy, splitPath, splitFile );

            return false;
        }
    }
    else
    {
        if( ( Recover_Option != E_d ) && ( Recover_Option != E_dn ) )
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

        // 백업 링크드리스트 재귀 탐색
        SearchedRecoverCnt = 0;
        searchState = Recursive_Search( BackupList->currentDirNode, backupPath, NULL, newRecoverPath );
        if( searchState == E_Invalid )
        {
            return false;
        }
    }

    if( IsFind == false )
    {
        printf( "Not found backup file\n" );
    }


    Delete_Memory( FileList, BackupList, pathCopy, splitPath, splitFile );


    return true;
}


E_Search Recursive_Search( DirectoryNode *travelDirNode, char *backupDirPath, char *backupFilePath, char *newRecoverPath )
{
    FileNode        *travelFileNode     = NULL;
    FileNode        *f_Node             = NULL;

    E_Search        searchState         = E_Invalid;

    char            firstFileTime[ BACKUP_TIME_FORMAT_SIZE ]    = { '\0' };
    char            firstFileSize[ MAX_FILE_SIZE_BUFFER ]       = { '\0' };

    char            backupTime[ BACKUP_TIME_FORMAT_SIZE ]       = { '\0' };
    char            fileName[ MAX_FILE_NAME_SIZE ]              = { '\0' };
    char            filePath[ MAX_PATH_BUFFER_SIZE ]            = { '\0' };
    char            fileSize[ MAX_FILE_SIZE_BUFFER ]            = { '\0' };
    char            compareFile[ MAX_PATH_BUFFER_SIZE ]         = { '\0' };

    int             backupFileLen       = 0;
    int             backupTimeLen       = 0;

    int             fileNodeIndex       = 0;

    int             selectNumber        = 0;

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
                SearchedRecoverCnt++;

                // 디렉토리 복구
                Directory_Recover( travelDirNode->subDirNode, backupDirPath, newRecoverPath );
            }
            else
            {
                travelFileNode = travelDirNode->subDirNode->fileNode;
                if( strcmp( travelFileNode->fileName, "" ) == 0 )
                {
                    if( travelFileNode->nextFileNode != NULL )
                    {
                        travelFileNode = travelFileNode->nextFileNode;
                    }
                }
                
                while( travelFileNode != NULL )
                {
                    // 복구 대상 파일 경로 생성
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

                    if( strcmp( fileName, backupFilePath ) == 0 )
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

                        SearchedRecoverCnt++;

                        // 파일 백업 상태에 따른 파일 복구 방식
                        if( SearchedRecoverCnt == 1 )
                        {
                            strcpy( firstFileTime, backupTime );
                            strcpy( firstFileSize, fileSize );
                        }
                        else if( SearchedRecoverCnt == 2 )
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
                        else if( SearchedRecoverCnt >= 3 )
                        {
                            printf( "%d. %s \t%sbytes\n", SearchedRecoverCnt, backupTime, fileSize );
                        }
                    }

                    travelFileNode = travelFileNode->nextFileNode;
                }

                if( SearchedRecoverCnt > 1 )
                {
                    // 복구할 파일 선택
                    selectNumber = 0;
                    selectNumber = Select_File( E_Type_Recover );
                    if( ( selectNumber < 0 ) || ( selectNumber > SearchedRecoverCnt ) )
                    {
                        printf( "Invalid number\n" );

                        return E_Invalid;
                    }
                    else if( selectNumber == 0 )
                    {
                        RecoverStop = true;

                        return E_Stop;
                    }
                }
                else if( SearchedRecoverCnt == 1 )
                {
                    selectNumber = 1;
                }

                f_Node = travelDirNode->subDirNode->fileNode;
                if( strcmp( f_Node->fileName, "" ) == 0 )
                {
                    if( f_Node->nextFileNode != NULL )
                        f_Node = f_Node->nextFileNode;
                }

                backupFileLen   = 0;
                fileNodeIndex   = 0;

                memset( compareFile, '\0', MAX_PATH_BUFFER_SIZE );

                while( f_Node != NULL )
                {
                    memset( compareFile, '\0', MAX_PATH_BUFFER_SIZE );

                    backupFileLen = strlen( f_Node->fileName );

                    strcpy( compareFile, f_Node->fileName );
                    compareFile[ backupFileLen - BACKUP_TIME_FORMAT_SIZE + 1 ] = '\0';

                    if( strcmp( backupFilePath, compareFile ) == 0 )
                    {
                        fileNodeIndex++;
                    }

                    if( selectNumber == fileNodeIndex )
                    {
                        // 파일 복구
                        File_Recover( travelDirNode->dirPath, f_Node->fileName, newRecoverPath );

                        break;
                    }

                    f_Node = f_Node->nextFileNode;
                }
            }


            return E_Stop;
        }

        
        if( travelDirNode != NULL & travelDirNode->subDirNode != NULL )
        {
            // 하위 디렉토리 재귀 탐색
            searchState = Recursive_Search( travelDirNode->subDirNode, backupDirPath, backupFilePath, newRecoverPath );
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


bool Directory_Recover( DirectoryNode *d_Node, char *backupDirPath, char *newRecoverPath )
{
    DirectoryNode   *travelDirNode      = NULL;

    FileNode        *travelFileNode     = NULL;
    FileNode        *f_Node             = NULL;

    E_Search        searchState         = E_Invalid;

    char            firstFileTime[ BACKUP_TIME_FORMAT_SIZE ]    = { '\0' };
    char            firstFileSize[ MAX_FILE_SIZE_BUFFER ]       = { '\0' };

    char            backupTime[ BACKUP_TIME_FORMAT_SIZE ]       = { '\0' };
    char            fileName[ MAX_FILE_NAME_SIZE ]              = { '\0' };
    char            filePath[ MAX_PATH_BUFFER_SIZE ]            = { '\0' };
    char            previousFileName[ MAX_PATH_BUFFER_SIZE ]    = { '\0' };
    char            nextFileName[ MAX_PATH_BUFFER_SIZE ]        = { '\0' };
    char            fileSize[ MAX_FILE_SIZE_BUFFER ]            = { '\0' };
    char            compareFile[ MAX_PATH_BUFFER_SIZE ]         = { '\0' };

    int             backupFileLen       = 0;

    int             fileNodeIndex       = 0;

    int             selectNumber        = -1;
    bool            selectState         = false;
    bool            nextIsSame          = false;

    bool            convertResult       = false;
    

    travelDirNode = d_Node;
    while( travelDirNode != NULL )
    {
        if( travelDirNode->subDirNode != NULL )
        {
            // 하위 디렉토리 탐색
            Directory_Recover( travelDirNode->subDirNode, travelDirNode->subDirNode->dirPath, newRecoverPath );
        }

        travelDirNode = travelDirNode->nextDirNode;
    }
    
    travelFileNode = d_Node->fileNode;

    SearchedRecoverCnt = 0;

    while( travelFileNode != NULL )
    {
        // 복구 대상 파일 경로 생성
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

        memset( fileSize, '\0', MAX_FILE_SIZE_BUFFER );

        if( IsFind == false )
        {
            IsFind = true;
        }
        
        // 파일 크기 출력 형식 변환
        convertResult = Convert_FileSize_Format( fileSize, filePath );
        if( convertResult == false )
        {
            return false;
        }
        
        memset( nextFileName, '\0', MAX_PATH_BUFFER_SIZE );
        if( travelFileNode->nextFileNode == NULL )
        {
            strcpy( nextFileName, "" );
        }
        else
        {
            backupFileLen = strlen( travelFileNode->nextFileNode->fileName );

            strcpy( nextFileName, travelFileNode->nextFileNode->fileName );
            nextFileName[ backupFileLen - BACKUP_TIME_FORMAT_SIZE + 1 ] = '\0';
        }

        selectState = false;
        nextIsSame  = false;
        if( strcmp( fileName, nextFileName ) == 0 )
        {
            SearchedRecoverCnt++;

            selectState = true;
            nextIsSame = true;
        }
        else
        {
            if( SearchedRecoverCnt >= 1 )
            {
                SearchedRecoverCnt++;

                selectState = true;
                nextIsSame = false;
            }
            else
            {
                SearchedRecoverCnt = 1;
            }
        }

        // 파일 백업 상태에 따른 파일 복구 방식
        if( SearchedRecoverCnt == 1 )
        {
            strcpy( firstFileTime, backupTime );
            strcpy( firstFileSize, fileSize );
        }
        else if( SearchedRecoverCnt == 2 )
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
        else if( SearchedRecoverCnt >= 3 )
        {
            printf( "%d. %s \t%sbytes\n", SearchedRecoverCnt, backupTime, fileSize );
        }

        if( ( selectState == true ) && ( nextIsSame == false ) && ( SearchedRecoverCnt > 1 ) )
        {
            // 복구할 파일 선택
            selectNumber = Select_File( E_Type_Recover );
            if( ( selectNumber < 0 ) || ( selectNumber > SearchedRecoverCnt ) )
            {
                printf( "Invalid number\n" );

                return false;
            }
            else if( selectNumber == 0 )
            {
                RecoverStop = true;

                return true;
            }

            fileNodeIndex = 0;
            f_Node = d_Node->fileNode;

            memset( compareFile, '\0', MAX_PATH_BUFFER_SIZE );

            while( f_Node != NULL )
            {
                memset( compareFile, '\0', MAX_PATH_BUFFER_SIZE );

                backupFileLen = strlen( f_Node->fileName );

                strcpy( compareFile, f_Node->fileName );
                compareFile[ backupFileLen - BACKUP_TIME_FORMAT_SIZE + 1 ] = '\0';

                if( strcmp( fileName, compareFile ) == 0 )
                {
                    fileNodeIndex++;
                }

                if( selectNumber == fileNodeIndex )
                {
                    // 파일 복구
                    File_Recover( d_Node->dirPath, f_Node->fileName, newRecoverPath );

                    break;
                }

                f_Node = f_Node->nextFileNode;
            }

            SearchedRecoverCnt = 0;
        }
        else if( ( selectState == false ) && ( nextIsSame == false ) && ( SearchedRecoverCnt == 1 ) )
        {
            selectNumber = 1;

            // 파일 복구
            File_Recover( d_Node->dirPath, travelFileNode->fileName, newRecoverPath );

            SearchedRecoverCnt = 0;
        }

        travelFileNode = travelFileNode->nextFileNode;
    }


    return true;
}


bool File_Recover( char *backupDirPath, char *backupFilePath, char *newRecoverPath )
{
    FILE            *backupFile     = NULL;
    FILE            *recoverFile    = NULL;

    char            backupPath[ MAX_PATH_BUFFER_SIZE ]      = { '\0' };
    char            recoverPath[ MAX_PATH_BUFFER_SIZE ]     = { '\0' };
    char            mkPath[ MAX_PATH_BUFFER_SIZE ]          = { '\0' };
            
    char            *strPtr         = NULL;

    int             backupFileLen   = 0;

    int             newFileIndex    = 0;

    int             readCnt         = 0;
    int             writeCnt        = 0;

    int             closeResult     = -1;

    int             mkdirResult     = -1;
    int             mkdirErrno;

    bool            newFile         = false;

    E_MD5_State     md5Result       = E_MD5_Invalid;
    E_SHA1_State    sha1Result      = E_SHA1_Invalid;


    if( RecoverStop == true )
    {
        return true;
    }

    strcpy( backupPath, backupDirPath );
    
    if( ( Recover_Option == E_dn ) && ( Input_Type_Is_File == false ) )
    {
        // -n 옵션에 따라 새로운 복구 경로를 생성
        if( *( newRecoverPath + 0 ) != '/' )
        {
            strcpy( recoverPath, Current_Work_Directory_Path );
            strcat( recoverPath, "/" );
            strcat( recoverPath, newRecoverPath );
        }
        else
        {
            strcpy( recoverPath, newRecoverPath );
        }

        char tempBuffer[ MAX_PATH_BUFFER_SIZE ] = { '\0' };
        char currentBuffer[ MAX_PATH_BUFFER_SIZE ] = { '\0' };
        
        char *strPtr = NULL;

        strcpy( currentBuffer, Current_Work_Directory_Path );

        strcpy( tempBuffer, Backup_Directory_Path );

        strcat( tempBuffer, &currentBuffer[ strlen( HomeDirPath ) ] );

        memset( currentBuffer, '\0', MAX_PATH_BUFFER_SIZE );
        strcpy( currentBuffer, &backupPath[ strlen( tempBuffer ) + 1 ] );

        int i = 0;
        for( i = 0; i < strlen( currentBuffer ); i++ )
        {
            if( currentBuffer[ i ] == '/' )
            {
                break;
            }
        }

        strcat( recoverPath, &currentBuffer[ i ] );
    }
    else if( Recover_Option == E_n )
    {
        // -n 옵션에 따라 새로운 복구 경로를 생성
        if( *( newRecoverPath + 0 ) != '/' )
        {
            strcpy( recoverPath, Current_Work_Directory_Path );
            strcat( recoverPath, "/" );
            strcat( recoverPath, newRecoverPath );
        }
        else
        {
            strcpy( recoverPath, newRecoverPath );
        }

        for( int i = strlen( recoverPath ); i >= 0; i-- )
        {
            if( *( recoverPath + i ) == '/' )
            {
                newFileIndex = i;

                break;
            }
        }

        char tempBuffer[ MAX_PATH_BUFFER_SIZE ] = { '\0' };
        char currentBuffer[ MAX_PATH_BUFFER_SIZE ] = { '\0' };
        
        char *strPtr = NULL;

        strcpy( currentBuffer, Current_Work_Directory_Path );

        strcpy( tempBuffer, Backup_Directory_Path );

        strcat( tempBuffer, &currentBuffer[ strlen( HomeDirPath ) ] );

        memset( currentBuffer, '\0', MAX_PATH_BUFFER_SIZE );
        strcpy( currentBuffer, &backupPath[ strlen( tempBuffer ) + 1 ] );

        int i = 0;
        for( i = 0; i < strlen( currentBuffer ); i++ )
        {
            if( currentBuffer[ i ] == '/' )
            {
                break;
            }
        }

        strcat( recoverPath, &currentBuffer[ i ] );

        *( recoverPath + newFileIndex ) = '\0';
    }
    else
    {
        strcpy( recoverPath, HomeDirPath );
        strcat( recoverPath, &backupPath[ strlen( Backup_Directory_Path ) ] );
    }

    // 복구 경로에 디렉토리 생성
    mkdirResult     = mkdir( recoverPath, MKDIR_MODE );
    mkdirErrno      = errno;

    if( mkdirResult == -1 )
    {
        if( mkdirErrno != EEXIST )
        {
            // 복구 경로에 존재하지 않는 디렉토리가 다수일 경우 모두 생성
            memset( mkPath, '\0', MAX_PATH_BUFFER_SIZE );

            strcpy( mkPath, HomeDirPath );

            strPtr = strtok( &recoverPath[ strlen( HomeDirPath ) ], "/" );
            while( strPtr != NULL )
            {
                strcat( mkPath, "/" );
                strcat( mkPath, strPtr );

                mkdirResult = mkdir( mkPath, MKDIR_MODE );
                if( mkdirErrno == -1 )
                {
                    printf( "Fail mkdir in backup directory\n" );
                    printf( "Error : %s\n", strerror( mkdirErrno ) );

                    return false;
                }

                strPtr = strtok( NULL, "/" );
            }
        }
    }
    
    if( backupFilePath != NULL )
    {
        strcat( backupPath, "/" );
        strcat( backupPath, backupFilePath );
    }

    memset( recoverPath, '\0', MAX_PATH_BUFFER_SIZE );
    strcpy( recoverPath, HomeDirPath );
    strcat( recoverPath, &backupPath[ strlen( Backup_Directory_Path ) ] );

    backupFileLen = strlen( recoverPath );

    // -n 옵션에 따라 새로운 복구 경로를 생성
    if( Recover_Option == E_n )
    {
        if( *( newRecoverPath + 0 ) == '/' )
        {
            memset( recoverPath, '\0', MAX_PATH_BUFFER_SIZE );

            if( *( newRecoverPath + 0 ) != '/' )
            {
                strcpy( recoverPath, Current_Work_Directory_Path );
                strcat( recoverPath, "/" );
                strcat( recoverPath, newRecoverPath );
            }
            else
            {
                strcpy( recoverPath, newRecoverPath );
            }

            char tempBuffer[ MAX_PATH_BUFFER_SIZE ] = { '\0' };
            char currentBuffer[ MAX_PATH_BUFFER_SIZE ] = { '\0' };
                
            char *strPtr = NULL;

            strcpy( currentBuffer, Current_Work_Directory_Path );

            strcpy( tempBuffer, Backup_Directory_Path );

            strcat( tempBuffer, &currentBuffer[ strlen( HomeDirPath ) ] );

            memset( currentBuffer, '\0', MAX_PATH_BUFFER_SIZE );
            strcpy( currentBuffer, &backupPath[ strlen( tempBuffer ) + 1 ] );

            int i = 0;
            for( i = 0; i < strlen( currentBuffer ); i++ )
            {
                if( currentBuffer[ i ] == '/' )
                {
                    break;
                }
            }

            strcat( recoverPath, &currentBuffer[ i ] );
            
            for( int i = strlen( newRecoverPath ); i >= 0; i-- )
            {
                if( *( newRecoverPath + i ) == '/' )
                {
                    *( recoverPath + newFileIndex ) = '\0';

                    strcat( recoverPath, &newRecoverPath[ i ] );

                    break;
                }
            }
        }
        else
        {
            recoverPath[ backupFileLen - strlen( backupFilePath ) - 1 ] = '\0';

            strcat( recoverPath, "/" );
            strcat( recoverPath, newRecoverPath );
        }
    }
    else
    {
        // -n 옵션에 따라 새로운 복구 경로를 생성
        if( Recover_Option == E_dn )
        {
            if( Input_Type_Is_File == true )
            {
                recoverPath[ backupFileLen - strlen( backupFilePath ) - 1 ] = '\0';

                strcat( recoverPath, "/" );
                strcat( recoverPath, newRecoverPath );

                newFile = true;
            }
            else
            {
                memset( recoverPath, '\0', MAX_PATH_BUFFER_SIZE );

                if( *( newRecoverPath + 0 ) != '/' )
                {
                    strcpy( recoverPath, Current_Work_Directory_Path );
                    strcat( recoverPath, "/" );
                    strcat( recoverPath, newRecoverPath );
                }
                else
                {
                    strcpy( recoverPath, newRecoverPath );
                }

                char tempBuffer[ MAX_PATH_BUFFER_SIZE ] = { '\0' };
                char currentBuffer[ MAX_PATH_BUFFER_SIZE ] = { '\0' };
                
                char *strPtr = NULL;

                strcpy( currentBuffer, Current_Work_Directory_Path );

                strcpy( tempBuffer, Backup_Directory_Path );

                strcat( tempBuffer, &currentBuffer[ strlen( HomeDirPath ) ] );

                memset( currentBuffer, '\0', MAX_PATH_BUFFER_SIZE );
                strcpy( currentBuffer, &backupPath[ strlen( tempBuffer ) + 1 ] );

                int i = 0;
                for( i = 0; i < strlen( currentBuffer ); i++ )
                {
                    if( currentBuffer[ i ] == '/' )
                    {
                        break;
                    }
                }

                strcat( recoverPath, &currentBuffer[ i ] );
            }
        }

        if( newFile == false )
        {
            backupFileLen = strlen( recoverPath );

            recoverPath[ backupFileLen - BACKUP_TIME_FORMAT_SIZE + 1 ] = '\0';
        }
    }

    // md5 또는 sha1 해시 함수로 해시 값 비교
    if( HashType == E_MD5 )
    {
        md5Result = Check_MD5( backupPath, recoverPath );
        if( md5Result == E_MD5_Invalid )
        {
            return false;
        }
        
        if( md5Result == E_MD5_Same )
        {
            printf( "\"%s\" is already recovered\n", backupPath );

            return true;
        }
    }
    else if( HashType == E_SHA1 )
    {
        sha1Result = Check_SHA1( backupPath, recoverPath );
        if( sha1Result == E_SHA1_Invalid )
        {
            return false;
        }
        
        if( sha1Result == E_SHA1_Same )
        {
            printf( "\"%s\" is already recovered\n", backupPath );

            return true;
        }
    }
    else if( HashType == E_Invalid_Hash )
    {
        return false;
    }
    

    backupFile = fopen( backupPath, "rb" );
    if( backupFile == NULL )
    {
        return false;
    }

    recoverFile = fopen( recoverPath, "wb" );
    if( recoverFile == NULL )
    {
        return false;
    }
    
    // 실질적인 파일 백업 진행
    while( feof( backupFile ) == 0 )
    {
        char    fileData;


        readCnt = 0;
        readCnt = fread( &fileData, sizeof( char ), FILE_RD_WR_BUFFER_SIZE, backupFile );
        if( readCnt != FILE_RD_WR_BUFFER_SIZE )
        {
            if( feof( backupFile ) != 0 )
            {
                break;
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

            if( recoverFile == NULL )
            {
                return false;
            }

            closeResult = fclose( recoverFile );
            if( closeResult != 0 )
            {
                return false;
            }

            return false;
        }

        writeCnt = fwrite( &fileData, sizeof( char ), FILE_RD_WR_BUFFER_SIZE, recoverFile );
        if( writeCnt != FILE_RD_WR_BUFFER_SIZE )
        {
            if( backupFile == NULL )
            {
                return false;
            }

            closeResult = fclose( backupFile );
            if( closeResult != 0 )
            {
                return false;
            }

            if( recoverFile == NULL )
            {
                return false;
            }

            closeResult = fclose( recoverFile );
            if( closeResult != 0 )
            {
                return false;
            }

            return false;
        }
    }

    if( InvalidRecoverPath == true )
    {
        printf( "\"%s\" backup file recover\n", backupPath );
    }
    else
    {
        printf( "\"%s\" backup recover to \"%s\"\n", backupPath, recoverPath );
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

    if( recoverFile == NULL )
    {
        return false;
    }

    closeResult = fclose( recoverFile );
    if( closeResult != 0 )
    {
        return false;
    }


    return true;
}


void Print_Usage()
{
    printf( "Usage : recover <FILENAME> [OPTION]\n" );
    printf( "  -d : recover directory recursive\n" );
    printf( "  -n <NEWNAME> : recover file with new name\n" );


    return;
}