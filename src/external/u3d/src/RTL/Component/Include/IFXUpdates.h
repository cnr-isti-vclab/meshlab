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
#ifndef IFXUPDATES_DOT_H
#define IFXUPDATES_DOT_H
 
#include "IFXDataTypes.h"
#include "IFXResult.h"

/**
The IFXUpdates interface manages a list of IFXResolutionChange and IFXFaceUpdate objects, 
which contain the actual data for performing incremental resolution changes to clodolution 
meshes.

IFXFaceUpdate:This object maintains the data needed to update one face. One complete resolution 
change typically involves several face updates
*/

typedef struct
{
	U32 face;    ///< index of face to modify.
	U32 corner;  ///< which corner of the face to modify.
	U32 newUp;   ///< new vertex index to use when increasing the resolution.
	U32 newDown; ///< new vertex index to use when decreasing the resolution.
} IFXFaceUpdate;

/** 
With IFXMesh a single resolution change may involve more than one vertex. 
*/
class IFXResolutionChange
{
public:
	IFXResolutionChange() { deltaVerts = 0; deltaFaces = 0; numFaceUpdates = 0; };
	U32 deltaVerts;	  ///< number of vertices added or removed.
	U32 deltaFaces;	  ///< number of faces added or removed.
	U32 numFaceUpdates;	 ///< number of faces that need to be modified because of resolution change.
};

/**
Just as IFXMeshGroup contains IFXMeshes, IFXUpdatesGroup contains IFXUpdates. 
Each IFXMesh in an IFXMeshGroup has a corresponding IFXUpdates object 
in the IFXUpdateGroup object.
*/
class IFXUpdates
{

public:
	
    /**Class constructor. */
	IFXUpdates();
	/**Class destructor. */
	~IFXUpdates();

	/**
		Allocates resolution change and updates number.
	
		@param	rc	Resolution change number
		@param	fu	Face update number

		@return	IFXResult
	 */
	
	IFXRESULT Allocate(U32 rc, U32 fu);

	U32 numResChanges;
	IFXResolutionChange *pResChanges;

	U32 numFaceUpdates;
	IFXFaceUpdate *pFaceUpdates;
};


IFXINLINE IFXUpdates::IFXUpdates() 
{ 
    numResChanges = 0; 
    numFaceUpdates = 0; 
    pResChanges = NULL; 
    pFaceUpdates = NULL; 
}
	

IFXINLINE IFXUpdates::~IFXUpdates() 
{
    if (pFaceUpdates)
    {
        delete[] pFaceUpdates; 
        pFaceUpdates = NULL;
    }
    
    if (pResChanges)
    {
        delete[] pResChanges; 
        pResChanges = NULL;
    }
}

/*
IFXINLINE IFXRESULT IFXUpdates::Allocate() 
{ 
    if (pResChanges)
    {
        delete pResChanges;
        pResChanges = NULL;
    }
	pResChanges = new IFXResolutionChange[numResChanges];
    
    if (pFaceUpdates)
    {
        delete pFaceUpdates;
        pFaceUpdates = NULL;
    }
	pFaceUpdates = new IFXFaceUpdate[numFaceUpdates];
	
    if (pResChanges && pFaceUpdates) 
		return IFX_OK;
	else 
		return IFX_E_OUT_OF_MEMORY;
};
*/
	
IFXINLINE IFXRESULT IFXUpdates::Allocate(U32 rc, U32 fu) 
{ 
	IFXRESULT returnCode = IFX_OK;
    if (pResChanges)
    {
        delete [] pResChanges;
        pResChanges = NULL;
    }   
    if (pFaceUpdates)
    {
        delete [] pFaceUpdates;
        pFaceUpdates = NULL;
    }

	pResChanges     = new IFXResolutionChange[rc];
	pFaceUpdates    = new IFXFaceUpdate[fu];

	if(pResChanges && pFaceUpdates) 
    {
//		memset(pResChanges,0,rc*sizeof(IFXResolutionChange));
//		memset(pFaceUpdates,0,fu*sizeof(IFXFaceUpdate));
		numResChanges = rc;
		numFaceUpdates = fu;
	} 
	else 
	{
		if (pResChanges)
		{
			delete [] pResChanges;
			pResChanges = NULL;
		}   
		if (pFaceUpdates)
		{
			delete [] pFaceUpdates;
			pFaceUpdates = NULL;
		}
		
		returnCode = IFX_E_OUT_OF_MEMORY;
	}
	return returnCode;
}

#endif
