#include "shader_object.h"

#include <stdlib.h>
#include <assert.h>

using namespace agl::shaders;

map< GLenum, string > shader_object::shader_string;

shader_object::shader_object( GLenum type, const string &src )
{
    if( shader_string.size() == 0 )
    {
        initialize_map();
    }

    id = glCreateShader( type );
    const char* cc = src.c_str();
    glShaderSource( id, 1, &cc, NULL );
    glCompileShader( id );

    log += shader_string[ type ] + " shader log\n-----------------------\n";

    GLint cs = 0;
    glGetShaderiv( id, GL_COMPILE_STATUS, &cs );
    log += "compilation ";
    ok = ( cs != GL_FALSE );

    if( ok )
    {
        log += "succeeded\n";
    }
    else
    {
        log += "failed\n";
    }

    int info_log_length = 0;
    glGetShaderiv( id, GL_INFO_LOG_LENGTH, &info_log_length );

    if( info_log_length > 0 )
    {
        int chars_written = 0;
        char* info_log = new char[ info_log_length  + 1 ];
        glGetShaderInfoLog( id, info_log_length, &chars_written, info_log );
        if ( chars_written > 0 )
        {
            info_log[ chars_written - 1 ] = '\0';
            log += info_log;
        }
        delete[] info_log;
    }
    else
    {
        log += "no log";
    }
    log += "\n";

    log += "-----------------------\n";

    assert( ok );
}

shader_object::~shader_object( void )
{
    if( id > 0 )
    {
        glDeleteShader( id );
    }
}

bool shader_object::is_ok( void )
{
    return ok;
}

string shader_object::get_log( void )
{
    return log;
}

GLuint shader_object::shader_id( void )
{
    if( ok )
    {
        return id;
    }
    return 0;
}

void shader_object::initialize_map( void )
{
    shader_string[ GL_VERTEX_SHADER     ] = "vertex";
    shader_string[ GL_FRAGMENT_SHADER   ] = "fragment";
    //shader_string[ GL_GEOMETRY_SHADER   ] = "geometry";
}

























