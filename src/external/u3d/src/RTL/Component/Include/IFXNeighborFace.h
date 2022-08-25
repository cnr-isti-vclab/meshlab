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
	@file	IFXNeighborFace.h

			Class definition for neighbor mesh faces and
			flag data structures.

	@note	The corner and face flags are cooperatively shared
			across different technologies.  The neighbor mesh
			resolution controller uses the named flags.
*/

#ifndef IFXNeighborFace_h
#define IFXNeighborFace_h

#include "IFXDebug.h"
#include "IFXDataTypes.h"

#define IFX_NEIGHBOR_HUGE


// Maximum index size for each bit layout
#ifdef IFX_NEIGHBOR_HUGE
#define MAX_NBR_MESH_INDEX 0xFFFFFFFF
typedef U32 IFXNBRINDEX;
#else
#define MAX_NBR_MESH_INDEX 0x0000FFFF
typedef U16 IFXNBRINDEX;
#endif

#define IFX_E_MESH_TOO_LARGE MAKE_IFXRESULT_FAIL( IFXRESULT_NEIGHBORMESH, 0x001)

//
//  Corner flags
//
struct IFXCornerFlags
{
	U8 cornerIndex		: 2;
	U8 flippedFlag		: 1;
	U8 collapsedFlag	: 1;

	U8 cornerFlag0		: 1;
	U8 cornerFlag1		: 1;
	U8 cornerFlag2		: 1;
	U8 cornerFlag3		: 1;
};


//
//  Face flags
//
struct IFXFaceFlags
{
	U8 collapseIndex	: 2;
	U8 duplicateFlag	: 1;

	U8 faceFlag0		: 1;
	U8 faceFlag1		: 1;
	U8 faceFlag2		: 1;
	U8 faceFlag3		: 1;
	U8 faceFlag4		: 1;
};


//
// Simple container object

/** The IFXNeighborFace interface defines neighbor mesh faces and flag data structures.
	The corner and face flags are cooperatively shared across different technologies.
	The neighbor mesh resolution controller uses the named flags.


	@note	Gets the face index of the next face in the circular-linked list
     of faces around the edge identified by cornerIndex (the edge opposite
	the corner).

	@note When there are no neighbor faces on the identified edge
    (as in the outermost extent of the mesh), this is best visualized
    as one face in the circular-linked list (of faces around the edge).
    Thus, the current face index is returned.

	@note No virtual methods. Adding a single virtual method costs four bytes per face.

	@note Define the preprocessor symbol IFX_NEIGHBOR_HUGE to compile Neighbor Mesh with the Huge layout.

		@verbatim

		Bit Layout of Compact vs. Huge Versions


		Corner bits		Compact		Huge
 		mesh index		16			32
 		face index		16			32
 		corner index	2			2
 		flipped			1			1
		collapsed		1           1
 		user flags		4			4
		Sum				40 			72

		All corners		120			216
		Face flag		8			8

		Total bits		128			224
		Total bytes		16			28
@endverbatim
*/
class IFXNeighborFace
{
public:

//	IFXNeighborFace() {}	// no constructor -- fast initialization by IFXNeighborMesh
//	~IFXNeighborFace() {}	// not virtual for memory reasons

    ///
    void GetNeighbor(/**in: */ U32 corner,				//!< corner index on current face:
					  /**out: */ U32* pNeighborMesh,	//!< mesh index of neighbor face
					  /**out: */ U32* pNeighborFace,	//!< face index of neighbor face
					  /**out: */ IFXCornerFlags** ppFlags //!< corner flags
					  );

    //! Sets neighbor mesh links for a corner on the current face.
    void SetNeighbor(/*in*/ U32 corner,				//!< corner index on current face:
													//!<   must be 0, 1, or 2
                      /*in*/ U32 neighborMesh,		//!< mesh index of neighbor face
                      /*in*/ U32 neighborFace,		//!< face index of neighbor face
                      /*in*/ IFXCornerFlags flags	//!< corner flags
					  );

