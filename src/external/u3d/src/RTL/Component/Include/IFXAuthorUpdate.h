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
//
// IFXAuthorUpdate.h
//
// DESCRIPTION:
//     Declaration of IFXAuthorUpdate interface
//
// NOTES
//
//*****************************************************************************
#ifndef __IFXAuthorUpdate_H__
#define __IFXAuthorUpdate_H__

#include "IFXDataTypes.h"
#include <memory.h>

/**
*  The structure containing information on how face data is added to or
*  removed from a mesh during mesh resolution changes.
*/
struct IFXAuthorFaceUpdate
{
	IFXAuthorFaceUpdate();

	enum AttributeType 
	{
		Undefined, 
		Position, 
		Normal, 
		Diffuse, 
		Specular, 
		Tex0
	};

	U32   FaceIndex;        ///< Index of the face to be changed.
	U32   Corner;         ///< Index of face corner to be changed (0,1,2).
	AttributeType     Attribute;  ///< The token describing the changed attribute.
	U32   IncrValue;        /**< [Increasing]: Attribute value for
							resolution increase. */
	U32   DecrValue;        /**< [Decreasing]: Attribute value for
							resolution decrease. */

};


/**
*  The structure containing information on how vertex data is added to or
*  removed from a mesh during mesh resolution changes.
*/
struct IFXAuthorVertexUpdate
{
	IFXAuthorVertexUpdate();
	~IFXAuthorVertexUpdate();
	/**
	*  A routine used during the reallocation of update records to make
	*  the reallocation more efficient. Copies the data from this update
	*  record to the specified record, including a transfer of the face update.
	*  When the routine completes, this update record no longer contains
	*  any face updates.
	*
	*  @param IFXAuthorVertexUpdate  The destination update record
	*/
	void CopyTo(IFXAuthorVertexUpdate*);

	IFXAuthorVertexUpdate& operator = (const IFXAuthorVertexUpdate&);

	U16 NumNewFaces;    ///< The number of new faces created by this update record.
	U16 NumNewNormals;  ///< The number of new normals created by this update record.
	U16 NumNewDiffuseColors;  /**< The number of new diffuse vertex colors
							  created by this update record. */
	U16 NumNewSpecularColors; /**< The number of new specular vertex colors
							  created by this update record. */
	U16 NumNewTexCoords;  /**< The number of new texture coordinates created
						  by this update record. */
	U16 NumFaceUpdates; /**< The number of updates required to make these
						changes, including updates for both new faces
						and old faces. */
	IFXAuthorFaceUpdate*  pFaceUpdates;  /**< The face updates which
											  compose this resolution change */
};

IFXINLINE IFXAuthorVertexUpdate::IFXAuthorVertexUpdate()
{
	NumNewFaces = 0;
	NumNewNormals = 0;
	NumNewDiffuseColors = 0;
	NumNewSpecularColors = 0;
	NumNewTexCoords = 0;
	NumFaceUpdates = 0;
	pFaceUpdates = NULL;

}

IFXINLINE IFXAuthorVertexUpdate::~IFXAuthorVertexUpdate()
{
	IFXDELETE_ARRAY(pFaceUpdates);
}

IFXINLINE void IFXAuthorVertexUpdate::CopyTo(IFXAuthorVertexUpdate* in_pVU)
{
	in_pVU->NumNewFaces = NumNewFaces;
	in_pVU->NumNewNormals = NumNewNormals;
	in_pVU->NumNewDiffuseColors = NumNewDiffuseColors;
	in_pVU->NumNewSpecularColors = NumNewSpecularColors;
	in_pVU->NumNewTexCoords = NumNewTexCoords;
	in_pVU->NumFaceUpdates = NumFaceUpdates;
	NumFaceUpdates = 0;
	IFXDELETE_ARRAY(in_pVU->pFaceUpdates)
		in_pVU->pFaceUpdates = pFaceUpdates;
	pFaceUpdates = NULL;
}

IFXINLINE IFXAuthorVertexUpdate& IFXAuthorVertexUpdate::operator = (const IFXAuthorVertexUpdate& in_rhs)
{
	NumNewFaces = in_rhs.NumNewFaces;
	NumNewNormals = in_rhs.NumNewNormals;
	NumNewDiffuseColors = in_rhs.NumNewDiffuseColors;
	NumNewSpecularColors = in_rhs.NumNewSpecularColors;
	NumNewTexCoords = in_rhs.NumNewTexCoords;
	NumFaceUpdates = in_rhs.NumFaceUpdates;

	if(NumFaceUpdates>0)
	{
		IFXDELETE_ARRAY(pFaceUpdates);
		pFaceUpdates = new IFXAuthorFaceUpdate[NumFaceUpdates];
		/// @todo: How to handle failed memory allocation?
		memcpy(pFaceUpdates, in_rhs.pFaceUpdates, sizeof(IFXAuthorFaceUpdate) * NumFaceUpdates);
	}

	return *this;
}

IFXINLINE IFXAuthorFaceUpdate::IFXAuthorFaceUpdate()
{
	FaceIndex = 0;
	Corner = 0;
	Attribute = Undefined;
	IncrValue = (U32)-1;
	DecrValue = 0;
}


#endif
