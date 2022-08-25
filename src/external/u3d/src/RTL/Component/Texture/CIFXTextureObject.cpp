//***************************************************************************
//
//  Copyright (c) 2001 - 2006 Intel Corporation
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

/**
	@file	CIFXTextureObject.cpp

			Main implementation file for the texture object.
*/

#include "CIFXTextureObject.h"
#include "IFXImageCodec.h"
#include "IFXPalette.h"
#include "IFXCheckX.h"
#include "IFXBitStreamX.h"
#include "IFXModifierChain.h"
#include "IFXModifierDataPacket.h"
#include "IFXDids.h"
#include "IFXMetaDataX.h"
#include "IFXReadBuffer.h"

// one ID manager for all texture objects...
IFXIDManagerPtr CIFXTextureObject::ms_spIDManager;

// constructor/destructor...
CIFXTextureObject::CIFXTextureObject()
{
  // IFXUnknown attributes...
  m_uRefCount     = 0;
  m_uLoadId     = 0;

  // Static object initialization
  if(ms_spIDManager.IsValid())
  {
    ms_spIDManager.IncRef();
  }
  else
  {
    ms_spIDManager.Create(CID_IFXIDManager, IID_IFXIDManager);

    // Texture Id 1 is reserved for the rendering system
    // to use as a dummy texture.  The first Id given by
    // a new IDManager is always 1.  This will prevent
    // any texture from getting Id 1.
    U32 uTexId = 0;
    ms_spIDManager->GetId(uTexId);
  }

  // CIFXTextureObject attributes...
  m_bInitialized    = IFX_FALSE;
  m_eTextureMapType = IFX_TEXTURE_2D;
  m_uWidth      = 0;
  m_uHeight     = 0;
  m_bHasAlphaColor  = IFX_FALSE;
  m_eRenderFormat   = IFX_FMT_DEFAULT;  // default render state
  m_eChannelOrder   = IFX_BGRA;
  m_uPitch      = 0;
  m_uPriority   = 1024 ;  // default priority * IFX_SHADERLITTEXTURE_PRIORITY_FACTOR
  m_uImageBufferSize  = 0;
  m_uPixelSize  = 3;
  m_uFormat     = TEXTURE_DEFAULT_FORMAT;
  m_bAnimated     = TEXTURE_DEFAULT_ANIMATED;
  m_eMagMode      = IFX_LINEAR;
  m_eMinMode      = IFX_LINEAR;
  m_eMipMode      = IFX_LINEAR;
  m_eCompressionState = IFXTEXTURECOMPRESSIONSTATE_FORCEDECOMPRESS;
  m_bImageDirty   = TRUE;
  m_bBlockQueueDirty  = TRUE;
  m_bOutputQueueDirty = TRUE;
  m_bKeepDecompressed = FALSE;
  m_bKeepCompressed   = TRUE;
  m_bDone       = TRUE;
  m_pCoreServices   = NULL;
  m_pImageBuffer    = NULL;
  m_pDataBlockQueueX   = NULL;
  m_pOutputBlockQueueX = NULL;
  m_pImageCodec		= NULL;
  memset(&m_ImageInfo, 0, sizeof(m_ImageInfo));
  m_pTextureName     = NULL;

  ms_spIDManager->GetId(m_uId);
  m_uVersion = 0;

  m_pCubeMapTexture = NULL;

  m_sImageInfo.m_imageType  = TEXTURE_DEFAULT_FORMAT;
  m_sImageInfo.m_name    = L"";
  m_sImageInfo.m_blockCompressionType[0] = TextureType_Jpeg24;
  m_sImageInfo.m_compressionQuality = 100;
  m_sImageInfo.m_height = 0;
  m_sImageInfo.m_width  = 0;
  m_sImageInfo.m_size   = 0;
  m_sImageInfo.m_pCodecCID   = NULL;
  m_pDefaultCodec    = NULL;
  m_uUserData      = 0;
  m_eSource    = IFXTEXURESOURCE_OTHER;
}

CIFXTextureObject::~CIFXTextureObject()
{
  // Release the reset of the data
  IFXRELEASE(m_pCoreServices);
  IFXRELEASE(m_pInputDataPacket);
  IFXRELEASE(m_pModifierDataPacket);

  IFXRELEASE(m_pDataBlockQueueX);
  IFXRELEASE(m_pOutputBlockQueueX);
  IFXRELEASE(m_pImageCodec);

  if (m_pImageBuffer)
  {
    IFXDeallocate(m_pImageBuffer);
    m_pImageBuffer = NULL;
  }
  IFXDELETE_ARRAY(m_ImageInfo.m_pData);

  if (m_pTextureName)
    delete m_pTextureName;

  if (m_pDefaultCodec)
  {
    IFXDeallocate(m_pDefaultCodec);
    m_pDefaultCodec = NULL;
  }

  if(ms_spIDManager.IsValid())
  {
    ms_spIDManager->ReleaseId(m_uId);
  }
  ms_spIDManager.DecRef();

  if (m_pCubeMapTexture)
  {
    U32 i = 0;
    for (i=0; i<MAX_CUBIC_MAP_SIZE; i++)
    {
      if (m_pCubeMapTexture->pCubeMapTexName[i])
        delete m_pCubeMapTexture->pCubeMapTexName[i];
      if (m_pCubeMapTexture->pTextureObjectList[i])
        IFXRELEASE(m_pCubeMapTexture->pTextureObjectList[i]);
    }
    delete m_pCubeMapTexture;
  }
}


//---------------------------------------------------------------------------
//  CIFXTextureObject_Factory (non-singleton)
//
//  This is the CIFXTextureObject component factory function.  The
//  CIFXTextureObject component is not a singleton.
//---------------------------------------------------------------------------
IFXRESULT IFXAPI_CALLTYPE CIFXTextureObject_Factory( IFXREFIID interfaceId, void** ppInterface )
{
  IFXRESULT iResult;

  if ( ppInterface )
  {
    // Try to create it.
    CIFXTextureObject *pComponent = new CIFXTextureObject;

    if ( pComponent )
    {
      // Perform a temporary AddRef for our usage of the component.
      pComponent->AddRef();

      // Attempt to obtain a pointer to the requested interface.
      iResult = pComponent->QueryInterface( interfaceId, ppInterface );

      // Perform a Release since our usage of the component is now
      // complete.  Note:  If the QI fails, this will cause the
      // component to be destroyed.
      pComponent->Release();
    }
    else
      iResult = IFX_E_OUT_OF_MEMORY;
  }
  else
    iResult = IFX_E_INVALID_POINTER;

  return iResult;
}


// IFXUnknown methods...
U32 CIFXTextureObject::AddRef(void) 
{
  return ++m_uRefCount;
}

U32 CIFXTextureObject::Release(void) 
{
  if (--m_uRefCount == 0) 
  {
    delete this;
    return 0;
  }
  return m_uRefCount;
}

IFXRESULT CIFXTextureObject::QueryInterface( IFXREFIID riid, void **ppv)
{
  //IFXRESULT rc = IFX_OK;
  if (NULL == ppv)
  {
    return IFX_E_INVALID_POINTER;
  }

  if (IID_IFXTextureObject == riid)
    *ppv = (IFXTextureObject *) this;
  else if (IID_IFXMarker == riid)
    *ppv = (IFXMarker *) this;
  else if (IID_IFXMarkerX == riid)
    *ppv = (IFXMarkerX *) this;
  else if (IID_IFXModifier == riid)
    *ppv = (IFXModifier *) this;
  else if (IID_IFXUnknown == riid)
    *ppv = (IFXUnknown*)this;
  else if(IID_IFXDecoderX == riid)
    *ppv = ( IFXDecoderX* ) this;
  else if(IID_IFXEncoderX == riid)
    *ppv = ( IFXEncoderX* ) this;
  else if(IID_IFXMetaDataX == riid)
    *ppv = ( IFXMetaDataX* ) this;
  else
    return IFX_E_UNSUPPORTED;

  AddRef();

  return IFX_OK;
}

//IFXMarker Method.
IFXRESULT CIFXTextureObject::InitializeObject()
{
  IFXRESULT iResult = IFX_OK;
  if(IFXSUCCESS(iResult))
  {
    IFXRELEASE(m_pCoreServices);
    iResult = m_pSceneGraph->GetCoreServices( &m_pCoreServices );
  }

  if(IFXSUCCESS(iResult))
    iResult = IFXCreateComponent(
					CID_IFXDataBlockQueueX, 
					IID_IFXDataBlockQueueX, 
					(void**)&m_pDataBlockQueueX);
  if(IFXSUCCESS(iResult))
    iResult = IFXCreateComponent(
					CID_IFXDataBlockQueueX, 
					IID_IFXDataBlockQueueX, 
					(void**)&m_pOutputBlockQueueX);

  if(IFXSUCCESS(iResult))
    m_bInitialized = TRUE;

  return iResult;
}

