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
	@file	DebugInfo.cpp

			Implementation of the DebugInfo class.
*/


//***************************************************************************
//  Defines
//***************************************************************************

#define DEFINE_DBGINFO

// bail if cannot  log
#define CHK_DUMP_ENTRY(x) if(!m_Flags[IFX_EDI_ENABLE] || !(m_Flags[IFX_EDI_DUMP] || !m_DbgFile || !m_Flags[x])) return;

//***************************************************************************
//  Includes
//***************************************************************************

#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "U3DHeaders.h"
#include "DebugInfo.h"

using namespace U3D_IDTF;

//***************************************************************************
//  Constants
//***************************************************************************


//***************************************************************************
//  Enumerations
//***************************************************************************


//***************************************************************************
//  Classes, structures and types
//***************************************************************************


//***************************************************************************
//  Global data
//***************************************************************************

namespace U3D_IDTF
{
	DebugInfo g_DbgInfo;
}

//***************************************************************************
//  Local data
//***************************************************************************


//***************************************************************************
//  Local function prototypes
//***************************************************************************


//***************************************************************************
//  Public methods
//***************************************************************************


DebugInfo::DebugInfo()
{
	memset(m_Flags, 0, sizeof(U32) * IFX_EDI_COUNT);
	m_DbgFile = NULL;
}

DebugInfo::~DebugInfo()
{
	if(m_DbgFile)
	{
		fclose(m_DbgFile);
	}
}


/**
	Close the debug info file if it is open.
*/
void DebugInfo::Close()
{
	// Close the file if it is open
	if(m_DbgFile)
	{
		fclose(m_DbgFile);
		m_DbgFile = NULL;
	}
}


/**
	Initializes the object for use.

	@param   const char* pFileName   Name of the output file, will append 
	".debuginfo.txt" to
	the input string. So you could just pass in the name of
	the target file of your export.

	@return  IFXRESULT         Return status of this method.
*/
IFXRESULT DebugInfo::Init(const char* pFileName)
{
	int i = 0;

	// By default, dump everything.  Set the flags appropriately to fit your needs.
	for( i =0; i < IFX_EDI_COUNT; ++i)
	{
		m_Flags[i] = 1;
	}

	// Close the file if it is open
	if(m_DbgFile)
	{
		fclose(m_DbgFile);
		m_DbgFile = NULL;
	}

	if(m_Flags[IFX_EDI_ENABLE])
	{
		char dumpFile[1024];

		strcpy(dumpFile, pFileName);
		strcat(dumpFile, ".DebugInfo.txt");

		m_DbgFile = fopen( dumpFile, "w" );
		
		if(!m_DbgFile)  // file open failed
		{
			m_Flags[IFX_EDI_ENABLE] = false;
		}
		else
		{
			time_t t;
			struct tm *nt;

			Write("Debug Info for %s\n", pFileName);
			DISCARD time(&t);
			nt = localtime(&t);
			char* time = asctime(nt);
			Write("Dumped at:  %s\n", time );
		}
	}

	return IFX_OK;
}


#undef READSETTINGS

/**
	Set the Debuging flag.

	@param   U32 in_Id   One of IFX_EDI_FLAGS values indicating the flag to set.
	@param   U32 in_value  New value of flag.

	@return  IFXRESULT   Return status of this method.
*/
IFXRESULT DebugInfo::SetFlag(U32 in_Id, U32 in_value)
{
	if(in_Id >= IFX_EDI_COUNT || IFX_EDI_ENABLE == in_Id)
		return IFX_E_INVALID_RANGE;

	m_Flags[in_Id] = in_value;

	return IFX_OK;
}


/**
	Writes a formatted string to the output file.

	@param   const char* in_Format Format of the output string - conforms to printf specification.
	@param   ...           Other parameters for the output string just like printf
*/
void DebugInfo::Write(const char* in_fmt, ...)
{
	if(!(m_Flags[IFX_EDI_ENABLE] && m_Flags[IFX_EDI_DUMP] && m_DbgFile))
		return;

	va_list marker;
	va_start( marker, in_fmt );     /* Initialize variable arguments. */
	DISCARD vfprintf(m_DbgFile, in_fmt, marker);
	va_end( marker );              /* Reset variable arguments.      */
}


/**
	Write this comment if the corresponding option is enabled.

	@param   U32 in_Flag       Flag to check.
	@param   const char* in_Format Format of the output string - conforms to printf specification.
	@param   ...           Other parameters for the output string just like printf.
*/
void DebugInfo::WriteIf(U32 in_Cond, const char * in_fmt, ...)
{
	if(!(m_Flags[IFX_EDI_ENABLE] && m_Flags[in_Cond] && m_Flags[IFX_EDI_DUMP] && m_DbgFile))
		return;

	va_list marker;
	va_start( marker, in_fmt );     /* Initialize variable arguments. */
	DISCARD vfprintf(m_DbgFile, in_fmt, marker);
	va_end( marker );              /* Reset variable arguments.      */
}


/**
	Writes a description of an IFXSubdivModifier to the debug file.

	@param   IFXSubdivModifier* pData   The object to write.
*/
void DebugInfo::Write(IFXSubdivModifier* pData)
{
	U32     iDepth = 0;
	F32     fTension = 0.0f;
	F32     fError = 0.0f;
	BOOL    bEnabled = FALSE;
	BOOL    bAdaptive = FALSE;

	CHK_DUMP_ENTRY(IFX_EDI_MODIFIER);

	if(NULL == pData)
		return ;

	if( pData)
	{
		DISCARD pData->GetEnable(&bEnabled);
		DISCARD pData->GetAdaptive(&bAdaptive);
		DISCARD pData->GetDepth(&iDepth);
		DISCARD pData->GetTension(&fTension);
		DISCARD pData->GetError(&fError);
	}

	Write("\t\t\tSubdivision Modifier:\n");

	if( TRUE == bEnabled)
	{
		Write("\t\t\t\tSubdivision enabled,");
	}
	else
	{
		Write("    Subdivision disabled,");
	}

	if( TRUE == bAdaptive)
	{
		Write(" Adaptive mode\n");
	}
	else
	{
		Write(" Non-adaptive mode\n");
	}

	Write("\t\t\t\tDepth = %d, Tension = %f, Error = %f\n", iDepth, fTension, fError);
}


/**
	Writes a description of an IFXCLODModifier to the debug file.

	@param   IFXCLODModifier* pData   The object to write.
*/
void DebugInfo::Write(IFXCLODModifier* pData)
{
	F32   fCLODLevel = 0.0f;
	F32   fCLODBias = 0.0f;
	BOOL  bControllerState = FALSE;

	CHK_DUMP_ENTRY(IFX_EDI_MODIFIER);
	if(NULL == pData)
		return ;

	if( pData)
	{
		DISCARD pData->GetCLODLevel(&fCLODLevel);
		DISCARD pData->GetLODBias(&fCLODBias);
		DISCARD pData->GetCLODScreenSpaceControllerState(&bControllerState);
	}

	Write("\t\t\tLOD (CLOD) Modifier:\n");


	Write("\t\t\t\tLevel=%f, Bias=%f, State=%d)", fCLODLevel, fCLODBias, bControllerState);
}

/**
	Writes a description of an IFXAnimationModifier to the debug file.

	@param   IFXAnimationModifier* pData  The object to write.
*/
void DebugInfo::Write(IFXAnimationModifier* pData)
{
	U32       uMotionsQueued = 0;
	F32       fOffsetTime = 0.0f;
	F32       fStartTime = 0.0f;
	F32       fStopTime = 0.0f;
	F32       fTimeScale = 0.0f;
	BOOL      bLoop = FALSE;
	BOOL      bSync = FALSE;
	BOOL      bDelayMap = FALSE;
	BOOL      bIsSingleTrack = FALSE;

	CHK_DUMP_ENTRY(IFX_EDI_MODIFIER);
	if(NULL == pData)
		return ;

	Write("\t\tAnimation modifier settings:\n");

	if( pData->IsKeyframe() == TRUE )
		bIsSingleTrack = TRUE;

	uMotionsQueued = pData->GetNumberQueued();

	// Write out general information
	Write("\t\t\tIs Single Track=%d, Motions Queued = %d\n",
		bIsSingleTrack, uMotionsQueued);

	// Write out information about queued motions

	if( uMotionsQueued > 0)
	{
		U32 i;

		Write("\t\t\tAnimation modifier motion queue:\n");

		for( i = 0; i < uMotionsQueued; i++)
		{
			IFXMotionMixer* pSkelMixer = pData->UseMotionMixerNoMap(i);  // Does not AddRef

			if( pSkelMixer)
			{
				IFXString sMotionName = pSkelMixer->GetPrimaryMotionName();

				DISCARD pSkelMixer->GetTimeOffset(&fOffsetTime);
				fStartTime = pSkelMixer->LocalStartTime();
				fStopTime = pSkelMixer->LocalEndTime();
				fTimeScale = pSkelMixer->TimeScale();
				bLoop = pSkelMixer->Loop();
				bSync = pSkelMixer->Sync();
				bDelayMap = pSkelMixer->DelayMap();

				Write("\t\t\t\t%d (", i);
				Write(&sMotionName);
				Write("):  Start = %f, Stop = %f, Offset = %f\n",
					fStartTime, fStopTime, fOffsetTime);
				Write("\t\t\t\t\tScale = %f, Loop: %d, Sync: %d, Delay: %d\n",
					fTimeScale, bLoop, bSync, bDelayMap);
			}  // Skeleton mixer
		}  // All motions
	}  // Any motions
	else
		Write("\t\t\tNo motions queued\n");
}


/**
	Writes a description of a IFXVector3 to the debug file.

	@param   IFXVector3* pData   Vector to be written to file.
*/
void DebugInfo::Write(IFXVector3* pData)
{
	CHK_DUMP_ENTRY(IFX_EDI_DUMP);
	if(NULL == pData)
		return ;

	Write("X: %f, Y: %f, Z: %f", pData->X(), pData->Y(), pData->Z());
}


/**
	Writes a description of a IFXView to the debug file.

	@param   IFXView* pView    View to be written to file.
*/
void DebugInfo::Write(IFXView* pView)
{
	IFXRESULT result = IFX_OK;
	U32       progMode = 0;

	CHK_DUMP_ENTRY(IFX_EDI_DUMP);
	if(NULL == pView)
		return ;

	Write("\t\tView Parameters:\n");

	// Projection mode
	U32 att = pView->GetAttributes() & ~IFX_PERCENTDIMEN; // mask screen position unit

	if( att == IFX_ORTHOGRAPHIC )
		progMode = IFX_ORTHOGRAPHIC;
	else if ( att == IFX_PERSPECTIVE3 )
		progMode = IFX_PERSPECTIVE3;

	if( IFXSUCCESS(result) && (IFX_ORTHOGRAPHIC == progMode))
	{
		F32 fOHeight;

		result = pView->GetOrthoHeight(&fOHeight);

		if( IFXSUCCESS(result) )
			Write("\t\t\tOrthographic view, height %f\n", fOHeight);
	}
	else if( IFXSUCCESS(result) && (IFX_PERSPECTIVE3 == progMode))
	{
		F32 fProjDegrees;

		result = pView->GetProjection(&fProjDegrees);

		if( IFXSUCCESS(result) )
			Write("\t\t\tPerspective view, field of view %f degrees\n", fProjDegrees);
	}
	else
	{
		Write("\t\t\tUnknown view\n");
	}

	if( IFXSUCCESS(result) )
	{
		F32 fprojPlaneDist = pView->GetDistanceToProjectionPlane();

		Write("\t\t\tProjection plane distance = %f\n", fprojPlaneDist);
	}

	IFXF32Rect   viewRect;
	I32     iSizeX = 0;
	I32     iSizeY = 0;
	I32     iOffsetX = 0;
	I32     iOffsetY = 0;
	F32     fNearClip = 0.0f;
	F32     fFarClip = 0.0f;

	if( IFXSUCCESS(result) )
		result = pView->GetViewport(viewRect);

	if( IFXSUCCESS(result) )
	{
		iOffsetX = (I32)(viewRect.m_X);
		iOffsetY = (I32)(viewRect.m_Y);
		iSizeX = (I32)(viewRect.m_Width);
		iSizeY = (I32)(viewRect.m_Height);
	}

	if( IFXSUCCESS(result) )
		fNearClip = pView->GetNearClip();

	if( IFXSUCCESS(result) )
		fFarClip = pView->GetFarClip();

	if( IFXSUCCESS(result) )
	{
		Write("\t\t\tOffset:  X=%d, Y=%d; Size:  X=%d, Y=%d\n",
			iOffsetX, iOffsetY, iSizeX, iSizeY);

		Write("\t\t\tNear Clip:  %f;  Far Clip:  %f\n",
			fNearClip, fFarClip);
	}

	BOOL bColorCleared=FALSE;
	BOOL bDepthCleared=FALSE;
	BOOL bStencilCleared=FALSE;
	IFXVector3 vBGColor( 0.0f, 0.0f, 0.0f);
	F32 fBufDepth=0.0f;
	U32 uStencilValue = 0;
	IFXRenderClear renderClear;
	IFXDECLARELOCAL( IFXViewResource, pViewResource );
	pViewResource = pView->GetViewResource();

	if( IFXSUCCESS(result) )
	{
		if( NULL != pViewResource )
			renderClear = pViewResource->GetRenderClear();
		else
		{
			result = IFX_E_INVALID_POINTER;
			IFXTRACE_GENERIC( L"NULL View Resource Pointer" );
		}
	}

	if( IFXSUCCESS(result) )
	{
		vBGColor = renderClear.GetColorValue();
		fBufDepth = renderClear.GetDepthValue();
		bColorCleared = renderClear.GetColorCleared();
		bDepthCleared = renderClear.GetDepthCleared();
		bStencilCleared = renderClear.GetStencilCleared();
		uStencilValue = renderClear.GetStencilValue();
	}

	if( IFXSUCCESS(result) )
	{
		Write("\t\t\tClear State:  ");

		if( bColorCleared)
			Write("Back (color) buffer cleared=TRUE, ");
		else
			Write("Back (color) buffer cleared=FALSE, ");

		if( bDepthCleared)
			Write("Depth buffer cleared=TRUE, ");
		else
			Write("Depth buffer cleared=FALSE, ");

		if( bStencilCleared)
			Write("Stencil buffer cleared=TRUE\n");
		else
			Write("Stencil buffer cleared=FALSE\n");

		Write("\t\t\tBuffer Color = ");
		Write(&vBGColor);
		Write("\n\t\t\tBuffer Depth = %f, Stencil Value = %d\n",fBufDepth,uStencilValue);
	}

	BOOL bStencilEnabled = FALSE;

	if( IFXSUCCESS(result) )
	{
		result = pViewResource->GetStencilEnabled(bStencilEnabled);
	}

	if( IFXSUCCESS(result) )
	{
		if( bStencilEnabled)
			Write("\t\t\tStencils enabled\n");
		else
			Write("\t\t\tStencils disabled\n");
	}

	BOOL bFogValue = FALSE;
	IFXRenderFog renderFog;

	if( IFXSUCCESS(result) )
	{
		result = pViewResource->GetFogEnableValue(&bFogValue);
	}

	if( IFXSUCCESS(result) )
	{
		if( bFogValue)
		{
			Write("\t\t\tFog enabled:\n");

			if( IFXSUCCESS(result) )
			{
				IFXenum   eFogMode;
				IFXVector4  cFogColor;
				F32     fLinearNear;
				F32     fFar;

				renderFog = pViewResource->GetRenderFog();

				eFogMode = renderFog.GetMode();
				cFogColor = renderFog.GetColor();
				fLinearNear = renderFog.GetLinearNear();
				fFar = renderFog.GetFar();

				if( IFX_FOG_LINEAR == eFogMode)
					Write("\t\t\t\tLinear Fog, ");
				else if( IFX_FOG_EXP == eFogMode)
					Write("\t\t\t\tExponential Fog, ");
				else if( IFX_FOG_EXP2 == eFogMode)
					Write("\t\t\t\tExponential squared Fog ,");

				Write("Linear Near = %f, Far = %f\n", fLinearNear, fFar);
				Write("\t\t\t\tFog color:  ");
				WriteColor(&cFogColor);
			}
		}
		else
			Write("\t\t\tFog disabled\n");
	}
}


