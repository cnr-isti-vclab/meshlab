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
	@file	CIFXNode.cpp                                                          
*/

#include "IFXSceneGraphPCH.h"
#include "CIFXNode.h"
#include "CIFXPrimitiveOverlap.h"
#include "CIFXCollisionResult.h"
#include "IFXBoundHierarchyMgr.h"
#include "IFXSimpleList.h"
#include "IFXPickObject.h"
#include "IFXAnimationModifier.h"
#include "IFXModifierDataPacket.h"
#include "IFXCoreCIDs.h"
#include "IFXDids.h"


CIFXNode::CIFXNode() :
IFXDEFINEMEMBER(m_pPickObjectList)
{
	// IFXUnknown
	m_uRefCount = 0;

	// IFXModifier
	IFXASSERT( m_pModifierDataPacket ==  NULL );
	IFXASSERT( m_pInputDataPacket == NULL );

	// IFXNode
	m_pCollections = NULL;
	m_uCollections = 0;
	m_uCollectionsAllocated = 0;

	m_debugFlags = 0;
	// These vars DO NOT participate in ref counting:
	m_Parents.Clear();
	m_Local.Clear();
	m_World.Clear();

	m_pChildren.Clear();

	m_uMyDataPacketTransformIndex = (U32)-1;
	m_uMyDataPacketTransformAspectBit = (U32)-1;
	m_pMyDataPacketAsSubjectNR = NULL;

	m_pMyIFXNodeNR = NULL;
	m_pMyIFXObserverNR = NULL;
	m_pSceneGraphSubjectNR = NULL;

	IFXMatrix4x4& LocalMatrix = m_Local.CreateNewElement();
	LocalMatrix.MakeIdentity();
}


void CIFXNode::PreDestruct()
{
	IFXRESULT rc = IFX_OK;
	CIFXModifier::PreDestruct();
	m_Parents.Clear();
	m_Local.Clear();

	// Detach from the modifier chain's transform data element.
	if( m_pMyDataPacketAsSubjectNR )
	{
		rc = m_pMyDataPacketAsSubjectNR->Detach( m_pMyIFXObserverNR );
		m_pMyDataPacketAsSubjectNR = NULL;
		IFXASSERT( IFXSUCCESS( rc ) );
	}

	IFXDECLARELOCAL(IFXNode, pThisNode);
	rc = this->QueryInterface( IID_IFXNode, (void**)&pThisNode);
	IFXASSERT( IFXSUCCESS( rc ) );
	// Release the immediate children
	IFXNode* pNextChildNR;
	while (m_pChildren.GetNumberElements() && IFXSUCCESS( rc )) 
	{
		pNextChildNR = m_pChildren[0];
		if (NULL != pNextChildNR) 
		{
			U32 i, numParents = 0;
			numParents = pNextChildNR->GetNumberOfParents();
			for (i = numParents; i != 0 && IFXSUCCESS( rc ); --i) 
			{
				rc = pNextChildNR->RemoveParent(i-1);
				IFXASSERT( IFXSUCCESS( rc ) );
			}
		}
	}

	// Delete Picking Cache
	IFXRELEASE( m_pPickObjectList );

	if( m_pSceneGraph )
	{
		SetSceneGraph( NULL );
		m_pMyIFXNodeNR = NULL;
		m_pSceneGraphSubjectNR = NULL;
	}

	// Release collections
	if( m_pCollections )
	{
		U32 i;
		for ( i = m_uCollections; i--; )
		{
			IFXASSERT( m_pCollections[i] );
			IFXRELEASE( m_pCollections[i] );
		}

		IFXDeallocate( m_pCollections );
		m_pCollections = 0;
	}
}


CIFXNode::~CIFXNode()
{
	IFXASSERT( NULL == m_pMyDataPacketAsSubjectNR );
}


// IFXObserver
IFXRESULT CIFXNode::Update( IFXSubject* pInSubject, U32 uInChangeBits, IFXREFIID rIType )
{
	if( !uInChangeBits ) // if subject is destructing...
	{
		// Detach from the parents modifier chain's transform data element.
		//IFXRESULT result = IFX_E_CANNOT_FIND;
		BOOL found = false;
		U32 parentIndex;
		for (parentIndex = 0; parentIndex < m_Parents.GetNumberElements(); parentIndex++) 
		{
			IFXParent& Element = m_Parents.GetElement(parentIndex);
			if (pInSubject == Element.pParentDataPacketAsSubjectNR) 
			{
				Element.pParentDataPacketAsSubjectNR = NULL;
				AttachToParentsWorldTransform(parentIndex);
				found = true;
			}
		}
		if (!found)
		{
			if( pInSubject == m_pMyDataPacketAsSubjectNR )  
			{
				m_pMyDataPacketAsSubjectNR = NULL;
			} 
			else if( pInSubject == m_pSceneGraphSubjectNR ) 
			{
				m_pSceneGraph = NULL;
				m_pSceneGraphSubjectNR = NULL;
			} 
			else if( pInSubject == m_pModChainSubNR ) 
			{
				m_pModChainSubNR = NULL;
				m_pModChainNR = NULL;
			} 
			else 
			{
				IFXASSERT( 0 );
			}
			pInSubject->Detach( m_pMyIFXObserverNR );
		}
	} 
	else 
	{
		U32 parentIndex;
		for (parentIndex = 0; parentIndex < m_Parents.GetNumberElements(); parentIndex++) 
		{
			IFXParent& Element = m_Parents.GetElement(parentIndex);
			if((Element.uParentsModChainsTransformAspectBit != (U32)-1 ) && 
			   ( uInChangeBits & Element.uParentsModChainsTransformAspectBit ) ) 
			{
				if( m_pModifierDataPacket )
					m_pModifierDataPacket->InvalidateDataElement( 
													m_uMyDataPacketTransformIndex );
			} 
			else if( pInSubject == m_pMyDataPacketAsSubjectNR ) 
			{
				if( uInChangeBits & m_uMyDataPacketTransformAspectBit ) 
				{
					PostChanges( uInChangeBits );
				}
			}
		}
	}

	return IFX_OK;
}


// IFXMarker
IFXRESULT CIFXNode::SetSceneGraph( IFXSceneGraph* pInSceneGraph )
{
	IFXRESULT rc = IFX_OK;

	if( !m_pMyIFXNodeNR )
	{
		rc = QueryInterface( IID_IFXNode, (void**)&m_pMyIFXNodeNR );
		// this is a weak ref'ed pointer to this conmponent's node interface
		// not used to manage this object's lifetime, but to determine
		// what activity needs to place in the weak ref mgmnt system.
		if( m_pMyIFXNodeNR )
			m_pMyIFXNodeNR->Release();
	}

	if( !m_pMyIFXObserverNR && IFXSUCCESS(rc))
	{
		rc = QueryInterface( IID_IFXObserver, (void**)&m_pMyIFXObserverNR );
		// this is a weak ref'ed pointer to this conmponent's node interface
		// not used to manage this object's lifetime, but to determine
		// what activity needs to place in the weak ref mgmnt system.
		if( m_pMyIFXObserverNR)
			m_pMyIFXObserverNR->Release();
	}

	// Create the collection to manage spatials for this node's sub-graph
	// by calling GetCollection() if it doesn't already exist.
	// Note: pCollection is intentionally not used.
	// Note: The call to GetCollection() must preceed the AddSpatials() call.
	if( !m_pCollections || !m_pCollections[0] )
	{
		IFXCollection* pCollection = NULL;
		if( IFXSUCCESS( rc ) )
			rc = GetCollection( CID_IFXSimpleCollection, &pCollection );
		IFXRELEASE( pCollection );
	}


	if( IFXSUCCESS( rc ) )
	{
		if( m_pSceneGraph )
		{
			// Change manager detach...
			if (NULL == m_pSceneGraphSubjectNR)
				rc = IFX_E_INVALID_POINTER;
			else
			{
				m_pSceneGraphSubjectNR->Detach( m_pMyIFXObserverNR );
				m_pSceneGraphSubjectNR = NULL;
				m_pSceneGraph = NULL;
			}
		}
		else
		{ // first initialization
			IFXSpatial* pSpatial = NULL;
			QueryInterface( IID_IFXSpatial, (void**)&pSpatial );
			if (NULL != pSpatial) 
			{
				rc = AddSpatials( &pSpatial, 1 );
				pSpatial->Release();
			}
		}
	}

	if( IFXSUCCESS( rc ) )
		rc = CIFXMarker::SetSceneGraph( pInSceneGraph );

	// Change manager attach...
	if( m_pSceneGraph )
	{
		m_pSceneGraph->QueryInterface( IID_IFXSubject, (void**)&m_pSceneGraphSubjectNR );
		IFXASSERT( m_pSceneGraphSubjectNR );
		if( m_pSceneGraphSubjectNR )
		{
			m_pSceneGraphSubjectNR->Release();
			m_pSceneGraphSubjectNR->Attach( m_pMyIFXObserverNR, 0 );
		}
	}

	return rc ;
}


