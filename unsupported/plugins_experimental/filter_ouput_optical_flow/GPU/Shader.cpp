#include "Shader.h"
#include <fstream>
#include <string>
#include <iostream>




bool GPU::Shader::PgObject::Allocate()
{
    m_Id = glCreateShaderObjectARB( m_PgType );
    return m_Id != 0;
}


bool GPU::Shader::PgObject::Unallocate()
{
    glDeleteObjectARB( m_Id );
    m_Id = 0;
    return true;
}


bool GPU::Shader::PgObject::CompileSrcFile( const std::string &filename,
                                            const GLenum pgType,
                                            std::string *logs )
{
    // Open the source files.
	std::ifstream file( filename.c_str(), std::ios::binary );
    if( file.fail() )
        return false;

	// Map the content of the shader source file into a memory buffer.
    file.seekg( 0, std::ios::end );
	int fileSize = (int) file.tellg();
	file.seekg( 0, std::ios::beg );

	char *sourceString = new char [ fileSize+1 ];
	file.read( sourceString, fileSize );
	sourceString[ fileSize ] = '\0';

    file.close();

	// Create the shader object.
    bool ok = CompileSrcString( sourceString, pgType, logs );

    // Termination.
    delete [] sourceString;
    return ok;
}


bool GPU::Shader::PgObject::CompileSrcString( const char *sourceString,
                                              const GLenum pgType,
                                              std::string *logs )
{
    // Ask for a free shader object name to OpenGL.
    m_PgType = pgType;
    if( Instantiate() )
    {
        // Create the shader object.
	    glShaderSourceARB ( m_Id, 1, (const GLcharARB**) &sourceString, NULL );
	    glCompileShaderARB( m_Id );

        // Check the compilation status.
	    GLint status;
        glGetObjectParameterivARB( m_Id, GL_OBJECT_COMPILE_STATUS_ARB, &status );

        if( status )
            return true;
        else if( logs )
            *logs = GPU::Shader::GetLogs( m_Id );

        Release();
    }

    return false;
}




std::string GPU::Shader::GetLogs( GLuint pgId )
{
    GLint logLength;
    glGetObjectParameterivARB( pgId, GL_OBJECT_INFO_LOG_LENGTH_ARB, &logLength );

    std::string infoLog;
    infoLog.resize( logLength );
    glGetInfoLogARB( pgId, logLength, &logLength, &infoLog[0] );

    return infoLog;
}


void GPU::Shader::RecoverActiveUniforms()
{
    m_Uniforms.clear();
    m_Samplers.clear();

	GLint count;
	GLcharARB nameBuffer[512];

    glGetObjectParameterivARB( m_Id, GL_OBJECT_ACTIVE_UNIFORMS_ARB, &count );

	for( GLint i=0; i<count; ++i )
	{
		GLsizei length;
        GLint   size;
        GLenum  type;

		glGetActiveUniformARB(
            m_Id,
            i,
            sizeof(nameBuffer),
            &length,
            &size,
            &type,
            nameBuffer
            );

		std::string name( nameBuffer );

        if( type<GL_SAMPLER_1D_ARB || type>GL_SAMPLER_2D_RECT_SHADOW_ARB )
        {
            m_Uniforms[name].location = glGetUniformLocationARB( m_Id, nameBuffer );
		    m_Uniforms[name].size = size;
		    m_Uniforms[name].type = type;
        }
        else
            m_Samplers[name] = glGetUniformLocationARB( m_Id, nameBuffer );
    }
}


void GPU::Shader::RecoverActiveAttributes()
{
    m_Attributes.clear();
    
    GLint count;
	GLcharARB nameBuffer[512];

	glGetObjectParameterivARB( m_Id, GL_OBJECT_ACTIVE_ATTRIBUTES_ARB, &count );

	for( GLint i=0; i<count; ++i )
	{
		GLsizei length;
        GLint   size;
        GLenum  type;

		glGetActiveAttribARB(
            m_Id,
            i,
            sizeof(nameBuffer),
            &length,
            &size,
            &type,
            nameBuffer
            );

		GLint attribId = glGetAttribLocationARB( m_Id, nameBuffer );
		if( attribId != -1 )
            m_Attributes[ std::string(nameBuffer) ] = attribId;
	}
}


bool GPU::Shader::Link( std::string *logs )
{
    if( !m_Id && !(m_Id = glCreateProgramObjectARB()) )
        return false;


    if( m_VertPg.IsInstantiated() )
        glAttachObjectARB( m_Id, m_VertPg.Id() );

    if( m_FragPg.IsInstantiated() )
        glAttachObjectARB( m_Id, m_FragPg.Id() );

    if( m_GeomPg.IsInstantiated() )
        glAttachObjectARB( m_Id, m_GeomPg.Id() );

    glLinkProgramARB( m_Id );

    GLint status;
    glGetObjectParameterivARB( m_Id, GL_OBJECT_LINK_STATUS_ARB, &status );
    if( !status )
    {
        if( logs )
            *logs = GetLogs( m_Id );

        glDeleteObjectARB( m_Id );
        m_Id = 0;

        return false;
    }


    RecoverActiveUniforms();
    RecoverActiveAttributes();

    return true;
}



