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
	@file  IFXSubdivisionManagerInterface.h

			The header file that defines the IFXSubdivisionManagerInterface interface.


  IFXSubdivisionManager contains all the methods necessary for creating and 
manipulating a subdivision surface.  The manager is basically a "mesh
in-mesh out" geometry-based interface. 

@note

  API users will first instantiate an IFXSubdivisionManager 
object. Next, a control or base mesh is passed to the manager with the
InitMesh() method.  Then, desired properties (such as 
MaxSubdivisionDepth) are set on the subdivision manager object. A subdivided
mesh is obtained with the UpdateMesh() method.

  Both uniform and adaptive subdivision is provided. The quality
and performance of adaptive subdivision depends heavily on the user defined
per-triangle adaptive metric provided with SetAdaptiveMetric(). A sample
screen space is provided.


@verbatim

Boolean Properties:

These are Boolean properties that can be set. True equates enable, false equates disable.

-Property____			____Action____					 __Default__

Adaptive				Enable/Disable adaptive subdivision		false
CrackFilling			Enable/Disable adaptive crackfilling	true
NormalSubdivision		Enable/Disable subdivision of normals	true
TexCoordSubdivision		Enable/Disable subdivision of texcorod	true
LazyEvaluation			Enable/Disable adaptive lazy evaluation	false

Integer Properties:

____Property____			____Action____				 _Default_	_Range_

MaxComputeDepth				Restricts subdivsion depth			0		[0..9]
MaxRenderDepth				Restricts depth of output mesh		0		[0..9]
CurrentComputeDepth			Current subdivision depth			0		[0..9]
MaxTriangleAllocation		Upper limit subdivision triangles	400000  [0..maxint]
MaxVertexAllocation			Upper limit subdivision vertices	200000  [0..maxint]
InitialTriangleAllocation	Initial free list allocation		5000	[0..maxint]
InitialVertexAllocation		Initial free list allocation		2500    [0..maxint]
GrowTriangleAllocation		Amount by which tri freelist grows	1000	[0..maxint]
GrowVertexAllocation		Amount by which vertfreelist grows	500		[0..maxint]
LazyEvaluationLimit			# of allowable subdivs per Update	2		[0..maxint]

Floating Point Properties:

____Property____			____Action____				 _Default_	_Range_
  SurfaceTension		Scaled tension control parameter     0.65		 [0.0..1.0]
@endverbatim
*/

#ifndef IFXSUBDIVISIONMANAGERINTERFACE_DOT_H
#define IFXSUBDIVISIONMANAGERINTERFACE_DOT_H

#include "IFXMeshGroup.h"
#include "IFXNeighborMesh.h"

class IFXAdaptiveMetric;

// IFXRESULT error codes, specific to IFXSubdivisionManager:
// Bad mesh data:
#define IFX_E_SUBDIVMGR_NON_MANIFOLD_TOPOLOGY	    MAKE_IFXRESULT_FAIL( IFXRESULT_SUBDIV, 0x0000 )
#define IFX_E_SUBDIVMGR_LARGE_MESHES_UNSUPPORTED	MAKE_IFXRESULT_FAIL( IFXRESULT_SUBDIV, 0x0001 )
#define IFX_E_SUBDIVMGR_NEIGHBOR_MESH_CREATION	    MAKE_IFXRESULT_FAIL( IFXRESULT_SUBDIV, 0x0002 )

/**
 IFXSubdivisionManager contains all the methods necessary for creating and 
manipulating a subdivision surface.  The manager is basically a "mesh
in-mesh out" geometry-based interface. 

@note

  API users will first instantiate an IFXSubdivisionManager 
object. Next, a control or base mesh is passed to the manager with the
InitMesh() method.  Then, desired properties (such as 
MaxSubdivisionDepth) are set on the subdivision manager object. A subdivided
mesh is obtained with the UpdateMesh() method.

  Both uniform and adaptive subdivision is provided. The quality
and performance of adaptive subdivision depends heavily on the user defined
per-triangle adaptive metric provided with SetAdaptiveMetric(). A sample
screen space is provided.


@verbatim

Boolean Properties:

These are Boolean properties that can be set. True equates enable, false equates disable.

-Property____			____Action____					 __Default__

Adaptive				Enable/Disable adaptive subdivision		false
CrackFilling			Enable/Disable adaptive crackfilling	true
NormalSubdivision		Enable/Disable subdivision of normals	true
TexCoordSubdivision		Enable/Disable subdivision of texcorod	true
LazyEvaluation			Enable/Disable adaptive lazy evaluation	false

Integer Properties:

____Property____			____Action____				 _Default_	_Range_

MaxComputeDepth				Restricts subdivsion depth			0		[0..9]
MaxRenderDepth				Restricts depth of output mesh		0		[0..9]
CurrentComputeDepth			Current subdivision depth			0		[0..9]
MaxTriangleAllocation		Upper limit subdivision triangles	400000  [0..maxint]
MaxVertexAllocation			Upper limit subdivision vertices	200000  [0..maxint]
InitialTriangleAllocation	Initial free list allocation		5000	[0..maxint]
InitialVertexAllocation		Initial free list allocation		2500    [0..maxint]
GrowTriangleAllocation		Amount by which tri freelist grows	1000	[0..maxint]
GrowVertexAllocation		Amount by which vertfreelist grows	500		[0..maxint]
LazyEvaluationLimit			# of allowable subdivs per Update	2		[0..maxint]

Floating Point Properties:

____Property____			____Action____				 _Default_	_Range_
  SurfaceTension		Scaled tension control parameter     0.65		 [0.0..1.0]
@endverbatim
*/


