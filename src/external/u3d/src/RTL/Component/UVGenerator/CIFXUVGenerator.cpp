//***************************************************************************
//
//  Copyright (c) 1999 - 2006 Intel Corporation
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//***************************************************************************
//
//  CIFXUVGenerator.cpp
//
//  DESCRIPTION
//    Class implementation file for the UV generator class.
//
//  NOTES
//      None.
//
//***************************************************************************
#include "CIFXUVGenerator.h"
#include "IFXUVMapper.h"
#include "IFXCoreCIDs.h"
#include "IFXCOM.h"

CIFXUVGenerator::CIFXUVGenerator()
{
  m_uRefCount=0;
  m_LastWrapMode = IFX_UV_NONE;
  m_pMapper = 0;
}

CIFXUVGenerator::~CIFXUVGenerator()
{
  IFXRELEASE(m_pMapper);
}

//---------------------------------------------------------------------------
//  CIFXUVGenerator::AddRef
//
//  This method increments the reference count for an interface on a
//  component.  It should be called for every new copy of a pointer to an
//  interface on a given component.  It returns a U32 that contains a value
//  from 1 to 2^32 - 1 that defines the new reference count.  The return
//  value should only be used for debugging purposes.
//---------------------------------------------------------------------------
U32 CIFXUVGenerator::AddRef()
{
  return ++m_uRefCount;
}

//---------------------------------------------------------------------------
//  CIFXUVGenerator::Release
//
//  This method decrements the reference count for the calling interface on a
//  component.  It returns a U32 that contains a value from 1 to 2^32 - 1
//  that defines the new reference count.  The return value should only be
//  used for debugging purposes.  If the reference count on a component falls
//  to zero, the component is destroyed.
//---------------------------------------------------------------------------
U32 CIFXUVGenerator::Release()
{
  if( 1 == m_uRefCount )
  {
    delete this;
    return 0;
  }

  return --m_uRefCount;
}

//---------------------------------------------------------------------------
//  CIFXUVGenerator::QueryInterface
//
//  This method provides access to the various interfaces supported by a
//  component.  Upon success, it increments the component's reference count,
//  hands back a pointer to the specified interface and returns IFX_OK.
//  Otherwise, it returns either IFX_E_INVALID_POINTER or IFX_E_UNSUPPORTED.
//
//  A number of rules must be adhered to by all implementations of
//  QueryInterface.  For a list of such rules, refer to the Microsoft COM
//  description of the IUnknown::QueryInterface method.
//---------------------------------------------------------------------------
IFXRESULT CIFXUVGenerator::QueryInterface( IFXREFIID interfaceId, void** ppInterface )
{
  IFXRESULT result  = IFX_OK;

  if ( ppInterface )
  {
    if ( interfaceId == IID_IFXUVGenerator ||
       interfaceId == IID_IFXUnknown )
      *ppInterface = ( CIFXUVGenerator* ) this;
    else
    {
      *ppInterface = NULL;

      result = IFX_E_UNSUPPORTED;
    }

    if ( IFXSUCCESS( result ) )
      ( ( IFXUnknown* ) *ppInterface )->AddRef();
  }
  else
    result = IFX_E_INVALID_POINTER;

  return result;
}

IFXRESULT IFXAPI_CALLTYPE CIFXUVGenerator_Factory( IFXREFIID interfaceId, void** ppInterface )
{
  IFXRESULT result;

  if ( ppInterface )
  {
    // It doesn't exist, so try to create it.  Note:  The component
    // class sets up gs_pSingleton upon construction and NULLs it
    // upon destruction.
    CIFXUVGenerator *pComponent = new CIFXUVGenerator;

    if ( pComponent )
    {
      // Perform a temporary AddRef for our usage of the component.
      pComponent->AddRef();

      // Attempt to obtain a pointer to the requested interface.
      result = pComponent->QueryInterface( interfaceId, ppInterface );

      // Perform a Release since our usage of the component is now
      // complete.  Note:  If the QI fails, this will cause the
      // component to be destroyed.
      pComponent->Release();
    }
    else
      result = IFX_E_OUT_OF_MEMORY;
  }
  else
    result = IFX_E_INVALID_POINTER;

  return result;
}


IFXRESULT CIFXUVGenerator::Generate(IFXMesh& pMesh,
                  IFXUVMapParameters* pMapParams,
                  IFXMatrix4x4* pModelMatrix,
                  IFXMatrix4x4* pViewMatrix,
                  const IFXLightSet* pLightSet)
{
  IFXRESULT iResult=IFX_OK;

  if(NULL == pViewMatrix)
    iResult=IFX_E_INVALID_POINTER;
  if(NULL == pLightSet)
    iResult=IFX_E_INVALID_POINTER;
  if(NULL == pMapParams)
    iResult=IFX_E_INVALID_POINTER;
  if(NULL == pModelMatrix)
    iResult=IFX_E_INVALID_POINTER;


  if(IFXSUCCESS(iResult))
  {
    // build the right UV mapper class
    if ((pMapParams->eWrapMode != m_LastWrapMode) || (!m_pMapper))
    {
      IFXRELEASE(m_pMapper);

      switch(pMapParams->eWrapMode)
      {
        case IFX_UV_NONE:
          iResult=IFXCreateComponent( CID_IFXUVMapperNone, IID_IFXUVMapper, ( void** ) &m_pMapper );
          break;
        case IFX_UV_PLANAR:
          iResult=IFXCreateComponent( CID_IFXUVMapperPlanar, IID_IFXUVMapper, ( void** ) &m_pMapper );
          break;
        case IFX_UV_CYLINDRICAL:
          iResult=IFXCreateComponent( CID_IFXUVMapperCylindrical, IID_IFXUVMapper, ( void** ) &m_pMapper );
          break;
        case IFX_UV_SPHERICAL:
          iResult=IFXCreateComponent( CID_IFXUVMapperSpherical, IID_IFXUVMapper, ( void** ) &m_pMapper );
          break;
        case IFX_UV_REFLECTION:
          iResult=IFXCreateComponent( CID_IFXUVMapperReflection, IID_IFXUVMapper, ( void** ) &m_pMapper );
          break;
        default:
          iResult=IFX_E_UNSUPPORTED;
      }
    }

    if ( IFXSUCCESS(iResult) && m_pMapper )
    {
      m_LastWrapMode = pMapParams->eWrapMode;
      iResult=m_pMapper->Apply(pMesh, pMapParams, pModelMatrix, pViewMatrix, pLightSet);
    }
  }

  return iResult;
}
