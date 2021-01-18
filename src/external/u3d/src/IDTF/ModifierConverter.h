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
@file ModifierConverter.h

This header defines the ... functionality.

@note
*/


#ifndef ModifierConverter_H
#define ModifierConverter_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "ConverterResult.h"

class IFXBoneWeightsModifier;
class IFXSkeleton;
class IFXString;

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

class Modifier;
class ShadingModifier;
class AnimationModifier;
class BoneWeightModifier;
class CLODModifier;
class SubdivisionModifier;
class GlyphModifier;
class ModifierList;
class SceneUtilities;

/**
This is the implementation of a class that is used to @todo: usage.

It supports the following interfaces:  @todo: interfaces.
*/
class ModifierConverter
{
public:
	ModifierConverter( 
		ModifierList* pModifierList, 
		SceneUtilities* pSceneUtils );
	virtual ~ModifierConverter();

	/**
	*/
	virtual IFXRESULT Convert();

protected:
	IFXRESULT ConvertModifier( const Modifier* pIDTFModifier );

	IFXRESULT ConvertShadingModifier( const ShadingModifier* pIDTFModifier );

	IFXRESULT ConvertAnimationModifier(
					const AnimationModifier* pIDTFModifier );

	IFXRESULT ConvertBoneWeightModifier(
					const BoneWeightModifier* pIDTFModifier );

	IFXRESULT ConvertCLODModifier( const CLODModifier* pIDTFModifier );

	IFXRESULT ConvertSubdivisionModifier( 
					const SubdivisionModifier* pIDTFModifier );

	IFXRESULT ConvertGlyphModifier( const GlyphModifier* pIDTFModifier );

private:
	ModifierConverter();

	ModifierList* m_pModifierList;
	SceneUtilities* m_pSceneUtils;
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

//***************************************************************************
//  Failure return codes
//***************************************************************************

/**
@todo:  Insert module/interface specific return code description.
*/
//#define IFX_E_????  MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_????, 0x0000 )

}

#endif
