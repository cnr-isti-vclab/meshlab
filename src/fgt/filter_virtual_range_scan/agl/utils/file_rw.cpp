#include "file_rw.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace agl::utils;

bool file_rw::read_file( string filename, string& target )
{
    FILE *fp;
    char *content   = 0;
    int count       = 0;

    if( !filename.empty() )
    {
        fp = fopen( filename.c_str(), "rt" );

        if( fp )
        {
            fseek( fp, 0, SEEK_END );
            count = ftell( fp );
            rewind( fp );

            if( count > 0 )
            {
                content = (char*)malloc( sizeof( char ) * ( count + 1 ) );
                count = fread( content, sizeof( char ), count, fp );
                content[ count ] = '\0';
                target = content;
            }

            fclose( fp );
            return ( count > 0 );
        }
    }

    return false;
}

bool file_rw::write_file( string filename, string& content )
{
    FILE* fp;
    int status = 0;

    if( !filename.empty() )
    {
        fp = fopen( filename.c_str(), "w" );

        if( fp )
        {
            const char* s = content.c_str();
            unsigned int len = strlen( s );
            if( fwrite( s, sizeof( char ), len, fp ) == len )
            {
                status = 1;
            }
            fclose( fp );
        }
    }

    return status;
}
