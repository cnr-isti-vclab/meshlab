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
	@file	IFXSkeletonDataElement.h

			The header file that defines the IFXSkeletonDataElement.  
*/

#ifndef __IFXSKELETONDATAELEMENT_H__
#define __IFXSKELETONDATAELEMENT_H__

#include "IFXDataTypes.h"

typedef struct IFXCurrentBoneData
{
	IFXMatrix4x4		boneTransform;
	IFXVector3			boneDisplacement;
	F32					boneLength;
	I32					iParentIndex;
	U32					isValid;
} IFXCurrentBoneData;

typedef struct IFXSkeletonDataElement
{
	U32					uNumBones;
	IFXCurrentBoneData	*pBoneArray;
} IFXSkeletonDataElement;

#endif
