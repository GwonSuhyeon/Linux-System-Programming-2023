

#include "Main.h"


int main( int argc, char* argv[] )
{
    char            pPromptInput[ MAX_PROMPT_BUFFER_SIZE ]    = { '\0' };
    char            **params                                = NULL;

    E_Command       Command             = E_invalid;

    bool            mkdirResult         = false;
    bool            runResult           = false;
    bool            getDirectoryResult  = false;
    bool            splitResult         = false;
    

    if( argc != 2 )
    {
        printf( "Usage: ssu_backup <md5 | sha1>\n" );
         
        return -1;
    }

    memset( SSU_BACKUP_PATH, '\0', MAX_PATH_BUFFER_SIZE );
    strcpy( SSU_BACKUP_PATH, argv[ 0 ] );

    if( ( strcmp( argv[ 1 ], "md5" ) != 0 ) && ( strcmp( argv[ 1 ], "sha1" ) != 0  ) )
    {
        printf( "Usage: ssu_backup <md5 | sha1>\n" );

        return -1;
    }

    if( strcmp( argv[ 1 ], "md5" ) == 0 )
    {
        HashType = E_MD5;
    }
    else if( strcmp( argv[ 1 ], "sha1" ) == 0 )
    {
        HashType = E_SHA1;
    }

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

    // 백업 디렉토리 생성
    mkdirResult = Make_Backup_Directory();
    if( mkdirResult == false )
    {
        return -1;
    }


    // 사용자의 프롬프트 입력
    fflush( stdin );
    while( true )
    {
        ExitState = false;

        Command = E_invalid;
        
        memset( pPromptInput, '\0', MAX_PROMPT_BUFFER_SIZE );

        if( params != NULL )
        {
            for( int i = 0; i < 10; i++ )
            {
                if( i == 0 )
                {
                    memset( *( params + i ), '\0', MAX_PATH_BUFFER_SIZE );
                }
                else
                {
                    memset( *( params + i ), '\0', MAX_OPTION_LEN );
                }
            }
        }
        
        printf( "%s> ", Student_Number );

        gets( pPromptInput );
        fflush( stdin );

        // 프롬프트 최대 길이 검사
        if( strlen( pPromptInput ) >= MAX_PROMPT_BUFFER_SIZE )
        {
            printf( "Too long prompt input\n" );

            continue;
        }

        if( pPromptInput[ 0 ] == '\0' )
        {
            continue;
        }
        
        // 명령어 획득
        Command = Get_Command( pPromptInput );

        // 명령어의 인자 획득
        splitResult = Split_Command_Params( pPromptInput, &params );
        if( splitResult == false )
        {
            printf( "Retry prompt input\n" );
            
            continue;
        }
        
        // 명령어 실행
        runResult = Run_Command( Command, params );
        if( runResult == false )
        {
            printf( "Fail to run %s\n", CommandList[ Command ] );

            return -1;
        }

        if( ExitState == true )
        {
            break;
        }
    }

    for( int i = 0; i < 10; i++ )
    {
        if( *( params + i ) != NULL )
        {
            free( *( params + i ) );

            *( params + i ) = NULL;
        }
    }

    if( params != NULL )
    {
        free( params );

        params = NULL;
    }

    
    return 0;
}


bool Get_HomeDirectory()
{
    char    resolvedPath[ MAX_PATH_BUFFER_SIZE ]    = { '\0' };
    char    homePath[ MAX_PATH_BUFFER_SIZE ]        = { '\0' };

    char    *realPathResult                         = NULL;

    char    *splitPtr                               = NULL;


    realPathResult = realpath( ".", resolvedPath );
    if( realPathResult == NULL )
    {
        printf( "Error : Invalid path\n" );

        return false;
    }
    realPathResult = NULL;
    
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
    memset( HomeDirPath, '\0', ( strlen( homePath ) + 1 ) );
    
    strcpy( HomeDirPath, homePath );

    splitPtr = NULL;


    return true;
}


