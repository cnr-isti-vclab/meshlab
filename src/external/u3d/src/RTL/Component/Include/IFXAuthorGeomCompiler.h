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
	@file	IFXAuthorGeomCompiler.h

			Contains declaration of the IFXAuthorGeomCompiler and related 
			structures.
*/


#ifndef IFXAuthorGeomCompiler_H__
#define IFXAuthorGeomCompiler_H__

#include "IFXSceneGraph.h"
#include "IFXAuthorMesh.h"
#include "IFXAuthorCLODGen.h"
#include "IFXAuthorCLODResource.h"
#include "IFXAuthorMeshScrub.h"
#include "IFXAuthorMeshMap.h"
#include "IFXMarkerX.h"
#include "IFXProgressCallback.h"


// {5321D219-A085-4581-BB94-0AFBA755317E}
IFXDEFINE_GUID(IID_IFXAuthorGeomCompiler,
0x5321D219, 0xA085, 0x4581, 0xBB, 0x94, 0x0A, 0xFB, 0xA7, 0x55, 0x31, 0x7E);

/**
	Contains parameters used to control the geometry compression
	algorithms.

	@note	All quality factor parameters must be in the range [0, 
			IFX_MAXIMUM_QUALITY_FACTOR].  As quality factors increase, the  
			actual quality also increases.  The default quality factor is 
			IFX_DEFAULT_QUALITY_FACTOR.
 */
struct IFXAuthorCLODCompressionParams
{
	IFXAuthorCLODCompressionParams();

	/**
	*  Set this boolean to TRUE when you change uDefaultQuality to something 
	*  other than its default.
	*/
	BOOL bSetDefaultQuality;
	/**
	*  The default quality factor.  This factor is 
	*  used for the compression of any mesh data pools (positions, 
	*  normals, diffuse colors, etc.) that do not have their quality
	*  specified by one of the other quality values in this structure.
	*/
	U32 uDefaultQuality;

	/**
	*  Set this boolean to TRUE to compress the mesh's position data with 
	*  a quality factor that differs from the default quality factor.
	*/
	BOOL bSetPositionQuality;
	/**
	*  The quality factor to be used when compressing the mesh's position
	*  data.  This factor is honored only when bSetPositionQuality is set
	*  to TRUE.
	*/
	U32 uPositionQuality;

	/**
	*  Set this boolean to TRUE to compress the mesh's texture coordinates 
	*  with a quality factor that differs from the default quality factor.
	*/
	BOOL bSetTexCoordQuality;
	/**
	*  The quality factor to be used when compressing the mesh's texture
	*  coordinates.  This factor is honored only when bSetTexCoordQuality is 
	*  set to TRUE.
	*/
	U32 uTexCoordQuality;

	/**
	*  Set this boolean to TRUE to compress the mesh's normal data with 
	*  a quality factor that differs from the default quality factor.
	*/
	BOOL bSetNormalQuality;
	/**
	*  The quality factor to be used when compressing the mesh's normal data.
	*  This factor is honored only when bSetNormalQuality is set to TRUE.
	*/
	U32 uNormalQuality;

	/**
	*  Set this boolean to TRUE to compress the diffuse colors of the mesh
	*  vertices with a quality factor that differs from the default quality factor.
	*/
	BOOL bSetDiffuseQuality;
	/**
	*  The quality factor to be used when compressing the diffuse colors 
	*  of the mesh vertices.
	*  This factor is honored only when bSetDiffuseQuality is set to TRUE.
	*/
	U32 uDiffuseQuality;

	/**
	*  Set this boolean to TRUE when you wish the specular colors of the mesh
	*  vertices to be compressed with a quality factor that differs from the
	*  default quality factor.
	*/
	BOOL bSetSpecularQuality;
	/**
	*  The quality factor to be used when compressing the specular colors 
	*  of the mesh vertices.
	*  This factor is honored only when bSetSpecularQuality is set to TRUE.
	*/
	U32 uSpecularQuality;