//---------------------------------------------------------------------------
//  CIFXTextureObject::SetDeaultTexture()
//
//  Default texture will be set to 2x2 of pink checker board image.
//  This creates the internal imagebuffer and it's properties.
//---------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::SetDefaultTexture()
{
  IFXRESULT iResult = IFX_OK;
  // set texture attributes...
  U32 pBuffer[48];
  STextureSourceInfo sImageInfo;

  pBuffer[0] = SET_ENDIAN32( 0xFFFFFFFF );
  pBuffer[1] = SET_ENDIAN32( 0xFFFFFFFF );
  pBuffer[2] = SET_ENDIAN32( 0xFFFFFFFF );
  pBuffer[3] = SET_ENDIAN32( 0xFF3366FF );
  pBuffer[4] = SET_ENDIAN32( 0x66FF3366 );
  pBuffer[5] = SET_ENDIAN32( 0x3366FF33 );
  pBuffer[6] = SET_ENDIAN32( 0xFFFFFFFF );
  pBuffer[7] = SET_ENDIAN32( 0xFFFFFFFF );
  pBuffer[8] = SET_ENDIAN32( 0xFFFFFFFF );
  pBuffer[9] = SET_ENDIAN32( 0xFF3366FF );
  pBuffer[10] = SET_ENDIAN32( 0x66FF3366 );
  pBuffer[11] = SET_ENDIAN32( 0x3366FF33 );
  pBuffer[12] = SET_ENDIAN32( 0xFFFFFFFF );
  pBuffer[13] = SET_ENDIAN32( 0xFFFFFFFF );
  pBuffer[14] = SET_ENDIAN32( 0xFFFFFFFF );
  pBuffer[15] = SET_ENDIAN32( 0xFF3366FF );
  pBuffer[16] = SET_ENDIAN32( 0x66FF3366 );
  pBuffer[17] = SET_ENDIAN32( 0x3366FF33 );
  pBuffer[18] = SET_ENDIAN32( 0xFFFFFFFF );
  pBuffer[19] = SET_ENDIAN32( 0xFFFFFFFF );
  pBuffer[20] = SET_ENDIAN32( 0xFFFFFFFF );
  pBuffer[21] = SET_ENDIAN32( 0xFF3366FF );
  pBuffer[22] = SET_ENDIAN32( 0x66FF3366 );
  pBuffer[23] = SET_ENDIAN32( 0x3366FF33 );

  pBuffer[24] = SET_ENDIAN32( 0xFF3366FF );
  pBuffer[25] = SET_ENDIAN32( 0x66FF3366 );
  pBuffer[26] = SET_ENDIAN32( 0x3366FF33 );
  pBuffer[27] = SET_ENDIAN32( 0xFFFFFFFF );
  pBuffer[28] = SET_ENDIAN32( 0xFFFFFFFF );
  pBuffer[29] = SET_ENDIAN32( 0xFFFFFFFF );
  pBuffer[30] = SET_ENDIAN32( 0xFF3366FF );
  pBuffer[31] = SET_ENDIAN32( 0x66FF3366 );
  pBuffer[32] = SET_ENDIAN32( 0x3366FF33 );
  pBuffer[33] = SET_ENDIAN32( 0xFFFFFFFF );
  pBuffer[34] = SET_ENDIAN32( 0xFFFFFFFF );
  pBuffer[35] = SET_ENDIAN32( 0xFFFFFFFF );
  pBuffer[36] = SET_ENDIAN32( 0xFF3366FF );
  pBuffer[37] = SET_ENDIAN32( 0x66FF3366 );
  pBuffer[38] = SET_ENDIAN32( 0x3366FF33 );
  pBuffer[39] = SET_ENDIAN32( 0xFFFFFFFF );
  pBuffer[40] = SET_ENDIAN32( 0xFFFFFFFF );
  pBuffer[41] = SET_ENDIAN32( 0xFFFFFFFF );
  pBuffer[42] = SET_ENDIAN32( 0xFF3366FF );
  pBuffer[43] = SET_ENDIAN32( 0x66FF3366 );
  pBuffer[44] = SET_ENDIAN32( 0x3366FF33 );
  pBuffer[45] = SET_ENDIAN32( 0xFFFFFFFF );
  pBuffer[46] = SET_ENDIAN32( 0xFFFFFFFF );
  pBuffer[47] = SET_ENDIAN32( 0xFFFFFFFF );

  sImageInfo.m_name = L"DefaultTexture";
  sImageInfo.m_height = 8;
  sImageInfo.m_width  = 8;
  sImageInfo.m_size = (sImageInfo.m_height * sImageInfo.m_width * 3);
  sImageInfo.m_imageType = IFXTEXTUREMAP_FORMAT_RGB24;
  sImageInfo.m_pCodecCID = NULL;

  iResult = SetAnimated(FALSE);
  if(IFXSUCCESS(iResult))
    iResult = SetRawImage(&sImageInfo, pBuffer);

  // tell the source not to throw away its source image
  if(IFXSUCCESS(iResult))
    iResult = SetKeepCompressed( FALSE);

  // tell the source not to throw away its Raw image
  if(IFXSUCCESS(iResult))
    iResult = SetKeepDecompressed( TRUE );  

  // Merge note: use IFXTextureFilterNearest instead of 0
  if(IFXSUCCESS(iResult))
    iResult = SetMagFilterMode(IFX_NEAREST); 

  return iResult;
}


//-----------------------------------------------------------------------------
// CIFXTextureObject::SetPriority
//
// Set the priority for the marker and this object.
//-----------------------------------------------------------------------------
void CIFXTextureObject::SetPriority( 
				U32 uInPriority,
                BOOL bRecursive,   // optional. default == IFX_TRUE.
                BOOL bPromotionOnly )  // optional. default == IFX_TRUE.
{

  CIFXMarker::SetPriority( uInPriority, bRecursive, bPromotionOnly );
  if ( CIFXMarker::GetPriority() < m_uPriority)
  {
    m_uPriority = CIFXMarker::GetPriority();

    //If the BlockQueue is not dirty, that means the
    //Blockqueue has created.
    if (!m_bBlockQueueDirty)
    {
      //Just because someone changes his/her mind about
      //the priority, so we need to reset the priority here again.
      //We could set the Input queue dirty, then it requires to
      //redo the compression over again and would take time.
      //But we are doing this way so it would be faster.

      try 
	  {
        // This is a hack to fix the Priority Texture problem.
        // Let move it to the SetPriority function to have fix.
        IFXDECLARELOCAL(IFXDataBlockQueueX,pTmpBlockQueueX);
        IFXCHECKX(IFXCreateComponent(
						CID_IFXDataBlockQueueX, 
						IID_IFXDataBlockQueueX,
						(void**)&pTmpBlockQueueX));
        if (NULL == pTmpBlockQueueX) 
		{
          IFXCHECKX(IFX_E_OUT_OF_MEMORY);
        }

        BOOL bDone = FALSE;
        while(FALSE == bDone) 
		{
          IFXDECLARELOCAL(IFXDataBlockX,pTmpDataBlockX);
          m_pDataBlockQueueX->GetNextBlockX(pTmpDataBlockX,bDone);
          if( pTmpDataBlockX ) 
		  {
              pTmpDataBlockX->SetPriorityX(m_uPriority);
              pTmpBlockQueueX->AppendBlockX(*pTmpDataBlockX);
          }
        }

        IFXRELEASE(m_pDataBlockQueueX);
        pTmpBlockQueueX->CopyX(m_pDataBlockQueueX);

        m_bBlockQueueDirty = FALSE;
      }

      catch (IFXException e) 
	  {
        ; // Catch any IFXException and do nothing with it
      }
    }
  }
  return;
}


//-----------------------------------------------------------------------------
// CIFXTextureObject::MakeDirty
//
// Remove this TextureObject from video memory.
//-----------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::MakeDirty(IFXRenderContext* pRenderContext)
{
  IFXRESULT iResult=IFX_OK;

  m_bImageDirty = TRUE;
  if (m_pModifierDataPacket)
	  m_pModifierDataPacket->InvalidateDataElement(m_uTextureDataElementIndex);

  return iResult;
}


// IFXMarkerX interface...
void CIFXTextureObject::GetEncoderX(IFXEncoderX*& rpEncoderX)
{
  rpEncoderX = (IFXEncoderX*)this;
  AddRef();
}


// IFXTextureObject methods...

// IFXAttributes methods...

//-----------------------------------------------------------------------------
// CIFXTextureObject::GetWidth
//
// Retrieve the width dimension of this texture object.
//-----------------------------------------------------------------------------

IFXRESULT CIFXTextureObject::GetWidth( U32 *puValue)
{
  IFXRESULT iResult = IFX_E_INVALID_POINTER;
  if (puValue)
  {
    if (!m_bInitialized)
      iResult = IFX_TEXTURE_OBJECT_NOT_INITIALIZED;
    else
    {
      iResult = IFX_OK;
      *puValue = m_uWidth;
    }
  }
  return iResult;

}

//-----------------------------------------------------------------------------
// CIFXTextureObject::SetWidth
//
// Set the new Widthdimension for this texture object.
//-----------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::SetWidth( U32  uValue)
{
  IFXRESULT iResult = IFX_OK;

  if (uValue < 1)
    return IFX_E_INVALID_RANGE;
  m_uWidth = uValue;

  //In the case that the user wants to scale up/down.
  // We remove the texture obj. from video to force
  // to do re-evaluate the whole image again.
  MakeDirty(NULL);

  return iResult;
}


//-----------------------------------------------------------------------------
// CIFXTextureObject::GetHeight
//
// Retrieve the width dimension of this texture object.
//-----------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::GetHeight( U32 *puValue)
{
  IFXRESULT iResult = IFX_E_INVALID_POINTER;
  if (puValue)
  {
    if (!m_bInitialized)
      iResult = IFX_TEXTURE_OBJECT_NOT_INITIALIZED;
    else
    {
      iResult = IFX_OK;
      *puValue = m_uHeight;
    }
  }
  return iResult;
}


//-----------------------------------------------------------------------------
// CIFXTextureObject::SetHeight
//
// Set the new Height dimension for this texture object.
//-----------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::SetHeight( U32  uValue)
{
  IFXRESULT iResult = IFX_OK;

  if (uValue < 1)
    return IFX_E_INVALID_RANGE;

  m_uHeight = uValue;
  // In the case that the user wants to scale up/down.
  // We remove the texture obj. from video to force
  // to do re-evaluate the whole image again.
  MakeDirty(NULL);

  return iResult;
}


//-----------------------------------------------------------------------------
// CIFXTextureObject::GetFormat
//
// Retrieve the raw image's format of this texture object.
//-----------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::GetFormat( U32 *puValue)
{
  IFXRESULT iResult = IFX_E_INVALID_POINTER;
  if (puValue)
  {
    if (!m_bInitialized)
      iResult = IFX_TEXTURE_OBJECT_NOT_INITIALIZED;
    else
    {
      iResult = IFX_OK;
      *puValue = m_uFormat;
    }
  }
  return iResult;
}

//-----------------------------------------------------------------------------
// CIFXTextureObject::SetFormat
//
// Set the new raw image's format for this texture object.
//-----------------------------------------------------------------------------

IFXRESULT CIFXTextureObject::SetFormat( U32  uValue)
{
  IFXRESULT iResult = IFX_OK;

  m_uFormat = uValue;

  return iResult;
}


//-----------------------------------------------------------------------------
// CIFXTextureObject::GetRenderFormat
//
// Retrieve Render Format that uses by the hardware.
//-----------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::GetRenderFormat(IFXenum& eFormat)
{
  IFXRESULT iResult = IFX_OK;
  if (!m_bInitialized)
    iResult = IFX_TEXTURE_OBJECT_NOT_INITIALIZED;
  else
  {
    eFormat = m_eRenderFormat;
  }

  return iResult;
}


//-----------------------------------------------------------------------------
// CIFXTextureObject::SetRenderFormat
//
// Set new Render Format that uses by the hardware.
//-----------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::SetRenderFormat(IFXenum eFormat)
{
  IFXRESULT iResult = IFX_OK;

  m_eRenderFormat = eFormat;
  return iResult;
}


