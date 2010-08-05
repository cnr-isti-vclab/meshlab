#ifndef UTILS_H
#define UTILS_H

#include <GL/glew.h>
#include <string>
#include <map>
#include <vector>
#include <assert.h>
#include <Qt>
#include <QImage>
#include <QDir>
#include <QTextStream>
#include <math.h>

using namespace std;

namespace vrs
{
    // a simple way to handle shader programs
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

    // PixelData is a texture whose valid pixels can be less than
    // the whole pixels inside the texture itself
    class PixelData
    {
    public:
        int length;
        GLuint textureId;
        int side;

        void bind( int textureUnit )
        {
            myUnit = GL_TEXTURE0 + (GLint)textureUnit;
            glActiveTexture( myUnit );
            glBindTexture( GL_TEXTURE_2D, textureId );
        }

        void unbind( void )
        {
            glActiveTexture( myUnit );
            glBindTexture( GL_TEXTURE_2D, 0 );
        }

    private:
        GLenum myUnit;
    };

    // A PixelDataManager object stores pairs string -> PixelData,
    // to retrieve PixelData object in a simple way
    class PixelDataManager
    {
    public:
        typedef map< string, PixelData* >   PDMap;
        typedef PDMap::iterator             PDIterator;

        PixelDataManager( void ){}
        ~PixelDataManager( void )
        {
            PixelData* tmpData = 0;
            int size = (int)m.size(), c = 0;
            GLuint texturesToDelete[ size ];
            for( PDIterator i = m.begin(); i != m.end(); i++ )
            {
                tmpData = (*i).second;
                texturesToDelete[ c++ ] = tmpData->textureId;
                delete tmpData;
            }
            m.clear();
            glDeleteTextures( size, texturesToDelete );
        }

        PixelData* get( string key )
        {
            PDIterator i = m.find( key );
            if( i != m.end() ) return (*i).second;
            return 0;
        }

        void getFromPrefix( string prefix, vector< string >& names, vector< PixelData* >& data )
        {
            names.clear();
            data.clear();

            for( PDIterator i = m.begin(); i != m.end(); ++i )
            {
                if( (*i).first.substr( 0, prefix.length() ) == prefix )
                {
                    names.push_back( (*i).first );
                    data.push_back( (*i).second );
                }
            }
        }

        void put( string key, PixelData* pData )
        {
            assert( pData );

            PDIterator i = m.find( key );
            if( i == m.end() )
            {
                m[ key ] = pData;
            }
            else
            {
                PixelData* p = (*i).second;
                if( p != pData )
                {
                    glDeleteTextures( 1, &(p->textureId) );
                    delete p;
                    m[ key ] = pData;
                }
            }
        }

        PixelData* operator[]( string key ){ return get(key); }

    private:
        PDMap m;

    };

    // an fbo attachment is a PixelData object at a given Level Of Detail
    class FboAttachment
    {
    public:
        PixelData* pData;
        GLenum attachmentPoint;
        int lod;
    };

    // and an fbo configuration is a vector of FboAttachments
    typedef vector< FboAttachment > FboConfiguration;

    // finally, an fbo is simply a container for a FboConfiguration
    class Fbo
    {

    public:
        Fbo( void )
        {
            glGenFramebuffers( 1, &fboId );
            current = 0;
        }

        ~Fbo( void )
        {
            bind();
            setConfiguration( 0 );  // detaches the current configuration
            unbind();
            glDeleteFramebuffers( 1, &fboId );
        }

        void bind   ( void ){ glBindFramebuffer( GL_FRAMEBUFFER, fboId ); }
        void unbind ( void ){ glBindFramebuffer( GL_FRAMEBUFFER, 0 ); }
        
        void setConfiguration( FboConfiguration* c )
        {
            if( c == current ) return;
            if( current ) setContent( false );
            current = c;
            if( current ) setContent( true );
        }

        void screenshots( string prefix = "color_attachment" )
        {
            char buf[ 100 ];
            int size = current->size();
            FboAttachment* a = 0;
            for( int i=0; i<size; i++ )
            {
                if( (*current)[i].attachmentPoint == GL_DEPTH_ATTACHMENT ) continue;
                a = &((*current)[i]);
                glReadBuffer( a->attachmentPoint );
                sprintf( buf, "%s_%d.bmp", prefix.c_str(), i );
                screenshot( buf );
            }
        }

    private:
        GLuint fboId;
        FboConfiguration* current;
        GLint currentViewport;