/**
	Writes a description of an IFXModifierDataPacket to the debug file.

	@param   IFXModifierDataPacket* pModDataPacket   The data packet to dump.
*/
void DebugInfo::Write( IFXModifierDataPacket* pModDataPacket )
{
	IFXRESULT result = IFX_OK;
	U32 meshIndex = 0;
	U32 uCLODIndex = 0;
	U32 boundingIndex = 0;
	U32 transformIndex = 0;
	U32 i;

	CHK_DUMP_ENTRY(IFX_EDI_DUMP);

	if(NULL == pModDataPacket)
		result = IFX_E_INVALID_POINTER;

	// Get us the current transform in the data packet, if we have one.
	result = pModDataPacket->GetDataElementIndex(
		DID_IFXTransform, transformIndex );

	if( IFXSUCCESS( result ) )
	{
		IFXArray<IFXMatrix4x4>* pWorldTransforms;

		result = pModDataPacket->GetDataElement(
			transformIndex, (void**)&pWorldTransforms ); // Does not AddRef

		if( pWorldTransforms && IFXSUCCESS(result) )
		{
			Write("\t\tNumber of instances - %d:\n",
				pWorldTransforms->GetNumberElements() );

			for( i = 0; i < pWorldTransforms->GetNumberElements(); ++i )
			{
				Write("\t\t\tCurrent Transform for instance %d:\n", i);
				IFXMatrix4x4& rWorldTransformMatrix =
					pWorldTransforms->GetElement( i );
				Write( &rWorldTransformMatrix, "\t\t\t" );
			}
		}
	}

	// Get us the bounding volume data packet, if available
	if( IFX_OK == pModDataPacket->GetDataElementIndex(
		DID_IFXRenderableGroupBounds, boundingIndex )
		)
	{
		IFXDECLARELOCAL( IFXBoundSphereDataElement, pBoundSphereElement );
		IFXDECLARELOCAL( IFXUnknown, pUnknown );

		result = pModDataPacket->GetDataElement( boundingIndex,
			(void**)&pUnknown );

		if( pUnknown && IFXSUCCESS( result ) )
		{
			result = pUnknown->QueryInterface(
				IID_IFXBoundSphereDataElement,
				(void **)&pBoundSphereElement );
		}

		if( pBoundSphereElement && IFXSUCCESS(result) )
		{
			Write("\t\tBounding Sphere at %f, %f, %f, radius %f\n",
				pBoundSphereElement->Bound().X(),
				pBoundSphereElement->Bound().Y(),
				pBoundSphereElement->Bound().Z(),
				pBoundSphereElement->Bound().A());
		}
	}

	// Get the CLODolution data element of the data packet, if it has one
	if( IFX_OK == pModDataPacket->GetDataElementIndex(
										DID_IFXCLODController, uCLODIndex )
	  )
	{
		IFXDECLARELOCAL( IFXCLODManager, pCLODManager );
		IFXDECLARELOCAL( IFXUnknown, pUnknown );

		result = pModDataPacket->GetDataElement( uCLODIndex,
			(void**)&pUnknown );

		if( pUnknown && IFXSUCCESS(result) )
		{
			result = pUnknown->QueryInterface(IID_IFXCLODManager,
				(void**)&pCLODManager);
		}

		if( pCLODManager && IFXSUCCESS(result))
		{
			Write("\t\tMesh Resolution:  Current=%d, Max=%d\n",
				pCLODManager->GetResolution(),
				pCLODManager->GetMaxResolution());
		}
	}

	// Get the renderable data element of the data packet, if it has one
	if( IFX_OK == pModDataPacket->GetDataElementIndex(
		DID_IFXRenderableGroup, meshIndex ) )
	{
		IFXDECLARELOCAL( IFXMeshGroup, pMeshGroup );
		IFXDECLARELOCAL( IFXUnknown, pUnknown );

		result = pModDataPacket->GetDataElement( meshIndex, (void**)&pUnknown );

		// QI the returned pointer into an IFXMeshGroup (we don't really need this here)
		if( pUnknown && IFXSUCCESS(result) )
		{
			result = pUnknown->QueryInterface(IID_IFXMeshGroup, (void**)&pMeshGroup);
		}

		// Gather information about the submeshes
		if( pMeshGroup && IFXSUCCESS(result))
		{
			U32 numMeshes = pMeshGroup->GetNumMeshes();

			Write("\t\tMeshGroup contains %d submeshes\n", numMeshes);

			for( i = 0; i < numMeshes && IFXSUCCESS(result); i++)
			{
				IFXMeshAttributes eRenderTexCoordsInUse;
				IFXVertexAttributes vertexAttributes;

				IFXMesh* pMesh = NULL;

				result = pMeshGroup->GetMesh( i, pMesh );

				if( pMesh)
				{
					Write("\t\t\t%d:  Vertices - Allocated:  %d, Max: %d, Current: %d\n",
						i, pMesh->GetNumAllocatedVertices(), pMesh->GetMaxNumVertices(),
						pMesh->GetNumVertices());
					Write("\t\t\t\tFaces - Allocated:  %d, Max: %d, Current: %d\n",
						pMesh->GetNumAllocatedFaces(), pMesh->GetMaxNumFaces(),
						pMesh->GetNumFaces());
					Write("\t\t\t\tLines - Allocated:  %d, Max: %d, Current: %d\n",
						pMesh->GetNumAllocatedLines(), pMesh->GetMaxNumLines(),
						pMesh->GetNumLines());

					vertexAttributes = pMesh->GetAttributes();

					Write("\t\t\t\tNumber Texture Coordinate Layers %d, HW TC Layers %d\n",
						vertexAttributes.m_uData.m_uNumTexCoordLayers,
						vertexAttributes.m_uData.m_uNumHWTexCoordLayers);
					Write("\t\t\t\tHas Positions %d, Normals %d, DColors %d, SColors %d\n",
						vertexAttributes.m_uData.m_bHasPositions,
						vertexAttributes.m_uData.m_bHasNormals,
						vertexAttributes.m_uData.m_bHasDiffuseColors,
						vertexAttributes.m_uData.m_bHasSpecularColors);

					Write("\t\t\t\tDiffuseBGR %d, SpecularBGR %d, 32bitIndices %d\n",
						vertexAttributes.m_uData.m_bDiffuseIsBGR,
						vertexAttributes.m_uData.m_bSpecularIsBGR,
						vertexAttributes.m_uData.m_bHasSpecularColors);

					Write("\t\t\t\tBytes Per layer:  0=%d, 1=%d, 2=%d, 3=%d, 4=%d, 5=%d, 6=%d, 7=%d\n",
						vertexAttributes.m_uData.m_uTexCoordSize0,
						vertexAttributes.m_uData.m_uTexCoordSize1,
						vertexAttributes.m_uData.m_uTexCoordSize2,
						vertexAttributes.m_uData.m_uTexCoordSize3,
						vertexAttributes.m_uData.m_uTexCoordSize4,
						vertexAttributes.m_uData.m_uTexCoordSize5,
						vertexAttributes.m_uData.m_uTexCoordSize6,
						vertexAttributes.m_uData.m_uTexCoordSize7);

					eRenderTexCoordsInUse = pMesh->GetRenderTexCoordsInUse();

					Write("\t\t\t\tTex Coords In Use: ");
					if( eRenderTexCoordsInUse[IFX_MESH_RENDER_TC0+0] )
						Write(" TC0");
					if( eRenderTexCoordsInUse[IFX_MESH_RENDER_TC0+1])
						Write(" TC1");
					if( eRenderTexCoordsInUse[IFX_MESH_RENDER_TC0+2])
						Write(" TC2");
					if( eRenderTexCoordsInUse[IFX_MESH_RENDER_TC0+3])
						Write(" TC3");
					if( eRenderTexCoordsInUse[IFX_MESH_RENDER_TC0+4])
						Write(" TC4");
					if( eRenderTexCoordsInUse[IFX_MESH_RENDER_TC0+5])
						Write(" TC5");
					if( eRenderTexCoordsInUse[IFX_MESH_RENDER_TC0+6])
						Write(" TC6");
					if( eRenderTexCoordsInUse[IFX_MESH_RENDER_TC0+7])
						Write(" TC7");
					Write("\n");
				}  // if we have a valid mesh pointer

				IFXRELEASE(pMesh);
			}  // for all submeshes
		}  // dump submesh information

		// Now QI that into an IFXRenderable so that we can get the
		// shader data.
		if( pMeshGroup && IFXSUCCESS(result) )
		{
			IFXDECLARELOCAL( IFXRenderable, pRenderable );
			result = pMeshGroup->QueryInterface(
				IID_IFXRenderable,
				(void**)&pRenderable );

			if( IFXSUCCESS(result) )
			{
				Write( pRenderable );
			}
		}
	} // there is a mesh group in this data packet
}


/**
	Writes a description of a renderable element to the debug file.

	@param   IFXRenderable* in_pRenderable   The renderable to be written to file.
*/
void DebugInfo::Write(IFXRenderable* in_pRenderable)
{
	IFXRESULT   result = IFX_OK;
	IFXShaderList  *pShaderList = NULL;
	U32       numElements = 0;
	U32       numShaders = 0;
	U32       i, j;
	U32       shaderID = 0;

	// Capture and dump the shader mapping for this mesh
	numElements = in_pRenderable->GetNumElements();

	Write("\t\tRenderable contains %d elements\n", numElements);

	if( numElements > 0)
	{
		Write("\t\tShader Set Mapping for elements:\n");

		for( i = 0; (i < numElements) && IFXSUCCESS(result); i++)
		{
			result = in_pRenderable->GetElementShaderList(i, &pShaderList);

			if( pShaderList && IFXSUCCESS(result) )
			{
				int gotone = 0;

				numShaders = pShaderList->GetNumShaders();
				Write("\t\t\t%d:  ShaderIDs (%d): ", i, numShaders);

				for( j = 0; (j < numShaders) && IFXSUCCESS(result); j++)
				{
					if( 0 == gotone)
						gotone = 1;
					else
						Write(", ");

					result = pShaderList->GetShader(j, &shaderID);

					Write("%d", shaderID);
				}

				Write("\n");
			}
			else
			{
				Write("\t\t\t%d:  GetElementShaderList returned an error: %x\n",
					i, result);
				break;
			}

			IFXRELEASE(pShaderList);
		}  // for all elements
	} // we have some elements
}


