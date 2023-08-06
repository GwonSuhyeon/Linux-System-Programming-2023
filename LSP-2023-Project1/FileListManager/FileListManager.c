

#include "FileListManager.h"


FileListNode    *FileList       = NULL;
FileListNode    *BackupList     = NULL;


bool FileListManager( char *path, E_ListType listType )
{
    FileListNode    *list   = NULL;

    bool            generateResult  = false;


    if( path == NULL )
    {
        return false;
    }

    if( ( listType == E_FileList ) || ( listType == E_RemoveFileList ) || ( listType == E_RecoverFileList ) )
    {
        if( FileList != NULL )
        {
            return false;
        }

        list = malloc( sizeof( FileListNode ) );
        if( list == NULL )
        {
            return false;
        }

        FileList = list;
    }
    else if ( listType == E_BackupList )
    {
        if( BackupList != NULL )
        {
            return false;
        }

        list = malloc( sizeof( FileListNode ) );
        if( list == NULL )
        {
            return false;
        }

        BackupList = list;
    }

    list->FLAG_NODE_IS_EMPTY = true;

    // 링크드리스트 생성
    generateResult = Generate_FileList( path, list, listType );
    if( generateResult == false )
    {
        return false;
    }


    return true;
}


bool Generate_FileList( char *path, FileListNode *list, E_ListType listType )
{
    bool    memsetResult    = false;
    bool    scanResult      = false;


    if( path == NULL )
    {
        return false;
    }

    list->FLAG_NODE_IS_EMPTY = true;

    // 디렉토리 스캔
    scanResult = Scan_Directory( path, list, listType );
    if( scanResult == false )
    {
        return false;
    }

    
    return true;
}


bool Scan_Directory( char *path, FileListNode *list, E_ListType listType )
{
    char            absolutePath[ MAX_PATH_BUFFER_SIZE ]    = { '\0' };
    char            fileName[ MAX_FILE_NAME_SIZE ]          = { '\0' };

    struct stat     fileStat;
    int             statResult      = -1;

    char            *strPtr         = NULL;

    int             scanCnt         = -1;
    bool            scanResult      = false;


    if( path == NULL )
    {
        return false;
    }

    list->currentDirNode = malloc( sizeof( DirectoryNode ) );
    list->currentDirNode->nextDirNode = NULL;

    realpath( path, absolutePath );

    statResult = stat( absolutePath, &fileStat );
    if( statResult == -1 )
    {
        if( ( listType == E_RemoveFileList ) || ( listType == E_RecoverFileList ) )
        {
            return true;
        }

        return false;
    }

    switch( fileStat.st_mode & S_IFMT )
    {
        // 입력 경로가 파일인 경우
        case S_IFREG:
        {
            for( int i = ( strlen( absolutePath ) - 1 ); i >= 0; i-- )
            {
                if( *( absolutePath + i ) == '/' )
                {
                    *( absolutePath + i ) = '\0';

                    break;
                }
            }

            strcpy( list->currentDirNode->dirPath, absolutePath );

            list->currentDirNode->fileNode = malloc( sizeof( FileNode ) );
            list->currentDirNode->fileNode->nextFileNode = NULL;
            
            strPtr = strtok( path, "/" );
            while( strPtr != NULL )
            {
                strcpy( list->currentDirNode->fileNode->fileName, strPtr );

                strPtr = strtok( NULL, "/" );
            }
        }
        break;

        // 입력 경로가 디렉토리인 경우        
        case S_IFDIR:
        {
            list->currentDirNode->fileNode = malloc( sizeof( FileNode ) );
            list->currentDirNode->fileNode->nextFileNode = NULL;
            memset( list->currentDirNode->fileNode->fileName, '\0', MAX_FILE_NAME_SIZE );

            // 디렉토리 전체를 탐색하기 위한 재귀 호출
            scanResult = Recursive_Scan( list->currentDirNode, absolutePath, listType );
            if( scanResult == false )
            {
                return false;
            }
        }
        break;


        default:
        {
            return false;
        }
    }

#ifdef FILE_LIST_PRINT
    Print_FileList( list->currentDirNode, E_ALL );
#endif

    
    return true;
}


