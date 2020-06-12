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
	@file	IFXPluginNameCIDs.h

			This header defines all of the CIDs that identify the various
			components contained in the Exporting DL.
*/


#ifndef IFXExportingCIDs_H
#define IFXExportingCIDs_H


//***************************************************************************
//	Includes
//***************************************************************************


#include "IFXGUID.h"


//***************************************************************************
//	Component identifiers
//***************************************************************************


//---------------------------------------------------------------------------
//	CID_IFXAuthorCLODEncoderX
//---------------------------------------------------------------------------
// {31470E65-95C7-45fc-9BE1-C8298E492A5B}
IFXDEFINE_GUID(CID_IFXAuthorCLODEncoderX,
0x31470e65, 0x95c7, 0x45fc, 0x9b, 0xe1, 0xc8, 0x29, 0x8e, 0x49, 0x2a, 0x5b);

//---------------------------------------------------------------------------
//	CID_IFXMaterialResourceEncoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXEncoderX. It is not a singleton.
//---------------------------------------------------------------------------
// {434B7805-61E9-4275-8676-FCF9F29F120F}
IFXDEFINE_GUID(CID_IFXMaterialResourceEncoder,
0x434b7805, 0x61e9, 0x4275, 0x86, 0x76, 0xfc, 0xf9, 0xf2, 0x9f, 0x12, 0xf);

//---------------------------------------------------------------------------
//	CID_IFXMotionResourceEncoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXEncoderX.  It is not a singleton.
//---------------------------------------------------------------------------
// {59FD56FA-F6F5-4c3c-B5DB-B3BEA1AE1D9E}
IFXDEFINE_GUID(CID_IFXMotionResourceEncoder,
0x59fd56fa, 0xf6f5, 0x4c3c, 0xb5, 0xdb, 0xb3, 0xbe, 0xa1, 0xae, 0x1d, 0x9e);

//---------------------------------------------------------------------------
//	CID_IFXCLODGeneratorEncoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXEncoderX.  It is not a singleton.
//---------------------------------------------------------------------------
// {C1A89136-FED7-493b-8F77-E2B9B5D8CF83}
IFXDEFINE_GUID(CID_IFXCLODGeneratorEncoder,
0xc1a89136, 0xfed7, 0x493b, 0x8f, 0x77, 0xe2, 0xb9, 0xb5, 0xd8, 0xcf, 0x83);

//---------------------------------------------------------------------------
//	CID_IFXGlyphGeneratorEncoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXEncoderX.  It is not a singleton.
//---------------------------------------------------------------------------
// {FE8FE162-8BFC-4fe0-B137-6790BFBF9D13}
IFXDEFINE_GUID(CID_IFXGlyphGeneratorEncoder,
0xfe8fe162, 0x8bfc, 0x4fe0, 0xb1, 0x37, 0x67, 0x90, 0xbf, 0xbf, 0x9d, 0x13);

//---------------------------------------------------------------------------
//	CID_IFXBoxGeneratorEncoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXEncoderX.  It is not a singleton.
//---------------------------------------------------------------------------
// {3AA37B55-A75E-4754-8C3F-6AEE25E22552}
IFXDEFINE_GUID(CID_IFXBoxGeneratorEncoder,
0x3aa37b55, 0xa75e, 0x4754, 0x8c, 0x3f, 0x6a, 0xee, 0x25, 0xe2, 0x25, 0x52);

//---------------------------------------------------------------------------
//	CID_IFXCylinderGeneratorEncoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXEncoderX.  It is not a singleton.
//---------------------------------------------------------------------------
// {54FA9551-7AD4-49db-B008-92CE0330209A}
IFXDEFINE_GUID(CID_IFXCylinderGeneratorEncoder,
0x54fa9551, 0x7ad4, 0x49db, 0xb0, 0x8, 0x92, 0xce, 0x3, 0x30, 0x20, 0x9a);

//---------------------------------------------------------------------------
//	CID_IFXPlaneGeneratorEncoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXEncoderX.  It is not a singleton.
//---------------------------------------------------------------------------
// {512AA1C6-1793-4f5e-A14A-E9D2B33C6112}
IFXDEFINE_GUID(CID_IFXPlaneGeneratorEncoder,
0x512aa1c6, 0x1793, 0x4f5e, 0xa1, 0x4a, 0xe9, 0xd2, 0xb3, 0x3c, 0x61, 0x12);

//---------------------------------------------------------------------------
//	CID_IFXSphereGeneratorEncoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXEncoderX.  It is not a singleton.
//---------------------------------------------------------------------------
// {7792A903-0CC2-4974-98D7-C17EA072F218}
IFXDEFINE_GUID(CID_IFXSphereGeneratorEncoder,
0x7792a903, 0xcc2, 0x4974, 0x98, 0xd7, 0xc1, 0x7e, 0xa0, 0x72, 0xf2, 0x18);

