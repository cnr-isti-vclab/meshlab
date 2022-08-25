#include "Texture.h"
#include "FrameBuffer.h"




bool GPU::Texture2D::Allocate()
{
    glGenTextures( 1, &m_Id );
    return m_Id != 0;
}


bool GPU::Texture2D::Unallocate()
{
    glDeleteTextures( 1, &m_Id );
    m_Id = 0;
    return true;
}


bool GPU::Texture2D::Create()
{
    if( Instantiate() )
    {
        glBindTexture( GL_TEXTURE_2D, m_Id );
        m_Width = m_Height = 0;
        return true;
    }
    else
        return false;
}


bool GPU::Texture2D::Create( GLint internalFormat,
                             GLsizei width,
                             GLsizei height,
                             GLint border,
                             GLenum format,
                             GLenum type,
                             const GLvoid* data )
{
    if( Instantiate() )
    {
        glBindTexture( GL_TEXTURE_2D, m_Id );
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            internalFormat,
            width,
            height,
            border,
            format,
            type,
            data
            );
        m_Width = width;
        m_Height = height;
        return true;
    }
    else
        return false;
}


bool GPU::Texture2D::Create( GLint internalFormat,
                             GLsizei width,
                             GLsizei height,
                             GLenum format,
                             GLenum type,
                             const GLvoid* data )
{
    if( Instantiate() )
    {
        glBindTexture( GL_TEXTURE_2D, m_Id );
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            internalFormat,
            width,
            height,
            0,
            format,
            type,
            data
            );
        m_Width = width;
        m_Height = height;
        return true;
    }
    else
        return false;
}


bool GPU::Texture2D::Create( GLint internalFormat,
                             GLsizei width,
                             GLsizei height )
{
    if( Instantiate() )
    {
        glBindTexture( GL_TEXTURE_2D, m_Id );
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            internalFormat,
            width,
            height,
            0,
            GL_RGB,
            GL_FLOAT,
            NULL
            );
        m_Width = width;
        m_Height = height;
        return true;
    }
    else
        return false;
}


void GPU::Texture2D::LoadMipMap( GLint level,
                                 GLint internalFormat,
                                 GLsizei width,
                                 GLsizei height,
                                 GLint border,
                                 GLenum format,
                                 GLenum type,
                                 const GLvoid* data )
{
    glBindTexture( GL_TEXTURE_2D, m_Id );
    glTexImage2D(
        GL_TEXTURE_2D,
        level,
        internalFormat,
        width,
        height,
        border,
        format,
        type,
        data
        );
    m_Width = width;
    m_Height = height;
}


void GPU::Texture2D::LoadMipMap( GLint level,
                                 GLint internalFormat,
                                 GLsizei width,
                                 GLsizei height,
                                 GLenum format,
                                 GLenum type,
                                 const GLvoid* data )
{
    glBindTexture( GL_TEXTURE_2D, m_Id );
    glTexImage2D(
        GL_TEXTURE_2D,
        level,
        internalFormat,
        width,
        height,
        0,
        format,
        type,
        data
        );
    m_Width = width;
    m_Height = height;
}


void GPU::Texture2D::LoadMipMap( GLint level,
                                 GLint internalFormat,
                                 GLsizei width,
                                 GLsizei height )
{
    glBindTexture( GL_TEXTURE_2D, m_Id );
    glTexImage2D(
        GL_TEXTURE_2D,
        level,
        internalFormat,
        width,
        height,
        0,
        GL_RGB,
        GL_FLOAT,
        NULL
        );
    m_Width = width;
    m_Height = height;
}


void GPU::Texture2D::Load( GLint internalFormat,
                           GLsizei width,
                           GLsizei height,
                           GLint border,
                           GLenum format,
                           GLenum type,
                           const GLvoid* data )
{
    glBindTexture( GL_TEXTURE_2D, m_Id );
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        internalFormat,
        width,
        height,
        border,
        format,
        type,
        data
        );
    m_Width = width;
    m_Height = height;
}


void GPU::Texture2D::Load( GLint internalFormat,
                           GLsizei width,
                           GLsizei height,
                           GLenum format,
                           GLenum type,
                           const GLvoid* data )
{
    glBindTexture( GL_TEXTURE_2D, m_Id );
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        internalFormat,
        width,
        height,
        0,
        format,
        type,
        data
        );
    m_Width = width;
    m_Height = height;
}


void GPU::Texture2D::Load( GLint internalFormat,
                           GLsizei width,
                           GLsizei height )
{
    glBindTexture( GL_TEXTURE_2D, m_Id );
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        internalFormat,
        width,
        height,
        0,
        GL_RGB,
        GL_FLOAT,
        NULL
        );
    m_Width = width;
    m_Height = height;
}


void GPU::Texture2D::Bind()
{
    glGetIntegerv( GL_ACTIVE_TEXTURE_ARB, &m_TexUnit );
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, m_Id );
}


void GPU::Texture2D::Bind( GLuint texUnit )
{
    m_TexUnit = GL_TEXTURE0_ARB + texUnit;
    glActiveTextureARB( m_TexUnit );
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, m_Id );
}


void GPU::Texture2D::Unbind() const
{
    glActiveTextureARB( m_TexUnit );
    glDisable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, 0 );
}


unsigned int GPU::Texture2D::RealWidth() const
{
    GLint width = 0;
    if( m_Id )
    {
        glPushAttrib( GL_TEXTURE_BIT );
        glBindTexture( GL_TEXTURE_2D, m_Id );
        glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width );
        glPopAttrib();
    }
    return width;
}


unsigned int GPU::Texture2D::RealHeight() const
{
    GLint height = 0;
    if( m_Id )
    {
        glPushAttrib( GL_TEXTURE_BIT );
        glBindTexture( GL_TEXTURE_2D, m_Id );
        glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height );
        glPopAttrib();
    }
    return height;
}


GLint GPU::Texture2D::InternalFormat() const
{
    GLint format = 0;
    if( m_Id )
    {
        glPushAttrib( GL_TEXTURE_BIT );
        glBindTexture( GL_TEXTURE_2D, m_Id );
        glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &format );
        glPopAttrib();
    }
    return format;
}


bool GPU::Texture2D::DumpTo( void *buffer,
                             const GLenum format,
                             const GLenum type,
                             const GLint level ) const
{
    if( m_Id )
    {
#if glGetTextureImageEXT    // Extension GL_EXT_direct_state_access is no supported by every common computer...
        glGetTextureImageEXT( m_Id, GL_TEXTURE_2D, level, format, type, buffer );
#else
        GPU::FrameBuffer fbuffer( m_Width, m_Height );
        fbuffer.Attach( GL_COLOR_ATTACHMENT0_EXT, this );
        fbuffer.DumpTo( GL_COLOR_ATTACHMENT0_EXT, buffer, format, type );
#endif
    }

    return m_Id != 0;
}


GPU::Texture2D GPU::Texture2D::Clone() const
{
    glPushAttrib( GL_TEXTURE_BIT );

    GPU::Texture2D newTex;
    newTex.Create();
    newTex.m_Width = Width();
    newTex.m_Height = Height();

    if( m_Id )
    {
        GPU::FrameBuffer fbuffer( Width(), Height() );
        fbuffer.Attach( GL_COLOR_ATTACHMENT0_EXT, this );
        fbuffer.Bind();

        newTex.Bind();
        glCopyTexImage2D( GL_TEXTURE_2D, 0, InternalFormat(), 0, 0, Width(), Height(), 0 );
        newTex.Unbind();

        fbuffer.Unbind();
    }

    glPopAttrib();

    return newTex;
}