IFXRESULT CIFXNode::Mark(void)
{
	IFXRESULT rc = IFX_OK;

	rc = CIFXMarker::Mark();

	if( IFXSUCCESS(rc) )
	{
		// Mark all children recursively
		IFXNode* pNextChildNR = NULL;
		IFXMarker* pMarker = NULL;
		U32 childrenCount, i;

		childrenCount = GetNumberOfChildren();
		for (i = 0; i < childrenCount && IFXSUCCESS(rc); i++)
		{
			pNextChildNR = GetChildNR(i);
			if (NULL != pNextChildNR)
				rc = pNextChildNR->QueryInterface( IID_IFXMarker, (void**)&pMarker );
			else
				rc = IFX_E_NOT_INITIALIZED;

			if( IFXSUCCESS(rc) )
			{
				rc = pMarker->Mark();
				IFXRELEASE( pMarker );
			}
		}
	}

	return rc;
}


void CIFXNode::SetPriority( U32 uInPriority,
						   BOOL bRecursive,
						   BOOL bPromotionOnly )
{
	CIFXMarker::SetPriority( uInPriority, bRecursive, bPromotionOnly );

	// Call SetPriority() on all children recursively:
	if( bRecursive )
	{
		IFXRESULT rc = IFX_OK;
		IFXNode* pNextChildNR = NULL;
		IFXMarker* pMarker = NULL;
		U32 childCounter, i;

		childCounter = GetNumberOfChildren();
		for (i = 0; i < childCounter && IFXSUCCESS( rc ); i++)
		{
			pNextChildNR = GetChildNR(i);
			if (NULL != pNextChildNR)
				rc = pNextChildNR->QueryInterface( IID_IFXMarker, (void**)&pMarker );
			else
				rc = IFX_E_INVALID_POINTER;

			if( IFXSUCCESS( rc ) )
			{
				pMarker->SetPriority( uInPriority,
					bRecursive,
					bPromotionOnly );

				IFXRELEASE( pMarker );
			}
		}
	} // end if( bRecursive == IFX_TRUE)
}


// IFXCollection
IFXRESULT CIFXNode::InitializeCollection(
	IFXSpatial**      pInSpatials,
	U32               uInNumberOfSpatials,
	IFXSpatial::eType eInType )
{
	IFXRESULT result = IFX_OK;
	U32 i;

	for ( i = m_uCollections; i-- && IFXSUCCESS(result); )
	{
		IFXASSERT( m_pCollections[i] );
		result = m_pCollections[i]->InitializeCollection( 
										pInSpatials, uInNumberOfSpatials, eInType );
	}

	return result;
}

IFXRESULT CIFXNode::InitializeCollection(
	IFXCollection* pInCollection )
{
	IFXRESULT result = IFX_OK;
	U32 i;

	for ( i = m_uCollections; i-- && IFXSUCCESS(result); )
	{
		IFXASSERT( m_pCollections[i] );
		result = m_pCollections[i]->InitializeCollection( pInCollection );
	}

	return result;
}

IFXRESULT CIFXNode::AddSpatials(IFXSpatial** pInSpatials,
								U32 uInNumberOfSpatials,
								IFXSpatial::eType eInType)
{
	IFXRESULT result = IFX_OK;
	IFXDECLARELOCAL(IFXCollection,pCollection);

	// Recursively propagate up the graph to the root
	U32 numberparents = m_Parents.GetNumberElements();
	U32 i;
	for (i = 0; i < numberparents && IFXSUCCESS(result); i++) 
	{
		IFXRELEASE(pCollection);

		if (NULL != m_Parents[i].pParentNR)
			result = m_Parents[i].pParentNR->QueryInterface( 
											IID_IFXCollection, (void**)&pCollection );
		else
			result = IFX_E_NOT_INITIALIZED;

		if( IFXSUCCESS(result) )
			result = pCollection->AddSpatials( 
									pInSpatials, uInNumberOfSpatials, eInType );
	}

	// Propagate to all associated collections
	U32 c;
	for ( c = m_uCollections; c-- && IFXSUCCESS(result); )
	{
		IFXRELEASE( pCollection );

		if (NULL != m_pCollections[c])
			result = m_pCollections[c]->QueryInterface( 
											IID_IFXCollection, (void**)&pCollection );
		else
			result = IFX_E_NOT_INITIALIZED;

		if( IFXSUCCESS(result) )
		{
			result = pCollection->AddSpatials( 
									pInSpatials, uInNumberOfSpatials, eInType );
		}
	}

	return result;
}

IFXRESULT CIFXNode::RemoveSpatials( IFXSpatial**      pInSpatials,
								   U32               uInNumberOfSpatials,
								   IFXSpatial::eType eInType )
{
	IFXRESULT result = IFX_OK;
	IFXDECLARELOCAL(IFXCollection,pCollection);

	// Recursively propagate up the graph to the root
	U32 i;
	for (i = 0; i < m_Parents.GetNumberElements() && IFXSUCCESS(result); i++) 
	{
		IFXRELEASE( pCollection );
		
		IFXNode *pParentNode;
		pParentNode = m_Parents[i].pParentNR;

		if (NULL != pParentNode) 
		{
			result = pParentNode->QueryInterface( 
										IID_IFXCollection, (void**)&pCollection );

			if( IFXSUCCESS(result) )
			{
				result = pCollection->RemoveSpatials( 
										pInSpatials, uInNumberOfSpatials, eInType );
			}
		}
	}

	// Propagate to all associated collections
	U32 c;
	for ( c = m_uCollections; c-- && IFXSUCCESS(result); )
	{
		if (NULL != m_pCollections[c])
			result = m_pCollections[c]->RemoveSpatials( 
											pInSpatials, uInNumberOfSpatials, eInType );
		else
			result = IFX_E_NOT_INITIALIZED;
	}


	if(m_uCollections > 1)
	{
		for(i = 0; i < uInNumberOfSpatials && IFXSUCCESS(result); i++)
		{
			if (NULL != pInSpatials[i])
				pInSpatials[i]->PostChanges(IFXSpatial::EOL);
			else
				result = IFX_E_NOT_INITIALIZED;
		}
	}

	return result;
}

IFXRESULT CIFXNode::GetSpatials(
								IFXSpatial**&     rpOutSpatials,
								U32&              ruOutNumberOfSpatials,
								IFXSpatial::eType eInType )
{
	IFXRESULT result = IFX_OK;

	if( m_pCollections && m_pCollections[0] )
		m_pCollections[0]->GetSpatials( 
								rpOutSpatials, ruOutNumberOfSpatials, eInType );
	else
		result = IFX_E_NOT_INITIALIZED;

	return result;
}