	/**
	*  Set this boolean to TRUE to set a minimum resolution above the default for the mesh. 
	*  This sets the minimum resolution to 0).
	*/
	BOOL bSetMinimumResolution;
	/**
	*  The minimum vertex count above which update records are stored
	*  for the model.  The model's resolution cannot be set to a value lower 
	*  than this value by the clodolution controller.
	*  This value is honored only when bSetMinimumResolution is set to TRUE.
	*/
	U32 uMinimumResolution;


	/**
	*  Set this boolean to TRUE to use a normal crease value
	*  that differs from the default (0.9f).
	*/
	BOOL bSetNormalCreaseParameter;
	/**
	*  Sets the normal crease parameter.  Normals at the same position 
	*  that are closer than the crease parameter are merged.  The crease
	*  parameter is the dot product between the normals (-1 to 1).
	*  This value is honored only when bSetNormalCreaseParameter is set
	*  to TRUE.
	*/
	F32 fNormalCreaseParameter;

	/**
	*  Set this boolean to TRUE to use a normal update value
	*  that differs from the default (-0.50f).
	*/
	BOOL bSetNormalUpdateParameter;
	/**
	*  Sets the normal update parameter. Corrections to the predicted 
	*  normal (determined during the compression process) that are smaller
	*  than the update value are dropped.  The update parameter is 
	*  expressed as a dot product between the original authored normal
	*  and the predicted normal (-1 to 1).
	*  This value is honored only when bSetNormalUpdateParameter is set
	*  to TRUE.
	*/
	F32 fNormalUpdateParameter;

	/**
	*  Set this boolean to TRUE to use a normal tolerance value
	*  that differs from the default (0.985f).
	*/
	BOOL bSetNormalTolerance;
	/**
	*  Sets the normal tolerance parameter. Normals smaller than this 
	*  value will be considered the same.  Larger values result in a smaller 
	*  render mesh at the sacrifice of accuracy.  Smaller values are accurate but
	*  result in a bulky mesh. The tolerance is expressed as the dot product
	*  between the two normals (-1 to 1).
	*  This value is honored only when bSetNormalTolerance is set to TRUE.
	*/
	F32 fNormalTolerance;

	/**
	*  Set this boolean to TRUE to use a streaming priority 
	*  value that differs from the default (256).
	*/
	BOOL bSetStreamingPriority;
	/**
	*  Specifies the model's streaming priority, which alters its ordering
	*  in the U3D file during streaming.  Lower priorities stream first,
	*  with higher priorities streaming last.  The lowest possible priority,
	*  0, is reserved for objects that must stream to the client to form
	*  a complete description of all objects that will appear in the scene
	*  after the U3D file completes loading.
	*/
	U32 uStreamingPriority;

	BOOL bSetExcludeNormals;
	BOOL bExcludeNormals;
};

IFXINLINE IFXAuthorCLODCompressionParams::IFXAuthorCLODCompressionParams()
{
	bSetDefaultQuality = FALSE;
	uDefaultQuality = IFX_DEFAULT_QUALITY_FACTOR;

	bSetPositionQuality = FALSE;
	uPositionQuality = IFX_DEFAULT_QUALITY_FACTOR;

	bSetTexCoordQuality = FALSE;
	uTexCoordQuality = IFX_DEFAULT_QUALITY_FACTOR;

	bSetNormalQuality = FALSE;
	uNormalQuality = IFX_DEFAULT_QUALITY_FACTOR;

	bSetDiffuseQuality = FALSE;
	uDiffuseQuality = IFX_DEFAULT_QUALITY_FACTOR;

	bSetSpecularQuality = FALSE;
	uSpecularQuality = IFX_DEFAULT_QUALITY_FACTOR;

	bSetMinimumResolution = FALSE;
	uMinimumResolution = 0;

	bSetNormalCreaseParameter = FALSE;
	fNormalCreaseParameter = 0.9f;

	bSetNormalUpdateParameter = FALSE;
	fNormalUpdateParameter = 0.5f;

	bSetNormalTolerance = FALSE;
	fNormalTolerance = 0.985f;

	bSetStreamingPriority = FALSE;
	uStreamingPriority = 256;

	bSetExcludeNormals = FALSE;
	bExcludeNormals = FALSE;
}

