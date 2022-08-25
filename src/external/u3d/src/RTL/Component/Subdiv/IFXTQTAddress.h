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

#ifndef IFXTQTAddress_DOT_H
#define IFXTQTAddress_DOT_H

#include "IFXResult.h"
#include "IFXDataTypes.h"

/** 
	@file	Triangular Quad Tree Address.

	IFXTQTAddress implements a triangle address labeling scheme based
	upon the work of Hanan Samet and company.  The advantage is that
	neighbor triangles can be computed in constant time via simple
	bitwise operations.  The technique has been extended to work with 
	top level meshes that have arbitrary triangulations.  This implies
	that another data structure (winged-edge, DTS, IFXNeighborMesh, etc) 
	defines the top-level neighbor relationships. My neighbor finding 
	extensions are also bitwise operations and constant time.
*/

// Forward declarations:
class IFXTQTAddress;


// Bitwise masks:
#define TQT_LEAST_SIGNIFICANT_PAIR	0x00000003	// 00000000.....11
// Samet's naming convention for EVEN/ODD is is a little weird...defined 
// over "one based" bit indexing instead of C-style "zero based."
#define TQT_ODD_BIT_MASK			0xAAAAAAAA	// 10101010...1010
#define TQT_EVEN_BIT_MASK			0x55555555	// 01010101...0101


// Enumerations
typedef enum {TQT_00, TQT_01, TQT_10, TQT_11, TQT_NoLabel}	IFXTQTTriangleLabel;
typedef enum {UndefinedTriangle = 0, LocalTriangle = 1, DistalTriangle = 2} IFXTQTLocality;

class IFXTQTAddress
{

public:

	// Enumerations:
	enum Direction			{Left, Base, Right, UnDefined = 0xFFFF};

	IFXTQTAddress();
	IFXTQTAddress(const IFXTQTAddress &rAddress);
	~IFXTQTAddress();

	// Address construction:
	void	Reset					();
	void	PushLabel				(IFXTQTTriangleLabel label);
	void	PopLabel				();
	U32	Address					();
	U32	Length					(); // Returns number of bit pairs (tree depth).  Base mesh is 0
	void	Assign					(IFXTQTAddress *pAddress);
	U32	GetIthPair				(U32 usIth);

	// Neighbor finding operations:
	bool	LocalNeighbor	(Direction direction, IFXTQTAddress *pAddress);
	void	DistalNeighbor	(Direction localDir, Direction distalDir, IFXTQTAddress *pAddress);
	bool	Faulted			();

    static Direction m_usReOrient[6];
	

private:
		
	U32	m_dwAddress;
	U32	m_usLength;		// # of bit pairs (This is also tree depth);

	// Mask generation methods:
	void MakeMask_00Id11	(U32 *pMask00Id11);
	void MakeMask_01Id11	(U32 *pMask01Id11);
	void MakeMask_10Id11	(U32 *pMask10Id11);
	void MakeMask_11Id11	(U32 *pMask11Id11);

	void MakeMask_01Id01	(U32 *pMask01Id01);
	void MakeMask_X0Id10	(U32 *pMaskX0Id10);

	// Local Neighbor Methods:
	void RightNeighbor		(IFXTQTAddress *pNeighborAddr);
	void LeftNeighbor		(IFXTQTAddress *pNeighborAddr);
	void BaseNeighbor		(IFXTQTAddress *pNeighborAddr);

	// Distal Neighbor Methods:
	void LeftLeft	(IFXTQTAddress *pNeighbor);
	void LeftRight	(IFXTQTAddress *pNeighbor);
	void LeftBase	(IFXTQTAddress *pNeighbor);
	void RightLeft	(IFXTQTAddress *pNeighbor);
	void RightRight	(IFXTQTAddress *pNeighbor);
	void RightBase	(IFXTQTAddress *pNeighbor);
	void BaseLeft	(IFXTQTAddress *pNeighbor);
	void BaseRight	(IFXTQTAddress *pNeighbor);
	void BaseBase	(IFXTQTAddress *pNeighbor);

};


IFXFORCEINLINE IFXTQTAddress::IFXTQTAddress()
{
	Reset();
}

IFXFORCEINLINE IFXTQTAddress::IFXTQTAddress(const IFXTQTAddress &rAddress)
{
	m_dwAddress	= rAddress.m_dwAddress;
	m_usLength	= rAddress.m_usLength;
}