    /// Sets neighbor mesh links for a corner on the current face.
    void SetNeighbor(/*in*/ U32 corner,				//!< corner index on current face
                      /*in*/ U32 neighborMesh,		//!< mesh index of neighbor face
                      /*in*/ U32 neighborFace,		//!< face index of neighbor face
                      /*in*/ U32 neighborCorner		//!< corner index of neighbor face
					  );

	/// Gets face flags for the current face.
    IFXFaceFlags* GetFaceFlags();

	/// Sets flags for the current face.
    void SetFaceFlags(/*in*/ IFXFaceFlags flags);

	/// Gets flags for a corner.
    IFXCornerFlags* GetCornerFlags(/*in*/ U32 corner);

	/// Sets flags for a corner.
    void SetCornerFlags(/**in: */ U32 corner,		//!< corner index on current face
						/**in: */ IFXCornerFlags flags //!< corner flags
						);

public:

#ifdef IFX_NEIGHBOR_HUGE

	U32 m_neighborMesh[3];				///< neighbor index
	U32 m_neighborFace[3];				///< neighbor face indices
	IFXCornerFlags m_cornerFlags[3];	///< corner flags
	IFXFaceFlags m_faceFlags;			///< face flags

#else // Compact

	U16 m_neighborMesh[3];				///< neighbor index
	U16 m_neighborFace[3];				///< neighbor face indices
	IFXCornerFlags m_cornerFlags[3];	///< corner flags
	IFXFaceFlags m_faceFlags;			///< face flags

#endif
};


//
//  IFXINLINEs
//

IFXINLINE void IFXNeighborFace::GetNeighbor(/*in*/ U32 corner,
										  /*out*/ U32* pNeighborMesh,
										  /*out*/ U32* pNeighborFace,
										  /*out*/ IFXCornerFlags** pFlags)
{
	IFXASSERT(corner < 3);
	*pNeighborMesh = U32(m_neighborMesh[corner]);
	*pNeighborFace = U32(m_neighborFace[corner]);
	*pFlags = &(m_cornerFlags[corner]);
}


IFXINLINE void IFXNeighborFace::SetNeighbor(/*in*/ U32 corner,
										  /*in*/ U32 neighborMesh,
										  /*in*/ U32 neighborFace,
										  /*in*/ IFXCornerFlags flags)
{
	IFXASSERT(corner < 3);
	m_neighborMesh[corner] = neighborMesh;
	m_neighborFace[corner] = neighborFace;
	m_cornerFlags[corner] = flags;
}


IFXINLINE void IFXNeighborFace::SetNeighbor(/*in*/ U32 corner,
										  /*in*/ U32 neighborMesh,
										  /*in*/ U32 neighborFace,
										  /*in*/ U32 neighborCorner)
{
	IFXASSERT(corner < 3);
	m_neighborMesh[corner] = neighborMesh;
	m_neighborFace[corner] = neighborFace;
	m_cornerFlags[corner].cornerIndex = neighborCorner;
}


IFXINLINE IFXFaceFlags* IFXNeighborFace::GetFaceFlags()
{
	return &m_faceFlags;
}


IFXINLINE void IFXNeighborFace::SetFaceFlags(/*in*/ IFXFaceFlags flags)
{
	m_faceFlags = flags;
}


IFXINLINE IFXCornerFlags* IFXNeighborFace::GetCornerFlags(/*in*/ U32 corner)
{
	return &(m_cornerFlags[corner]);
}


IFXINLINE void IFXNeighborFace::SetCornerFlags(/*in*/ U32 corner,
											/*in*/ IFXCornerFlags flags)
{
	m_cornerFlags[corner] = flags;
}

// IFXNeighborMesh corner flags for parsing discontinuities:
#define CORNER_FLAG_DISCONTINUOUS_FINISHED		cornerFlag0
#define CORNER_FLAG_DISCONTINUOUS_NORMAL		cornerFlag1
#define CORNER_FLAG_DISCONTINUOUS_TEXCOORD		cornerFlag2
#define CORNER_FLAG_DISCONTINUOUS_SURFACE		cornerFlag3

#endif