bool Recursive_Scan( DirectoryNode *dirNode, char *path, E_ListType listType )
{
    DirectoryNode   *travelDirNode;

    struct dirent   **namelist;
    struct stat     fileStat;

    char            currentPath[ MAX_PATH_BUFFER_SIZE ] = { '\0' };

    int             scanCnt     = -1;
    int             statResult  = -1;

    bool            scanResult  = false;


    if( path == NULL )
    {
        return false;
    }

    travelDirNode = dirNode;
    strcpy( travelDirNode->dirPath, path );

    // 디렉토리 스캔
    scanCnt = scandir( travelDirNode->dirPath, &namelist, Directory_Filter, alphasort );
    if( scanCnt < 0 )
    {
        return false;
    }

    for( int i = 0; i < scanCnt; i++ )
    {
        travelDirNode = dirNode;

        memset( currentPath, '\0', MAX_PATH_BUFFER_SIZE );
        strcpy( currentPath, travelDirNode->dirPath );
        strcat( currentPath, "/" );
        strcat( currentPath, ( *( namelist + i ) )->d_name );

        statResult = stat( currentPath, &fileStat );
        if( statResult == -1 )
        {
            return false;
        }

        switch( fileStat.st_mode & S_IFMT )
        {
            // 파일
            case S_IFREG:
            {
                FileNode        *travelFileNode = NULL;
                FileNode        *allocNode = NULL;


                travelFileNode = dirNode->fileNode;

                if( travelFileNode == NULL )
                {
                    dirNode->fileNode = malloc( sizeof( FileNode ) );
                    dirNode->fileNode->nextFileNode = NULL;
                    memset(dirNode->fileNode->fileName, '\0', MAX_FILE_NAME_SIZE);
                }
                else
                {
                    if( strcmp( travelFileNode->fileName, "" ) != 0 )
                    {
                        while( travelFileNode->nextFileNode != NULL )
                        {
                            travelFileNode = travelFileNode->nextFileNode;
                        }
                        
                        allocNode = malloc( sizeof( FileNode ) );
                        if( allocNode == NULL )
                        {
                            printf("allocNode is null\n");

                            return false;
                        }
                        allocNode->nextFileNode = NULL;

                        travelFileNode->nextFileNode = allocNode;
                        memset( travelFileNode->nextFileNode->fileName, '\0', MAX_FILE_NAME_SIZE );

                        travelFileNode = travelFileNode->nextFileNode;
                    }
                }

                strcpy( travelFileNode->fileName, ( *( namelist + i ) )->d_name );
            }
            break;

            // 디렉토리
            case S_IFDIR:
            {
                struct dirent   **subDirNameList;
                int             subDirScanCnt = 0;

                // 접근 권한 확인
                if( access( currentPath, R_OK ) == -1 )
                {
                    printf( "FileListManager : Non Permission - Can't \"%s\" access\n", currentPath );
                }
                else
                {
                    // 디렉토리 스캔
                    subDirScanCnt = scandir( currentPath, &subDirNameList, Directory_Filter, alphasort );
                    if( subDirScanCnt < 1 )
                    {
                        if( listType == E_BackupList )
                        {
                            while( travelDirNode->nextDirNode != NULL )
                            {
                                travelDirNode = travelDirNode->nextDirNode;
                            }

                            travelDirNode->nextDirNode = malloc( sizeof( DirectoryNode ) );
                            travelDirNode->nextDirNode->nextDirNode = NULL;

                            strcpy( travelDirNode->nextDirNode->dirPath, currentPath );
                        }

                        break;
                    }


                    while( travelDirNode->nextDirNode != NULL )
                    {
                        travelDirNode = travelDirNode->nextDirNode;
                    }

                    travelDirNode->nextDirNode = malloc( sizeof( DirectoryNode ) );
                    travelDirNode->nextDirNode->nextDirNode = NULL;

                    strcpy( travelDirNode->nextDirNode->dirPath, currentPath );
                }
            }
            break;
        }
    }

    travelDirNode = dirNode;
    while( travelDirNode->nextDirNode != NULL )
    {
        travelDirNode = travelDirNode->nextDirNode;

        travelDirNode->subDirNode = malloc( sizeof( DirectoryNode ) );
        travelDirNode->subDirNode->nextDirNode = NULL;

        travelDirNode->subDirNode->fileNode = malloc( sizeof( FileNode ) );
        travelDirNode->subDirNode->fileNode->nextFileNode = NULL;
        memset( travelDirNode->subDirNode->fileNode->fileName, '\0', MAX_FILE_NAME_SIZE );

        // 재귀 탐색
        scanResult = Recursive_Scan( travelDirNode->subDirNode, travelDirNode->dirPath, listType );
        if( scanResult == false )
        {
            return false;
        }
    }


    return true;
}


