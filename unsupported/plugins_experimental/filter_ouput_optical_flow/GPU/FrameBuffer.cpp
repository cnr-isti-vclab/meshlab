#include "FrameBuffer.h"




bool GPU::RenderBuffer::Allocate()
{
    glGenRenderbuffersEXT( 1, &m_Id );
    return m_Id != 0;
}


bool GPU::RenderBuffer::Unallocate()
{
    glDeleteRenderbuffersEXT( 1, &m_Id );
    m_Id = 0;
    return true;
}


bool GPU::RenderBuffer::Create( const GLuint format,
                                const unsigned int w,
                                const unsigned int h )
{
    if( w && h && Instantiate() )
    {
        m_Width  = w;
        m_Height = h;

        glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, m_Id );
        glRenderbufferStorageEXT( GL_RENDERBUFFER, format, w, h );

        return true;
    }

    return false;       
}


GLint GPU::RenderBuffer::InternalFormat() const
{
    GLint format = 0;
    if( m_Id )
    {
        glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, m_Id );
        glGetRenderbufferParameterivEXT( GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_INTERNAL_FORMAT, &format );
    }
    return format;
}
        



bool GPU::FrameBuffer::Create( const unsigned int w,
                               const unsigned int h )
{
    Release();

    m_Width  = w;
    m_Height = h;

    glGenFramebuffersEXT( 1, &m_Id );
    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_Id );
    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );

    return true;
}


void GPU::FrameBuffer::Release()
{
    if( m_Id )
    {
        for( BufferMap::iterator b=m_Buffers.begin(); b!=m_Buffers.end(); ++b )
            delete b->second;

        m_Buffers.clear();
        m_EnabledBuffers.clear();

        glDeleteFramebuffersEXT( 1, &m_Id );
        m_Id = 0;
    }
}


bool GPU::FrameBuffer::Attach( const GLuint attachment,
                               const GLuint format )
{
    RenderBuffer buff( format, m_Width, m_Height );
    return Attach( attachment, buff );
}


bool GPU::FrameBuffer::Attach( const GLuint attachment,
                               const GPU::RenderBuffer &buff )
{
    if( m_Id &&
        buff.IsInstantiated() &&
        m_Width==buff.Width() &&
        m_Height==buff.Height() )
    {
        Detach( attachment );

        glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_Id );
        glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, attachment, GL_RENDERBUFFER, buff.Id() );

        m_Buffers[ attachment ] = new RenderBuffer( buff );
        Enable( attachment );

        glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
        return true;
    }

    return false;
}


bool GPU::FrameBuffer::Attach( const GLuint attachment,
                               const GPU::Texture2D &tex )
{
    if( m_Id &&
        tex.IsInstantiated() &&
        m_Width==tex.Width() &&
        m_Height==tex.Height() )
    {
        Detach( attachment );

        glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_Id );
        glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, attachment, GL_TEXTURE_2D, tex.Id(), 0 );

        m_Buffers[ attachment ] = new Texture2D( tex );
        Enable( attachment );

        glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
        return true;
    }

    return false;
}


void GPU::FrameBuffer::Detach( const GLuint attachment )
{
    BufferMap::iterator foundBuffer = m_Buffers.find( attachment );
    if( foundBuffer != m_Buffers.end() )
    {
        Disable( attachment );
        delete foundBuffer->second;
        m_Buffers.erase( foundBuffer );
    }
}


void GPU::FrameBuffer::Enable( const GLuint attachment )
{
    if( attachment<GL_COLOR_ATTACHMENT0_EXT || attachment>GL_COLOR_ATTACHMENT15_EXT )
        return;

    for( std::vector<GLenum>::iterator b=m_EnabledBuffers.begin(); b!=m_EnabledBuffers.end(); ++b )
        if( *b == attachment )
            return;

    m_EnabledBuffers.push_back( attachment );
}


void GPU::FrameBuffer::Disable( const GLuint attachment )
{
    if( attachment<GL_COLOR_ATTACHMENT0_EXT || attachment>GL_COLOR_ATTACHMENT15_EXT )
        return;

    for( std::vector<GLenum>::iterator b=m_EnabledBuffers.begin(); b!=m_EnabledBuffers.end(); ++b )
        if( *b == attachment )
        {
            *b = m_EnabledBuffers.back();
            m_EnabledBuffers.pop_back();
            return;
        }
}


void GPU::FrameBuffer::Bind() const
{
    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_Id );

    if( !m_EnabledBuffers.empty() )
        glDrawBuffersARB( m_EnabledBuffers.size(), &m_EnabledBuffers.front() );
    else
        glDrawBuffersARB( 0, NULL );

    glViewport( 0, 0, m_Width, m_Height );
}


bool GPU::FrameBuffer::DumpTo( const GLuint attachment,
                               void *buffer,
                               const GLuint format,
                               const GLuint type ) const
{
    // Read data from the buffer into which the mask has been painted.
    GLint currentFb;
    glGetIntegerv( GL_FRAMEBUFFER_BINDING, &currentFb );

    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_Id );
    glReadBuffer( attachment );
    glReadPixels( 0, 0, m_Width, m_Height, format, type, buffer );

    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, currentFb );
    return true;
}


bool GPU::FrameBuffer::DumpTo( const GLuint attachment,
                               void *buffer,
                               const unsigned int x,
                               const unsigned int y,
                               const unsigned int w,
                               const unsigned int h,
                               const GLuint format,
                               const GLuint type ) const
{
    // Read data from the buffer into which the mask has been painted.
    GLint currentFb;
    glGetIntegerv( GL_FRAMEBUFFER_BINDING, &currentFb );

    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_Id );
    glReadBuffer( attachment );
    glReadPixels( x, y, w, h, format, type, buffer );

    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, currentFb );
    return true;
}


const GPU::Texture2D* GPU::FrameBuffer::AsTexture( const GLuint attachment ) const
{
    BufferMap::const_iterator b = m_Buffers.find( attachment );
    return b!=m_Buffers.end()? reinterpret_cast<Texture2D*>(b->second) : NULL;
}


const GPU::RenderBuffer* GPU::FrameBuffer::AsBuffer( const GLuint attachment ) const
{
    BufferMap::const_iterator b = m_Buffers.find( attachment );
    return b!=m_Buffers.end()? reinterpret_cast<RenderBuffer*>(b->second) : NULL;
}
