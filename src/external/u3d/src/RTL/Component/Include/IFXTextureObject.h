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
	@file	IFXTextureObject.h
	
			Header file for the interfaces classes for the texture manager.
*/

#ifndef __IFXTEXTURE_OBJECT_H__
#define __IFXTEXTURE_OBJECT_H__


#include "IFXCoreCIDs.h"

#include "IFXMarkerX.h"
#include "IFXRenderContext.h"
#include "IFXEnums.h"
#include "IFXBlockTypes.h"
#include "IFXArray.h"

class IFXRenderContext;

class STextureSourceInfo;

/**
	This IID identifies the IFXTextureObject interface.

	@note	The GUID string is {DEC1B7AA-3DC7-11d3-81F4-0004AC2EDBCD}.
*/
// {DEC1B7AA-3DC7-11d3-81F4-0004AC2EDBCD}
IFXDEFINE_GUID(IID_IFXTextureObject,
0xdec1b7aa, 0x3dc7, 0x11d3, 0x81, 0xf4, 0x0, 0x4, 0xac, 0x2e, 0xdb, 0xcd);

/**
	This IID identifies the IFXTextureImageTools interface.

	@note	The GUID string is {4159DAFC-3E4A-45a2-92CB-16143E7D7447}.
*/
// {4159DAFC-3E4A-45a2-92CB-16143E7D7447}
IFXDEFINE_GUID(IID_IFXTextureImageTools,
0x4159dafc, 0x3e4a, 0x45a2, 0x92, 0xcb, 0x16, 0x14, 0x3e, 0x7d, 0x74, 0x47);

const U32 IFX_MAX_CONTINUATIONIMAGE_COUNT = 4;  // max could be R + G + B + Alpha

/**
	This struct contains renderable image information
*/
struct STextureOutputInfo 
{
	U32 m_width;		///< Desired width of image
	U32 m_height;	///< Desired height of image
	U8* m_pData;		///< Pointer to memory location to copy image into
	U32 m_pitch;		///< Pitch (in bytes) between rows of the image
	IFXenum eRenderFormat;	///< Desired render format of the image
	IFXenum eChannelOrder;	///< Desired channel order of the image (rgba or bgra)
};


class IFXTextureObject : virtual public IFXMarkerX
{
public:
	/**
		Use one of these values to indicate the format of uncompressed image.
		For example: IFXTEXTUREMAP_FORMAT_RGB24 indicates that the uncompressed
		             image is RGB 24 bits alignment.
	*/
	
	// STextureSourceInfo.m_eEncoding
	enum Format
	{
		IFXTEXTUREMAP_FORMAT_LUMINANCE			= 1,
		IFXTEXTUREMAP_FORMAT_RGB24				= 2,
		IFXTEXTUREMAP_FORMAT_BGR24				= 3,
		IFXTEXTUREMAP_FORMAT_RGBA32				= 4,
		IFXTEXTUREMAP_FORMAT_BGRA32				= 5,
		IFXTEXTUREMAP_FORMAT_ALPHA				= 6,
		IFXTEXTUREMAP_FORMAT_LUMINANCE_ALPHA	= 7
	};

	enum 
	{
		TextureType_Jpeg24  = 1,
		TextureType_Png		= 2,
		TextureType_Jpeg8	= 3
	};

	enum ChannelType 
	{
			IFXIMAGECHANNEL_ALPHA		= 0x01,
			IFXIMAGECHANNEL_BLUE		= 0x02,
			IFXIMAGECHANNEL_GREEN		= 0x04,
			IFXIMAGECHANNEL_RED			= 0x08,
			IFXIMAGECHANNEL_LUMINANCE	= 0x10
	};


	// IFXTextureObject methods...

	/**
		This method sets the texture object as dirty and removes the texture data
		out of video memory.

		@param	IFXRenderContext* pRenderContext
				The pointer to the render layer.
		@return An IFXRESULT code.
		@retval	IFX_OK	Successful.
		@retval	IFX_E_INVALID_POINTER	Pointer specified is invalid.
	*/

