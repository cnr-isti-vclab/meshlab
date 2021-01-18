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
//  CIFXPalette.cpp
//
//  DESCRIPTION
//    Main implementation file for the palette subsystem.
//
//  NOTES
//    None.
//
//***************************************************************************


#include <string.h>
#include "IFXSubject.h"
#include "CIFXPalette.h"
#include "IFXResourceClient.h"
#include "IFXSceneGraph.h"
#include "IFXModel.h"

#include "IFXCoreCIDs.h"

#define _MINIMUM_HASH_TABLE_SIZE	128
#define _UNIQUE_MODE_CLASH_SUFFIX	L"-##"


//---------------------------------------------------------------------------
//  CIFXPalette constructor
//
//  This method initializes all the data/variables use in by the palette class.
//---------------------------------------------------------------------------
CIFXPalette::CIFXPalette(  )
{
  m_uRefCount = 0;

  m_pPalette = NULL;
  m_pHashMap = NULL;
  m_uPaletteSize = 0;
  m_uLastFree = 0;
  m_uGrowthSize = 0;
  m_uNumberEntries = 0;
  m_bUniqueNameMode = FALSE;
  m_bDefaultMode = FALSE;
  m_uLatestTagID = 0;
}


//---------------------------------------------------------------------------
//  CIFXPalette destructor
//
//  This method deallocates all the data/variables use in by the palette class.
//  It walks down the palette list, deallocates the name of the palette object,
//  and calls Release(  ) on any data member it holds.  Finally, it deletes the
//  palette array itself.
//---------------------------------------------------------------------------
CIFXPalette::~CIFXPalette(  )
{
  if ( ( m_uPaletteSize == 0 ) && m_pPalette  )
  {
    IFXDeallocate( m_pPalette  );
    m_pPalette = NULL;
  }
  else if ( m_uPaletteSize != 0  )
  {
    U32 uIndex;
    m_bDefaultMode = TRUE;
    for( uIndex = 0; uIndex <= m_uPaletteSize; uIndex++  )
    {
      DeleteById( uIndex  );
    }

    // delete the palette structure
    IFXDeallocate( m_pPalette  );
    m_pPalette = NULL;
    m_bDefaultMode = FALSE;
  }

  if( m_pHashMap != NULL  )
  {
    m_pHashMap->Release(  );
  }
}


//---------------------------------------------------------------------------
//  CIFXPalette::AddRef
//
//  This method increments the reference count for an interface on a
//  component.  It should be called for every new copy of a pointer to an
//  interface on a given component.  It returns a U32 that contains a value
//  from 1 to 2^32 - 1 that defines the new reference count.  The return
//  value should only be used for debugging purposes.
//---------------------------------------------------------------------------
U32 CIFXPalette::AddRef( void )
{
  return ++m_uRefCount;
}

//---------------------------------------------------------------------------
//  CIFXPalette::Release
//
//  This method decrements the reference count for the calling interface on a
//  component.  It returns a U32 that contains a value from 1 to 2^32 - 1
//  that defines the new reference count.  The return value should only be
//  used for debugging purposes.  If the reference count on a component falls
//  to zero, the component is destroyed.
//---------------------------------------------------------------------------
U32 CIFXPalette::Release( void )
{
  if ( 1 == m_uRefCount )
  {
    CIFXSubject::PreDestruct();
    delete this;

    // This second return point is used so that the deleted object's
    // reference count isn't referenced after the memory is released.
    return 0;
  }

  return --m_uRefCount;
}

//---------------------------------------------------------------------------
//  CIFXPalette::QueryInterface
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
IFXRESULT CIFXPalette::QueryInterface( IFXREFIID interfaceId, void** ppInterface  )
{
  IFXRESULT result   = IFX_OK;

  if ( ppInterface  )
  {
    if ( interfaceId == IID_IFXPalette ||
      interfaceId == IID_IFXUnknown  )
    {
      *ppInterface = ( IFXPalette*  ) this;
    }
    else if( interfaceId == IID_IFXSubject )
    {
      *ppInterface = ( IFXSubject* ) this;
    }
    else
    {
      *ppInterface = NULL;

      result = IFX_E_UNSUPPORTED;
    }

    if ( IFXSUCCESS( result  )  )
      AddRef();
  }
  else
    result = IFX_E_INVALID_POINTER;

  return result;

}


// IFXPalette methods


//---------------------------------------------------------------------------
//  CIFXPalette::Initialize
//
//  This allocates the initial array that holds the palette based on the passed
//  in initial size of the palette.  It initializes everything to null/zero.
//---------------------------------------------------------------------------
IFXRESULT CIFXPalette::Initialize( U32 uInitialSize, U32 uGrowthSize )
{
  m_uGrowthSize = uGrowthSize;

  // pre-existing list exists
  if( m_pPalette != NULL )
    return IFX_E_ALREADY_INITIALIZED;
  if( uInitialSize == 0 )
    return IFX_E_INVALID_RANGE;

  // allocate the palette contiguously as a table
  m_pPalette = ( SPaletteObject* )IFXAllocate( ( sizeof( SPaletteObject )*( uInitialSize+1 ) )  );
  if( m_pPalette == NULL )
    return IFX_E_OUT_OF_MEMORY;

  m_uPaletteSize = uInitialSize;

  // initialize the next index pointers
  U32 uIndex;
  for(  uIndex = 0; uIndex <= m_uPaletteSize; uIndex++ )
  {
    m_pPalette[uIndex].m_pName = NULL;
    m_pPalette[uIndex].m_pObject = NULL;
    m_pPalette[uIndex].m_pModChain = NULL;
    m_pPalette[uIndex].m_pSimpleObject = NULL;
    m_pPalette[uIndex].m_uNextFreeId = uIndex+1;
    m_pPalette[uIndex].m_bHidden = FALSE;
  }
  m_uNumberEntries = 0;
  m_uLastFree = 1;

  // initialize the hash map for fast name lookup
  // do NOT allocate a new one if one exists, this is for the
  // case that you're resizing the palette table, but not the
  // hashmap
  if( m_pHashMap == NULL )
  {
    IFXRESULT iResult;
    iResult = IFXCreateComponent( CID_IFXHashMap,
                    IID_IFXHashMap,
                    ( void** )&m_pHashMap  );
    if( IFXSUCCESS( iResult ) )
    {
      if( uInitialSize < _MINIMUM_HASH_TABLE_SIZE )
        m_pHashMap->Initialize( _MINIMUM_HASH_TABLE_SIZE );
      else
        m_pHashMap->Initialize( ( 3*uInitialSize )/5 );
    } else {
      return iResult;
    }
  }
  return IFX_OK;
}

