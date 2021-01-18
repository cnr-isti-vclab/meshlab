#ifndef UTILS_H
#define UTILS_H

#include <GL/glew.h>
#include <QString>
#include <assert.h>
#include <QDir>
#include <QTextStream>
#include <QImage>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <string>

using namespace std;

namespace vs
{
    /* -------------- a simple shaders aggregator ------------------------------------ */
    class Program
    {
    public:

        Program( const string& vertexProgram, const string& fragmentProgram )
        {
            assert( !vertexProgram.empty() && !fragmentProgram.empty() );
            vertId = createShaderPart( GL_VERTEX_SHADER, vertexProgram );
            fragId = createShaderPart( GL_FRAGMENT_SHADER, fragmentProgram );
            programId = glCreateProgram();
            glAttachShader( programId, vertId );
            glAttachShader( programId, fragId );
            glLinkProgram( programId );
        }

        ~Program( void )
        {
            glDeleteProgram( programId );
            glDeleteShader( vertId );
            glDeleteShader( fragId );
        }

        void load   ( void ){ glUseProgram( programId); }
        void unload ( void ){ glUseProgram( 0 ); }

        GLuint programId;

    private:
        GLuint vertId;
        GLuint fragId;

        GLuint createShaderPart( GLenum type, const string& src )
        {
            GLuint id = glCreateShader( type );
            const char* cc = src.c_str();
            glShaderSource( id, 1, &cc, NULL );
            glCompileShader( id );
            GLint cs = 0;
            glGetShaderiv( id, GL_COMPILE_STATUS, &cs );

            if( cs == GL_FALSE )
            {
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
                        qDebug( info_log );
                    }
                    delete[] info_log;
                }
            }
            assert( cs == GL_TRUE );
            return id;
        }
    };
    /* ------------------------------------------------------------------------------- */

    /* -------------------- PixelData: a simple texture abstraction ------------------ */
    class PixelData
    {
    public:

        PixelData( GLint side = 1, bool depthData = false )
        {
            textureId               = (GLuint)0;
            assert( side );
            this->side              = side;
            this->elements          = 0;
            this->depthData         = depthData;
            this->bound             = false;
            this->textureUnit       = GL_TEXTURE0;
            this->mipmapsGenerated  = false;
            create();
        }

        ~PixelData( void )
        {
            destroy();
        }

        void bind( GLint textureUnitIndex = (GLint)0 )
        {
            assert( (int)textureId );
            if( bound ) unbind();
            textureUnit = GL_TEXTURE0 + textureUnitIndex;
            glActiveTexture( textureUnit );
            glBindTexture( GL_TEXTURE_2D, textureId );
            bound = true;
            textureUnit = GL_TEXTURE0 + textureUnitIndex;
        }

        void unbind( void )
        {
            if( !bound ) return;
            glActiveTexture( textureUnit );
            glBindTexture( GL_TEXTURE_2D, (GLuint)0 );
            bound = false;
            textureUnit = GL_TEXTURE0;
        }

        void resize( GLint newSide )
        {
            if( newSide == side ) return;
            destroy();
            this->side = newSide;
            create();
        }

        void generateMipmaps( void )
        {
            bool previouslyBound = bound;
            if( !bound ) bind();
            glGenerateMipmap( GL_TEXTURE_2D );
            if( !previouslyBound ) unbind();
            mipmapsGenerated = true;
        }

        GLint getMipmapLevels( void )
        {
            int levels = 1;
            int tmpSide = (int)side;
            while( tmpSide > 1 )
            {
                tmpSide /= 2;
                levels++;
            }
            return (GLint)levels;
        }

        std::pair< GLint, GLint > getSize( GLint lod = 0 )
        {
            bool previouslyBound = bound;
            if( !bound ) bind();

            GLint width = (GLint)0, height = (GLint)0;
            glGetTexLevelParameteriv( GL_TEXTURE_2D, lod, GL_TEXTURE_WIDTH, &width );
            glGetTexLevelParameteriv( GL_TEXTURE_2D, lod, GL_TEXTURE_HEIGHT, &height );

            if( !previouslyBound ) unbind();

            return std::pair< GLint, GLint >( width, height );
        }

        void resizeToFit( GLint elementsCount )
        {
            GLint outputSide = 1;
            bool ok = false;
            while( !ok )
            {
                ok = ( outputSide * outputSide >= elementsCount );
                outputSide *= 2;
            }
            outputSide /= 2;
            if( side == outputSide ) return;

            resize( outputSide );
            if( mipmapsGenerated )
            {   // mipmaps were generated, so we re-generate them
                mipmapsGenerated = false;
                generateMipmaps();
            }
        }

        GLfloat* download( GLint lod = 0 )
        {
            bool previouslyBound = bound;
            if( !bound ) bind();

            // computes texture size
            GLint pixelBytes = (GLint)12;   // GL_RGB32F -> 3 channels * 4 bytes
            GLint width = (GLint)0, height = (GLint)0;
            glGetTexLevelParameteriv( GL_TEXTURE_2D, lod, GL_TEXTURE_WIDTH, &width );
            glGetTexLevelParameteriv( GL_TEXTURE_2D, lod, GL_TEXTURE_HEIGHT, &height );
            GLint textureBytes = width * height * pixelBytes;

            // downloads and returns the texture data
            void* texData = malloc( (int)textureBytes );
            glGetTexImage( GL_TEXTURE_2D, lod, depthData? GL_DEPTH_COMPONENT : GL_RGB, GL_FLOAT, texData );
            if( !previouslyBound ) unbind();
            return (GLfloat*)texData;
        }

        void dumpToFile( QString filename, int lod = 0 )
        {
            QFile f( filename );
            if( f.open( QFile::WriteOnly ) )
            {
                GLfloat* pix = download( lod );
                int tmpSide = (int)side / (int)( pow( 2.0, lod ) );
                QTextStream stream( &f );

                char buf[300];
                sprintf( buf, "Texture id:\t%d\nTexture side:\t%d x %d\nElements (level 0):\t%d\n\n",
                         (int)textureId, tmpSide, tmpSide, (int)elements );
                stream << buf;

                int c = 0;
                for( int i=0; i<tmpSide; i++ )
                {
                    for( int j=0; j<tmpSide; j++ )
                    {
                        if( depthData )
                        {
                            sprintf( buf, "%d)\t%f\t(%d, %d)\n", c++, pix[0], j, i );
                            pix++;
                        }
                        else
                        {
                            sprintf( buf, "%d)\t(%f,\t%f,\t%f)\t(%d, %d)\n", c++, pix[0], pix[1], pix[2], j, i );
                            pix++; pix++; pix++;
                        }
                        stream << buf;
                    }
                }

                f.close();
            }
        }

        GLuint  textureId;
        GLint   side;
        GLint   elements;
        bool    depthData;

    private:
        bool    bound;
        GLenum  textureUnit;
        bool    mipmapsGenerated;

        void create( void )
        {
            glGenTextures( (GLsizei)1, &textureId );
            glActiveTexture( GL_TEXTURE0 );
            glBindTexture( GL_TEXTURE_2D, textureId );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

            glTexImage2D( GL_TEXTURE_2D, (GLint)0,
                          depthData? GL_DEPTH_COMPONENT24 : GL_RGB32F,
                          (GLsizei)side, (GLsizei)side, (GLint)0,
                          depthData? GL_DEPTH_COMPONENT : GL_RGB,
                          GL_FLOAT, (GLvoid*)0 );

            glBindTexture( GL_TEXTURE_2D, 0 );

            this->elements = 0;
        }

        void destroy( void )
        {
            unbind();
            glDeleteTextures( (GLsizei)1, &textureId );
            this->textureId = (GLint)-1;
            this->elements = 0;
            this->side = 1;
        }
    };
    /* ------------------------------------------------------------------------------- */

    /* --------------- framebuffer objects management -------------------------------- */
    class Fbo
    {

    public:
        Fbo( void )
        {
            glGenFramebuffers( (GLint)1, &fboId );
            bound       = false;
            depthOn     = false;
            colAttOn    = 0;
        }

        ~Fbo( void )
        {
            unbind();
            glDeleteFramebuffers( (GLint)1, &fboId );
        }

        void bind( void )
        {
            if( bound ) return;
            glBindFramebuffer( GL_FRAMEBUFFER, fboId );
            bound = true;
        }

        void unbind( void )
        {
            if( !bound ) return;
            glBindFramebuffer( GL_FRAMEBUFFER, (GLuint)0 );
            bound = false;
        }

        void load( std::vector< PixelData* >& colorAttachments, int lod = 0 )
        {
            bool previouslyBound = bound;
            if( !bound ) bind();

            assert( colorAttachments.size() > 0 );
            GLenum att = 0;
            GLenum* drawBuffers = new GLenum[ colorAttachments.size() ];
            for( unsigned int i=0; i<colorAttachments.size(); i++ )
            {
                att = GL_COLOR_ATTACHMENT0 + (GLint)i;
                glFramebufferTexture2D( GL_FRAMEBUFFER, att, GL_TEXTURE_2D,
                                        colorAttachments[i]->textureId, lod );

                drawBuffers[ i ] = att;
            }

            // sets the draw buffers
            glDrawBuffers( (GLsizei)colorAttachments.size(), drawBuffers );
            delete[] drawBuffers;

            assert( isOk() );
            colAttOn = colorAttachments.size();

            // updates the viewport
            std::pair< GLint, GLint > size = colorAttachments[0]->getSize( lod );
            glViewport( (GLint)0, (GLint)0, (GLsizei)size.first, (GLsizei)size.second );

            if( !previouslyBound ) unbind();

            assert( isOk() );
        }

        void loadDepth( GLuint depthTextureId )
        {
            bool previouslyBound = bound;
            if( !bound ) bind();

            if( depthOn )
            {
                glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0 );
            }

            glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                                    depthTextureId, 0 );

            assert( isOk() );
            depthOn = true;

            if( !previouslyBound ) unbind();
        }

        void unload( void )
        {
            if( !depthOn && colAttOn == 0 ) return;
            bool previouslyBound = bound;
            if( !bound ) bind();

            if( depthOn )
            {
                glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0 );
            }

            if( colAttOn )
            {
                GLenum att = 0;
                for( int i=0; i<colAttOn; i++ )
                {
                    att = GL_COLOR_ATTACHMENT0 + (GLint)i;
                    glFramebufferTexture2D( GL_FRAMEBUFFER, att, GL_TEXTURE_2D, 0, 0 );
                }
            }

            depthOn = false;
            colAttOn = 0;

            if( !previouslyBound ) unbind();
        }

        void screenshots( string prefix = "color_attachment" )
        {
            char buf[ 100 ];
            for( int i=0; i<colAttOn; i++ )
            {
                glReadBuffer( GL_COLOR_ATTACHMENT0 + (GLint)i );
                sprintf( buf, "%s_%d.bmp", prefix.c_str(), i );
                screenshot( buf );
            }
        }

    private:
        GLuint  fboId;
        int     colAttOn;
        bool    depthOn;
        bool    bound;

        bool isOk( void )
        {
            GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

            if ( fboStatus != GL_FRAMEBUFFER_COMPLETE_EXT)
            {
                switch (fboStatus)
                {
                case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
                    qDebug( "FBO Incomplete: Attachment" );
                    break;
                case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
                    qDebug("FBO Incomplete: Missing Attachment");
                    break;
                case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
                    qDebug("FBO Incomplete: Dimensions");
                    break;
                case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
                    qDebug("FBO Incomplete: Formats");
                    break;
                case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
                    qDebug("FBO Incomplete: Draw Buffer");
                    break;
                case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
                    qDebug("FBO Incomplete: Read Buffer");
                    break;
                default:
                    qDebug("Undefined FBO error");
                    assert(0);
                }

                return false;
            }

            return ( glCheckFramebufferStatus( GL_FRAMEBUFFER ) == GL_FRAMEBUFFER_COMPLETE );
        }

        static void screenshot( const QString& dest )
        {
            int w, h;
            GLint viewport[4];
            glGetIntegerv( GL_VIEWPORT, viewport );
            w = viewport[2];
            h = viewport[3];
            QImage img( w, h, QImage::Format_ARGB32 );
            glReadPixels( 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, img.bits() );
            img = img.rgbSwapped().mirrored();
            img.save( dest, "bmp" );
        }
    };
    /* ------------------------------------------------------------------------------- */
}

#endif // UTILS_H