IFXFORCEINLINE IFXTQTAddress::~IFXTQTAddress()
{
}


IFXFORCEINLINE void IFXTQTAddress::Reset()
{
	m_dwAddress = 0x00000000;
	m_usLength	= 0;
}

IFXFORCEINLINE void IFXTQTAddress::PushLabel(IFXTQTTriangleLabel label)
{
	m_dwAddress <<= 2;
	m_dwAddress |= label;
	m_usLength ++;
}

IFXFORCEINLINE void IFXTQTAddress::PopLabel()
{
	m_dwAddress >>= 2;
	m_usLength --;
}

IFXFORCEINLINE U32 IFXTQTAddress::Length()
{
	return m_usLength;
}

IFXFORCEINLINE U32 IFXTQTAddress::Address()
{
	return m_dwAddress;
}

IFXFORCEINLINE void IFXTQTAddress::Assign(IFXTQTAddress *pAddress)
{
	m_dwAddress	= pAddress->m_dwAddress;
	m_usLength	= pAddress->m_usLength;
}



IFXFORCEINLINE void IFXTQTAddress::MakeMask_00Id11(U32 *pMask00Id11)
{
	// Identify all 00s:
	(*pMask00Id11)	=	(m_dwAddress >> 1) | m_dwAddress;
	(*pMask00Id11)	^=	TQT_EVEN_BIT_MASK;					// XOR with even bits
	(*pMask00Id11)	&=	TQT_EVEN_BIT_MASK;					// AND with even bits

	// Duplicate bits in 00Id11:
	(*pMask00Id11)	|=	((*pMask00Id11) << 1);

	// Clean up 11 masks for leading 00s: 
	unsigned short uShift = 32 - (m_usLength*2);
	(*pMask00Id11) <<= uShift;
	(*pMask00Id11) >>= uShift;
}


IFXFORCEINLINE void IFXTQTAddress::MakeMask_01Id11(U32 *pMask01Id11)
{
	// Identify all 01's:
	(*pMask01Id11) =	(~m_dwAddress) & TQT_ODD_BIT_MASK;
	(*pMask01Id11) >>=	1;
	(*pMask01Id11) &=	m_dwAddress;
	(*pMask01Id11) |=	(*pMask01Id11) << 1;
}

IFXFORCEINLINE void IFXTQTAddress::MakeMask_10Id11(U32 *pMask10Id11)
{
	// Identify all 10's:
	(*pMask10Id11)	=	(~m_dwAddress) & TQT_EVEN_BIT_MASK;
	(*pMask10Id11)	<<=	1;
	(*pMask10Id11)	&=	m_dwAddress;
	(*pMask10Id11)	|=	(*pMask10Id11)>>1;
}

IFXFORCEINLINE void IFXTQTAddress::MakeMask_11Id11(U32 *pMask11Id11)
{
	// Identify all 11's:
	(*pMask11Id11)	=	m_dwAddress >> 1;
	(*pMask11Id11)	&=	m_dwAddress;
	(*pMask11Id11)	&=	TQT_EVEN_BIT_MASK;
	(*pMask11Id11)	|=	(*pMask11Id11) << 1;
}

IFXFORCEINLINE void IFXTQTAddress::MakeMask_01Id01(U32 *pMask01Id01)
{	
	// Identify all 01's:
	(*pMask01Id01) = (~m_dwAddress) & TQT_ODD_BIT_MASK;
	(*pMask01Id01) >>= 1;
	(*pMask01Id01) &= m_dwAddress;
}


IFXFORCEINLINE void IFXTQTAddress::MakeMask_X0Id10(U32 *pMaskX0Id10)
{
	// Copy input address
	*pMaskX0Id10 = m_dwAddress;
	
	// Identify all X0's's:							
	*pMaskX0Id10 = (~(*pMaskX0Id10)) & TQT_EVEN_BIT_MASK;		// COMPLEMENT & with EVEN		
	(*pMaskX0Id10) <<= 1;									// SHIFT left
}



IFXFORCEINLINE void IFXTQTAddress::RightNeighbor(IFXTQTAddress *pNeighborAddr)
{
	U32 mask00Id11;

	// Initialize:
	pNeighborAddr->Assign(this);

	// Make our 00s mask:
	MakeMask_00Id11(&mask00Id11);

	// Change all 00s to 11s:
	pNeighborAddr->m_dwAddress	^=	mask00Id11;
	
	// Add 1 (move right):
	pNeighborAddr->m_dwAddress	++;

	// Restore unchanged 00s:
	pNeighborAddr->m_dwAddress	^=	mask00Id11;

	// Fixup 00s that got hit with at carry:
	pNeighborAddr->m_dwAddress	&=	((~mask00Id11) | TQT_EVEN_BIT_MASK);
}