/**
	Writes a description of a modifier chain to the debug file.

	@param   IFXModifierChain* pModChain   The modifier chain to be written to file.
*/
void DebugInfo::Write(IFXModifierChain* pModChain)
{
	IFXRESULT     result = IFX_OK;
	U32           i;
	U32           modCount = 0;
	U32           oneMod = 0;
	IFXModifier*      pModifier = NULL;
	IFXAnimationModifier*   pBonesModifier = NULL;
	IFXSkeleton*      pSkeleton = NULL;
	IFXSubdivModifier*  pSubdiv = NULL;
	IFXCLODModifier*    pCLODModifier = NULL;
	IFXShadingModifier* pShadingModifier = NULL;

	CHK_DUMP_ENTRY(IFX_EDI_DUMP);
	if( NULL == pModChain )
		return ;

	if( IFXSUCCESS( result ) )
		result = pModChain->GetModifierCount( modCount );

	// List all the modifiers in this chain
	if( IFXSUCCESS( result ) )
	{
		for( i = 1; IFXSUCCESS(result) && (i < modCount); i++)
		{
			result = pModChain->GetModifier( i, pModifier );

			if( oneMod )
				Write(", ");

			if( IFX_OK == pModifier->QueryInterface( IID_IFXSkeleton,
				(void**)&pSkeleton ) )
			{
				Write("BoneWeight");
				oneMod = 1;
				IFXRELEASE(pSkeleton);
			}
			else if( IFX_OK == pModifier->QueryInterface( IID_IFXAnimationModifier,
				(void**)&pBonesModifier ) )
			{
				Write("Animation");
				oneMod = 1;
				IFXRELEASE(pBonesModifier);
			}
			else if( IFX_OK == pModifier->QueryInterface( IID_IFXSubdivModifier,
				(void**)&pSubdiv ) )
			{
				Write("Subdivision");
				oneMod = 1;
				IFXRELEASE(pSubdiv);
			}
			else if( IFX_OK == pModifier->QueryInterface( IID_IFXCLODModifier,
				(void**)&pCLODModifier ) )
			{
				Write("LOD");
				oneMod = 1;
				IFXRELEASE(pCLODModifier);
			}
			else if( IFX_OK == pModifier->QueryInterface( IID_IFXShadingModifier,
				(void**)&pShadingModifier ) )
			{
				Write("Shading");
				oneMod = 1;
				IFXRELEASE(pShadingModifier);
			}
			else
			{
				Write("Unknown");
				oneMod = 1;
			}

			IFXRELEASE(pModifier)
		} // for all modifiers in the chain

		Write("\n\t\t\t----\n");
	}

	// Now dump out the detailed information about
	// each modifier we found in this chain.
	if( IFXSUCCESS(result) )
	{
		for( i = 1; IFXSUCCESS(result) && (i < modCount); i++ )
		{
			result = pModChain->GetModifier(i, pModifier);

			if( IFX_OK == pModifier->QueryInterface( IID_IFXSkeleton,
				(void**)&pSkeleton ) )
			{
				WriteBoneWeightGenerator(pSkeleton);
				Write("\n");
				IFXRELEASE(pSkeleton);
			}
			else if( IFX_OK == pModifier->QueryInterface( IID_IFXAnimationModifier,
				(void**)&pBonesModifier ) )
			{
				Write(pBonesModifier);
				Write("\n");
				IFXRELEASE(pBonesModifier);
			}
			else if( IFX_OK == pModifier->QueryInterface( IID_IFXSubdivModifier,
				(void**)&pSubdiv ) )
			{
				Write(pSubdiv);
				Write("\n");
				IFXRELEASE(pSubdiv);
			}
			else if( IFX_OK == pModifier->QueryInterface( IID_IFXCLODModifier,
				(void**)&pCLODModifier ) )
			{
				Write(pCLODModifier);
				Write("\n");
				IFXRELEASE(pCLODModifier);
			}
			else if( IFX_OK == pModifier->QueryInterface( IID_IFXShadingModifier,
				(void**)&pShadingModifier ) )
			{
				//Write(pShadingDModifier);
				Write("\n");
				IFXRELEASE(pShadingModifier);
			}

			IFXRELEASE(pModifier)
		} // for all modifiers in the chain

		Write("\n");
	}
}


/**
	Writes a description of a IFXModel to the debug file.

	@param   IFXModel* pModel    The model node to be written to file.
*/
void DebugInfo::Write(IFXModel* pModel)
{
	CHK_DUMP_ENTRY(IFX_EDI_DUMP);
	if(NULL == pModel)
		return;

	// We're keeping this function 
	// as a place holder just in case there's something specific from a 
	// model not already written that we want to write in the future.
}


/**
	Writes a description of a IFXNode the debug file.

	@param   IFXNode* pCurrNode    The node to be written to file.
*/
void DebugInfo::Write(IFXNode* pCurrNode)
{
	IFXRESULT result = IFX_OK;
	IFXDECLARELOCAL( IFXModifierChain, pModChain );

	CHK_DUMP_ENTRY(IFX_EDI_DUMP);

	if( NULL == pCurrNode )
		result = IFX_E_INVALID_POINTER;

	// -- Dump the node's priority
	if( IFXSUCCESS(result) )
	{
		IFXDECLARELOCAL( IFXMarker, pMarker );

		if( IFXSUCCESS(result) )
			result = pCurrNode->QueryInterface( IID_IFXMarker, (void**)&pMarker );

		U32 priority = 0;

		if( IFXSUCCESS(result) )
			priority = pMarker->GetPriority();

		if( IFXSUCCESS(result) )
			Write("\t\tNode priority:  %d\n", priority);
	}

	// -- Get the node's modifier chain
	if( IFXSUCCESS(result) )
		result = pCurrNode->GetModifierChain( &pModChain );

	// -- Get the data packet at the end of this chain and dump it out.
	// -- This will give us the final values of the data elements we
	// -- are interested in, at the current time.
	if( IFXSUCCESS(result) )
	{
		IFXDECLARELOCAL( IFXModifierDataPacket, pModDataPacket );

		if( IFXSUCCESS(result) )
			result = pModChain->GetDataPacket(pModDataPacket);

		if( IFXSUCCESS(result) )
			Write(pModDataPacket);
	}

	// -- Dump the modifier chain
	if( IFXSUCCESS(result))
	{
		U32 modCount = 0;
		result = pModChain->GetModifierCount( modCount );

		if( ( modCount > 0 ) && IFXSUCCESS( result ) )
		{
			Write("\t\tModifiers associated with this node (%d):\n", modCount-1);
			Write("\t\t\t");

			Write(pModChain);
		}
	}
}


/**
	Writes a description of a IFXVector4 to the debug file.

	@param   IFXVector4* pData   Vector to be written to file.
*/
void DebugInfo::Write(IFXVector4* pData)
{
	CHK_DUMP_ENTRY(IFX_EDI_DUMP);
	if(NULL == pData)
		return ;

	Write("(W: %10f, X: %10f, Y: %10f, Z: %10f)", pData->Value(0), pData->Value(1), pData->Value(2), pData->Value(3));
}


/**
	Writes a description of a IFXMatrix4x4 to the debug file.

	@param   IFXVector4x4* pData   Vector to be written to file.
	@param   char *pTabchars     A character string to write as a prefix
	before each row of the matrix
*/
void DebugInfo::Write( IFXMatrix4x4* pData, const char* pTabchars )
{
	int i, count;
	float *pRawArray = NULL;

	CHK_DUMP_ENTRY(IFX_EDI_DUMP);
	if( NULL == pData )
		return ;

	count = 0;
	pRawArray = pData->Raw();

	if( NULL == pRawArray )
		return ;

	for( i = 0; i < 4; ++i )
	{
		Write("%s(", pTabchars);

		Write("%2d:  %10f, %2d:  %10f, %2d:  %10f, %2d:  %10f",
			count, pRawArray[count], count+1, pRawArray[count+1],
			count+2, pRawArray[count+2], count+3, pRawArray[count+3]);

		count += 4;

		Write(")\n");
	}
}


/**
	Writes a description of a IFXVector3 (considered as a color) to the debug file.

	@param   IFXVector3* pData   Vector to be written to file.
*/
void DebugInfo::WriteColor(IFXVector3 * pData)
{
	CHK_DUMP_ENTRY(IFX_EDI_DUMP);
	if(NULL == pData)
		return ;

	Write("R: %10f, G: %10f, B: %10f", pData->Value(0), pData->Value(1), pData->Value(2));
}


/**
	Writes a description of a IFXVector4 (considered as a color) to the debug file.

	@param   IFXVector4* pData   Vector to be written to file.
*/
void DebugInfo::WriteColor(IFXVector4 * pData)
{
	CHK_DUMP_ENTRY(IFX_EDI_DUMP);
	if(NULL == pData)
		return ;

	Write("R: %10f, G: %10f, B: %10f, A: %10f", pData->Value(0), pData->Value(1), pData->Value(2), pData->Value(3));
}


/**
	Writes a description of a IFXQuaternion to the debug file.

	@param   IFXQuaternion * pData   IFX quaternion to be written to file.
*/
void DebugInfo::Write(IFXQuaternion * pData)
{
	CHK_DUMP_ENTRY(IFX_EDI_DUMP);
	if(NULL == pData)
		return ;

	Write("Quat: %10f, %10f, %10f, %10f", (*pData)[0], (*pData)[1] , (*pData)[2], (*pData)[3]);
}


/**
	Writes a description of a float to the debug file.

	@param   F32 * pData   Float to be written to file.
*/
void DebugInfo::Write(F32 * pData)
{
	CHK_DUMP_ENTRY(IFX_EDI_DUMP);
	if(NULL == pData)
		return ;

	Write("F32: %f", *pData);
}


/**
	Writes a description of a IFXBoneInfo to the debug file.

	@param   IFXBoneInfo * pData   Bone info object to be written to file.
*/
void DebugInfo::Write(IFXBoneInfo * pData)
{
	IFXRESULT result=IFX_OK;
	U8      tStr[255];

	CHK_DUMP_ENTRY(IFX_EDI_SKELETON);
	if(NULL == pData)
		return ;

	result = pData->stringBoneName.ConvertToRawU8(tStr, 255);


	if( IFXSUCCESS(result) )
	{
		Write("\t\t\t\tBone Name: %s\n", tStr);
		Write("\t\t\t\tParent ID: %d\n", pData->iParentBoneID);
		Write("\t\t\t\tBone Length: "); Write(& pData->fBoneLength);
		Write("\n\t\t\t\tBone Disp: "); Write(& pData->v3BoneDisplacement);
		Write("\n\t\t\t\tBone Rotation: "); Write(& pData->v4BoneRotation);
		Write("\n");
		//    Write("Parent ID: %d\n", pData->iParentID);
	}
}


/**
	Writes a description of an IFXShaderLitTexture to the debug file.

	@param   IFXShaderLitTexture* pShader    Shader to write.
*/
void DebugInfo::Write(IFXShaderLitTexture* pData)
{
	IFXRESULT   result = IFX_OK;
	IFXMarker   *pMarker = NULL;
	U32       priority = 0;


	CHK_DUMP_ENTRY(IFX_EDI_SHADER);
	if(NULL == pData)
		return ;

	Write("\t\tShader Properties\n");

	// Dump the shader's priority
	if( IFXSUCCESS(result) )
		result = pData->QueryInterface( IID_IFXMarker, (void**)&pMarker );


	if( IFXSUCCESS(result) )
		priority = pMarker->GetPriority();

	if( IFXSUCCESS(result) )
		Write("\t\t\tShader priority:  %d\n", priority);

	IFXRELEASE(pMarker);

	//----
	// Channels enabled
	//----
	U32 uChannels = pData->GetChannels();
	U32 uFlags = pData->GetFlags();

	Write("\t\t\tEnabled Channels:");

	if( 0 != (uChannels|uFlags))
	{
		if( (1<<0) & uChannels)
			Write("  Base");

		if( (1<<3) & uChannels)
			Write("  Gloss");

		if( (1<<4) & uChannels)
			Write("  SpecularLighting");

		if( (1<<1) & uChannels)
			Write("  DiffuseLighting");

		if( (1<<2) & uChannels)
			Write("  Reflection");

		if( IFXShaderLitTexture::MATERIAL & uFlags)
			Write("  Material");

		if( IFXShaderLitTexture::USEDIFFUSE & uFlags)
			Write("  UseDiffuse");

		if( IFXShaderLitTexture::FLAT & uFlags)
			Write("  Flat");

		if( IFXShaderLitTexture::WIRE & uFlags)
			Write("  Wire");

		if( IFXShaderLitTexture::POINT & uFlags)
			Write("  Point");
	}
	else
		Write("None");

	Write("\n");

	//----
	//  Other shader properties
	//----
	if( pData->GetUseDiffuse())
		Write("\t\t\tUse Diffuse, ");
	else
		Write("\t\t\tDo not use Diffuse, ");

	if( pData->GetAlphaTestEnabled())
		Write("Alpha Test Enabled, ");
	else
		Write("Alpha Test Disabled, ");

	if( pData->GetLightingEnabled())
		Write("Lighting Enabled\n");
	else
		Write("Lighting Disabled\n");

	//----
	//  IDs and Intensities
	//----
	U32 materialID = pData->GetMaterialID();

	U32 baseID = 0;
	if( IFXSUCCESS(result) )
		result = pData->GetTextureID( 0, &baseID );
	F32 fBaseIntensity=-1.0f;
	if( IFXSUCCESS(result) )
		result = pData->GetTextureIntensity( 0, &fBaseIntensity );

	U32 uGlossID = 0;
	if( IFXSUCCESS(result) )
		result = pData->GetTextureID( 3, &uGlossID );
	F32 fGlossIntensity=-1.0f;
	if( IFXSUCCESS(result) )
		result = pData->GetTextureIntensity( 3, &fGlossIntensity );

	U32 uSLightID = 0;
	if( IFXSUCCESS(result) )
		result = pData->GetTextureID( 4, &uSLightID );
	F32 fSLightIntensity=-1.0f;
	if( IFXSUCCESS(result) )
		result = pData->GetTextureIntensity( 4, &fSLightIntensity );

	U32 uReflectID = 0;
	if( IFXSUCCESS(result) )
		result = pData->GetTextureID( 2, &uReflectID );
	F32 fReflectIntensity=-1.0f;
	if( IFXSUCCESS(result) )
		result = pData->GetTextureIntensity( 2, &fReflectIntensity );

	U32 uDLightID = 0;
	if( IFXSUCCESS(result) )
		result = pData->GetTextureID( 1, &uDLightID );
	F32 fDLightIntensity=-1.0f;
	if( IFXSUCCESS(result) )
		result = pData->GetTextureIntensity( 1, &fDLightIntensity );

	Write("\t\t\tMaterial ID:  %d\n", materialID);

	if( (1<<0) & uChannels)
	{
		Write("\t\t\tBase ID:  %d    Base (Diffuse) Intensity: %f\n",
			baseID, fBaseIntensity);
		Write(pData, 0);
	}

	if( (1<<3) & uChannels)
	{
		Write("\t\t\tGlossID:  %d    Gloss Intensity:  %f\n",
			uGlossID, fGlossIntensity);
		Write(pData, 3);
	}

	if( (1<<4) & uChannels)
	{
		Write("\t\t\tSpecular Lighting ID:  %d    Specular Lighting Intensity:  %f\n",
			uSLightID, fDLightIntensity);
		Write(pData, 4);
	}

	if( (1<<1) & uChannels)
	{
		Write("\t\t\tDiffuse Lighting ID:  %d    Diffuse Lighing Intensity:  %f\n",
			uDLightID, fDLightIntensity);
		Write(pData, 1);
	}

	if( (1<<2) & uChannels)
	{
		Write("\t\t\tReflection ID:  %d    Reflection Intensity:  %f\n",
			uReflectID, fReflectIntensity);
		Write(pData, 2);
	}

	Write("\t-----------------------\n");
}