// IFXNode
IFXRESULT CIFXNode::ApplyTransformToNode( IFXMatrix4x4* pPinF, U32 parent )
{
	IFXRESULT result = IFX_OK;
	IFXNode *pParent = NULL;

	if( NULL == pPinF )
		result = IFX_E_INVALID_POINTER;

	if (IFXSUCCESS(result))
	{
		pParent = GetParentNR(parent);
		if( NULL == pParent )
			result = IFX_E_INVALID_POINTER;
	}

	// to avoid the problem when more than one Node Instance have the same
	// Local matrix we should exclude the cases when Node Parent has more 
	// than one World matrices
	if (IFXSUCCESS(result))
	{
		U32 numGrandInstances;
		result = pParent->GetNumberOfInstances(&numGrandInstances);
		if (IFXSUCCESS(result) && numGrandInstances > 1)
			result = IFX_E_UNSUPPORTED;
	}

	IFXMatrix4x4 P, Pi;
	// Change the model's local transform so that it's modifier chain's
	// resultant transform generates the desired Transform.

	// If  P is the (p)arent's world transform,
	// and oL is the (l)ocal transform,
	// and oF is the (f)inal world transform,
	// then: P oL = oF

	// Given nF, (n)ew (f)inal world transform,
	// nL, the (n)ew (l)ocal, can be found using:

	// nL = Pi (Pi nF) P oL oFi

	// Note: Pi is the inverse of P and oFi is the inverse of the old final 
	// world transform.

	// Start building terms...
	// P/Pi
	if (IFXSUCCESS(result)) {
		IFXMatrix4x4* pParentsWorld = NULL;

		if ( pParent->GetNumberOfParents() > 0 )
		{
			// give the first World matrix because our parent has 
			// one or less parents (limitation)
			result = pParent->GetWorldMatrix(0, &pParentsWorld);

			if (IFXSUCCESS(result)) {
				memcpy( P.Raw(), pParentsWorld->RawConst(), 64 );
				Pi.Invert3x4(*pParentsWorld);
			}
		}
		else
		{
			P.Reset();
			Pi.Reset();
		}
	}

	if (IFXSUCCESS(result))
	{
		// oFi
		IFXMatrix4x4 oFi;
		IFXMatrix4x4* pWorldTransform = NULL;

		// world matrix related to required parent
		U32 numInstances;
		U32 matrixIndex = 0;
		result = GetNumberOfInstances(&numInstances);
		if (IFXSUCCESS(result))
		{
			if (parent == 0 || numInstances == GetNumberOfParents())
				matrixIndex = parent;
			else
			{
				U32 i;
				for (i = 0; i < parent && IFXSUCCESS(result); i++)
				{
					IFXNode *pTempParent;
					pTempParent = GetParentNR(i);
					U32 tempNumInstances;
					result = pTempParent->GetNumberOfInstances(&tempNumInstances);
					matrixIndex += tempNumInstances;
				}
			}
		}
		if (IFXSUCCESS(result))
			result = GetWorldMatrix(matrixIndex, &pWorldTransform );

		if( IFXSUCCESS(result) )
		{
			IFXMatrix4x4 oL;
			oL.Multiply( Pi, *pWorldTransform );

			if (pWorldTransform)
				oFi.Invert3x4(*pWorldTransform);
			else
				oFi.Reset();

			// Build the result
			IFXMatrix4x4 temp;
			temp.Multiply( Pi, *pPinF );
			IFXMatrix4x4 nL;
			nL.Multiply( temp, P );
			temp.Multiply( nL, oL );
			nL.Multiply( temp, oFi );

			// !Q.E.D.
			result = SetMatrix(parent, &nL );
		}
	}


	return result;
}


IFXRESULT CIFXNode::GetCollection( IFXREFCID       rInCollectionCID,
								  IFXCollection** ppOutCollection )
{
	IFXRESULT result = IFX_OK;

	if( !ppOutCollection )
		return IFX_E_INVALID_POINTER;

	// Scan for an existing collection of the given type.
	U32 c, uCollection = (U32)-1;

	for ( c = m_uCollections; c--; )
	{
		if( m_pCollections[c] )
		{
			if( m_pCollections[c]->GetCID() == rInCollectionCID )
			{
				uCollection = c;
				break;
			}
		}
	}

	// Create one if it doesn't exist.
	if( uCollection == (U32)-1 )
	{
		uCollection = m_uCollections;

		// (Re)allocate the collection array as needed
		if( m_uCollectionsAllocated )
		{ // Reallocate
			if( ( m_uCollections >= m_uCollectionsAllocated ) )
			{
				m_pCollections = (IFXCollection**)IFXReallocate( 
											m_pCollections,
											(m_uCollections+2)*sizeof(IFXCollection*) );
				if( m_pCollections)
				{
					m_uCollectionsAllocated = (m_uCollections+2);
				}
				else
					result = IFX_E_OUT_OF_MEMORY;
			}
		}
		else // Initial allocation
		{
			m_pCollections = (IFXCollection**)IFXAllocate( 2*sizeof(IFXCollection*) );
			if( m_pCollections )
				m_uCollectionsAllocated = 2;
			else
				result = IFX_E_OUT_OF_MEMORY;
		}

		// Create the collection
		if( IFXSUCCESS(result) )
			result = IFXCreateComponent( 
							rInCollectionCID, IID_IFXCollection,
							(void**)&m_pCollections[m_uCollections] );

		// Initialize it if it is not the first
		if( IFXSUCCESS(result) && m_uCollections )
		{
			m_pCollections[m_uCollections]->InitializeCollection(m_pCollections[0]);
		}

		if( IFXSUCCESS(result) )
			m_uCollections++;
	}

	if( IFXSUCCESS(result) )
	{
		m_pCollections[uCollection]->AddRef();
		*ppOutCollection = m_pCollections[uCollection];
	}

	return result;
}

/**
	Get child by index.

	@param	const U32 ChildIndex - index of required child
	
	@return	IFXNode* ppChildNode - result value

	@note	no references
*/
IFXNode* CIFXNode::GetChildNR(const U32 childIndex)
{
	IFXNode *pOutNode;

	if (m_pChildren.GetNumberElements() > childIndex)
		pOutNode = m_pChildren[childIndex];
	else
		pOutNode = NULL;

	return pOutNode;
}

/**
	Add child to parent.

	@param	IFXNode* pChildNode - Node to add it as child to the m_pChildren 
			array

	@return	IFXRESULT

	@note	Node->AddRef
*/
IFXRESULT CIFXNode::AddChild(IFXNode* pChildNode)
{
	IFXRESULT result = IFX_OK;

	if (NULL != pChildNode) 
	{
		U32 i;
		for (i = 0; i < GetNumberOfChildren(); i++) 
		{
			if (m_pChildren[i] == pChildNode) 
			{
				result = IFX_W_ALREADY_EXISTS;
				break;
			}
		}

		if (IFX_W_ALREADY_EXISTS != result) 
		{
			IFXNode*& pNode = m_pChildren.CreateNewElement();
			pNode = pChildNode;
			pChildNode->AddRef();
		}
	}
	else
		result = IFX_E_NOT_INITIALIZED;

	return result;
}

/**
	Remove child

	@param	IFXNode* pChildNode - requested child

	@return	IFXRESULT

	@note	Node->Release
			fuction looks for requested Node and removes it from m_pChildren 
			array
*/
IFXRESULT CIFXNode::RemoveChild(IFXNode* pChildNode)
{
	IFXRESULT result = IFX_OK;

	if (NULL == pChildNode)
		result = IFX_E_NOT_INITIALIZED;

	if (IFXSUCCESS(result)) 
	{
		U32 i;
		for (i = 0; i < GetNumberOfChildren(); i++) 
		{
			if (m_pChildren[i] == pChildNode)
			{
				m_pChildren.DeleteElement(i);
				break;
			}
		}

		pChildNode->Release();
	}

	return result;
}