//---------------------------------------------------------------------------
//  CIFXPalette::Add
//
//  !!! DEPRECATED - USE THE CIFXSTRING VERSION!!
//---------------------------------------------------------------------------
IFXRESULT CIFXPalette::Add( const IFXCHAR* pPaletteEntryName, U32* pPaletteEntryId )
{
  IFXRESULT iResult = IFX_OK;

  if( pPaletteEntryName == NULL || pPaletteEntryId == NULL )
    iResult = IFX_E_INVALID_POINTER;
  if( m_pPalette == NULL )
    iResult = IFX_E_NOT_INITIALIZED;

  if( IFXSUCCESS( iResult ) )
  {
    IFXString sName( pPaletteEntryName );
    iResult = Add( &sName, pPaletteEntryId );
  }
  return iResult;
}


//---------------------------------------------------------------------------
//  CIFXPalette::Add
//
//  This function conditionally adds a new entry to the palette.  If an entry
//  with that name already exists, it returns a handle to the already existing
//  entry.  If the entry is new, then the next free index ( from 0 ) is passed
//  back.  If the array is full, it attempt to realloc the size of the array
//  based on the growth factor.
//---------------------------------------------------------------------------
IFXRESULT CIFXPalette::Add( IFXString* pPaletteEntryName, U32* pPaletteEntryId )
{
  IFXRESULT iResult = IFX_OK;
  IFXRESULT iFound = IFX_OK;

  if( pPaletteEntryId == NULL )
    iResult = IFX_E_INVALID_POINTER;

  if( m_pPalette == NULL )
    iResult = IFX_E_NOT_INITIALIZED;

  if( pPaletteEntryName == NULL )
    iResult = IFX_E_INVALID_POINTER;

  if( FALSE == m_bDefaultMode )
  {
    if( 0 == pPaletteEntryId )
      iResult = IFX_E_CANNOT_CHANGE;
  }

  IFXString* pNewName = new IFXString( pPaletteEntryName );
  if( pNewName == NULL )
    iResult = IFX_E_OUT_OF_MEMORY;

  if( IFXSUCCESS( iResult ) )
  {
    iFound = Find( pPaletteEntryName, pPaletteEntryId );
    if( m_bUniqueNameMode == FALSE )
    {
      // does the palette object already exist?
      if( IFXSUCCESS( iFound ) )
      {
        delete pNewName;
        iResult = IFX_W_ALREADY_EXISTS;
      }
    }
    else
    {
      if( IFXSUCCESS( iFound ) )
      {
        U32 uCurrentTag = ++m_uLatestTagID;
        do
        {
          pNewName->Assign( pPaletteEntryName );
          IFXString cDigits;
          cDigits.ToString( uCurrentTag,10 );
          pNewName->Concatenate( _UNIQUE_MODE_CLASH_SUFFIX );
          pNewName->Concatenate( &cDigits );
          uCurrentTag++;
        } while( Find( pNewName, pPaletteEntryId ) == IFX_OK  );
      }
    }
  }


  if( IFXSUCCESS( iResult ) && iResult != IFX_W_ALREADY_EXISTS )
  {
    // Is palette full?
    if( m_uLastFree == ( m_uPaletteSize+1 ) )
    {
      U32 uNewPaletteSize = 0; void* pReallocBlock = NULL;
      if( m_uGrowthSize == 0 )
        uNewPaletteSize = m_uPaletteSize*2+2;
      else
        uNewPaletteSize = m_uPaletteSize+m_uGrowthSize+1;

      U32 size = sizeof( SPaletteObject );
      pReallocBlock = IFXReallocate( m_pPalette, ( size*uNewPaletteSize )  );
      if( pReallocBlock == NULL )
      {
        // not enough memory to grow - just add one
        pReallocBlock = IFXReallocate( m_pPalette, ( size*( m_uPaletteSize+1 ) )  );
        if( pReallocBlock == NULL )
        {
          // not enough memory to grow even one - die
          return IFX_E_OUT_OF_MEMORY;
        }
        m_pPalette = ( SPaletteObject* )pReallocBlock;
        m_pPalette[m_uPaletteSize].m_pName = NULL;
        m_pPalette[m_uPaletteSize].m_uNextFreeId = m_uPaletteSize+1;
        m_pPalette[m_uPaletteSize].m_bHidden = FALSE;
        m_pPalette[m_uPaletteSize].m_pObject = NULL;
        m_pPalette[m_uPaletteSize].m_pModChain = NULL;
        m_pPalette[m_uPaletteSize].m_pSimpleObject = NULL;
        m_uPaletteSize++;

        //* should probably reallocate to m_uPaletteSize+1+1

      }
      else
      {
        // clear and initialize all entries
        m_pPalette = ( SPaletteObject* )pReallocBlock;

        U32 uIndex;
        for(  uIndex = ( m_uPaletteSize+1 ); uIndex<uNewPaletteSize; uIndex++ )
        {
          m_pPalette[uIndex].m_pName = NULL;
          m_pPalette[uIndex].m_pObject = NULL;
          m_pPalette[uIndex].m_pModChain = NULL;
          m_pPalette[uIndex].m_uNextFreeId = uIndex+1;
          m_pPalette[uIndex].m_pSimpleObject = NULL;
          m_pPalette[uIndex].m_bHidden = FALSE;
        }

        m_uPaletteSize = ( uNewPaletteSize-1 );

      }
    }
    // we now have a guaranteed free allocation unit
    *pPaletteEntryId = m_uLastFree;

    m_pPalette[*pPaletteEntryId].m_pName = pNewName;
    m_pPalette[*pPaletteEntryId].m_pObject = NULL;
    m_pPalette[*pPaletteEntryId].m_pModChain = NULL;
    m_uLastFree = m_pPalette[*pPaletteEntryId].m_uNextFreeId;

    // add an entry to the hashmap with this info:
    // NamePtr = m_pPalette[*uPaletteEntryId].m_pName    ID = *pPaletteEntryId
    IFXString* pHashName = new IFXString( pPaletteEntryName );
    if ( pHashName == NULL )
    {
      iResult = IFX_E_OUT_OF_MEMORY;
    }
    else
    {
      m_pHashMap->Add( pHashName, *pPaletteEntryId );
      IFXDELETE( pHashName );

      m_uNumberEntries++;

      /**
	  @todo Why this is commented?
      // Notify observers
	  // Note:  This is currently being handled instead by SetResourcePtr.
      if ( m_bUniqueNameMode )
        PostChanges( IFX_SUBJECT_PALETTE_ADD_HIDDEN );
      else
        PostChanges( IFX_SUBJECT_PALETTE_ADD );
      */
    }
  }

  return iResult;
}


