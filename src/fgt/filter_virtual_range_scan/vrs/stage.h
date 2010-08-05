#ifndef STAGE_H
#define STAGE_H

#include "resources.h"

#define PI 3.1415926535

namespace vrs
{
    class Stage
    {
    public:
        Stage( SamplerResources* res )
        {
            this->res = res;
            p = 0;
            pixelDataNames = 0;
            units = 0;
        }

        virtual ~Stage( void ){}
        virtual void go( void ) = 0;

    protected:
        SamplerResources* res;
        Program* p;

        void on( string configName, string shaderName )
        {
            on( configName, (*(res->shaders))[ shaderName ] );
        }

        void on( string configName, Program* pr )
        {
            FboConfiguration* c = (*(res->configs))[ configName ];
            p = pr;
            assert( p && c );
            res->fbo->setConfiguration( c );
            p->load();
        }

        void off( void )
        {
            if( units > 0 )
            {
                for( int i=0; i<units; i++ )
                {
                    PixelData* pData = (*(res->pdm))[ pixelDataNames[i] ];
                    assert( pData );
                    pData->unbind();
                }
                pixelDataNames = 0;
                units = 0;
            }
            p->unload();
            res->fbo->setConfiguration( 0 );
        }

        void bindPixelData( string* dataName, int dataCount, string* shaderSamplersNames )
        {
            GLint loc = -1;
            PixelData* pData = 0;
            units = dataCount;
            pixelDataNames = dataName;

            for( int i=0; i<dataCount; i++ )
            {
                loc = glGetUniformLocation( p->programId, shaderSamplersNames[i].c_str() );
                pData = (*(res->pdm))[ dataName[i] ];
                assert( pData );
                pData->bind( i );
                glUniform1i( loc, i );
            }
        }

        void bindPixelData( string dataPrefix, bool bind = true )
        {
            vector< string > names;
            vector< PixelData* > data;
            res->pdm->getFromPrefix( dataPrefix, names, data );

            GLint loc = -1;
            for( unsigned int i=0; i<data.size(); i++ )
            {
                if( bind )
                {
                    loc = glGetUniformLocation( p->programId, names[i].c_str() );
                    data[i]->bind(i);
                    glUniform1i( loc, (GLint)i );
                }
                else
                {
                    data[i]->unbind();
                }
            }
        }

        void setShaderInt( string varName, int val )
        {
            GLint loc = glGetUniformLocation( p->programId, varName.c_str() );
            glUniform1i( loc, (GLint)val );
        }

        void setShaderFloat( string varName, float val )
        {
            GLint loc = glGetUniformLocation( p->programId, varName.c_str() );
            glUniform1f( loc, (GLfloat)val );
        }

        void dumpData( string pixelDataName, int lod = 0 )
        {
            PixelData* d = (*(res->pdm))[ pixelDataName ];
            char buf[100];
            sprintf( buf, "%s_%d.txt", pixelDataName.c_str(), lod );
            Utils::downloadAndDumpTexture( buf, d->textureId, lod );
        }

        void dumpConfiguration( string configName, string prefix )
        {
            FboConfiguration* c = (*(res->configs))[ configName ];
            FboAttachment* a = 0;
            char fname[250];
            for( unsigned int i=0; i<c->size(); i++ )
            {
                a = &((*c)[i]);
                sprintf( fname, "%s_%d.txt", prefix.c_str(), i );
                Utils::downloadAndDumpTexture( fname, a->pData->textureId, a->lod );
            }
        }

    private:
        string* pixelDataNames;
        int     units;
    };
}

#endif // STAGE_H
