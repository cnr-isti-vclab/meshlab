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
	@file	IFXImportingCIDs.h

			This header contains the declarations of CIDs.

	@note
*/

#ifndef IFXImportingCIDs_H
#define IFXImportingCIDs_H

#include "IFXExportingCIDs.h"

#include "IFXRenderingCIDs.h"
#include "IFXSchedulingCIDs.h"

//---------------------------------------------------------------------------
//	CID_IFXAuthorCLODDecoder
//---------------------------------------------------------------------------
// {4E706E9B-F9BD-43f7-A41D-995FF00678AA}
IFXDEFINE_GUID(CID_IFXAuthorCLODDecoder,
0x4e706e9b, 0xf9bd, 0x43f7, 0xa4, 0x1d, 0x99, 0x5f, 0xf0, 0x6, 0x78, 0xaa);


//---------------------------------------------------------------------------
//	CID_IFXGroupDecoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXDecoder.  It is not a singleton.
//---------------------------------------------------------------------------
// {AB161035-8C30-4192-8698-E9702B341893}
IFXDEFINE_GUID(CID_IFXGroupDecoder,
0xab161035, 0x8c30, 0x4192, 0x86, 0x98, 0xe9, 0x70, 0x2b, 0x34, 0x18, 0x93);


//---------------------------------------------------------------------------
//	CID_IFXLightDecoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXDecoder.  It is not a singleton.
//---------------------------------------------------------------------------
// {A8617218-E02E-484e-9528-C2231375EBF4}
IFXDEFINE_GUID(CID_IFXLightDecoder,
0xa8617218, 0xe02e, 0x484e, 0x95, 0x28, 0xc2, 0x23, 0x13, 0x75, 0xeb, 0xf4);

//---------------------------------------------------------------------------
//	CID_IFXLightResourceDecoder
//---------------------------------------------------------------------------
// {D29F8772-DABB-444a-9269-CF22D92A22F0}
IFXDEFINE_GUID(CID_IFXLightResourceDecoder,
0xd29f8772, 0xdabb, 0x444a, 0x92, 0x69, 0xcf, 0x22, 0xd9, 0x2a, 0x22, 0xf0);


//---------------------------------------------------------------------------
//	CID_IFXMaterialDecoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXDecoder. It is not a singleton.
//---------------------------------------------------------------------------
// {04A74BB1-EE49-4675-841C-0ECDD01F67B5}
IFXDEFINE_GUID(CID_IFXMaterialDecoder,
0x4a74bb1, 0xee49, 0x4675, 0x84, 0x1c, 0xe, 0xcd, 0xd0, 0x1f, 0x67, 0xb5);

//---------------------------------------------------------------------------
//	CID_IFXModelDecoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXDecoder.  It is not a singleton.
//---------------------------------------------------------------------------
// {87B11324-D108-4dfd-94D0-AD15D4E397F3}
IFXDEFINE_GUID(CID_IFXModelDecoder,
0x87b11324, 0xd108, 0x4dfd, 0x94, 0xd0, 0xad, 0x15, 0xd4, 0xe3, 0x97, 0xf3);

//---------------------------------------------------------------------------
//	CID_IFXCLODModifierParamDecoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXDecoder. It is not a singleton.
//---------------------------------------------------------------------------
// {50BF2246-77B6-44ab-B7D8-81E122FCAC48}
IFXDEFINE_GUID(CID_IFXCLODModifierParamDecoder,
0x50bf2246, 0x77b6, 0x44ab, 0xb7, 0xd8, 0x81, 0xe1, 0x22, 0xfc, 0xac, 0x48);

//---------------------------------------------------------------------------
//	CID_IFXSkeletonModifierDecoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXDecoder. It is not a singleton.
//---------------------------------------------------------------------------
// {6139229A-3112-443c-B743-A500D9451353}
IFXDEFINE_GUID(CID_IFXSkeletonModifierDecoder,
0x6139229a, 0x3112, 0x443c, 0xb7, 0x43, 0xa5, 0x0, 0xd9, 0x45, 0x13, 0x53);

//---------------------------------------------------------------------------
//	CID_IFXAnimationModifierDecoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXDecoder. It is not a singleton.
//---------------------------------------------------------------------------
// {D2A3315E-BB0E-40ea-B897-D7FFBCFC0FDF}
IFXDEFINE_GUID(CID_IFXAnimationModifierDecoder,
0xd2a3315e, 0xbb0e, 0x40ea, 0xb8, 0x97, 0xd7, 0xff, 0xbc, 0xfc, 0xf, 0xdf);