bool Get_WorkDirectory()
{
    char    resolvedPath[ MAX_PATH_BUFFER_SIZE ]    = { '\0' };

    char    *realPathResult                         = NULL;


    realPathResult = realpath( ".", resolvedPath );
    if( realPathResult == NULL )
    {
        printf( "Error : Invalid path\n" );

        return false;
    }

    Current_Work_Directory_Path = malloc( sizeof( char ) * ( strlen( resolvedPath ) + 1 ) );
    strcpy( Current_Work_Directory_Path, resolvedPath );
    strcat( Current_Work_Directory_Path, "/" );


    return true;
}


bool Make_Backup_Directory()
{
    int     mkdirResult;
    int     mkdirErrno;


    Backup_Directory_Path = malloc( sizeof( char ) * ( strlen( HomeDirPath ) + strlen( BACKUP_DIRECTORY_NAME ) + 1 ) );
    if( Backup_Directory_Path == NULL )
    {
        printf( "Fail to get directory path\n" );

        return false;
    }

    strcpy( Backup_Directory_Path, HomeDirPath );
    strcat( Backup_Directory_Path, BACKUP_DIRECTORY_NAME );

    mkdirResult     = mkdir( Backup_Directory_Path, MKDIR_MODE );
    mkdirErrno      = errno;

    if( mkdirErrno == EEXIST )
    {
        return true;
    }

    if( mkdirResult == -1 )
    {
        printf( "Fail mkdir backup directory\n" );
        printf( "Error : %s\n", strerror( mkdirErrno ) );

        return false;
    }


    return true;
}


E_Command Get_Command( char *prompt )
{
    char        *pPromptCopy    = NULL;
    char        *splitPtr       = NULL;

    char        *memsetResult   = NULL;

    E_Command   command         = E_invalid;


    pPromptCopy = malloc( sizeof( char ) * MAX_PROMPT_BUFFER_SIZE );
    if( pPromptCopy == NULL )
    {
        return E_invalid;
    }

    memsetResult = NULL;
    memsetResult = memset( pPromptCopy, '\0', sizeof( char ) * MAX_PATH_BUFFER_SIZE );
    if( memsetResult == NULL )
    {
        return E_invalid;
    }

    strcpy( pPromptCopy, prompt );


    splitPtr = strtok( pPromptCopy, " " );
    if( splitPtr == NULL )
    {
        return E_invalid;
    }

    for( int i = 0; i < COMMAND_CNT; i++ )
    {
        if( strcmp( splitPtr, CommandList[ i ] ) == 0 )
        {
            command = i;

            break;
        }
    }

    if( pPromptCopy != NULL )
    {
        free( pPromptCopy );

        pPromptCopy = NULL;
    }


    return command;
}


bool Split_Command_Params( char *prompt, char ***params )
{
    char        *pPromptCopy    = NULL;
    char        *splitPtr       = NULL;

    char        *memsetResult   = NULL;

    int         allocSize       = 0;

    int         splitIndex      = 0;


    pPromptCopy = malloc( sizeof( char ) * ( strlen( prompt ) + 1 ) );
    if( pPromptCopy == NULL )
    {
        return false;
    }

    memsetResult = NULL;
    memsetResult = memset( pPromptCopy, '\0', sizeof( char ) * ( strlen( prompt ) + 1 ) );
    if( memsetResult == NULL )
    {
        return false;
    }

    if( *params == NULL )
    {
        *params = malloc( sizeof( char* ) * 10 );
        if( *params == NULL )
        {
            return false;
        }

        for( int i = 0; i < 10; i++ )
        {
            allocSize = sizeof( char ) * MAX_PATH_BUFFER_SIZE;

            *( *params + i ) = malloc( allocSize );

            if( *( *params + i ) == NULL )
            {
                return false;
            }

            memsetResult = NULL;
            memsetResult = memset( *( *params + i ), '\0', allocSize );
            if( memsetResult == NULL )
            {
                return false;
            }
        }
    }

    strcpy( pPromptCopy, prompt );

    splitPtr = strtok( pPromptCopy, " " );
    
    splitIndex = 0;
    if( HashType == E_MD5 )
    {
        strcpy( *( *params + splitIndex ), "md5" );
    }
    else if( HashType == E_SHA1 )
    {
        strcpy( *( *params + splitIndex ), "sha1" );
    }
    splitIndex++;
    
    strcpy( *( *params + splitIndex ), splitPtr );
    splitIndex++;

    while( true )
    {
        splitPtr = strtok( NULL, " " );
        if( splitPtr == NULL )
        {
            break;
        }

        if( strlen( splitPtr ) >= MAX_PATH_BUFFER_SIZE )
        {
            printf( "Too long prompt input\n" );

            return false;
        }

        strcpy( *( *params + splitIndex ), splitPtr );

        splitIndex++;
    }

    if( pPromptCopy != NULL )
    {
        free( pPromptCopy );

        pPromptCopy = NULL;
    }


    return true;
}


