#ifndef FBO_H
#define FBO_H

#include <QImage>
#include <map>
#include <algorithm>
#include <GL/glew.h>

#include "../textures/textures_manager.h"

using namespace std;
using namespace agl::textures;

namespace agl
{
    namespace buffers
    {
        class FboAttachment
        {
        public:
            FboAttachment( GLenum attachmentPoint, texture2d* renderTarget, int lod = 0 )
            {
                this->attachmentPoint = attachmentPoint;
                this->renderTarget = renderTarget;
                this->lod = lod;
            }

            FboAttachment( const FboAttachment& other )
            {
                this->attachmentPoint = other.attachmentPoint;
                this->renderTarget = other.renderTarget;
                this->lod = other.lod;
            }

            bool operator==( FboAttachment& a )
            {
                return ( lod == a.lod && attachmentPoint == a.attachmentPoint && renderTarget == a.renderTarget );
            }

            int lod;
            GLenum attachmentPoint;
            texture2d* renderTarget;
        };

        class Fbo;

        class FboConfiguration
        {
            friend class Fbo;

        public:
            FboConfiguration(){}

            FboConfiguration( const FboConfiguration& other )
            {
                for( unsigned int i=0; i<other.attachments.size(); i++ )
                {
                    addAttachment( new FboAttachment( *(other.attachments[i]) ) );
                }
            }

            ~FboConfiguration( void )
            {
                clearAttachments();
            }

            void clearAttachments( void )
            {
                for( unsigned int i=0; i<attachments.size(); i++ )
                {
                    delete attachments[i];
                }
                attachments.clear();
            }

            void addAttachment( FboAttachment* a )
            {
                removeAttachment( a->attachmentPoint );
                attachments.push_back( a );
            }

            void removeAttachment( FboAttachment* a )
            {
                vector< FboAttachment* >::iterator i =
                        find( attachments.begin(), attachments.end(), a );
                if( i != attachments.end() )
                {
                    attachments.erase( i );
                }
            }

            FboAttachment* getAttachment( GLenum attachmentPoint )
            {
                bool found = false;
                vector< FboAttachment* >::iterator i = attachments.begin();

                while( !found && i != attachments.end() )
                {
                    found = ( (*i)->attachmentPoint == attachmentPoint );
                    ++i;
                }

                if( found )
                {
                    --i;
                    return (*i);
                }
                return 0;
            }

            void removeAttachment( GLenum attachmentPoint )
            {
                bool found = false;
                vector< FboAttachment* >::iterator i = attachments.begin();

                while( !found && i != attachments.end() )
                {
                    found = ( (*i)->attachmentPoint == attachmentPoint );
                    ++i;
                }

                if( found )
                {
                    --i;
                    attachments.erase( i );
                }
            }

            void getColorAttachments( vector< FboAttachment* >& v )
            {
                GLenum current = 0;

                for( unsigned int i=0; i<attachments.size(); i++ )
                {
                    current = attachments[i]->attachmentPoint;

                    if( current != GL_DEPTH_ATTACHMENT && current != GL_STENCIL_ATTACHMENT )
                    {
                        v.push_back( attachments[i] );
                    }
                }
            }

            void getTextures( vector< texture2d* >& tv, bool onlyColorAttachments = true )
            {
                GLenum currentAttachment = 0;
                for( unsigned int i=0; i<attachments.size(); i++ )
                {
                    if( onlyColorAttachments )
                    {
                        currentAttachment = attachments[i]->attachmentPoint;

                        if( currentAttachment != GL_DEPTH_ATTACHMENT && currentAttachment != GL_STENCIL_ATTACHMENT )
                        {
                            tv.push_back( attachments[i]->renderTarget );
                        }
                    }
                    else
                    {
                        tv.push_back( attachments[i]->renderTarget );
                    }
                }
            }

            bool hasAttachment( GLenum attachmentPoint )
            {
                bool found = false;
                unsigned int i=0;

                while( !found && i<attachments.size() )
                {
                    found = ( attachments[i]->attachmentPoint == attachmentPoint );
                    i++;
                }

                return found;
            }