//-----------------------------------------------------------------------------
// CIFXTextureObject::GetAnimated
//
// Retrieve animated properties of this texture object.
//-----------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::GetAnimated( BOOL *pbValue)
{
  IFXRESULT iResult = IFX_E_INVALID_POINTER;
  if (pbValue)
  {
    if (!m_bInitialized)
      iResult = IFX_TEXTURE_OBJECT_NOT_INITIALIZED;
    else
    {
      iResult = IFX_OK;
      *pbValue = m_bAnimated;
    }
  }
  return iResult;
}


//-----------------------------------------------------------------------------
// CIFXTextureObject::SetAnimated
//
// Set animated properties of this texture object.
//-----------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::SetAnimated( BOOL  bValue)
{
  IFXRESULT iResult = IFX_OK;

  m_bAnimated = bValue;

  return iResult;
}

//-----------------------------------------------------------------------------
// CIFXTextureObject::GetKeepDecompressed
//
// Retrieve KeepDecompressed properties of this texture object.
//-----------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::GetKeepDecompressed( BOOL *pbValue)
{
  IFXRESULT iResult = IFX_E_INVALID_POINTER;
  if (pbValue)
  {
    if (!m_bInitialized)
      iResult = IFX_TEXTURE_OBJECT_NOT_INITIALIZED;
    else
    {
      iResult = IFX_OK;
      *pbValue = m_bKeepDecompressed;
    }
  }
  return iResult;
}


//-----------------------------------------------------------------------------
// CIFXTextureObject::SetKeepDecompressed
//
// Set KeepDecompressed properties for this texture object.
//-----------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::SetKeepDecompressed( BOOL  bValue)
{
  IFXRESULT iResult = IFX_OK;

  m_bKeepDecompressed = bValue;

  return iResult;
}



//-----------------------------------------------------------------------------
// CIFXTextureObject::GetKeepCompressed
//
// Retrieve KeepCompressed properties of this texture object.
//-----------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::GetKeepCompressed( BOOL *pbValue)
{
  IFXRESULT iResult = IFX_E_INVALID_POINTER;

  if (pbValue)
  {
    if (!m_bInitialized)
      iResult = IFX_TEXTURE_OBJECT_NOT_INITIALIZED;
    else
    {
      *pbValue = m_bKeepCompressed;
      iResult = IFX_OK;
    }
  }
  return iResult;
}


//-----------------------------------------------------------------------------
// CIFXTextureObject::SetKeepCompressed
//
// Set KeepCompressed properties for this texture object.
//-----------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::SetKeepCompressed( BOOL  bValue)
{
  IFXRESULT iResult = IFX_OK;

  m_bKeepCompressed = bValue;

  return iResult;
}


//-----------------------------------------------------------------------------
// CIFXTextureObject::GetRawImageBufferSize
//
// Retrieve the image buffer size of this texture object.
//-----------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::GetRawImageBufferSize( U32 *puValue)
{
  IFXRESULT iResult = IFX_E_INVALID_POINTER;
  if (puValue)
  {
    *puValue = 0;
    if (!m_bInitialized)
      iResult = IFX_TEXTURE_OBJECT_NOT_INITIALIZED;
    else
    {
      *puValue = m_uImageBufferSize;
      iResult = IFX_OK;
    }
  }
  return iResult;
}


//-----------------------------------------------------------------------------
// CIFXTextureObject::GetMagFilterMode
//
// Retrieve the Magnification Filter mode properties of this texture object.
//-----------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::GetMagFilterMode( IFXenum *peFilterMode)
{
  IFXRESULT iResult = IFX_E_INVALID_POINTER;

  if (peFilterMode)
  {
    if (!m_bInitialized)
      iResult = IFX_TEXTURE_OBJECT_NOT_INITIALIZED;
    else
    {
      *peFilterMode = m_eMagMode;
      iResult = IFX_OK;
    }
  }
  return iResult;
}


//-----------------------------------------------------------------------------
// CIFXTextureObject::SetMagFilterMode
//
// Set the Magnification Filter mode properties from this texture object.
//-----------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::SetMagFilterMode( IFXenum  eFilterMode)
{
  IFXRESULT iResult = IFX_OK;

  m_eMagMode = eFilterMode;

  return iResult;
}


//-----------------------------------------------------------------------------
// CIFXTextureObject::GetMinFilterMode
//
// Retrieve the Minification Filter mode properties of this texture object.
//-----------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::GetMinFilterMode( IFXenum *peFilterMode)
{
  IFXRESULT iResult = IFX_OK;

  if (peFilterMode)
  {
    if (!m_bInitialized)
      iResult = IFX_TEXTURE_OBJECT_NOT_INITIALIZED;
    else
    {
      *peFilterMode = m_eMinMode;
      iResult = IFX_OK;
    }
  }
  return iResult;
}


//-----------------------------------------------------------------------------
// CIFXTextureObject::SetMinFilterMode
//
// Set the Minification Filter mode properties from this texture object.
//-----------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::SetMinFilterMode( IFXenum  eFilterMode)
{
  IFXRESULT iResult = IFX_OK;

  m_eMinMode = eFilterMode;

  return iResult;
}


//-----------------------------------------------------------------------------
// CIFXTextureObject::GetMipMode
//
// Retrieve the MipMap mode properties of this texture object.
//-----------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::GetMipMode( IFXenum *peMipMode)
{
  IFXRESULT iResult = IFX_OK;

  if (NULL == peMipMode)
    iResult = IFX_E_INVALID_POINTER;
  else
    *peMipMode = m_eMipMode;
  return iResult;
}


//-----------------------------------------------------------------------------
// CIFXTextureObject::SetMipMode
//
// Set the MipMap mode properties for this texture object.
//-----------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::SetMipMode( IFXenum eMipMode)
{
  IFXRESULT iResult = IFX_OK;

  m_eMipMode = eMipMode;
  return iResult;
}

//-----------------------------------------------------------------------------
// CIFXTextureObject::GetTextureSource
//
// Retrieve the type of this texture object.
//-----------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::GetTextureSource( IFXenum *peTextureSource)
{
  IFXRESULT iResult = IFX_OK;

  if (NULL == peTextureSource)
    iResult = IFX_E_INVALID_POINTER;
  else
    *peTextureSource = m_eSource;
  return iResult;
}


//-----------------------------------------------------------------------------
// CIFXTextureObject::SetTextureSource
//
// Set the type of this texture object.
//-----------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::SetTextureSource( IFXenum eTextureSource)
{
  IFXRESULT iResult = IFX_OK;

  m_eSource = eTextureSource;

  return iResult;
}

//-----------------------------------------------------------------------------
// CIFXTextureObject::GetTextureType
//
// Retrieve the texture map type of this texture object.
//-----------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::GetTextureType( IFXenum *puMapType)
{
  IFXRESULT iResult = IFX_OK;

  *puMapType = m_eTextureMapType;

  return iResult;
}

//-----------------------------------------------------------------------------
// CIFXTextureObject::SetTextureType
//
// Set the texture map type of this texture object.
//-----------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::SetTextureType( IFXenum  uMapType)
{
  IFXRESULT iResult = IFX_OK;
  IFXPalette* pTexturePalette = NULL;
  IFXUnknown* punk      = NULL;
  U32     uTmpTextureID = 0;

  m_eTextureMapType = uMapType;

  //For now we only worry about this two types
  if (uMapType != IFX_TEXTURE_CUBE  &&
    uMapType != IFX_TEXTURE_2D)
  {
    iResult = IFX_E_UNSUPPORTED;
    //Reset back to default;
    m_eTextureMapType = IFX_TEXTURE_2D;
  }

  if ( IFX_TEXTURE_CUBE== m_eTextureMapType)
  {
    //We are doing some initialization here.
    if (NULL == m_pCubeMapTexture)
    {
      m_pCubeMapTexture = new CubeMapTexture;
      if (m_pCubeMapTexture)
      {
        U32 i = 0;
        for (i=0; i<MAX_CUBIC_MAP_SIZE; i++)
        {
          m_pCubeMapTexture->puTextureId[i] = 0;
          m_pCubeMapTexture->pCubeMapTexName[i] = NULL;
          m_pCubeMapTexture->pTextureObjectList[i] = NULL;
        }
      }
      else
        iResult = IFX_E_OUT_OF_MEMORY;
    }

    //Get the name of this texture for the palette.
    if(IFXSUCCESS(iResult) && m_pSceneGraph)
    {
      iResult = m_pSceneGraph->GetPalette( IFXSceneGraph::TEXTURE, &pTexturePalette);
    }
    if (IFXSUCCESS(iResult))
    {
      iResult = this->QueryInterface(IID_IFXUnknown,(void**)&punk);
    }

    if(IFXSUCCESS(iResult))
    {
      iResult = pTexturePalette->FindByResourcePtr(punk,&uTmpTextureID);
    }
    if(IFXSUCCESS(iResult))
    {
      m_pTextureName = new IFXString;
      if (NULL == m_pTextureName)
        iResult = IFX_E_OUT_OF_MEMORY;
    }

    if(IFXSUCCESS(iResult))
    {
      iResult = pTexturePalette->GetName(uTmpTextureID, m_pTextureName);
    }
    //Force the output blockqueue to refresh itself
    if(IFXSUCCESS(iResult))
      m_bOutputQueueDirty = TRUE;
  }

  return iResult;
}


//-----------------------------------------------------------------------------
// CIFXTextureObject::GetCubeMapTexture
//
//
//-----------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::GetCubeMapTexture( IFXenum ePosition, IFXTextureObject **ppTexture)
{
  IFXRESULT iResult = IFX_OK;

  IFXPalette* pTexturePalette = NULL;
  IFXUnknown* punk      = NULL;
  U8      uIndex = 0;
  U32     uTmpTextureID = 0;

  if (!m_bInitialized)
    iResult = IFX_E_NOT_INITIALIZED;

  if ( ePosition < IFX_TEXTURE_CUBE_PX &&
     ePosition > IFX_TEXTURE_CUBE_NZ)
     iResult = IFX_E_INVALID_RANGE;

  //Translate the define ePosition into local array index
  uIndex = (U8)ePosition - IFX_TEXTURE_CUBE_PX;

  if(m_pCubeMapTexture)
  {
    if(IFXSUCCESS(iResult))
    {
      iResult = m_pSceneGraph->GetPalette( 
									IFXSceneGraph::TEXTURE, 
									&pTexturePalette);
    }

    if(IFXSUCCESS(iResult))
    {
      iResult = pTexturePalette->Find(
									m_pCubeMapTexture->pCubeMapTexName[uIndex],
									&uTmpTextureID);
    }

    if(IFXSUCCESS(iResult))
    {
      //We do cross check here to see if there's any changes
      //in the Texture Palette.  We reassigned the texture id
      //that match with the name.  We try our best to get the
      //correct texture obj.
      if (uTmpTextureID != m_pCubeMapTexture->puTextureId[uIndex])
        iResult  = IFX_E_CANNOT_FIND;
      else
        m_pCubeMapTexture->puTextureId[uIndex] = uTmpTextureID;

    }

    if (IFXSUCCESS(iResult))
    {
      iResult = pTexturePalette->GetResourcePtr(uTmpTextureID,&punk);
    }

    if (IFXSUCCESS(iResult))
    {
      iResult = punk->QueryInterface(IID_IFXTextureObject,
                    (void**)ppTexture);
    }
  }
  else
  {
    *ppTexture = NULL;
  }

  IFXRELEASE(punk);

  IFXRELEASE(pTexturePalette);

  return iResult;
}


