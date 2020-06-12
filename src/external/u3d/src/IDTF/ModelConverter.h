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
	@file	ModelConverter.h

			This header defines the common functionality for models conversion.
*/


#ifndef ModelConverter_H
#define ModelConverter_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "IConverter.h"
#include "SceneUtilities.h"
#include "ShadingDescriptionList.h"

class IFXSkeleton;

namespace U3D_IDTF
{
//***************************************************************************
//  Defines
//***************************************************************************


//***************************************************************************
//  Constants
//***************************************************************************


//***************************************************************************
//  Enumerations
//***************************************************************************


//***************************************************************************
//  Classes, structures and types
//***************************************************************************

class ModelNode;
class ModelSkeleton;
struct BoneInfo;

/**
This is the implementation of a class that is used to convert models.

It supports the following interfaces:  IConverter.
*/
class ModelConverter : public IConverter
{
public:
	ModelConverter( SceneUtilities* pSceneUtils );
	virtual ~ModelConverter();

	/**
	Convert model
	*/
	IFXRESULT Convert();

	/**
	Set default geometry quality factor
	*/
	void SetDefaultQuality( U32 defaultGeoQuality );

	/**
	Set position quality factor
	*/
	void SetPositionQuality( U32 positionQuality );

	/**
	Set texture coordinate quality factor
	*/
	void SetTexCoordQuality( U32 texCoordQuality );

	/**
	Set normal quality factor
	*/
	void SetNormalQuality( U32 normalQuality );

	/**
	Set diffuse color quality factor
	*/
	void SetDiffuseColorQuality( U32 diffuseQuality );

	/**
	Set specular color quality factor
	*/
	void SetSpecularColorQuality( U32 specularQuality );

	/**
	Set zero area faces removal
	*/
	void SetZeroAreaFacesRemoval( BOOL isRemove );

	/**
	Set zero area face tolerance
	*/
	void SetZeroAreaFaceTolerance( F32 tolerance );

	/**
	Set exclude normals mode
	*/
	void SetNormalsExclusion( BOOL isExcludeNormals );

protected:

	/**
	@return IFXRESULT Return status of this method.
	*/
	IFXRESULT ConvertShadingDescriptions(
					const ShadingDescriptionList& rShadingDescriptions,
					const U32 numberOfShaders,
					IFXAuthorMaterial* pShaders );

	/**
	Read bone information from the IDTF file, using this to create
	the connectivity and reference-orientation representation of
	the skeleton. The IDTF file also specifies the animation of
	each bone. After collecting these motions for all bones, we
	create an IFXMotionResource for the that specifies the animation
	of the entire skeletal figure over the period defined by the keyframes.

	@param  ModelSkeleton& rIDTFSkeleton
	@param  IFXSkeleton** ppSkeleton  The populated skeletal bone data

	@return IFXRESULT Return status of this method.
	*/
	IFXRESULT ConvertSkeleton(
					const ModelSkeleton& rIDTFSkeleton,
					IFXSkeleton** ppSkeleton );

	/**
	Load in the reference orientation and keyframes for the
	specified bone.

	@param  IFXSkeleton* pSkeleton  The container holding information
									about all bones in this skeleton.
	@param  U32 boneIndex     The index of the bone being
							  converted.

	@return IFXRESULT       Return status of this method.
	*/
	IFXRESULT ConvertBone(
					IFXSkeleton* pSkeleton,
					U32 boneIndex,
					const BoneInfo& rIDTFBoneInfo );

	/**
	Return the ID of the bone that has the specified name

	@param   IFXSkeleton *pSkeleton The data structure containing information
									about all bones in the skeleton.
	@param   const IFXString& rName The name of the bone to search for

	@return  U32 -1 if there is an error or the named bone cannot be found.
				 0...N if we find the named bone in the skeleton.
	*/
	I32 GetBoneIdx( IFXSkeleton* pSkeleton, const IFXString& rName );

	SceneUtilities* m_pSceneUtils;

	U32 m_defaultGeoQuality;
	U32 m_positionQuality;
	U32 m_texCoordQuality;
	U32 m_normalQuality;
	U32 m_diffuseQuality;
	U32 m_specularQuality;
	BOOL m_removeZeroAreaFaces;
	F32 m_zeroAreaFaceTolerance;
	BOOL m_excludeNormals;

private:
	ModelConverter();
};

//***************************************************************************
//  Inline functions
//***************************************************************************


//***************************************************************************
//  Global function prototypes
//***************************************************************************


//***************************************************************************
//  Global data
//***************************************************************************

}

#endif