GPU::Shader& GPU::Shader::Attach( const PgObject &pg )
{
    PgObject *dst = NULL;

    switch( pg.Type() )
    {
        case GL_VERTEX_SHADER_ARB:   dst = &m_VertPg; break;
        case GL_FRAGMENT_SHADER_ARB: dst = &m_FragPg; break;
        case GL_GEOMETRY_SHADER_EXT: dst = &m_GeomPg; break;
    }

    if( dst )
    {
        if( m_Id && dst->IsInstantiated() )
            glDetachObjectARB( m_Id, dst->Id() );
        *dst = pg;
    }

    return *this;
}


GPU::Shader& GPU::Shader::DetachVertPg()
{
    if( m_Id && m_VertPg.IsInstantiated() )
        glDetachObjectARB( m_Id, m_VertPg.Id() );
    m_VertPg.Release();
    return *this;
}


GPU::Shader& GPU::Shader::DetachFragPg()
{
    if( m_Id && m_FragPg.IsInstantiated() )
        glDetachObjectARB( m_Id, m_FragPg.Id() );
    m_FragPg.Release();
    return *this;
}


GPU::Shader& GPU::Shader::DetachGeomPg()
{
    if( m_Id && m_GeomPg.IsInstantiated() )
        glDetachObjectARB( m_Id, m_GeomPg.Id() );
    m_GeomPg.Release();
    return *this;
}


/*
bool GPU::Shader::CreateFromFiles( const std::string &vshFilename,
                                   const std::string &fshFilename )
{
    VertPg vsh;
    FragPg fsh;

    return vsh.LoadFile( vshFilename ) &&
           fsh.LoadFile( fshFilename ) &&
           Create( vsh, fsh );
}


bool GPU::Shader::CreateFromSources( const char *vshSourceStr,
                                     const char *fshSourceStr )
{
    VertPg vsh;
    FragPg fsh;

    return vsh.LoadSource( vshSourceStr ) &&
           fsh.LoadSource( fshSourceStr ) &&
           Create( vsh, fsh );
}
*/


void GPU::Shader::Release()
{
    if( m_Id )
    {
        if( m_VertPg.Id() )
        {
            glDetachObjectARB( m_Id, m_VertPg.Id() );
            m_VertPg.Release();
        }
        if( m_FragPg.Id() )
        {
            glDetachObjectARB( m_Id, m_FragPg.Id() );
            m_FragPg.Release();
        }
        if( m_GeomPg.Id() )
        {
            glDetachObjectARB( m_Id, m_GeomPg.Id() );
            m_GeomPg.Release();
        }

        glDeleteObjectARB( m_Id );
        m_Id = 0;

        m_Uniforms.clear();
        m_Attributes.clear();
    }
}


void GPU::Shader::SetUniform( const std::string& name,
                              const void *value )
{
    UniformMap::iterator uu = m_Uniforms.find( name );
    if( uu != m_Uniforms.end() )
    {
	    GLuint activeProgBackup = glGetHandleARB( GL_PROGRAM_OBJECT_ARB );
	    glUseProgramObjectARB( m_Id );

        Uniform &u = uu->second;
	    switch( u.type )
	    {
		    case GL_FLOAT:          glUniform1fvARB( u.location, u.size, (GLfloat*) value ); break;
		    case GL_FLOAT_VEC2_ARB: glUniform2fvARB( u.location, u.size, (GLfloat*) value ); break;
		    case GL_FLOAT_VEC3_ARB: glUniform3fvARB( u.location, u.size, (GLfloat*) value ); break;
		    case GL_FLOAT_VEC4_ARB: glUniform4fvARB( u.location, u.size, (GLfloat*) value ); break;
		    case GL_INT:            glUniform1ivARB( u.location, u.size, (GLint*) value ); break;
		    case GL_INT_VEC2_ARB:   glUniform2ivARB( u.location, u.size, (GLint*) value ); break;
		    case GL_INT_VEC3_ARB:   glUniform3ivARB( u.location, u.size, (GLint*) value ); break;
		    case GL_INT_VEC4_ARB:   glUniform4ivARB( u.location, u.size, (GLint*) value ); break;
		    case GL_BOOL_ARB:       glUniform1ivARB( u.location, u.size, (GLint*) value ); break;
		    case GL_BOOL_VEC2_ARB:  glUniform2ivARB( u.location, u.size, (GLint*) value ); break;
		    case GL_BOOL_VEC3_ARB:  glUniform3ivARB( u.location, u.size, (GLint*) value ); break;
		    case GL_BOOL_VEC4_ARB:  glUniform4ivARB( u.location, u.size, (GLint*) value ); break;
		    case GL_FLOAT_MAT2_ARB: glUniformMatrix2fvARB( u.location, u.size, GL_FALSE, (GLfloat*) value ); break;
		    case GL_FLOAT_MAT3_ARB: glUniformMatrix3fvARB( u.location, u.size, GL_FALSE, (GLfloat*) value ); break;
		    case GL_FLOAT_MAT4_ARB: glUniformMatrix4fvARB( u.location, u.size, GL_FALSE, (GLfloat*) value ); break;
	    }

	    glUseProgramObjectARB( activeProgBackup );
    }
}


void GPU::Shader::SetSampler( const std::string& name,
                              const GLint texUnit )
{
    SamplerMap::iterator s = m_Samplers.find( name );
    if( s != m_Samplers.end() )
    {
	    GLuint activeProgBackup = glGetHandleARB( GL_PROGRAM_OBJECT_ARB );
	    glUseProgramObjectARB( m_Id );

        glUniform1iARB( s->second, texUnit );

	    glUseProgramObjectARB( activeProgBackup );
    }
}
