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
  @file DebugInfo.h

      This header defines the ... functionality.

  @note
*/


#ifndef DebugInfo_H
#define DebugInfo_H


//***************************************************************************
//  Includes
//***************************************************************************


namespace U3D_IDTF
{
//***************************************************************************
//  Defines
//***************************************************************************

#define IFXDBGINDENT_AMT 3
#define DISCARD (void)
//#define PURE   = 0

//---------------------------------------------------------------------------
//  Debug output routines.
//---------------------------------------------------------------------------
#ifdef _DEBUG
#define DBGOUT(x) {IFXTRACE_GENERIC(L"*** MESSAGE -- from: %s at %d\n",__FILE__, __LINE__); IFXTRACE_GENERIC(L"*** %s\n", (x));};
#define DBGOUTFMT(x,y) {IFXTRACE_GENERIC(L"*** MESSAGE -- from: %s at %d\n",__FILE__, __LINE__); IFXTRACE_GENERIC(x,y);}  // must look like DBGOUTFMT("Format", Args);
#define DBGOUTIFERR(x,y) if(IFXFAILURE(x)){  DBGOUT(y); }
#else
#define DBGOUT(x)
#define DBGOUTFMT(x,y)
#define DBGOUTIFERR(x,y)
#endif

//***************************************************************************
//  Constants
//***************************************************************************


//***************************************************************************
//  Enumerations
//***************************************************************************


//***************************************************************************
//  Classes, structures and types
//***************************************************************************

/**
    Provides runtime debugging info to a text file.

  @b Description: Has methods to write most types of IFX data to the debug file in a textual form.
*/
class DebugInfo
{
public:
	/** @enum IFX_EDI_FLAGS
		Debug Flags

	@b Description:
	@remarks
	*/
	enum IFX_EDI_FLAGS
	{
	IFX_EDI_ENABLE,     /// Is the DebugInfo Enabled
	IFX_EDI_DUMP,     // Is DebugInfo currently writing info, if off no out put is
				// written, can be used to progamatically enable and disable logging.
	IFX_EDI_SKELETON,   // Dump Skeletons
	IFX_EDI_SKELETON_VERTEXWEIGHTS,  // Dump the vertex weights off of the Skeleton
	IFX_EDI_BONES,      // Write random comments from the Bone converter.
	IFX_EDI_MESHGROUP,    // Dump Mesh Groups.
	IFX_EDI_MESH,     // Dump Meshes
	IFX_EDI_LIGHT,      // Dump Lights.
	IFX_EDI_TEXTUREOBJECT,
	IFX_EDI_MOTIONRESOURCE,
	IFX_EDI_SHADER,
	IFX_EDI_KEYFRAME,
	IFX_EDI_GENERATOR,
	IFX_EDI_MODIFIER,
	IFX_EDI_PALETTE,
	IFX_EDI_NODE_PALETTE,
	IFX_EDI_MODEL_PALETTE,
	IFX_EDI_LIGHT_PALETTE,
	IFX_EDI_VIEW_PALETTE,
	IFX_EDI_MATERIAL_PALETTE,
	IFX_EDI_SHADER_PALETTE,
	IFX_EDI_TEXTURE_PALETTE,
	IFX_EDI_SIMTASK_PALETTE,
	IFX_EDI_MOTION_PALETTE,
	IFX_EDI_DUMP_PALETTE_ENTRIES,
	IFX_EDI_COUNT
	};

	DebugInfo();
	virtual ~DebugInfo();

	IFXRESULT SetFlag(U32 in_Id, U32 in_value);
	IFXRESULT Init(const char* pFileName);
	void Close();
	void WriteIf(U32 in_Flag, const char* in_Format, ...);

	void Write(const char*, ...);
	void Write(IFXShaderLitTexture* pShader);
	void Write(IFXShaderLitTexture* pShader, U32 channel);

	void Write(IFXAnimationModifier*);
	void Write(IFXSubdivModifier*);
	void Write(IFXCLODModifier*);

	void Write(IFXAuthorCLODResource*);
	void Write(IFXAuthorLineSetResource*);
	void Write(IFXGenerator*);
	void WriteBoneWeightGenerator(IFXSkeleton *);

	void Write(IFXMotionResource*);
	void Write(IFXMixerConstruct*, IFXPalette *pMixerPalette, IFXPalette *pMotionPalette);
	void Write(IFXMaterialResource*);

	void Write(IFXBoneInfo * pData);
	void Write(IFXQuaternion * pData);
	void Write(IFXKeyFrame*);

	void Write(IFXTextureObject*);

	void Write(IFXSimulationTask*);

	void Write(IFXModifierChain*);
	void Write(IFXModifierDataPacket*);
	void Write(IFXRenderable*);
	void Write(IFXMeshGroup*);
	void Write(IFXMesh*);
	void Write(IFXView*);
	void Write(IFXModel*);
	void Write(IFXLight*);
	void Write(IFXNode*);

	void Write(IFXVector3 * pData);
	void Write(IFXVector4 * pData);
	void Write(IFXMatrix4x4 * pData, const char* pTabchars);

	void Write(IFXString* pData);
	void Write(F32 * pData);
	void WriteColor(IFXVector4 * pData);
	void WriteColor(IFXVector3 * pData);

	void Write(IFXPalette * pData);
	void WriteNodePalette(IFXPalette * pData);
	void WriteLightPalette(IFXPalette * pData);
	void WriteViewPalette(IFXPalette * pData);
	void WriteModelPalette(IFXPalette * pData);
	void WriteMaterialPalette(IFXPalette * pData);
	void WriteShaderPalette(IFXPalette * pData);
	void WriteMotionPalette(IFXPalette * pData);
	void WriteMixerPalette(IFXPalette * pData, IFXPalette *pMotionPalette);
	void WriteTexturePalette(IFXPalette * pData);
	void WriteSimulationTaskPalette(IFXPalette * pData);

	IFXRESULT DumpChildNodes(IFXPalette *pNodePalette, IFXNode *pNode,
				U32 indent, U32 numSiblings);

private:
	/**
	The handle of the output file

	@b Description:
	@remarks
	*/
	FILE* m_DbgFile;

	/**
	The option flags of what to write

	@b Description:
	@remarks
	*/
	U32 m_Flags[IFX_EDI_COUNT];
};

//***************************************************************************
//  Inline functions
//***************************************************************************


//***************************************************************************
//  Global function prototypes
//***************************************************************************


//***************************************************************************
//  Global data
//***************************************************************************

#ifndef DEFINE_DBGINFO
extern DebugInfo g_DbgInfo;
#endif

}

#endif