/**
	Writes a description of the blending values for one channel of an IFXShaderLitTexture.

	@param   IFXShaderLitTexture* pShader        The shader whose channel we will dump
	@param   IFXShaderLitTexture::Channels channel   The channel to dump
*/
void DebugInfo::Write(IFXShaderLitTexture* pShader, U32 channel)
{
	IFXShaderLitTexture::BlendFunction  blendFunction;
	IFXShaderLitTexture::BlendSource  blendSource;
	IFXShaderLitTexture::TextureMode  textureMode;
	F32                 blendConstant;
	U8                textureRepeat = 0;
	IFXRESULT             result = IFX_OK;
	IFXMatrix4x4            *pTextureTransform;
	IFXMatrix4x4            *pWrapTransform;

	if( !pShader)
		return;

	if( IFXSUCCESS(result) )
	{
		result = pShader->GetBlendFunction( channel, &blendFunction );

		if( IFXSUCCESS(result) )
		{
			Write("\t\t\t\t");
			if( IFXShaderLitTexture::MULTIPLY == blendFunction)
				Write("Blend Function:  Multiply");

			if( IFXShaderLitTexture::ADD == blendFunction)
				Write("Blend Function:  Add");

			if( IFXShaderLitTexture::REPLACE == blendFunction)
				Write("Blend Function:  Replace");

			if( IFXShaderLitTexture::BLEND == blendFunction)
				Write("Blend Function:  Blend");
		}
	}

	if( IFXSUCCESS(result) )
	{
		result = pShader->GetBlendSource( channel, &blendSource);

		if( IFXSUCCESS(result) )
		{
			if( IFXShaderLitTexture::ALPHA == blendSource)
				Write(",  Blend Source:  Alpha\n");
			if( IFXShaderLitTexture::CONSTANT == blendSource)
				Write(",  Blend Source:  Constant\n");
		}
	}

	if( IFXSUCCESS(result) )
	{
		result = pShader->GetTextureMode( channel, &textureMode );

		if( IFXSUCCESS(result) )
		{
			Write("\t\t\t\t");
			if( IFXShaderLitTexture::TM_NONE == textureMode)
				Write("Texture Mode:  None");

			if( IFXShaderLitTexture::TM_PLANAR == textureMode)
				Write("Texture Mode:  Planar");

			if( IFXShaderLitTexture::TM_CYLINDRICAL == textureMode)
				Write("Texture Mode:  Cylindrical");

			if( IFXShaderLitTexture::TM_SPHERICAL == textureMode)
				Write("Texture Mode:  Spherical");

			if( IFXShaderLitTexture::TM_REFLECTION == textureMode)
				Write("Texture Mode:  Reflection");
		}
	}

	if( IFXSUCCESS(result) )
	{
		result = pShader->GetTextureRepeat( channel, &textureRepeat );

		if( IFXSUCCESS(result) )
		{
			if( textureRepeat & 0x01 )
				Write(" , Texture Repeat U = TRUE\n");
			else
				Write(" , Texture Repeat U = FALSE\n");

			if( textureRepeat & 0x02 )
				Write(" , Texture Repeat V = TRUE\n");
			else
				Write(" , Texture Repeat V = FALSE\n");

			if( textureRepeat & 0x04 )
				Write(" , Texture Repeat W = TRUE\n");
			else
				Write(" , Texture Repeat W = FALSE\n");
		}
	}

	if( IFXSUCCESS(result) )
	{
		result = pShader->GetBlendConstant( channel, &blendConstant );

		if( IFXSUCCESS(result) )
		{
			Write("\t\t\t\tBlend Constant: %f\n", blendConstant);
		}
	}

	if( IFXSUCCESS(result) )
	{
		result = pShader->GetTextureTransform( channel, &pTextureTransform );

		if( pTextureTransform && IFXSUCCESS(result) )
		{
			Write("\t\t\t\tTexture Transform\n");
			Write(pTextureTransform, "\t\t\t\t\t");
		}
	}

	if( IFXSUCCESS(result) )
	{
		result = pShader->GetWrapTransform( channel, &pWrapTransform );

		if( pWrapTransform && IFXSUCCESS(result) )
		{
			Write("\t\t\t\tWrap Transform\n");
			Write(pWrapTransform, "\t\t\t\t\t");
		}
	}
}


/**
	Writes a description of an IFXGenerator to the debug file.

	@param   IFXGenerator* pData The generator to be written to file.
*/
void DebugInfo::Write(IFXGenerator* pData)
{
	IFXMatrix4x4  transformMatrix;
	U32       priority = 0;

	CHK_DUMP_ENTRY(IFX_EDI_GENERATOR);
	if(NULL == pData)
		return ;


	priority = pData->GetPriority();

	Write("\t\t\tGenerator priority:  %d\n", priority);

	transformMatrix = pData->GetTransform();

	Write("\t\t\tCurrent Transform:\n");

	Write(&transformMatrix,"\t\t\t\t");

	Write("\n");
}

/**
Writes a description of an IFXAuthorCLODResource to the debug file.

@param   IFAuthorXCLODResource* pData  The model resource to be written to file.
*/
void DebugInfo::Write(IFXAuthorCLODResource* pData)
{
	IFXRESULT       result=IFX_OK;
	IFXVector4        vBoundingSphere;
	IFXTransform      transform;
	IFXDECLARELOCAL( IFXAuthorCLODMesh, pAuthorMesh );
	IFXMatrix4x4      transformMatrix;
	IFXAuthorMeshDesc   *pAuthorMeshDesc = NULL;
	IFXAuthorMaterial *pAuthorMeshMaterials = NULL;
	IFXAuthorVertexUpdate  *pAuthorVertexUpdates = NULL;
	IFXAuthorVertexUpdate  *pAuthorVertexUpdate = NULL;
	F32           fCreaseAngle = 0.0f;
	F32           fCLODLevel = 0.0f;
	U32           uMinResolution = 0;
	U32           uMaxResolution = 0;
	U32           uCurrResolution = 0;
	U32           uNumAllocatedTextureLayers = 0;
	U32           i, j;

	CHK_DUMP_ENTRY(IFX_EDI_GENERATOR);

	if(NULL == pData)
		return ;

	Write("\t\tAuthor CLOD Resource\n");

	if( IFXSUCCESS(result))
	{
		result = pData->GetCreaseAngle(fCreaseAngle);
	}

	if( IFXSUCCESS(result))
	{
		result = pData->GetAuthorMesh(pAuthorMesh);
	}

	if( IFXSUCCESS(result))
	{
		vBoundingSphere = pData->GetBoundingSphere();
		transform = pData->GetTransform();
		fCLODLevel = pData->GetCLODLevel();
	}

	if( IFXSUCCESS(result))
	{
		Write("\t\t\tBounding Sphere at %f, %f, %f, radius %f\n",
			vBoundingSphere.X(),
			vBoundingSphere.Y(),
			vBoundingSphere.Z(),
			vBoundingSphere.A());

		Write("\t\t\tCLODLevel = %f, Crease Angle = %f\n", fCLODLevel, fCreaseAngle);

		transformMatrix = transform.MatrixConst();

		Write("\t\t\tCurrent Transform:\n");

		Write(&transformMatrix,"\t\t\t\t");
	}

	if( pAuthorMesh && IFXSUCCESS(result))
	{
		uMinResolution = pAuthorMesh->GetMinResolution();
		uMaxResolution = pAuthorMesh->GetMaxResolution();
		uCurrResolution = pAuthorMesh->GetResolution();

		Write("\t\t\tResolutions:  Min = %d, Current = %d, Max = %d\n", uMinResolution,
			uCurrResolution, uMaxResolution);
	}

	if( pAuthorMesh && IFXSUCCESS(result))
	{
		result = pAuthorMesh->GetNumAllocatedTexFaceLayers(&uNumAllocatedTextureLayers);
	}

	if( pAuthorMesh && IFXSUCCESS(result))
	{
		pAuthorMeshDesc = (IFXAuthorMeshDesc *)pAuthorMesh->GetMaxMeshDesc();

		Write("\t\t\tMesh Statistics:\n");
		Write("\t\t\t\tNum Faces = %d, Num Vertices = %d, Num Normals = %d\n",
			pAuthorMeshDesc->NumFaces, pAuthorMeshDesc->NumPositions,
			pAuthorMeshDesc->NumNormals);
		Write("\t\t\t\tNum Diffuse Colors = %d, Num Specular Colors = %d\n",
			pAuthorMeshDesc->NumDiffuseColors, pAuthorMeshDesc->NumSpecularColors);
		Write("\t\t\t\tNum Texture Coordinates = %d, Num Materials = %d\n",
			pAuthorMeshDesc->NumTexCoords, pAuthorMeshDesc->NumMaterials);
		Write("\t\t\t\tNum Allocated Texture Layers = %d, Num Base Vertices = %d\n",
			uNumAllocatedTextureLayers, pAuthorMeshDesc->NumBaseVertices);
	}

	// Lock the mesh so that we can look deeper
	if( pAuthorMesh && IFXSUCCESS(result))
	{
		result = pAuthorMesh->Lock();
	}

	if( pAuthorMesh && IFXSUCCESS(result))
	{
		result = pAuthorMesh->GetUpdates(&pAuthorVertexUpdates);
	}

	if( pAuthorMesh && IFXSUCCESS(result))
	{
		result = pAuthorMesh->GetMaterials(&pAuthorMeshMaterials);
	}

	if( pAuthorMeshMaterials && IFXSUCCESS(result))
	{
		Write("\t\t\tSubmesh Data:\n");

		for( i = 0; i < pAuthorMeshDesc->NumMaterials; i++)
		{

			if( IFXSUCCESS(result))
			{
				pAuthorVertexUpdate = &pAuthorVertexUpdates[i];
			}

			Write("\t\t\t\t%d:  Face Updates = %d\n", i,
				pAuthorVertexUpdate->NumFaceUpdates);
			Write("\t\t\t\t\tNumTextureLayers = %d, OriginalMaterialID = %d\n",
				pAuthorMeshMaterials->m_uNumTextureLayers,
				pAuthorMeshMaterials->m_uOriginalMaterialID);
			Write("\t\t\t\t\tDiffuseColors = %d, SpecularColors = %d\n",
				pAuthorMeshMaterials->m_uDiffuseColors,
				pAuthorMeshMaterials->m_uSpecularColors);

			for( j = 0; j < (&pAuthorMeshMaterials[i])->m_uNumTextureLayers; j++)
			{
				Write("\t\t\t\t\tLayer %d has dimensions of %d\n",
					j, (&pAuthorMeshMaterials[i])->m_uTexCoordDimensions[j]);
			}
		}
	}

	if( pAuthorMesh && IFXSUCCESS(result))
	{
		result = pAuthorMesh->Unlock();
	}
}

