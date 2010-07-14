#include "error_bin.h"

using namespace agl::utils;

error_bin* error_bin::instance = 0;

error_bin::error_bin( void )
{
    ;
}

error_bin::~error_bin( void )
{
    ;
}

error_bin* error_bin::get_instance( void )
{
    if( instance == 0 )
    {
        instance = new error_bin();
    }
    return instance;
}

void error_bin::clear( void )
{
    errors.clear();
}

void error_bin::check( void )
{
    GLenum err = glGetError();
    if( err == GL_NO_ERROR ) return;
    const GLubyte* err_str_gl = gluErrorString( err );
    const char* err_str_c = (const char*)err_str_gl;
    errors.push_back( error( err, string( err_str_c ) ) );
}

void error_bin::reset( void )
{
    glGetError();   // resets the error state variable
    clear();
}

bool error_bin::is_empty( void )
{
    return ( errors.size() == 0 );
}

string error_bin::get_errors( void )
{
    if( is_empty() ) return 0;

    string s =  "\n-------- OpenGL errors -------------\n\n";

    for( unsigned int i=0; i<errors.size(); i++ )
    {
        s += errors[i].second;
        s += "\n";
    }

    s += "\n------------------------------------\n\n";

    return s;
}

string error_bin::get_last( void )
{
    if( is_empty() ) return 0;
    return errors[ errors.size() - 1 ].second;
}

int error_bin::get_error_count ( void )
{
    return errors.size();
}

string error_bin::get_error_at( unsigned int index )
{
    if( index >= errors.size() ) return string( "null" );
    return errors[ index ].second;
}