class IFXSubdivisionManagerInterface
{

public:
    virtual ~IFXSubdivisionManagerInterface() {}
    /** 
		BooleanProperty enumerator \n

		- @b Adaptive
		- @b CrackFilling
		- @b NormalSubdivision
		- @b TexCoordSubdivision 
		- @b LazyEvaluation	
	*/
    enum BooleanProperty    {Adaptive, CrackFilling, NormalSubdivision,
							 TexCoordSubdivision, LazyEvaluation,
	                         MAX_NUM_BOOLEAN_PROPERTIES};
    /** 
		IntegerProperty enumerator

	- @b MaxComputeDepth
	- @b MaxRenderDepth 
	- @b CurrentComputeDepth
	- @b MaxTriangleAllocation
	- @b MaxVertexAllocation
	- @b InitialTriangleAllocation
	- @b InitialVertexAllocation
	- @b GrowTriangleAllocation
	- @b GrowVertexAllocation
    - @b LazyEvaluationLimit
	*/
	enum IntegerProperty    {MaxComputeDepth, MaxRenderDepth, CurrentComputeDepth,
                             MaxTriangleAllocation, MaxVertexAllocation, 
                             InitialTriangleAllocation, InitialVertexAllocation, 
                             GrowTriangleAllocation, GrowVertexAllocation,
                             LazyEvaluationLimit, MAX_NUM_INTEGER_PROPERTIES};
    /** 
		FloatProperty enumerator
	
		- @b SurfaceTension
	*/
	enum FloatProperty      {SurfaceTension,
	                         MAX_NUM_FLOAT_PROPERTIES};

    //              --- Property accessor methods ---

	/** 
		This method sets a Boolean property (true/false).

		@param	property	A BooleanProperty enumeration denoting which property to set.
		@param	bValue		The new vaule of the property.\n

  		@return	IFXRESULT\n

		- @b IFX_OK				-	No error.
		- @b IFX_E_UNDEFINED	-	Property value is undefined.
	*/
    virtual IFXRESULT IFXAPI  SetBool 	    (BooleanProperty property, 	BOOL bValue) = 0;

	/** 
		This method returns the value of the specified Boolean Property (true/false).

		@param	property	A BooleanProperty enumeration denoting which property to set.
		@param	pbValue		A BOOL pointer to the value of the property.\n

  		@return	IFXRESULT\n

		- @b IFX_OK				-	No error.
		- @b IFX_E_UNDEFINED	-	Property value is undefined.
	*/
	virtual IFXRESULT IFXAPI  GetBool 	    (BooleanProperty property, 	BOOL *pbValue) = 0;

	/** 
		This method sets the value of the specified integer property (0...maxint).

		@param	property	A IntegerProperty enumeration denoting which property to set.
		@param	uValue		A U32 denoting the value of the property.\n

  		@return	IFXRESULT\n

		- @b IFX_OK				-	No error.
		- @b IFX_E_UNDEFINED	-	Property value is undefined.
	*/
  	virtual IFXRESULT IFXAPI  SetInteger    (IntegerProperty property, 	U32 uValue) = 0;

	/** 
		This method returns the value of the specified integer property (0...maxint).

		@param	property	A IntegerProperty enumeration denoting which property to set.
		@param	puValue		A U32 pointer to the value of the property.\n

  		@return	IFXRESULT\n

		- @b IFX_OK				-	No error.
		- @b IFX_E_UNDEFINED	-	Property value is undefined.
	*/
    virtual IFXRESULT IFXAPI  GetInteger    (IntegerProperty property, 	U32 *puValue) = 0;
	
