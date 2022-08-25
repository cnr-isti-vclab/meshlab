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
	@file  IFXNode.h                                                             
*/

#ifndef __IFXNODE_H__
#define __IFXNODE_H__

#include "IFXArray.h"
#include "IFXCollection.h"
#include "IFXMatrix4x4.h"
#include "IFXModifier.h"
#include "IFXSpatial.h"
#include "IFXSimpleList.h"


// {4517C9F3-B31E-11d3-94B1-00A0C9A0FBAE}
IFXDEFINE_GUID(IID_IFXNode,
			   0x4517c9f3,0xb31e,0x11d3,0x94,0xb1,0x0,0xa0,0xc9,0xa0,0xfb,0xae);

enum EIFXNodeCounterType
{
	IFX_NODE_COUNTER_MODELS = 0,
	IFX_NODE_COUNTER_MODELS_POLYGONS,
	IFX_NODE_COUNTER_LIGHTS,
	IFX_NODE_COUNTER_VIEWS,
	IFX_NODE_COUNTER_NODES,
	IFX_NODE_COUNTER_GROUPS
};

/**The interface that defines IFXNode.*/
class IFXNode : virtual public IFXModifier,
	virtual public IFXCollection
{
public:
	/// Transform hierarchy control
	virtual IFXRESULT IFXAPI ApplyTransformToNode( IFXMatrix4x4* pPinF, U32 parent ) = 0;
	/// Transform hierarchy control
	virtual const IFXMatrix4x4& IFXAPI GetMatrix(U32 index) const = 0;
	/// Transform hierarchy control
	virtual IFXRESULT IFXAPI SetMatrix( U32 index, IFXMatrix4x4* pMatrix )=0;

	/// Transform hierarchy control
	virtual IFXRESULT IFXAPI InvalidateMatrix()=0;

	/// Returns the current world matrix from the node's dataPacket.
	virtual IFXRESULT IFXAPI GetWorldMatrix(U32 Index, IFXMatrix4x4** ppWorldMatrix )=0;
	virtual IFXRESULT IFXAPI GetWorldMatrices( IFXArray<IFXMatrix4x4>** ppWorldMatrices)=0;

	/// Connectivity
	virtual IFXRESULT IFXAPI Prune()=0;

	virtual U32 IFXAPI GetNumberOfParents() = 0;
	virtual IFXRESULT IFXAPI GetNumberOfInstances(U32* numInstances) = 0;
	virtual IFXNode* IFXAPI GetParentNR(const U32 parentIndex) = 0;
	virtual IFXRESULT IFXAPI AddParent( IFXNode* pNode )=0;
	virtual IFXRESULT IFXAPI RemoveParent(const U32 index)=0;

	/// Subgraph lists.
	virtual IFXRESULT IFXAPI GetCollection( 
							IFXREFCID rInCID, IFXCollection** ppOutCollection )=0;

	/// Debug mode control
	virtual U32       IFXAPI GetDebugFlags()=0;
	/// Debug mode control
	virtual void      IFXAPI SetDebugFlags( U32 inDebugFlags )=0;

	/// Bounds
	virtual IFXRESULT IFXAPI GetSubgraphBound( IFXVector4& rSphere, U32 rootInstance )=0;

	/// Statistics
	virtual void      IFXAPI Counter( EIFXNodeCounterType type, U32* puOutCount )=0;

	/// Picking
	virtual IFXRESULT IFXAPI Pick( 
						U32 myInstance, IFXVector3& position, IFXVector3& direction, 
						IFXSimpleList** ppPickedObjectList )=0;
	
	/// Traversal
	virtual U32 IFXAPI GetNumberOfChildren( const BOOL bInDeepCount = FALSE )=0;
	virtual IFXNode*  IFXAPI GetChildNR(const U32 childIndex) = 0;
	virtual IFXRESULT IFXAPI AddChild(IFXNode* pChildNode) = 0;
	virtual IFXRESULT IFXAPI RemoveChild(IFXNode* pChildNode) = 0;
	virtual IFXRESULT IFXAPI RemoveChild(const U32 index) = 0;

	/// Motion Marking
	virtual IFXRESULT IFXAPI MarkMotions()=0;
};

#endif