        void setContent( bool set )
        {
            if( set )
            {
                glPushAttrib( GL_VIEWPORT_BIT );
            }

            FboAttachment* a = 0;

            int size = (int)current->size();
            for( int i=0; i<size; i++ )
            {
                a = &((*current)[i]);
                glFramebufferTexture2D( GL_FRAMEBUFFER,
                                        a->attachmentPoint,
                                        GL_TEXTURE_2D,
                                        set? a->pData->textureId : 0,
                                        set? a->lod : 0 );
            }

            if( !set )
            {
                glPopAttrib();
            }
            else
            {
                FboAttachment& a = (*current)[0];
                int divider = pow( 2.0, a.lod );
                int side = a.pData->side / divider;
                glViewport( 0, 0, (GLsizei)side, (GLsizei)side );
                GLenum* buffers = new GLenum[ size ];
                int j = 0;
                for( int i=0; i<size; i++ )
                {
                    if( (*current)[i].attachmentPoint != GL_DEPTH_ATTACHMENT )
                    {
                        buffers[j] = GL_COLOR_ATTACHMENT0 + (GLint)j;
                        j++;
                    }
                }
                glDrawBuffers( j, buffers );
                delete[] buffers;
            }
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

    class Utils
    {
    public:

        static GLuint createTexture( int side, bool depthTexture )
        {
            GLuint id;
            glGenTextures( 1, &id );
            glBindTexture( GL_TEXTURE_2D, id );

            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

            glTexImage2D( GL_TEXTURE_2D, 0,
                          depthTexture? GL_DEPTH_COMPONENT24 : GL_RGB32F,
                          (GLsizei)side, (GLsizei)side, 0,
                          depthTexture? GL_DEPTH_COMPONENT : GL_RGB, GL_FLOAT, 0 );

            glBindTexture( GL_TEXTURE_2D, 0 );
            return id;
        }

        static PixelData* createPixelData( int side, bool depthTexture = false, int length = -1 )
        {
            GLuint id = createTexture( side, depthTexture );

            int dataLength = side * side;
            if( length != -1 ) dataLength = length;

            PixelData* pData = new PixelData();
            pData->length = dataLength;
            pData->side = side;
            pData->textureId = id;
            return pData;
        }

        static FboConfiguration* createFboConfiguration
                ( int colorAttachmentsCount, int textureSide, bool depth = true )
        {
            FboConfiguration* result = new FboConfiguration();
            FboAttachment* a = new FboAttachment();
            a->lod = 0;
            for( int i=0; i<colorAttachmentsCount; i++ )
            {
                a->pData = createPixelData( textureSide, false );
                a->attachmentPoint = GL_COLOR_ATTACHMENT0 + (GLint)i;
                result->push_back( *a );
            }

            if( depth )
            {
                a->pData = createPixelData( textureSide, true );
                a->attachmentPoint = GL_DEPTH_ATTACHMENT;
                result->push_back( *a );
            }

            delete a;
            return result;
        }

        static void saveMatrices( void )
        {
            glMatrixMode( GL_PROJECTION );
            glPushMatrix();
            glLoadIdentity();
            glMatrixMode( GL_MODELVIEW );
            glPushMatrix();
            glLoadIdentity();
        }

        static void restoreMatrices( void )
        {
            glMatrixMode( GL_PROJECTION );
            glPopMatrix();
            glMatrixMode( GL_MODELVIEW );
            glPopMatrix();
        }

        static void fullscreenQuad( void )
        {
            glBegin (GL_QUADS);
            glVertex3i (-1, -1, -1);
            glVertex3i (1, -1, -1);
            glVertex3i (1, 1, -1);
            glVertex3i (-1, 1, -1);
            glEnd ();
        }

        /*
                feeds the pipeline with <count> vertices whose coordinates are
                (x, y) pair, where 0 <= x < x_max and 0 <= y < y_max. If
                count > x_max * y_max, then only x_max * y_max points are feed
                into the pipeline.
         */
        static void feed_coords( int x_max, int y_max, int count )
        {
            int max_points = x_max * y_max;
            if( count > max_points ) count = max_points;

            int rows = count / x_max;
            int cols = count % x_max;

            glBegin( GL_POINTS );

            int i = 0;
            for( i = 0; i < rows; i++ )
            {
                for( int j = 0; j < x_max; j++ )
                {
                    glVertex2i( j, i );
                }
            }

            for( int j = 0; j < cols; j++ )
            {
                glVertex2i( j, i );
            }

            glEnd();
        }

        static void readResource( QString resName, QString& target )
        {
            QString completeName = QString(":/") + resName;
            QFile f( completeName );
            if(f.open( QFile::ReadOnly ) )
            {
                QTextStream stream(&f);
                target = stream.readAll();
                f.close();
            }
        }

        static void downloadAndDumpTexture( QString filename, GLuint textureId, int lod = 0 )
        {
            glActiveTexture( GL_TEXTURE0 );
            glBindTexture( GL_TEXTURE_2D, textureId );

            int pixelSize = 3 * 3 * 4;  // RGB32F
            int width = 0, height = 0;
            glGetTexLevelParameteriv( GL_TEXTURE_2D, lod, GL_TEXTURE_WIDTH, &width );
            glGetTexLevelParameteriv( GL_TEXTURE_2D, lod, GL_TEXTURE_HEIGHT, &height );
            int textureSize = width * height * pixelSize;

            void* tex = malloc( textureSize );
            glGetTexImage( GL_TEXTURE_2D, lod, GL_RGB, GL_FLOAT, tex );
            GLfloat* pix = (GLfloat*)tex;
            string description;
            char buf[300];
            int c = 0;

            QFile f( filename );
            if(f.open( QFile::WriteOnly ) )
            {
                QTextStream stream(&f);

                for( int i=0; i<height; i++ )
                {
                    for( int j=0; j<width; j++ )
                    {
                        sprintf( buf, "%d)\t(%f,\t%f,\t%f)\t(%d, %d)\n", c++, pix[0], pix[1], pix[2], j, i );
                        stream << buf;
                        pix++; pix++; pix++;
                    }
                }
                f.close();
            }

            free(tex);
            glBindTexture( GL_TEXTURE_2D, 0 );
        }
    };
}

#endif // UTILS_H