/**
	remove child by index

	@note	Node->Release
*/
IFXRESULT CIFXNode::RemoveChild(U32 index)
{
	IFXRESULT result = IFX_OK;
	IFXNode *pChildNode = NULL;

	if (GetNumberOfChildren() > index) 
	{
		pChildNode = m_pChildren[index];
		m_pChildren.DeleteElement(index);
		pChildNode->Release();
	}
	else
		result = IFX_E_INVALID_RANGE;

	return result;
}

const IFXMatrix4x4& CIFXNode::GetMatrix(U32 index) const
{
	return m_Local[index];
}

BOOL IsThisBranch(
						IFXNode* pMyNode, U32 uMyInstance, 
						IFXNode* pParentNode, U32 uParentInstance)
{
	BOOL result = FALSE;
	U32 count = 0, i = 0, index = 0;
	U32 myInstances = 0;

	pMyNode->GetNumberOfInstances(&myInstances);
	count = pMyNode->GetNumberOfParents();

	for (i = 0; i < count; i++) 
	{
		U32 parentCount = 1;

		while(
				(i < count-1) && 
				(pMyNode->GetParentNR(i) == pMyNode->GetParentNR(i+parentCount))
			 )
			parentCount++;

		{
			U32 parentInstances = 0;
			pMyNode->GetParentNR(i)->GetNumberOfInstances(&parentInstances);
			U32 index2 = index + parentCount*parentInstances;

			if (uMyInstance < index2) 
			{
				if (pMyNode->GetParentNR(i) == pParentNode)
				{
					result = (parentCount + 1 - index - (uMyInstance-index)/parentCount == uParentInstance);
				}
				else
					result = IsThisBranch(
						pMyNode->GetParentNR(i), (myInstances-1-uMyInstance-index)/parentCount, 
						pParentNode, uParentInstance);
				break;
			}
			index = index2;
		}
		i += parentCount-1;
	}

	return result;
}

IFXRESULT CIFXNode::GetSubgraphBound( IFXVector4& rSphere, U32 rootInstance )
{
	IFXRESULT result = IFX_OK;

	IFXSpatial** pModels = NULL;
	U32          uModels = 0;
	IFXVector4  b;
	U32          s, i, numInst;
	rSphere.Reset();

	if( m_pCollections && m_pCollections[0] )
	{
		// Opaque models
		result = m_pCollections[0]->GetSpatials( pModels,
			uModels,
			IFXSpatial::OPAQUE_MODEL );

		IFXModel *pModel = NULL;

		for ( s = 0; IFXSUCCESS( result ) && s < uModels; s++ )
		{
			result = pModels[s]->QueryInterface( IID_IFXModel, (void**)&pModel );

			if( IFXSUCCESS( result ) )
			{
				result = pModel->GetNumberOfInstances( &numInst );
				IFXRELEASE( pModel );
			}

			IFXDECLARELOCAL(IFXNode, pNode);
			if( IFXSUCCESS( result ) )
				result = pModels[s]->QueryInterface(IID_IFXNode, (void**)&pNode);
#ifdef _DEBUG
			U32 uRID;
			IFXDECLARELOCAL( IFXSceneGraph, pSceneGraph );
			IFXDECLARELOCAL(IFXPalette, pNodePalette);
			IFXString ModelName;
			
			if (IFXSUCCESS(result))
				result = pNode->GetSceneGraph(&pSceneGraph);
			if (IFXSUCCESS(result))
				result = pSceneGraph->GetPalette(IFXSceneGraph::NODE, &pNodePalette);
			if (IFXSUCCESS(result))
				result = pNodePalette->FindByResourcePtr(pNode, &uRID);
			if (IFXSUCCESS(result))
				result = pNodePalette->GetName(uRID, &ModelName);
#endif
			if (pNode == (IFXNode*)this)
			{
#ifdef _DEBUG
					if (IFXSUCCESS(result)) 
						IFXTRACE_GENERIC(L"[SamplePlayer] subgraph %ls (%i)\n", ModelName.Raw(), rootInstance);
#endif
				result = pModels[s]->GetSpatialBound( rSphere, rootInstance );
			}
			else
			{
				for( i = 0; i < numInst && IFXSUCCESS( result ); i++ )
				{
#ifdef _DEBUG
					if (IFXSUCCESS(result)) 
						IFXTRACE_GENERIC(L"[SamplePlayer] %ls (%i)\n", ModelName.Raw(), i);
#endif
					if (!IsThisBranch(pNode, i, (IFXNode*)this, rootInstance))
						continue;
#ifdef _DEBUG
					if (IFXSUCCESS(result)) 
						IFXTRACE_GENERIC(L"[SamplePlayer] subgraph %ls (%i)\n", ModelName.Raw(), i);
#endif
					result = pModels[s]->GetSpatialBound( b, i );

					if( IFXSUCCESS( result ) )
					{
						rSphere.IncorporateSphere( &b );
					}
				}
			}
		}

		// Translucent models
		if( IFXSUCCESS( result ) )
		{
			pModels = NULL;
			result = m_pCollections[0]->GetSpatials( 
											pModels, uModels, 
											IFXSpatial::TRANSLUCENT_MODEL );
		}

		for ( s = uModels; IFXSUCCESS( result ) && s--; )
		{
			result = pModels[s]->QueryInterface( IID_IFXModel, (void**)&pModel );

			if( IFXSUCCESS( result ) )
			{
				result = pModel->GetNumberOfInstances( &numInst );
				IFXRELEASE( pModel );
			}

			IFXDECLARELOCAL(IFXNode, pNode);

			if( IFXSUCCESS( result ) )
				result = pModels[s]->QueryInterface(IID_IFXNode, (void**)&pNode);

			if (pNode == (IFXNode*)this)
				result = pModels[s]->GetSpatialBound( rSphere, rootInstance );
			else
			{
				for( i = 0; i < numInst && IFXSUCCESS( result ); i++ )
				{
					if (!IsThisBranch(pNode, i, (IFXNode*)this, rootInstance))
						continue;

					result = pModels[s]->GetSpatialBound( b, i );

					if( IFXSUCCESS( result ) )
						rSphere.IncorporateSphere( &b );
				}
			}
		}
	}

	return result;
}

/**
  Number of parent's children.

  @param   BOOL bInDeepCount - flag to switch method of Number of children calculation.
  @param   U32* puOutCount - return value.

  @return  IFXRESULT.

  @note    with bInDeepCount == FALSE function doesn't return real number of children.
           if child is parented by one parent more than one times 
		   it stored in m_pChildren array only once.
*/
U32 CIFXNode::GetNumberOfChildren(BOOL bInDeepCount)
	{
	U32 result = 0;
		if( bInDeepCount)
		{
			Counter(IFX_NODE_COUNTER_NODES, &result); // Count recursively
		}
		else
		{ // Count shallow ( direct children only )
			result = m_pChildren.GetNumberElements();
		}

	return result;
}

IFXRESULT CIFXNode::GetWorldMatrices( IFXArray<IFXMatrix4x4>** ppWorldMatrix )
{
	IFXRESULT result = IFX_OK;

	if( ppWorldMatrix )
	{
		if ( m_pModChainNR )
		{
			IFXModifierDataPacket* pMDP = NULL;
			result = m_pModChainNR->GetDataPacket( pMDP );

			if( IFXSUCCESS(result) )
				result = pMDP->GetDataElement( 
									m_uMyDataPacketTransformIndex, 
									(void**)ppWorldMatrix );

			IFXRELEASE( pMDP );
		}
		else // allow functioning in disabled state.
		{
			*ppWorldMatrix = &m_Local;
		}
	}
	else
		result = IFX_E_INVALID_POINTER;

	IFXASSERT( IFXSUCCESS( result ) );
	return result;
}

