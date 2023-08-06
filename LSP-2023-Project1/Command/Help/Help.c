

#include "Help.h"


int main( int argc, char *argv[] )
{
    Print_Help_Usage();

    
    return 0;
}


void Print_Help_Usage()
{
    printf( "Usage:\n" );
    printf( " > add  [FILENAME] [OPTION]\n" );
    printf( "   -d : add directory recursive\n" );
    printf( " > remove [FILENAME] [OPTION]\n" );
    printf( "   -a : remove all file(recursive)\n" );
    printf( "   -c : clear backup directory\n" );
    printf( " > recover [FILENAME] [OPTION]\n" );
    printf( "   -d : recover directory recursive\n" );
    printf( "   -n [NEWNAME] : recover file with new name\n" );
    printf( " > ls\n" );
    printf( " > vi\n" );
    printf( " > vim\n" );
    printf( " > help\n" );
    printf( " > exit\n" );


    return;
}