int Directory_Filter( const struct dirent *item )
{
    // .과 .. 디렉토리 제외
    if( strcmp( "..", item->d_name ) == 0 )
    {
        return 0;
    }

    if( strcmp( ".", item->d_name ) == 0 )
    {
        return 0;
    }


    return 1;
}


void Delete_FileList( DirectoryNode *dirNode )
{
    DirectoryNode   *travelDirNode;
    DirectoryNode   *travelSubDirNode;
    DirectoryNode   *deleteDirNode;

    FileNode        *travelFileNode;
    FileNode        *deleteFileNode;


    if( dirNode == NULL )
    {
        return;
    }
    
    Delete_FileList( dirNode->subDirNode );

    travelDirNode = dirNode->nextDirNode;
    while( travelDirNode != NULL )
    {
        deleteDirNode = travelDirNode;
        travelDirNode = travelDirNode->nextDirNode;

        free( deleteDirNode );
    }

    travelSubDirNode = dirNode->subDirNode;
    while( travelSubDirNode != NULL )
    {
        deleteDirNode = travelSubDirNode;
        travelSubDirNode = travelSubDirNode->nextDirNode;

        free( deleteDirNode );
    }

    travelFileNode = dirNode->fileNode;
    while( travelFileNode != NULL )
    {
        deleteFileNode = travelFileNode;
        travelFileNode = travelFileNode->nextFileNode;

        free( deleteFileNode );
    }

    free( dirNode );


    return;
}


void Print_FileList( DirectoryNode *dirNode, E_Print_Type printType )
{
    FileNode        *f_Node;
    DirectoryNode   *d_Node;

    char            path[ MAX_PATH_BUFFER_SIZE ] = { '\0' };


    d_Node = dirNode;
    while( d_Node != NULL )
    {
        if( ( printType == E_ALL ) || ( printType == E_Dir ) )
        {
            printf( "Dir : %s\n", d_Node->dirPath );
        }

        d_Node = d_Node->nextDirNode;
    }

    if( dirNode->fileNode != NULL )
    {
        f_Node = dirNode->fileNode;
    }
    else
    {
        return;
    }
    
    if( strcmp( f_Node->fileName, "" ) == 0 )
    {
        if( f_Node->nextFileNode != NULL )
            f_Node = f_Node->nextFileNode;
    }

    while( f_Node != NULL )
    {
        memset( path, '\0', MAX_PATH_BUFFER_SIZE );
        
        strcpy( path, dirNode->dirPath );
        strcat( path, "/" );
        strcat( path, f_Node->fileName );

        if( ( printType == E_ALL ) || ( printType == E_File ) )
        {
            printf( "File : %s\n", path );
        }
        // else if( printType == E_Remove )
        // {
        // }
        // else if( printType == E_Recover )
        // {
        // }

        f_Node = f_Node->nextFileNode;
    }

    if( dirNode->nextDirNode == NULL )
    {
        return;
    }

    d_Node = dirNode->nextDirNode;
    while( d_Node != NULL )
    {
        Print_FileList( d_Node->subDirNode, printType );

        if( d_Node->nextDirNode == NULL )
        {
            break;
        }

        d_Node = d_Node->nextDirNode;
    }

    
    return;
}