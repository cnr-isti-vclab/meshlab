#ifndef __GPU__INSTANTIATEDOBJECT_H__
#define __GPU__INSTANTIATEDOBJECT_H__




#include "commonDefs.h"


/*
 *
 *  CLASS DECLARATION.
 *
 */

namespace GPU
{
    class InstantiatedObject
    {
        /********************\
        | Member variable(s) |
        \********************/
    private:
        unsigned int                *m_NbInstances;

        /*****************************\
        | Constructor(s) / destructor |
        \*****************************/
    public:
        inline                      InstantiatedObject() : m_NbInstances(NULL)          {}
        inline                      InstantiatedObject( const InstantiatedObject &obj );
        inline virtual              ~InstantiatedObject()                               { Release(); }

        /********************\
        | Member function(s) |
        \********************/
    protected:
        virtual bool                Allocate()   = 0;
        virtual bool                Unallocate() = 0;

    public:
        inline InstantiatedObject&  operator=( const InstantiatedObject& obj );

        inline bool                 Instantiate();
        inline bool                 Release();
        inline unsigned int         NbInstances() const                                 { return m_NbInstances? *m_NbInstances : 0; }
        inline bool                 IsInstantiated() const                              { return m_NbInstances != NULL; }
    };
};


/*
 *
 *  INLINE FUNCTIONS' DEFINITIONS.
 *
 */

inline GPU::InstantiatedObject::InstantiatedObject( const InstantiatedObject &obj )
{
    m_NbInstances = obj.m_NbInstances;
    if( m_NbInstances )
        ++ (*m_NbInstances);
}


inline GPU::InstantiatedObject& GPU::InstantiatedObject::operator=( const InstantiatedObject& obj )
{
    if( &obj!=this && obj.m_NbInstances!=m_NbInstances )
    {
        Release();

        m_NbInstances = obj.m_NbInstances;
        if( m_NbInstances )
            ++ (*m_NbInstances);
    }

    return *this;
}


inline bool GPU::InstantiatedObject::Instantiate()
{
    if( Release() && Allocate() )
    {
        m_NbInstances = new unsigned int;
        *m_NbInstances = 1;
        return true;
    }
    else
    {
        Release();
        return false;
    }
}


inline bool GPU::InstantiatedObject::Release()
{
    bool res = true;

    if( m_NbInstances )
    {
        if( (*m_NbInstances) == 1 )
        {
            delete m_NbInstances;
            res = Unallocate();
        }
        else
            -- (*m_NbInstances);

        m_NbInstances = NULL;
    }

    return res;
}




#endif //__GPU__INSTANTIATEDOBJECT_H__
