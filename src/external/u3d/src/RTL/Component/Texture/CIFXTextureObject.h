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

/**
  @file CIFXTextureObject.h
*/

#ifndef __CIFXTEXTURE_OBJECT_H__
#define __CIFXTEXTURE_OBJECT_H__


#include "IFXTextureObject.h"
#include "CIFXTextureImageTools.h"
#include "IFXTextureErrors.h"
#include "IFXBitStreamX.h"
#include "IFXDataBlockQueueX.h"
#include "IFXIDManager.h"
#include "IFXEncoderX.h"
#include "IFXDecoderX.h"
#include "IFXAutoRelease.h"
#include "CIFXModifier.h"

class IFXImageCodec;

#define MAX_CUBIC_MAP_SIZE  6

class CIFXTextureObject : private CIFXModifier,
				  virtual public  IFXEncoderX,
				  virtual public  IFXDecoderX,
                  virtual public  IFXTextureObject
{
            CIFXTextureObject();
  virtual  ~CIFXTextureObject();
  friend
  IFXRESULT IFXAPI_CALLTYPE CIFXTextureObject_Factory( IFXREFIID interfaceId,
                                       void**    ppInterface );

public:
  // IFXUnknown
  virtual U32 IFXAPI  AddRef(void);
  virtual U32 IFXAPI  Release(void);
  virtual IFXRESULT IFXAPI  QueryInterface( IFXREFIID riid, void **ppv);

  // IFXMarker
  void IFXAPI   SetPriority( U32 uInPriority, BOOL bRecursive, BOOL bPromotionOnly );

  // IFXMarkerX interface...
  void IFXAPI  GetEncoderX (IFXEncoderX*& rpEncoderX);

  // IFXModifier
  IFXRESULT IFXAPI  GetOutputs ( IFXGUID**& rpOutOutputs,
    U32&     rOutNumberOfOutputs,
    U32*&    rpOutOutputDepAttrs );

  IFXRESULT IFXAPI  GetDependencies ( IFXGUID* pInOutputDID,
    IFXGUID**& rppOutInputDependencies,
    U32&       rOutNumberInputDependencies,
    IFXGUID**& rppOutOutputDependencies,
    U32&       rOutNumberOfOutputDependencies,
    U32*&      rpOutOutputDepAttrs );

  IFXRESULT IFXAPI  GenerateOutput (
          U32 inOutputDataElementIndex,
          void*& rpOutData,
          BOOL& rNeedRelease );

  IFXRESULT IFXAPI  SetDataPacket (
          IFXModifierDataPacket* pInInputDataPacket,
          IFXModifierDataPacket* pInDataPacket );

  IFXRESULT IFXAPI  Notify (
          IFXModifierMessage eInMessage,
          void* pMessageContext );

  // IFXTextureObject methods...
  virtual IFXRESULT IFXAPI  GetRenderImage(STextureOutputInfo& rImageInfo);
  virtual IFXRESULT IFXAPI  MakeDirty(IFXRenderContext* pRenderContext);

  //Exporter interface.
  virtual IFXRESULT IFXAPI  SetRawImage(STextureSourceInfo* pImageInfo, void* pImage);
  virtual IFXRESULT IFXAPI  ForceCompressionState( IFXenum eCompressionState );
  virtual IFXRESULT IFXAPI  GetCompressedState( IFXenum* eCompressionState );
  virtual IFXRESULT IFXAPI  SetDefaultTexture();

  // IFXEncoderX interface:
  virtual void IFXAPI  EncodeX(IFXString &rName, IFXDataBlockQueueX &rDataBlockQueue, F64 units = 1.0f);
  virtual void IFXAPI  InitializeX(IFXCoreServices &rCoreServices);
  virtual void IFXAPI  SetObjectX(IFXUnknown &rObject) ;        // Also in IFXEncoderX

  // IFXDecoderX
  virtual void IFXAPI  InitializeX( const IFXLoadConfig &pLC );
  virtual void IFXAPI  PutNextBlockX(IFXDataBlockX &rDataBlockX) ;
  virtual void IFXAPI  TransferX(IFXRESULT &rWarningPartialTransfer) ;

  // This object's Attributes methods...
  virtual IFXRESULT IFXAPI  GetWidth(  U32 *puValue);
  virtual IFXRESULT IFXAPI  SetWidth(  U32  uValue);
  virtual IFXRESULT IFXAPI  GetHeight( U32 *puValue);
  virtual IFXRESULT IFXAPI  SetHeight( U32  uValue);
  virtual IFXRESULT IFXAPI  GetFormat( U32 *puValue);
  virtual IFXRESULT IFXAPI  SetFormat( U32  uValue);
  virtual IFXRESULT IFXAPI  GetAnimated( BOOL *pbValue);
  virtual IFXRESULT IFXAPI  SetAnimated( BOOL  bValue);
  virtual IFXRESULT IFXAPI  GetRenderFormat(IFXenum& eFormat);
  virtual IFXRESULT IFXAPI  SetRenderFormat(IFXenum eFormat);
  virtual IFXRESULT IFXAPI  GetKeepDecompressed( BOOL *pbValue);
  virtual IFXRESULT IFXAPI  SetKeepDecompressed( BOOL  bValue);
  virtual IFXRESULT IFXAPI  GetKeepCompressed( BOOL *pbValue);
  virtual IFXRESULT IFXAPI  SetKeepCompressed( BOOL  bValue);

  virtual IFXRESULT IFXAPI  GetMagFilterMode( IFXenum *peFilterMode);
  virtual IFXRESULT IFXAPI  SetMagFilterMode( IFXenum  eFilterMode);

  virtual IFXRESULT IFXAPI  GetMinFilterMode( IFXenum *peFilterMode);
  virtual IFXRESULT IFXAPI  SetMinFilterMode( IFXenum  eFilterMode);

  virtual IFXRESULT IFXAPI  GetMipMode( IFXenum *peMipMode);
  virtual IFXRESULT IFXAPI  SetMipMode( IFXenum eMipMode);

  virtual IFXRESULT IFXAPI  GetTextureSource( IFXenum *peTextureSource);
  virtual IFXRESULT IFXAPI  SetTextureSource( IFXenum eTextureSource);

  virtual IFXRESULT IFXAPI  GetTextureType( IFXenum *puMapType);
  virtual IFXRESULT IFXAPI  SetTextureType( IFXenum  uMapType);

  virtual IFXRESULT IFXAPI  GetCubeMapTexture(
            IFXenum ePosition, IFXTextureObject **ppTexture);
  virtual IFXRESULT IFXAPI  SetCubeMapTexture( U32 uTextureId, IFXenum ePosition);

  virtual BOOL    IFXAPI  IsInitialize() {return m_bInitialized;}
  virtual IFXRESULT IFXAPI  GetRawImageBufferSize( U32 *puValue);

  virtual IFXRESULT IFXAPI  GetImageCodec(IFXCID* pCodecCID);
  virtual IFXRESULT IFXAPI  SetImageCodec(const IFXCID* pCodecCID);
  virtual IFXRESULT IFXAPI  GetImageCodecQuality(U32& uImageCodecQuality);
  virtual IFXRESULT IFXAPI  SetImageCodecQuality(const U32 uImageCodecQuality);

  virtual U32     IFXAPI  GetId() { return m_uId; }
  virtual U32     IFXAPI  GetVersion() { return m_uVersion; }

  virtual IFXRESULT IFXAPI  PurgeRenderImage();

  virtual BOOL  IFXAPI  HasAlphaColor() {return m_bHasAlphaColor;}

  IFXRESULT IFXAPI   InitializeObject();

  /**
   *  This will force a release of the object ID manager (even though some objects
   *  may still be allocated).  In debug mode, this will dump a list of objects
   *  that are still allocated to the debug console.
   */
  static IFXRESULT IFXAPI Shutdown()
  {
    if(ms_spIDManager.IsValid())
    {
      IFXTRACE_GENERIC(L"\nError: IFXTextureObject Instances Still Allocated!\n");
      ms_spIDManager->OutputAllocatedIds();
      ms_spIDManager = 0;
    }

    return IFX_OK;
  }

  // Client control function for retrieving / changing compression properities for
  // texture image encoding

  virtual IFXRESULT IFXAPI  GetContinuationImageCount( U32& rCount) const;

  virtual IFXRESULT IFXAPI  GetImageType(IFXTextureObject::Format& rFormat)const;

  virtual IFXRESULT IFXAPI  GetImageCompressionProperties(
              U8 *ppBlockCompressionTypeList,
              IFXTextureObject::ChannelType* ppImageChannelFlagsList,
              BOOL *ppExternalFileReferenceList,
              IFXArray<IFXString*>** ppExternalFileReferenceArrayList);

  virtual IFXRESULT IFXAPI  SetImageCompressionProperties(
              U32 continuationImageCount,
              U8 *pBlockCompressionTypeList,
              IFXTextureObject::ChannelType* pImageChannelFlagsList,
              BOOL* pExternalFileReferenceList,
              IFXArray<IFXString*>** pExternalFileReferenceArrayList );

private:
  // IFXUnknown attributes...
  U32 m_uRefCount;
  U32 m_uLoadId;
  BOOL m_bExternal;

  // IFXModifier
  static const IFXDID* m_scpOutputDIDs[];
  U32 m_uTextureDataElementIndex;
  STextureOutputInfo m_ImageInfo;

  // private data for IFXDecoder and IFXEncoder
  IFXCoreServices* m_pCoreServices;

  BOOL m_bInitialized;

  // attribute variables...
  IFXenum m_eTextureMapType;
  U32  m_uWidth;
  U32  m_uHeight;
  U8   m_uFormat;
  U32  m_uPriority;
  BOOL m_bHasAlphaColor;

  void *m_pImageBuffer;
  U32  m_uImageBufferSize;
  U8   m_uPixelSize;
  BOOL m_bImageDirty;
  BOOL m_bAnimated;
  IFXCID *m_pDefaultCodec;
  STextureSourceInfo m_sImageInfo;


  // mip mapping...
  IFXenum m_eMagMode;
  IFXenum m_eMinMode;
  IFXenum m_eMipMode;

  U8  m_uCompressionQuality;
  U8  m_uBlockCompressionType;

  U32 m_uPitch;
  IFXenum m_eRenderFormat;
  IFXenum m_eChannelOrder;

  // IFXDataBlockQueue
  BOOL m_bBlockQueueDirty;
  BOOL m_bOutputQueueDirty;
  IFXDataBlockQueueX* m_pDataBlockQueueX; // holds the compressed output data block queue
  IFXDataBlockQueueX* m_pOutputBlockQueueX;
  IFXImageCodec* m_pImageCodec;
  BOOL m_bDone;

  BOOL m_bKeepCompressed;
  BOOL m_bKeepDecompressed;
  IFXenum m_eCompressionState;

  U32 m_uOriginalWidth;
  U32 m_uOriginalHeight;
  BOOL m_bInVideoMemory;

  IFXString* m_pTextureName;      // original name of texture when read in

  IFXenum m_eSource;
  U32   m_uUserData;

  U32 m_uId;
  U32 m_uVersion;

  struct CubeMapTexture
  {
    U32     puTextureId[MAX_CUBIC_MAP_SIZE];
    IFXString* pCubeMapTexName[MAX_CUBIC_MAP_SIZE];
    IFXTextureObject* pTextureObjectList[MAX_CUBIC_MAP_SIZE];
  };

//  Cubic Map
  CubeMapTexture* m_pCubeMapTexture;

  static IFXIDManagerPtr ms_spIDManager;

  U32     ComputeBufferSize(  U32 uWidth, U32 uHeight, U32 uFormat);
  IFXRESULT Reallocate( U32 uWidth, U32 uHeight, U32 uFormat);
  IFXRESULT ConstructImageFromBuffer( STextureSourceInfo* pImageInfo, void* pBuffer );
  IFXRESULT ConstructQueueFromImage( IFXDataBlockQueueX* pDataBlockQueueX );
  IFXRESULT DecompressImage( );
  IFXRESULT ConstructImage( IFXRenderContext* pRenderContext);
  IFXRESULT CheckCompressionProperties(
            const U32 uContImageCount,
            const U8* pBlockCompressionType,
            IFXTextureObject::ChannelType* pImageChannelFlagsList);
  IFXRESULT CheckCompressionType(
            const U32 uContImageCount,
            const U8* pBlockCompressionType);
  void CleanFileRefList(U32 contImage);
  bool CheckChannel(
            Format imageFormat,
            U32 uContImageCount,
            ChannelType* pImageChannelFlagsList);
  BOOL IsWholeImageFromExternalFile(const STextureSourceInfo* pImageInfo);

  //Cube Map Texture Support Methods
  void CheckAndProcessCubeMapTextureX(IFXDataBlockQueueX &rSourceQueueX);
  void ProcessCubeMapDeclarationBlockX(IFXBitStreamX& rBitStreamX);
  void MakeCubeMapDeclarationBlockX(IFXDataBlockQueueX &rDataBlockQueueX);


  // private class constants...
  enum {
      TEXTURE_DEFAULT_ANIMATED = 0,
      TEXTURE_DEFAULT_WIDTH = 0,
      TEXTURE_DEFAULT_HEIGHT = 0,
      TEXTURE_DEFAULT_FORMAT = IFXTEXTUREMAP_FORMAT_RGB24
  };

};

#endif
