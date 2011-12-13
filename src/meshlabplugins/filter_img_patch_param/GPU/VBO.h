#ifndef __GPU__VBO_H__
#define __GPU__VBO_H__




#include "InstantiatedObject.h"


// Macro enabling to recover the offset, in bytes, of a field in a structure.
#ifdef OffsetOf
#undef OffsetOf
#endif
#define OffsetOf( STRUCT, FIELD )   ( (unsigned int) &((STRUCT*)NULL)->FIELD )


namespace GPU
{
    // Class used to stop recursive template inheritance of class VBO.
    class DefaultVBO
    {
    public:
        inline bool Alloc()     { return true; }
        inline bool Destroy()   { return true; }
        inline void Bind()      {}
        inline void Unbind()    {}
        inline void Enable()    {}
        inline void Disable()   {}
    };


    // Class defining a VBO's component without a neutral behaviour. Necessary for recursive template inheritance
    // with an arbitrary number of unordered parameters.
    template <typename TStoreMode,typename T>
    class DefaultVBOComponent : public T
    {
    public:
        inline bool Alloc()     { return T::Alloc(); }
        inline bool Destroy()   { return T::Destroy(); }
        inline void Bind()      { T::Bind(); }
        inline void Unbind()    { T::Unbind(); }
        inline void Enable()    { T::Enable(); }
        inline void Disable()   { T::Disable(); }
    };


    class InterlacedBuffers {};
    class DistinctBuffers   {};




    template < typename TStoreMode,
               template <typename,typename> class A = DefaultVBOComponent,
               template <typename,typename> class B = DefaultVBOComponent,
               template <typename,typename> class C = DefaultVBOComponent,
               template <typename,typename> class D = DefaultVBOComponent,
               template <typename,typename> class E = DefaultVBOComponent,
               template <typename,typename> class F = DefaultVBOComponent,
               template <typename,typename> class G = DefaultVBOComponent,
               template <typename,typename> class H = DefaultVBOComponent > class VBO;


    template < template <typename,typename> class A,
               template <typename,typename> class B,
               template <typename,typename> class C,
               template <typename,typename> class D,
               template <typename,typename> class E,
               template <typename,typename> class F,
               template <typename,typename> class G,
               template <typename,typename> class H >
    class VBO<InterlacedBuffers,A,B,C,D,E,F,G,H> : public H< InterlacedBuffers,
                                                          G< InterlacedBuffers,
                                                          F< InterlacedBuffers,
                                                          E< InterlacedBuffers,
                                                          D< InterlacedBuffers,
                                                          C< InterlacedBuffers,
                                                          B< InterlacedBuffers,
                                                          A< InterlacedBuffers,DefaultVBO > > > > > > > >,
                                                   public InstantiatedObject
    {
    private:
        typedef H< InterlacedBuffers,
                G< InterlacedBuffers,
                F< InterlacedBuffers,
                E< InterlacedBuffers,
                D< InterlacedBuffers,
                C< InterlacedBuffers,
                B< InterlacedBuffers,
                A< InterlacedBuffers,DefaultVBO > > > > > > > >  Super;

        GLuint  m_DataVBOId;

    protected:
        inline bool Allocate()
        {
            glGenBuffersARB( 1, &m_DataVBOId );
            glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_DataVBOId );
            glBindBufferARB( GL_ARRAY_BUFFER_ARB, 0 );
            return m_DataVBOId!=0 && Super::Alloc();
        }
        inline bool Unallocate()
        {
            glDeleteBuffersARB( 1, &m_DataVBOId );
            m_DataVBOId = 0;
            return Super::Destroy();
        }

    public:
        inline      VBO() : InstantiatedObject(), m_DataVBOId(0)    {}
        inline      ~VBO()                                          { Release(); }

        inline bool Create()    { return Instantiate(); }
        template <typename T>
        inline bool Create( GLenum usage, T *buffer, GLsizei count )
        {
            return Instantiate() && LoadData( usage, buffer, count );
        }

