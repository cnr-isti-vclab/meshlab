#ifndef SIMPLE_SHADER_VARIABLES_H
#define SIMPLE_SHADER_VARIABLES_H

#include "shader_variables.h"
#include "../textures/texture2d.h"
#include <string>
#include <vector>

using namespace std;
using namespace agl::textures;

namespace agl
{
    namespace shaders
    {
        class simple_shaders_variables: public shader_variables
        {

        public:

            ~simple_shaders_variables( void )
            {
                clear();
            }

            void clear( void )
            {
                vector< shader_variable_descriptor* >::iterator i;
                for( i = descriptors.begin(); i != descriptors.end(); i++ )
                {
                    delete (*i);
                }
                descriptors.clear();
            }

            virtual void set( GLuint program_id )
            {
                shader_variable_descriptor* svd = 0;
                GLint loc = -1;

                for( unsigned int i=0; i<descriptors.size(); i++ )
                {
                    svd = descriptors[i];
                    loc = glGetUniformLocation( program_id, svd->name.c_str() );

                    if( svd->type == GL_SAMPLER_2D )
                    {
                        pair< GLint, texture2d* >* p = (pair< GLint, texture2d* >*)svd->values;
                        p->second->bind( p->first );
                        glUniform1i( loc, p->first );
                        continue;
                    }

                    if( svd->matrix )
                    {
                        // todo
                    }
                    else
                    {
                        switch( svd->type )
                        {
                        case GL_FLOAT:

                            switch( svd->components )
                            {
                            case 1:
                                glUniform1fv( loc, svd->count, (GLfloat*)svd->values );
                                break;
                            case 2:
                                glUniform2fv( loc, svd->count, (GLfloat*)svd->values );
                                break;
                            case 3:
                                glUniform3fv( loc, svd->count, (GLfloat*)svd->values );
                                break;
                            case 4:
                                glUniform4fv( loc, svd->count, (GLfloat*)svd->values );
                                break;
                            }

                            break;
                        case GL_INT:

                            switch( svd->components )
                            {
                            case 1:
                                glUniform1iv( loc, svd->count, (GLint*)svd->values );
                                break;
                            case 2:
                                glUniform2iv( loc, svd->count, (GLint*)svd->values );
                                break;
                            case 3:
                                glUniform3iv( loc, svd->count, (GLint*)svd->values );
                                break;
                            case 4:
                                glUniform4iv( loc, svd->count, (GLint*)svd->values );
                                break;
                            }

                            break;
                        case GL_UNSIGNED_INT:

                            switch( svd->components )
                            {
                            case 1:
                                glUniform1uiv( loc, svd->count, (GLuint*)svd->values );
                                break;
                            case 2:
                                glUniform2uiv( loc, svd->count, (GLuint*)svd->values );
                                break;
                            case 3:
                                glUniform3uiv( loc, svd->count, (GLuint*)svd->values );
                                break;
                            case 4:
                                glUniform4uiv( loc, svd->count, (GLuint*)svd->values );
                                break;
                            }

                            break;

                        default:
                            assert( 0 );
                            break;
                        }
                    }
                }
            }

            virtual void unset( GLuint program_id )
            {
                shader_variable_descriptor* svd = 0;
                GLint loc = -1;

                for( unsigned int i=0; i<descriptors.size(); i++ )
                {
                    svd = descriptors[i];

                    if( svd->uniform )
                    {
                        loc = glGetUniformLocation( program_id, svd->name.c_str() );

                        if( svd->type == GL_SAMPLER_2D )
                        {
                            pair< GLint, texture2d* >* p = (pair< GLint, texture2d* >*)svd->values;
                            p->second->unbind();
                            continue;
                        }



                    }else
                    {

                    }

                }
            }

            void bind_texture( string name, GLint unit, texture2d* t )
            {
                shader_variable_descriptor* svd = new shader_variable_descriptor;
                svd->uniform = true;
                svd->name = name;
                svd->type = GL_SAMPLER_2D;
                svd->components = 1;
                svd->count = 1;
                svd->matrix = false;
                svd->rows = -1;
                svd->columns = -1;
                svd->values = new pair< GLint, texture2d* >( unit, t );
                descriptors.push_back( svd );
            }

            void set_uniform_vector( string name, GLenum type, GLvoid* v,
                                     GLint components = 1, GLint count = 1 )
            {
                shader_variable_descriptor* svd = new shader_variable_descriptor;
                svd->uniform = true;
                svd->name = name;
                svd->type = type;
                svd->components = components;
                svd->count = count;
                svd->matrix = false;
                svd->rows = -1;
                svd->columns = -1;
                svd->values = v;
                descriptors.push_back( svd );
            }

        private:

            class shader_variable_descriptor
            {

            public:

                bool uniform;		// flag che distingue una variabile uniform da una attributo
                string name;		// nome variabile
                GLenum type;		// tipo della variabile (GL_INT, GL_FLOAT...)
                GLint components;	// numero componenti per elemento (1, 2, 3 o 4)
                GLint count;		// numero elementi
                bool matrix;		// indica se la variabile è una matrice
                GLint rows;             // numero di righe della matrice
                GLint columns;		// numero colonne della matrice
                GLvoid* values;		// valore da settare

            };

            vector< shader_variable_descriptor* > descriptors;

        };
    }
}

#endif // SIMPLE_SHADER_VARIABLES_H
