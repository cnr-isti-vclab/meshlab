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
	@file	IFXCLODModifier.h

			The header file that defines the IFXCLODModifier interface.
*/

#ifndef IFXCLODMODIFIER_H
#define IFXCLODMODIFIER_H

#include "IFXUnknown.h"
#include "IFXModifier.h"


/// The interface ID (IFXIID) for the IFXCLODModifier interface.
// {CEE2BE13-466D-426e-91A3-D6CC794921F8}
IFXDEFINE_GUID(IID_IFXCLODModifier,
0xcee2be13, 0x466d, 0x426e, 0x91, 0xa3, 0xd6, 0xcc, 0x79, 0x49, 0x21, 0xf8);


/**
 *	The IFXCLODModifier interface exposes a set of methods to configure
 *	the CLOD controller that determines the geometric resolution of a model
 *	according to the following function:
 * @verbatim
 *	if ( pFrustrum->GetProjectionMode() == IFX_FRUSTUM_PERSPECTIVE_PROJECTION )
 *	{
 *		F32 ProjectionDegrees = pFrustrum->GetFOV();
 *		fNewRes = (pBounds->Bound().Radius() * ViewportHeight * g_fCLODMultiplier
 *				  * m_fCLODScreenSpaceBias * LOD_PERSPECTIVE_SCALE_FACTOR)
 *				  / (sqrtf(viewToModel.DotProduct(viewToModel)) * ProjectionDegrees);
 *	}
 *	else // IFX_FRUSTUM_ORTHOGRAPHIC_PROJECTION
 *	{
 *		F32 OrthoHeight = pFrustrum->GetPlane(IFX_FRUSTUM_TOP).position.
 *				CalcDistanceFrom(pFrustrum->GetPlane(IFX_FRUSTUM_BOTTOM).position);
 *		fNewRes = (pBounds->Bound().Radius() * ViewportHeight * g_fCLODMultiplier
 *				  * m_fCLODScreenSpaceBias * LOD_ORTHO_SCALE_FACTOR / OrthoHeight);
 *	}
 * @endverbatim
 */
class IFXCLODModifier : virtual public IFXModifier
{
public:
	/**
	 *	Retrieves the state of the CLOD screen space controller.
	 *
	 *	@param	pbOutOn	The address of a BOOL to contain the value.
	 *
	 *	@return	An IFXRESULT value.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_INVALID_POINTER	The @a pbOutOn value was 0.
	 */
	virtual IFXRESULT IFXAPI  GetCLODScreenSpaceControllerState(BOOL* pbOutOn)=0;

	/**
	 *	Sets the state of the CLOD screen space controller.
	 *
	 *	@param	bInOn	A BOOL used to enable use of the CLOD screen space
	 *	                controller.
	 *
	 *	@return	An IFXRESULT value.
	 *
	 *	@retval	IFX_OK	No error.
	 */
	virtual IFXRESULT IFXAPI  SetCLODScreenSpaceControllerState(BOOL bInOn)=0;

	/**
	 *	Retrieves the CLOD Bias value used in the metric.  The Bias is used to
	 *	compensate for non-uniform tessellation.
	 *
	 *	@param	pfOutCLODScreenSpaceBias	The address of an F32 to contain the CLOD Bias.
	 *
	 *	@return	An IFXRESULT value.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_INVALID_POINTER	The @a pfOutCLODScreenSpaceBias value was 0.
	 */
	virtual IFXRESULT IFXAPI  GetLODBias(F32* pfOutCLODScreenSpaceBias)=0;

	/**
	 *	Sets the CLOD Bias value.
	 *
	 *	@param	fInCLODScreenSpaceBias	An F32 value to be used as the CLOD Bias value.
	 *
	 *	@return	An IFXRESULT value.
	 *
	 *	@retval	IFX_OK	No error.
	 */
	virtual IFXRESULT IFXAPI  SetLODBias(F32 fInCLODScreenSpaceBias)=0;

	/**
	 *	Retrieves the CLOD level as a ratio of the maximum number of CLOD updates.
	 *
	 *	@param	pfOutCLODLevel	A pointer to an F32 to contain the CLOD level.
	 *
	 *	@return	An IFXRESULT value.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_INVALID_POINTER	The @a pfOutCLODLevel value was 0.
	 */
	virtual IFXRESULT IFXAPI  GetCLODLevel(F32* pfOutCLODLevel)=0;

	/**
	 *	Sets the CLOD level as a ratio of the maximum number of CLOD updates.
	 *
	 *	@param	fInCLODLevel	An F32 value to be used as the CLOD level.
	 *
	 *	@return	An IFXRESULT value.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_INVALID_RANGE	The @a fInCLODLevel value was not between 0 and 1.
	 */
	virtual IFXRESULT IFXAPI  SetCLODLevel(F32 fInCLODLevel)=0;
};


#endif