        template <typename T>
        inline bool LoadData( GLenum usage, T *buffer, GLsizei count )
        {
            if( m_DataVBOId )
            {
                glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_DataVBOId );
                glBufferDataARB( GL_ARRAY_BUFFER_ARB, count*sizeof(T), buffer, usage );
                glBindBufferARB( GL_ARRAY_BUFFER_ARB, 0 );
                return true;
            }
            return false;
        }

        inline void Bind()
        {
            glPushClientAttrib( GL_CLIENT_VERTEX_ARRAY_BIT );
            if( m_DataVBOId )
            {
                glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_DataVBOId );
                Super::Bind();
            }
        }
        inline void Unbind()
        {
            Super::Unbind();
            glBindBufferARB( GL_ARRAY_BUFFER_ARB, 0 );
            glPopClientAttrib();
        }

        inline void DrawArrays( GLenum mode, GLint first, GLsizei count )   { glDrawArrays( mode, first, count ); }
    };


    template < template <typename,typename> class A,
               template <typename,typename> class B,
               template <typename,typename> class C,
               template <typename,typename> class D,
               template <typename,typename> class E,
               template <typename,typename> class F,
               template <typename,typename> class G,
               template <typename,typename> class H >
    class VBO<DistinctBuffers,A,B,C,D,E,F,G,H> : public H< DistinctBuffers,
                                                        G< DistinctBuffers,
                                                        F< DistinctBuffers,
                                                        E< DistinctBuffers,
                                                        D< DistinctBuffers,
                                                        C< DistinctBuffers,
                                                        B< DistinctBuffers,
                                                        A< DistinctBuffers,DefaultVBO > > > > > > > >,
                                                 public InstantiatedObject
    {
    private:
        typedef H< DistinctBuffers,
                G< DistinctBuffers,
                F< DistinctBuffers,
                E< DistinctBuffers,
                D< DistinctBuffers,
                C< DistinctBuffers,
                B< DistinctBuffers,
                A< DistinctBuffers,DefaultVBO > > > > > > > >  Super;

    protected:
        inline bool Allocate()                      { return Super::Alloc();   }
        inline bool Unallocate()                    { return Super::Destroy(); }

    public:
        inline      VBO() : InstantiatedObject()    {}
        inline      ~VBO()                          { Release(); }

        inline bool Create()                        { return Instantiate(); }

        inline void Bind()
        {
            glPushClientAttrib( GL_CLIENT_VERTEX_ARRAY_BIT );
            Super::Bind();
        }
        inline void Unbind()
        {
            Super::Unbind();
            glBindBufferARB( GL_ARRAY_BUFFER_ARB, 0 );
            glPopClientAttrib();
        }

        inline void DrawArrays( GLenum mode, GLint first, GLsizei count )   { glDrawArrays( mode, first, count ); }
    };




    template <typename TStoreMode> class VBOComponentContainer;


    template <>
    class VBOComponentContainer<InterlacedBuffers>
    {
        static const GLuint ENABLE_MASK = 1 << (8*sizeof(GLuint) - 1);
        static const GLuint STRIDE_MASK = ((GLuint)-1) ^ ENABLE_MASK;

    private:
        GLuint          m_StrideEnable;
        GLuint          m_Offset;

    public:
        inline          VBOComponentContainer() : m_StrideEnable(0), m_Offset(0)   {}

        inline void     SetPointer( GLuint stride, GLuint offset )
        {
            m_StrideEnable = stride | (m_StrideEnable & ENABLE_MASK);
            m_Offset = offset;
        }
        inline GLuint   Stride() const                                  { return m_StrideEnable & STRIDE_MASK; }
        inline GLuint   Offset() const                                  { return m_Offset; }

        inline bool     Alloc()                                         { return true; }
        inline bool     Destroy()                                       { return true; }
        inline void     Bind()                                          {}
        inline void     Unbind()                                        {}

        inline bool     IsEnabled() const                               { return (m_StrideEnable & ENABLE_MASK) == 0; }
        inline bool     IsDisabled() const                              { return !IsEnabled(); }
        inline void     Enable()                                        { m_StrideEnable &= STRIDE_MASK; }
        inline void     Disable()                                       { m_StrideEnable |= ENABLE_MASK; }
    };


    template <>
    class VBOComponentContainer<DistinctBuffers> : public VBOComponentContainer<InterlacedBuffers>
    {
    private:
        GLuint      m_DataVBOId;

    public:
        inline      VBOComponentContainer() : VBOComponentContainer<InterlacedBuffers>(), m_DataVBOId(0)  {}

        inline bool Alloc()
        {
            glGenBuffersARB( 1, &m_DataVBOId );
            glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_DataVBOId );
            glBindBufferARB( GL_ARRAY_BUFFER_ARB, 0 );
            return m_DataVBOId!=0;
        }
        inline bool Destroy()
        {
            glDeleteBuffersARB( 1, &m_DataVBOId );
            m_DataVBOId = 0;
            return true;
        }
        inline void Bind()
        {
            if( m_DataVBOId )
                glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_DataVBOId );
        }
        inline void Unbind()    {}

        template <typename T>
        inline bool LoadData( GLenum usage, T *buffer, GLsizei count )
        {
            if( m_DataVBOId )
            {
                glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_DataVBOId );
                glBufferDataARB( GL_ARRAY_BUFFER_ARB, count*sizeof(T), buffer, usage );
                glBindBufferARB( GL_ARRAY_BUFFER_ARB, 0 );
                return true;
            }
            return false;
        }
    };




    template <typename TStoreMode,int TSize,int TGLType,typename T>
    class VertexVBOComponent : public T
    {
    public:
        VBOComponentContainer<TStoreMode>  Vertex;
        inline bool Alloc()     { return Vertex.Alloc() && T::Alloc(); }
        inline bool Destroy()   { return Vertex.Destroy() && T::Destroy(); }
        inline void Bind()
        {
            if( Vertex.IsEnabled() )
            {
                Vertex.Bind();
                glEnableClientState( GL_VERTEX_ARRAY );
                glVertexPointer( TSize, TGLType, Vertex.Stride(), (const GLvoid*)(Vertex.Offset()) );
            }
            T::Bind();
        }
        inline void Unbind()    { Vertex.Unbind(); T::Unbind(); }
        inline void Enable()    { Vertex.Enable(); T::Enable(); }
        inline void Disable()   { Vertex.Disable(); T::Disable(); }
    };

    template <typename TStoreMode,typename T> class Vertex2s : public VertexVBOComponent<TStoreMode,2,GL_SHORT ,T> {};
    template <typename TStoreMode,typename T> class Vertex2i : public VertexVBOComponent<TStoreMode,2,GL_INT   ,T> {};
    template <typename TStoreMode,typename T> class Vertex2f : public VertexVBOComponent<TStoreMode,2,GL_FLOAT ,T> {};
    template <typename TStoreMode,typename T> class Vertex2d : public VertexVBOComponent<TStoreMode,2,GL_DOUBLE,T> {};
    template <typename TStoreMode,typename T> class Vertex3s : public VertexVBOComponent<TStoreMode,3,GL_SHORT ,T> {};
    template <typename TStoreMode,typename T> class Vertex3i : public VertexVBOComponent<TStoreMode,3,GL_INT   ,T> {};
    template <typename TStoreMode,typename T> class Vertex3f : public VertexVBOComponent<TStoreMode,3,GL_FLOAT ,T> {};
    template <typename TStoreMode,typename T> class Vertex3d : public VertexVBOComponent<TStoreMode,3,GL_DOUBLE,T> {};
    template <typename TStoreMode,typename T> class Vertex4s : public VertexVBOComponent<TStoreMode,4,GL_SHORT ,T> {};
    template <typename TStoreMode,typename T> class Vertex4i : public VertexVBOComponent<TStoreMode,4,GL_INT   ,T> {};
    template <typename TStoreMode,typename T> class Vertex4f : public VertexVBOComponent<TStoreMode,4,GL_FLOAT ,T> {};
    template <typename TStoreMode,typename T> class Vertex4d : public VertexVBOComponent<TStoreMode,4,GL_DOUBLE,T> {};


    template <typename TStoreMode,int TGLType,typename T>
    class NormalVBOComponent : public T
    {
    public:
        VBOComponentContainer<TStoreMode>  Normal;
        inline bool Alloc()     { return Normal.Alloc() && T::Alloc(); }
        inline bool Destroy()   { return Normal.Destroy() && T::Destroy(); }
        inline void Bind()
        {
            if( Normal.IsEnabled() )
            {
                Normal.Bind();
                glEnableClientState( GL_NORMAL_ARRAY );
                glNormalPointer( TGLType, Normal.Stride(), (const GLvoid*)(Normal.Offset()) );
            }
            T::Bind();
        }
        inline void Unbind()    { Normal.Unbind(); T::Unbind(); }
        inline void Enable()    { Normal.Enable(); T::Enable(); }
        inline void Disable()   { Normal.Disable(); T::Disable(); }
    };

    template <typename TStoreMode,typename T> class Normal3b : public NormalVBOComponent< TStoreMode,GL_BYTE  ,T> {};
    template <typename TStoreMode,typename T> class Normal3s : public NormalVBOComponent< TStoreMode,GL_SHORT ,T> {};
    template <typename TStoreMode,typename T> class Normal3i : public NormalVBOComponent< TStoreMode,GL_INT   ,T> {};
    template <typename TStoreMode,typename T> class Normal3f : public NormalVBOComponent< TStoreMode,GL_FLOAT ,T> {};
    template <typename TStoreMode,typename T> class Normal3d : public NormalVBOComponent< TStoreMode,GL_DOUBLE,T> {};


    template <typename TStoreMode,int TSize,int TGLType,typename T>
    class ColorVBOComponent : public T
    {
    public:
        VBOComponentContainer<TStoreMode>  Color;
        inline bool Alloc()     { return Color.Alloc() && T::Alloc(); }
        inline bool Destroy()   { return Color.Destroy() && T::Destroy(); }
        inline void Bind()
        {
            if( Color.IsEnabled() )
            {
                Color.Bind();
                glEnableClientState( GL_COLOR_ARRAY );
                glColorPointer( TSize, TGLType, Color.Stride(), (const GLvoid*)(Color.Offset()) );
            }
            T::Bind();
        }
        inline void Unbind()    { Color.Unbind(); T::Unbind(); }
        inline void Enable()    { Color.Enable(); T::Enable(); }
        inline void Disable()   { Color.Disable(); T::Disable(); }
    };

    template <typename TStoreMode,typename T> class Color3b  : public ColorVBOComponent<TStoreMode,3,GL_BYTE          ,T> {};
    template <typename TStoreMode,typename T> class Color3ub : public ColorVBOComponent<TStoreMode,3,GL_UNSIGNED_BYTE ,T> {};
    template <typename TStoreMode,typename T> class Color3s  : public ColorVBOComponent<TStoreMode,3,GL_SHORT         ,T> {};
    template <typename TStoreMode,typename T> class Color3us : public ColorVBOComponent<TStoreMode,3,GL_UNSIGNED_SHORT,T> {};
    template <typename TStoreMode,typename T> class Color3i  : public ColorVBOComponent<TStoreMode,3,GL_INT           ,T> {};
    template <typename TStoreMode,typename T> class Color3ui : public ColorVBOComponent<TStoreMode,3,GL_UNSIGNED_INT  ,T> {};
    template <typename TStoreMode,typename T> class Color3f  : public ColorVBOComponent<TStoreMode,3,GL_FLOAT         ,T> {};
    template <typename TStoreMode,typename T> class Color3d  : public ColorVBOComponent<TStoreMode,3,GL_DOUBLE        ,T> {};
    template <typename TStoreMode,typename T> class Color4b  : public ColorVBOComponent<TStoreMode,4,GL_BYTE          ,T> {};
    template <typename TStoreMode,typename T> class Color4ub : public ColorVBOComponent<TStoreMode,4,GL_UNSIGNED_BYTE ,T> {};
    template <typename TStoreMode,typename T> class Color4s  : public ColorVBOComponent<TStoreMode,4,GL_SHORT         ,T> {};
    template <typename TStoreMode,typename T> class Color4us : public ColorVBOComponent<TStoreMode,4,GL_UNSIGNED_SHORT,T> {};
    template <typename TStoreMode,typename T> class Color4i  : public ColorVBOComponent<TStoreMode,4,GL_INT           ,T> {};
    template <typename TStoreMode,typename T> class Color4ui : public ColorVBOComponent<TStoreMode,4,GL_UNSIGNED_INT  ,T> {};
    template <typename TStoreMode,typename T> class Color4f  : public ColorVBOComponent<TStoreMode,4,GL_FLOAT         ,T> {};
    template <typename TStoreMode,typename T> class Color4d  : public ColorVBOComponent<TStoreMode,4,GL_DOUBLE        ,T> {};


    template <typename TStoreMode,int TSize,int TGLType,typename T>
    class TexCoordVBOComponent : public T
    {
    public:
        VBOComponentContainer<TStoreMode>  TexCoord;
        inline bool Alloc()     { return TexCoord.Alloc() && T::Alloc(); }
        inline bool Destroy()   { return TexCoord.Destroy() && T::Destroy(); }
        inline void Bind()
        {
            if( TexCoord.IsEnabled() )
            {
                TexCoord.Bind();
                glEnableClientState( GL_TEXTURE_COORD_ARRAY );
                glTexCoordPointer( TSize, TGLType, TexCoord.Stride(), (const GLvoid*)(TexCoord.Offset()) );
            }
            T::Bind();
        }
        inline void Unbind()    { TexCoord.Unbind(); T::Unbind(); }
        inline void Enable()    { TexCoord.Enable(); T::Enable(); }
        inline void Disable()   { TexCoord.Disable(); T::Disable(); }
    };

    template <typename TStoreMode,typename T> class TexCoord1s : public TexCoordVBOComponent<TStoreMode,1,GL_SHORT ,T> {};
    template <typename TStoreMode,typename T> class TexCoord1i : public TexCoordVBOComponent<TStoreMode,1,GL_INT   ,T> {};
    template <typename TStoreMode,typename T> class TexCoord1f : public TexCoordVBOComponent<TStoreMode,1,GL_FLOAT ,T> {};
    template <typename TStoreMode,typename T> class TexCoord1d : public TexCoordVBOComponent<TStoreMode,1,GL_DOUBLE,T> {};
    template <typename TStoreMode,typename T> class TexCoord2s : public TexCoordVBOComponent<TStoreMode,2,GL_SHORT ,T> {};
    template <typename TStoreMode,typename T> class TexCoord2i : public TexCoordVBOComponent<TStoreMode,2,GL_INT   ,T> {};
    template <typename TStoreMode,typename T> class TexCoord2f : public TexCoordVBOComponent<TStoreMode,2,GL_FLOAT ,T> {};
    template <typename TStoreMode,typename T> class TexCoord2d : public TexCoordVBOComponent<TStoreMode,2,GL_DOUBLE,T> {};
    template <typename TStoreMode,typename T> class TexCoord3s : public TexCoordVBOComponent<TStoreMode,3,GL_SHORT ,T> {};
    template <typename TStoreMode,typename T> class TexCoord3i : public TexCoordVBOComponent<TStoreMode,3,GL_INT   ,T> {};
    template <typename TStoreMode,typename T> class TexCoord3f : public TexCoordVBOComponent<TStoreMode,3,GL_FLOAT ,T> {};
    template <typename TStoreMode,typename T> class TexCoord3d : public TexCoordVBOComponent<TStoreMode,3,GL_DOUBLE,T> {};
    template <typename TStoreMode,typename T> class TexCoord4s : public TexCoordVBOComponent<TStoreMode,4,GL_SHORT ,T> {};
    template <typename TStoreMode,typename T> class TexCoord4i : public TexCoordVBOComponent<TStoreMode,4,GL_INT   ,T> {};
    template <typename TStoreMode,typename T> class TexCoord4f : public TexCoordVBOComponent<TStoreMode,4,GL_FLOAT ,T> {};
    template <typename TStoreMode,typename T> class TexCoord4d : public TexCoordVBOComponent<TStoreMode,4,GL_DOUBLE,T> {};


    template <typename TRealType,int TGLType,typename T>
    class IndexVBOComponent : public T
    {
    private:
        GLuint  m_IndexVBOId;

    public:
        inline bool Alloc()
        {
            glGenBuffersARB( 1, &m_IndexVBOId );
            glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, m_IndexVBOId );
            glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, 0 );
            return m_IndexVBOId!=0 && T::Alloc();
        }
        inline bool Destroy()
        {
            glDeleteBuffersARB( 1, &m_IndexVBOId );
            m_IndexVBOId = 0;
            return T::Destroy();
        }

        inline void Bind()
        {
            if( m_IndexVBOId )
                glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, m_IndexVBOId );
            T::Bind();
        }
        inline void Unbind()
        {
            glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, 0 );
            T::Unbind();
        }

        inline bool LoadIndices( GLenum usage, TRealType *buffer, GLsizei count )
        {
            if( m_IndexVBOId )
            {
                glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, m_IndexVBOId );
                glBufferDataARB( GL_ELEMENT_ARRAY_BUFFER_ARB, count*sizeof(TRealType), buffer, usage );
                glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, 0 );
                return true;
            }
            return false;
        }

        inline void DrawElements( GLenum mode, GLint first, GLsizei count )
        {
            glDrawElements(	mode, count, TGLType, (const GLvoid*)(first*sizeof(TRealType)) );
        }
    };

    template <typename TStoreMode,typename T> class Indexub : public IndexVBOComponent<GLubyte ,GL_UNSIGNED_BYTE ,T> {};
    template <typename TStoreMode,typename T> class Indexus : public IndexVBOComponent<GLushort,GL_UNSIGNED_SHORT,T> {};
    template <typename TStoreMode,typename T> class Indexui : public IndexVBOComponent<GLuint  ,GL_UNSIGNED_INT  ,T> {};
}; // namespace GPU




#endif /*__GPU__VBO_H__*/
