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
	@file	IFXScreenSpaceMetricInterface.h

			The header file that defines the IFXScreenSpaceMetricInterface interface.
*/

#ifndef IFXSREENSPACEMETRICINTERFACE_DOT_H
#define IFXSREENSPACEMETRICINTERFACE_DOT_H

#include "IFXAdaptiveMetric.h"


class IFXTQTTriangle;

/**
	This is the main interface for IFXScreenSpaceMetricInterface.
*/
class IFXScreenSpaceMetricInterface : public IFXAdaptiveMetric
{

/**
	IFXScreenSpaceMetric implements Hoppe's Screen Space Metric (SIGGRAPH 1997). 
    Modified for subdivision triangles.
*/
public:
	virtual ~IFXScreenSpaceMetricInterface() {}

 	/**
		This method evaluates the triangle and determines which action to take.

		@param	pTriangle	A pointer to an IFXTQTTriangle object.
		@param	pAction		A pointer to the Action to take for the triangle.

		- @b Subdivide 
		- @b Consolidate 
		- @b Sustain 

		@return void
	*/
	virtual void EvaluateTriangle (IFXTQTTriangle *pTriangle, Action *pAction) = 0;

 	/**
		This method updates the cameras viewpoint.

		@param	pCameraX		An IFXVector3 reference to the X direction of the camera.
		@param	pCameraY		An IFXVector3 reference to the Y direction of the camera.
		@param	pCameraZ		An IFXVector3 reference to the Z direction of the camera.
		@param	pCameraPos		An IFXVector3 reference to the position of the camera.
		@param	fAspect			A F32 denoting the cameras aspect ratio.
		@param	fFieldOfView	A F32 denoting the cameras field of view.

		@return void
	*/
     virtual void IFXAPI  UpdateViewpoint (IFXVector3 &pCameraX, IFXVector3 &pCameraY,
								  IFXVector3 &pCameraZ, IFXVector3 &pCameraPos, 
								  F32 fAspect, F32 fFieldOfView) = 0;

	/**
		This method returns the pixel tolerance ratio.

		@return A F32 denoting the pixel tolerance ratio. 
	*/
	virtual F32 IFXAPI  GetPixelTolerance() = 0;

	/**
		This method sets the pixel tolerance ratio.

		@param A F32 denoting the pixel tolerance ratio.

		@return void
	*/
	virtual void IFXAPI  SetPixelTolerance(F32 factor) = 0;

	/**
		This method returns the cameras field of view.

		@return A F32 denoting the cameras field of view. 
	*/
	virtual F32 IFXAPI  GetFieldofView() = 0;

	/**
		This method sets the cameras field of view.

		@param A F32 denoting the cameras field of view.

		@return void
	*/
	virtual void IFXAPI  SetFieldofView(F32 degrees) = 0;

	/**
		This method returns a boolean indicating whether or not the 
		viewpoint is locked.

		@return BOOL
		
   		- @b TRUE	-	The viewpoint is locked.\n 
		- @b FALSE	-	The viewpoint is not locked.\n 
	*/
	virtual BOOL IFXAPI  GetViewpointLock() = 0;

	/**
		This method sets the boolean indicating whether or not to lock 
		viewpoint.

		@param BOOL
		
   		- @b TRUE	-	Lock the viewpoint.\n 
		- @b FALSE	-	Do not lock the viewpoint.\n 
	*/
	virtual void IFXAPI  SetViewpointLock(BOOL state) = 0;
};

#endif