	virtual IFXRESULT IFXAPI  MakeDirty(IFXRenderContext* pRenderContext) = 0;

  	/**
  		This method fills texture information structure.

  		@param	STextureSourceInfo* pImageInfo
  				The pointer to the texture information structure.
  		@param	void* pImage
  				The pointer to the image data.
  		@return An IFXRESULT code.
		@retval	IFX_OK	Successful.
		@retval	IFX_E_INVALID_POINTER	Pointer specified is invalid.
  	*/

	virtual IFXRESULT IFXAPI  SetRawImage(STextureSourceInfo* pImageInfo, void* pImage) = 0;

	/**
		This method will get the image into a renderable format and copied
		to the memory location specified in rImageInfo.pData.

		@param	rImageInfo	This STextureOutputInfo struct contains
							the memory location, desired size, and
							desired render format for the image to be
							copied into.
		@return IFXRESULT
		@retval	IFX_OK	Successful.
		@retval	IFX_TEXTURE_OBJECT_NOT_INITIALIZED	Object is not yet initialized.
		@retval	IFX_E_INVALID_RANGE	The render format or channel order specified is not valid.
	*/
	virtual IFXRESULT IFXAPI  GetRenderImage(STextureOutputInfo& rImageInfo) = 0;

	/**
		This method causes the raw image data to be compressed. This methods should be
		called after SetRawImage.

		@param	IFXTextureCompressionState eCompressionState
				Specifies the compression state.
		@return An IFXRESULT code.
		@retval	IFX_OK	Successful.
		@retval	IFX_TEXTURE_OBJECT_NOT_INITIALIZED	Object is not yet initialized.
		@retval	IFX_E_OUT_OF_MEMORY	System memory request failed.
		@retval	IFX_E_UNSUPPORTED	Compression state is not supported.
		@retval	IFX_TEXTURE_MAP_INVALID_SIZEIncorrect specified image size.
	*/

	virtual IFXRESULT IFXAPI  ForceCompressionState( IFXenum eCompressionState ) = 0;

	/**
		This method gets current compression state.

		@param	IFXTextureCompressionState* eCompressionState
				The current compression state returns in this parameter.
		@return An IFXRESULT code.
		@retval	IFX_OK	Successful.
		@retval	IFX_E_INVALID_POINTER	Pointer specified is invalid.
	*/
	virtual IFXRESULT IFXAPI  GetCompressedState( IFXenum* eCompressionState ) = 0;

	/**
		This method sets the default texture.
		@param	IFXTextureCompressionState* eCompressionState
				The current compression state returns in this parameter.
		@return An IFXRESULT code.
		@retval	IFX_OK	Successful.
		@retval	IFX_TEXTURE_OBJECT_NOT_INITIALIZED	Object is not yet initialized.
		@retval	IFX_E_OUT_OF_MEMORY	System memory request failed.
	*/
	virtual IFXRESULT IFXAPI  SetDefaultTexture() = 0;


	// Texture Object properties methods...
	/**
		This method gets the current width of the texture object.

		@param	U32 *puValue
				The returned value is store in this parameter.

		@return An IFXRESULT code.
		@retval	IFX_OK	Successful.
		@retval	IFX_TEXTURE_OBJECT_NOT_INITIALIZED	Object is not yet initialized.
		@retval	IFX_E_INVALID_POINTER	Pointer specified is invalid.

	*/
	virtual IFXRESULT IFXAPI  GetWidth( U32 *puValue)  = 0;

	/**
		This method sets the new width for the texture object. If the new value is
		not power of 2, then it will set to power of 2 and resize the texture image.

		@param	U32 uValue
				Sets uValue as the width for the texture object.
				If the uValue is not power of 2, it will convert to power of 2
				in next UpdateAndMakeCurrent().

		@return An IFXRESULT code.
		@retval	IFX_OK	Successful.
		@retval IFX_E_INVALID_RANGE	One or more parameters are out of range.

	*/
	virtual IFXRESULT IFXAPI  SetWidth( U32  uValue)   = 0;