//---------------------------------------------------------------------------
//	CID_IFXParticleGeneratorEncoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXEncoderX.  It is not a singleton.
//---------------------------------------------------------------------------
// {119CAB2C-BB04-475b-ACBE-E6BFDB795307}
//IFXDEFINE_GUID(CID_IFXParticleGeneratorEncoder,
//0x119cab2c, 0xbb04, 0x475b, 0xac, 0xbe, 0xe6, 0xbf, 0xdb, 0x79, 0x53, 0x7);

//---------------------------------------------------------------------------
//	CID_IFXShaderLitTextureEncoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXEncoderX.  It is not a singleton.
//---------------------------------------------------------------------------
// {6F56E302-40A2-4112-BADB-C64ED5A91A8B}
IFXDEFINE_GUID(CID_IFXShaderLitTextureEncoder,
0x6f56e302, 0x40a2, 0x4112, 0xba, 0xdb, 0xc6, 0x4e, 0xd5, 0xa9, 0x1a, 0x8b);

//---------------------------------------------------------------------------
//	CID_IFXSkeletonModifierEncoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXEncoderX.  It is not a singleton.
//---------------------------------------------------------------------------
// {CCE63D52-781D-4008-AE51-D9F3747E9977}
IFXDEFINE_GUID(CID_IFXSkeletonModifierEncoder,
0xcce63d52, 0x781d, 0x4008, 0xae, 0x51, 0xd9, 0xf3, 0x74, 0x7e, 0x99, 0x77);

//---------------------------------------------------------------------------
//	CID_IFXSubdivisionModifierEncoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXEncoderX.  It is not a singleton.
//---------------------------------------------------------------------------
// {CDC4A54F-3DCE-456a-9672-3A8FE1D33B18}
IFXDEFINE_GUID(CID_IFXSubdivisionModifierEncoder,
0xcdc4a54f, 0x3dce, 0x456a, 0x96, 0x72, 0x3a, 0x8f, 0xe1, 0xd3, 0x3b, 0x18);

//---------------------------------------------------------------------------
//	CID_IFXShadingModifierEncoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXEncoderX.  It is not a singleton.
//---------------------------------------------------------------------------
// {61788FD5-4B95-4974-A5A5-1F4C2320F589}
IFXDEFINE_GUID(CID_IFXShadingModifierEncoder,
0x61788fd5, 0x4b95, 0x4974, 0xa5, 0xa5, 0x1f, 0x4c, 0x23, 0x20, 0xf5, 0x89);

//---------------------------------------------------------------------------
//	CID_IFXInkerModifierEncoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXEncoderX.  It is not a singleton.
//---------------------------------------------------------------------------
// {B98DEB40-A9F6-4277-B5B8-3747F1BC83DE}
IFXDEFINE_GUID(CID_IFXInkerModifierEncoder,
0xb98deb40, 0xa9f6, 0x4277, 0xb5, 0xb8, 0x37, 0x47, 0xf1, 0xbc, 0x83, 0xde);

//---------------------------------------------------------------------------
//	CID_IFXCLODGeneratorParamEncoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXEncoderX.  It is not a singleton.
//---------------------------------------------------------------------------
// {113173B4-1524-47bb-95CC-F6D0D13F1D56}
IFXDEFINE_GUID(CID_IFXCLODGeneratorParamEncoder,
0x113173b4, 0x1524, 0x47bb, 0x95, 0xcc, 0xf6, 0xd0, 0xd1, 0x3f, 0x1d, 0x56);

//---------------------------------------------------------------------------
//	CID_IFXAnimationModifierEncoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXEncoderX.  It is not a singleton.
//---------------------------------------------------------------------------
// {F6347D40-77E5-4bde-BFF5-7DF3A452D3E2}
IFXDEFINE_GUID(CID_IFXAnimationModifierEncoder,
0xf6347d40, 0x77e5, 0x4bde, 0xbf, 0xf5, 0x7d, 0xf3, 0xa4, 0x52, 0xd3, 0xe2);

//---------------------------------------------------------------------------
//	CID_IFXGroupNodeEncoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXEncoder.  It is not a singleton.
//---------------------------------------------------------------------------
// {782E31D5-6EE7-403e-8164-2A079BC51820}
IFXDEFINE_GUID(CID_IFXGroupNodeEncoder,
0x782e31d5, 0x6ee7, 0x403e, 0x81, 0x64, 0x2a, 0x7, 0x9b, 0xc5, 0x18, 0x20);