/**
	Writes a description of an IFXAuthorCLODResource to the debug file.

	@param   IFAuthorXCLODResource* pData  The model resource to be written to file.
*/
void DebugInfo::Write( IFXAuthorLineSetResource* pData )
{
	IFXRESULT     result = IFX_OK;
	IFXVector4        boundingSphere;
	IFXTransform      transform;
	IFXAuthorLineSet* pAuthorLineSet = NULL;
	IFXMatrix4x4      transformMatrix;
	IFXAuthorLineSetDesc* pAuthorLineSetDesc = NULL;
	IFXAuthorMaterial*  pAuthorMaterials = NULL;
	U32           numAllocatedTextureLayers = 0;
	U32           i, j;

	CHK_DUMP_ENTRY(IFX_EDI_GENERATOR);
	if(NULL == pData)
		return ;

	Write("\t\tAuthor LineSet Resource\n");

	if( IFXSUCCESS(result))
		result = pData->GetAuthorLineSet( pAuthorLineSet );

	if( IFXSUCCESS(result))
	{
		boundingSphere = pData->GetBoundingSphere();
		transform = pData->GetTransform();
	}

	if( IFXSUCCESS(result))
	{
		Write("\t\t\tBounding Sphere at %f, %f, %f, radius %f\n",
			boundingSphere.X(),
			boundingSphere.Y(),
			boundingSphere.Z(),
			boundingSphere.A());

		transformMatrix = transform.MatrixConst();

		Write("\t\t\tCurrent Transform:\n");

		Write( &transformMatrix,"\t\t\t\t" );
	}

	if( pAuthorLineSet && IFXSUCCESS( result ) )
		result = pAuthorLineSet->GetNumAllocatedTexLineLayers( &numAllocatedTextureLayers );

	if( pAuthorLineSet && IFXSUCCESS(result))
	{
		pAuthorLineSetDesc = pAuthorLineSet->GetMaxLineSetDesc();

		Write("\t\t\tLineSet Statistics:\n");
		Write("\t\t\t\tNum Lines = %d, Num Positions = %d, Num Normals = %d\n",
			pAuthorLineSetDesc->m_numLines, pAuthorLineSetDesc->m_numPositions,
			pAuthorLineSetDesc->m_numNormals);
		Write("\t\t\t\tNum Diffuse Colors = %d, Num Specular Colors = %d\n",
			pAuthorLineSetDesc->m_numDiffuseColors, pAuthorLineSetDesc->m_numSpecularColors);
		Write("\t\t\t\tNum Texture Coordinates = %d, Num Materials = %d\n",
			pAuthorLineSetDesc->m_numTexCoords, pAuthorLineSetDesc->m_numMaterials );
		Write("\t\t\t\tNum Allocated Texture Layers = %d\n",
			numAllocatedTextureLayers );
	}

	if( pAuthorLineSet && IFXSUCCESS(result))
		result = pAuthorLineSet->GetMaterials(&pAuthorMaterials);

	if( pAuthorMaterials && IFXSUCCESS(result))
	{
		Write("\t\t\tLine Data:\n");

		for( i = 0; i < pAuthorLineSetDesc->m_numMaterials; ++i )
		{
			Write("\t\t\t\tNumTextureLayers = %d, OriginalMaterialID = %d\n",
				pAuthorMaterials->m_uNumTextureLayers,
				pAuthorMaterials->m_uOriginalMaterialID);
			Write("\t\t\t\tDiffuseColors = %d, SpecularColors = %d\n",
				pAuthorMaterials->m_uDiffuseColors,
				pAuthorMaterials->m_uSpecularColors);

			for( j = 0; j < (&pAuthorMaterials[i])->m_uNumTextureLayers; j++)
			{
				Write("\t\t\t\t\tLayer %d has dimensions of %d\n",
					j, (&pAuthorMaterials[i])->m_uTexCoordDimensions[j]);
			}
		}
	}
	IFXRELEASE(pAuthorLineSet);
}


/**
	Writes a description of an bone weight generator (IFXSkeleton) to
	the debug file.

	@param   IFXSkeleton *pData    The IFXSkeleton to be written to file.
*/
void DebugInfo::WriteBoneWeightGenerator(IFXSkeleton *pData)
{
	IFXRESULT       result = IFX_OK;
	IFXModifierChain    *pModChain = NULL;
	IFXModifierDataPacket *pModDataPacket = NULL;
	IFXModifier     *pCurrentModifier = NULL;
	IFXModifier     *pPreviousModifier = NULL;
	IFXMeshGroup      *pMeshGroup = NULL;
	IFXUnknown        *pUnknown = NULL;
	IFXBoneInfo       tBoneInfo;
	U32           meshIndex = 0;
	U32           uNumBones = 0;
	U32           uModifierIndex = 0;
	U32           i = 0;
	/*
	U32           numMeshes = 0;
	*/

	CHK_DUMP_ENTRY(IFX_EDI_GENERATOR);
	if(NULL == pData)
		return ;

	result = pData->GetNumBones(uNumBones);

	Write("\t\tBone Weight Generator\n");
	Write("\t\t\tNumber of Bones = %d\n", uNumBones);

	// Get the data packet from the previous modifier in the
	// chain - we need to know about the mesh we are assigning
	// weights to.
	// Cast the generator as a modifier
	if( IFXSUCCESS(result) )
		result = pData->QueryInterface(  IID_IFXModifier,
		(void**)&pCurrentModifier);

	// Get the previous modifier in the chain.
	if( pCurrentModifier && IFXSUCCESS(result) )
		result = pCurrentModifier->GetModifierChainIndex(uModifierIndex);

	if( pCurrentModifier && IFXSUCCESS(result) )
		result = pCurrentModifier->GetModifierChain(&pModChain);

	if( pModChain && IFXSUCCESS(result) )
		result = pModChain->GetModifier((uModifierIndex-1), pPreviousModifier);

	IFXRELEASE(pModChain);
	IFXRELEASE(pCurrentModifier);

	// Get the data packet coming out of the previous modifier
	if( pPreviousModifier && IFXSUCCESS(result) )
		result = pPreviousModifier->GetDataPacket(pModDataPacket);

	IFXRELEASE(pPreviousModifier);

	if( pModDataPacket && IFXSUCCESS(result))
		result = pModDataPacket->GetDataElementIndex(DID_IFXRenderableGroup,
		meshIndex);
	if( pModDataPacket && IFXSUCCESS(result))
		result = pModDataPacket->GetDataElement( meshIndex,
		(void**)&pUnknown );

	// QI the returned pointer into an IFXMeshGroup
	if( pUnknown && IFXSUCCESS(result) )
		result = pUnknown->QueryInterface(IID_IFXMeshGroup,
		(void**)&pMeshGroup);

	// After all that - now we know how many meshes there are...  Gasp!
	/*
	if( pMeshGroup && IFXSUCCESS(result) )
		numMeshes = pMeshGroup->GetNumMeshes();
	*/

	IFXRELEASE(pUnknown);
	IFXRELEASE(pMeshGroup);
	IFXRELEASE(pModDataPacket);

	// Dump information about the bone weights.

	if( IFXSUCCESS(result) )
	{
		i = 0;

		result  = pData->GetBoneInfo(i, &tBoneInfo);
		while (IFXSUCCESS(result))
		{
			Write("\t\t\tSkeleton Bone %d\n", i);
			Write(&tBoneInfo);
			result  = pData->GetBoneInfo(++i, &tBoneInfo);
		}
		result = IFX_OK;

		Write("\t\t\tBone Weights:\n");
		/*
		U32           uMeshWeightCount = 0;
		for(U32 m = 0; m < numMeshes; m++)
		{
			result = pData->GetTotalBoneWeightCount(m, &uMeshWeightCount);

			if( IFXSUCCESS(result) )
			Write("\t\t\t\tMesh %d:  TotalWeights = %d\n", m, uMeshWeightCount);
			else
			{
				Write("\t\t\t\tMesh %d:  GetTotalBoneWeightCount returned error %x\n",
				m, result);
				break;
			}
		}*/
	}
}


/**
	Writes a description of a IFXKeyFrame to the debug file.

	@param   IFXKeyFrame* pData    IFX keyframe to be written to file.
*/
void DebugInfo::Write(IFXKeyFrame* pData)
{
	CHK_DUMP_ENTRY(IFX_EDI_KEYFRAME);
	if(NULL == pData)
		return ;
}


/**
	Writes a description of a IFXMaterialResource to the debug file.

	@param   IFXMaterialResource* pData  IFX material resource to be written to file.
*/
void DebugInfo::Write(IFXMaterialResource* pData)
{
	F32     fValue = 0.0f;
	BOOL    bValue = FALSE;
	IFXVector4  LightComponent;
	IFXMarker *pMarker = NULL;
	IFXRESULT result = IFX_OK;

	CHK_DUMP_ENTRY(IFX_EDI_MATERIAL_PALETTE);
	if(NULL == pData)
		return ;

	// Dump the material's priority
	if( IFXSUCCESS(result) )
		result = pData->QueryInterface( IID_IFXMarker, (void**)&pMarker );

	U32 priority = 0;

	if( IFXSUCCESS(result) )
		priority = pMarker->GetPriority();

	if( IFXSUCCESS(result) )
		Write("\t\tMaterial priority:  %d\n", priority);

	IFXRELEASE(pMarker);

	// Dump the material priorities
	Write("\t\tMaterial Resource:\n");

	if( IFXSUCCESS(result) )
		result = pData->GetAmbient( &LightComponent );

	if( IFXSUCCESS(result) )
		Write("\t\t\tAmbient:  %f %f %f", LightComponent.X(),
		LightComponent.Y(), LightComponent.Z());


	if( IFXSUCCESS(result) )
		result = pData->GetDiffuse( &LightComponent );

	if( IFXSUCCESS(result) )
		Write("    Diffuse:  %f %f %f\n", LightComponent.X(),
		LightComponent.Y(), LightComponent.Z());

	//--

	if( IFXSUCCESS(result) )
		result = pData->GetSpecular( &LightComponent );

	if( IFXSUCCESS(result) )
		Write("\t\t\tSpecular:  %f %f %f", LightComponent.X(),
		LightComponent.Y(), LightComponent.Z());


	if( IFXSUCCESS(result) )
		result = pData->GetEmission( &LightComponent );

	if( IFXSUCCESS(result) )
		Write("    Emission:  %f %f %f\n", LightComponent.X(),
		LightComponent.Y(), LightComponent.Z());

	//--

	if( IFXSUCCESS(result) )
		result = pData->GetOpacity( &fValue );

	if( IFXSUCCESS(result) )
		Write("\t\t\tOpacity:  %f", fValue);


	if( IFXSUCCESS(result) )
		result = pData->GetTransparent( &bValue );

	if( IFXSUCCESS(result) )
		if( TRUE == bValue)
			Write("    Transparent");

	if( IFXSUCCESS(result) )
		result = pData->GetReflectivity( &fValue );

	if( IFXSUCCESS(result) )
		Write("    Reflectivity:  %f\n", fValue);

	Write("\n\t-----------------------\n");
}


/**
	Writes a description of a IFXLight to the debug file.

	@param   IFXLight* pData   IFX light resource to be written to file.
*/
void DebugInfo::Write(IFXLight* pData)
{
	IFXRESULT           result = IFX_OK;
	IFXLightResource::LightType       type=IFXLightResource::AMBIENT;

	CHK_DUMP_ENTRY(IFX_EDI_LIGHT);
	if(NULL == pData)
		return ;

	IFXDECLARELOCAL( IFXLightResource, pLightRes );
	pLightRes = pData->GetLightResource();

	//----
	// Light type
	//----
	if( NULL != pLightRes )
		type = pLightRes->GetType();
	else
	{
		result = IFX_E_INVALID_POINTER;
		IFXTRACE_GENERIC( L"NULL Light Resource Pointer" );
	}

	if( IFXSUCCESS(result) )
	{
		switch(type)
		{
		case IFXLightResource::AMBIENT:
			Write("\t\t-- Ambient\n");
			break;
		case IFXLightResource::DIRECTIONAL:
			Write("\t\t-- Directional light\n");
			break;
		case IFXLightResource::POINT:
			Write("\t\t-- Point light\n");
			break;
		case IFXLightResource::SPOT:
			Write("\t\t-- Spot light\n");
			break;
		default:
			Write("\t\t-- Unknown\n");
			break;
		}
	}

	//----
	// Get its color
	//----
	IFXVector4 vColor;

	if( IFXSUCCESS(result) )
		vColor = pLightRes->GetColor();

	if( IFXSUCCESS(result) )
		Write("\t\t\tR: %f, G: %f, B: %f, Alpha ignored\n",
		vColor.R(), vColor.G(), vColor.B());

	//----
	// Attenuation
	//----
	F32 fAttenuation[3];

	if( IFXSUCCESS(result) )
		result = pLightRes->GetAttenuation((F32 *)&fAttenuation);

	if( IFXSUCCESS(result) )
	{
		Write("\t\t\tAttenuation:  Const = %f, Linear = %f, Quadradic = %f\n",
			fAttenuation[0], fAttenuation[1], fAttenuation[2]);
	}

	//----
	//  Spot Angle
	//----
	if( IFXLightResource::SPOT == type)
	{
		F32 fSpotAngle=0.0f;

		if( IFXSUCCESS(result) )
			fSpotAngle = pLightRes->GetSpotAngle();

		if( IFXSUCCESS(result) )
		{
			Write("\t\t\tSpot Angle:  %f\n", fSpotAngle);
		}
	}

	//----
	// Attributes
	//----
	U8 uAttributes=0x00;

	if( IFXSUCCESS(result) )
		uAttributes = pLightRes->GetAttributes();

	if( IFXSUCCESS(result) )
	{
		Write("\t\t\tAttributes:");
		if( IFXLightResource::ENABLED & uAttributes)
			Write(" Enabled");

		if( IFXLightResource::SPECULAR & uAttributes)
			Write(" Specular");

		if( IFXLightResource::SPOTDECAY & uAttributes)
			Write(" SpotDecay");
	}

	Write("\n");
}