//-----------------------------------------------------------------------------
// CIFXTextureObject::SetCubeMapTexture
//
//
//-----------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::SetCubeMapTexture( U32 uTextureId, IFXenum ePosition)
{
  IFXRESULT iResult = IFX_OK;
  IFXPalette* pTexturePalette = NULL;
  U8      uIndex = 0;

  if (!m_bInitialized && NULL == m_pCubeMapTexture)
    iResult = IFX_E_NOT_INITIALIZED;

  if ( ePosition < IFX_TEXTURE_CUBE_PX &&
     ePosition > IFX_TEXTURE_CUBE_NZ)
     iResult = IFX_E_INVALID_RANGE;

  //Translate the define ePosition into local array index
  uIndex = (U8)ePosition - IFX_TEXTURE_CUBE_PX;

  if(IFXSUCCESS(iResult))
  {
    iResult = m_pSceneGraph->GetPalette( IFXSceneGraph::TEXTURE, &pTexturePalette);
  }

  //Look up the texture palette to see we got the id for it yet.
  //We may have to create one if there not one yet.
  if(IFXSUCCESS(iResult))
  {
    if (NULL == m_pCubeMapTexture->pCubeMapTexName[uIndex])
      m_pCubeMapTexture->pCubeMapTexName[uIndex] = new IFXString;
    if (NULL == m_pCubeMapTexture->pCubeMapTexName[uIndex])
      iResult = IFX_E_OUT_OF_MEMORY;

    // Look for texture name in Texture Palette
    if (IFXSUCCESS(iResult))
      iResult = pTexturePalette->GetName(
									uTextureId, 
									m_pCubeMapTexture->pCubeMapTexName[uIndex]);

    if (IFXFAILURE(iResult))
      iResult = IFX_E_CANNOT_FIND;

    //Found it
    if(IFXSUCCESS(iResult))
    {
      m_pCubeMapTexture->puTextureId[uIndex] = uTextureId;
    }
    //Force the output blockqueue to refresh itself
    if(IFXSUCCESS(iResult))
      m_bOutputQueueDirty = TRUE;
  }

  IFXRELEASE(pTexturePalette);

  return iResult;
}

//-----------------------------------------------------------------------------
// CIFXTextureObject::GetImageCodec
//
//
//-----------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::GetImageCodec(IFXCID* pCodecCID)
{
  IFXRESULT iResult = IFX_OK;

  if (!m_bInitialized && pCodecCID)
    iResult = IFX_E_NOT_INITIALIZED;
  //We want set as the default;
  if (NULL == m_sImageInfo.m_pCodecCID)
  {
    memset((IFXCID*)pCodecCID,0,sizeof(IFXCID));
    return IFX_OK;
  }

  if(IFXSUCCESS(iResult))
  {
    *pCodecCID= *m_sImageInfo.m_pCodecCID;
  }
  return iResult;
}


//-----------------------------------------------------------------------------

// CIFXTextureObject::SetImageCodec
//
//
//-----------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::SetImageCodec(const IFXCID* pCodecCID)
{
  IFXRESULT iResult = IFX_OK;

  if (!m_bInitialized)
    iResult = IFX_E_NOT_INITIALIZED;

  if(IFXSUCCESS(iResult))
  {
    if (NULL == pCodecCID)
    {
      //This will set to default codec.
      if (m_sImageInfo.m_pCodecCID)
      {
        if (m_pDefaultCodec)
          *m_sImageInfo.m_pCodecCID = *m_pDefaultCodec;
        else
          memset((IFXCID*)m_sImageInfo.m_pCodecCID,0,sizeof(IFXCID));
      }
    }
    else
    {
      if (NULL == m_sImageInfo.m_pCodecCID)
        m_sImageInfo.m_pCodecCID = (IFXGUID*)IFXAllocate(sizeof(IFXCID));
      if (NULL == m_sImageInfo.m_pCodecCID)
        iResult = IFX_E_OUT_OF_MEMORY;

      if(IFXSUCCESS(iResult))
      {
        *m_sImageInfo.m_pCodecCID= *pCodecCID;
      }
      //Want to keep the original default setting.
      if (NULL == m_pDefaultCodec)
      {
        m_pDefaultCodec = (IFXCID*)IFXAllocate(sizeof(IFXCID));
        if (NULL == m_sImageInfo.m_pCodecCID)
          iResult = IFX_E_OUT_OF_MEMORY;
        if(IFXSUCCESS(iResult))
          *m_pDefaultCodec = *m_sImageInfo.m_pCodecCID;
      }
    }
  }
  return iResult;
}


//-----------------------------------------------------------------------------
// CIFXTextureObject::GetImageCodecQuality
//
//
//-----------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::GetImageCodecQuality(U32& uImageCodecQuality)
{
  IFXRESULT iResult = IFX_OK;

  if (!m_bInitialized)
    iResult = IFX_E_NOT_INITIALIZED;

  if(IFXSUCCESS(iResult))
    uImageCodecQuality = m_sImageInfo.m_compressionQuality;

  return iResult;
}


//-----------------------------------------------------------------------------
// CIFXTextureObject::SetImageCodecQuality
//
//
//-----------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::SetImageCodecQuality(const U32 uImageCodecQuality)
{
  IFXRESULT iResult = IFX_OK;

  if (!m_bInitialized)
    iResult = IFX_E_NOT_INITIALIZED;

  if(IFXSUCCESS(iResult))
    m_sImageInfo.m_compressionQuality = uImageCodecQuality;

  return iResult;
}


//-----------------------------------------------------------------------------
// CIFXTextureObject::ComputeBufferSize
//
// Helper for computing the image size of past in dimension and format.
//----------------------------------------------------------------------------
U32 CIFXTextureObject::ComputeBufferSize( U32 uWidth, U32 uHeight, U32 uFormat)
{
	///@todo: add processing ALPHA and LUMINANCE & ALPHA
	U32 uSize = 0;
	if (IFXTEXTUREMAP_FORMAT_LUMINANCE == uFormat)
	{
		m_uPixelSize = 1;
	}
	else if (IFXTEXTUREMAP_FORMAT_RGB24 == uFormat || IFXTEXTUREMAP_FORMAT_BGR24 == uFormat )
	{
		m_uPixelSize = 3;
	}
	else if (IFXTEXTUREMAP_FORMAT_RGBA32 == uFormat || IFXTEXTUREMAP_FORMAT_BGRA32 == uFormat)
	{
		m_uPixelSize = 4;
	}
	else if ( IFXTEXTUREMAP_FORMAT_ALPHA == uFormat )
	{
		m_uPixelSize = 1;
	}
	else if ( IFXTEXTUREMAP_FORMAT_LUMINANCE_ALPHA == uFormat ) 
	{
		m_uPixelSize = 2;
	}
	uSize = uWidth * uHeight * m_uPixelSize;

	return uSize;
}


//-----------------------------------------------------------------------------
// CIFXTextureObject::Reallocate
//
// Central place to expand or contract the size of the imagebuffer.
//-----------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::Reallocate( U32 uWidth, U32 uHeight, U32 uFormat)
{
  // check dimensions...
  if ((uWidth < 1) || (uHeight < 1))
  {
    return IFX_TEXTURE_MAP_INVALID_SIZE;
  }

  // check format and compute size...
  U32 uSize = ComputeBufferSize(uWidth, uHeight, uFormat);
  if (0 == uSize)
  {
    return IFX_TEXTURE_MAP_INVALID_FORMAT;
  }

  // optimization...
  // check for no change in memory requirements...
  if (m_uImageBufferSize == uSize)
  {
    return IFX_OK;
  }

  // use C-library reallocate for efficiency on reallocation...
  m_pImageBuffer = IFXReallocate( m_pImageBuffer, uSize);

  if (m_pImageBuffer)
  {
    // save size...
    m_uImageBufferSize = uSize;
    // information...
    IFXTRACE_CUSTOM(IFXRESULT_COMPONENT_TEXTURE_MANAGER, IFXDEBUG_MESSAGE,
            L"reallocated CIFXTextureMap(%p) to %u x %u (%u bpp)\n",
            this, uWidth, uHeight, uFormat*8);

    // return status...
    return IFX_OK;
  }
  else
  {
    // error, set size to zero...
    m_uImageBufferSize = 0;
    return IFX_E_OUT_OF_MEMORY;
  }
}


