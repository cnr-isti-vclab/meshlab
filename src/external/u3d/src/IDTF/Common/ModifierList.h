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
@file SceneResources.h

This header defines the ... functionality.

@note
*/


#ifndef ModifierList_H
#define ModifierList_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXResult.h"
#include "Modifier.h"
#include "ShadingModifier.h"
#include "AnimationModifier.h"
#include "BoneWeightModifier.h"
#include "CLODModifier.h"
#include "SubdivisionModifier.h"
#include "GlyphModifier.h"

#include "IFXString.h" // needed for IFXArray.h
#include "IFXArray.h"

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


/**
This is the implementation of a class that is used to @todo: usage.

It supports the following interfaces:  @todo: interfaces.
*/
class ModifierList
{
public:
	ModifierList();
	virtual ~ModifierList();

	IFXRESULT AddModifier( const Modifier* pModifier );
	const Modifier* GetModifier( U32 index ) const;
	U32   GetModifierCount() const;

private:
	IFXArray< Modifier* > m_modifierPointerList;
	IFXArray< ShadingModifier > m_shadingModifierList;
	IFXArray< AnimationModifier > m_animationModifierList;
	IFXArray< BoneWeightModifier > m_boneWeightModifierList;
	IFXArray< CLODModifier > m_clodModifierList;
	IFXArray< SubdivisionModifier > m_subdivModifierList;
	IFXArray< GlyphModifier > m_glyphModifierList;
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
