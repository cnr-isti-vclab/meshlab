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
  @file ModifierUtilities.cpp

      This module defines ...
*/


//***************************************************************************
//  Defines
//***************************************************************************


//***************************************************************************
//  Includes
//***************************************************************************

#include "SceneUtilities.h"
using namespace U3D_IDTF;

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

/**
Attach an IFXModifier to an IFXNode or IFXAuthorResource.

@param   const IFXString& rModName  The name of modifier.
@param   const IFXString& rModChainType  The type of modifier chain.
@param   IFXModifier* pModifier  The modifier to add to the modifier chain.

@return  IFXRESULT Return status of this method.
**/
IFXRESULT SceneUtilities::AddModifier( 
								const IFXString& rModName,
								const IFXString& rModChainType,
								IFXModifier* pModifier )
{
	IFXRESULT result = IFX_OK;
	IFXDECLARELOCAL( IFXModifierChain, pModChain );
	IFXDECLARELOCAL( IFXSceneGraph, pSceneGraph );

	if( !m_bInit || !pModifier )
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT( result != IFX_OK );
	}

	if( IFXSUCCESS( result ) )
	{
		if( rModChainType == L"NODE" )
		{
			IFXDECLARELOCAL( IFXNode, pNode );

			if( IFXSUCCESS( result ) )
				result = FindNode( rModName, &pNode );

			// Get the pointer to the scenegraph from the IFXNode
			if( IFXSUCCESS( result ) )
			{
				result = pNode->GetSceneGraph( &pSceneGraph );

				if( IFXSUCCESS( result ) && !pSceneGraph )
					result = IFX_E_INVALID_POINTER;
			}

			// Get instance modifier chain
			if( IFXSUCCESS( result ) )
			{
				result = pNode->GetModifierChain( &pModChain );
			}
		}
		else if( rModChainType == L"MODEL" )
		{
			IFXDECLARELOCAL( IFXGenerator, pGenerator );

			result = FindModelResource( rModName, &pGenerator );

			// Get the pointer to the scenegraph from the IFXGenerator
			if( IFXSUCCESS( result ) )
			{
				result = pGenerator->GetSceneGraph( &pSceneGraph );

				if( IFXSUCCESS( result ) && !pSceneGraph )
					result = IFX_E_INVALID_POINTER;
			}

			// Get resource modifier chain
			if( IFXSUCCESS( result ) )
			{
				result = pGenerator->GetModifierChain( &pModChain );
			}
		}
		else
			result = IFX_E_UNSUPPORTED;
	}

	// Initialize the modifier
	if( IFXSUCCESS( result ) )
	{
		result = pModifier->SetSceneGraph( pSceneGraph );
	}

	// Add modifier to the end of the modifier chain associated with this
	// IFXModifier
	if( IFXSUCCESS( result ) )
	{
		result = pModChain->AddModifier( *pModifier );
	}

	return result;
}

/**
Attach an IFXAnimationModifier (which can produce both bones
and keyframe animation) to an IFXNode or IFXGenerator.

@param   const IFXString& rModName  The name of modifier.
@param   const IFXString& rModChainType  The type of modifier chain.
@param   BOOL  isBonesModifier  Should this modifier produce
                single-track animation (dragging
                the model transform around), or is it
                also transforming a bones system?
@param   IFXAnimationModifier** ppAnimationModifier  The returned modifier.

@return  IFXRESULT     Return status of this method.

@note    If a model both undergoes rigid-body animation and skeletal
    animation, the single-track IFXAnimationModifier must come @b after the
    multi-track IFXAnimationModifier in the model's modifier chain.

@note   The IFXAnimationModifier can be used to transform any type of node,
		not just those containing mesh data.
*/
IFXRESULT SceneUtilities::AddAnimationModifier( 
							const IFXString& rModName,
							const IFXString& rModChainType,
							BOOL isKeyFrameModifier,
							IFXAnimationModifier** ppAnimationModifier )
{
	IFXRESULT result = IFX_OK;
	IFXAnimationModifier* pAnimationModifier = NULL;

	if( !m_bInit || !ppAnimationModifier )
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT( IFXSUCCESS( result ) );
	}

	// Create an IFXAnimationModifier.
	if( IFXSUCCESS( result ) )
	{
		result = IFXCreateComponent(  
						CID_IFXAnimationModifier,
						IID_IFXAnimationModifier,
						(void**)&pAnimationModifier );
	}

	// Set the type of object we are going to create
	if( isKeyFrameModifier )
		pAnimationModifier->SetAsKeyframe();
	else
		pAnimationModifier->SetAsBones();

	if( IFXSUCCESS( result ) )
	{
		result = AddModifier( rModName, rModChainType, pAnimationModifier );
	}

	if (ppAnimationModifier && IFXSUCCESS(result))
		*ppAnimationModifier = pAnimationModifier;

	// Releasing pAnimationModifier is left to the user.

	return result;
}

