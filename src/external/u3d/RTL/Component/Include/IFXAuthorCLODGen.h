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
	@file	IFXAuthorCLODGen.h

			Declaration of IFXAuthorCLODGen interface
*/
#ifndef IFXAuthorCLODGen_H__
#define IFXAuthorCLODGen_H__

#include "IFXAuthorCLODMesh.h"
#include "IFXProgressCallback.h"
#include "IFXAuthorMeshMap.h"

// {106BCE29-BD0A-4B9E-8EA3-6FEEACA29EB6}
IFXDEFINE_GUID(IID_IFXAuthorCLODGen,
0x106BCE29, 0xBD0A, 0x4B9E, 0x8E, 0xA3, 0x6F, 0xEE, 0xAC, 0xA2, 0x9E, 0xB6);

enum IFXAuthorCLODNormalsMode
{
	NoNormals=0,			 ///< Output mesh will have no normals, even if normals are present in input.
	NoUpdates,		 ///< Normals correct at highest resolution only
	UpdateToParent,  ///< After collapse, the updated faces get the normal at the parent vertex.
	TrackSurfaceChanges,  ///< Normals will be updated so that they match the actual surface after each edge collapse.
	
	NM_PAD_4BYTES=0xFFFFFFFF			//Do not use this item. Added this so that this data type  
	                            		//always be 4-bytes long on both PC and Mac.
};

class IFXAuthorCLODGenParam
{
public:
	IFXAuthorCLODGenParam();

	BOOL	MergeWithin;	///< Should the CLOD process attempt to merge unconnected points?
	F32		MergeThresh;	///< Merging threshold.  Values >=0 set threshold, <0 disabled merging 

	IFXAuthorCLODNormalsMode		NormalsMode; ///< Specifies how CLODGen should deal with normals
	F32							NormalsCreaseAngle;  ///<  The minimum crease angle which CLOD will consider a sharp edge
	/**< if normals are to be updated to track the changes to the surface */
};


class IFXAuthorCLODGen : public IFXUnknown
{
public:
	//*		IFXAuthorCLODGen methods
	/**
	 *	This method generates the multi-resolution representation of the
	 *  mesh.  The multi-resolution representation has faces and vertices
	 *  reordered compared to the input mesh, and also includes updates
	 *  which specify how faces are added or removed from the mesh during
	 *  resolution changes.
	 *
	 *	@param	in_pParams	The parameters that describe how CLOD should
	 *                      build the clodolution version of the mesh.
	 *	@param	in_pProgress  The callback to be used to give the user 
	 *                        feedback on the process of the mesh compilation
	 *                        and compression activities.
	 *	@param	in_pMesh	  The input mesh.
	 *	@param	out_ppMesh	  The resulting multi-resolution version of the
	 *                        input mesh.
	 *	@param	out_ppMapping	A mapping table that describes where the
	 *                          vertices, normals, etc. from the input mesh
	 *                          ended up in the output mesh.
	 *
	 *	@return	An IFXRESULT value
	 *	@retval	IFX_OK	No error.
	 */
	virtual IFXRESULT IFXAPI  Generate(	const IFXAuthorCLODGenParam* in_pParams,
								IFXProgressCallback* in_pProgress,
								IFXAuthorCLODMesh* in_pMesh,
								IFXAuthorCLODMesh** out_ppMesh,
								IFXAuthorMeshMap** out_ppMapping) = 0;

};

inline 	IFXAuthorCLODGenParam::IFXAuthorCLODGenParam()
{
	NormalsCreaseAngle = 75.0f;
	NormalsMode = TrackSurfaceChanges;
	MergeThresh = -1.0f; 
	MergeWithin = FALSE;
}

#endif
