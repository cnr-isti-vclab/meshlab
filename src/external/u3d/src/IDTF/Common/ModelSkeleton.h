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
@file ModelSkeleton.h

This header defines the ... functionality.

@note
*/


#ifndef ModelSkeleton_H
#define ModelSkeleton_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXArray.h"
#include "Point.h"
#include "Quat.h"

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

struct BoneInfo
{
	IFXString name;
	IFXString parentName;
	F32 length;
	Point displacement;
	Quat orientation;
};

/**
This is the implementation of a class that is used to @todo: usage.

It supports the following interfaces:  @todo: interfaces.
*/
class ModelSkeleton
{
public:
	ModelSkeleton() {};
	~ModelSkeleton() {};

	void AddBoneInfo( const BoneInfo& rBoneInfo );
	const BoneInfo& GetBoneInfo( U32 index ) const;
	U32 GetBoneInfoCount() const;

private:
	IFXArray< BoneInfo > m_boneInfoList; // number of bone infos
};

//***************************************************************************
//  Inline functions
//***************************************************************************

IFXFORCEINLINE void
	ModelSkeleton::AddBoneInfo( const BoneInfo& rBoneInfo )
{
	BoneInfo& boneInfo = m_boneInfoList.CreateNewElement();
	boneInfo = rBoneInfo;
}

IFXFORCEINLINE const BoneInfo& ModelSkeleton::GetBoneInfo( U32 index ) const
{
	return m_boneInfoList.GetElementConst( index );
}

IFXFORCEINLINE U32 ModelSkeleton::GetBoneInfoCount() const
{
	return m_boneInfoList.GetNumberElements();
}


//***************************************************************************
//  Global function prototypes
//***************************************************************************


//***************************************************************************
//  Global data
//***************************************************************************

}

#endif