	/**
		This method gets the current height of the texture object.

		@param	U32 *puValue
				The returned value is store in this parameter.
		@return An IFXRESULT code.
		@retval	IFX_OK	Successful.
		@retval	IFX_TEXTURE_OBJECT_NOT_INITIALIZED	Object is not yet initialized.
		@retval	IFX_E_INVALID_POINTER	Pointer specified is invalid.

	*/
	virtual IFXRESULT IFXAPI  GetHeight( U32 *puValue) = 0;

	/**
		This method sets the new height for the texture object. If the new value is
		not power of 2, then it will set to power of 2 and resize the texture image.

		@param	U32 uValue
				Sets uValue as the height for the texture object.
				If the uValue is not power of 2, it will convert to power of 2
				in next UpdateAndMakeCurrent().

		@return An IFXRESULT code.
		@retval	IFX_OK	Successful.
		@retval IFX_E_INVALID_RANGE	One or more parameters are out of range.

	*/
	virtual IFXRESULT IFXAPI  SetHeight( U32  uValue)  = 0;

	/**
		This method gets the current format of uncompressed image.

		@param	U32 *puValue
				The returned value is store in this parameter.

		@return An IFXRESULT code.
		@retval	IFX_OK	Successful.
		@retval IFX_E_INVALID_RANGE	One or more parameters are out of range.

	*/

	virtual IFXRESULT IFXAPI  GetFormat( U32 *puValue) = 0;

	/**
		This method sets Animated flag to indicate that the raw input image
		is animated image data.

		@param	BOOL bValue
				- @b TRUE  - From animated source image.
				- @b FALSE - None animated source image.

		@return An IFXRESULT code.
		@retval	IFX_OK	Successful.

	*/
	virtual IFXRESULT IFXAPI  SetAnimated( BOOL  bValue)   = 0;


	/**
		This method gets pixel format that uses to render the Texture Object.
		The returned value stores in puValue parameter.
		@param	IFXenum &eFormat
				Specifies pixel format with one of these types:
				- @b IFX_RGBA_8888
				- @b IFX_RGBA_8880
				- @b IFX_RGBA_5650
				- @b IFX_RGBA_5550
				- @b IFX_RGBA_5551
				- @b IFX_RGBA_4444
				- @b IFX_TEXCOMPRESS_1
				- @b IFX_TEXCOMPRESS_2
				- @b IFX_TEXCOMPRESS_3
				- @b IFX_TEXCOMPRESS_4
				- @b IFX_TEXCOMPRESS_5
				- @b IFX_FMT_DEFAULT
		@return An IFXRESULT code.
		@retval	IFX_OK	Successful.

	*/
	virtual IFXRESULT IFXAPI  GetRenderFormat(IFXenum& eFormat) = 0;

	/**
		This method sets pixel format to use when rendering the Texture Object.

		@param	IFXenum eFormat
				The eFormat is one of these types:
				- @b IFX_RGBA_8888
				- @b IFX_RGBA_8880
				- @b IFX_RGBA_5650
				- @b IFX_RGBA_5550
				- @b IFX_RGBA_5551
				- @b IFX_RGBA_4444
				- @b IFX_TEXCOMPRESS_1
				- @b IFX_TEXCOMPRESS_2
				- @b IFX_TEXCOMPRESS_3
				- @b IFX_TEXCOMPRESS_4
				- @b IFX_TEXCOMPRESS_5
				- @b IFX_FMT_DEFAULT
		@return An IFXRESULT code.
		@retval	IFX_OK	Successful

	*/
	virtual IFXRESULT IFXAPI  SetRenderFormat(IFXenum eFormat)   = 0;