/**
	Writes a description of a IFXTextureObject to the debug file.

	@param   IFXTextureObject* pData   IFX texture object to be written to file.
*/
void DebugInfo::Write(IFXTextureObject* pData)
{
	IFXRESULT result = IFX_OK;

	U32     uWidth = 0;
	U32     uHeight = 0;
	U32     uFormat = 0;
	U32     uBufferSize = 0;
	U32     uCodecQuality = 0;
	U32     uTextureID = 0;
	U32     uTextureVersion = 0;
	IFXMarker *pMarker = NULL;
	IFXenum   eEnum;
	BOOL    bBool = FALSE;
	IFXCID    CodecCID;

	CHK_DUMP_ENTRY(IFX_EDI_TEXTUREOBJECT);
	if(NULL == pData)
		return ;

	// Dump the texture's priority
	if( IFXSUCCESS(result) )
		result = pData->QueryInterface( IID_IFXMarker, (void**)&pMarker );

	U32 priority = 0;

	if( IFXSUCCESS(result) )
		priority = pMarker->GetPriority();

	if( IFXSUCCESS(result) )
		Write("\t\tTexture priority:  %d\n", priority);

	IFXRELEASE(pMarker);

	// Now dump data on the texture.
	if(IFXSUCCESS(result))
		result = pData->GetWidth(&uWidth);

	if(IFXSUCCESS(result))
		result = pData->GetHeight( &uHeight);

	if(IFXSUCCESS(result))
		result = pData->GetFormat( &uFormat);

	if( IFXSUCCESS(result))
	{
		Write("\t\tWidth:  %d, Height: %d,  ", uWidth, uHeight);

		if( pData->HasAlphaColor())
			Write("Supports Alpha Color, ");
		else
			Write("No Alpha Color Support, ");

		if( IFXTextureObject::IFXTEXTUREMAP_FORMAT_LUMINANCE == uFormat)
			Write("Format:  8-bit Alpha\n");
		else if(IFXTextureObject::IFXTEXTUREMAP_FORMAT_RGB24 == uFormat)
			Write("Format:  24-bit RGB\n");
		else if( IFXTextureObject::IFXTEXTUREMAP_FORMAT_RGBA32 == uFormat)
			Write("Format:  32-bit RGBA\n");
		else
			Write("Format unknown:  %d\n", uFormat);
	}

	if( IFXSUCCESS(result))
		result = pData->GetCompressedState(&eEnum);

	if( IFXSUCCESS(result))
		result = pData->GetKeepDecompressed(&bBool);

	if( IFXSUCCESS(result))
	{
		if( IFXTEXTURECOMPRESSIONSTATE_FORCEDECOMPRESS == eEnum)
			Write("\t\tCompression State:  Force Decompress, ");
		else if( IFXTEXTURECOMPRESSIONSTATE_FORCECOMPRESSED == eEnum)
			Write("\t\tCompression State:  Force Compressed, ");
		else
			Write("\t\tCompression State Unknown:  %d, ", eEnum);

		if( TRUE == bBool)
			Write("Keep Compressed = TRUE\n");
		else
			Write("Keep Compressed = FALSE\n");
	}

	if( IFXSUCCESS(result))
		result = pData->GetRenderFormat(eEnum);

	if( IFXSUCCESS(result))
	{
		if( IFX_BGRA == eEnum)
			Write("\t\tRender Format:  IFX_BGRA\n");
		else if( IFX_RGBA == eEnum)
			Write("\t\tRender Format:  IFX_RGBA\n");
		else if( IFX_RGBA_8888 == eEnum)
			Write("\t\tRender Format:  IFX_RGBA_8888\n");
		else if( IFX_RGBA_8880 == eEnum)
			Write("\t\tRender Format:  IFX_RGBA_8880\n");
		else if( IFX_RGBA_5650 == eEnum)
			Write("\t\tRender Format:  IFX_RGBA_5650\n");
		else if( IFX_RGBA_5550 == eEnum)
			Write("\t\tRender Format:  IFX_RGBA_5550\n");
		else if( IFX_RGBA_5551 == eEnum)
			Write("\t\tRender Format:  IFX_RGBA_5551\n");
		else if( IFX_RGBA_4444 == eEnum)
			Write("\t\tRender Format:  IFX_RGBA_4444\n");
		else if( IFX_TEXCOMPRESS_1 == eEnum)
			Write("\t\tRender Format:  IFX_TEXCOMPRESS_1\n");
		else if( IFX_TEXCOMPRESS_2 == eEnum)
			Write("\t\tRender Format:  IFX_TEXCOMPRESS_2\n");
		else if( IFX_TEXCOMPRESS_3 == eEnum)
			Write("\t\tRender Format:  IFX_TEXCOMPRESS_3\n");
		else if( IFX_TEXCOMPRESS_4 == eEnum)
			Write("\t\tRender Format:  IFX_TEXCOMPRESS_4\n");
		else if( IFX_TEXCOMPRESS_5 == eEnum)
			Write("\t\tRender Format:  IFX_TEXCOMPRESS_5\n");
		else if( IFX_FMT_DEFAULT == eEnum)
			Write("\t\tRender Format:  IFX_FMT_DEFAULT\n");
		else
			Write("\t\tRender Format Unknown:  %d\n", eEnum);
	}

	if( IFXSUCCESS(result))
		result = pData->GetMagFilterMode(&eEnum);

	if( IFXSUCCESS(result))
	{
		Write("\t\tFilter Modes:  ");

		if( IFX_NEAREST == eEnum)
			Write("Magnification = NEAREST, ");
		else if( IFX_LINEAR == eEnum)
			Write("Magnification = LINEAR, ");
		else
			Write("Magnification Unknown %d, ", eEnum);
	}

	if( IFXSUCCESS(result))
		result = pData->GetMinFilterMode(&eEnum);

	if( IFXSUCCESS(result))
	{
		if( IFX_NEAREST == eEnum)
			Write("Minification = NEAREST, ");
		else if( IFX_LINEAR == eEnum)
			Write("Minification = LINEAR, ");
		else
			Write("Minification Unknown %d, ", eEnum);
	}

	if( IFXSUCCESS(result))
		result = pData->GetMipMode(&eEnum);

	if( IFXSUCCESS(result))
	{
		if( IFX_NEAREST == eEnum)
			Write("MipMap = NEAREST\n");
		else if( IFX_LINEAR == eEnum)
			Write("MipMap = LINEAR\n");
		else if( IFX_NONE == eEnum)
			Write("MipMap = NONE\n");
		else
			Write("MipMap Unknown %d\n", eEnum);
	}

	if( IFXSUCCESS(result))
		result = pData->GetTextureSource(&eEnum);

	if( IFXSUCCESS(result))
	{
		if( IFXTEXTURESOURCE_DECODED == eEnum)
			Write("\t\tTexture Source = Imported, ");
		else if( IFXTEXURESOURCE_OTHER == eEnum)
			Write("\t\tTexture Source = Cast Member, ");
		else if( IFXTEXTURECOMPRESSIONSTATE_FORCEDECOMPRESS == eEnum)
			Write("\t\tTexture Source = Image Object, ");
		else
			Write("\t\tTexture Source Unknown %d, ", eEnum);
	}

	if( IFXSUCCESS(result))
		result = pData->GetTextureType(&eEnum);

	if( IFXSUCCESS(result))
	{
		if( IFX_TEXTURE_2D == eEnum)
			Write("Texture Type = 2D\n");
		else if( IFX_TEXTURE_CUBE == eEnum)
			Write("Texture Type = Cubic\n");
		else
			Write("Texture Type Unknown %d\n", eEnum);
	}

	if( IFXSUCCESS(result))
		result = pData->GetRawImageBufferSize(&uBufferSize);

	if( IFXSUCCESS(result))
		result = pData->GetImageCodec(&CodecCID);

	if( IFXSUCCESS(result))
		result = pData->GetImageCodecQuality(uCodecQuality);

	if( IFXSUCCESS(result))
	{
		uTextureID  = pData->GetId();
		uTextureVersion = pData->GetVersion();
	}

	if( IFXSUCCESS(result))
	{
		Write("\t\tImage Buffer Size = %d, Image CODEC Quality = %d\n",
			uBufferSize, uCodecQuality);

		Write("\t\tTexture ID = %d, Texture Version = %d\n", uTextureID,
			uTextureVersion);

		Write("\t\tImage CODEC CID:  %x-%x-%x-%2x%2x%2x%2x%2x%2x%2x%2x\n", CodecCID.A,
			CodecCID.B, CodecCID.C, CodecCID.D[0],
			CodecCID.D[1], CodecCID.D[2], CodecCID.D[3], CodecCID.D[4],
			CodecCID.D[5], CodecCID.D[6], CodecCID.D[7]);
	}
}


/**
	Writes a description of a IFXSimulationTask to the debug file.

	@param   IFXSimulationTask* pData    IFX simulation task object to be written to file.
*/
void DebugInfo::Write(IFXSimulationTask* pData)
{
	CHK_DUMP_ENTRY(IFX_EDI_SIMTASK_PALETTE);
	if(NULL == pData)
		return ;

	Write("\t\tSimulation Task\n");
}


/**
	Writes a description of a IFXMotionResource to the debug file.

	@param   IFXMotionResource* pData    IFX motion resource to be written to file.
*/
void DebugInfo::Write(IFXMotionResource* pData)
{
	IFXRESULT result = IFX_OK;
	U32 TrackCount = 0;
	U32 KeyFrameCount = 0;
	IFXString tStr;

	CHK_DUMP_ENTRY(IFX_EDI_MOTIONRESOURCE);
	if(NULL == pData)
		return ;

	// Get number of tracks
	result = pData->GetTrackCount(&TrackCount);
	Write("Dumping IFXMotion Object with %d Tracks.\n", TrackCount);
	if(IFXSUCCESS(result))
	{
		U32 i;
		for(i = 0; i < TrackCount; ++i)
		{
			result = pData->GetTrackName(i, &tStr);
			Write("Track: "); Write(&tStr); Write("\n");

			// Get key frames from track
			result = pData->GetKeyFrameCount(i, &KeyFrameCount);
			if(IFXSUCCESS(result))
			{
				IFXKeyFrame* Frames = new IFXKeyFrame[KeyFrameCount];
				result = pData->GetKeyFrames(i, 0, KeyFrameCount, Frames);
				for(U32 j = 0; j < KeyFrameCount; j++)
				{
					Write("\tKeyFrame Time: %f", Frames[j].Time());
					Write("\n\t\tPOSITION: "); Write(&(Frames[j].Location()));
					Write("\n\t\tROTATION: "); Write(&(Frames[j].Rotation()));
					Write("\n\t\tSCALE: "); Write(&(Frames[j].Scale()));
					Write("\n");
				}
				IFXDELETE_ARRAY(Frames);
			}
		}
	}
}


/**
	Writes a description of a IFXMixerConstruct to the debug file.

	@param   IFXMixerConstruct* pData    IFX motion resource to be written to file.
*/
void DebugInfo::Write(IFXMixerConstruct* pData,
					  IFXPalette *pMixerPalette,
					  IFXPalette *pMotionPalette)
{
	IFXRESULT result = IFX_OK;
	const IFXList<IFXMapEntry>* pConstructList = NULL;
	/*
	IFXMixerConstruct::MotionType motionType = IFXMixerConstruct::NONE;
	*/
	IFXMotionResource*        pMotionResource = NULL;
	IFXString           cMotionName;
	F32               fDuration = 0.0f;
	U32               numElements = 0;
	U32               uMotionID = 0;

	CHK_DUMP_ENTRY(IFX_EDI_MOTIONRESOURCE);
	if(!pData || !pMixerPalette || !pMotionPalette)
		return ;

	pConstructList = pData->GetEntryList();
	fDuration = pData->GetDuration();
	/*
	motionType = pData->GetType();
	*/
	pMotionResource = pData->GetMotionResource();

	/*
	//
	// The callbacks do not appear to return any meaningful data for
	// motionType and fDuration.
	//
	if( IFXMixerConstruct::SINGLETRACK == motionType)
	Write("\t\tMotion Type:  Single Track");
	else if( IFXMixerConstruct::MULTITRACK == motionType)
	Write("\t\tMotion Type:  Multi-Track");
	else
	Write("\t\tMotion Type:  None");

	Write(", Duration: %f\n", fDuration);
	*/

	if( pMotionResource)
	{
		result = pMotionPalette->FindByResourcePtr(pMotionResource, &uMotionID);

		result = pMotionPalette->GetName(uMotionID, &cMotionName);

		Write("\t\tUses motion id: %d (", uMotionID );
		Write( &cMotionName );
		Write(")\n");
	}
	else
	{
		Write("\t\tMaps to no motion\n");
	}

	if( pConstructList )
	{
		IFXListContext context;
		IFXMapEntry *pConMap;

		numElements = pConstructList->GetNumberElements();

		if( numElements > 0)
			Write("\t\tMixer Construct has %d submaps:\n", fDuration);
		else
			Write("\t\tMixer Construct has no submaps.\n", fDuration);


		DISCARD pConstructList->ToHead(context);

		while(IFXSUCCESS(result) &&
			(pConMap=pConstructList->PostIncrement(context)) != NULL)
		{
			U32         uMixerID;
			IFXString     cMixerName;
			IFXMixerConstruct *pOtherMixer=pConMap->m_pMixerConstruct;
			IFXString     cBoneName=pConMap->m_boneName;

			if( pOtherMixer)
			{
				result = pMixerPalette->FindByResourcePtr(pOtherMixer, &uMixerID);

				result = pMixerPalette->GetName(uMixerID, &cMixerName);

				Write("\t\t\t- Mixer id %d (", uMixerID );
				Write(&cMixerName);
				Write(") for bone ");
				Write(&cBoneName);
				Write("\n");
			}

		}  // For all sub-constructs
	}  // If we have a construct list

	Write("\n");
}