IFXRESULT CIFXNode::GetWorldMatrix(U32 Index, IFXMatrix4x4** ppWorldMatrix)
{
	IFXRESULT result = IFX_OK;
	IFXArray<IFXMatrix4x4> *ppWorldMatrices;
	U32 NumberParents;

	if( ppWorldMatrix )
	{
		if ( m_pModChainNR )
		{
			IFXModifierDataPacket* pMDP = NULL;
			result = m_pModChainNR->GetDataPacket( pMDP );

			if( IFXSUCCESS(result) )
				result = pMDP->GetDataElement( 
									m_uMyDataPacketTransformIndex, 
									(void**)&ppWorldMatrices);

			if(IFXSUCCESS(result))
				NumberParents = ppWorldMatrices->GetNumberElements();

			if (IFXSUCCESS(result) && NumberParents < Index+1)
				result = IFX_E_INVALID_RANGE;

			if(IFXSUCCESS(result))
				*ppWorldMatrix = &ppWorldMatrices->GetElement(Index);

			IFXRELEASE( pMDP );
		}
		else // allow functioning in disabled state.
		{
			*ppWorldMatrix = &m_Local[Index];
		}
	}
	else
		result = IFX_E_INVALID_POINTER;

	IFXASSERT( IFXSUCCESS( result ) );
	return result;
}

/**
	number of Node's instances.

	@param	U32* puOutCount - return value
	@return IFXRESULT
*/
IFXRESULT CIFXNode::GetNumberOfInstances(U32 *pOutCount)
{
	IFXRESULT result = IFX_OK;
	IFXDECLARELOCAL(IFXModifierChain, pModifierChain);
	IFXDECLARELOCAL(IFXModifierDataPacket, pDataPacket);
	IFXArray<IFXMatrix4x4> *ppWorldMatrices;

	if (pOutCount == NULL)
		result = IFX_E_NOT_INITIALIZED;

	if( IFXSUCCESS(result) )
		result = GetModifierChain( &pModifierChain );

	if( IFXSUCCESS(result) )
		result = pModifierChain->GetDataPacket( pDataPacket );

	if(IFXSUCCESS(result))
		result = pDataPacket->GetDataElement( 
									m_uMyDataPacketTransformIndex,
									(void**)&ppWorldMatrices );

	if(IFXSUCCESS(result))
		*pOutCount = ppWorldMatrices->GetNumberElements();

	return result;
}

IFXRESULT CIFXNode::Pick(
						U32 myInstance, IFXVector3& position, 
						IFXVector3& direction, IFXSimpleList** ppPickedObjectList)
{
	if( !ppPickedObjectList )
		return IFX_E_INVALID_POINTER;

	if( !m_pSceneGraph )
		return IFX_E_NOT_INITIALIZED;

	IFXRESULT result = IFX_OK;

	// Null the list in case the pointer still points to another
	// list of objects.  The objects should get deleted below.
	*ppPickedObjectList = NULL;

	// Get the models
	IFXSpatial** pSpatials[2];
	U32          uSpatials[2];
	GetSpatials( pSpatials[0], uSpatials[0], IFXSpatial::OPAQUE_MODEL );
	GetSpatials( pSpatials[1], uSpatials[1], IFXSpatial::TRANSLUCENT_MODEL );

	// If any models exist, we iterate through them and test
	// each model against the input ray
	if( uSpatials[0] || uSpatials[1] )
	{
		IFXModifierDataPacket* pDataPacket  = NULL;
		IFXModifierChain*      pModChain    = NULL;
		IFXMeshGroup*          pMeshGroup   = NULL;
		//IFXModel*              pModel       = NULL;

		CIFXPrimitiveOverlap overlap;
		CIFXCollisionResult* pCollisionResult  = NULL;
		IFXBoundHierarchy*   pBoundHierarchy   = NULL;
		IFXVector4           vBoundingSphere;
		IFXVector3           vTranslation;
		IFXVector3           vScale;
		//F32                  fCLODLevel         = -1;
		U32                  uPickedModels     =  0;
		U32                  i;

		// Normalize the ray's direction
		direction.Normalize();

		// Check if m_pPickObjectList has been created
		//    - If it's NULL then create it
		//    - If it's not NULL then remove any lingering models from it

		if( !m_pPickObjectList )
		{
			result = IFXCreateComponent( CID_IFXSimpleList,
				IID_IFXSimpleList,
				(void**)&m_pPickObjectList );
		}
		else
		{
			U32 uListSize;
			I32 i;

			m_pPickObjectList->GetCount(&uListSize);

			// Release existing pickObjects
			for(i=uListSize-1; i>=0 && IFXSUCCESS(result); i-- )
				result = m_pPickObjectList->Delete(i);
		}

		// Test each model for intersection with the ray
		U32 list;

		for ( list = 0; list < 2; list++ )
			for ( i=0; i<uSpatials[list] && IFXSUCCESS(result); i++ )
			{
				IFXModel* pModel = NULL;
				if (NULL != (pSpatials[list])[i])
					result = (pSpatials[list])[i]->QueryInterface( 
											IID_IFXModel, (void**)&pModel );
				else
					result = IFX_E_INVALID_POINTER;

				if( IFXSUCCESS(result) && pModel->GetPickable() )
				{
					U32 numInst, instance;
					result = pModel->GetNumberOfInstances(&numInst);

					for (
						instance = 0; 
						instance < numInst && IFXSUCCESS(result); 
						instance++) 
					{
						if (!HasParent(
							(IFXNode*)pModel, instance, 
							(IFXNode*)this, myInstance))
								continue;

						(pSpatials[list])[i]->GetSpatialBound(
													vBoundingSphere, instance);

						// Test Ray/Sphere intersection
						if( overlap.RaySphereIntersection(
										vBoundingSphere, position, direction) )
						{
							// We need to hang onto the current CLOD level and
							// restore it when we're done:
							//result = pModelResource->GetCLODLevel( &fCLODLevel );

							//if( IFXSUCCESS(result))
							//  result = pModelResource->SetCLODLevel(1.0f);

							if( IFXSUCCESS(result) )
								result = pModel->GetModifierChain(&pModChain);

							if( IFXSUCCESS(result) )
								result = pModChain->GetDataPacket(pDataPacket);

							U32 MeshGroupIDX = (U32)-1;
							if( IFXSUCCESS(result) )
								result = pDataPacket->GetDataElementIndex( 
															DID_IFXRenderableGroup, 
															MeshGroupIDX );
							if( IFXSUCCESS(result) )
								result = pDataPacket->GetDataElement( 
															MeshGroupIDX,
															IID_IFXMeshGroup, 
															(void**)&pMeshGroup );

							if( IFXSUCCESS(result) )
								result = pMeshGroup->GetBoundHierarchy(&pBoundHierarchy);

							if( IFXSUCCESS(result) )
							{
								IFXMatrix4x4* pWorldTransform = NULL;
								IFXMatrix4x4  mUnscaledWorldMatrix;
								IFXArray<IFXMatrix4x4> *pAWorldTransform;

								// Get unscaledWorldMatrix and scaleFactor
								if( IFXSUCCESS(result) )
									result = pModel->GetWorldMatrices(&pAWorldTransform);

								if( IFXSUCCESS(result) ) 
								{
#ifdef _DEBUG
									U32 numParents =
#endif
									pAWorldTransform->GetNumberElements();
									IFXASSERT(numParents == numInst);
									pWorldTransform = 
										&pAWorldTransform->GetElement(instance);
								}

								// Decompose the world matrix
								if( IFXSUCCESS(result) )
								{
									mUnscaledWorldMatrix.Reset();
									result = pWorldTransform->Decompose( 
																	vTranslation,
																	mUnscaledWorldMatrix,
																	vScale );
								}

								if( IFXSUCCESS(result) )
								{
									mUnscaledWorldMatrix.SetTranslation(vTranslation);

									// Set them on the boundHierarchy
									/// @todo: May be 0 below should be 'list'?
									pBoundHierarchy->SetMatrix(
														0, mUnscaledWorldMatrix, vScale);

									// Test ray/mesh intersection
									result = pBoundHierarchy->IntersectRay( 
																	position,
																	direction,
																	pModel->GetPickable(),
																	&pCollisionResult );
								}
							}

							IFXRELEASE(pMeshGroup);
							IFXRELEASE(pDataPacket);
							IFXRELEASE(pModChain);
							IFXRELEASE(pBoundHierarchy);

							if( IFXSUCCESS(result) && pCollisionResult)
							{
								IFXVector3     v1, v2, v3;
								IFXPickObject* pNewPickObject = NULL;
								IFXPickObject* pPickObject    = NULL;
								F32  fPickObjDistance;
								BOOL bDone     = FALSE;
								F32  fDistance = (pCollisionResult != NULL) 
														? pCollisionResult->GetDistance()
														: 0;
								I32  pPos      = uPickedModels - 1;
								I32  cPos      = uPickedModels;

								while( !bDone && IFXSUCCESS(result) && pPos >= 0 )
								{
									result = m_pPickObjectList->Get(
													pPos, (IFXUnknown**)&pPickObject);

									if( IFXSUCCESS(result) )
									{
										fPickObjDistance = pPickObject->GetDistance();

										if( fDistance < fPickObjDistance )
										{
											result = m_pPickObjectList->Set(
												cPos, (IFXUnknown*)pPickObject);

											if (!IFXSUCCESS(result))
												result = m_pPickObjectList->Add(
												(IFXUnknown*)pPickObject, (U32*)&cPos);

											if( IFXSUCCESS(result) )
											{
												cPos = pPos;
												pPos --;
											}
										}
										else
											bDone = TRUE;
									}

									IFXRELEASE(pPickObject);
								}

								if( IFXSUCCESS(result) )
									result = IFXCreateComponent( 
													CID_IFXPickObject,
													IID_IFXPickObject,
													(void**)&pNewPickObject );

								if( IFXSUCCESS(result) )
								{
									if (pCollisionResult != NULL) 
									{
										pNewPickObject->SetUVTCoords( 
											pCollisionResult->GetUCoord(),
											pCollisionResult->GetVCoord(),
											fDistance );

										pNewPickObject->SetIDs( 
											pCollisionResult->GetMeshID(),
											pCollisionResult->GetFaceID() );

										pCollisionResult->GetVertices( v1, v2, v3 );
										pNewPickObject->SetVertices( v1, v2, v3 );
									}

									pNewPickObject->SetModel(pModel);
									pNewPickObject->SetInstance(instance);

									result = m_pPickObjectList->Set(
													cPos, (IFXUnknown*)pNewPickObject);

									if (!IFXSUCCESS(result))
										result = m_pPickObjectList->Add(
													(IFXUnknown*)pNewPickObject, 
													(U32*)&cPos);

									if( IFXSUCCESS(result) )
										uPickedModels ++;
								}

								IFXRELEASE(pNewPickObject);
							}

							/*
							Restore the CLOD level if need be:
							if( pModelResource && fCLODLevel != -1 )
							pModelResource->SetCLODLevel( fCLODLevel );
							*/

							IFXRELEASE( pBoundHierarchy );
						}
					}
				}
				IFXRELEASE( pModel );
			}

			if( uPickedModels > 0 )
			{
				*ppPickedObjectList = m_pPickObjectList;
				m_pPickObjectList->AddRef();
			}
	}

	return result;
}