//---------------------------------------------------------------------------
//	CID_IFXMotionDecoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXDecoder.  It is not a singleton.
//---------------------------------------------------------------------------
// {392CE6F4-8B83-4511-8E01-245571A4796C}
IFXDEFINE_GUID(CID_IFXMotionDecoder,
0x392ce6f4, 0x8b83, 0x4511, 0x8e, 0x1, 0x24, 0x55, 0x71, 0xa4, 0x79, 0x6c);

//---------------------------------------------------------------------------
//	CID_IFXCLODGeneratorDecoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXDecoder.  It is not a singleton.
//---------------------------------------------------------------------------
// {94A1A80D-7EE1-4a9c-AFA0-D6CD3D87FE8A}
IFXDEFINE_GUID(CID_IFXCLODGeneratorDecoder,
0x94a1a80d, 0x7ee1, 0x4a9c, 0xaf, 0xa0, 0xd6, 0xcd, 0x3d, 0x87, 0xfe, 0x8a);

//---------------------------------------------------------------------------
//	CID_IFXGlyphGeneratorDecoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXDecoder.  It is not a singleton.
//---------------------------------------------------------------------------
// {F435BA07-2E46-49b2-A222-A009C1C97839}
IFXDEFINE_GUID(CID_IFXGlyphGeneratorDecoder,
0xf435ba07, 0x2e46, 0x49b2, 0xa2, 0x22, 0xa0, 0x9, 0xc1, 0xc9, 0x78, 0x39);

//---------------------------------------------------------------------------
//	CID_IFXBoxGeneratorDecoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXDecoder.  It is not a singleton.
//---------------------------------------------------------------------------
// {43B0FAE5-260A-47c7-9627-0F10CACE8D8A}
IFXDEFINE_GUID(CID_IFXBoxGeneratorDecoder,
0x43b0fae5, 0x260a, 0x47c7, 0x96, 0x27, 0xf, 0x10, 0xca, 0xce, 0x8d, 0x8a);

//---------------------------------------------------------------------------
//	CID_IFXCylinderGeneratorDecoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXDecoder.  It is not a singleton.
//---------------------------------------------------------------------------
// {806B076F-9C88-4340-B99B-AFA746FBFD4C}
IFXDEFINE_GUID(CID_IFXCylinderGeneratorDecoder,
0x806b076f, 0x9c88, 0x4340, 0xb9, 0x9b, 0xaf, 0xa7, 0x46, 0xfb, 0xfd, 0x4c);

//---------------------------------------------------------------------------
//	CID_IFXPlaneGeneratorDecoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXDecoder.  It is not a singleton.
//---------------------------------------------------------------------------
// {705F5998-50A7-4698-BAC2-0B5FED842E0E}
IFXDEFINE_GUID(CID_IFXPlaneGeneratorDecoder,
0x705f5998, 0x50a7, 0x4698, 0xba, 0xc2, 0xb, 0x5f, 0xed, 0x84, 0x2e, 0xe);

//---------------------------------------------------------------------------
//	CID_IFXSphereGeneratorDecoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXDecoder.  It is not a singleton.
//---------------------------------------------------------------------------
// {2B16B30D-9F4E-48f0-B88D-50FAC8BA8740}
IFXDEFINE_GUID(CID_IFXSphereGeneratorDecoder,
0x2b16b30d, 0x9f4e, 0x48f0, 0xb8, 0x8d, 0x50, 0xfa, 0xc8, 0xba, 0x87, 0x40);

//---------------------------------------------------------------------------
//	CID_IFXParticleGeneratorDecoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXDecoder.  It is not a singleton.
//---------------------------------------------------------------------------
// {18256C12-B2CF-4f6f-8501-3F98EE878328}
//IFXDEFINE_GUID(CID_IFXParticleGeneratorDecoder,
//0x18256c12, 0xb2cf, 0x4f6f, 0x85, 0x1, 0x3f, 0x98, 0xee, 0x87, 0x83, 0x28);

//---------------------------------------------------------------------------
//	CID_IFXShaderLitTextureDecoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXDecoder.  It is not a singleton.
//---------------------------------------------------------------------------
// {889CCEC7-FDD9-49dc-9402-CF053417FBCB}
IFXDEFINE_GUID(CID_IFXShaderLitTextureDecoder,
0x889ccec7, 0xfdd9, 0x49dc, 0x94, 0x2, 0xcf, 0x5, 0x34, 0x17, 0xfb, 0xcb);

//---------------------------------------------------------------------------
//	CID_IFXTextureDecoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXDecoder.  It is not a singleton.
//---------------------------------------------------------------------------
// {429EDF6B-7679-4c70-9882-A509E7A2EBE7}
IFXDEFINE_GUID(CID_IFXTextureDecoder,
0x429edf6b, 0x7679, 0x4c70, 0x98, 0x82, 0xa5, 0x9, 0xe7, 0xa2, 0xeb, 0xe7);