//---------------------------------------------------------------------------
//  CIFXPalette::AddHidden
//
//  This function adds a new entry to the palette marked as hidden.  Hidden
//  entries are not exposed to the author and not written to the file. If an
//  entry with the given name already exists, a unique name is created.
//---------------------------------------------------------------------------
IFXRESULT CIFXPalette::AddHidden( IFXString* pPaletteEntryName, U32* pPaletteEntryId )
{
  m_bUniqueNameMode = TRUE;
  IFXRESULT result = Add( pPaletteEntryName, pPaletteEntryId  );
  m_bUniqueNameMode = FALSE;

  if ( result == IFX_OK  )
    m_pPalette[*pPaletteEntryId].m_bHidden = TRUE;

  return result;
}


//---------------------------------------------------------------------------
//  CIFXPalette::IsHidden
//
//  This function returns the hidden status of an entry.
//---------------------------------------------------------------------------
IFXRESULT CIFXPalette::IsHidden( U32 uInIndex, BOOL* pbOutHidden )
{
  IFXRESULT iResult = IFX_OK;

  if( m_pPalette == NULL )
    iResult = IFX_E_NOT_INITIALIZED;

  if( pbOutHidden == NULL )
    iResult = IFX_E_INVALID_POINTER;

  if( uInIndex > m_uPaletteSize )
    iResult = IFX_E_INVALID_RANGE;

  if( IFXSUCCESS( iResult ) )
    if( m_pPalette[uInIndex].m_pName == NULL )
      iResult = IFX_E_INVALID_RANGE;

  if( IFXSUCCESS( iResult ) )
    *pbOutHidden = m_pPalette[uInIndex].m_bHidden;

  return iResult;
}


//---------------------------------------------------------------------------
//  CIFXPalette::SetDefault
//
//  This function conditionally adds a new entry to the palette.  It puts in
//  the zero'th entry the value specified.
//---------------------------------------------------------------------------
IFXRESULT CIFXPalette::SetDefault( IFXString* pPaletteEntryName )
{
  IFXRESULT iResult = IFX_OK;

  if( m_pPalette == NULL )
    iResult = IFX_E_NOT_INITIALIZED;

  if( pPaletteEntryName == NULL )
    iResult = IFX_E_INVALID_POINTER;

  // does the entry already exist?
  if( IFXSUCCESS( iResult ) )
  {
    // deallocate/release anything that is already in the ID = 0 spot
    m_bDefaultMode = TRUE;
    DeleteById( 0 );
    m_bDefaultMode = FALSE;

    IFXString* pName = new IFXString( *pPaletteEntryName );
    if ( pName == NULL )
    {
      iResult = IFX_E_OUT_OF_MEMORY;
    }
    else
    {
      m_pPalette[0].m_pName = pName;
      m_pPalette[0].m_pObject = NULL;
      m_pPalette[0].m_pModChain = NULL;
      m_pPalette[0].m_pSimpleObject = NULL;
      m_uLastFree = m_pPalette[0].m_uNextFreeId;

      // add an entry to the hashmap with this info:
      IFXString* pHashName = new IFXString( pName );
      if ( pHashName == NULL )
        iResult = IFX_E_OUT_OF_MEMORY;
      else
      {
        m_pHashMap->Add( pHashName, 0 );
        delete pHashName;
        m_uNumberEntries++;
        PostChanges( IFX_SUBJECT_PALETTE_ADD );
      }
    }
  }
  return iResult;
}


//---------------------------------------------------------------------------
//  CIFXPalette::Find
//
//  !!! DEPRECATED - USE THE CIFXSTRING VERSION!!
//---------------------------------------------------------------------------
IFXRESULT CIFXPalette::Find( const IFXCHAR* pPaletteEntryName, U32* pPaletteEntryId )
{
  IFXRESULT iResult = IFX_OK;

  if( m_pPalette == NULL )
    iResult = IFX_E_NOT_INITIALIZED;

  if( pPaletteEntryName == NULL )
    iResult = IFX_E_INVALID_POINTER;

  if( pPaletteEntryId == NULL )
    iResult = IFX_E_INVALID_POINTER;

  if( IFXSUCCESS( iResult ) )
  {
    IFXString sName( pPaletteEntryName );
    iResult = Find( &sName, pPaletteEntryId );
  }
  return iResult;
}