IFXRESULT SceneUtilities::AssignAnimationToModifier(
                  IFXAnimationModifier* pAnimationModifier,
                  const IFXString& rMotionName,
				  F32 rTimeOffset, F32 rTimeScale,
				  BOOL loop, BOOL sync )
{
	IFXRESULT result = IFX_OK;

	if( !m_bInit || !pAnimationModifier )
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	if( IFXSUCCESS( result ) )
	{
		BOOL loop = TRUE;
		result = pAnimationModifier->Push( 
									rMotionName, &rTimeOffset, NULL, NULL, 
									&rTimeScale, &loop, &sync );

		// Cause the position of the model to reset when the
		// animation loops.
		U32 bits = ( IFXMixerQueue::CatLocation );
		pAnimationModifier->Concatenation() |= bits;
		pAnimationModifier->Concatenation() ^= bits;
	}

  return result;
}

/**
Attach IFXBoneWeightsModifier to the IFXNode or IFXGenerator.

@param   const IFXString& rModName  The name of modifier.
@param   const IFXString& rModChainType  The type of modifier chain.
@param     IFXBoneWeightsModifier** ppBoneWeightModifier The returned modifier.

@return    IFXRESULT         Return status of this method.
*/
IFXRESULT SceneUtilities::AddBoneWeightModifier(
							const IFXString& rModName,
							const IFXString& rModChainType,
                            IFXBoneWeightsModifier** ppBoneWeightModifier )
{
    IFXRESULT result = IFX_OK;
    IFXBoneWeightsModifier* pBoneWeightMod = NULL;

    if( !m_bInit || !ppBoneWeightModifier )
    {
        result = IFX_E_NOT_INITIALIZED;
        IFXASSERT(0);
    }

    // Create an IFXBoneWeightsModifier.
    if( IFXSUCCESS( result ) )
    {
        result = IFXCreateComponent( 
							CID_IFXBoneWeightsModifier,
                            IID_IFXBoneWeightsModifier,
                            (void**)&pBoneWeightMod );
    }

	if( IFXSUCCESS( result ) )
	{
		result = AddModifier( rModName, rModChainType, pBoneWeightMod );
	}

    if( IFXSUCCESS( result ) )
        *ppBoneWeightModifier = pBoneWeightMod;

    // Releasing pBoneWeightMod is left to the user.

   return result;
}

/**
Attach IFXShadingModifier to the IFXNode or IFXGenerator.

@param   const IFXString& rModName  The name of modifier.
@param   const IFXString& rModChainType  The type of modifier chain.
@param   IFXShadingModifier** ppShadingModifier The returned modifier.

@return  IFXRESULT         Return status of this method.
*/
IFXRESULT SceneUtilities::AddShadingModifier(
                  const IFXString& rModName,
				  const IFXString& rModChainType,
                  IFXShadingModifier** ppShadingModifier )
{
	IFXRESULT result = IFX_OK;
	IFXShadingModifier* pShadingModifier = NULL;

	if( !m_bInit || !ppShadingModifier )
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT( IFXSUCCESS( result ) );
	}

	// Create a Shading modifier
	if( IFXSUCCESS( result ) )
	{
		result = IFXCreateComponent( CID_IFXShadingModifier, IID_IFXShadingModifier,
					(void**)&pShadingModifier );
	}

	if( IFXSUCCESS( result ) )
	{
		result = AddModifier( rModName, rModChainType, pShadingModifier );
	}

	if( IFXSUCCESS( result ) )
	{
		// Releasing pShadingModifier is left to the caller
		*ppShadingModifier = pShadingModifier;
	}

	return result;
}