//---------------------------------------------------------------------------
//	CID_IFXSubdivisionModifierDecoder
//
//	This component supports the following interfaces:  IFXUnknown
//	and IFXDecoder.  It is not a singleton.
//---------------------------------------------------------------------------
// {8E910781-E1A6-4f25-A3DD-A1367670628D}
IFXDEFINE_GUID(CID_IFXSubdivisionModifierDecoder,
0x8e910781, 0xe1a6, 0x4f25, 0xa3, 0xdd, 0xa1, 0x36, 0x76, 0x70, 0x62, 0x8d);

//---------------------------------------------------------------------------
//	CID_IFXShadingModifierDecoder
//
//	This component supports the following interfaces:  IFXUnknown
//	and IFXDecoder.  It is not a singleton.
//---------------------------------------------------------------------------
// {BF7C4143-DE64-4179-ACB9-84CEBDB39A61}
IFXDEFINE_GUID(CID_IFXShadingModifierDecoder,
0xbf7c4143, 0xde64, 0x4179, 0xac, 0xb9, 0x84, 0xce, 0xbd, 0xb3, 0x9a, 0x61);


//---------------------------------------------------------------------------
//	CID_IFXExternalPluginDecoder
//
//	This component supports the following interfaces:  IFXUnknown
//	and IFXDecoderX.  It is not a singleton.
//---------------------------------------------------------------------------
// {BF74BA93-A3CB-497c-BBFD-8A45ED0CB750}
IFXDEFINE_GUID(CID_IFXExternalPluginDecoder,
0xbf74ba93, 0xa3cb, 0x497c, 0xbb, 0xfd, 0x8a, 0x45, 0xed, 0xc, 0xb7, 0x50);
/*
//---------------------------------------------------------------------------
//	CID_IFXInkerModifierDecoder
//
//	This component supports the following interfaces:  IFXUnknown
//	and IFXDecoder.  It is not a singleton.
//---------------------------------------------------------------------------
// {3364602B-7482-40fc-8775-E9C89CF1A05D}
IFXDEFINE_GUID(CID_IFXInkerModifierDecoder,
0x3364602b, 0x7482, 0x40fc, 0x87, 0x75, 0xe9, 0xc8, 0x9c, 0xf1, 0xa0, 0x5d);
*/
//---------------------------------------------------------------------------
//	CID_IFXDecoderChainX
//
//	This component supports the following interfaces:  IFXUnknown and
//	CID_IFXDecoderChainX.  It is not a singleton.
//---------------------------------------------------------------------------
// {24C4DE60-C9D1-4ec4-A358-636FEF948452}
IFXDEFINE_GUID(CID_IFXDecoderChainX,
0x24c4de60, 0xc9d1, 0x4ec4, 0xa3, 0x58, 0x63, 0x6f, 0xef, 0x94, 0x84, 0x52);

//---------------------------------------------------------------------------
//	CID_IFXViewDecoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXDecoder.  It is not a singleton.
//---------------------------------------------------------------------------
// {CFE3891C-50A9-49b6-9EF4-FBAD36C11F7C}
IFXDEFINE_GUID(CID_IFXViewDecoder,
0xcfe3891c, 0x50a9, 0x49b6, 0x9e, 0xf4, 0xfb, 0xad, 0x36, 0xc1, 0x1f, 0x7c);

//---------------------------------------------------------------------------
//	CID_IFXViewResourceDecoder
//---------------------------------------------------------------------------
// {3352E49B-96EC-4bbb-BE54-85CDE039B600}
IFXDEFINE_GUID(CID_IFXViewResourceDecoder,
0x3352e49b, 0x96ec, 0x4bbb, 0xbe, 0x54, 0x85, 0xcd, 0xe0, 0x39, 0xb6, 0x0);



//---------------------------------------------------------------------------
//	CID_IFXBlockReaderX
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXBlockReader.  It is not a singleton.
//---------------------------------------------------------------------------
// {D0A7E190-EDB3-4878-A944-F4FB7BEDC4C7}
IFXDEFINE_GUID(CID_IFXBlockReaderX,
0xd0a7e190, 0xedb3, 0x4878, 0xa9, 0x44, 0xf4, 0xfb, 0x7b, 0xed, 0xc4, 0xc7);

//---------------------------------------------------------------------------
//	CID_IFXBTTHash
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXBTTHash.  It is not a singleton.
//---------------------------------------------------------------------------
// {3A822F78-62AC-4597-8A8B-240DDC35608D}
IFXDEFINE_GUID(CID_IFXBTTHash,
0x3a822f78, 0x62ac, 0x4597, 0x8a, 0x8b, 0x24, 0xd, 0xdc, 0x35, 0x60, 0x8d);