IFXFORCEINLINE void IFXTQTAddress::LeftNeighbor(IFXTQTAddress *pNeighborAddr)
{
	U32 mask01Id01;

	// Initialize:
	pNeighborAddr->Assign(this);

	// Make our 01s mask and apply it:
	MakeMask_01Id01 (&mask01Id01);
	pNeighborAddr->m_dwAddress	^=	mask01Id01;

	// Subtract 1 (move left)
	pNeighborAddr->m_dwAddress	--;
	
	// Restore unchanged 01s:
	pNeighborAddr->m_dwAddress	^=	mask01Id01;

	// Fixup 01's that got hit with a borrow:
	pNeighborAddr->m_dwAddress	&= ~(mask01Id01 << 1);
}



IFXFORCEINLINE void IFXTQTAddress::BaseNeighbor(IFXTQTAddress *pNeighborAddr)
{
	U32 maskX0Id10, mask;

	// Initialize:
	pNeighborAddr->Assign(this);

	// Make our masks:
	MakeMask_X0Id10 (&maskX0Id10);
	mask	=	~maskX0Id10;
	mask	++;
	mask	&=	maskX0Id10;
	pNeighborAddr->m_dwAddress ^= mask;
}


// Address faults are signaled by any non-zero carry bits to
// the left of the valid address bits.  Knowing the length of
// the valid bits, we simply right shift by that amount and check
// for non-zero bits:
IFXFORCEINLINE bool IFXTQTAddress::Faulted()
{
	if (m_dwAddress >> (m_usLength*2))
		return true;
	else
		return false;
}


// Returns true if an address fault occurs.
IFXFORCEINLINE bool IFXTQTAddress::LocalNeighbor(Direction direction, IFXTQTAddress *pNeighborAddr)
{
	switch (direction)
	{
	case Left:	LeftNeighbor(pNeighborAddr);
				break;

	case Right:	RightNeighbor(pNeighborAddr);
				break;

	case Base:	BaseNeighbor(pNeighborAddr);
				break;

	case UnDefined:
				break;
	}

	// Check to see if an address fault was computed:
	if (pNeighborAddr->Faulted())
		return true;
	else
		return false;
}



// DistalNeighor() computes a neighbor address for a neighbor triangle that
// is a child of "top-level" neighbor.  The top level neighbor is likely identified
// with a winged-edge (or DTSMESH) style neighbor data structure.  DistalNeighbor()
// is likely only invoked in the case where a call to LocalNeighbor() generated a
// IFXTQTAddress fault, thus signaling a non-local neighbor.
//
// DistalNeighbor() computes a neighbor address based on the relative orientations 
// of the two "top level" triangles.  This is indicated by the local direction for
// neighbor we are seeking and the corresponding direction in the "top-level" 
// neighbor which is directly opposite.  It is assumed that all top-level neighbor
// triangles are by convention named initially according to Samet's "Tip Up" triangle
// naming convention.  In other words, the root triangles are always named according
// to the "Tip Up" labelling scheme

IFXFORCEINLINE void IFXTQTAddress::DistalNeighbor (Direction localDir, Direction distalDir, IFXTQTAddress *pNeighbor)
{
	switch	(localDir)
	{
	case Left:		
		switch (distalDir)
		{
		case Left:		LeftLeft	(pNeighbor); return;
		case Right:		LeftRight	(pNeighbor); return;
		case Base:		LeftBase	(pNeighbor); return;
		case UnDefined: break;
		}
		break;

	case Right:		
		switch (distalDir)
		{
		case Left:		RightLeft	(pNeighbor); return;
		case Right:		RightRight	(pNeighbor); return;
		case Base:		RightBase	(pNeighbor); return;
		case UnDefined: break;
		}
		break;

	case Base:		
		switch (distalDir)
		{
		case Left:		BaseLeft	(pNeighbor); return;
		case Right:		BaseRight	(pNeighbor); return;
		case Base:		BaseBase	(pNeighbor); return;
		case UnDefined: break;
		}
		break;

	case UnDefined: 
		break;
	}
}


