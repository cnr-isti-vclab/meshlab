#ifndef __GPU__TEXTURE_H__
#define __GPU__TEXTURE_H__




#include "commonDefs.h"
#include "InstantiatedObject.h"
#include <map>
#include <vector>


/*
 *
 *  CLASS DECLARATION.
 *
 */

namespace GPU
{
    class Texture2D : public InstantiatedObject
    {
        /********************\
        | Member variable(s) |
        \********************/
    private:
        GLuint              m_Id;
        GLint               m_TexUnit;
        unsigned int        m_Width;
        unsigned int        m_Height;

        /*****************************\
        | Constructor(s) / destructor |
        \*****************************/
    public:
        inline              Texture2D() : InstantiatedObject(), m_Id(0), m_Width(0), m_Height(0)  {}
        inline              ~Texture2D()                                    { Release(); }

        /********************\
        | Member function(s) |
        \********************/
    protected:
        bool                Allocate();
        bool                Unallocate();

    public:
        bool                Create();
        bool                Create( GLint internalFormat,
                                    GLsizei width,
                                    GLsizei height,
                                    GLint border,
                                    GLenum format,
                                    GLenum type,
                                    const GLvoid* data );
        bool                Create( GLint internalFormat,
                                    GLsizei width,
                                    GLsizei height,
                                    GLenum format,
                                    GLenum type,
                                    const GLvoid* data );
        bool                Create( GLint internalFormat,
                                    GLsizei width,
                                    GLsizei height );
        inline GLuint       Id() const                      { return m_Id; }

        void                LoadMipMap( GLint level,
                                        GLint internalFormat,
                                        GLsizei width,
                                        GLsizei height,
                                        GLint border,
                                        GLenum format,
                                        GLenum type,
                                        const GLvoid* data );
        void                LoadMipMap( GLint level,
                                        GLint internalFormat,
                                        GLsizei width,
                                        GLsizei height,
                                        GLenum format,
                                        GLenum type,
                                        const GLvoid* data );
        void                LoadMipMap( GLint level,
                                        GLint internalFormat,
                                        GLsizei width,
                                        GLsizei height );
        void                Load( GLint internalFormat,
                                  GLsizei width,
                                  GLsizei height,
                                  GLint border,
                                  GLenum format,
                                  GLenum type,
                                  const GLvoid* data );
        void                Load( GLint internalFormat,
                                  GLsizei width,
                                  GLsizei height,
                                  GLenum format,
                                  GLenum type,
                                  const GLvoid* data );
        void                Load( GLint internalFormat,
                                  GLsizei width,
                                  GLsizei height );

        void                Bind();
        void                Bind( GLuint texUnit );
        void                Unbind() const;

        inline void         SetParam( const GLenum param, const float  value )  { glBindTexture( GL_TEXTURE_2D, m_Id ); glTexParameterf ( GL_TEXTURE_2D, param, value ); }
        inline void         SetParam( const GLenum param, const float* value )  { glBindTexture( GL_TEXTURE_2D, m_Id ); glTexParameterfv( GL_TEXTURE_2D, param, value ); }
        inline void         SetParam( const GLenum param, const int    value )  { glBindTexture( GL_TEXTURE_2D, m_Id ); glTexParameteri ( GL_TEXTURE_2D, param, value ); }
        inline void         SetParam( const GLenum param, const int*   value )  { glBindTexture( GL_TEXTURE_2D, m_Id ); glTexParameteriv( GL_TEXTURE_2D, param, value ); }
        inline void         SetFiltering( const GLenum minFilter, const GLenum magFilter )
        {
            glBindTexture( GL_TEXTURE_2D, m_Id );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter );
        }
        inline void         SetFiltering( const GLenum bothFilters )            { SetFiltering(bothFilters,bothFilters); }

        inline unsigned int Width() const                                       { return m_Width; }
        inline unsigned int Height() const                                      { return m_Height; }
        unsigned int        RealWidth() const;
        unsigned int        RealHeight() const;

        GLint               InternalFormat() const;

        bool                DumpTo( void *buffer,
                                    const GLenum format,
                                    const GLenum type,
                                    const GLint level = 0 ) const;

        GPU::Texture2D      Clone() const;
    };
};




#endif //__GPU__TEXTURE_H__