            bool hasDepthAttachment( void )
            {
                return hasAttachment( GL_DEPTH_ATTACHMENT );
            }

            bool hasStencilAttachment( void )
            {
                return hasAttachment( GL_STENCIL_ATTACHMENT );
            }

            bool operator==( FboConfiguration& c )
            {
                if( attachments.size() != c.attachments.size() )
                {
                    return false;
                }

                bool equal = true;
                unsigned int i=0;

                while( equal && i<attachments.size() )
                {
                    equal = ( *(attachments[i]) == *(c.attachments[i]) );
                    i++;
                }

                return equal;
            }

            FboAttachment* operator[]( GLenum attachmentPoint )
            {
                return getAttachment( attachmentPoint );
            }

        private:
            vector< FboAttachment* > attachments;

            void load( void )
            {
                FboAttachment* current = 0;
                for( unsigned int i=0; i<attachments.size(); i++ )
                {
                    current = attachments[i];
                    glFramebufferTexture2D
                            ( GL_FRAMEBUFFER,
                              current->attachmentPoint,
                              GL_TEXTURE_2D,
                              current->renderTarget->get_id(),
                              current->lod );
                }
            }

            void unload( void )
            {
                FboAttachment* current = 0;
                for( unsigned int i=0; i<attachments.size(); i++ )
                {
                    current = attachments[i];
                    glFramebufferTexture2D
                            ( GL_FRAMEBUFFER,
                              current->attachmentPoint,
                              GL_TEXTURE_2D,
                              0,
                              0 );
                }
            }

            void bind( void )
            {
                vector< FboAttachment* > colorAttachments;
                getColorAttachments( colorAttachments );

                int arraySize = colorAttachments.size() * sizeof( GLenum );
                GLenum* attArray = (GLenum*)malloc( arraySize );

                for( unsigned int i=0; i<colorAttachments.size(); i++ )
                {
                    attArray[i] = colorAttachments[i]->attachmentPoint;
                }

                glDrawBuffers( colorAttachments.size(), attArray );
                free( attArray );
            }
        };

        class FboConfigurationFactory
        {

        public:

            static FboConfiguration* extractConfiguration
                    ( FboConfiguration* input, vector< GLenum >& targets )
            {
                FboAttachment* tmpAttachment = 0;
                FboConfiguration* output = new FboConfiguration();

                for( unsigned int i=0; i<targets.size(); i++ )
                {
                    tmpAttachment = input->getAttachment( targets[i] );
                    if( tmpAttachment )
                    {
                        output->addAttachment( new FboAttachment( *tmpAttachment ) );
                    }
                }

                return output;
            }

            static FboConfiguration* create
                    ( TexturesManager* tm, int attWidth, int attHeight, GLint internalFormat,
                      GLenum format, GLenum type, int colorAttachmentsCount,
                      bool depth = true, bool stencil = false )
            {
                FboConfiguration* output = new FboConfiguration();

                if( depth )
                {
                    texture2d* depthBuffer = new texture2d
                        ( attWidth, attHeight, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT, 0 );
                    vector< texture2d* > tmpVector;
                    tmpVector.push_back( depthBuffer );
                    tm->registerTextures( tmpVector );
                    FboAttachment* depthAttachment = new FboAttachment( GL_DEPTH_ATTACHMENT, depthBuffer, 0 );
                    output->addAttachment( depthAttachment );
                }

                vector< texture2d* > myTextures;
                tm->generateTextures( colorAttachmentsCount, attWidth, attHeight, internalFormat,
                                      format, type, 0, myTextures );
                FboAttachment* tmpAttachment = 0;
                for( unsigned int i=0; i<myTextures.size(); i++ )
                {
                    tmpAttachment = new FboAttachment( GL_COLOR_ATTACHMENT0 + i, myTextures[i], 0 );
                    output->addAttachment( tmpAttachment );
                }

                return output;
            }