//---------------------------------------------------------------------------
//  CIFXPalette::Find
//
//  This function walks through the palette and tries to locate an entry with
//  the specified name.  It returns the index of the entry added, or 0 zero if
//  it couldn't find it.
//---------------------------------------------------------------------------
IFXRESULT CIFXPalette::Find(const IFXString* pPaletteEntryName, U32* pPaletteEntryId )
{
  IFXRESULT iResult = IFX_OK;

  if( m_pPalette == NULL )
    iResult = IFX_E_NOT_INITIALIZED;

  if( pPaletteEntryId == NULL )
    iResult = IFX_E_INVALID_POINTER;

  if( pPaletteEntryName == NULL )
    iResult = IFX_E_INVALID_POINTER;

  if( IFXSUCCESS( iResult ) )
  {
	IFXString sName( pPaletteEntryName );
	iResult = m_pHashMap->Find( &sName, pPaletteEntryId );
  }

  return iResult;
}

//---------------------------------------------------------------------------
//  CIFXPalette::FindByResourcePtr
//
//  This function walks through the palette and tries to locate an entry with
//  the the specified IFXUnknown resource pointer.  It returns the index of the
//  entry if found, or 0 and an error code if it couldn't find it.
//---------------------------------------------------------------------------
IFXRESULT CIFXPalette::FindByResourcePtr( IFXUnknown* pPointer, U32* pIndex )
{
  IFXRESULT iResult = IFX_OK;

  if( pPointer == NULL )
    iResult = IFX_E_INVALID_POINTER;

  if( pIndex == NULL )
    iResult = IFX_E_INVALID_POINTER;

  if( m_pPalette == NULL )
    iResult = IFX_E_NOT_INITIALIZED;

  if( IFXSUCCESS( iResult )  )
  {
    U32 uIndex = 0;
    BOOL bFound = FALSE;

    while( ( uIndex <= m_uPaletteSize ) && ( bFound == FALSE )  )
    {
      if( m_pPalette[uIndex].m_pObject == pPointer )
      {
        bFound = TRUE;
        *pIndex = uIndex;
      }
      uIndex++;
    }

    if( bFound != TRUE  )
    {
      iResult = IFX_E_CANNOT_FIND;
      *pIndex = 0;
    }
  }
  return iResult;
}


//---------------------------------------------------------------------------
//  CIFXPalette::GetName
//
//  !!! DEPRECATED - USE THE CIFXSTRING VERSION!!
//---------------------------------------------------------------------------
IFXRESULT CIFXPalette::GetName( U32 uPaletteEntryId, IFXCHAR* pPaletteEntryName, U32* pNameLength )
{
  IFXRESULT iResult = IFX_OK;

  if( pNameLength == NULL )
    iResult = IFX_E_INVALID_POINTER;

  if( m_pPalette == NULL )
    iResult = IFX_E_NOT_INITIALIZED;

  if( IFXSUCCESS( iResult ) )
  {
    IFXString* pName = new IFXString(  );
    if ( pName == NULL )
      iResult = IFX_E_OUT_OF_MEMORY;
    else
      iResult = GetName( uPaletteEntryId, pName );

    if( IFXSUCCESS( iResult ) && pPaletteEntryName != NULL)
	{
		pPaletteEntryName = (IFXCHAR*)(const IFXCHAR*)pName;
	}

    if( pName  )
      delete pName;
  }

  return iResult;
}

//---------------------------------------------------------------------------
//  CIFXPalette::GetName
//
//  Returns the name of a given palette entry.  It will only copy as much of
//  the name into the destination as the destination will allow to prevent
//  buffer overrun.
//---------------------------------------------------------------------------
IFXRESULT CIFXPalette::GetName( U32 uPaletteEntryId, IFXString* pPaletteEntryName )
{
  IFXRESULT iResult = IFX_OK;

  if( m_pPalette == NULL )
    iResult = IFX_E_NOT_INITIALIZED;

  if( pPaletteEntryName == NULL )
    iResult = IFX_E_INVALID_POINTER;

  if( ( uPaletteEntryId > m_uPaletteSize ) || ( m_pPalette[uPaletteEntryId].m_pName == NULL ) )
    iResult = IFX_E_INVALID_RANGE;

  if( IFXSUCCESS( iResult ) )
  {
    pPaletteEntryName->Assign( m_pPalette[uPaletteEntryId].m_pName );
  }
  return iResult;
}

//---------------------------------------------------------------------------
//  CIFXPalette::GetPaletteSize
//
//  This method returns the current number of items in the palette.  NOT the
//  physical size of the palette ( array size ), but the actual number of filled
//  entries.
//---------------------------------------------------------------------------
IFXRESULT CIFXPalette::GetPaletteSize( U32* pPaletteSize )
{
  IFXRESULT iResult = IFX_OK;

  if( m_pPalette == NULL )
    iResult = IFX_E_NOT_INITIALIZED;
  if( pPaletteSize == NULL )
    iResult = IFX_E_INVALID_POINTER;

  if( IFXSUCCESS( iResult ) )
    *pPaletteSize = m_uNumberEntries;

  return iResult;
}

//---------------------------------------------------------------------------
//  CIFXPalette::DeleteByName
//
//  !!! DEPRECATED - USE THE CIFXSTRING VERSION!!
//---------------------------------------------------------------------------
IFXRESULT CIFXPalette::DeleteByName( const IFXCHAR* pPaletteEntryName )
{
  IFXRESULT iResult = IFX_OK;

  if( m_pPalette == NULL )
    iResult = IFX_E_NOT_INITIALIZED;

  if( pPaletteEntryName == NULL )
    iResult = IFX_E_INVALID_POINTER;

  if( IFXSUCCESS( iResult ) )
  {
    IFXString sName( pPaletteEntryName );
    iResult = DeleteByName( &sName );
  }

  return iResult;
}