//---------------------------------------------------------------------------
//  CIFXTextureObject::DecompressImage
//
//  This method ships the current data block queue off to the IFXImageCodec
//  decompressor and fills the local imageBuffer.
//---------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::DecompressImage()
{
  IFXRESULT iResult=IFX_OK;
  if(m_pCoreServices==NULL || !m_bInitialized)
    iResult=IFX_E_NOT_INITIALIZED;

  if(IFXSUCCESS(iResult)) {
    // If local image data is dirty, build it from the DataBlockQueue
    if( TRUE == m_bImageDirty )
    {
      // We should use other way into indicate the BlockQueue is nit initialized.
      if( TRUE == m_bBlockQueueDirty)
      {
        // If both are dirty, then we have no source data yet.
        iResult = IFX_E_NOT_INITIALIZED;
      }
      else
      {
		if( NULL == m_pImageCodec )
		{
			iResult = IFXCreateComponent( CID_IFXImageCodec,
										IID_IFXImageCodec,
										(void**)&m_pImageCodec );

 			if( IFXSUCCESS(iResult) )
				iResult = m_pImageCodec->Initialize(NULL, m_pCoreServices);
		}

        if(IFXSUCCESS(iResult))
        {
          STextureSourceInfo sImageInfo;
          void* pImage=NULL;
          IFXCID* pCodecCID = NULL;
          U32 uTempQuality  = 0 ;

          sImageInfo.m_pCodecCID = NULL;
          sImageInfo.m_width  = 0;
          sImageInfo.m_height = 0;
          sImageInfo.m_imageType = TEXTURE_DEFAULT_FORMAT;

          //Check if it's any setting other than default.
          //We store the current setting and restore later.
          if (m_sImageInfo.m_pCodecCID)
          {
            pCodecCID = (IFXCID*)IFXAllocate(sizeof(IFXCID));
            if (pCodecCID)
            {
              GetImageCodec(pCodecCID);
            }
             else
               iResult = IFX_E_OUT_OF_MEMORY;
          }

          GetImageCodecQuality(uTempQuality);

          // This function is smart enough not to consume the datablock queue when
          // it traverses it.

		  if(IFXSUCCESS(iResult)) {
            iResult = m_pImageCodec->DecompressBlockQueueToImage(m_pDataBlockQueueX,
                                      &sImageInfo, &pImage);
		  }
			
          m_sImageInfo.m_name.Assign(&sImageInfo.m_name);

          // Save the original, unmodified for rendering dimensions.
          // Set the output image formatting information.
          if(IFXSUCCESS(iResult) && (iResult != IFX_W_READING_NOT_COMPLETE))
          {
            if (m_pImageBuffer)
              IFXDeallocate(m_pImageBuffer);
            m_pImageBuffer = pImage;
            m_sImageInfo = sImageInfo;
            m_uWidth  = sImageInfo.m_width;
            m_uHeight = sImageInfo.m_height;
            m_uFormat = sImageInfo.m_imageType;
            m_uImageBufferSize = ComputeBufferSize(m_uWidth,m_uHeight,m_uFormat);
            m_uPitch  = m_uWidth*m_uPixelSize;
            m_bImageDirty = FALSE;
            SetImageCodec(pCodecCID);
            SetImageCodecQuality(uTempQuality);
            m_bBlockQueueDirty = FALSE;  // We decompressed already.

            /*
            IFXTRACE_CUSTOM(IFXRESULT_COMPONENT_TEXTURE_MANAGER,
              IFXDEBUG_MESSAGE,
              L"Width: %6d\tName:%ls\n",m_uWidth,m_pTextureName->Raw());
              */

            if ( IFXTEXTUREMAP_FORMAT_RGBA32 == sImageInfo.m_imageType )
               m_bHasAlphaColor = IFX_TRUE;
            else
              m_bHasAlphaColor = IFX_FALSE;
            // If the user does not want to keep the compressed image
            // around.  We have to obey the user.
            if (!m_bKeepCompressed)
            {
              m_pDataBlockQueueX->ClearX();
              m_bBlockQueueDirty = TRUE;  // set it to no data in the queue;
            }
          }
          IFXDeallocate(pCodecCID);
        }
          //IFXRELEASE( pImageCodec );
      }
    }
  }
  return iResult;
}

// CIFXTextureObject private methods

//---------------------------------------------------------------------------
//  CIFXTextureObject::ConstructImage
//
// Decompress the data and delete the texture object from video memory.
//---------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::ConstructImage( IFXRenderContext* pRenderContext)
{
  IFXRESULT iResult = IFX_OK;

  // optimization:
  // if we're not dirty and the requested size and format
  // is the same as our current format, then m_pITextureMap
  // contains the proper decompressed image and we are done...
  if (m_bImageDirty == TRUE )
  {
    // 1. Decompress the image from datablock.
    iResult =  DecompressImage();

    // 2. Notify the reder that we have newer image.
    //    We delete the texture object in the video memory
    //    and will update with new data.

    //MakeDirty(pRenderLayer);

  }

  return iResult;
}

//---------------------------------------------------------------------------
//  CIFXTextureObject::ConstructImageFromBuffer
//
//  This method takes the buffer and image information, and correctly fills
//  the m_pITextureMap object.
//---------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::ConstructImageFromBuffer( 
									STextureSourceInfo* pImageInfo, 
									void* pBuffer )
{
	IFXRESULT iResult=IFX_OK;

	if( NULL == pImageInfo ) 
	{
		iResult = IFX_E_INVALID_POINTER;
	}else{
		if( !IsWholeImageFromExternalFile(pImageInfo) ) 
		{
			if( NULL == pBuffer ) 
			{
				iResult = IFX_E_INVALID_POINTER;	
			}
		}
	}

	if(IFXSUCCESS(iResult))
	{
		// Delete/resize any m_pITextureMap data
		iResult = Reallocate(
						pImageInfo->m_width, 
						pImageInfo->m_height, 
						pImageInfo->m_imageType);
		
		if (IFXSUCCESS(iResult))
		{
			//just to make sure that both of the Input buffer size
			// and the one we reallocated are correctly.
			if (pImageInfo->m_size == m_uImageBufferSize)
			{
				m_sImageInfo = *pImageInfo;
				m_uWidth  = pImageInfo->m_width;
				m_uHeight = pImageInfo->m_height;
				m_uFormat = pImageInfo->m_imageType;
				m_uPitch = m_uWidth * m_uPixelSize;
			}
			else
				iResult = IFX_TEXTURE_MAP_INVALID_SIZE;
		}

		if(IFXSUCCESS(iResult))
		{
			//We don't want copy the same image into itself.
			if (pBuffer != m_pImageBuffer)
			{
				if(pBuffer != NULL)
					memcpy(m_pImageBuffer, pBuffer, pImageInfo->m_size);
			}
			m_bImageDirty = FALSE;
		}
	}

	return iResult;
}


//---------------------------------------------------------------------------
//  CIFXTextureObject::ConstructQueueFromImage
//
//  Pass the image Buffer to ImageTool to pack up into datablock.
//---------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::ConstructQueueFromImage( 
									IFXDataBlockQueueX* pDataBlockQueueX )
{
	IFXRESULT iResult=IFX_OK;

	try {
		if(NULL == pDataBlockQueueX || NULL == m_pCoreServices)
			IFXCHECKX(IFX_E_INVALID_POINTER);

		if( NULL == m_pImageCodec )
		{
			iResult = IFXCreateComponent( 
							CID_IFXImageCodec,
							IID_IFXImageCodec,
							(void**)&m_pImageCodec );

 			if( IFXSUCCESS(iResult) )
				iResult = m_pImageCodec->Initialize(NULL, m_pCoreServices);
		}

		//We clear the block queue if this texture is imported from file.
		if(IFXTEXTURESOURCE_DECODED == m_eSource) 
		{
			pDataBlockQueueX->ClearX();
		}

		IFXCHECKX( m_pImageCodec->CompressImageToBlockQueue( 
										pDataBlockQueueX, &m_sImageInfo,
										(void*) m_pImageBuffer, m_uPriority) );

	}

	catch (IFXException e) 
	{
		iResult = e.GetIFXResult();
	}

	return iResult;
}


//---------------------------------------------------------------------------
//  CIFXTextureObject::GetRenderImage
//
//  This method allows you to get the texture image formated correctly and
//  copied into the memory location specified by rImageInfo.pData
//---------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::GetRenderImage(STextureOutputInfo& rImageInfo)
{
	IFXDECLARELOCAL(IFXModifierChain, pModChain);
	GetModifierChain(&pModChain);
	IFXDECLARELOCAL(IFXModifierDataPacket, pDataPacket);
	pModChain->GetDataPacket(pDataPacket);

	STextureOutputInfo* pImageInfo = NULL;
	IFXRESULT result = pDataPacket->GetDataElement( 
										m_uTextureDataElementIndex, 
										(void**)&pImageInfo);
	
	IFXDECLARELOCAL(IFXTextureImageTools, pTextureIT);
	IFXCreateComponent(
			CID_IFXTextureImageTools, 
			IID_IFXTextureImageTools,
			(void**)&pTextureIT);
	pTextureIT->SetTexels(
			pImageInfo->m_width, 
			pImageInfo->m_height, 
			m_uFormat, 
			pImageInfo->m_pData);
	pTextureIT->CopyRenderImage(&rImageInfo);
	
	return result;
}

//---------------------------------------------------------------------------
//  CIFXTextureObject::SetRawImage
//
//  This method allows you to set the raw image data.  It fills up the
//  internal data buffer w/ the specified info.
//---------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::SetRawImage(
									STextureSourceInfo* pImageInfo, 
									void* pImage)
{
	IFXRESULT iResult=IFX_OK;

	if( NULL == pImageInfo ) 
	{
		iResult = IFX_E_INVALID_POINTER;
	}
	else
	{
		if( !IsWholeImageFromExternalFile(pImageInfo) ) 
		{
			if( NULL == pImage ) 
			{
				iResult = IFX_E_INVALID_POINTER;	
			}
		}
	}

	if(IFXSUCCESS(iResult))
	{
		if ( IFXTEXTUREMAP_FORMAT_RGBA32 == pImageInfo->m_imageType )
			m_bHasAlphaColor = IFX_TRUE;
		else
			m_bHasAlphaColor = IFX_FALSE;

		//After calling the ContructImageFromBuffer,  all the
		//dimensions info should be filled.
		iResult=ConstructImageFromBuffer(pImageInfo, pImage);

		if(IFXSUCCESS(iResult))
		{
			// Set image as ok, and the output datablock queues as dirty
			m_bOutputQueueDirty  = TRUE;
			m_bBlockQueueDirty   = TRUE;

			//Let the RenderLayer know that this texture
			//has newer data, it needs to update a new copy.
			m_uVersion += 1;
		}
	}
	return iResult;
}