//---------------------------------------------------------------------------
//	CID_IFXLightNodeEncoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXEncoder.  It is not a singleton.
//---------------------------------------------------------------------------
// {79C1C3A8-7D80-42c7-AF8B-20D48E40C077}
IFXDEFINE_GUID(CID_IFXLightNodeEncoder,
0x79c1c3a8, 0x7d80, 0x42c7, 0xaf, 0x8b, 0x20, 0xd4, 0x8e, 0x40, 0xc0, 0x77);

//---------------------------------------------------------------------------
//	CID_IFXLightResourceEncoder
//---------------------------------------------------------------------------
// {5C910672-1E1E-4c4f-82DD-58BF6EFD56CF}
IFXDEFINE_GUID(CID_IFXLightResourceEncoder,
0x5c910672, 0x1e1e, 0x4c4f, 0x82, 0xdd, 0x58, 0xbf, 0x6e, 0xfd, 0x56, 0xcf);


//---------------------------------------------------------------------------
//	CID_IFXModelNodeEncoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXEncoder.  It is not a singleton.
//---------------------------------------------------------------------------
// {D745E333-000D-4834-A403-6F01418452BC}
IFXDEFINE_GUID(CID_IFXModelNodeEncoder,
0xd745e333, 0xd, 0x4834, 0xa4, 0x3, 0x6f, 0x1, 0x41, 0x84, 0x52, 0xbc);

//---------------------------------------------------------------------------
//	CID_IFXViewNodeEncoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXEncoder.  It is not a singleton.
//---------------------------------------------------------------------------
// {FD9478A2-3ED0-4b9b-A8B4-9453F6A874C5}
IFXDEFINE_GUID(CID_IFXViewNodeEncoder,
0xfd9478a2, 0x3ed0, 0x4b9b, 0xa8, 0xb4, 0x94, 0x53, 0xf6, 0xa8, 0x74, 0xc5);

//---------------------------------------------------------------------------
//	CID_IFXViewResourceEncoder
//---------------------------------------------------------------------------
// {810ED95A-F373-417b-837F-24961E008432}
IFXDEFINE_GUID(CID_IFXViewResourceEncoder,
0x810ed95a, 0xf373, 0x417b, 0x83, 0x7f, 0x24, 0x96, 0x1e, 0x0, 0x84, 0x32);



// {1A90ADF3-86A4-4b13-9C8B-C8E40B8FB886}
IFXDEFINE_GUID(CID_IFXFileReferenceEncoder,
0x1a90adf3, 0x86a4, 0x4b13, 0x9c, 0x8b, 0xc8, 0xe4, 0xb, 0x8f, 0xb8, 0x86);

//---------------------------------------------------------------------------
//	CID_IFXBlockPriorityQueueX
//
//	This component supports the following interfaces:  IFXUnknown,
//	IFXBlockPriorityQueueX and IFXBlockPriorityQueue. It is not a singleton.
//---------------------------------------------------------------------------
// {EB76E354-4A95-43eb-86C1-E0AE3500B387}
IFXDEFINE_GUID(CID_IFXBlockPriorityQueueX,
0xeb76e354, 0x4a95, 0x43eb, 0x86, 0xc1, 0xe0, 0xae, 0x35, 0x0, 0xb3, 0x87);

//---------------------------------------------------------------------------
//	CID_IFXBlockWriterX
//
//	This component supports the following interfaces:  IFXUnknown,
//	IFXBlockWriterX and IFXBlockWriter.  It is not a singleton.
//---------------------------------------------------------------------------
// {11D005BF-F01C-412d-BDC9-11A14F7D4696}
IFXDEFINE_GUID(CID_IFXBlockWriterX,
0x11d005bf, 0xf01c, 0x412d, 0xbd, 0xc9, 0x11, 0xa1, 0x4f, 0x7d, 0x46, 0x96);

//---------------------------------------------------------------------------
//	CID_IFXWriteManager
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXWriteManager.  It is not a singleton.
//---------------------------------------------------------------------------
// {C7C93F08-D804-11d3-A924-00A0C977EB2F}
IFXDEFINE_GUID(CID_IFXWriteManager,
0xc7c93f08, 0xd804, 0x11d3, 0xa9, 0x24, 0x0, 0xa0, 0xc9, 0x77, 0xeb, 0x2f);

