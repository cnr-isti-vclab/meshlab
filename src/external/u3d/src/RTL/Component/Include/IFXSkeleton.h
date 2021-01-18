//***************************************************************************
//
//  Copyright (c) 2000 - 2006 Intel Corporation
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
	@file	IFXSkeleton.h

			Declaration of IFXSkeleton interface.
			The IFXSkeleton interface is used to get and set bone information.
			The bone information is used for bones based character animation.
*/

#ifndef IFXSKELETON_H__
#define IFXSKELETON_H__

#include "IFXUnknown.h"
#include "IFXBones.h"

// {1FC935D0-47CB-4077-80BA-9AE89070B796}
IFXDEFINE_GUID(IID_IFXSkeleton,
0x1fc935d0, 0x47cb, 0x4077, 0x80, 0xba, 0x9a, 0xe8, 0x90, 0x70, 0xb7, 0x96);

/** 
	These members directly reflect attributes used by IFXBonesManager.
*/
struct IFXBoneInfo
{
	IFXBoneInfo(void)
	{
		//* defaults
		stringBoneName=L"<unnamed>";
		stringParentName=L"<unnamed>";
		iParentBoneID=				-1;
		fBoneLength=				1.0f;
		v3BoneDisplacement.Reset();
		v4BoneRotation.Set(1.0f,0.0f,0.0f,0.0f);
		uBoneAttributes=			0x0;
		fRotationConstraintXMax=	0.0f;
		fRotationConstraintXMin=	0.0f;
		fRotationConstraintYMax=	0.0f;
		fRotationConstraintYMin=	0.0f;
		fRotationConstraintZMax=	0.0f;
		fRotationConstraintZMin=	0.0f;
		uNumLinks=					0;
		fLinkLength=				1.0f;
		v2StartJointCenter.Reset();
		v2StartJointScale.Reset();
		v2EndJointCenter.Reset();
		v2EndJointScale.Reset();
		iLinkBoneID=				0;
	};

	IFXString			stringBoneName;
	IFXString			stringParentName;
	I32					iParentBoneID;
	F32					fBoneLength;
	IFXVector3			v3BoneDisplacement;
	IFXVector4			v4BoneRotation;
	U32					uBoneAttributes;
	F32					fRotationConstraintXMax;
	F32					fRotationConstraintXMin;
	F32					fRotationConstraintYMax;
	F32					fRotationConstraintYMin;
	F32					fRotationConstraintZMax;
	F32					fRotationConstraintZMin;
	U32					uNumLinks;
	F32					fLinkLength;
	IFXVector2			v2StartJointCenter;
	IFXVector2			v2StartJointScale;
	IFXVector2			v2EndJointCenter;
	IFXVector2			v2EndJointScale;
	I32					iLinkBoneID;
};

/**
	The IFXSkeleton interface is used to get and set bone information used 
	for bones based character animation.

	A skeleton holds the bone data and vertex weights for a particular model.

    @note	Underlying weight data is designed for sequential, not random, access.
			Restrictions are on a per-mesh independent basis.
			Vertex weight writes must be done in ascending order or
			data will be corrupted.
			Vertex weight reads/replaces should be done in ascending order or heavy
			performance penalties may result (Read & Replace use the same iterator).
			Write pass may be done only once (per mesh).
			Read/Replace location should never exceed write location.
			Replacements must use the exact number of weights as previously set.
*/
class IFXSkeleton : virtual public IFXUnknown
{
public:
		/// Sets all the data for a particular indexed bone from an IFXBoneInfo.
		virtual	IFXRESULT IFXAPI 	SetBoneInfo(U32 uBoneID, IFXBoneInfo *pBoneInfo)	=0;

		/// Gets all the data for a particular indexed bone to an IFXBoneInfo.
		virtual	IFXRESULT IFXAPI 	GetBoneInfo(U32 uBoneID, IFXBoneInfo *pBoneInfo)	=0;

		/// Gets the number of bones contained int he skeleton.
		virtual IFXRESULT IFXAPI 	GetNumBones(U32& uNumBones)						=0;

		/** Sets the blocktype from which this object was derived. There can be
			several blocktypes which can correspond on a one to one basis with
			a given object. This is generally the case with backwards file
			format compatibility issues. The purpose for setting this parameter
			is so that an encoder knows whether or not a cached copy of the
			blocks for this object is held by the generator to which this
			modifier belongs. */
		virtual IFXRESULT IFXAPI 	SetBlockTypeBase(U32 BlockType)					=0;

		/** Gets the blocktype from which this object was derived. There can be
			several blocktypes which can correspond on a one to one basis with
			a given object. This is generally the case with backwards file
			format compatibiliy issues. The purpose for determining this
			blocktype is so that an encoder knows whether or not a cached
			copy of the blocks for this object is held by the generator to
			which this modifier belongs. */
		virtual IFXRESULT IFXAPI 	GetBlockTypeBase(U32* pBlockType)				=0;

		/// Accesses the core Bonesmanager for this resource.
		virtual IFXBonesManager* IFXAPI GetBonesManagerNR(void)						=0;
};

#define IFXSKELETON_LINKPRESENT					(1 << 0)
#define IFXSKELETON_JOINTPRESENT				(1 << 1)
#define IFXSKELETON_ROTATIONCONSTRAINTACTIVE_X	(1 << 2)
#define IFXSKELETON_ROTATIONCONSTRAINTLIMITED_X	(1 << 3)
#define IFXSKELETON_ROTATIONCONSTRAINTACTIVE_Y	(1 << 4)
#define IFXSKELETON_ROTATIONCONSTRAINTLIMITED_Y	(1 << 5)
#define IFXSKELETON_ROTATIONCONSTRAINTACTIVE_Z	(1 << 6)
#define IFXSKELETON_ROTATIONCONSTRAINTLIMITED_Z	(1 << 7)

#endif