IFXFORCEINLINE void IFXTQTAddress::LeftLeft (IFXTQTAddress *pNeighbor)
{
	U32 mask00Id11, mask01Id11;
	
	MakeMask_00Id11 (&mask00Id11);	
	MakeMask_01Id11	(&mask01Id11);

	pNeighbor->Assign(this);

	// Change all 01s -> 00s:
	pNeighbor->m_dwAddress	&=	(~mask01Id11);

	// Change all original 00s -> 01:
	pNeighbor->m_dwAddress	|=	(mask00Id11 & TQT_EVEN_BIT_MASK);
}


IFXFORCEINLINE void IFXTQTAddress::LeftRight (IFXTQTAddress *pNeighbor)
{
	U32 mask01Id11;

	MakeMask_01Id11 (&mask01Id11);
	
	// 01 -> 11:
	pNeighbor->Assign(this);
	pNeighbor->m_dwAddress |= mask01Id11;
}



IFXFORCEINLINE void IFXTQTAddress::LeftBase (IFXTQTAddress *pNeighbor)
{
	U32 mask00Id11;

	MakeMask_00Id11 (&mask00Id11);
	
	// 00 -> 11:
	pNeighbor->Assign(this);
	pNeighbor->m_dwAddress |= mask00Id11;
}



IFXFORCEINLINE void IFXTQTAddress::RightLeft (IFXTQTAddress *pNeighbor)
{
	U32 mask11Id11;

	MakeMask_11Id11 (&mask11Id11);

	// 11 -> 01:
	pNeighbor->Assign(this);
	pNeighbor->m_dwAddress	&=	~mask11Id11;						// Replace 11s with 00s
	pNeighbor->m_dwAddress	|=	(mask11Id11 & TQT_EVEN_BIT_MASK);	// Replace with 01s
}


IFXFORCEINLINE void IFXTQTAddress::RightRight (IFXTQTAddress *pNeighbor)
{
	U32 mask11Id11, mask00Id11, temp;

	MakeMask_11Id11 (&mask11Id11);
	MakeMask_00Id11 (&mask00Id11);

	// 11 -> 00:
	temp	=	m_dwAddress & ~mask11Id11;	

	// Combine with other mask, 00 -> 11:
	pNeighbor->Assign(this);
	pNeighbor->m_dwAddress = temp | mask00Id11;
}


IFXFORCEINLINE void IFXTQTAddress::RightBase (IFXTQTAddress *pNeighbor)
{
	U32 mask00Id11;

	MakeMask_00Id11(&mask00Id11);

	// 00 -> 01:
	pNeighbor->Assign(this);
	pNeighbor->m_dwAddress	|=	(mask00Id11	& TQT_EVEN_BIT_MASK);
}


IFXFORCEINLINE void IFXTQTAddress::BaseLeft (IFXTQTAddress *pNeighbor)
{
	U32 mask11Id11;

	MakeMask_11Id11 (&mask11Id11);

	// 11 -> 00:
	pNeighbor->Assign(this);
	pNeighbor->m_dwAddress &= (~mask11Id11);
}


IFXFORCEINLINE void IFXTQTAddress::BaseRight (IFXTQTAddress *pNeighbor)
{
	U32 mask01Id11;

	MakeMask_01Id11 (&mask01Id11);

	// 01s -> 00s:
	pNeighbor->Assign(this);
	pNeighbor->m_dwAddress &= (~mask01Id11);
}


IFXFORCEINLINE void IFXTQTAddress::BaseBase (IFXTQTAddress *pNeighbor)
{
	U32 mask01Id11, mask11Id11;

	MakeMask_01Id11 (&mask01Id11);
	MakeMask_11Id11 (&mask11Id11);

	// 11 -> 01:
	pNeighbor->Assign (this);
	pNeighbor->m_dwAddress	&=	~mask11Id11;	// Replace all 11s with 00s
	pNeighbor->m_dwAddress	|=	(mask11Id11 & TQT_EVEN_BIT_MASK);	// 01s

	// 01 -> 11:
	pNeighbor->m_dwAddress	|=	mask01Id11;
}

IFXFORCEINLINE U32	IFXTQTAddress::GetIthPair(U32 usIth)
{
	U32 usTemp = m_dwAddress >> (usIth * 2);
	return (usTemp & TQT_LEAST_SIGNIFICANT_PAIR);
}

#endif
