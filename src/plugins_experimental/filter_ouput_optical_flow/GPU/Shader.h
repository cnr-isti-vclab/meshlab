#ifndef __GPU__SHADER_H__
#define __GPU__SHADER_H__




#include "commonDefs.h"
#include "InstantiatedObject.h"
#include <string>
#include <map>


/*
 *
 *  CLASS DECLARATION.
 *
 */

namespace GPU
{
    class Shader
    {
        /******************\
        | Internal type(s) |
        \******************/
    public:
        struct Uniform
        {
            GLint       location;
            GLint       size;
            GLenum      type;
        };

        class PgObject : public InstantiatedObject
        {
            /********************\
            | Member variable(s) |
            \********************/
        private:
            GLhandleARB     m_Id;
            GLenum          m_PgType;

            /*****************************\
            | Constructor(s) / destructor |
            \*****************************/
        public:
            inline  PgObject() : InstantiatedObject(), m_Id(0)  {}
            virtual ~PgObject()                                 { Release(); }

            /********************\
            | Member function(s) |
            \********************/
        protected:
            bool        Allocate();
            bool        Unallocate();

        public:
            bool        CompileSrcFile( const std::string &filename,
                                        const GLenum pgType,
                                        std::string *logs = NULL );
            bool        CompileSrcString( const char *sourceString,
                                          const GLenum pgType,
                                          std::string *logs = NULL );
            GLhandleARB Id() const                                      { return m_Id; }
            GLenum      Type() const                                    { return m_PgType; }
        };

		class VertPg : public PgObject
        {
        public:
			inline bool CompileSrcFile( const std::string &filename,
                                        std::string *logs = NULL )      { return PgObject::CompileSrcFile( filename, GL_VERTEX_SHADER_ARB, logs ); }
            inline bool CompileSrcString( const char *sourceString,
                                          std::string *logs = NULL )    { return PgObject::CompileSrcString( sourceString, GL_VERTEX_SHADER_ARB, logs ); }
        };

		class FragPg : public PgObject
        {
        public:
			inline bool CompileSrcFile( const std::string &filename,
                                        std::string *logs = NULL )      { return PgObject::CompileSrcFile( filename, GL_FRAGMENT_SHADER_ARB, logs ); }
            inline bool CompileSrcString( const char *sourceString,
                                          std::string *logs = NULL )    { return PgObject::CompileSrcString( sourceString, GL_FRAGMENT_SHADER_ARB, logs ); }
        };

		class GeomPg : public PgObject
        {
        public:
			inline bool CompileSrcFile( const std::string &filename,
                                        std::string *logs = NULL )      { return PgObject::CompileSrcFile( filename, GL_GEOMETRY_SHADER_EXT, logs ); }
            inline bool CompileSrcString( const char *sourceString,
                                          std::string *logs = NULL )    { return PgObject::CompileSrcString( sourceString, GL_GEOMETRY_SHADER_EXT, logs ); }
            inline void SetInputPrimitives( int value )                 { if( IsInstantiated() ) glProgramParameteriARB( Id(), GL_GEOMETRY_INPUT_TYPE_EXT , value ); }
            inline void SetOutputPrimitives( int value )                { if( IsInstantiated() ) glProgramParameteriARB( Id(), GL_GEOMETRY_OUTPUT_TYPE_EXT, value ); }
        };

    private:
        typedef std::map<std::string,Uniform> UniformMap;
	    typedef std::map<std::string,GLint>   SamplerMap;
	    typedef std::map<std::string,GLint>   AttribMap;

        /********************\
        | Member variable(s) |
        \********************/
    private:
        GLhandleARB m_Id;

        VertPg      m_VertPg;
        FragPg      m_FragPg;
        GeomPg      m_GeomPg;

        UniformMap  m_Uniforms;
        SamplerMap  m_Samplers;
        AttribMap   m_Attributes;

        /*****************************\
        | Constructor(s) / destructor |
        \*****************************/
    public:
        inline  Shader() : m_Id(0)                          {}
        inline  ~Shader()                                   { Release(); }

        /*******************\
        | Class function(s) |
        \*******************/
    public:
        static std::string      GetLogs( GLuint pgId );

        /********************\
        | Member function(s) |
        \********************/
    protected:
        void					RecoverActiveUniforms();
        void					RecoverActiveAttributes();

    public:
        bool                    Link( std::string *logs = NULL );

        Shader&                 Attach( const PgObject &pg );
        inline bool             AttachAndLink( const PgObject &pg,
                                               std::string *logs = NULL )           { Attach(pg); return Link(logs); }
        Shader&                 DetachVertPg();
        Shader&                 DetachFragPg();
        Shader&                 DetachGeomPg();

        void					Release();
        inline bool				IsCreated() const                                   { return m_Id != 0; }
        inline GLhandleARB		Id() const                                          { return m_Id; }

        inline const VertPg&    VertProgram() const                                 { return m_VertPg; }
        inline const FragPg&    FragProgram() const                                 { return m_FragPg; }
        inline const GeomPg&    GeomProgram() const                                 { return m_GeomPg; }

        inline void				Bind() const                                        { glUseProgramObjectARB( m_Id ); }
        inline void				Unbind() const                                      { glUseProgramObjectARB( 0 ); }

        void					SetUniform( const std::string& name,
								            const void *value );
        void					SetSampler( const std::string& name,
								            const GLint texUnit );

        inline const Uniform*   UniformLoc( const std::string& name ) const
        {
            UniformMap::const_iterator u = m_Uniforms.find( name );
            return (u!=m_Uniforms.end())? &u->second : NULL;
        }
        inline GLint            SamplerLoc( const std::string& name ) const
        {
            SamplerMap::const_iterator s = m_Samplers.find( name );
            return (s!=m_Samplers.end())? s->second : -1;
        }
        inline GLint            AttribLoc( const std::string& name ) const
        {
            AttribMap::const_iterator a = m_Attributes.find( name );
            return (a!=m_Attributes.end())? a->second : -1;
        }
    };
};




#endif //__GPU__SHADER_H__