//---------------------------------------------------------------------------
//  CIFXPalette::DeleteByName
//
//  Delete's a specific palette entry by name.  It searches the palette for the
//  name and calls DeleteByID with the found id.
//---------------------------------------------------------------------------
IFXRESULT CIFXPalette::DeleteByName( IFXString* pPaletteEntryName )
{
  IFXRESULT iResult = IFX_OK;

  if( m_pPalette == NULL )
    iResult = IFX_E_NOT_INITIALIZED;
  if( pPaletteEntryName == NULL )
    iResult = IFX_E_INVALID_POINTER;

  if( IFXSUCCESS( iResult ) )
  {
    U32 uPaletteID = 0;
    iResult = Find( pPaletteEntryName, &uPaletteID );
    if( IFXSUCCESS( iResult ) )
      iResult = DeleteById( uPaletteID );
  }
  return iResult;

}

//---------------------------------------------------------------------------
//  CIFXPalette::DeleteById
//
//  This function removes a reference count from a palette entry.  This function
//  does not garantee an entry is deleted, only if the last reference count for
//  the object is released(  ).  If there is more than one reference count on an
//  object, it is simply decremented.  If it is the last reference count, the
//  object is released, the memory for the name deallocated, and the entry
//  cleared.
//---------------------------------------------------------------------------
IFXRESULT CIFXPalette::DeleteById( U32 uPaletteEntryId )
{
  IFXRESULT iResult = IFX_OK;

  if ( m_pPalette == NULL )
    iResult = IFX_E_NOT_INITIALIZED;

  if ( uPaletteEntryId > m_uPaletteSize )
    iResult = IFX_E_INVALID_RANGE;

  if ( IFXSUCCESS( iResult ) )
  {
    if( m_pPalette[uPaletteEntryId].m_uNextFreeId == 0 || m_pPalette[uPaletteEntryId].m_pName == NULL )
      iResult = IFX_E_INVALID_RANGE;
  }

  // Don't allow re-setting of default value
  if ( FALSE == m_bDefaultMode  )
  {
    if ( 0 == uPaletteEntryId  )
      iResult = IFX_E_CANNOT_CHANGE;
  }

  if( IFXSUCCESS( iResult ) )
  {
    if( m_pPalette[uPaletteEntryId].m_pName != NULL )
    {
      // delete the hashmap item ( using its name )
      IFXString sName( m_pPalette[uPaletteEntryId].m_pName );
      m_pHashMap->Delete( &sName );

      // delete the palette item name
      delete m_pPalette[uPaletteEntryId].m_pName;
    }

    // Post changes to the object and remove the observer
    if( m_pPalette[uPaletteEntryId].m_pSimpleObject &&
      uPaletteEntryId != IFX_NULL_RESOURCE )
    {
      m_pPalette[uPaletteEntryId].m_pSimpleObject->PostChanges(IFX_SUBJECT_PALETTE_REMOVE_RESOURCE);
    }

    IFXRELEASE( m_pPalette[uPaletteEntryId].m_pSimpleObject );
    IFXRELEASE( m_pPalette[uPaletteEntryId].m_pModChain );
    IFXRELEASE( m_pPalette[uPaletteEntryId].m_pObject );

    BOOL bWasHidden = m_pPalette[uPaletteEntryId].m_bHidden;

    m_pPalette[uPaletteEntryId].m_pName = NULL;
    m_pPalette[uPaletteEntryId].m_uNextFreeId = m_uLastFree;
    m_pPalette[uPaletteEntryId].m_bHidden = FALSE;
    m_uLastFree = uPaletteEntryId;
    m_uNumberEntries--;

    if ( bWasHidden )
      PostChanges( IFX_SUBJECT_PALETTE_REMOVE_HIDDEN );
    else
      PostChanges( IFX_SUBJECT_PALETTE_REMOVE );
  }

  return iResult;
}

//---------------------------------------------------------------------------
//  CIFXPalette::GetResourcePtr
//
//  This function returns the resource pointer for an entry in the palette.
//  It AddRef(  )'s the object, so you must remember to call Release(  ) on it.
//  The object returned always must support the IFXUnknown interface.
//---------------------------------------------------------------------------
IFXRESULT CIFXPalette::GetResourcePtr( U32 uIndex, IFXUnknown** ppObject )
{
  IFXRESULT iResult = IFX_OK;

  if( m_pPalette == NULL )
    iResult = IFX_E_NOT_INITIALIZED;

  if( ppObject == NULL )
    iResult = IFX_E_INVALID_POINTER;

  if( uIndex > m_uPaletteSize )
    iResult = IFX_E_INVALID_RANGE;

  if( IFXSUCCESS( iResult ) )
  {
    if( m_pPalette[uIndex].m_pName == NULL )
      iResult = IFX_E_INVALID_RANGE;
  }

  if( IFXSUCCESS( iResult ) )
  {
    *ppObject = m_pPalette[uIndex].m_pObject;
    if( NULL != m_pPalette[uIndex].m_pObject )
      m_pPalette[uIndex].m_pObject->AddRef();
    else
      iResult = IFX_E_PALETTE_NULL_RESOURCE_POINTER;
  }
  return iResult;
}