bool Run_Command( E_Command command, char **params )
{
    pid_t     pid             = -1;

    int         waitState       = 0;

    bool        executeResult   = true;


    switch( command )
    {
        case E_add:
        {
            // 자식 프로세스 생성
            pid = fork();
            if( pid == -1 )
            {
                return false;
            }

            if( Add_Command_Path == NULL )
            {
                // 내장 명령어의 실행 인자 준비
                Add_Command_Path = malloc( sizeof( char ) * ( strlen( Current_Work_Directory_Path ) + strlen( SSU_BACKUP_PATH ) + strlen( ADD_COMMAND_FILE_NAME ) + 1 ) );

                strcpy( Add_Command_Path, Current_Work_Directory_Path );
                strcat( Add_Command_Path, SSU_BACKUP_PATH );

                for( int i = strlen( Add_Command_Path ); i >= 0; i-- )
                {
                    if( *( Add_Command_Path + i ) == '/' )
                    {
                        *( Add_Command_Path + ( i + 1 ) ) = '\0';

                        strcat( Add_Command_Path, ADD_COMMAND_FILE_NAME );

                        break;
                    }
                }
            }

            executeResult = Execute_Child_Process( pid, Add_Command_Path, params );
            if( executeResult == false )
            {
                if( pid == 0 )
                {
                    exit( 0 );
                }
                else
                {
                    return false;
                }
            }
        }
        break;

        case E_remove:
        {
            pid = fork();
            if( pid == -1 )
            {
                return false;
            }

            if( Remove_Command_Path == NULL )
            {
                Remove_Command_Path = malloc( sizeof( char ) * ( strlen( Current_Work_Directory_Path ) + strlen( SSU_BACKUP_PATH ) + strlen( REMOVE_COMMAND_FILE_NAME ) + 1 ) );

                strcpy( Remove_Command_Path, Current_Work_Directory_Path );
                strcat( Remove_Command_Path, SSU_BACKUP_PATH );

                for( int i = strlen( Remove_Command_Path ); i >= 0; i-- )
                {
                    if( *( Remove_Command_Path + i ) == '/' )
                    {
                        *( Remove_Command_Path + ( i + 1 ) ) = '\0';

                        strcat( Remove_Command_Path, REMOVE_COMMAND_FILE_NAME );

                        break;
                    }
                }
            }

            executeResult = Execute_Child_Process( pid, Remove_Command_Path, params );
            if( executeResult == false )
            {
                if( pid == 0 )
                {
                    exit( 0 );
                }
                else
                {
                    return false;
                }
            }
        }
        break;

        case E_recover:
        {
            pid = fork();
            if( pid == -1 )
            {
                return false;
            }

            if( Recover_Command_Path == NULL )
            {
                Recover_Command_Path = malloc( sizeof( char ) * ( strlen( Current_Work_Directory_Path ) + strlen( SSU_BACKUP_PATH ) + strlen( RECOVER_COMMAND_FILE_NAME ) + 1 ) );
                
                strcpy( Recover_Command_Path, Current_Work_Directory_Path );
                strcat( Recover_Command_Path, SSU_BACKUP_PATH );

                for( int i = strlen( Recover_Command_Path ); i >= 0; i-- )
                {
                    if( *( Recover_Command_Path + i ) == '/' )
                    {
                        *( Recover_Command_Path + ( i + 1 ) ) = '\0';

                        strcat( Recover_Command_Path, RECOVER_COMMAND_FILE_NAME );

                        break;
                    }
                }
            }

            executeResult = Execute_Child_Process( pid, Recover_Command_Path, params );
            if( executeResult == false )
            {
                if( pid == 0 )
                {
                    exit( 0 );
                }
                else
                {
                    return false;
                }
            }
        }
        break;

        case E_ls:
        {
            pid = fork();
            if( pid == -1 )
            {
                return false;
            }

            memset( *( params + 1 ), '\0', MAX_PATH_BUFFER_SIZE );
            strcpy( *( params + 1 ), "/bin/ls" );
            
            char **ls_params = NULL;
            int size = 0;

            for( size = 0; size < 10; size++ )
            {
                if( strlen( *( params + size + 1 ) ) == 0 )
                {
                    break;
                }
            }

            ls_params = malloc( sizeof( char * ) * size );
            if( ls_params == NULL )
            {
                return false;
            }

            for( int i = 0; i <= size; i++ )
            {
                *( ls_params + i ) = malloc( sizeof( char ) * ( strlen( *( params+ i + 1 ) ) + 1 ) );
                if( *( ls_params + i ) == NULL )
                {
                    return false;
                }

                if( i < size )
                {
                    strcpy( *( ls_params + i ), *( params + i + 1 ) );
                }
                else
                {
                    *( ls_params + i ) = NULL;
                }
                
            }

            executeResult = Execute_Child_Process( pid, *( ls_params + 0 ), ls_params );
            if( executeResult == false )
            {
                if( pid == 0 )
                {
                    exit( 0 );
                }
                else
                {
                    return false;
                }
            }
        }
        break;

        case E_vi:
        {
            pid = fork();
            if( pid == -1 )
            {
                return false;
            }

            memset( *( params + 1 ), '\0', MAX_PATH_BUFFER_SIZE );
            strcpy( *( params + 1 ), "/bin/vi" );
            
            char **ls_params = NULL;
            int size = 0;

            for( size = 0; size < 10; size++ )
            {
                if( strlen( *( params + size + 1 ) ) == 0 )
                {
                    break;
                }
            }

            ls_params = malloc( sizeof( char * ) * size );
            if( ls_params == NULL )
            {
                return false;
            }

            for( int i = 0; i <= size; i++ )
            {
                *( ls_params + i ) = malloc( sizeof( char ) * ( strlen( *( params+ i + 1 ) ) + 1 ) );
                if( *( ls_params + i ) == NULL )
                {
                    return false;
                }

                if( i < size )
                {
                    strcpy( *( ls_params + i ), *( params + i + 1 ) );
                }
                else
                {
                    *( ls_params + i ) = NULL;
                }
                
            }

            executeResult = Execute_Child_Process( pid, *( ls_params + 0 ), ls_params );
            if( executeResult == false )
            {
                if( pid == 0 )
                {
                    exit( 0 );
                }
                else
                {
                    return false;
                }
            }
        }
        break;

        case E_vim:
        {
            pid = fork();
            if( pid == -1 )
            {
                return false;
            }

            memset( *( params + 1 ), '\0', MAX_PATH_BUFFER_SIZE );
            strcpy( *( params + 1 ), "/bin/vim" );
            
            char **ls_params = NULL;
            int size = 0;

            for( size = 0; size < 10; size++ )
            {
                if( strlen( *( params + size + 1 ) ) == 0 )
                {
                    break;
                }
            }

            ls_params = malloc( sizeof( char * ) * size );
            if( ls_params == NULL )
            {
                return false;
            }

            for( int i = 0; i <= size; i++ )
            {
                *( ls_params + i ) = malloc( sizeof( char ) * ( strlen( *( params+ i + 1 ) ) + 1 ) );
                if( *( ls_params + i ) == NULL )
                {
                    return false;
                }

                if( i < size )
                {
                    strcpy( *( ls_params + i ), *( params + i + 1 ) );
                }
                else
                {
                    *( ls_params + i ) = NULL;
                }
                
            }

            executeResult = Execute_Child_Process( pid, *( ls_params + 0 ), ls_params );
            if( executeResult == false )
            {
                if( pid == 0 )
                {
                    exit( 0 );
                }
                else
                {
                    return false;
                }
            }
        }
        break;

        case E_help:
        {
            pid = fork();
            if( pid == -1 )
            {
                return false;
            }

            if( Help_Command_Path == NULL )
            {
                Help_Command_Path = malloc( sizeof( char ) * ( strlen( Current_Work_Directory_Path ) + strlen( SSU_BACKUP_PATH ) + strlen( HELP_COMMAND_FILE_NAME ) + 1 ) );

                strcpy( Help_Command_Path, Current_Work_Directory_Path );
                strcat( Help_Command_Path, SSU_BACKUP_PATH );

                for( int i = strlen( Help_Command_Path ); i >= 0; i-- )
                {
                    if( *( Help_Command_Path + i ) == '/' )
                    {
                        *( Help_Command_Path + ( i + 1 ) ) = '\0';

                        strcat( Help_Command_Path, HELP_COMMAND_FILE_NAME );

                        break;
                    }
                }
            }

            executeResult = Execute_Child_Process( pid, Help_Command_Path, params );
            if( executeResult == false )
            {
                if( pid == 0 )
                {
                    exit( 0 );
                }
                else
                {
                    return false;
                }
            }
        }
        break;

        case E_exit:
        {
            ExitState = true;

            return true;
        }
        break;

        case E_invalid:
        {
            pid = fork();
            if( pid == -1 )
            {
                return false;
            }

            if( Help_Command_Path == NULL )
            {
                Help_Command_Path = malloc( sizeof( char ) * ( strlen( Current_Work_Directory_Path ) + strlen( SSU_BACKUP_PATH ) + strlen( HELP_COMMAND_FILE_NAME ) + 1 ) );

                strcpy( Help_Command_Path, Current_Work_Directory_Path );
                strcat( Help_Command_Path, SSU_BACKUP_PATH );

                for( int i = strlen( Help_Command_Path ); i >= 0; i-- )
                {
                    if( *( Help_Command_Path + i ) == '/' )
                    {
                        *( Help_Command_Path + ( i + 1 ) ) = '\0';

                        strcat( Help_Command_Path, HELP_COMMAND_FILE_NAME );

                        break;
                    }
                }
            }

            executeResult = Execute_Child_Process( pid, Help_Command_Path, params );
            if( executeResult == false )
            {
                if( pid == 0 )
                {
                    exit( 0 );
                }
                else
                {
                    return false;
                }
            }
        }
        break;
    }

    // 부모 프로세스가 자식 프로세스의 종료를 기다림
    if( pid > 0 )
    {
        waitpid( pid, &waitState, WAITING_PROCESS_TERMINATION );
    }


    return true;
}


bool Execute_Child_Process( pid_t pid, char *commandPath, char **params )
{
    int    execResult  = 0;


    if( pid == 0 )
    {
        // 자식 프로세스 execute
        execResult = execv( commandPath, params );
        
        if( execResult == -1 )
        {
            return false;
        }
    }
    else if ( pid == -1 )
    {
        return false;
    }


    return true;
}