	/**
		This method gets the flag that indicates the decompressed image should
		be kept around in Texture object or not.

		@param	BOOL *pbValue
				- @b TRUE  - The compressed image is in Texture object.
				- @b FALSE - The compressed image is not in Texture object.

		@return An IFXRESULT code.
		@retval	IFX_OK	Successful

	*/
	virtual IFXRESULT IFXAPI  GetKeepDecompressed( BOOL *pbValue) = 0;

	/**
		This method sets the flag to indicate that the decompressed raw image should
		be keep around in Texture ojbect or not.

		@param	BOOL bValue
				- @b TRUE  - Keeps the decompressed raw image in Texture object.
				- @b FALSE - Removeds the decompressed raw image in Texture object.

		@return An IFXRESULT code.
		@retval	IFX_OK	Successful

	*/
	virtual IFXRESULT IFXAPI  SetKeepDecompressed( BOOL  bValue)   = 0;


	/**
		This method gets the flag that indicates the compressed raw image should
		be kept around in Texture object or not.

		@param	BOOL *pbValue
				- @b TRUE  - The compressed image is in Texture object.
				- @b FALSE - The compressed image is not in Texture object.

		@return An IFXRESULT code.
		@retval	IFX_OK	Successful

	*/
	virtual IFXRESULT IFXAPI  GetKeepCompressed( BOOL *pbValue) = 0;

	/**
		This method sets the flag to indicate that the compressed raw image should
		be kept around in Texture object or not.

		@note The compressed image should always be kept in Texture Object.  When
		Texture object needs to update for rendering, it always goes back and uses
		the compressed image to update.

		@param	BOOL  bValue
				- @b TRUE  - Keeps the compressed image in Texture object.
				- @b FALSE - Don't keep compressed image from Texture object.

		@return An IFXRESULT code.
		@retval	IFX_OK	Successful


	*/
	virtual IFXRESULT IFXAPI  SetKeepCompressed( BOOL bValue) = 0;


	/**
		This method gets current Magnification filter setting in Texture Object.
		The returned value stores in peFilterMode parameter.

		@param	IFXTextureObjectFilterMode *peFilterMode

		@return An IFXRESULT code.
		@retval	IFX_OK	Successful
		@retval	IFX_TEXTURE_OBJECT_NOT_INITIALIZED	Object is not yet initialized.
		@retval	IFX_E_INVALID_POINTER	Pointer specified is invalid.

	*/
	virtual IFXRESULT IFXAPI  GetMagFilterMode( IFXenum *peFilterMode)  = 0;

	/**
		This method sets Magnification filter for Texture Object.

		@param	IFXTextureObjectFilterMode eFilterMode

		@return An IFXRESULT code.
		@retval	IFX_OK	Successful


	*/
	virtual IFXRESULT IFXAPI  SetMagFilterMode( IFXenum  eFilterMode)   = 0;

	/**
		This method gets current Minification filter setting in Texture Object.
		The returned value stores in peFilterMode parameter.

		@param	IFXTextureObjectFilterMode *peFilterMode

		@return An IFXRESULT code.
		@retval	IFX_OK	Successful
		@retval	IFX_TEXTURE_OBJECT_NOT_INITIALIZED	Object is not yet initialized.
		@retval	IFX_E_INVALID_POINTER	Pointer specified is invalid.

	*/
	virtual IFXRESULT IFXAPI  GetMinFilterMode( IFXenum *peFilterMode)  = 0;

	/**
		This method sets Minification filter for Texture Object.

		@param	IFXTextureObjectFilterMode eFilterMode

		@return An IFXRESULT code.
		@retval	IFX_OK	Successful


	*/
	virtual IFXRESULT IFXAPI  SetMinFilterMode( IFXenum  eFilterMode)   = 0;

