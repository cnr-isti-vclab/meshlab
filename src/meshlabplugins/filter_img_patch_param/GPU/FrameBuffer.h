#ifndef __GPU__FRAMEBUFFER_H__
#define __GPU__FRAMEBUFFER_H__




#include "commonDefs.h"
#include "Texture.h"
#include <map>
#include <vector>


/*
 *
 *  CLASS DECLARATION.
 *
 */

namespace GPU
{
    class RenderBuffer : public InstantiatedObject
    {
        /********************\
        | Member variable(s) |
        \********************/
    private:
        GLuint              m_Id;
        unsigned int        m_Width;
        unsigned int        m_Height;

        /*****************************\
        | Constructor(s) / destructor |
        \*****************************/
    public:
        inline              RenderBuffer() :
            m_Id(0),
            m_Width(0),
            m_Height(0)
        {}
        inline              RenderBuffer( const GLuint format,
                                          const unsigned int w,
                                          const unsigned int h ) :
            m_Id(0),
            m_Width(0),
            m_Height(0)
        {
            Create( format, w, h );
        }
        inline              ~RenderBuffer()                 { Release(); }

        /********************\
        | Member function(s) |
        \********************/
    protected:
        bool                Allocate();
        bool                Unallocate();

    public:
        bool                Create( const GLuint format,
                                    const unsigned int w,
                                    const unsigned int h );

        inline unsigned int Width() const                   { return m_Width; }
        inline unsigned int Height() const                  { return m_Height; }
        inline GLuint       Id() const                      { return m_Id; }
        GLint               InternalFormat() const;
    };


    class FrameBuffer
    {
        /******************\
        | Internal type(s) |
        \******************/
    private:
        typedef std::map<GLuint,InstantiatedObject*> BufferMap;

        /********************\
        | Member variable(s) |
        \********************/
    private:
        GLuint              m_Id;
        GLuint              m_Width;
        GLuint              m_Height;
        BufferMap           m_Buffers;
        std::vector<GLenum> m_EnabledBuffers;

        /*****************************\
        | Constructor(s) / destructor |
        \*****************************/
    public:
        inline  FrameBuffer() :
            m_Id(0),
            m_Width(0),
            m_Height(0)
        {}
        inline  FrameBuffer( const unsigned int w,
                             const unsigned int h ) :
            m_Id(0),
            m_Width(0),
            m_Height(0)
        {
            Create(w,h);
        }
        inline  ~FrameBuffer()                                          { Release(); }

        /********************\
        | Member function(s) |
        \********************/
    public:
        bool                Create( const unsigned int w,
                                    const unsigned int h );
        inline bool         IsCreated() const                           { return m_Id != 0; }
        void                Release();

        inline GLuint       Width() const                               { return m_Width; }
        inline GLuint       Height() const                              { return m_Height; }
        inline GLuint       Id() const                                  { return m_Id; }

        bool                Attach( const GLuint attachment,
                                    const GLuint format );
        bool                Attach( const GLuint attachment,
                                    const GPU::RenderBuffer &buff );
        inline bool         Attach( const GLuint attachment,
                                    const GPU::RenderBuffer *buff )     { return buff? Attach(attachment,*buff) : false; }
        bool                Attach( const GLuint attachment,
                                    const GPU::Texture2D &tex );
        inline bool         Attach( const GLuint attachment,
                                    const GPU::Texture2D *tex )         { return tex? Attach(attachment,*tex) : false; }
        void                Detach( const GLuint attachment );

        void                Enable ( const GLuint attachment );
        void                Disable( const GLuint attachment );

        void                Bind() const;
        inline void         Unbind() const                              { glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 ); }

        bool                DumpTo( const GLuint attachment,
                                    void *buffer,
                                    const GLuint format,
                                    const GLuint type ) const;
        bool                DumpTo( const GLuint attachment,
                                    void *buffer,
                                    const unsigned int x,
                                    const unsigned int y,
                                    const unsigned int w,
                                    const unsigned int h,
                                    const GLuint format,
                                    const GLuint type ) const;

        const Texture2D*    AsTexture( const GLuint attachment ) const;
        const RenderBuffer* AsBuffer( const GLuint attachment ) const;
    };
};




#endif //__GPU__FRAMEBUFFER_H__