IFXRESULT CIFXNode::Prune()
{
	IFXRESULT result = IFX_OK;

	U32 i;
	U32 limit = m_Parents.GetNumberElements();
	//to optimize IFXArray::Delete we need to remove elements in reverse order
	while (limit--) 
	{
		AddRef();
		IFXNode* pCurrParent;

		if (IFXSUCCESS(result)) 
		{
			pCurrParent = m_Parents[limit].pParentNR;
			result = RemoveParent(limit);
		}

		if (IFXSUCCESS(result)) 
		{
			for (i = limit; i > 0; i--) 
			{
				if (pCurrParent == m_Parents[i-1].pParentNR) 
				{
					m_Parents.DeleteElement(i-1);
					limit--;
				}
			}
		}

		Release();
	}

	return result;
}

/**
	Remove existing parent completely. 
	All links of given parent with children will be cutted.

	@param	U32 Index	Index of parent which should be removed from the list.

	@return	IFXRESULT
*/
IFXRESULT CIFXNode::RemoveParent(U32 index)
{
	IFXRESULT result = IFX_OK;
	if (NULL != m_Parents[index].pParentNR) 
	{
		AddRef();
		
		IFXDECLARELOCAL(IFXNode, pThisNode);
		U32 i;
		BOOL removeIt = TRUE;

		result = this->QueryInterface( IID_IFXNode, (void**)&pThisNode);

		//find Parents equal to given
		for (i = 0; i < GetNumberOfParents() && IFXSUCCESS(result); i++)
		{
			if (i != index && m_Parents[i].pParentNR == m_Parents[index].pParentNR)
			{
				removeIt = FALSE;
				break;
			}
		}

		if (removeIt && IFXSUCCESS(result)) 
		{
			//remove child from m_pChildren
			if (NULL != m_Parents[index].pParentNR)
				result = m_Parents[index].pParentNR->RemoveChild(pThisNode);
			else
				result = IFX_E_NOT_INITIALIZED;

			U32 i;
			for ( i = 0; i < IFXSpatial::TYPE_COUNT && IFXSUCCESS(result); i++ ) 
			{
				IFXSpatial** pSpatials = NULL;
				U32 nSpatials = 0;

				result = GetSpatials( pSpatials, nSpatials, (IFXSpatial::eType)i );

				if(IFXSUCCESS(result) && nSpatials)
					result = m_Parents[index].pParentNR->RemoveSpatials( 
										pSpatials, nSpatials, (IFXSpatial::eType)i );
			}
			if (IFXSUCCESS(result)) 
			{
				m_Parents[index].pParentNR = NULL;
					result = AttachToParentsWorldTransform(index);
			}
		}

		if (IFXSUCCESS(result)) 
		{
			m_World.DeleteElement(index);
			m_Parents.DeleteElement(index);
		}

		Release();
	}
	else
		result = IFX_E_NOT_INITIALIZED;

	return result;
}