	/**
		This method gets current MipMap setting in Texture Object.
		The returned value stores in peMipMode parameter.

		@param	IFXTextureObjectMipModes *peMipMode
		@brief The IFXTextureObjectMipModes can be one of the following values:
			- @b IFXTextureMipModeNone
			- @b IFXTextureMipModeNearest
			- @b IFXTextureMipModeLinear

		@return An IFXRESULT code.
		@retval	IFX_OK	Successful.
		@retval	IFX_E_INVALID_POINTER	Pointer specified is invalid.

	*/
	virtual IFXRESULT IFXAPI  GetMipMode( IFXenum *peMipMode)  = 0;

	/**
		This method sets MipMap value for Texture Object.

		@param	IFXTextureObjectMipModes eMipMode
		@brief The IFXTextureObjectMipModes can be one of the following values:
			- @b IFXTextureMipModeNone
			- @b IFXTextureMipModeNearest
			- @b IFXTextureMipModeLinear

		@return An IFXRESULT code.
		@retval	IFX_OK	Successful


	*/
	virtual IFXRESULT IFXAPI  SetMipMode( IFXenum eMipMode)   = 0;

	/**
   This method hands back the texture source type.  If the texture object 
   is known to have been decoded from a block it will be 
   IFXTEXTURESOURCE_DECODED.  Otherwise, it will be IFXTEXURESOURCE_OTHER.
 
   @param peTextureSource Pointer to a value that upon success will be 
         initialized with the type of texture source 
         used.  The default is IFXTEXURESOURCE_OTHER.
 
   @return Upon success, IFX_OK is returned.  Otherwise, 
     IFX_E_INVALID_POINTER is returned.
  */


	virtual IFXRESULT IFXAPI  GetTextureSource( IFXenum *peTextureSource) = 0;

	/**
   This method sets the texture source type.  The texture source type is 
   used by the texture object to determine how to handle some operations.
 
   @param eTextureSource Specify the type of texture source used by 
         the texture object.  Use IFXTEXTURESOURCE_DECODED 
         if the texture object came from a decoded block.  
         Otherwise, specify IFXTEXURESOURCE_OTHER which is 
         the default.
 
   @return IFX_OK is always returned.
  */


	virtual IFXRESULT IFXAPI  SetTextureSource( IFXenum eTextureSource)   = 0;

	/**
		This method gets a tuxture map type of this Texture object.
		Texture map type can be one of two types,IFX_TEXTURE_2D or IFX_TEXTURE_CUBE.

		@return IFXenum *puMapType.
				- @b IFX_TEXTURE_2D	 - Regular 2D texture map type.
				- @b IFX_TEXTURE_CUBE  - Cubic environment map type.
		@retval	IFX_OK	Successful
	*/
	virtual IFXRESULT IFXAPI  GetTextureType( IFXenum *puMapType) = 0;

	/**
		This method sets a 2D or cubic map type for this Texture object.
		Texture map type can be one of two types,IFX_TEXTURE_2D or IFX_TEXTURE_CUBE.
		If texture created for use as cubic environment mapping, it needs to set as
		IFX_TEXTURE_CUBE. When creates the Texture object, the default texture type
		sets to IFX_TEXTURE_2D.

		@return An IFXRESULT code.
		@retval	IFX_OK	Successful
	*/
	virtual IFXRESULT IFXAPI  SetTextureType( IFXenum  uMapType) = 0;

	/**
		This method gets Texture object of ePosition in cube map texture.
		The Texture object returns back in the pTexture parameter.

		@return An IFXRESULT code.
		@retval	IFX_OK	Successful
	*/
	virtual IFXRESULT IFXAPI  GetCubeMapTexture( 
							IFXenum ePosition, 
							IFXTextureObject **ppTexture) = 0;

	/**
		This method sets new Texture object for ePosition in cube map texture.
		The Texture object returns back in the pTexture parameter.

		@return An IFXRESULT code.
		@retval	IFX_OK	Successful
	*/
	virtual IFXRESULT IFXAPI  SetCubeMapTexture( U32 uTextureName, IFXenum ePosition) = 0;