//---------------------------------------------------------------------------
//  CIFXTextureObject::ForceCompressionState
//
//  This method forces the texture source into a state
//---------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::ForceCompressionState( IFXenum eCompressionState )
{
  IFXRESULT iResult=IFX_OK;

  if (!m_bInitialized)
    iResult = IFX_E_NOT_INITIALIZED;

  if(IFXSUCCESS(iResult)) {
    switch(eCompressionState)
    {

    // this method forces the image to be decomressed and ready for display
    // much like a pre-load or pre-decompress
    case IFXTEXTURECOMPRESSIONSTATE_FORCEDECOMPRESS:
      if (IFXTEXTURESOURCE_DECODED != m_eSource)
      {
        m_bKeepCompressed = FALSE;
        m_bKeepDecompressed = TRUE;
      }
      else
        m_bKeepCompressed = TRUE;

      m_eCompressionState = IFXTEXTURECOMPRESSIONSTATE_FORCEDECOMPRESS;
      if( TRUE == m_bImageDirty )
      {

        // 1. decompress image if it hasn't been already
        iResult =  DecompressImage( );

      }

      break;

    // this method clears the current decompressed image buffer.  It forces
    // its compression into the output datablock queue if it hasn't already
    // been done
    case IFXTEXTURECOMPRESSIONSTATE_FORCECOMPRESSED:
      m_bKeepCompressed = TRUE;
      // build a proper input datablock queue (if needed)
      if( TRUE ==  m_bBlockQueueDirty)
      {
        iResult = ConstructQueueFromImage( m_pDataBlockQueueX );
        if (IFXSUCCESS(iResult))
        {
          m_bBlockQueueDirty = FALSE;

          //The user wants to compress the image to save some memory.
          //Therefore we compressed the image and del the raw image.
          if ( IFXTEXTURESOURCE_DECODED != m_eSource)
            m_bKeepDecompressed = FALSE;
        }
      }

      if (!m_bKeepDecompressed )
      {
        // resize the image data to 1x1 - deallocating its space
        // this is a cheezy way to save memory.
        // Well, from this point on, the user unable to rebuild
        // the BlockQueue again, 'cause we resize the image data here.
        if(IFXSUCCESS(iResult))
        {
          iResult = Reallocate( 
						1, 1, 
						IFXTextureImageTools::IFXTEXTUREMAP_FORMAT_LUMINANCE);

          m_bImageDirty = TRUE;
		  if (m_pModifierDataPacket)
			  m_pModifierDataPacket->InvalidateDataElement(
											m_uTextureDataElementIndex);
        }
      }
      if(IFXSUCCESS(iResult))
      {
        m_eCompressionState = IFXTEXTURECOMPRESSIONSTATE_FORCECOMPRESSED;
        m_bOutputQueueDirty = TRUE;
      }
      else
      {
        if (IFX_TEXTURE_COMPRESSION_UNSUPPORTED == iResult)
          iResult = IFX_OK;  //Not
      }

      break;

    default:
      iResult = IFX_E_UNSUPPORTED;
    }
  }
  return iResult;
}


//---------------------------------------------------------------------------
//  CIFXTextureObject::GetCompressedState
//
//  This method forces the texture source into a compressed state
//---------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::GetCompressedState( IFXenum* pCompressionState )
{
  IFXRESULT iResult = IFX_OK;
  if( NULL == pCompressionState )
    iResult = IFX_E_INVALID_POINTER;

  if(IFXSUCCESS(iResult)) {
    *pCompressionState = m_eCompressionState;
  }
  return iResult;
}


// IFXEncoderX
void CIFXTextureObject::SetObjectX( IFXUnknown& rObject )
{
  // Since the object that is to be encoded is this object, this method basically
  // performs a no-op.
  return;
}

void CIFXTextureObject::InitializeX( IFXCoreServices& rCoreServices )
{
	IFXLoadConfig lc( &rCoreServices, 0, m_bExternal, IFXSceneGraph::NUMBER_OF_PALETTES );
	InitializeX(lc);
}

void CIFXTextureObject::InitializeX( const IFXLoadConfig &pLC )
{
  // Since the purpose of this method is to get a core services object, and
  // since this core services object was already passed in elsewhere via another
  // interface, this method effectively performs a no-op.
	m_uLoadId = pLC.m_loadId;
	m_bExternal = pLC.m_external;
}

void CIFXTextureObject::EncodeX( 
							IFXString& rName, 
							IFXDataBlockQueueX& rDataBlockQueue, F64 units )
{
  IFXRESULT iResult = IFX_OK;

  if ( !m_bInitialized )
  {
    IFXCHECKX( IFX_E_NOT_INITIALIZED );
  }
  else
  {
    if ( IFX_TEXTURE_CUBE == m_eTextureMapType )
    {
      //prepare Cube Map texture.
      if ( TRUE == m_bOutputQueueDirty )
      {
        m_pOutputBlockQueueX->ClearX();
        MakeCubeMapDeclarationBlockX( *m_pDataBlockQueueX );
        m_pDataBlockQueueX->CopyX( m_pOutputBlockQueueX );
        m_bOutputQueueDirty = FALSE;
      }
    }
    else
    {
      //Prepare the 2d Texture DataBlocks
      if ( TRUE == m_bOutputQueueDirty )
      {
        // Clear the output queue; Done at Initialization already.
        m_pOutputBlockQueueX->ClearX();

        // Verify if the DataBlockQueue is required to do the update
        if ( TRUE == m_bBlockQueueDirty )
        {
		  //  Declaration block encoded in Image Tools ...

		  // Build a valid output datablock queue from the input queue or the
          //    raw image, whichever is newest.
          iResult = ConstructQueueFromImage( m_pDataBlockQueueX );
		  if (IFXSUCCESS(iResult))
            m_bBlockQueueDirty = FALSE;
        }

        if (IFXSUCCESS(iResult))
        {
          // We are about to make a new copy of the DataBlockQueue.
          // We delete old one if any.
          IFXRELEASE( m_pOutputBlockQueueX );

          //*****************************************************
          // We have to make another copy of the blockqueue in order
          // for the CloneModelFromCastMember to work correctly.
          // The Second copy of DataBlockQueue will be clear out
          // later after it has used.  Check in the GetNextBlock
          // function for more details.
          //*****************************************************
          m_pDataBlockQueueX->CopyX( m_pOutputBlockQueueX );
          m_bOutputQueueDirty = FALSE;
        }
      }

      // The user must have requested to keep the original image around.
      if ( !m_bKeepDecompressed && IFXSUCCESS(iResult))
      {
        // At this point, the data has been write to bitstream.
        // There is no point to keep all its image data around.
        // We just want to reduct it memory down to minimal,
        // Then cleanup the rest when this object goes away.
        IFXCHECKX( Reallocate( 1, 1, IFXTextureImageTools::IFXTEXTUREMAP_FORMAT_LUMINANCE ) );
        m_bImageDirty = TRUE;
		if (m_pModifierDataPacket)
			m_pModifierDataPacket->InvalidateDataElement(m_uTextureDataElementIndex);
      }
    }
  }

  // At this stage, there exists an internal output queue with all pertinent blocks ready to
  // be copied to an external priority queue.

  BOOL bDone = FALSE, bFirst = TRUE;
  while ( FALSE == bDone) 
  {
    IFXDECLARELOCAL(IFXDataBlockX,pDataBlockX);
    m_pOutputBlockQueueX->GetNextBlockX( pDataBlockX, bDone );
	if (bFirst)
	{
		// set metadata
		IFXDECLARELOCAL(IFXMetaDataX, pBlockMD);
		IFXDECLARELOCAL(IFXMetaDataX, pObjectMD);
		pDataBlockX->QueryInterface(IID_IFXMetaDataX, (void**)&pBlockMD);
		this->QueryInterface(IID_IFXMetaDataX, (void**)&pObjectMD);
		if( pBlockMD )
			pBlockMD->AppendX(pObjectMD);
		bFirst = FALSE;
	}
    if (pDataBlockX)
      rDataBlockQueue.AppendBlockX( *pDataBlockX );
  }

  m_bOutputQueueDirty = TRUE;
  m_bDone       = TRUE;
  m_pOutputBlockQueueX->ClearX();
}


// IFXDecoderX
void CIFXTextureObject::PutNextBlockX(IFXDataBlockX &rDataBlockX)
{
  //  1. validate all the parameters
  if ( !m_bInitialized )
    IFXCHECKX(IFX_E_NOT_INITIALIZED);

  // 1. Set image as dirty
  // 1a.Set output Queue to dirity
  m_bImageDirty = TRUE;
  if (m_pModifierDataPacket)
	  m_pModifierDataPacket->InvalidateDataElement(m_uTextureDataElementIndex);
  m_bOutputQueueDirty = TRUE;

  //  2. Determine the type of datablock - first or continuation
  U32 uBlockType=0;
  rDataBlockX.GetBlockTypeX( uBlockType );

  //  3. If it's a 'first' block, clean out any old datablocks in the queue
  if (BlockType_ResourceTextureU3D == uBlockType) 
  {
    m_pDataBlockQueueX->ClearX();
  }

  //  4.  save the passed in data block into the local datablock queue
  m_pDataBlockQueueX->AppendBlockX( rDataBlockX );

  // set metadata
  IFXDECLARELOCAL(IFXMetaDataX, pBlockMD);
  IFXDECLARELOCAL(IFXMetaDataX, pObjectMD);
  rDataBlockX.QueryInterface(IID_IFXMetaDataX, (void**)&pBlockMD);
  this->QueryInterface(IID_IFXMetaDataX, (void**)&pObjectMD);
  pObjectMD->AppendX(pBlockMD);

  //Update the BlockQueue flag to not dirty.
  m_bBlockQueueDirty = FALSE;

  //We only interesting in the first declaration block.
  if ( BlockType_ResourceTextureU3D == uBlockType ) 
  {
    CheckAndProcessCubeMapTextureX( *m_pDataBlockQueueX );
  }

  SetTextureSource( IFXTEXTURESOURCE_DECODED );
  //Let the RenderLayer know that this texture
  //has newer data, it needs to update a new copy.
  m_uVersion += 1;
  m_eCompressionState = IFXTEXTURECOMPRESSIONSTATE_FORCECOMPRESSED;
}

void CIFXTextureObject::TransferX(IFXRESULT &rWarningPartialTransfer)
{
	rWarningPartialTransfer = IFX_OK;
}

//---------------------------------------------------------------------------
//  CIFXTextureObject::CheckAndSetCubicMapTexture
//
//---------------------------------------------------------------------------
void CIFXTextureObject::CheckAndProcessCubeMapTextureX(
									IFXDataBlockQueueX &rSourceQueueX)
{
  IFXDECLARELOCAL(IFXDataBlockX,pDataBlockX);
  rSourceQueueX.PeekNextBlockX(pDataBlockX);

  if(pDataBlockX) 
  {

    IFXDECLARELOCAL(IFXBitStreamX,pBitStreamX);
    IFXCHECKX(IFXCreateComponent(
					CID_IFXBitStreamX,
					IID_IFXBitStreamX,
					(void**)&pBitStreamX));

	pBitStreamX->SetDataBlockX(*pDataBlockX);

    if (m_pTextureName)
      delete m_pTextureName;

    m_pTextureName = new IFXString;
    if(NULL == m_pTextureName) {
      IFXCHECKX(IFX_E_OUT_OF_MEMORY);
    }

    pBitStreamX->ReadIFXStringX(*m_pTextureName);

    pBitStreamX->ReadU32X( m_uHeight);       
	pBitStreamX->ReadU32X( m_uWidth);
    pBitStreamX->ReadU8X( m_uFormat);
  }
}