//---------------------------------------------------------------------------
//  CIFXPalette::GetResourcePtr
//
//  This function returns the resource pointer for an entry in the palette.
//  It AddRef(  )'s the object, so you must remember to call Release(  ) on it.
//  The object returned always must support the IFXUnknown interface.
//---------------------------------------------------------------------------
IFXRESULT CIFXPalette::GetResourcePtr( U32 uIndex, IFXREFIID interfaceId, void** ppObject )
{
  IFXRESULT iResult = IFX_OK;

  if( m_pPalette == NULL )
    iResult = IFX_E_NOT_INITIALIZED;

  if( ppObject == NULL )
    iResult = IFX_E_INVALID_POINTER;

  if( uIndex > m_uPaletteSize )
    iResult = IFX_E_INVALID_RANGE;

  if( m_pPalette[uIndex].m_pName == NULL )
    iResult = IFX_E_INVALID_RANGE;

  if( IFXSUCCESS( iResult ) )
  {
    //*ppObject = m_pPalette[uIndex].m_pObject;
    *ppObject = NULL;
    if( NULL != m_pPalette[uIndex].m_pObject )
    {
      iResult = m_pPalette[uIndex].m_pObject->QueryInterface( interfaceId, ppObject );
    }
    else
    {
      iResult = IFX_E_PALETTE_NULL_RESOURCE_POINTER;
    }
  }
  return iResult;
}


//---------------------------------------------------------------------------
//  CIFXPalette::SetResourcePtr
//
//  This function sets the resource pointer for a palette entry.  After a name
//  is specified for a palette entry, its contents are set by this function
//  which releases anything its currently pointing to and sets the pointer to
//  this new object.  The object must support the IFXUnknown interface for
//  reference counting.
//---------------------------------------------------------------------------
IFXRESULT CIFXPalette::SetResourcePtr( U32 uIndex, IFXUnknown* pObject )
{
  IFXRESULT iResult = IFX_OK;

  if( m_pPalette == NULL )
    iResult = IFX_E_NOT_INITIALIZED;

  if( uIndex > m_uPaletteSize )
    iResult = IFX_E_INVALID_RANGE;

  if( IFXSUCCESS( iResult ) )
  {
    if( m_pPalette[uIndex].m_pName == NULL )
      iResult = IFX_E_INVALID_RANGE;
  }

  // Don't allow re-setting of default value
  if( FALSE == m_bDefaultMode )
  {
    if( 0 == uIndex )
      iResult = IFX_E_CANNOT_CHANGE;
  }

  if( IFXSUCCESS( iResult ) )
  {
    BOOL bOrigEntryValid = m_pPalette[uIndex].m_pObject ? TRUE : FALSE;

    IFXRELEASE( m_pPalette[uIndex].m_pModChain );
    IFXRELEASE( m_pPalette[uIndex].m_pObject );

    m_pPalette[uIndex].m_pObject = pObject;
    if( pObject )
    {
      IFXModifier* pMod2 = NULL;

      m_pPalette[uIndex].m_pObject->AddRef(  );

      if( IFXSUCCESS( pObject->QueryInterface( IID_IFXModifier, ( void** )&pMod2 ) ) )
      {
        if( IFXFAILURE( pMod2->GetModifierChain( &( m_pPalette[uIndex].m_pModChain ) ) ) )
        {
          IFXSceneGraph* pSceneGraph = NULL;

          iResult = IFXCreateComponent( CID_IFXModifierChain,
                          IID_IFXModifierChain,
                          ( void** )&( m_pPalette[uIndex].m_pModChain )  );
          if( IFXSUCCESS( iResult ) )
          {
            IFXResourceClient* pResourceClient = NULL;

            // Always give the mod chain a clock
            pMod2->GetSceneGraph( &pSceneGraph  );

            IFXSubject* pClockSubject = NULL;
            pSceneGraph->GetSimClockSubject( &pClockSubject  );

            m_pPalette[uIndex].m_pModChain->SetClock( pClockSubject  );
            IFXRELEASE( pClockSubject );

            pMod2->QueryInterface( IID_IFXResourceClient, ( void** )&pResourceClient  );
            if( pResourceClient )
            {
              U32 uResourceIndex = pResourceClient->GetResourceIndex(  );
              iResult = pResourceClient->SetResourceIndex( uResourceIndex );

              if( IFXSUCCESS( iResult ) )
              {
                IFXModifierChain* pResourceChain = NULL;
                IFXModifier* pResourceMod = NULL;
                IFXPalette* pPalette = NULL;

                iResult = pSceneGraph->GetPalette(
                    ( IFXSceneGraph::EIFXPalette )( pResourceClient->GetResourcePalette(  ) ),
                    &pPalette  );

                if( IFXSUCCESS(iResult) )
                  pPalette->GetResourcePtr( uResourceIndex,
                                  IID_IFXModifier,
                                (void**)&pResourceMod  );
                if( pResourceMod )
                {
                  iResult = pResourceMod->GetModifierChain( &pResourceChain );

                  if IFXSUCCESS( iResult )
                    iResult = m_pPalette[uIndex].m_pModChain->
                              PrependModifierChain( pResourceChain );
                }

                IFXRELEASE( pPalette );
                IFXRELEASE( pResourceMod );

                if( IFXSUCCESS( iResult ) )
                  iResult = m_pPalette[uIndex].m_pModChain->
                    PrependModifierChain( pResourceChain );

                IFXRELEASE( pResourceChain );
              }
            }

            IFXRELEASE( pResourceClient );
          }

          if( IFXSUCCESS( iResult ) )
          {
            iResult = ( m_pPalette[uIndex].m_pModChain )->SetModifier( *pMod2, 0, FALSE );
          }

          if( IFXFAILURE( iResult ) )
          {
            IFXRELEASE( m_pPalette[uIndex].m_pModChain );
            IFXRELEASE( m_pPalette[uIndex].m_pObject );
          }

          IFXRELEASE( pSceneGraph );
        }
#ifdef _DEBUG
        else
        {
          // Ensure that only head modifiers are in palettes
          U32 uModIndex = ( U32 )-1;
          pMod2->GetModifierChainIndex( uModIndex  );
          IFXASSERT( uModIndex == 0  );
        }
#endif // _DEBUG

        IFXRELEASE( pMod2 );
      }

      if ( IFXSUCCESS( iResult ) )
      {
        // Notify observers
        PostChanges( m_pPalette[uIndex].m_bHidden ?
               IFX_SUBJECT_PALETTE_ADD_HIDDEN :
               IFX_SUBJECT_PALETTE_ADD );
      }
    }
    else
    {
      if ( bOrigEntryValid )
      {
        // Notify observers
        PostChanges( m_pPalette[uIndex].m_bHidden ?
               IFX_SUBJECT_PALETTE_REMOVE_HIDDEN :
               IFX_SUBJECT_PALETTE_REMOVE );
      }
    }

  }
  return iResult;
}