	/**
		This method gets the flag of this Texture object that indicates if it has
		been initialized.

		@return BOOL
		@retval	TRUE	Object has initialized.
		@retval	FALSE	Object has not yet initialized.

	*/
	virtual BOOL	  IFXAPI  IsInitialize() = 0;

	/**
		This method gets the size of the raw image in Texture object.
		The size of the raw image is returned in the puValue.

		@param  U32 *upValue
		@return An IFXRESULT code.
		@retval	IFX_OK	Successful.
				IFX_TEXTURE_OBJECT_NOT_INITIALIZED - Object is not yet initialized.
	*/
	virtual IFXRESULT IFXAPI  GetRawImageBufferSize( U32 *puValue) = 0;


	/**
		This method gets a Class ID of the image codec.
		The Class ID info is returned and stores in CodeCID.

		@param  IFXCID& CodecCID
		@return An IFXRESULT code.
		@retval	IFX_OK	Successful
		@retval	IFX_TEXTURE_OBJECT_NOT_INITIALIZED	Object is not yet initialized
	*/
	virtual IFXRESULT IFXAPI  GetImageCodec(IFXCID* m_pCodecCID) = 0;

	/**
		This method sets a new Class ID of the image codec.
		The Class ID info is stored in CodeCID.

		@param  IFXCID* m_pCodecCID
		@return An IFXRESULT code.
		@retval	IFX_OK	Successful
		@retval	IFX_TEXTURE_OBJECT_NOT_INITIALIZED	Object is not yet initialized
	*/
	virtual IFXRESULT IFXAPI  SetImageCodec(const IFXCID* m_pCodecCID) = 0;

	/**
		This method gets a compression quality value of image codec.
		The quality setting value is returned and stores in uImageCodecQuality.

		@param  U32 uImageCodecQuality
		@return An IFXRESULT code.
		@retval	IFX_OK	Successful
		@retval	IFX_TEXTURE_OBJECT_NOT_INITIALIZED	Object is not yet initialized
	*/
	virtual IFXRESULT IFXAPI  GetImageCodecQuality(U32& uImageCodecQuality) = 0;

	/**
		This method sets a new compression quality value for image codec.
		The quality setting value is stored in uImageCodecQuality.

		@param  U32 uImageCodecQuality
		@return An IFXRESULT code.
		@retval	IFX_OK	Successful
		@retval	IFX_TEXTURE_OBJECT_NOT_INITIALIZED	Object is not yet initialized
	*/
	virtual IFXRESULT IFXAPI  SetImageCodecQuality(const U32 uImageCodecQuality) = 0;


	/**
		This method will get the unique ID associated with this texture.

		@return U32 value representing the unique ID for this texutre.
	*/
	virtual U32		  IFXAPI  GetId() = 0;

	/**
		This method will get the "version" of the texture.  Whenever
		any texture property changes, this version number will change.

		@return U32 value representing the version of this texture.
	*/
	virtual U32		  IFXAPI  GetVersion() = 0;

	/**
		This method deletes the temporary rendering image buffer.  This
		method only call by the Render Layer.

		@return An IFXRESULT code.
		@retval	IFX_OK	Successful
	*/
	virtual IFXRESULT IFXAPI 	PurgeRenderImage() = 0;

	/**
		This method returns true if the image has Alpha color, otherwise
		it returns false;

		@return An IFXBOOL code.
		@retval	IFX_TRUE Supports Alpha color.
		@retval IFX_FALSE Not supports Alpha color.
	*/
	virtual BOOL IFXAPI 	HasAlphaColor() = 0;


	/**
		This method returns Texture Image Type.		

		@param	Texture Image Type returns in this parameter 
		@return An IFXRESULT value.
		@retval IFX_OK	Successful
		@retval IFX_E_NOT_INITIALIZED
	*/
	virtual IFXRESULT IFXAPI  GetImageType(IFXTextureObject::Format& rImageType) const = 0;
 
