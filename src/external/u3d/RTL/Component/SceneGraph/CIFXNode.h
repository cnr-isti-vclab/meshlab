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
	@file  CIFXNode.h 
*/

#ifndef __CIFXNODE_H__
#define __CIFXNODE_H__

#include "IFXNode.h"
#include "CIFXModifier.h"

#include "IFXArray.h" 
#include "IFXAutoRelease.h"

class CIFXNode :	protected CIFXModifier,
			virtual public    IFXNode,
			virtual public    IFXObserver
{
public:
	/// Identification
	virtual const IFXGUID& GetCID(){return m_pCollections[0]->GetCID();}
	// IFXMarker
	IFXRESULT   IFXAPI	SetSceneGraph( IFXSceneGraph* pInSceneGraph );
	IFXRESULT   IFXAPI 	Mark( void ) ;
	void		IFXAPI  SetPriority( U32  uInPriority,
									BOOL bRecursive,
									BOOL bPromotionOnly );

	// IFXObserver
	virtual IFXRESULT  IFXAPI  Update( 
						IFXSubject* pInSubject, U32 inChangeBits,IFXREFIID rIType );

	// IFXCollection
	// The Node implementation of the IFXCollection interface acts as a facad
	// exposing a set of collections as one.  It also propagates calls to
	// Add/RemoveSpatials() to parents recursivly.
	IFXRESULT  IFXAPI 	 InitializeCollection( 
					IFXSpatial**      pInSpatials = NULL,
					U32               inNumberOfSpatials = 0,
					IFXSpatial::eType eInType = IFXSpatial::UNSPECIFIED );

	IFXRESULT  IFXAPI 	 InitializeCollection( IFXCollection*    pInCollection);

	IFXRESULT  IFXAPI 	 AddSpatials(          
					IFXSpatial**      pInSpatials,
					U32               inNumberOfSpatials,
					IFXSpatial::eType eInType = IFXSpatial::UNSPECIFIED );

	IFXRESULT  IFXAPI 	 RemoveSpatials(       
					IFXSpatial**	  pInSpatials,
					U32               inNumberOfSpatials,
					IFXSpatial::eType eInType = IFXSpatial::UNSPECIFIED );

	IFXRESULT  IFXAPI 	 GetSpatials(          
					IFXSpatial**&     rpOutSpatials,
					U32&              rOutNumberOfSpatials,
					IFXSpatial::eType eInType );
  
	IFXRESULT IFXAPI  Notify ( IFXModifierMessage eInMessage,
					void*	pMessageContext );



	// IFXNode
	IFXRESULT  IFXAPI 	 ApplyTransformToNode( IFXMatrix4x4* pPinF, U32 parent );
	IFXRESULT  IFXAPI 	 GetCollection( IFXREFCID rInCID, IFXCollection** ppOutCollection );
	U32     IFXAPI     GetDebugFlags( void ) { return m_debugFlags; };
	void     IFXAPI    SetDebugFlags( U32 uIn ) { m_debugFlags = uIn; };
	IFXRESULT  IFXAPI 	 GetSubgraphBound( IFXVector4& rSphere, U32 rootInstance );
	
	const IFXMatrix4x4& IFXAPI GetMatrix(U32 index) const;
	
	U32 IFXAPI   GetNumberOfChildren(const BOOL bInDeepCount = FALSE);
	IFXNode*  IFXAPI GetChildNR(const U32 childIndex);
	IFXRESULT  IFXAPI 	 AddChild(IFXNode* pChildNode);
	IFXRESULT  IFXAPI 	 RemoveChild(IFXNode* pChildNode);
	IFXRESULT  IFXAPI 	 RemoveChild(const U32 index);

	IFXRESULT  IFXAPI 	 GetWorldMatrix(U32 Index, IFXMatrix4x4** ppWorldMatrix );
	IFXRESULT  IFXAPI 	 GetWorldMatrices( IFXArray<IFXMatrix4x4>** ppWorldMatrices);

	IFXRESULT  IFXAPI 	 MarkMotions();

	IFXRESULT  IFXAPI 	 Pick(
				U32 myInstance, IFXVector3& position, IFXVector3& direction, 
				IFXSimpleList** ppPickedObjectList);

	IFXRESULT  IFXAPI 	 SetMatrix(U32 Index, IFXMatrix4x4* pMatrix);
	
	IFXRESULT  IFXAPI 	 InvalidateMatrix();

	
	IFXRESULT  IFXAPI 	 AddParent(IFXNode* pParentNode);
	IFXRESULT  IFXAPI 	 RemoveParent(const U32 index);
	U32 IFXAPI   GetNumberOfParents();
	IFXRESULT  IFXAPI 	 GetNumberOfInstances(U32* numInstances);
	IFXNode* IFXAPI GetParentNR(const U32 parentIndex);

	IFXRESULT  IFXAPI 	 Prune();

	void   IFXAPI   PreDestruct();

protected:
	CIFXNode();
	virtual ~CIFXNode();

	// IFXUnknown
	U32                  m_uRefCount;

	// IFXModifier
	IFXRESULT IFXAPI  GenerateOutput ( 
					U32 inOutputDataElementIndex, 
					void*& rpOutData, 
					BOOL& rNeedRelease );

	IFXRESULT IFXAPI  SetDataPacket (
					IFXModifierDataPacket* pInInputDataPacket,
					IFXModifierDataPacket* pInDataPacket);

	// IFXNode
	IFXCollection**      m_pCollections;
	U32                  m_uCollections;
	U32                  m_uCollectionsAllocated;

	struct IFXParent 
	{
		IFXNode*	pParentNR;
		IFXSubject*	pParentDataPacketAsSubjectNR;
		U32			uParentsModChainsTransformAspectBit;
	};
	IFXArray<IFXParent>		m_Parents;
	IFXArray<IFXMatrix4x4>	m_Local;
	IFXArray<IFXMatrix4x4>	m_World;
	IFXArray<IFXNode*>		m_pChildren;

	U32                  m_debugFlags;

	IFXDECLAREMEMBER(IFXSimpleList,m_pPickObjectList);

protected:
	U32				m_uMyDataPacketTransformAspectBit;
	U32				m_uMyDataPacketTransformIndex;
	IFXSubject*     m_pMyDataPacketAsSubjectNR;
	IFXObserver*    m_pMyIFXObserverNR;

private:
	IFXNode*        m_pMyIFXNodeNR;
	IFXSubject*     m_pSceneGraphSubjectNR;

	IFXRESULT     AttachToParentsWorldTransform(U32 parentIndex); 

	BOOL IsThisNode(IFXNode* pParentNode, IFXNode* pThisNode);
	static BOOL HasParent(
					IFXNode* pMyNode, U32 uMyInstance, 
					IFXNode* pParentNode, U32 uParentInstance = 0xFFFFFFFF);
};

#endif
