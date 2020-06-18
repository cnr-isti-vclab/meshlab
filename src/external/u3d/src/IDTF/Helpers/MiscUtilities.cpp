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
@file MiscUtilities.cpp

This module defines ...
*/


//***************************************************************************
//  Includes
//***************************************************************************

#include "SceneUtilities.h"
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


//***************************************************************************
//  Protected methods
//***************************************************************************


//***************************************************************************
//  Private methods
//***************************************************************************


//***************************************************************************
//  Global functions
//***************************************************************************


//-------------------------------------------------------------------------------------------------
/**
@brief   Remove duplicate keyframes from the IFXMotionResource.

@param   IFXMotionResource*  in_MotionResource The motion resource to compress.

@return  IFXRESULT       Return status of this method.
**/
//-------------------------------------------------------------------------------------------------

IFXRESULT SceneUtilities::CompressMotionTracks(IFXMotionResource* in_MotionResource)
{
	IFXRESULT iResult = IFX_OK;

	if(!m_bInit || !in_MotionResource)
	{
		iResult = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	// Call the method which does the trimming of the motion track.
	if ( in_MotionResource && IFXSUCCESS(iResult) )
	{
		const F32 deltatime=0.01f;
		const F32 deltadisplacement=0.01f;
		const F32 deltarotation=0.001f;

		iResult = in_MotionResource->FilterAndCompress(deltatime,
			deltadisplacement, deltarotation);
	}

	return iResult;
}


//-------------------------------------------------------------------------------------------------
/**
@brief   Search the IFXAuthorCLODResource's modifier chain for the specified modifier, and
return whether or not we were able to find it.

@param   IFXAuthorCLODResource * in_pCLODResource    The IFXAuthorCLODResource whose modifier chain we should
search for the specified modifier.
@param   IFXREFIID in_ModifierID       The ID of the modifier component to search for.
@param   U32 out_Found           Whether or not we found the specified modifier

@return  IFXRESULT         Return status of this method.
**/
//-------------------------------------------------------------------------------------------------
IFXRESULT SceneUtilities::TestModifierResourceChain(  IFXAuthorCLODResource * in_pCLODResource,
													IFXREFIID in_ModifierID,
													U32 *out_pFound)
{
	IFXRESULT     iResult = IFX_OK;

	IFXModifier   *pModifier = NULL;
	IFXModifierChain  *pModChain = NULL;
	IFXSubdivModifier *pSubdiv = NULL;
	U32         ModifierCount = 0;
	U32         CurrentModifier = 0;
	U32         found = 0;

	if (!m_bInit || !in_pCLODResource || !out_pFound )
	{
		iResult = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	// Get access to the resource modifier chain
	iResult = in_pCLODResource->GetModifierChain(&pModChain);

	// Find out how many modifiers/generators it contains
	if (IFXSUCCESS(iResult))
		iResult = pModChain->GetModifierCount(ModifierCount);

	// Get the first modifier after the IFXAuthorCLODResource
	if (ModifierCount > 1)
		CurrentModifier = 1; // Skip the pCLODResource in the modifier chain

	// Now iterate through all the modifiers looking for the one of interest
	if ( IFXSUCCESS(iResult) )
	{
		for (; CurrentModifier < ModifierCount; CurrentModifier++)
		{
			if (IFXSUCCESS(iResult))
				iResult =  pModChain->GetModifier(CurrentModifier, pModifier);

			if (pModifier && IFXSUCCESS(iResult) )
			{
				if (IID_IFXSubdivModifier == in_ModifierID)
				{
					if ( IFX_OK == pModifier->QueryInterface( IID_IFXSubdivModifier,
						(void**)&pSubdiv ) )
					{
						IFXRELEASE(pSubdiv);
						found = 1;
					}
				}
			}  // if pModifier

			IFXRELEASE(pModifier);
		}
	}

	IFXRELEASE(pModChain);

	if (out_pFound)
	{
		if (0 == found)
			*out_pFound = 0;
		else
			*out_pFound = 1;
	}

	return iResult;
}


//-------------------------------------------------------------------------------------------------
/**
@brief   Search the IFXNode's modifier chain for the specified modifier, and
return whether or not we were able to find it.

@param   IFXNode * in_pNode      The IFXNode whose modifier chain we should
search for the specified modifier.
@param   IFXREFIID in_ModifierID   The ID of the modifier component to search for.
@param   U32 out_Found       Whether or not we found the specified modifier

@return  IFXRESULT         Return status of this method.
**/
//-------------------------------------------------------------------------------------------------
IFXRESULT SceneUtilities::TestModifierNodeChain(  IFXNode * in_pNode,
												IFXREFIID in_ModifierID,
												U32 *out_pFound)
{
	IFXRESULT     iResult = IFX_OK;

	IFXModifier   *pModifier = NULL;
	IFXModifierChain  *pModChain = NULL;
	IFXAnimationModifier  *pAnimation = NULL;
	IFXSubdivModifier *pSubdiv = NULL;
	U32         ModifierCount = 0;
	U32         CurrentModifier = 0;
	U32         found = 0;

	if (!m_bInit || !in_pNode || !out_pFound )
	{
		iResult = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	// Get access to the resource modifier chain
	iResult = in_pNode->GetModifierChain(&pModChain);

	// Find out how many modifiers/generators it contains
	if (IFXSUCCESS(iResult))
		iResult = pModChain->GetModifierCount(ModifierCount);

	// Get the first modifier after the IFXAuthorCLODResource
	if (ModifierCount > 1)
		CurrentModifier = 1; // Skip the pCLODResource in the modifier chain

	// Now iterate through all the modifiers looking for the one of interest
	if ( IFXSUCCESS(iResult) )
	{
		for (; CurrentModifier < ModifierCount; CurrentModifier++)
		{
			if (IFXSUCCESS(iResult))
				iResult =  pModChain->GetModifier(CurrentModifier, pModifier);

			if (pModifier && IFXSUCCESS(iResult) )
			{
				if (IID_IFXAnimationModifier == in_ModifierID)
				{
					if ( IFX_OK == pModifier->QueryInterface( IID_IFXAnimationModifier,
						(void**)&pAnimation ) )
					{
						IFXRELEASE(pAnimation);
						found = 1;
					}
				}

				if (IID_IFXSubdivModifier == in_ModifierID)
				{
					if ( IFX_OK == pModifier->QueryInterface( IID_IFXSubdivModifier,
						(void**)&pSubdiv ) )
					{
						IFXRELEASE(pSubdiv);
						found = 1;
					}
				}
			}  // if pModifier

			IFXRELEASE(pModifier);
		}
	}

	IFXRELEASE(pModChain);

	if (out_pFound)
	{
		if (0 == found)
			*out_pFound = 0;
		else
			*out_pFound = 1;
	}

	return iResult;
}


//-------------------------------------------------------------------------------------------------
/**
@brief   Gets a node's world transform by evaluating its modifier chain.

@param   IFXNode *in_pNode         The node whose world transform we need
@param   IFXMatrix4x4 *out_pWorldTransform The returned transform

@return  IFXRESULT         Return status of this method.
**/
//-------------------------------------------------------------------------------------------------
IFXRESULT SceneUtilities::GetWorldTransform(IFXNode *in_pNode,
											IFXMatrix4x4 *out_pWorldTransform)
{
	IFXRESULT       iResult = IFX_OK;
	IFXModifierChain    *pModifierChain = NULL;
	IFXModifierDataPacket *pMDP = NULL;
	IFXMatrix4x4      *pTransformMatrix = NULL;
	U32           uTransformDataElementIndex = 0;

	if (!m_bInit || !in_pNode || !out_pWorldTransform )
	{
		iResult = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	if (in_pNode && IFXSUCCESS(iResult))
	{
		iResult = in_pNode->GetModifierChain( &pModifierChain );
	}

	if (pModifierChain && IFXSUCCESS(iResult))
	{
		iResult = pModifierChain->GetDataPacket( pMDP );
	}

	if (pMDP  && IFXSUCCESS(iResult))
	{
		iResult = pMDP->GetDataElementIndex( DID_IFXTransform,
			uTransformDataElementIndex );
	}

	if (pMDP  && IFXSUCCESS(iResult))
	{
		iResult = pMDP->GetDataElement( uTransformDataElementIndex,
			(void**)&pTransformMatrix );   // Does not AddRef
	}

	if (pTransformMatrix  && IFXSUCCESS(iResult))
	{
		out_pWorldTransform = pTransformMatrix;
	}

	return iResult;
}


//***************************************************************************
//  Local functions
//***************************************************************************