	/**
		This method gets a number of Continutaion images used in U3D file where object stored 
		
		@param	rCount - Continuation image count returns here
		@return An IFXRESULT value.
		@retval	IFX_OK	Successful
		@retval	IFX_E_NOT_INITIALIZED	
	*/
    virtual IFXRESULT IFXAPI  GetContinuationImageCount(U32& rCount) const = 0;

	/**
		This method gets compression settings used at image decoding from U3D file

		@param	pBlockCompressionTypeList	array of Compression Types 
		@param	pImageChannelFlagsList		array of Channel Types
					For above parameters should be at at least rCount elements allocated 
					rCount could be retrieved using GetContinuationImageCount method 
		@param	pExternalFileReferenceList	array of elements, where each element 
					indicates if continuation image retrieved from external files should 
					be at at least rCount bool elements allocated at *pBlockComporession 
					or if pExternalFileReferenceList == NULL - so correspodning image 
					data just won't be returned by the call\n
		@param	ppExternalFileReferenceArrayList	
					array of arrays storing external file names for corresponding 
					continuation image should be at at least rCount IFXArray* elements
					allocated at * ppExternalFileReferenceArrayList or 
					if ppExternalFileReferenceArrayList == NULL - so correspodning image
					data just won't be returned by the call
		@return An IFXRESULT value.
		@retval	IFX_OK	No error
		@retval IFX_E_NOT_INITIALIZED
		@retval IFX_E_INVALID_POINTER

	*/
	virtual IFXRESULT IFXAPI  GetImageCompressionProperties(
							U8 *pBlockCompressionTypeList, 
							IFXTextureObject::ChannelType* pImageChannelFlagsList,   
							BOOL *pExternalFileReferenceList,  
							IFXArray<IFXString*>** ppExternalFileReferenceArrayList)= 0 ;
 
	/**
		This method allows client set required compression settings that will 
		be used at image encoding to U3D file 
		@param  continuationImageCount		Continuation Image count that 
											will be used at encoding   
		@param	pBlockCompressionTypeList	array of Compression types, where 
											single element corresponds to single 
											continutaion image	
		@param	pImageChannelFlagsList		array of Channel types, where single element 
											corresponds to single continutaion image
											Above arrays should be at least 
											continuationImageCount size 
		@param	pExternalFileReferenceList	array of elements, where each element 
											indicates if continuation image should be 
											referred to external files
		@param	pExternalFileReferenceArrayList array of arrays settin external file 
					names for corresponding continuation image
					if pExternalFileReferenceList and pExternalFileReferenceArrayList 
					are NULL - it means none of Continutaion images won't be referenced 
					as external file if they are not NULLs they should be at least 
					continuationImageCount size

		@return An IFXRESULT value.
		@retval IFX_OK	No error
		@retval IFX_E_NOT_INITIALIZED	
		@retval IFX_E_INVALID_POINTER
		@retval IFX_E_INVALID_CONT_IMAGE_COUNT
		@retval IFX_E_INVALID_COMPRESSION_TYPE
		@retval IFX_E_UNSUPPORTED                      
	*/
    virtual IFXRESULT IFXAPI  SetImageCompressionProperties(
							U32 continuationImageCount, U8 *pBlockCompressionTypeList, 
							IFXTextureObject::ChannelType* pImageChannelFlagsList,
							BOOL* pExternalFileReferenceList,  
							IFXArray<IFXString*>** pExternalFileReferenceArrayList ) = 0;

};

class STextureSourceInfo
{
public:
	IFXString m_name;			//Name of texture
	U32 m_width;				//The Width of image in pixels
	U32 m_height;				//The Height of image in pixels
	U32 m_size;					//The Size of image in bytes

	// Number of continuation images(blocks) the texture was loaded from
	U32	m_imageLoadCount;