/**
Adds a Glyph2D modifier to the end of the modifier chain
associated with the specified IFXModel.

@param   const IFXString& rModName  The name of modifier.
@param   const IFXString& rModChainType  The type of modifier chain.
@param   F64 defaultWidth The default width of created glyphs
@param   F64 defaultSpacing The default spacing of created glyphs
@param   F64 defaultHeight The default height of created glyphs
@param   IFXGlyphCommandList* pCommandList list of commands for initializing
								glyph string
@param   IFXGlyph2DModifier** ppGlyph2DModifier The returned modifier

@return  IFXRESULT         Return status of this method.
*/
IFXRESULT SceneUtilities::AddGlyphModifier(
					const IFXString& rModName,
					const IFXString& rModChainType,
					F64 defaultWidth, F64 defaultSpacing, F64 defaultHeight,
					IFXGlyphCommandList* pCommandList,
					IFXGlyph2DModifier** ppGlyph2DModifier )
{
	IFXRESULT result = IFX_OK;
	IFXDECLARELOCAL( IFXSimpleList, pGlyphData );
	IFXGlyph2DModifier* pGlyph2DModifier = NULL;

	if( !m_bInit || !ppGlyph2DModifier || !pCommandList )
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT( IFXSUCCESS( result ) );
	}

	// Create a Glyph2D modifier
	if( IFXSUCCESS( result ) )
	{
		result = IFXCreateComponent( 
							CID_IFXGlyph2DModifier, 
							IID_IFXGlyph2DModifier,
							(void**)&pGlyph2DModifier );
	}

	if( IFXSUCCESS( result ) )
	{
		result = AddModifier( rModName, rModChainType, pGlyph2DModifier );
	}

	if( IFXSUCCESS( result ) )
	{
		result = pGlyph2DModifier->Initialize( defaultWidth, defaultSpacing, defaultHeight );
	}

	if( IFXSUCCESS( result ) )
	{
		result = pCommandList->GetList( &pGlyphData );
	}

	if( IFXSUCCESS( result ) )
	{
		result = pGlyph2DModifier->SetGlyphCommandList( pGlyphData );
	}

	if( IFXSUCCESS( result ) )
	{
		// Releasing pGlyph2DModifier is left to the caller
		*ppGlyph2DModifier = pGlyph2DModifier;
	}

	return result;
}


/**
Adds an IFXCLODModifier (node-specific LOD controls)
to the modifier param chain for the specified node or generator.

@param   const IFXString& rModName  The name of modifier.
@param   const IFXString& rModChainType  The type of modifier chain.
@param   IFXCLODModifier **ppCLODModifier    The returned modifier

@return  IFXRESULT         Return status of this method.
*/
IFXRESULT SceneUtilities::AddCLODModifier(
					const IFXString& rModName,
					const IFXString& rModChainType,
                    IFXCLODModifier** ppCLODModifier )
{
	IFXRESULT result = IFX_OK;
	IFXCLODModifier* pCLODModifier = NULL;

	if(!m_bInit || !ppCLODModifier )
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT( IFXSUCCESS(result) );
	}

	// Create an IFXBoneWeightsModifier.
	if( IFXSUCCESS( result ) )
	{
		result = IFXCreateComponent( 
							CID_IFXCLODModifier,
							IID_IFXCLODModifier,
							(void**)&pCLODModifier );
	}

	if( IFXSUCCESS( result ) )
	{
		result = AddModifier( rModName, rModChainType, pCLODModifier );
	}

	if( IFXSUCCESS( result ) )
		*ppCLODModifier = pCLODModifier;

	// Releasing ppBoneWeightMod is left to the user.

	return result;
}


/**
Adds a subdivision modifier to the end of the modifier chain
associated with the specified IFXModel node.

@param   const IFXString& rModName  The name of modifier.
@param   const IFXString& rModChainType  The type of modifier chain.
@param   IFXSubdivModifier** ppSubDiv The newly minted and
                    hooked-up subvision modifier

@return  IFXRESULT Return status of this method.
@note    Subdivision modifiers can only be added to the node chain
    of a node.  They cannot be added to the resource chain.
*/
IFXRESULT SceneUtilities::AddSubdivisionModifier(
					const IFXString& rModName,
					const IFXString& rModChainType,
					IFXSubdivModifier** ppSubDiv )
{
	IFXRESULT result = IFX_OK;
	IFXSubdivModifier* pSubDiv = NULL;

	if(!m_bInit || !ppSubDiv )
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT( IFXSUCCESS(result) );
	}

	if( IFXSUCCESS( result ) )
	{
		IFXDECLARELOCAL( IFXNode, pNode );

		if( IFXSUCCESS( result ) )
			result = FindNode( rModName, &pNode );

		// Don't allow double subdivision modifiers
		if( IFXSUCCESS(result) )
		{
			U32 found = 0;

			result = TestModifierNodeChain(pNode, IID_IFXSubdivModifier, &found);

			if(1 == found)
			{
				result = IFX_E_ABORTED;
			}
		}
	}

	// Create an IFXBoneWeightsModifier.
	if( IFXSUCCESS( result ) )
	{
		result = IFXCreateComponent(  
						CID_IFXSubdivModifier,
						IID_IFXSubdivModifier,
						(void**)&pSubDiv );
	}

	if( IFXSUCCESS( result ) )
	{
		result = AddModifier( rModName, rModChainType, pSubDiv );
	}

	if( ppSubDiv && IFXSUCCESS(result) )
	{
		// Releasing pSubDiv is left to the caller
		*ppSubDiv = pSubDiv;
	}

	return result;
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
