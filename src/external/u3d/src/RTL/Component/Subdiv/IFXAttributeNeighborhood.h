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
#ifndef IFXATTRIBUTENEIGHBORHOOD_DOT_H
#define IFXATTRIBUTENEIGHBORHOOD_DOT_H

#include "IFXTQTAddress.h"
#include "IFXTQTVertex.h"
#include <memory.h>

// Forward declarations:
class IFXTQTTriangle;


#define BUTTERFLY_MASK_SIZE 10
typedef struct
{
    F32                 *m_ppValues[BUTTERFLY_MASK_SIZE];
} IFXAttributeLayout;

/**
	IFXAttributeNeighborhood is a container class that temporarily collects
	of all of the attribute inputs for subdivision computation at a single midpoint.  
	It is created and written by ButterflyScheme::GatherEdgeNeighborhood() 
	and used to marshall the inputs to the subdivision computation.
*/
class IFXAttributeNeighborhood
{
public:
    void Reset();
    void PointAt(U32 uIndex, IFXTQTVertex *pVertex, IFXTQTTriangle *pTriangle = NULL, 
        IFXTQTAddress *pAddress = NULL, 
		IFXTQTAddress::Direction uDirection = IFXTQTAddress::UnDefined, 
        IFXTQTLocality locality = UndefinedTriangle);
	IFXTQTAddress::Direction ReOrient (U32 uIndex, IFXTQTAddress::Direction uDirection);

    // Accessors:
    bool VertexPresent (U32 uIndex);
    IFXTQTTriangle* GetTriangle(U32 uIndex);

    IFXTQTVertex* GetMidpoint();
    void SetMidpoint(IFXTQTVertex *pVertex);

	bool Discontinuous();
	void MarkDiscontinuous(bool bDiscon = true);

    F32 * GetMidpointPosition();
    F32 * GetMidpointNormal();
    F32 * GetMidpointTexCoord();

    F32 **GetPositionMask();
    F32 **GetNormalMask();
    F32 **GetTexCoordMask();

private:
    
    // Temporary Neighbor Info:
    IFXTQTTriangle				*m_ppTriangle	[BUTTERFLY_MASK_SIZE];
    IFXTQTAddress				m_neighAddr		[BUTTERFLY_MASK_SIZE];
    IFXTQTAddress::Direction    m_uReOrientation[BUTTERFLY_MASK_SIZE];

    // The Attribute pointers (The actual mask data):
    IFXAttributeLayout      m_position;
    IFXAttributeLayout      m_normal;
    IFXAttributeLayout      m_texCoord;
	bool					m_bDiscontinuous;

	// The Subdivision results:
    IFXTQTVertex            *m_pMidpoint;

    // Implementation methods:
    void CheckForDiscontinuities(U32 uIndex, IFXTQTTriangle *pTriangle, 
		IFXTQTAddress::Direction uReOreintedDirection);
};

IFXINLINE bool IFXAttributeNeighborhood::Discontinuous()
{
	return m_bDiscontinuous;
}
	
IFXINLINE void IFXAttributeNeighborhood::MarkDiscontinuous(bool bDiscon)
{
	m_bDiscontinuous = bDiscon;
}


IFXINLINE void IFXAttributeNeighborhood::Reset()
{
    // NULL out all pointers:
    memset (m_position.m_ppValues, 0, sizeof (F32 *) * BUTTERFLY_MASK_SIZE);
    memset (m_normal.m_ppValues, 0, sizeof (F32 *) * BUTTERFLY_MASK_SIZE);
    memset (m_texCoord.m_ppValues, 0, sizeof (F32 *) * BUTTERFLY_MASK_SIZE);
    m_pMidpoint = NULL;
	m_bDiscontinuous = false;
}


// Copy out pointers to individual vertex attributes:
IFXINLINE void IFXAttributeNeighborhood::PointAt(U32 uIndex, IFXTQTVertex *pVertex, 
                                              IFXTQTTriangle *pTriangle, 
                                              IFXTQTAddress *pAddress, 
                                              IFXTQTAddress::Direction uReOreintedDirection,
                                              IFXTQTLocality locality)
{
    if (pVertex)
    {
        m_position.m_ppValues[uIndex]   = (F32 *) &pVertex->m_position;
        m_normal.m_ppValues[uIndex]     = (F32 *) &pVertex->m_normal;
        m_texCoord.m_ppValues[uIndex]   = (F32 *) &pVertex->m_texCoord;
    }

    if (pTriangle && pAddress)
    {
        // Look for attribute disconinuities:
        if (locality == DistalTriangle)
            CheckForDiscontinuities(uIndex, pTriangle, uReOreintedDirection);

        m_ppTriangle[uIndex] = pTriangle;
        m_neighAddr[uIndex] = *pAddress;
        m_uReOrientation[uIndex] = uReOreintedDirection;
    }
}


IFXINLINE bool IFXAttributeNeighborhood::VertexPresent (U32 uIndex)
{
    if (m_position.m_ppValues[uIndex])
        return true;
    else
        return false;
}

IFXINLINE IFXTQTTriangle *IFXAttributeNeighborhood::GetTriangle(U32 uIndex)
{
    return this->m_ppTriangle[uIndex];
}




IFXINLINE IFXTQTAddress::Direction IFXAttributeNeighborhood::ReOrient (U32 uIndex, 
				IFXTQTAddress::Direction uDirection)
{
    return (IFXTQTAddress::m_usReOrient[m_uReOrientation[uIndex] + uDirection]);
}


IFXINLINE IFXTQTVertex* IFXAttributeNeighborhood::GetMidpoint()
{
    return m_pMidpoint;
}

IFXINLINE void IFXAttributeNeighborhood::SetMidpoint(IFXTQTVertex *pVertex)
{
    m_pMidpoint = pVertex;
}


IFXINLINE F32 * IFXAttributeNeighborhood::GetMidpointPosition()
{
    return ((F32 *) &m_pMidpoint->m_position);
}

IFXINLINE F32 * IFXAttributeNeighborhood::GetMidpointNormal()
{
    return ((F32 *) &m_pMidpoint->m_normal);
}

IFXINLINE F32 * IFXAttributeNeighborhood::GetMidpointTexCoord()
{
    return ((F32 *) &m_pMidpoint->m_texCoord);
}

IFXINLINE F32 **IFXAttributeNeighborhood::GetPositionMask()
{
    return (this->m_position.m_ppValues);
}

IFXINLINE F32 **IFXAttributeNeighborhood::GetNormalMask()
{
    return (this->m_normal.m_ppValues);
}

IFXINLINE F32 **IFXAttributeNeighborhood::GetTexCoordMask()
{
    return (this->m_texCoord.m_ppValues);
}

#endif