	U8  m_imageType;				//ImageType
	U8  m_compressionQuality;	//The compression quality setting

	//Compression flag: Jpeg24, Png or Jpeg8
	U8  m_blockCompressionType[IFX_MAX_CONTINUATIONIMAGE_COUNT];

	// Channels per block - could be OR IFXIMAGECHANNEL_ALPHA,  
	// IFXIMAGECHANNEL_RED, IFXIMAGECHANNEL_GREEN, IFXIMAGECHANNEL_BLUE
	// IFXIMAGECHANNEL_LUMINANCE
	U8	m_blockChannels[IFX_MAX_CONTINUATIONIMAGE_COUNT];

	IFXCID* m_pCodecCID;	//Codec's ClassID .
	U32	    m_imageURLCount;
	BOOL    m_keepExternalImagesAtEncoding;

	// List URL names for corresponding continutaion image 
	IFXArray<IFXString*> m_pURLFileNameList[IFX_MAX_CONTINUATIONIMAGE_COUNT];

	// index of element from corresponding m_pURLFileNameList image was loaded from   
	U32 m_URLFileLoadedIndex[IFX_MAX_CONTINUATIONIMAGE_COUNT]; 

	// true if corresponding continuation image stored in external file
	BOOL m_URLFileRef[IFX_MAX_CONTINUATIONIMAGE_COUNT];

	STextureSourceInfo() 
	{
		m_name   = L"";
		m_width  = 0; 
		m_height = 0;
		m_size   = 0; 
		m_imageLoadCount = 1;
		m_imageType = 0;
		m_compressionQuality = 100; 
		m_pCodecCID		= NULL;
		m_imageURLCount  = 0;
		m_keepExternalImagesAtEncoding = true;
		
		U32 i;
		for (i=0; i<IFX_MAX_CONTINUATIONIMAGE_COUNT; i++) 
		{
			m_blockCompressionType[i]	= IFXTextureObject::TextureType_Png; 
			m_blockChannels[i]          = 
				(IFXTextureObject::IFXIMAGECHANNEL_RED|
				IFXTextureObject::IFXIMAGECHANNEL_GREEN|
				IFXTextureObject::IFXIMAGECHANNEL_BLUE);
			m_URLFileLoadedIndex[i]		= (U32)-1;
			m_URLFileRef[i]				= false;
		}
	}

	~STextureSourceInfo () 
	{
		U32 contImage;
		for(contImage = 0; contImage < IFX_MAX_CONTINUATIONIMAGE_COUNT; contImage++ )
		{
			U32 fileInd;
			for( fileInd=0; fileInd < m_pURLFileNameList[contImage].GetNumberElements();fileInd++ ) 
			{
				IFXString* ptr = (m_pURLFileNameList[contImage])[fileInd];
				IFXDELETE( ptr );
			}
			m_pURLFileNameList[contImage].Clear();
		}
	}

	STextureSourceInfo& operator=(const STextureSourceInfo& src) 
	{
		if( this != &src )
		{
			m_name							= src.m_name;
			m_width							= src.m_width; 
			m_height						= src.m_height;
			m_size							= src.m_size; 
			m_imageLoadCount				= src.m_imageLoadCount;
			m_imageType						= src.m_imageType;
			m_compressionQuality			= src.m_compressionQuality;
			m_pCodecCID						= src.m_pCodecCID;		
			m_imageURLCount					= src.m_imageURLCount;
			m_keepExternalImagesAtEncoding	= src.m_keepExternalImagesAtEncoding;

			U32 i;
			for (i =0; i<m_imageLoadCount; i++) 
			{       
				m_blockCompressionType[i] = src.m_blockCompressionType[i];
				m_blockChannels[i]        = src.m_blockChannels[i];
				m_URLFileLoadedIndex[i]   = src.m_URLFileLoadedIndex[i];
				m_URLFileRef[i]           = src.m_URLFileRef[i];
			}
		}
		return *this;
	}
};

#endif