	/** 
		This method sets the value of the specified float property.

		@param	property	A FloatProperty enumeration denoting which property to set.
		@param	fValue		A F32 denoting the value of the property.\n

  		@return	IFXRESULT\n

		- @b IFX_OK				-	No error.
		- @b IFX_E_UNDEFINED	-	Property value is undefined.
	*/
     virtual IFXRESULT IFXAPI  SetFloat      (FloatProperty property, 	F32 fValue) = 0;
	
	/** 
		This method returns the value of the specified float property.

		@param	property	A FloatProperty enumeration denoting which property to set.
		@param	pfValue		A F32 pointer denoting the value of the property.\n

  		@return	IFXRESULT\n

		- @b IFX_OK				-	No error.
		- @b IFX_E_UNDEFINED	-	Property value is undefined.
	*/
    virtual IFXRESULT IFXAPI  GetFloat      (FloatProperty property, 	F32 *pfValue) = 0;

    
	//              --- Subdivision interface methods ---

	/**
		This method provides an interface to a per triangle metric. This metric is
		evaluated to determine adaptive subdivision. IFXSreenSpaceMetric is
		provided an example.
		
		@param	pInterface	An IFXAdaptiveMetric pointer.

  		@return	IFXRESULT\n

		- @b IFX_OK				-	No error.
		- @b IFX_E_UNDEFINED	-	pInterfacew value is undefined.
	*/
	virtual IFXRESULT IFXAPI  SetAdaptiveMetric (IFXAdaptiveMetric *pInterface) = 0;

	/**
		This method returns a pointer to the current adaptive metric.
		
		@param	pInterface	An IFXAdaptiveMetric pointer to the current metric.

  		@return	IFXRESULT\n

		- @b IFX_OK				-	No error.
		- @b IFX_E_UNDEFINED	-	ppMetric value is undefined.
	*/
    virtual IFXRESULT IFXAPI  GetAdaptiveMetric (IFXAdaptiveMetric **ppMetric) = 0;
    
	/** 
		This method initializes the subdivision manager with pMeshGrp and 
		allocates memory for computation and for the output mesh group.

		@param pMeshGrp			An IFXMeshGroup pointer to the new meshGroup.
		@param pNeighborMesh	An IFXNeighborMesh pointer to the new neighborMesh.
  
  		@return	IFXRESULT\n

		- @b IFX_OK										-	No error.
		- @b IFX_E_INVALID_POINTER						-	Invalid pointer passed in.
		- @b IFX_E_SUBDIVMGR_LARGE_MESHES_UNSUPPORTED	-	A mesh in the meshgroup is 
															too large to subdivide.
		- @b IFX_E_OUT_OF_MEMORY						-	Memory allocation failure.
	*/
	virtual IFXRESULT IFXAPI  InitMesh 			(IFXMeshGroup 	*pMeshGrp,
	                                     IFXNeighborMesh* pNeighborMesh ) = 0;

	/** 
		Executes the current subdivision properties and collect the output mesh.

		@note	pInMeshGrp is here for consistency with other tech mgrs and is
				ignored by IFXSubdivisionManager::UpdateMesh()

		@param pOutMeshGrp	An IFXMeshGroup pointer to the updated meshGroup.
		@param pUpdated		A BOOL pointer indicating if the meshGroup was updated.
  
  		@return	IFXRESULT\n

		- @b IFX_OK					-	No error.
		- @b IFX_E_INVALID_POINTER	-	Invalid pointer passed in.
		- @b IFX_E_OUT_OF_MEMORY	-	Memory allocation failure.
	*/
	virtual IFXRESULT IFXAPI  UpdateMesh(IFXMeshGroup **pOutMeshGrp, BOOL *pUpdated) = 0;

	//              --- Cleanup methods ---

	/** 
		This method forces consolidation of an entire subdivision level.
	
		@param	uLevel A U32 denoting the level to consolidate.

  		@return	IFXRESULT\n

		- @b IFX_OK	-	No error.
	*/
    virtual IFXRESULT IFXAPI  ConsolidateLevel (U32 uLevel) = 0;

	/**
		The method resets all subdivision computations.
	
  		@return	IFXRESULT\n

		- @b IFX_OK	-	No error.
	*/
    virtual IFXRESULT IFXAPI  ResetAll() = 0;
};

#endif