//---------------------------------------------------------------------------
//  CIFXTextureObject::PurgeRenderImage
//
//---------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::PurgeRenderImage()
{
  IFXRESULT iResult = IFX_OK;

  IFXDELETE_ARRAY(m_ImageInfo.m_pData);

  return iResult;
}

void CIFXTextureObject::ProcessCubeMapDeclarationBlockX(IFXBitStreamX& rBitStreamX)
{
  IFXString sName;

  if (!m_bInitialized)
    IFXCHECKX(IFX_E_NOT_INITIALIZED);

  IFXDECLARELOCAL(IFXPalette,pTexturePalette);
  IFXCHECKX(m_pSceneGraph->GetPalette( IFXSceneGraph::TEXTURE, &pTexturePalette));

  U32 i = 0;
  for (i = 0; i< MAX_CUBIC_MAP_SIZE ; i++) {

    U8 u8Index = 0;
    rBitStreamX.ReadU8X(u8Index);
    rBitStreamX.ReadIFXStringX( sName );
    if(!sName.IsEmpty() ) {
      if (NULL == m_pCubeMapTexture->pCubeMapTexName[u8Index])
        m_pCubeMapTexture->pCubeMapTexName[u8Index] = new IFXString;

      if (m_pCubeMapTexture->pCubeMapTexName[u8Index])
        IFXCHECKX(m_pCubeMapTexture->pCubeMapTexName[u8Index]->Assign( &sName ));
      else
        IFXCHECKX(IFX_E_OUT_OF_MEMORY);

      //Look up the texture palette to see we got the id for it yet.
      //We may have to create one if there not one yet.
      // Look for texture name in Texture Palette
      U32 uTextureID = 0;
      //IFXUnknown* punk      = NULL;
      IFXRESULT iFindResult = pTexturePalette->Find(
									m_pCubeMapTexture->pCubeMapTexName[u8Index],
									&uTextureID);
      //Found it
      if (IFXSUCCESS(iFindResult)) {
        m_pCubeMapTexture->puTextureId[u8Index] = uTextureID;
      }
    }
  }
}

void CIFXTextureObject::MakeCubeMapDeclarationBlockX(
								IFXDataBlockQueueX &rDataBlockQueueX)
{
}

// IFXModifier

const IFXGUID* CIFXTextureObject::m_scpOutputDIDs[] =
{
	&DID_IFXTexture
};

IFXRESULT CIFXTextureObject::GetOutputs( 
									IFXGUID**& rpOutOutputs,
									U32&       rOutNumberOfOutputs,
									U32*&      rpOutOutputDepAttrs )
{
	rOutNumberOfOutputs = sizeof(m_scpOutputDIDs) / sizeof(IFXGUID*);
	rpOutOutputs = (IFXGUID**)&m_scpOutputDIDs;
	rpOutOutputDepAttrs = NULL;
	return IFX_OK;
}

IFXRESULT CIFXTextureObject::GetDependencies( 
									IFXGUID*   pInOutputDID,
									IFXGUID**& rppOutInputDependencies,
									U32&       rOutNumberInputDependencies,
									IFXGUID**& rppOutOutputDependencies,
									U32&       rOutNumberOfOutputDependencies,
									U32*&      rpOutOutputDepAttrs )
{
	IFXRESULT result = IFX_OK;

	if (pInOutputDID == &DID_IFXTexture)
	{
		rppOutInputDependencies = NULL;
		rOutNumberInputDependencies = 0;
		rppOutOutputDependencies = NULL;
		rOutNumberOfOutputDependencies = 0;
		rpOutOutputDepAttrs = NULL;
	}
	else
	{
		IFXASSERTBOX( 1, 
			"CIFXTextureObject::GetDependencies() called with unknown output!" );
		result = IFX_E_UNDEFINED;
	}

	return result;
}

IFXRESULT CIFXTextureObject::GenerateOutput( 
										U32    inOutputDataElementIndex,
										void*& rpOutData, BOOL& rNeedRelease )
{
	IFXRESULT result = IFX_OK;
	if (inOutputDataElementIndex == m_uTextureDataElementIndex)
	{
		if (m_bImageDirty)
			result = ConstructImage(NULL);

        if (IFXSUCCESS(result) && (result != IFX_W_READING_NOT_COMPLETE)) 
		{     
			m_ImageInfo.eChannelOrder = m_eChannelOrder;
			m_ImageInfo.eRenderFormat = m_eRenderFormat;
			IFXDELETE_ARRAY(m_ImageInfo.m_pData);
			m_ImageInfo.m_pData = new U8[m_uImageBufferSize];

			if( NULL != m_ImageInfo.m_pData )
			{
				memcpy(m_ImageInfo.m_pData, m_pImageBuffer, m_uImageBufferSize);
				m_ImageInfo.m_height = m_uHeight;
				m_ImageInfo.m_width = m_uWidth;
				m_ImageInfo.m_pitch  = m_uPitch;
				if (!m_bKeepDecompressed)
				{
					Reallocate(1, 1, m_uFormat);
					m_bImageDirty = TRUE;
					if (m_pModifierDataPacket)
						m_pModifierDataPacket->InvalidateDataElement(
													m_uTextureDataElementIndex);
				}
				rpOutData = &m_ImageInfo;
			}
			else
				result = IFX_E_OUT_OF_MEMORY;
	    } 
		else
			rpOutData = (void*)-1;
	}

	return result;
}

IFXRESULT CIFXTextureObject::SetDataPacket( 
									IFXModifierDataPacket* pInInputDataPacket,
									IFXModifierDataPacket* pInOutputDataPacket )
{
	IFXRESULT result = IFX_OK;

	IFXRELEASE(m_pModifierDataPacket);
	IFXRELEASE(m_pInputDataPacket);

	if (pInOutputDataPacket && pInInputDataPacket)
	{
		pInOutputDataPacket->AddRef();
		m_pModifierDataPacket = pInOutputDataPacket;
		pInInputDataPacket->AddRef();
		m_pInputDataPacket = pInInputDataPacket;
	} 
	else
		result = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(result))
		result = pInOutputDataPacket->GetDataElementIndex( 
											DID_IFXTexture, 
											m_uTextureDataElementIndex );

	return result;
}

IFXRESULT CIFXTextureObject::Notify( 
								IFXModifierMessage eInMessage, 
								void* pMessageContext )
{
	return IFX_OK;
}


//-----------------------------------------------------------------------------
//
// CIFXTextureObject::CheckCompressionProperties
//
//-----------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::CheckCompressionProperties(
								const U32 uContImageCount, 
 								const U8* pBlockCompressionType, 
								IFXTextureObject::ChannelType* pImageChannelFlagsList)
{
	IFXRESULT rc = IFX_CANCEL;

	const U8* pB = pBlockCompressionType;
	const IFXTextureObject::ChannelType* pCh = pImageChannelFlagsList;
	if( !pB || !pCh )
		rc = IFX_E_INVALID_POINTER;
	else
	{
		U32 j8  = TextureType_Jpeg8;
		U32 j24 = TextureType_Jpeg24;
		U32 png = TextureType_Png;

		IFXTextureObject::ChannelType ch_a = IFXTextureObject::IFXIMAGECHANNEL_ALPHA;
		IFXTextureObject::ChannelType ch_r = IFXTextureObject::IFXIMAGECHANNEL_RED;
		IFXTextureObject::ChannelType ch_b = IFXTextureObject::IFXIMAGECHANNEL_BLUE;
		IFXTextureObject::ChannelType ch_g = IFXTextureObject::IFXIMAGECHANNEL_GREEN;
		IFXTextureObject::ChannelType ch_l = IFXTextureObject::IFXIMAGECHANNEL_LUMINANCE; 

		switch(m_sImageInfo.m_imageType) 
		{
		case IFXTEXTUREMAP_FORMAT_ALPHA: 
			// 1 bpp: 1 continuation channel: PNG or JPEG8
			if(uContImageCount==1)
			{
				if( (pB[0]==png || pB[0]==j8) && 
					(pImageChannelFlagsList[0] == IFXTextureObject::IFXIMAGECHANNEL_ALPHA) )
				{
					rc = IFX_OK;
				}
				else
				{
					rc = IFX_E_INVALID_COMPRESSION_TYPE;
				}		
			}
			else
			{
				rc = IFX_E_INVALID_CONT_IMAGE_COUNT;
			}		
			break;

		case IFXTEXTUREMAP_FORMAT_RGB24: 
		case IFXTEXTUREMAP_FORMAT_BGR24: 
			// 3 bpp: 1 cont image block : JPEG24 or PNG
			//	: 3 continuation channels : 1+1+1 - JPEG8,JPEG8,JPEG8 or all PNG
			//										but can not be JPEG24
			if(uContImageCount==1)
			{

				if( (pB[0]==j24 || pB[0]==png) &&
					(pCh[0])== (ch_r | ch_b | ch_g) ) 
				{
					rc = IFX_OK;
				} 
				else 
				{
					rc = IFX_E_INVALID_COMPRESSION_TYPE;
				}		
			} 
			else if (uContImageCount==3 || uContImageCount==2)
			{

				if (pB[0]!=j24 && pB[1]!=j24 && pB[2]!=j24 ) 
				{
					rc = IFX_OK;
				} 
				else 
				{
					rc = IFX_E_INVALID_COMPRESSION_TYPE;
				}
			}
			else
			{
				rc = IFX_E_INVALID_CONT_IMAGE_COUNT;
			}

			break;

		case IFXTEXTUREMAP_FORMAT_RGBA32: 
		case IFXTEXTUREMAP_FORMAT_BGRA32: 
			// 4 bpp : 1 continuation channel : PNG 
			//		  : 2 continuation channels: 
			//			3+1 - PNG+JPEG8; JPEG24 + JPEG8; JPREG24 + PNG; PNG + PNG
			//		  : 4 continuation channels: 1+1+1+1 PNG,PNG,PNG,PNG   
			//						  :	1+1+1+1 JPEG8,JPEG8,JPEG8,JPEG8
			if(uContImageCount == 1)
			{
				if( (pB[0]==png) &&
					(pImageChannelFlagsList[0] == 
					(	IFXTextureObject::IFXIMAGECHANNEL_RED | 
					IFXTextureObject::IFXIMAGECHANNEL_BLUE | 
					IFXTextureObject::IFXIMAGECHANNEL_GREEN | 
					IFXTextureObject::IFXIMAGECHANNEL_ALPHA) ) ) 
				{
					rc = IFX_OK;
				}
				else
				{
					rc = IFX_E_INVALID_COMPRESSION_TYPE;
				}
			} 
			else if(uContImageCount == 2)
			{
				if( ( pB[0]==png && pB[1]==j8) || ( pB[0]==j8  && pB[1]==png)  || 
					( pB[0]==j24 && pB[1]==j8) || ( pB[0]==j8  && pB[1]==j24)  || 
					( pB[0]==j24 && pB[1]==png) || ( pB[0]==png  && pB[1]==j24)  || 
					( pB[0]==png && pB[1]==png) ) 
				{
					rc = IFX_OK;
				}
				else
				{
					rc = IFX_E_INVALID_COMPRESSION_TYPE;
				}
			} 
			else if(uContImageCount == 4)
			{
				if ( pB[0]!=j24 && pB[1]!=j24 && pB[2]!=j24 && pB[3]!=j24 ) 
				{
					rc = IFX_OK;
				} 
				else 
				{
					rc = IFX_E_INVALID_COMPRESSION_TYPE;				
				}	
			} 
			else
			{
				rc = IFX_E_INVALID_CONT_IMAGE_COUNT;
			}
			break;
		case IFXTEXTUREMAP_FORMAT_LUMINANCE: 
			if(uContImageCount == 1)
			{
				if( (pB[0]==png || pB[0]==j8) && 
					(pImageChannelFlagsList[0] == 
					IFXTextureObject::IFXIMAGECHANNEL_LUMINANCE)  )
				{
					rc = IFX_OK;
				}
				else
				{
					rc = IFX_E_INVALID_COMPRESSION_TYPE;
				}		
			} 
			else 
			{
				rc = IFX_E_INVALID_CONT_IMAGE_COUNT;
			}
			break;

		case IFXTEXTUREMAP_FORMAT_LUMINANCE_ALPHA: 	
			if(uContImageCount == 1)
			{
				if( (pB[0]==png || pB[0]==j8) && (pCh[0])== (ch_l | ch_a)  )
				{
					rc = IFX_OK;
				}
				else
				{
					rc = IFX_E_INVALID_COMPRESSION_TYPE;
				}		
			} 
			else if(uContImageCount == 2) 
			{
				if ( (pB[0]!=j24 && pB[1]!=j24) &&
					( !(pCh[0] | ch_r) && !(pCh[0] | ch_b) && !(pCh[0] | ch_g) && 
					!(pCh[1] | ch_r) && !(pCh[1] | ch_b) && !(pCh[1] | ch_g) ) ) 
				{
					rc = IFX_OK;
				} 
				else 
				{
					rc = IFX_E_INVALID_COMPRESSION_TYPE;
				}
			} 
			else 
			{
				rc = IFX_E_INVALID_CONT_IMAGE_COUNT;
			}
			break;
		default:
			IFXASSERT(0);
			rc = IFX_E_UNSUPPORTED;
			break;
		}
	}
	return rc;
}