IFXRESULT CIFXNode::SetMatrix(U32 index, IFXMatrix4x4* pMatrix)
{
	IFXRESULT result = IFX_OK;

	if( NULL != pMatrix && NULL != &m_Local[index] )
	{
		m_Local[index] = *pMatrix;

		if( m_pModifierDataPacket )
		{
#ifdef _DEBUG
			U32 uId;
			result = m_pModifierDataPacket->GetDataElementIndex( DID_IFXTransform, uId );

			IFXASSERT( IFXSUCCESS( result ) );
			IFXASSERT( uId == m_uMyDataPacketTransformIndex );
#endif

			result = m_pModifierDataPacket->InvalidateDataElement( 
												m_uMyDataPacketTransformIndex );
		}

		IFXASSERT( IFXSUCCESS( result ) );
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}

/**
	helper function to check the parent/children tree to know 
	if the Node already parent of given one

	@param	IFXNode* pParentNode - Node to find

	@return BOOL - result of operation. TRUE - Node was found in the tree
*/
BOOL CIFXNode::IsThisNode(IFXNode* pParentNode, IFXNode* pThisNode)
{
	BOOL result = FALSE;
	IFXNode* pNodeNR = NULL;

	if (pParentNode != pThisNode) 
	{
		U32 i;
		for (i = 0; i < pParentNode->GetNumberOfParents() && IFXSUCCESS( result ); i++)
		{
			pNodeNR = pParentNode->GetParentNR(i);
			result = IsThisNode(pNodeNR, pThisNode);
		}
	} 
	else
		result = TRUE;

	return result;
}

BOOL CIFXNode::HasParent(
						IFXNode* pMyNode, U32 uMyInstance, 
						IFXNode* pParentNode, U32 uParentInstance)
{
	BOOL result = FALSE;
	U32 count = 0, i = 0, index = 0;
	U32 myInstances = 0;

	pMyNode->GetNumberOfInstances(&myInstances);
	count = pMyNode->GetNumberOfParents();

	for (i = 0; i < count; i++) 
	{
		U32 parentCount = 1;

		while(
				(i < count-1) && 
				(pMyNode->GetParentNR(i) == pMyNode->GetParentNR(i+parentCount))
			 )
			parentCount++;

		{
			U32 parentInstances = 0;
			pMyNode->GetParentNR(i)->GetNumberOfInstances(&parentInstances);
			U32 index2 = index + parentCount*parentInstances;

			if (uMyInstance < index2) 
			{
				if (pMyNode->GetParentNR(i) == pParentNode)
				{
					result = ((uMyInstance-index)/parentCount == uParentInstance);
				}
				else
					result = HasParent(
						pMyNode->GetParentNR(i), (uMyInstance-index)/parentCount, 
						pParentNode, uParentInstance);
				break;
			}
			index = index2;
		}
		i += parentCount-1;
	}

	return result;
}

/**
	Add parent to child.

	@param	IFXNode* pNewParentNode - parent to add it

	@result	IFXRESULT

	@note	fucntion adds parent to IFXParent array. In case if this Node 
			wasn't already parented by pNewParentNode it is adding to
			m_pChildren array.
			Spatial setting.
*/
IFXRESULT CIFXNode::AddParent(IFXNode* pNewParentNode)
{
	IFXRESULT rc = IFX_OK;
	IFXNode* pThisNode = NULL;
	IFXNode* pParentNodeNR = NULL;
	U32 nParents;

	if (NULL == m_pSceneGraph || NULL == pNewParentNode)
		rc = IFX_E_NOT_INITIALIZED;

	// we need to walk up the graph to make sure we aren't forming
	// a cyclic link by taking this parent assignment.

	// get our own IFXNode handle:
	if(IFXSUCCESS(rc))
		rc = this->QueryInterface( IID_IFXNode, (void**)&pThisNode);

	if(IFXSUCCESS(rc))
	{
		if (!IsThisNode(pNewParentNode, pThisNode)) 
		{
			nParents = this->GetNumberOfParents();
			U32 i;
			for ( i = 0; i < nParents; i++) 
			{
				pParentNodeNR = this->GetParentNR(i);
				if (pParentNodeNR == pNewParentNode)
					break;
			}

			// No AddRef() here to keep from cyclic reference.
			U32 numelements = m_Parents.GetNumberElements();
			IFXParent& NewElement = m_Parents.CreateNewElement();
			NewElement.pParentNR = pNewParentNode;
			NewElement.uParentsModChainsTransformAspectBit = (U32)-1;
			NewElement.pParentDataPacketAsSubjectNR = NULL;

			//now we need to create and initialize local and world 
			//matrices for each instance of node

			//we already have the first local identity matrix
			if (numelements > 0) 
			{
				IFXMatrix4x4& NewLocal = m_Local.CreateNewElement();
				NewLocal.MakeIdentity();
			}

			IFXMatrix4x4& NewWorld = m_World.CreateNewElement();
			NewWorld.MakeIdentity();

			// Start observing the new parent's modifier chain's
			// transform data element.
			AttachToParentsWorldTransform(numelements);

			if( m_pModifierDataPacket)
				m_pModifierDataPacket->InvalidateDataElement( 
											m_uMyDataPacketTransformIndex );

			if (IFXSUCCESS(rc) && pParentNodeNR != pNewParentNode) 
			{
				IFXSceneGraph *pParentSG = NULL;
				IFXMarker* pMarker;

				// disallow assignments across scenegraphs
				if( IFXSUCCESS(rc)) 
					rc = pNewParentNode->QueryInterface(
											IID_IFXMarker, (void**)&pMarker);

				if( IFXSUCCESS(rc)) 
				{
					rc = pMarker->GetSceneGraph(&pParentSG);
					IFXRELEASE( pMarker );
				}

				if  ((pParentSG != m_pSceneGraph) && (IFXSUCCESS(rc)))
					rc = IFX_E_INVALID_POINTER ;

				IFXRELEASE( pParentSG );

				// Attatch the node to the parent
				if( IFXSUCCESS(rc))
					rc = pNewParentNode->AddChild(pThisNode);

				IFXSpatial** pSpatials[IFXSpatial::TYPE_COUNT];
				U32 nSpatials[IFXSpatial::TYPE_COUNT];
				U32 i;

				for ( i = 0; i<IFXSpatial::TYPE_COUNT && IFXSUCCESS(rc); i++ )
				{
					rc = GetSpatials( pSpatials[i], nSpatials[i], (IFXSpatial::eType)i );

					if(IFXSUCCESS(rc) && nSpatials[i])
						rc = pNewParentNode->AddSpatials( 
												pSpatials[i], nSpatials[i], 
												(IFXSpatial::eType)i );
				}
			}

			// Do not Release pParentNode
			// pParentNode will be either NULL or equal to thisNode without an AddRef

			IFXRELEASE( pThisNode );
			IFXASSERT( IFXSUCCESS(rc) );
		}
	}

	return rc;
}

/**
	get number of parents.

	@result	U32 - real number of parents even if child more than
			one time is parented by one of the parent
*/
U32 CIFXNode::GetNumberOfParents()
{
	return m_Parents.GetNumberElements();
}

/**
	get parent by index

	@param	const U32 ParentIndex - index of element in parent's array

	@return	IFXNode* - required parent node

	@note	no references
*/
IFXNode* CIFXNode::GetParentNR(const U32 ParentIndex)
{
	IFXNode* pParentNodeNR = NULL;

	if (GetNumberOfParents() > ParentIndex) 
	{
		IFXParent& Parent = m_Parents.GetElement(ParentIndex);
		pParentNodeNR = Parent.pParentNR;
	}

	return pParentNodeNR;
}

IFXRESULT CIFXNode::GenerateOutput( U32    inOutputDataElementIndex,
								   void*& rpOutData, BOOL& rNeedRelease )
{
	IFXRESULT result = IFX_OK;
	IFXNode* parentNR = NULL;

	if( inOutputDataElementIndex == m_uMyDataPacketTransformIndex ) 
	{
		U32 numParents = 0, NPi = 0;
		numParents = GetNumberOfParents();
		if (numParents != 0) 
		{
			U32 i;
			for ( i = 0; i < numParents && IFXSUCCESS(result); i++) 
			{
				parentNR = GetParentNR(i);

				// Update the parent transform if needed.
				if(parentNR) 
				{
					IFXMatrix4x4* pParentsTransform = NULL;
					U32 NP = 0;
					IFXArray<IFXMatrix4x4> *pAParentsTransform;

					result = parentNR->GetWorldMatrices( &pAParentsTransform );
					
					if(IFXSUCCESS(result))
						NP = pAParentsTransform->GetNumberElements();

					while (NP-- && IFXSUCCESS(result)) 
					{
						pParentsTransform = &pAParentsTransform->GetElement(NP);
						if(NULL != pParentsTransform)
						{
							if (m_World.GetNumberElements() < NPi+1) 
							{
								IFXMatrix4x4& NewWorld = m_World.CreateNewElement();
								NewWorld.MakeIdentity();
							}

							m_World[NPi].Multiply( *pParentsTransform, m_Local[i]);
							NPi++;
						}
						else
							result = IFX_E_INVALID_POINTER;
					}
				}
			}
			if(IFXSUCCESS(result))
				rpOutData = (void*)&(m_World);
		} 
		else 
		{
			rpOutData = (void*)&(m_Local);
			result = IFX_OK;
		}
	}

	return result;
}

IFXRESULT CIFXNode::SetDataPacket(IFXModifierDataPacket* pInInputDataPacket,
								  IFXModifierDataPacket* pInOutputDataPacket)
{
	IFXRESULT result = IFX_OK;

	if( m_pMyDataPacketAsSubjectNR )
	{
		result = m_pMyDataPacketAsSubjectNR->Detach( m_pMyIFXObserverNR );
		m_pMyDataPacketAsSubjectNR = NULL;
		IFXASSERT( IFXSUCCESS( result ) );
	}

	IFXRELEASE( m_pModifierDataPacket );
	IFXRELEASE( m_pInputDataPacket );

	if( pInOutputDataPacket && pInInputDataPacket && IFXSUCCESS( result ))
	{
		result = pInOutputDataPacket->QueryInterface( 
											IID_IFXSubject, 
											(void**)&m_pMyDataPacketAsSubjectNR );
		if (IFXSUCCESS( result ))
			result = pInOutputDataPacket->GetDataElementAspectBit( 
											DID_IFXTransform,
											m_uMyDataPacketTransformAspectBit );

		// I want to know if my transform changes:
		if (IFXSUCCESS( result ))
			result = m_pMyDataPacketAsSubjectNR->Attach( 
											m_pMyIFXObserverNR, 
											m_uMyDataPacketTransformAspectBit );

		if (IFXSUCCESS( result ))
		{
			m_pModifierDataPacket = pInOutputDataPacket;

			pInInputDataPacket->AddRef();
			m_pInputDataPacket = pInInputDataPacket;
		}
	}

	if( pInOutputDataPacket && pInInputDataPacket && IFXSUCCESS(result) )
	{
		result = pInOutputDataPacket->GetDataElementIndex( DID_IFXTransform,
			m_uMyDataPacketTransformIndex );
	}

	U32 parentIndex;
	for (parentIndex = 0; parentIndex < m_Parents.GetNumberElements() && IFXSUCCESS(result); parentIndex++)
		result = AttachToParentsWorldTransform(parentIndex);

	return result;
}

IFXRESULT CIFXNode::AttachToParentsWorldTransform(U32 parentIndex)
{
	IFXRESULT result = IFX_OK;
	IFXDECLARELOCAL(IFXModifierChain, pModifierChain);
	IFXDECLARELOCAL(IFXModifierDataPacket, pParentMDP);

	if (m_Parents.GetNumberElements()) 
	{
		IFXParent& Element = m_Parents.GetElement(parentIndex);

		// Stop observing the parent's modifier chain's transform data element.
		if( Element.pParentDataPacketAsSubjectNR )
		{
			result = Element.pParentDataPacketAsSubjectNR->Detach( m_pMyIFXObserverNR );
			Element.pParentDataPacketAsSubjectNR = NULL;

			IFXASSERT(IFXSUCCESS(result));
		}

		if (IFXSUCCESS(result)) 
		{
			if (NULL != Element.pParentNR)
				result = Element.pParentNR->GetModifierChain( &pModifierChain );
			else
				result = IFX_W_FINISHED;

			if( IFXSUCCESS(result) && result != IFX_W_FINISHED) 
			{
				result = pModifierChain->GetDataPacket( pParentMDP );

				if( IFXSUCCESS(result) ) 
				{
					Element.uParentsModChainsTransformAspectBit = (U32)-1;

					result = pParentMDP->GetDataElementAspectBit(
						DID_IFXTransform, Element.uParentsModChainsTransformAspectBit );

					if( IFXSUCCESS(result) ) 
					{
						result = pParentMDP->QueryInterface( 
									IID_IFXSubject,
									(void**)&Element.pParentDataPacketAsSubjectNR );

						IFXASSERT(IFXSUCCESS(result));

						result = Element.pParentDataPacketAsSubjectNR->Attach( 
									m_pMyIFXObserverNR,
									Element.uParentsModChainsTransformAspectBit,
									IID_IFXDataPacket );

						Element.pParentDataPacketAsSubjectNR->Release();
					}
				}
			}
		}
	} 
	else
		result = IFX_E_INVALID_RANGE;

	return result;
}

IFXRESULT CIFXNode::InvalidateMatrix()
{
	IFXRESULT rc = IFX_OK;

	if( m_pModifierDataPacket )
	{
		rc = m_pModifierDataPacket->InvalidateDataElement( 
											m_uMyDataPacketTransformIndex );

		IFXASSERT( IFXSUCCESS( rc ) );
	}

	return rc;
}


IFXRESULT CIFXNode::MarkMotions()
{
	IFXRESULT result = IFX_OK;

	IFXModifierChain *pModChain = NULL;
	IFXPalette *pIFXMotionPalette = NULL;

	result = m_pSceneGraph->GetPalette(IFXSceneGraph::MOTION, &pIFXMotionPalette);

	if( IFXSUCCESS(result) )
		result = GetModifierChain( &pModChain );

	if( IFXSUCCESS(result) )
	{
		IFXAnimationModifier* pBonesMod = NULL;
		IFXModifier*     pModifier = NULL;
		U32               uNumModifiers = 0;
		U32               i;

		pModChain->GetModifierCount( uNumModifiers );

		// loop through all the modifiers, searching for playlists
		for(i=1; i<uNumModifiers && IFXSUCCESS(result); i++)
		{
			result = pModChain->GetModifier( i, pModifier );

			if( IFXSUCCESS(result) )
				pModifier->QueryInterface( IID_IFXAnimationModifier,(void**)&pBonesMod );

			if( pBonesMod )
			{
				// get the name of each mixer
				U32 numQd = pBonesMod->GetNumberQueued();

				U32 index;
				for( index=0; index < numQd; index++ )
				{
					IFXMotionMixer *pMixer;
					pMixer = pBonesMod->GetMotionMixerNR( index );

					IFXString mixername = pMixer->GetPrimaryMotionName();

					// Use the Mixer name as a motion name:
					// should parse the mixers and mark them too, but that is hairy
					// instead only copy the motions of fully mapped mixers of the same
					// name
					U32 uID;
					result = pIFXMotionPalette->Find(&mixername,&uID);

					if (IFXSUCCESS(result))
					{
						IFXUnknown *pIFXUnkown = 0;
						result = pIFXMotionPalette->GetResourcePtr(uID, &pIFXUnkown);
						if (pIFXUnkown)
						{
							IFXMarker *pIFXMarker = 0;
							result = pIFXUnkown->QueryInterface(
													IID_IFXMarker, (void**)&pIFXMarker);

							if (pIFXMarker)
							{
								pIFXMarker->Mark();
								IFXRELEASE(pIFXMarker);
							} // end if got a marker interface

							IFXRELEASE(pIFXUnkown);

						} // end if got an "Unknown" pointer to a motion
					} // end if found a motion corresponding to the name in the playlist
				}  // end loop through playlist
			}

			IFXRELEASE( pModifier );
			IFXRELEASE( pBonesMod );
		}
	}

	IFXRELEASE( pIFXMotionPalette );
	IFXRELEASE( pModChain );

	return result;
}

IFXRESULT CIFXNode::Notify( IFXModifierMessage eInMessage,
									void* pMessageContext )
{
	return IFX_OK;
}
