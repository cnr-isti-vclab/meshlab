#include "shader_set.h"

#include <string.h>
#include <assert.h>
#include <Qt>

#include "../utils/file_rw.h"

using namespace agl::shaders;
using namespace agl::utils;

const int shader_set::VERTEX_SHADER = 0;
const int shader_set::FRAGMENT_SHADER = 1;
const int shader_set::GEOMETRY_SHADER = 2;

shader_set::shader_set( void )
{
    program = 0;
    ok      = false;
    sv      = 0;
}

shader_set::~shader_set( void )
{
    if( program > 0 )
    {
        glDeleteProgram( program );
    }
}

void shader_set::load( void )
{
    if( program > 0 )
    {
        glUseProgram( program );
        if( sv )
        {
            sv->set( program );
        }
    }
}

void shader_set::unload( void )
{
    if( program > 0 )
    {
        if( sv )
        {
            sv->unset( program );
        }
        glUseProgram( 0 );
    }
}

bool shader_set::is_ok( void )
{
    bool b = true;
    unsigned int i = 0;

    while( b && i<shaders.size() )
    {
        b = shaders.at(i++)->is_ok();
    }

    return (b && ok);
}

GLuint shader_set::program_id( void )
{
    return program;
}

void shader_set::set_variables( shader_variables *sv )
{
    this->sv = sv;
}

shader_set* shader_set::create_from_file
                          ( string v_file,
                            string f_file,
                            string g_file )
{
    string v_src;
    file_rw::read_file( v_file, v_src );

    string f_src;
    file_rw::read_file( f_file, f_src );

    string g_src;
    file_rw::read_file( g_file, g_src );

    return create_from_source( v_src, f_src, g_src );
}

shader_set* shader_set::create_from_file_fast
                          ( string filename )
{
   if( filename.empty() ) return 0;

   string v_file = filename + ".vert";
   string f_file = filename + ".frag";
   string g_file = filename + ".geom";

   return create_from_file( v_file, f_file, g_file );
}

shader_set* shader_set::create_from_source
                          ( string v_source,
                            string f_source,
                            string g_source )
{
    assert( !v_source.empty() );
    assert( !f_source.empty() );

    shader_set* ss = new shader_set();

    shader_object* vs = new shader_object( GL_VERTEX_SHADER, v_source );
    ss->shaders.push_back( vs );

    shader_object* fs = new shader_object( GL_FRAGMENT_SHADER, f_source );
    ss->shaders.push_back( fs );

    shader_object* gs = 0;

    /*
    if( !g_source.empty() )
    {
        gs = new shader_object( GL_GEOMETRY_SHADER, g_source );
        ss->shaders.push_back( gs );
    }
    */

    ss->program = glCreateProgram();
    glAttachShader( ss->program, vs->shader_id() );
    glAttachShader( ss->program, fs->shader_id() );

    if( gs )
    {
        glAttachShader( ss->program, gs->shader_id() );
    }

    glLinkProgram( ss->program );

    ss->log = "************ program log ***************\nlinking ";
    GLint ls = 0;
    glGetProgramiv( ss->program, GL_LINK_STATUS, &ls );
    ss->ok = ( ls != GL_FALSE );

    if( ss->ok )
    {
        ss->log += "succeeded\n";
    }
    else
    {
        ss->log += "failed\n";
    }

    GLint info_log_length = 0;
    glGetProgramiv( ss->program, GL_INFO_LOG_LENGTH, &info_log_length );

    if( info_log_length > 0 )
    {
        char* pl = new char[ info_log_length + 1 ];
        glGetProgramInfoLog( ss->program, info_log_length, &info_log_length, pl );

        if( info_log_length > 0 )
        {
            pl[ info_log_length - 1 ] = '\0';
            ss->log += pl;
        }

        delete[] pl;
    }
    else
    {
        ss->log += "no log";
    }
    ss->log += "\n**********************************\n";

    if( !ss->ok )
    {
        qDebug( ss->log.c_str() );
    }

    for( unsigned int i = 0; i < ss->shaders.size(); i++ )
    {
        if( !(ss->shaders[i]->is_ok()) )
        {
            qDebug( ss->shaders[i]->get_log().c_str() );
        }
    }

    return ss;
}

shader_set* shader_set::create_from_builder
        ( ShaderBuilder* builder )
{
    string vsSource = "", fsSource = "";

    builder->buildVSVariables();
    builder->buildVSFunctions();
    builder->buildVSBody();

    builder->buildFSVariables();
    builder->buildFSFunctions();
    builder->buildFSBody();

    builder->getShaderSource( vsSource, fsSource );

    return create_from_source( vsSource, fsSource, "" );
}
