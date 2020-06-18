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


#ifndef IFXTQTBaseTriangle_DOT_H
#define IFXTQTBaseTriangle_DOT_H

#include "IFXDeque.h"
#include "IFXTQTTriangle.h"

// Typedef a Breadth First Traversal (BFT) Deque:
class IFXTQTTriangle;
//typedef std::deque<IFXTQTTriangle *> IFXBFTDeque; // remove stl dependency.
typedef IFXDeque<IFXTQTTriangle *> IFXBFTDeque;



class IFXTQTBaseTriangle : public IFXTQTTriangle
{

private:

	// Base mesh triangle neighbor data:
	// Constructed from input such as IFXMesh/IFXMeshGroup/IFXNeighborMesh:
	IFXTQTBaseTriangle *m_pNeighbor[3];
    IFXEdgeContinuityDescriptor m_pEdgeDescriptor[3];

    // Breadth First traversal support:
    IFXBFTDeque m_BftDeque;

	// Support for multi-mesh meshgroups. Index indicates to which mesh this 
	// triangle and all its subtriangle belong.
	U32	m_uMeshIndex;

public:
	
	IFXTQTBaseTriangle (){};
	~IFXTQTBaseTriangle ()	{};

	void SetBaseNeighbors (IFXTQTBaseTriangle *pLeftNeighbor, 
		IFXTQTBaseTriangle *pBaseNeighbor, IFXTQTBaseTriangle *pRightNeighbor);
	void GetBaseNeighbor (IFXTQTAddress::Direction usDirection, 
		IFXTQTBaseTriangle	**ppBaseNeighbor, IFXTQTAddress::Direction *pOrientation);

	void SetMeshIndex (U32 uMeshIndex);
	void GetMeshIndex (U32 *puMeshIndex);

    void BreadthFirstEvaluate (IFXSubdivisionManager *pSubdivMgr);
    void ResetAll(IFXSubdivisionManager *pSubdivMgr);

    IFXEdgeContinuityDescriptor *GetEdgeDescriptor(IFXTQTAddress::Direction direction);


	// For Debug:
	U16	m_usId;
};




IFXINLINE void IFXTQTBaseTriangle::GetBaseNeighbor (IFXTQTAddress::Direction usDirection, 
				IFXTQTBaseTriangle	**ppBaseNeighbor, IFXTQTAddress::Direction *pOrientation)
{
	// Grab the base neighbor pointer:
	*ppBaseNeighbor = m_pNeighbor[usDirection];

    if (!*ppBaseNeighbor)
        return;
        
	
	// Determine orientation offset, if neighbor pointer is valid:
	if (ppBaseNeighbor)
	{
		if (this == (*ppBaseNeighbor)->m_pNeighbor[IFXTQTAddress::Left])
			*pOrientation = IFXTQTAddress::Left;
		else if (this == (*ppBaseNeighbor)->m_pNeighbor[IFXTQTAddress::Base])
			*pOrientation = IFXTQTAddress::Base;
		else if (this == (*ppBaseNeighbor)->m_pNeighbor[IFXTQTAddress::Right])
			*pOrientation = IFXTQTAddress::Right;
	}
}



IFXINLINE 	void IFXTQTBaseTriangle::SetBaseNeighbors (IFXTQTBaseTriangle *pLeftNeighbor, IFXTQTBaseTriangle *pBaseNeighbor, IFXTQTBaseTriangle *pRightNeighbor)
{
	m_pNeighbor[IFXTQTAddress::Left]	= pLeftNeighbor;
	m_pNeighbor[IFXTQTAddress::Base]	= pBaseNeighbor;
	m_pNeighbor[IFXTQTAddress::Right]	= pRightNeighbor;
}

IFXINLINE IFXEdgeContinuityDescriptor *IFXTQTBaseTriangle::GetEdgeDescriptor(IFXTQTAddress::Direction direction)
{
    return &m_pEdgeDescriptor[direction];
}


IFXINLINE void IFXTQTBaseTriangle::SetMeshIndex (U32 uMeshIndex)
{
	m_uMeshIndex = uMeshIndex;
}

IFXINLINE void IFXTQTBaseTriangle::GetMeshIndex (U32 *puMeshIndex)
{
	*puMeshIndex = m_uMeshIndex;
}

#endif