// Default entry methods
//---------------------------------------------------------------------------
//  CIFXPalette::SetDefaultResourcePtr
//
//  This method
//---------------------------------------------------------------------------
IFXRESULT CIFXPalette::SetDefaultResourcePtr( IFXUnknown* pObject )
{
  IFXRESULT iResult = IFX_OK;

  if( m_pPalette == NULL )
    iResult = IFX_E_NOT_INITIALIZED;
  if( NULL == pObject )
    iResult = IFX_E_INVALID_POINTER;

  if( IFXSUCCESS( iResult ) )
  {
    m_bDefaultMode = TRUE;
    iResult = SetResourcePtr( 0, pObject );
    m_bDefaultMode = FALSE;
  }
  return iResult;
}

//---------------------------------------------------------------------------
//  CIFXPalette::DeleteDefault
//
//  This method
//---------------------------------------------------------------------------
IFXRESULT CIFXPalette::DeleteDefault(  )
{
  IFXRESULT iResult = IFX_OK;

  if( m_pPalette == NULL )
    iResult = IFX_E_NOT_INITIALIZED;

  if( IFXSUCCESS( iResult ) )
  {
    m_bDefaultMode = TRUE;
    iResult = DeleteById( 0 );
    m_bDefaultMode = FALSE;
  }
  return iResult;

}



// IFXPaletteManager Iterator methods

//---------------------------------------------------------------------------
//  CIFXPalette::First ( iterator extension )
//
//  This method takes a palette index and returns the index of the first element
//  in the palette.  It returns 0 if there is no elements in the palette.
//---------------------------------------------------------------------------
IFXRESULT CIFXPalette::First( U32* pID )
{
  IFXRESULT iResult = IFX_OK;
  U32 i = 0;

  // Check for invalid pointer and for empty palette
  if( NULL == pID )
  {
    iResult = IFX_E_INVALID_POINTER;
  }
  else if( ( 0 == m_uPaletteSize ) || ( 0 == m_uNumberEntries ) )
  {
    iResult = IFX_E_PALETTE_INVALID_ENTRY;
  }

  // Find the first valid entry
  if( IFXSUCCESS( iResult ) )
  {
    while( i<m_uPaletteSize && m_pPalette[i].m_pName == NULL )
    {
      i++;
    }

    if( i == m_uPaletteSize && m_pPalette[i].m_pName == NULL )
    {
      iResult = IFX_E_PALETTE_INVALID_ENTRY;
    } else {
      *pID = i;
    }
  }

  // return a zero if no entry was found
  if( IFX_E_PALETTE_INVALID_ENTRY == iResult )
  {
    *pID = 0;
  }

  return iResult;
}

//---------------------------------------------------------------------------
//  CIFXPalette::Last ( iterator extension )
//
//  This method takes a palette index and returns the index of the last element
//  in the palette.  It returns 0 if there is no elements in the palette.
//---------------------------------------------------------------------------
IFXRESULT CIFXPalette::Last( U32* pID )
{
  IFXRESULT iResult = IFX_OK;

  // Check for invalid pointer and for empty palette
  if( NULL == pID )
  {
    iResult = IFX_E_INVALID_POINTER;
  }
  else if( ( 0 == m_uPaletteSize ) || ( 0 == m_uNumberEntries ) )
  {
    iResult = IFX_E_PALETTE_INVALID_ENTRY;
  }

  // Find the last valid entry
  if( IFXSUCCESS( iResult ) )
  {
    U32 i = m_uPaletteSize;
    while( ( i > 0 ) && ( m_pPalette[i].m_pName == NULL ) )
    {
      i--;
    }

    if( i == 0 && m_pPalette[i].m_pName == NULL )
    {
      iResult = IFX_E_PALETTE_INVALID_ENTRY;
    }
    else
    {
      *pID = i;
    }
  }

  // return a zero if no entry was found
  if( IFX_E_PALETTE_INVALID_ENTRY == iResult )
  {
    *pID = 0;
  }

  return iResult;
}

//---------------------------------------------------------------------------
//  CIFXPalette::Next ( iterator extension )
//
//  This method takes a palette index and returns the next occupied one.  It
//  skips over any blank palette entries.  It returns 0 if you've run off the
//  end of the palette.
//---------------------------------------------------------------------------
IFXRESULT CIFXPalette::Next( U32* pID )
{
  IFXRESULT iResult = IFX_OK;

  if( pID == NULL )
    iResult = IFX_E_INVALID_POINTER;
  if( m_uPaletteSize == 0 )
    iResult = IFX_E_PALETTE_INVALID_ENTRY;

  if( IFXSUCCESS( iResult ) )
  {
    if( *pID == m_uPaletteSize )
      iResult = IFX_E_PALETTE_INVALID_ENTRY;
  }

  U32 uIndex;
  if( IFXSUCCESS( iResult ) )
  {
    uIndex = ( *pID+1 );
    while( uIndex<m_uPaletteSize && m_pPalette[uIndex].m_pName == NULL )
    {
      uIndex++;
    }

    if( ( uIndex >= m_uPaletteSize ) && ( m_pPalette[uIndex].m_pName == NULL ) )
    {
      iResult = IFX_E_PALETTE_INVALID_ENTRY;
    }
    else
    {
      *pID = uIndex;
    }
  }
  return iResult;
}