/**
 *	Used to set one or more of the parameters which describe how a mesh will
 *  be converted into a form optimized for rendering, and how the mesh
 *  will be compressed for streaming.
 */
struct IFXAuthorGeomCompilerParams
{
	IFXAuthorGeomCompilerParams();
	/**
	*  Indicates whether degenerate faces or other illegal mesh
	*  features should be scrubbed from the mesh before compiling it.
	*/
	BOOL bScrub;
	/**
	*  Parameters which control the thoroughness of the mesh scrubbing
	*  process.
	*/
	IFXAuthorMeshScrubParams ScrubParams;
	/**
	*  A flag which determines whether or not a multi-resolution 
	*  representation of the mesh should be generated.
	*/
	BOOL bCLOD;
	/**
	*  Parameters which control the generation of the multi-resolution
	*  mesh representation
	*/
	IFXAuthorCLODGenParam CLODParams;
	/**
	*  A flag which indicates whether or not the user is passing in
	*  parameters which control the compression of the mesh.
	*/
	BOOL bCompressSettings; 

	/**
	 *	Parameters which control the compression of geometry compression and
	 *  its representation.  Note:  After the mesh is compiled, none of these
	 *  values may be changed.
	 */
	IFXAuthorCLODCompressionParams CompressParams;
	/**
	*   A callback that can be used to give the user feedback on the
	*   process of mesh compilation and compression activities.
	*/
	IFXProgressCallback* pProgressCallback;
};

class IFXAuthorGeomCompiler : public IFXUnknown
{
public:

	/**
	*  A function which passes the scenegraph pointer to the mesh
	*  compiler.
	*/
	virtual IFXRESULT IFXAPI  SetSceneGraph(IFXSceneGraph*) = 0;

	/**
	 *	Compiles the input IFXAuthorMesh.  This may involve "scrubbing" bad 
	 *  faces and unused mesh data from the mesh, producing a clodolution
	 *  representation of the mesh, and compressing the mesh into a form
	 *  optimized for streaming over the Internet.
	 *  NOTE: Please make sure that you turned srubbing on and set remove zero 
	 *  area faces option on before calling this function. In other case you can
	 *  experiance wrong behavior with models containing "zero" faces.
	 *
	 *	@param	rName	  The name that will be given to the generator
	 *                    created by this operation.  This is also the
	 *                    name given the data blocks when they are written
	 *                    to disk.
	 *	@param	in_pMesh  The input IFXAuthorMesh to compile.
	 *	@param	out_ppResource	The returned pointer to the resource
	 *                    (IFXAuthorCLODResource) created by this compiler.
	 *                    You must release this resource once you have
	 *                    finished using it.
	 *	@param	in_pParams	The optional parameters which control the
	                      compilation process.
	 *
	 *	@return An IFXRESULT value.
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_INVALID_POINTER	One (or more) of the input arguments 
	 *                    was NULL.
	 *	@retval	IFX_E_OUT_OF_MEMORY	There was not enough memory to create a
	 *                    requested component.
	 *	@retval	IFX_E_UNDEFINED	An error was detected in the compression 
	 *                    parameters.
	 */
	virtual IFXRESULT IFXAPI  Compile(IFXString& rName,IFXAuthorMesh* in_pMesh, 
					IFXAuthorCLODResource** out_ppResource,
					BOOL forceCompress,
					IFXAuthorGeomCompilerParams* in_pParams= NULL) = 0;

	/**
	*  This function currently does nothing.
	*/
	virtual IFXRESULT IFXAPI  Recompile(IFXAuthorCLODResource* in_pResource,
					IFXAuthorMeshMap* in_pMeshMap, 
					IFXAuthorMeshMap** out_ppMeshMap = NULL
					/*Compress Settings, res settings*/ ) = 0;
};


inline IFXAuthorGeomCompilerParams::IFXAuthorGeomCompilerParams()
{
	bScrub = TRUE;
	bCLOD = TRUE;
	bCompressSettings = TRUE;
	pProgressCallback = NULL;
}


#endif
