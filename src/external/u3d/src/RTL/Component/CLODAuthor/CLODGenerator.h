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
#ifndef CLODGENERATOR_DOT_H
#define CLODGENERATOR_DOT_H
#include "IFXAuthorCLODAccess.h"

#include "IFXAuthorCLODMesh.h"
#include "IFXAuthorMeshMap.h"
#include "IFXProgressCallback.h"

class VertexPairContractor;

class CLODGenerator
{
public:
	typedef void (* PROGRESS_CALLBACK) (U32 pairsRemaining, void* pUserData);

	// Basic functionality for most users	
	IFXRESULT	SetInputMesh(IFXAuthorMesh* pMesh);  ///< pMesh will be modified in place.
	IFXRESULT	Generate();							  ///< starts CLOD generation process,
	IFXAuthorCLODMesh*	GetOutputMesh();              ///< returns pMesh from setinputmesh. 
	IFXAuthorMeshMap * GetMeshMap();

    void   SetProgressCallBack(IFXProgressCallback *pPCB) { m_pProgress = pPCB; };

	IFXRESULT	Kill (); ///< call this to terminate CLODGen early.

	/// postive values allow neighboring vetices to merge even if they are not connected by edges.
	IFXRESULT	EnableUnconnectedVertexMerge(F32 maxDistance, 
						BOOL acrossObjectsOnly = TRUE);  
	IFXRESULT   DisableUnconnectedVertexMerge();
				
	/** The normals will be updated to reflect the actual normal of the simplified surface.
	Hard edges will be formed when the angle between face normals exceeds the crease angle parameter.*/
	IFXRESULT	SetNormalModeTrackSurfaceChanges(F32 creaseAngle);
				
	/** Normals will be correct only at highest resolution, 
	No normal updates will be generated.
	Normal faces will be reordered so that normals are right at full resolution.*/
	IFXRESULT	SetNormalModeNoUpdates();
				
	/// Totaly ignore normals, no normals will be present in the output mesh 
	IFXRESULT	SetNormalModeNone();

	IFXRESULT	SetMaxNormalChange(F32 maxNormalChange);
	

	CLODGenerator( U32 baseVert, U32* pBaseVert );
	~CLODGenerator();

	typedef enum
	{
		None=0,			 ///< output mesh will have no normals, even if normals are present in input.
		NoUpdates,		 ///< normals correct at highest resolution only
		UpdateToParent,  ///< after collapse the updated faces get the normal at the parent vertex.
		TrackSurfaceChanges,  ///< normals will be updated so that they match the actual surface after each edge collapse.
		NM_PAD_4BYTES=0xFFFFFFFF ///< Do not use this item. Added this in so that this data type always be 4 bytes long on both PC and Mac.
	} NormalsMode;

	class Params
	{
	public:
		IFXAuthorCLODMesh		*pMesh;	///< A Pointer to a copy of the input mesh
		F32		mergeThresh;
		BOOL	mergeWithin;
		PROGRESS_CALLBACK progressCallback;
		U32		progressFrequency;
		void	*pProgressCallBackUserData;
		U32		numBaseVertices;
		U32		*baseVertices;
		NormalsMode		normalsMode;
		F32		normalsCreaseAngle;
		F32		*meshDamage;   ///< array of floats to store mesh damge results in.
		IFXAuthorMeshMap		*pMeshMap;
		F32		maxNormalChange;
	};

private:

	CLODGenerator() { };

	VertexPairContractor	*m_pVPC;				///< vpc is a ptr to the backbone object of CLODGen.
	Params					m_Params;
	BOOL					m_GenerateRunning;		///< current state
	BOOL					m_GenerateCompletedOK;	///< current state
    IFXProgressCallback     *m_pProgress;
	
};

#endif // CLODGENERATOR_DOT_H