//---------------------------------------------------------------------------
//	CID_IFXLoadManager
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXLoadManager and IFXTask.  It is not a singleton.
//---------------------------------------------------------------------------
// {916782C2-D811-11d3-9519-00A0C90AB136}
IFXDEFINE_GUID(CID_IFXLoadManager,
0x916782c2, 0xd811, 0x11d3, 0x95, 0x19, 0x0, 0xa0, 0xc9, 0xa, 0xb1, 0x36);

//---------------------------------------------------------------------------
//	CID_IFXStdioReadBuffer
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXStdio and IFXReadBuffer.  It is not a singleton.
//---------------------------------------------------------------------------
// {4AC57841-EB76-11d3-9519-00A0C90AB136}
IFXDEFINE_GUID(CID_IFXStdioReadBuffer,
0x4ac57841, 0xeb76, 0x11d3, 0x95, 0x19, 0x0, 0xa0, 0xc9, 0xa, 0xb1, 0x36);

//---------------------------------------------------------------------------
//	CID_IFXStdioReadBufferX
//
//	This component supports the following interfaces:  IFXUnknown,
//	IFXStdio, IFXReadBuffer and IFXReadBufferX.  It is not a singleton.
//---------------------------------------------------------------------------
// {3C87F0C4-B422-436d-8AC3-E37F0FDF28B9}
IFXDEFINE_GUID(CID_IFXStdioReadBufferX,
0x3c87f0c4, 0xb422, 0x436d, 0x8a, 0xc3, 0xe3, 0x7f, 0xf, 0xdf, 0x28, 0xb9);

//---------------------------------------------------------------------------
//	CID_IFXInternetReadBuffer
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXStdio and IFXReadBuffer.  It is not a singleton.
//---------------------------------------------------------------------------
// {BC9B6F41-CC5E-4262-B23E-20D27DD9B81A}
IFXDEFINE_GUID(CID_IFXInternetReadBuffer,
0xbc9b6f41, 0xcc5e, 0x4262, 0xb2, 0x3e, 0x20, 0xd2, 0x7d, 0xd9, 0xb8, 0x1a);

//---------------------------------------------------------------------------
//	CID_IFXInternetReadBufferX
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXStdio and IFXReadBuffer.  It is not a singleton.
//---------------------------------------------------------------------------
// {B756E8B6-4D62-4c35-A94A-01DCC68E3DB5}
IFXDEFINE_GUID(CID_IFXInternetReadBufferX,
0xb756e8b6, 0x4d62, 0x4c35, 0xa9, 0x4a, 0x1, 0xdc, 0xc6, 0x8e, 0x3d, 0xb5);

//---------------------------------------------------------------------------
//	CID_IFXPointSetDecoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXDecoder.  It is not a singleton.
//---------------------------------------------------------------------------
// {E993D83A-B99E-4843-8303-AD33BE15E38F}
IFXDEFINE_GUID(CID_IFXPointSetDecoder,
0xe993d83a, 0xb99e, 0x4843, 0x83, 0x3, 0xad, 0x33, 0xbe, 0x15, 0xe3, 0x8f);

//---------------------------------------------------------------------------
//	CID_IFXLineSetDecoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXDecoder.  It is not a singleton.
//---------------------------------------------------------------------------
// {08C5BE23-711E-4fca-9B7E-94C8264AB37F}
IFXDEFINE_GUID(CID_IFXLineSetDecoder,
0x8c5be23, 0x711e, 0x4fca, 0x9b, 0x7e, 0x94, 0xc8, 0x26, 0x4a, 0xb3, 0x7f);

//---------------------------------------------------------------------------
//	CID_IFXBoneWeightsModifierDecoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXDecoder.  It is not a singleton.
//---------------------------------------------------------------------------
// {CEEBFD29-C1E0-4f8f-A94E-F43B986635D3}
IFXDEFINE_GUID(CID_IFXBoneWeightsModifierDecoder,
0xceebfd29, 0xc1e0, 0x4f8f, 0xa9, 0x4e, 0xf4, 0x3b, 0x98, 0x66, 0x35, 0xd3);

//---------------------------------------------------------------------------
//	CID_IFXCLODModifierDecoder
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXDecoder.  It is not a singleton.
//---------------------------------------------------------------------------
// {1894EF02-F15A-4430-876C-5389E9782279}
IFXDEFINE_GUID(CID_IFXCLODModifierDecoder,
0x1894ef02, 0xf15a, 0x4430, 0x87, 0x6c, 0x53, 0x89, 0xe9, 0x78, 0x22, 0x79);

// {B204360C-10E2-4d66-AC0C-C6EF86C1E683}
IFXDEFINE_GUID(CID_IFXDummyModifierDecoder,
0xb204360c, 0x10e2, 0x4d66, 0xac, 0xc, 0xc6, 0xef, 0x86, 0xc1, 0xe6, 0x83);


#endif