/*
//---------------------------------------------------------------------------
//	CID_IFXAuthorMeshMap
//---------------------------------------------------------------------------
// {4C63D47D-6399-4af4-83B1-AD1FBDC035D5}
IFXDEFINE_GUID(CID_IFXAuthorMeshMap,
0x4c63d47d, 0x6399, 0x4af4, 0x83, 0xb1, 0xad, 0x1f, 0xbd, 0xc0, 0x35, 0xd5);

//---------------------------------------------------------------------------
//	CID_IFXBitStreamX
//
//	This component supports the following interfaces:  IFXUnknown,
//	IFXBitStream and IFXBitStreamX.  It is not a singleton.
//---------------------------------------------------------------------------
// {C8877656-222E-4d58-A7F5-57EFE15DD128}
IFXDEFINE_GUID(CID_IFXBitStreamX,
0xc8877656, 0x222e, 0x4d58, 0xa7, 0xf5, 0x57, 0xef, 0xe1, 0x5d, 0xd1, 0x28);

// {92973AA2-5F26-4d76-8C6A-4415726D8502}
IFXDEFINE_GUID(CID_IFXMeshMap,
0x92973aa2, 0x5f26, 0x4d76, 0x8c, 0x6a, 0x44, 0x15, 0x72, 0x6d, 0x85, 0x2);*/

//---------------------------------------------------------------------------
//	CID_IFXAuthorGeomCompiler
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXAuthorGeomCompiler.  It is not a singleton.
//---------------------------------------------------------------------------
// {A8C3F723-D4-42ED-A28B-3623534F2667}
IFXDEFINE_GUID(CID_IFXAuthorGeomCompiler,
0xA8C3F723, 0xD4, 0x42ED, 0xA2, 0x8B, 0x36, 0x23, 0x53, 0x4F, 0x26, 0x67);

//---------------------------------------------------------------------------
//	CID_IFXStdioWriteBuffer
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXStdio and IFXWriteBuffer.  It is not a singleton.
//---------------------------------------------------------------------------
// {4AC57842-EB76-11d3-9519-00A0C90AB136}
IFXDEFINE_GUID(CID_IFXStdioWriteBuffer,
0x4ac57842, 0xeb76, 0x11d3, 0x95, 0x19, 0x0, 0xa0, 0xc9, 0xa, 0xb1, 0x36);

//---------------------------------------------------------------------------
//	CID_IFXStdioWriteBufferX
//
//	This component supports the following interfaces:  IFXUnknown,
//	IFXStdio, IFXWriteBuffer and IFXWriteBufferX.  It is not a singleton.
//---------------------------------------------------------------------------
// {BE794A69-699E-4acf-B504-F7E1D39C0E62}
IFXDEFINE_GUID(CID_IFXStdioWriteBufferX,
0xbe794a69, 0x699e, 0x4acf, 0xb5, 0x4, 0xf7, 0xe1, 0xd3, 0x9c, 0xe, 0x62);

//---------------------------------------------------------------------------
//	CID_IFXPointSetEncoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXEncoderX.  It is not a singleton.
//---------------------------------------------------------------------------
// {1B365F66-E77E-4729-A98B-F3BC5AA63A1F}
IFXDEFINE_GUID(CID_IFXPointSetEncoder,
0x1b365f66, 0xe77e, 0x4729, 0xa9, 0x8b, 0xf3, 0xbc, 0x5a, 0xa6, 0x3a, 0x1f);

//---------------------------------------------------------------------------
//	CID_IFXLineSetEncoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXEncoderX.  It is not a singleton.
//---------------------------------------------------------------------------
// {94775068-4AC0-477d-B3FA-C178B0C532CD}
IFXDEFINE_GUID(CID_IFXLineSetEncoderX,
0x94775068, 0x4ac0, 0x477d, 0xb3, 0xfa, 0xc1, 0x78, 0xb0, 0xc5, 0x32, 0xcd);

//---------------------------------------------------------------------------
//	CID_IFXBoneWeightsModifierEncoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXEncoderX.  It is not a singleton.
//---------------------------------------------------------------------------
// {0CDE08FD-7C2F-4cd9-97DA-766E6D57A4B7}
IFXDEFINE_GUID(CID_IFXBoneWeightsModifierEncoder,
0xcde08fd, 0x7c2f, 0x4cd9, 0x97, 0xda, 0x76, 0x6e, 0x6d, 0x57, 0xa4, 0xb7);

//---------------------------------------------------------------------------
//	CID_IFXCLODModifierEncoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXEncoderX.  It is not a singleton.
//---------------------------------------------------------------------------
// {70145895-6A52-4950-A92B-11082F8BF688}
IFXDEFINE_GUID(CID_IFXCLODModifierEncoder,
0x70145895, 0x6a52, 0x4950, 0xa9, 0x2b, 0x11, 0x8, 0x2f, 0x8b, 0xf6, 0x88);

// {F3340FAA-C6AB-4530-A3D8-B30A630F5659}
IFXDEFINE_GUID(CID_IFXDummyModifierEncoder,
0xf3340faa, 0xc6ab, 0x4530, 0xa3, 0xd8, 0xb3, 0xa, 0x63, 0xf, 0x56, 0x59);


#endif