/**
	Writes a description of an IFXMeshGroup to the debug file.

	@param   IFXMeshGroup* pData   IFX mesh group to be written to file.
*/
void DebugInfo::Write(IFXMeshGroup* pData)
{
	CHK_DUMP_ENTRY(IFX_EDI_MESHGROUP);
	if(NULL == pData)
		return ;
}


/**
	Writes a description of an IFXMesh to the debug file.

	@param   IFXMesh* pData    IFX mesh to be written to file.
*/
void DebugInfo::Write(IFXMesh* pData)
{
	CHK_DUMP_ENTRY(IFX_EDI_MESH);
	if(NULL == pData)
		return ;
}


/**
	Writes a string to the debug file.

	@param   IFXString* pData    String to be written to file.
*/
void DebugInfo::Write( IFXString* pData )
{
	CHK_DUMP_ENTRY(IFX_EDI_DUMP);

	if(NULL != pData)
	{
		U32 length = 0;
		
		if(IFXSUCCESS(pData->GetLengthU8(&length)))
		{
			U8* pString = new U8[length+1];

			if(NULL != pString)
			{
				if( IFXSUCCESS(pData->ConvertToRawU8( pString, length+1 ) ) )
					Write( (const char*)pString );

				delete [] pString;
			}
		}
	}
}


/**
	Writes a description of a IFXPalette to the debug file.

	@param   IFXPalette * pData  IFX palette to be written to file.
*/
void DebugInfo::Write(IFXPalette * pData)
{
	U32     tID, Item = 0;
	IFXString tName;
	IFXRESULT result = IFX_OK;

	CHK_DUMP_ENTRY(IFX_EDI_PALETTE);
	if(NULL == pData)
		return ;


	if(IFXSUCCESS(pData->First(&tID)))
	{
		do
		{
			Write("\t\t*** Entry %d:  Index: %d   Name: ", Item, tID);
			result = pData->GetName(tID, &tName);
			if(IFXSUCCESS(result))
			{
				Write(&tName);
				Write("\n");
			}

			Item++;

		}
		while(IFXSUCCESS(pData->Next(&tID)));
	}
	else
	{
		Write("\tEmpty Palette\n");
	}
}


/**
	Writes a description of a IFXPalette containing IFXNodes to the debug file.

	@param   IFXPalette * pData    IFX node palette to be written to file.
*/
void DebugInfo::WriteNodePalette(IFXPalette * pData)
{
	IFXRESULT result = IFX_OK;
	IFXNode*  pCurrNode = NULL;
	IFXUnknown* pUnknown = NULL;
	U32 tID, Item = 0;
	IFXString tName;

	CHK_DUMP_ENTRY(IFX_EDI_NODE_PALETTE);
	if(NULL == pData)
		return ;

	Write("\n*****************\n");
	Write("Node Palette\n");
	Write("*****************\n");

	if(IFXSUCCESS(pData->First(&tID)))
	{
		// Dump the raw palette
		do
		{
			IFXLight  *pLight = NULL;
			IFXView   *pView = NULL;
			IFXModel  *pModel = NULL;
			U32     uPaletteID = 0;

			// Write out node info
			Write("\t*** Entry %d:  Index: %d   Name: ", Item, tID);
			result = pData->GetName(tID, &tName);
			if( IFXSUCCESS(result) )
				Write(&tName);
			Write(",");

			// Get the node from the palette
			if( IFXSUCCESS(result) )
				result = pData->GetResourcePtr(tID, &pUnknown);

			if( IFX_E_PALETTE_NULL_RESOURCE_POINTER == result)
				result = IFX_OK;  // All palettes have a "null" entry in

			if( pUnknown && IFXSUCCESS(result) )
				result = pUnknown->QueryInterface( IID_IFXNode, (void**)&pCurrNode );

			//----
			// Figure out the type of the node and its resource pointer
			//----
			if( pUnknown && IFXSUCCESS(result) &&
				(IFX_OK == pUnknown->QueryInterface(
				IID_IFXLight,
				(void**)&pLight)))
			{
				// is it a light?
				if( pLight && IFXSUCCESS(result) )
				{
					Write("\tNodeType:  Light\n");

					Write(pLight);

					IFXRELEASE(pLight);
				}
				else
					result = IFX_OK;  // Reset status for next check
			}
			else if( pUnknown && IFXSUCCESS(result) &&
				(IFX_OK == pUnknown->QueryInterface(
				IID_IFXView,
				(void**)&pView)))
			{
				// if it a view (camera)
				if( pView && IFXSUCCESS(result) )
				{
					Write("\tNodeType:  View (camera)\n");

					Write(pView);

					IFXRELEASE(pView);
				}
				else
					result = IFX_OK;  // Reset status for next check
			}
			else if( pUnknown && IFXSUCCESS(result) &&
				(IFX_OK == pUnknown->QueryInterface(
				IID_IFXModel,
				(void**)&pModel)))
			{
				// is it a model?
				if( pModel && IFXSUCCESS(result) )
				{
					uPaletteID = pModel->GetResourceIndex();

					// Now dump information on the model.
					if( IFXSUCCESS(result) )
						Write("\tNodeType:  Model,   Generator PaletteID:  %d\n",
						uPaletteID);

					Write(pModel);

					IFXRELEASE(pModel);
				}
				else
					result = IFX_OK;  // Reset status for next check
			}
			else
			{
				Write("\tNodeType:  Unknown\n");
			}

			// Dump basic information about the node
			Write(pCurrNode);

			Write("\t-----------------------\n");

			IFXRELEASE(pCurrNode);
			IFXRELEASE(pUnknown);

			Item++;

		}
		while(IFXSUCCESS(pData->Next(&tID)));

		//----
		// Now display the hierarchy
		//----
		IFXNode* pNode = NULL;
		IFXNode* pChildNode = NULL;
		U32      numChildren = 0;

		Write("\n\t*****************\n");
		Write("\tHierarchy display\n");
		Write("\t*****************\n");

		// Tell us about the root
		if( IFXSUCCESS(result) )
			result = pData->First(&tID);

		if( IFXSUCCESS(result) )
			result = pData->GetResourcePtr( tID, &pUnknown );

		if( pUnknown && IFXSUCCESS(result) )
			result = pUnknown->QueryInterface( IID_IFXNode, (void**)&pNode );

		if( IFXSUCCESS(result) )
		{
			Write("\t%*.c", IFXDBGINDENT_AMT, ' ');

			result = pData->GetName(tID, &tName);
			if( IFXSUCCESS(result) )
			{
				if( tName == "" ) // world group node
					tName = L"World";

				Write(&tName); // dump node name

				Write("\n");
			}
		}

		IFXRELEASE(pUnknown);

		// Now dump the children
		if( pNode && IFXSUCCESS(result) )
			numChildren = pNode->GetNumberOfChildren();

		if( pNode && (numChildren > 0))
		{
			if( IFXSUCCESS(result) )
				pChildNode = pNode->GetChildNR(0);

			if( IFXSUCCESS(result) && NULL != pChildNode)
				result = DumpChildNodes(pData, pChildNode,
				(IFXDBGINDENT_AMT + IFXDBGINDENT_AMT), numChildren);
		}

		IFXRELEASE(pNode);

		Write("\n");
	}
	else
	{
		Write("\tEmpty Palette\n");
	}
}


/**
	Writes a description of a IFXPalette containing IFXLight nodes to the debug
	file.

	@param   IFXPalette * pData    IFX light palette to be written to file.
*/
void DebugInfo::WriteLightPalette(IFXPalette * pData)
{
	U32       tID, Item = 0;
	IFXString   tName;
	IFXRESULT   result = IFX_OK;
	IFXUnknown    *pUnknown = NULL;

	CHK_DUMP_ENTRY(IFX_EDI_LIGHT_PALETTE);
	if(NULL == pData)
		return ;

	Write("\n*****************\n");
	Write("Light Palette\n");
	Write("*****************\n");

	if(IFXSUCCESS(pData->First(&tID)))
	{
		// Dump the raw palette
		do
		{
			// Write out light info
			Write("\t*** Entry %d:  Index: %d   Name: ", Item, tID);
			result = pData->GetName(tID, &tName);
			if( IFXSUCCESS(result) )
				Write(&tName);
			Write(",");

			// Get the view from the palette
			//      if( IFXSUCCESS(result) )
			//        result = pData->GetResourcePtr(tID, &pUnknown);

			Write("\t-----------------------\n");

			IFXRELEASE(pUnknown);

			Item++;

		}
		while(IFXSUCCESS(pData->Next(&tID)));

		Write("\n");
	}
	else
	{
		Write("\tEmpty Palette\n");
	}
}


/**
	Writes a description of a IFXPalette containing IFXView nodes to the debug
	file.

	@param   IFXPalette * pData    IFX view palette to be written to file.
*/
void DebugInfo::WriteViewPalette(IFXPalette * pData)
{
	U32       tID, Item = 0;
	IFXString   tName;
	IFXRESULT   result = IFX_OK;
	IFXUnknown    *pUnknown = NULL;

	CHK_DUMP_ENTRY(IFX_EDI_VIEW_PALETTE);
	if(NULL == pData)
		return ;

	Write("\n*****************\n");
	Write("View Palette\n");
	Write("*****************\n");

	if(IFXSUCCESS(pData->First(&tID)))
	{
		// Dump the raw palette
		do
		{
			// Write out light info
			Write("\t*** Entry %d:  Index: %d   Name: ", Item, tID);
			result = pData->GetName(tID, &tName);
			if( IFXSUCCESS(result) )
				Write(&tName);
			Write(",");

			// Get the view from the palette
			//      if( IFXSUCCESS(result) )
			//        result = pData->GetResourcePtr(tID, &pUnknown);

			Write("\t-----------------------\n");

			IFXRELEASE(pUnknown);

			Item++;
		}
		while(IFXSUCCESS(pData->Next(&tID)));

		Write("\n");

	}
	else
	{
		Write("\tEmpty Palette\n");
	}
}


/**
	Write the children of a Node.

	@param   IFXPalette *pNodePalette  The Palette containing the nodes.
	@param   IFXNode *pNode        The node for which we are dumping children
	@param   U32 indent          The indentation level for hierarchy printout
	@param   U32 numSiblings       How many siblings this model has

	@return  IFXRESULT         Return status of this method.
*/
IFXRESULT DebugInfo::DumpChildNodes( IFXPalette *pNodePalette,
									IFXNode *pNode,
									U32 indent,
									U32 numSiblings)
{
	U32     numChildren = 0;
	U32     sibCount = 0;
	IFXRESULT result = IFX_OK;
	IFXString tName;
	IFXNode   *pChildNode = NULL;
	IFXNode   *pSiblingNode = NULL;
	IFXUnknown  *pUnknown = NULL;

	if( !pNode)
	{
		return IFX_E_INVALID_POINTER;
	}

	pSiblingNode = pNode;

	// Now loop through this node and all its siblings

	while (pSiblingNode && (sibCount < numSiblings))
	{
		numChildren = 0;
		U32 PID=0;

		// Tell us about the node.
		if( IFXSUCCESS(result) )
			result = pSiblingNode->QueryInterface( IID_IFXUnknown, (void**)&pUnknown );

		if( IFXSUCCESS(result) )
			result = pNodePalette->FindByResourcePtr(pUnknown, &PID);

		// Some nodes report that the world is their child.
		// FindByResourcePtr will fail in this case, so we pass this
		// one over for further consideration and try the next sibling.
		if( IFX_OK != result)
		{
			result = IFX_OK;

			sibCount++;

			// Now get the next sibling.
/**@todo: (MP)			if( sibCount < numSiblings)
				result = pSiblingNode->GetSiblingNextNR(&pSiblingNode);*/

			IFXRELEASE(pUnknown);

			continue;
		}

		if( IFXSUCCESS(result) )
		{
#ifdef MACINTOSH
			int i;
			Write("\t");
			for( i=0; i< indent; i++)
				Write(" ");
#else
			Write("\t%*.c", indent, ' ');
#endif
			result = pNodePalette->GetName(PID, &tName);
			if( IFXSUCCESS(result) )
			{
				Write(&tName);
				Write("\n");
			}
		}

		IFXRELEASE(pUnknown);

		// See if we have children
		if( IFXSUCCESS(result) )
			numChildren = pSiblingNode->GetNumberOfChildren();

		if( numChildren > 0)
		{
			// Recurse to the children if we have them...
/**@todo: (MP)			if( IFXSUCCESS(result) )
				result = pSiblingNode->GetFirstChildNR(&pChildNode);*/

			if( IFXSUCCESS(result) )
				result = DumpChildNodes(pNodePalette, pChildNode,
				(indent+IFXDBGINDENT_AMT),
				numChildren);
		}

		sibCount++;

		// Now get the next sibling.
/**@todo: (MP)		if( IFXSUCCESS(result) && (sibCount < numSiblings))
			result = pSiblingNode->GetSiblingNextNR(&pSiblingNode);*/
	}  // While sibling node

	return result;
}