//---------------------------------------------------------------------------
//  CIFXPalette::Previous ( iterator extension )
//
//  This method takes a palette index and returns the previous one.  It skips over
//  any blank palette entries.  It returns 0 if you've run off the beginning.
//---------------------------------------------------------------------------
IFXRESULT CIFXPalette::Previous( U32* pID )
{
  IFXRESULT iResult = IFX_OK;

  if( pID == NULL )
    iResult = IFX_E_INVALID_POINTER;

  if( IFXSUCCESS( iResult ) )
  {
    U32 i = ( *pID-1 );
    if( m_uPaletteSize == 0 || *pID == 0 )
    {
      *pID = 0;
      iResult = IFX_E_PALETTE_INVALID_ENTRY;
    }
    else
    {
      while( ( i > 0 ) && ( m_pPalette[i].m_pName == NULL ) )
        i--;
      if( i == 0 && m_pPalette[i].m_pName == NULL )
        iResult = IFX_E_PALETTE_INVALID_ENTRY;
      else
        *pID = i;
    }
  }
  return iResult;
}

//---------------------------------------------------------------------------
//  CIFXPalette_Factory ( non-singleton )
//
//  This is the CIFXSystemInfo component factory function.  The
//  CIFXSystemInfo component is NOT a singleton.  This function creates the
//  palette object, addref(  )'s it and returns it.
//---------------------------------------------------------------------------
IFXRESULT IFXAPI_CALLTYPE CIFXPalette_Factory( IFXREFIID  interfaceId,
                void**    ppInterface  )
{
  IFXRESULT result;

  if ( ppInterface  )
  {
    // It doesn't exist, so try to create it.
    CIFXPalette *pComponent = new CIFXPalette;

    if ( pComponent  )
    {
      // Perform a temporary AddRef for our usage of the component.
      pComponent->AddRef(  );

      // Attempt to obtain a pointer to the requested interface.
      result = pComponent->QueryInterface( interfaceId, ppInterface  );

      // Perform a Release since our usage of the component is now
      // complete.  Note:  If the QI fails, this will cause the
      // component to be destroyed.
      pComponent->Release(  );
    }
    else
      result = IFX_E_OUT_OF_MEMORY;
  }
  else
    result = IFX_E_INVALID_POINTER;

  return result;
}


IFXRESULT CIFXPalette::SetResourceObserver(U32 uCurrentIndex, U32 uNewIndex, IFXModel* pModel)
{
  IFXRESULT result = IFX_OK;

  if( pModel )
  {
    // Ensure that we have different indices, otherwise
    // simply attempt to Prepend the modChain

    if( uCurrentIndex != uNewIndex )
    {
      IFXObserver* pObserver = NULL;
      U32          uChangeBits = IFX_SUBJECT_PALETTE_ADD_RESOURCE |
                     IFX_SUBJECT_PALETTE_REMOVE_RESOURCE;

      result = pModel->QueryInterface( IID_IFXObserver, (void**)&pObserver );

      if( IFXSUCCESS(result) )
      {
        if( uCurrentIndex == IFX_NULL_RESOURCE )
        {
          // Model's resource is currently NULL and is now being
          // set to a valid resource

          if( m_pPalette[uNewIndex].m_pSimpleObject == NULL )
          {
            // Create a new simpleObject for this slot and
            // observe the node on it
            result = IFXCreateComponent( CID_IFXSimpleObject,
                           IID_IFXSubject,
                           (void**)&(m_pPalette[uNewIndex].m_pSimpleObject) );
          }

          if( IFXSUCCESS(result) )
            result = m_pPalette[uNewIndex].m_pSimpleObject->Attach( pObserver,
                                        uChangeBits );
          if( IFXSUCCESS(result) )
            m_pPalette[uNewIndex].m_pSimpleObject->PostChanges( IFX_SUBJECT_PALETTE_ADD_RESOURCE );
        }
        else
        {
          // Model's resource is being set to either
          //    a) NULL - if uNewIndex == IFX_NULL_RESOURCE
          //    b) a different resource

          // Detach the model's current observer

		  if (m_pPalette[uCurrentIndex].m_pSimpleObject)
			result = m_pPalette[uCurrentIndex].m_pSimpleObject->Detach( pObserver );

          if( uNewIndex != IFX_NULL_RESOURCE )
          {
            // Set the current resource to the new resource

            if( m_pPalette[uNewIndex].m_pSimpleObject == NULL )
            {
              // Create a new simpleObject for this slot and
              // observe the node on it
              result = IFXCreateComponent( CID_IFXSimpleObject,
                             IID_IFXSubject,
                             (void**)&(m_pPalette[uNewIndex].m_pSimpleObject) );
            }

            if( IFXSUCCESS(result) )
              result = m_pPalette[uNewIndex].m_pSimpleObject->Attach( pObserver,
                                          uChangeBits );
            if( IFXSUCCESS(result) )
              m_pPalette[uNewIndex].m_pSimpleObject->PostChanges( IFX_SUBJECT_PALETTE_ADD_RESOURCE );
          }
        }
      }

      IFXRELEASE(pObserver);
    }

    // Prepend modChains

    if( IFXSUCCESS(result) )
    {
      IFXModifierChain* pRsrcModChain = NULL;
      IFXModifierChain* pModChain     = NULL;
      IFXModifier*     pMod2         = NULL;

      GetResourcePtr( uNewIndex, IID_IFXModifier, (void**)&pMod2 );

      if( pMod2 )
        pMod2->GetModifierChain(&pRsrcModChain);

      pModel->GetModifierChain(&pModChain);

      if ( pModChain )
      {
        if( pRsrcModChain )
          result = pModChain->PrependModifierChain(pRsrcModChain);
        else
          result = pModChain->PrependModifierChain(NULL);
      }

      IFXRELEASE(pRsrcModChain);
      IFXRELEASE(pModChain);
      IFXRELEASE(pMod2);
    }
  }
  else
    result = IFX_E_INVALID_POINTER;

  return result;
}


