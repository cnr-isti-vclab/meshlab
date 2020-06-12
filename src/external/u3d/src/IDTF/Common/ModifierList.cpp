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
  @file ModifierList.cpp

      This module defines ...
*/


//***************************************************************************
//  Includes
//***************************************************************************

#include "ModifierList.h"
#include "Tokens.h"

using namespace U3D_IDTF;

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


//***************************************************************************
//  Global data
//***************************************************************************


//***************************************************************************
//  Local data
//***************************************************************************


//***************************************************************************
//  Local function prototypes
//***************************************************************************


//***************************************************************************
//  Public methods
//***************************************************************************

ModifierList::ModifierList()
{
}

ModifierList::~ModifierList()
{
}

IFXRESULT ModifierList::AddModifier( const Modifier* pModifier )
{
	IFXRESULT result = IFX_OK;
	Modifier* pTmpModifier = NULL;

	const IFXString& rModifierType = pModifier->GetType();

	if( rModifierType == IDTF_SHADING_MODIFIER )
	{
		ShadingModifier& shadingModifier = m_shadingModifierList.CreateNewElement();
		shadingModifier = *static_cast<const ShadingModifier*>(pModifier);
		pTmpModifier = &shadingModifier;
	}
	else if( rModifierType == IDTF_ANIMATION_MODIFIER )
	{
		AnimationModifier& animationModifier = 
			m_animationModifierList.CreateNewElement();
		animationModifier = *static_cast<const AnimationModifier*>(pModifier);
		pTmpModifier = &animationModifier;
	}
	else if( rModifierType == IDTF_BONE_WEIGHT_MODIFIER )
	{
		BoneWeightModifier& boneWeightModifier = 
			m_boneWeightModifierList.CreateNewElement();
		boneWeightModifier = *static_cast<const BoneWeightModifier*>(pModifier);
		pTmpModifier = &boneWeightModifier;
	}
	else if( rModifierType == IDTF_CLOD_MODIFIER )
	{
		CLODModifier& clodModifier = 
			m_clodModifierList.CreateNewElement();
		clodModifier = *static_cast<const CLODModifier*>(pModifier);
		pTmpModifier = &clodModifier;
	}
	else if( rModifierType == IDTF_SUBDIVISION_MODIFIER )
	{
		SubdivisionModifier& subdivModifier = 
			m_subdivModifierList.CreateNewElement();
		subdivModifier = *static_cast<const SubdivisionModifier*>(pModifier);
		pTmpModifier = &subdivModifier;
	}
	else if( rModifierType == IDTF_GLYPH_MODIFIER )
	{
		GlyphModifier& glyphModifier = 
			m_glyphModifierList.CreateNewElement();
		glyphModifier = *static_cast<const GlyphModifier*>(pModifier);
		pTmpModifier = &glyphModifier;
	}
	else
		result = IFX_E_UNDEFINED;

	if( IFXSUCCESS( result ) )
	{
		// add new modifier pointer to the modifier pointer list only 
		// if it has known type
		Modifier*& modifier = m_modifierPointerList.CreateNewElement();
		modifier = pTmpModifier;
	}

	return result;
}

const Modifier* ModifierList::GetModifier( U32 index ) const
{
	return m_modifierPointerList.GetElementConst( index );
}

U32	  ModifierList::GetModifierCount() const
{
	return m_modifierPointerList.GetNumberElements();
}

//***************************************************************************
//  Protected methods
//***************************************************************************


//***************************************************************************
//  Private methods
//***************************************************************************


//***************************************************************************
//  Global functions
//***************************************************************************


//***************************************************************************
//  Local functions
//***************************************************************************