/**
	Writes a description of a IFXPalette containing generators to the debug file.

	@param   IFXPalette * pData  IFX model palette to be written to file.
*/
void DebugInfo::WriteModelPalette( IFXPalette * pData )
{
	U32           tID, Item = 0;
	IFXModifier*  pModifier = NULL;
	IFXAuthorCLODResource* pAuthorCLODResource = NULL;
	IFXAuthorLineSetResource* pAuthorLineSetResource = NULL;
	IFXGenerator* pGenerator = NULL;
	IFXModifierChain* pModChain = NULL;
	IFXString       tName;
	U32           modCount = 0;
	IFXRESULT       result = IFX_OK;

	CHK_DUMP_ENTRY(IFX_EDI_MODEL_PALETTE);
	if(NULL == pData)
		return ;

	Write("\n*****************\n");
	Write("Generator (Model) Palette\n");
	Write("*****************\n");

	result = pData->First( &tID );
	if( IFXSUCCESS( result ) )
	{
		do
		{
			Write("\t*** Entry %d:  Index: %d   Name: ", Item, tID );
			result = pData->GetName(tID, &tName);
			if(IFXSUCCESS(result))
			{
				Write(&tName);
				Write("\n");
			}

			if( tName != L"" )
			{
				if(IFXSUCCESS(result) && m_Flags[IFX_EDI_DUMP_PALETTE_ENTRIES])
				{
					pGenerator = NULL;

					// Get the generator interface if possible
					DISCARD pData->GetResourcePtr(tID, IID_IFXGenerator,
						(void**) &pGenerator);

					if( IFX_OK == pData->GetResourcePtr(tID, IID_IFXAuthorCLODResource,
						(void**) &pAuthorCLODResource))
					{
						Write(pAuthorCLODResource);
						IFXRELEASE(pAuthorCLODResource);

						if( pGenerator)
							Write(pGenerator);
					}
					else if( IFX_OK == pData->GetResourcePtr( tID, IID_IFXAuthorLineSetResource,
						(void**) &pAuthorLineSetResource))
					{
						Write( pAuthorLineSetResource );
						IFXRELEASE( pAuthorLineSetResource );

						if( pGenerator)
							Write( pGenerator );
					}
					else
					{
						Write("\t\tUnknown model type\n");
					}

					IFXRELEASE(pGenerator);

					// Dump the modifier chain for this model
					result = pData->GetResourcePtr(tID, IID_IFXModifier,
						(void**) &pModifier);

					if( pModifier && IFXSUCCESS(result))
					{
						result = pModifier->GetModifierChain(&pModChain);

						if( pModChain && IFXSUCCESS(result))
						{
							result = pModChain->GetModifierCount(modCount);

							if( (modCount > 1) && IFXSUCCESS(result) )
							{
								Write("\t\tModifiers associated with this model (%d):\n", modCount-1);
								Write("\t\t\t");

								Write(pModChain);
							}
						}

						IFXRELEASE(pModChain);
						IFXRELEASE(pModifier);
					}
				}  // Dump detailed model palette
			}
			Write("\n\t-------------------------------------------------------\n");
			Item++;

			result = pData->Next(&tID);

		}
		while(IFXSUCCESS(result));
	}
	else
	{
		Write("\tEmpty Palette\n");
	}
}


/**
	Writes a description of a IFXPalette containing IFXMaterialResources to the debug file.

	@param   IFXPalette * pData  IFX material palette to be written to file.
*/
void DebugInfo::WriteMaterialPalette(IFXPalette * pData)
{
	U32           tID, Item = 0;
	IFXMaterialResource*  tPtr = NULL;
	IFXString       tName;
	IFXRESULT       result = IFX_OK;

	CHK_DUMP_ENTRY(IFX_EDI_MATERIAL_PALETTE);
	if(NULL == pData)
		return ;

	Write("\n*****************\n");
	Write("Material Palette\n");
	Write("*****************\n");

	result = pData->First(&tID);
	if(IFXSUCCESS(result))
	{
		do
		{
			Write("\t*** Entry %d:  Index: %d   Name: ", Item, tID);
			result = pData->GetName(tID, &tName);
			if(IFXSUCCESS(result))
			{
				Write(&tName);
				Write("\n");
			}

			if(m_Flags[IFX_EDI_DUMP_PALETTE_ENTRIES])
			{
				result = pData->GetResourcePtr(tID, IID_IFXMaterialResource, (void**)&tPtr);
				if(IFXSUCCESS(result))
					Write(tPtr);
				else
				{
					Write("\n\t-----------------------\n");
				}
				IFXRELEASE(tPtr);
			}
			Item++;

			result = pData->Next(&tID);
		}
		while(IFXSUCCESS(result));
	}
	else
	{
		Write("\tEmpty Palette\n");
	}
}


/**
	Writes a description of a IFXPalette containing IFXShaderLitTextures to the debug file.

	@param   IFXPalette * pData    IFX shader palette to be written to file.
*/
void DebugInfo::WriteShaderPalette(IFXPalette * pData)
{
	U32           tID, Item = 0;
	IFXShaderLitTexture*  tPtr = NULL;
	IFXString       tName;
	IFXRESULT       result = IFX_OK;

	CHK_DUMP_ENTRY(IFX_EDI_SHADER_PALETTE);
	if(NULL == pData)
		return ;

	Write("\n*****************\n");
	Write("Shader Palette\n");
	Write("*****************\n");

	result = pData->First(&tID);
	if(IFXSUCCESS(result))
	{
		do
		{
			Write("\t*** Entry %d:  Index: %d   Name: ", Item, tID);
			result = pData->GetName(tID, &tName);
			if(IFXSUCCESS(result))
			{
				Write(&tName);
				Write("\n");
			}

			if(m_Flags[IFX_EDI_DUMP_PALETTE_ENTRIES])
			{
				result = pData->GetResourcePtr(tID, IID_IFXShaderLitTexture, (void**)&tPtr);
				if(IFXSUCCESS(result))
					Write(tPtr);
				else
				{
					Write("\n\t-----------------------\n");
				}
				IFXRELEASE(tPtr);
			}
			Item++;

			result = pData->Next(&tID);

		}
		while(IFXSUCCESS(result));
	}
	else
	{
		Write("\tEmpty Palette\n");
	}
}


/**
	Writes a description of a IFXPalette containing IFXMotionResources to the debug file.

	@param   IFXPalette * pData    IFX motion palette to be written to file.
*/
void DebugInfo::WriteMotionPalette(IFXPalette * pData)
{
	U32         tID, Item = 0;
	IFXMotionResource*  tPtr = NULL;
	IFXString     tName;
	IFXRESULT     result = IFX_OK;

	CHK_DUMP_ENTRY(IFX_EDI_MOTION_PALETTE);
	if(NULL == pData)
		return ;

	Write("\n*****************\n");
	Write("Motion Palette\n");
	Write("*****************\n");

	result = pData->First(&tID);
	if(IFXSUCCESS(result))
	{
		do
		{
			Write("\n");
			Write("    ********************************************************\n");
			Write("    ********************************************************\n");
			Write("\t*** Entry %d:  Index: %d   Name: ", Item, tID);
			result = pData->GetName(tID, &tName);
			if(IFXSUCCESS(result))
			{
				Write(&tName);
				Write("\n");
			}

			if(m_Flags[IFX_EDI_DUMP_PALETTE_ENTRIES])
			{
				result = pData->GetResourcePtr(tID, IID_IFXMotionResource, (void**)&tPtr);
				if(IFXSUCCESS(result))
					Write(tPtr);
				IFXRELEASE(tPtr);
			}
			Item++;

			result = pData->Next(&tID);
		}
		while(IFXSUCCESS(result));
	}
	else
	{
		Write("\tEmpty Palette\n");
	}
}


/**
	Writes a description of a IFXPalette containing IFXMixerConstructs to the debug file.

	@param   IFXPalette * pData    IFX mixer palette to be written to file.
*/
void DebugInfo::WriteMixerPalette(IFXPalette * pData, IFXPalette *pMotionPalette)
{
	U32         tID, Item = 0;
	IFXMixerConstruct*  pMixer = NULL;
	IFXString     tName;
	IFXRESULT     result = IFX_OK;

	CHK_DUMP_ENTRY(IFX_EDI_MOTION_PALETTE);
	if(!pData || !pMotionPalette)
		return ;

	Write("\n*****************\n");
	Write("Mixer Palette\n");
	Write("*****************\n");

	result = pData->First(&tID);
	if(IFXSUCCESS(result))
	{
		do
		{
			Write("    ********************************************************\n");
			Write("    ********************************************************\n");
			Write("\t*** Entry %d:  Index: %d   Name: ", Item, tID);
			result = pData->GetName(tID, &tName);
			if(IFXSUCCESS(result))
			{
				Write(&tName);
				Write("\n");
			}

			if(m_Flags[IFX_EDI_DUMP_PALETTE_ENTRIES])
			{
				result = pData->GetResourcePtr(tID, IID_IFXMixerConstruct, (void**)&pMixer);
				if(IFXSUCCESS(result))
					Write(pMixer, pData, pMotionPalette);
				IFXRELEASE(pMixer);
			}
			Item++;

			result = pData->Next(&tID);
		}
		while(IFXSUCCESS(result));
	}
	else
	{
		Write("\tEmpty Palette\n");
	}
}


/**
	Writes a description of a IFXPalette containing IFXTextureObjects to the debug file.

	@param   IFXPalette * pData    IFX texture palette to be written to file.
*/
void DebugInfo::WriteTexturePalette(IFXPalette * pData)
{
	U32         tID, Item = 0;
	IFXTextureObject* tPtr = NULL;
	IFXString     tName;
	IFXRESULT     result = IFX_OK;

	CHK_DUMP_ENTRY(IFX_EDI_TEXTURE_PALETTE);
	if(NULL == pData)
		return ;

	Write("\n*****************\n");
	Write("Texture Palette\n");
	Write("*****************\n");

	result = pData->First(&tID);
	if(IFXSUCCESS(result))
	{
		int first = 1;
		do
		{
			Write("\t*** Entry %d:  Index: %d   Name: ", Item, tID);
			result = pData->GetName(tID, &tName);
			if(IFXSUCCESS(result))
			{
				Write(&tName);
				Write("\n");
			}

			if((0 == first) && m_Flags[IFX_EDI_DUMP_PALETTE_ENTRIES])
			{
				IFXRESULT result = IFX_OK;
				result = pData->GetResourcePtr(tID, IID_IFXTextureObject, (void**)&tPtr);
				if( IFX_OK == result)
					Write(tPtr);
				IFXRELEASE(tPtr);
			}
			Item++;
			first = 0;  // Don't bother dumping the default texture, which is a 32-bit 2x2 image.
			result = pData->Next(&tID);
		}
		while(IFXSUCCESS(result));
	}
	else
	{
		Write("\tEmpty Palette\n");
	}
}


/**
	Writes a description of a IFXPalette containing IFXSimulationTask to the debug file.

	@param   IFXPalette * pData    IFX simulation task palette to be written to file.
*/
void DebugInfo::WriteSimulationTaskPalette(IFXPalette * pData)
{
	U32         tID, Item = 0;
	IFXSimulationTask *pSimTask = NULL;
	IFXString     tName;
	IFXRESULT     result = IFX_OK;

	CHK_DUMP_ENTRY(IFX_EDI_SIMTASK_PALETTE);
	if(NULL == pData)
		return ;

	Write("\n***********************\n");
	Write("Simulation Task Palette\n");
	Write("***********************\n");

	result = pData->First(&tID);
	if(IFXSUCCESS(result))
	{
		do
		{
			Write("\t*** Entry %d:  Index: %d   Name: ", Item, tID);
			result = pData->GetName(tID, &tName);
			if(IFXSUCCESS(result))
			{
				Write(&tName);
				Write("\n");
			}

			if(m_Flags[IFX_EDI_DUMP_PALETTE_ENTRIES])
			{
				if(IFX_OK == pData->GetResourcePtr(tID, IID_IFXSimulationTask,
					(void**)&pSimTask))
				{
					Write(pSimTask);
					IFXRELEASE(pSimTask);
				}
				else
				{
					Write("\t\tUnknown\n");
				}
			}
			Item++;
			result = pData->Next(&tID);
		}
		while(IFXSUCCESS(result));
	}
	else
	{
		Write("\tEmpty Palette\n");
	}
}

//***************************************************************************
//  Protected methods
//***************************************************************************


//***************************************************************************
//  Private methods
//***************************************************************************


//***************************************************************************
//  Global functions
//***************************************************************************


//***************************************************************************
//  Local functions
//***************************************************************************