            static FboConfiguration* createFromTexture
                    ( TexturesManager* tm, texture2d* prototype,
                      int colorAttachmentsCount, bool depth = true, bool stencil = false )
            {
                FboConfiguration* config = new FboConfiguration();

                if( depth )
                {
                    texture2d* depthBuffer = new texture2d
                        ( prototype->get_width(), prototype->get_height(),
                          GL_DEPTH_COMPONENT24,
                          GL_DEPTH_COMPONENT, GL_FLOAT, 0 );
                    vector< texture2d* > tmpVector;
                    tmpVector.push_back( depthBuffer );
                    tm->registerTextures( tmpVector );
                    FboAttachment* depthAttachment = new FboAttachment
                                                     ( GL_DEPTH_ATTACHMENT, depthBuffer, 0 );
                    config->addAttachment( depthAttachment );
                }

                vector< texture2d* > attachments;
                tm->generateTextures( colorAttachmentsCount, prototype, attachments );
                FboAttachment* currentAttachment;
                for( unsigned int i=0; i<attachments.size(); i++ )
                {
                    currentAttachment = new FboAttachment
                                        ( GL_COLOR_ATTACHMENT0 + i, attachments[i], 0 );
                    config->addAttachment( currentAttachment );
                }

                return config;
            }
        };

        class Fbo
        {

        public:
            Fbo( void )
            {
                this->configuration = 0;
                glGenFramebuffers( 1, &id );
            }            

            ~Fbo( void )
            {
                clearConfigurations();
                glDeleteFramebuffers( 1, &id );
            }

            FboConfiguration* getConfiguration( void )
            {
                return configuration;
            }


            void setConfiguration( FboConfiguration* newConfig )
            {
                if( newConfig == 0 )
                {
                    if( configuration )
                    {
                        configuration->unload();
                    }
                    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
                    configuration = 0;
                    return;
                }

                glBindFramebuffer( GL_FRAMEBUFFER, id );

                if( configuration )
                {
                    configuration->unload();
                }

                newConfig->load();
                newConfig->bind();

                configuration = newConfig;

                FboAttachment* a = (*configuration)[ GL_COLOR_ATTACHMENT0 ];
                texture2d* data = a->renderTarget;

                assert( glCheckFramebufferStatus( GL_FRAMEBUFFER ) == GL_FRAMEBUFFER_COMPLETE );

                glViewport( 0, 0, data->get_width(a->lod), data->get_height(a->lod) );
            }

            void setConfiguration( string name )
            {
                map< string, FboConfiguration* >::iterator mi = configs.find( name );
                if( mi != configs.end() )
                {
                    setConfiguration( (*mi).second );
                }
            }

            void saveConfiguration( string name )
            {
                configs[ name ] = configuration;
            }

            void saveConfiguration( string name, FboConfiguration* conf )
            {
                configs[ name ] = conf;
            }

            void clearConfigurations( void )
            {
                setConfiguration( 0 );
                map< string, FboConfiguration* >::iterator mi;
                for( mi = configs.begin(); mi != configs.end(); mi++ )
                {
                    if( (*mi).second != configuration )
                    {
                        delete (*mi).second;
                    }
                }
                configs.clear();
            }

            void deleteConfiguration( string name )
            {
                map< string, FboConfiguration* >::iterator i = configs.find( name );
                if( i != configs.end() )
                {
                    delete (*i).second;
                    configs.erase( i );
                }
            }

            /* note: the fbo must be bound */
            void saveScreenshots( string prefix = "color attachment " )
            {
                if( !configuration ) return;
                vector< FboAttachment* > colorAttachments;
                configuration->getColorAttachments( colorAttachments );
                char buf[ 100 ];

                for( unsigned int i=0; i<colorAttachments.size(); i++ )
                {
                    glReadBuffer( colorAttachments[i]->attachmentPoint );
                    sprintf( buf, "%s %d.bmp", prefix.c_str(), i );
                    screenshot( buf );
                }
            }

        private:
            FboConfiguration* configuration;
            map< string, FboConfiguration* > configs;
            GLuint id;

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
    }
}



#endif // FBO_H