//-----------------------------------------------------------------------------
//  CIFXTextureObject::CheckCompressionType
//  return TRUE - if all Compression types are expected values,
//  return FALSE - otherwise 
//-----------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::CheckCompressionType(
									const U32 uContImageCount,
									const U8* pBlockCompressionType)
{
	IFXRESULT rc = IFX_OK;
	if( !pBlockCompressionType )
		rc = IFX_E_INVALID_POINTER;
	U32 i;
	for( i=0; IFXSUCCESS( rc ) && i < uContImageCount; i++ )
	{		
		if( pBlockCompressionType[i]==TextureType_Jpeg24 ||
			pBlockCompressionType[i]==TextureType_Jpeg8 ||
			pBlockCompressionType[i]==TextureType_Png ){
		}
		else
		{
			rc = IFX_FALSE;
		}
	}
	return rc;
}

IFXRESULT CIFXTextureObject::GetImageType(IFXTextureObject::Format& rFormat) const
{
	IFXRESULT rc = IFX_OK;
    if (!m_bInitialized)
		rc = IFX_E_NOT_INITIALIZED;
	else 
		rFormat = (IFXTextureObject::Format)m_sImageInfo.m_imageType;
	return rc;
}


IFXRESULT CIFXTextureObject::GetContinuationImageCount( U32& rCount) const
{
	IFXRESULT rc = IFX_OK;
    if (!m_bInitialized)
		rc = IFX_E_NOT_INITIALIZED;
	else 
		rCount = m_sImageInfo.m_imageLoadCount;
	return rc;
}

//-----------------------------------------------------------------------------
//
// CIFXTextureObject::GetImageCompressionProperties
//
//-----------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::GetImageCompressionProperties(
								U8 *pBlockCompressionTypeList, 
								IFXTextureObject::ChannelType* pImageChannelFlagsList,
								BOOL *pExternalFileReferenceList,
								IFXArray<IFXString*>** ppExternalFileReferenceArrayList)
{
	IFXRESULT rc = IFX_OK;
    if (!m_bInitialized)
		rc = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(rc)) 
	{
		if( pBlockCompressionTypeList == NULL || pImageChannelFlagsList == NULL )
		{
			rc = IFX_E_INVALID_POINTER;
		}
	}

	if (IFXSUCCESS(rc)) 
	{
		U32 rContImageCount = m_sImageInfo.m_imageLoadCount;
		U32 i;
		for (i=0; i<rContImageCount; i++) 
		{
			pBlockCompressionTypeList[i] = m_sImageInfo.m_blockCompressionType[i];
			pImageChannelFlagsList[i] = 
				(IFXTextureObject::ChannelType)m_sImageInfo.m_blockChannels[i];
			
			if(pExternalFileReferenceList != NULL )
			{
				pExternalFileReferenceList[i] = m_sImageInfo.m_URLFileRef[i];
			}
				
 			if( ppExternalFileReferenceArrayList != NULL && 
				pExternalFileReferenceList != NULL )
			{
				if (pExternalFileReferenceList[i]) 
				{
					U32 uFileRefCount = 
						m_sImageInfo.m_pURLFileNameList[i].GetNumberElements();
					IFXArray<IFXString*>* pPtrStringArray = 
						ppExternalFileReferenceArrayList[i];

					pPtrStringArray->ResizeToExactly(uFileRefCount);
					U32 indFile;
					for (indFile = 0; indFile < uFileRefCount; indFile++ ) 
					{
						IFXString*& pStr = 
							m_sImageInfo.m_pURLFileNameList[i].GetElement(indFile);
						(*pPtrStringArray)[indFile] = new IFXString(pStr);
					}
				}
			}
		}
	}
	return rc;
}

//-----------------------------------------------------------------------------
//
// CIFXTextureObject::SetImageCompressionProperties
//
//-----------------------------------------------------------------------------
IFXRESULT CIFXTextureObject::SetImageCompressionProperties(
					U32 continuationImageCount, U8 *pBlockCompressionTypeList, 
					IFXTextureObject::ChannelType* pImageChannelFlagsList,
					BOOL* pExternalFileReferenceList,
					IFXArray<IFXString*>** ppExternalFileReferenceArrayList )
{
	IFXRESULT rc = IFX_OK;

    if (!m_bInitialized)
		rc = IFX_E_NOT_INITIALIZED;
	if( pBlockCompressionTypeList == NULL || pImageChannelFlagsList == NULL )
	{
		rc = IFX_E_INVALID_POINTER;
	}

	if (IFXSUCCESS(rc)) 
	{
		if( continuationImageCount > IFX_MAX_CONTINUATIONIMAGE_COUNT ) 
		{
			rc = IFX_E_INVALID_RANGE;
		}	
		if( IFXSUCCESS(rc) ) 
		{
			if( pBlockCompressionTypeList == NULL || 
				pImageChannelFlagsList == NULL ) 
			{
				rc = IFX_E_INVALID_POINTER;
			}
		}
    }

	if(IFXSUCCESS( rc)) 
	{
		rc = CheckCompressionProperties(
						continuationImageCount, 
						pBlockCompressionTypeList, 
						pImageChannelFlagsList);		
	} 
	else 
	{
		IFXTRACE_GENERIC(L"[TextureObject] Wrong compression type or image "
						L"channel at SetImageCompressionProperties\n");
		rc = IFX_E_UNSUPPORTED;
	}

	if(IFXSUCCESS( rc)) 
	{
		m_sImageInfo.m_imageLoadCount = continuationImageCount;

		U32 count;
		for(count = 0; count < continuationImageCount; count++ ) 
		{
			m_sImageInfo.m_blockCompressionType[count] = 
				pBlockCompressionTypeList[count];
			m_sImageInfo.m_blockChannels[count] = pImageChannelFlagsList[count];

			if( pExternalFileReferenceList != NULL && 
				ppExternalFileReferenceArrayList != NULL )
			{
				m_sImageInfo.m_URLFileRef[count] = pExternalFileReferenceList[count];
				if (pExternalFileReferenceList[count]) 
				{
					U32 uFileRefCount = 
						ppExternalFileReferenceArrayList[count]->GetNumberElements();
					
					CleanFileRefList(count); 
					
					m_sImageInfo.m_pURLFileNameList[count].ResizeToExactly(uFileRefCount);
					U32 indFile;
					for ( indFile = 0; indFile < uFileRefCount; indFile++ ) 
					{
						IFXString*& pStr = 
							ppExternalFileReferenceArrayList[count]->GetElement(indFile);
						(m_sImageInfo.m_pURLFileNameList[count])[indFile] = new IFXString(pStr);
					}
				} 
				else 
				{
					CleanFileRefList(count); // no external file ref
				}

			}
		}
		m_bBlockQueueDirty = TRUE;
	} 

	if (IFXSUCCESS(rc)) {}

	return rc;
}

void CIFXTextureObject::CleanFileRefList(U32 contImage) 
{
	U32 fileInd;
	for( fileInd=0; 
		 fileInd < m_sImageInfo.m_pURLFileNameList[contImage].GetNumberElements(); 
		 fileInd++ ) 
	{
		IFXString* ptr = (m_sImageInfo.m_pURLFileNameList[contImage])[fileInd];
		if (ptr) 
		{
			delete ptr;
		}
	} 
	m_sImageInfo.m_pURLFileNameList[contImage].Clear();
}


/**
	return TRUE  if all images are external files
*/
BOOL CIFXTextureObject::IsWholeImageFromExternalFile(
							const STextureSourceInfo* pImageInfo) 
{
	BOOL res = FALSE; 
	if( NULL == pImageInfo)
		return res;	
	
	if (pImageInfo->m_imageURLCount != 0) 
	{
		res = TRUE;
		U32 i;
		for (i=0; i<pImageInfo->m_imageURLCount;i++) 
		{
			if ( !(pImageInfo->m_URLFileRef[i]) ) 
			{
				res = FALSE;
			}
		}
	} 
	return res;
}
