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
	@file  IFXCoreCIDs.h

	This file defines all of the IFXCOM component identifiers that
	are exposed by the core IFX engine DLL.

	@note
    Interface identifiers are defined in the header that actually defines
	the interface.
*/


#ifndef IFXCORECIDS_H
#define IFXCORECIDS_H


//***************************************************************************
//  Includes
//***************************************************************************


#include "IFXUnknown.h"


// {7F28191F-EA05-4b0d-A511-62164D18205F}
IFXDEFINE_GUID(CID_IFXNameMap,
0x7f28191f, 0xea05, 0x4b0d, 0xa5, 0x11, 0x62, 0x16, 0x4d, 0x18, 0x20, 0x5f);

// {1FD3AD91-A90E-11d3-94A5-00A0C9A0FBAE}
IFXDEFINE_GUID(CID_IFXDevice,
0x1fd3ad91, 0xa90e, 0x11d3, 0x94, 0xa5, 0x0, 0xa0, 0xc9, 0xa0, 0xfb, 0xae);

// {0ABA4D0D-8CB6-4b26-A3C7-363BFDF202F3}
IFXDEFINE_GUID(CID_IFXCLODManager,
0xaba4d0d, 0x8cb6, 0x4b26, 0xa3, 0xc7, 0x36, 0x3b, 0xfd, 0xf2, 0x2, 0xf3);

// {B0E8A72D-3D0C-4a7b-8B1D-17373E65D9B1}
IFXDEFINE_GUID(CID_IFXSetAdjacencyX,
0xb0e8a72d, 0x3d0c, 0x4a7b, 0x8b, 0x1d, 0x17, 0x37, 0x3e, 0x65, 0xd9, 0xb1);

// {2F28A2C4-CD3C-44bb-9C32-E19F6229A991}
IFXDEFINE_GUID(CID_IFXSetX,
0x2f28a2c4, 0xcd3c, 0x44bb, 0x9c, 0x32, 0xe1, 0x9f, 0x62, 0x29, 0xa9, 0x91);

// {538C3521-9C85-49C1-9931-CBCC14F02853}
IFXDEFINE_GUID(CID_IFXAuthorCLODGen,
0x538C3521, 0x9C85, 0x49C1, 0x99, 0x31, 0xCB, 0xCC, 0x14, 0xF0, 0x28, 0x53);

// {3F66C635-D6EC-43e8-883B-BFDACBF2E3C9}
IFXDEFINE_GUID(CID_IFXAuthorLineSetAnalyzer,
0x3f66c635, 0xd6ec, 0x43e8, 0x88, 0x3b, 0xbf, 0xda, 0xcb, 0xf2, 0xe3, 0xc9);

// {F3193659-4C2D-4087-B82D-375A4C2A5BA7}
IFXDEFINE_GUID(CID_IFXMetaDataX,
0xf3193659, 0x4c2d, 0x4087, 0xb8, 0x2d, 0x37, 0x5a, 0x4c, 0x2a, 0x5b, 0xa7);

// {25105C80-C86A-4675-BA21-31D81C015D9B}
IFXDEFINE_GUID(CID_IFXFileReference,
0x25105c80, 0xc86a, 0x4675, 0xba, 0x21, 0x31, 0xd8, 0x1c, 0x1, 0x5d, 0x9b);

//---------------------------------------------------------------------------
//	CID_IFXMeshCompiler
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXMeshCompiler.  It is not a singleton.
//---------------------------------------------------------------------------
// {2601D295-C427-41CA-84BD-1930016FACF7}
IFXDEFINE_GUID(CID_IFXMeshCompiler,
0x2601D295, 0xC427, 0x41CA, 0x84, 0xBD, 0x19, 0x30, 0x01, 0x6F, 0xAC, 0xF7);

// {92973AA2-5F26-4d76-8C6A-4415726D8502}
IFXDEFINE_GUID(CID_IFXMeshMap,
0x92973aa2, 0x5f26, 0x4d76, 0x8c, 0x6a, 0x44, 0x15, 0x72, 0x6d, 0x85, 0x2);

//---------------------------------------------------------------------------
//	CID_IFXAuthorMeshMap
//---------------------------------------------------------------------------
// {4C63D47D-6399-4af4-83B1-AD1FBDC035D5}
IFXDEFINE_GUID(CID_IFXAuthorMeshMap,
0x4c63d47d, 0x6399, 0x4af4, 0x83, 0xb1, 0xad, 0x1f, 0xbd, 0xc0, 0x35, 0xd5);

//---------------------------------------------------------------------------
//  CID_IFXGlyph2DCommands
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXGlyph2DCommands.  It is not a singleton.
//---------------------------------------------------------------------------
// {CBC4ECA1-6324-11d4-A145-00A0C9A0F93B}
IFXDEFINE_GUID(CID_IFXGlyph2DCommands,
0xcbc4eca1, 0x6324, 0x11d4, 0xa1, 0x45, 0x0, 0xa0, 0xc9, 0xa0, 0xf9, 0x3b);

//---------------------------------------------------------------------------
//  CID_IFXGlyphCommandList
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXGlyphCommandList.  It is not a singleton.
//---------------------------------------------------------------------------
// {84E06D18-053E-4215-B40B-5A99D57EFBDD}
IFXDEFINE_GUID(CID_IFXGlyphCommandList,
0x84e06d18, 0x53e, 0x4215, 0xb4, 0xb, 0x5a, 0x99, 0xd5, 0x7e, 0xfb, 0xdd);

//---------------------------------------------------------------------------
//  CID_IFXGlyph3DGenerator
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXGlyph3DGenerator.  It is not a singleton.
//---------------------------------------------------------------------------
// {139C63DB-109C-11d4-A120-00A0C9A0F93B}
IFXDEFINE_GUID(CID_IFXGlyph3DGenerator,
0x139c63db, 0x109c, 0x11d4, 0xa1, 0x20, 0x0, 0xa0, 0xc9, 0xa0, 0xf9, 0x3b);

//---------------------------------------------------------------------------
//  CID_IFXAuthorMeshScrub
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXAuthorMeshScrub.  It is not a singleton.
//---------------------------------------------------------------------------
// {375C3259-E0BA-436A-BE98-F4F6B5C09314}
IFXDEFINE_GUID(CID_IFXAuthorMeshScrub,
0x375C3259, 0xE0BA, 0x436A, 0xBE, 0x98, 0xF4, 0xF6, 0xB5, 0xC0, 0x93, 0x14);

//---------------------------------------------------------------------------
//  CID_IFXAuthorMesh
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXAuthorMesh.  It is not a singleton.
//---------------------------------------------------------------------------
// {88D59E40-7F1B-4CAB-8587-83A74B671400}
IFXDEFINE_GUID(CID_IFXAuthorMesh,
0x88D59E40, 0x7F1B, 0x4CAB, 0x85, 0x87, 0x83, 0xA7, 0x4B, 0x67, 0x14, 0x00);

//---------------------------------------------------------------------------
//  CID_IFXAuthorCLODResource
//---------------------------------------------------------------------------
// {CA658131-72F1-4dc6-973B-D16CDE9941DF}
IFXDEFINE_GUID(CID_IFXAuthorCLODResource,
0xca658131, 0x72f1, 0x4dc6, 0x97, 0x3b, 0xd1, 0x6c, 0xde, 0x99, 0x41, 0xdf);

//---------------------------------------------------------------------------
//  CID_IFXBitStreamX
//
//  This component supports the following interfaces:  IFXUnknown,
//  IFXBitStream and IFXBitStreamX.  It is not a singleton.
//---------------------------------------------------------------------------
// {C8877656-222E-4d58-A7F5-57EFE15DD128}
IFXDEFINE_GUID(CID_IFXBitStreamX,
0xc8877656, 0x222e, 0x4d58, 0xa7, 0xf5, 0x57, 0xef, 0xe1, 0x5d, 0xd1, 0x28);

//---------------------------------------------------------------------------
//  CID_IFXSkeleton
//
//  This component supports the following interfaces:  IFXUnknown,
//  IFXModifier, IFXMarker and IFXSkeleton.  It is not a singleton.
//---------------------------------------------------------------------------
// {265F92AE-1BFD-4896-AC7B-DBC5D98950A8}
IFXDEFINE_GUID(CID_IFXSkeleton,
0x265f92ae, 0x1bfd, 0x4896, 0xac, 0x7b, 0xdb, 0xc5, 0xd9, 0x89, 0x50, 0xa8);

//---------------------------------------------------------------------------
//  CID_IFXBoneWeightsModifier
//
//  This component supports the following interfaces:  IFXUnknown,
//  IFXModifier, IFXMarker and IFXSkeleton.  It is not a singleton.
//---------------------------------------------------------------------------
// {F19059D5-88DB-408b-A1BC-15E688481FC1}
IFXDEFINE_GUID(CID_IFXBoneWeightsModifier,
0xf19059d5, 0x88db, 0x408b, 0xa1, 0xbc, 0x15, 0xe6, 0x88, 0x48, 0x1f, 0xc1);

//---------------------------------------------------------------------------
//  CID_IFXBoundHierarchy
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXBoundHierarchy.  It is not a singleton.
//---------------------------------------------------------------------------
// {651FA3B9-8E03-4046-9688-4BA58FA2316A}
IFXDEFINE_GUID(CID_IFXBoundHierarchy,
0x651fa3b9, 0x8e03, 0x4046, 0x96, 0x88, 0x4b, 0xa5, 0x8f, 0xa2, 0x31, 0x6a);

//---------------------------------------------------------------------------
//  CID_IFXBoundSphereDataElement
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXBoundHierarchy.  It is not a singleton.
//---------------------------------------------------------------------------
// {4F01B5AC-0CD7-4501-B94B-3C1533681756}
IFXDEFINE_GUID(CID_IFXBoundSphereDataElement,
0x4f01b5ac, 0xcd7, 0x4501, 0xb9, 0x4b, 0x3c, 0x15, 0x33, 0x68, 0x17, 0x56);

//---------------------------------------------------------------------------
/**
	This CID identifies the CIFXConnector component.  It is not a
	singleton.

	@note	The GUID string is {765AA442-DFF6-43a4-8085-29070613F4A9}
*/
IFXDEFINE_GUID(CID_IFXConnector,
0x765aa442, 0xdff6, 0x43a4, 0x80, 0x85, 0x29, 0x7, 0x6, 0x13, 0xf4, 0xa9);

//---------------------------------------------------------------------------
//  CID_IFXContour
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXContour.  It is not a singleton.
//---------------------------------------------------------------------------
// {139C63D1-109C-11d4-A120-00A0C9A0F93B}
IFXDEFINE_GUID(CID_IFXContour,
0x139c63d1, 0x109c, 0x11d4, 0xa1, 0x20, 0x0, 0xa0, 0xc9, 0xa0, 0xf9, 0x3b);

//---------------------------------------------------------------------------
//  CID_IFXContourExtruder
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXContourExtruder.  It is not a singleton.
//---------------------------------------------------------------------------
// {139C63D3-109C-11d4-A120-00A0C9A0F93B}
IFXDEFINE_GUID(CID_IFXContourExtruder,
0x139c63d3, 0x109c, 0x11d4, 0xa1, 0x20, 0x0, 0xa0, 0xc9, 0xa0, 0xf9, 0x3b);

//---------------------------------------------------------------------------
//  CID_IFXContourGenerator
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXContourGenerator.  It is not a singleton.
//---------------------------------------------------------------------------
// {139C63D2-109C-11d4-A120-00A0C9A0F93B}
IFXDEFINE_GUID(CID_IFXContourGenerator,
0x139c63d2, 0x109c, 0x11d4, 0xa1, 0x20, 0x0, 0xa0, 0xc9, 0xa0, 0xf9, 0x3b);

//---------------------------------------------------------------------------
//  CID_IFXContourTessellator
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXContourTessellator.  It is not a singleton.
//---------------------------------------------------------------------------
// {139C63D4-109C-11d4-A120-00A0C9A0F93B}
IFXDEFINE_GUID(CID_IFXContourTessellator,
0x139c63d4, 0x109c, 0x11d4, 0xa1, 0x20, 0x0, 0xa0, 0xc9, 0xa0, 0xf9, 0x3b);

//---------------------------------------------------------------------------
//  CID_IFXCoreServices
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXCoreServices.  It is not a singleton.
//---------------------------------------------------------------------------
// {AE842700-A9E5-11d3-98A8-00A0C9902D25}
IFXDEFINE_GUID(CID_IFXCoreServices,
0xae842700, 0xa9e5, 0x11d3, 0x98, 0xa8, 0x0, 0xa0, 0xc9, 0x90, 0x2d, 0x25);

//---------------------------------------------------------------------------
//  CID_IFXCoreServicesRef
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXCoreServicesRef.  It is not a singleton.
//---------------------------------------------------------------------------
// {B47CC270-AD04-11d3-98A9-00A0C9902D25}
IFXDEFINE_GUID(CID_IFXCoreServicesRef,
0xb47cc270, 0xad04, 0x11d3, 0x98, 0xa9, 0x0, 0xa0, 0xc9, 0x90, 0x2d, 0x25);

//---------------------------------------------------------------------------
//  CID_IFXDataBlock
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXDataBlock.  It is not a singleton.
//---------------------------------------------------------------------------
// {9DCB6A21-9DE3-11d3-98A5-00A0C9902D25}
IFXDEFINE_GUID(CID_IFXDataBlock,
0x9dcb6a21, 0x9de3, 0x11d3, 0x98, 0xa5, 0x0, 0xa0, 0xc9, 0x90, 0x2d, 0x25);

//---------------------------------------------------------------------------
//  CID_IFXDataBlockX
//
//  This component supports the following interfaces:  IFXUnknown,
//  IFXDataBlock and IFXDataBlockX.  It is not a singleton.
//---------------------------------------------------------------------------
// {989A64CE-1E2D-48d9-B9F7-967BA866E0C4}
IFXDEFINE_GUID(CID_IFXDataBlockX,
0x989a64ce, 0x1e2d, 0x48d9, 0xb9, 0xf7, 0x96, 0x7b, 0xa8, 0x66, 0xe0, 0xc4);

//---------------------------------------------------------------------------
//  CID_IFXDataBlockQueueX
//
//  This component supports the following interfaces:  IFXUnknown,
//  IFXDataBlockQueue and IFXDataBlockQueueX.  It is not a singleton.
//---------------------------------------------------------------------------
// {6B53D3D0-7684-4a2f-94B1-3DA4CB618D69}
IFXDEFINE_GUID(CID_IFXDataBlockQueueX,
0x6b53d3d0, 0x7684, 0x4a2f, 0x94, 0xb1, 0x3d, 0xa4, 0xcb, 0x61, 0x8d, 0x69);

//---------------------------------------------------------------------------
//  CID_IFXModifierDataPacket
//
//  This component supports the following interfaces:  IFXUnknown,
//  IFXDataPacket, and IFXModifierDataPacket.  It is not a singleton.
//---------------------------------------------------------------------------

// {54751263-EFD1-4711-9C45-3C730D89B063}
IFXDEFINE_GUID(CID_IFXModifierDataPacket,
0x54751263, 0xefd1, 0x4711, 0x9c, 0x45, 0x3c, 0x73, 0xd, 0x89, 0xb0, 0x63);

//---------------------------------------------------------------------------
//  CID_IFXGlyphCurveToBlock
//
//  This component supports the following interfaces:  IFXUnknown,
//  IFXGlyph2DCommands and IFXGlyphCurveToBlock.  It is not a singleton.
//---------------------------------------------------------------------------
// {CBC4ECA5-6324-11d4-A145-00A0C9A0F93B}
IFXDEFINE_GUID(CID_IFXGlyphCurveToBlock,
0xcbc4eca5, 0x6324, 0x11d4, 0xa1, 0x45, 0x0, 0xa0, 0xc9, 0xa0, 0xf9, 0x3b);

//---------------------------------------------------------------------------
//  CID_IFXGlyphLineToBlock
//
//  This component supports the following interfaces:  IFXUnknown,
//  IFXGlyph2DCommands and IFXGlyphLineToBlock.  It is not a singleton.
//---------------------------------------------------------------------------
// {CBC4ECA4-6324-11d4-A145-00A0C9A0F93B}
IFXDEFINE_GUID(CID_IFXGlyphLineToBlock,
0xcbc4eca4, 0x6324, 0x11d4, 0xa1, 0x45, 0x0, 0xa0, 0xc9, 0xa0, 0xf9, 0x3b);

//---------------------------------------------------------------------------
//  CID_IFXGlyphMoveToBlock
//
//  This component supports the following interfaces:  IFXUnknown,
//  IFXGlyph2DCommands and IFXGlyphMoveToBlock.  It is not a singleton.
//---------------------------------------------------------------------------
// {CBC4ECA3-6324-11d4-A145-00A0C9A0F93B}
IFXDEFINE_GUID(CID_IFXGlyphMoveToBlock,
0xcbc4eca3, 0x6324, 0x11d4, 0xa1, 0x45, 0x0, 0xa0, 0xc9, 0xa0, 0xf9, 0x3b);

//---------------------------------------------------------------------------
//  CID_IFXGlyphTagBlock
//
//  This component supports the following interfaces:  IFXUnknown,
//  IFXGlyph2DCommands and IFXGlyphTagBlock.  It is not a singleton.
//---------------------------------------------------------------------------
// {CBC4ECA2-6324-11d4-A145-00A0C9A0F93B}
IFXDEFINE_GUID(CID_IFXGlyphTagBlock,
0xcbc4eca2, 0x6324, 0x11d4, 0xa1, 0x45, 0x0, 0xa0, 0xc9, 0xa0, 0xf9, 0x3b);

//---------------------------------------------------------------------------
//  CID_IFXGroup
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXGroup.  It is not a singleton.
//---------------------------------------------------------------------------
// {4517C9F2-B31E-11d3-94B1-00A0C9A0FBAE}
IFXDEFINE_GUID(CID_IFXGroup,
0x4517c9f2, 0xb31e, 0x11d3, 0x94, 0xb1, 0x0, 0xa0, 0xc9, 0xa0, 0xfb, 0xae);

//---------------------------------------------------------------------------
//  CID_IFXHashMap
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXHashMap.  It is not a singleton.
//---------------------------------------------------------------------------
// {A14FECF1-C916-11d3-A101-00A0C9A0F93B}
IFXDEFINE_GUID(CID_IFXHashMap,
0xa14fecf1, 0xc916, 0x11d3, 0xa1, 0x1, 0x0, 0xa0, 0xc9, 0xa0, 0xf9, 0x3b);

//---------------------------------------------------------------------------
//  CID_IFXIDManager
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXIDManager.  It is not a singleton.
//---------------------------------------------------------------------------
// {89D9CE38-D0AD-48c0-AB8A-DBA10A50075F}
IFXDEFINE_GUID(CID_IFXIDManager,
0x89d9ce38, 0xd0ad, 0x48c0, 0xab, 0x8a, 0xdb, 0xa1, 0xa, 0x50, 0x7, 0x5f);

//---------------------------------------------------------------------------
//  CID_IFXLight
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXLight.  It is not a singleton.
//---------------------------------------------------------------------------
// {4517C9F1-B31E-11d3-94B1-00A0C9A0FBAE}
IFXDEFINE_GUID(CID_IFXLight,
0x4517c9f1, 0xb31e, 0x11d3, 0x94, 0xb1, 0x0, 0xa0, 0xc9, 0xa0, 0xfb, 0xae);

//---------------------------------------------------------------------------
//  CID_IFXLightResource
//---------------------------------------------------------------------------
// {B16EC135-9B26-4ee2-8880-BC56C3B11258}
IFXDEFINE_GUID(CID_IFXLightResource,
0xb16ec135, 0x9b26, 0x4ee2, 0x88, 0x80, 0xbc, 0x56, 0xc3, 0xb1, 0x12, 0x58);

//---------------------------------------------------------------------------
//  CID_IFXViewResource
//---------------------------------------------------------------------------
// {70007141-496B-4c5f-B070-EEFF9939A7D5}
IFXDEFINE_GUID(CID_IFXViewResource,
0x70007141, 0x496b, 0x4c5f, 0xb0, 0x70, 0xee, 0xff, 0x99, 0x39, 0xa7, 0xd5);

//---------------------------------------------------------------------------
//  CID_IFXLightSet
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXLightSet.  It is not a singleton.
//---------------------------------------------------------------------------
// {C02ABB29-DA38-4f3a-B155-ED60F3D80F78}
IFXDEFINE_GUID(CID_IFXLightSet,
0xc02abb29, 0xda38, 0x4f3a, 0xb1, 0x55, 0xed, 0x60, 0xf3, 0xd8, 0xf, 0x78);

//---------------------------------------------------------------------------
//  CID_IFXImageCodec
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXImageCodec.  It is not a singleton.
//---------------------------------------------------------------------------
// {84946839-A431-4882-8A27-06E150928404}
IFXDEFINE_GUID(CID_IFXImageCodec,
0x84946839, 0xa431, 0x4882, 0x8a, 0x27, 0x6, 0xe1, 0x50, 0x92, 0x84, 0x4);

//---------------------------------------------------------------------------
//  CID_IFXCLODModifier
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXCLODModifier.  It is not a singleton.
//---------------------------------------------------------------------------
// {1E63DE7A-D087-4a5c-A37D-84FF30BF27CC}
IFXDEFINE_GUID(CID_IFXCLODModifier,
0x1e63de7a, 0xd087, 0x4a5c, 0xa3, 0x7d, 0x84, 0xff, 0x30, 0xbf, 0x27, 0xcc);

//---------------------------------------------------------------------------
//  CID_IFXMaterialResource
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXMaterialResource.  It is not a singleton.
//---------------------------------------------------------------------------
// {CA165392-B33D-11d3-94B1-00A0C9A0FBAE}
IFXDEFINE_GUID(CID_IFXMaterialResource,
0xca165392, 0xb33d, 0x11d3, 0x94, 0xb1, 0x0, 0xa0, 0xc9, 0xa0, 0xfb, 0xae);

IFXDEFINE_GUID(CID_IFXMesh,
0xf13d4299, 0xb4c2, 0x47de, 0xa1, 0xc0, 0x3e, 0x52, 0x2a, 0x72, 0x16, 0x19);
IFXDEFINE_GUID(CID_IFXInterleavedData,
0xec5d3f96, 0xab92, 0x44b9, 0x97, 0xa2, 0x91, 0x74, 0x49, 0x80, 0xaf, 0x64);
IFXDEFINE_GUID(CID_IFXMeshGroup,
0x936c8bd6, 0xca9c, 0x4b24, 0xb0, 0xc3, 0x3a, 0xd0, 0xff, 0xf2, 0xb, 0x81);
IFXDEFINE_GUID(CID_IFXNeighborMesh,
0x39db791d, 0xc7b7, 0x4407, 0xa7, 0xcd, 0x73, 0x68, 0xc1, 0x5f, 0x43, 0xf2);

//---------------------------------------------------------------------------
//  CID_IFXMixerConstruct
//
//  This component supports the following interfaces:  IFXUnknown,
//  and IFXMixerConstruct.  It is not a singleton.
//---------------------------------------------------------------------------
// {DCDB21A1-528A-11d4-BE4C-00A0C9D6AB25}
IFXDEFINE_GUID(CID_IFXMixerConstruct,
0xdcdb21a1, 0x528a, 0x11d4, 0xbe, 0x4c, 0x0, 0xa0, 0xc9, 0xd6, 0xab, 0x25);

//---------------------------------------------------------------------------
//  CID_IFXModel
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXModel.  It is not a singleton.
//---------------------------------------------------------------------------
// {4517C9F0-B31E-11d3-94B1-00A0C9A0FBAE}
IFXDEFINE_GUID(CID_IFXModel,
0x4517c9f0, 0xb31e, 0x11d3, 0x94, 0xb1, 0x0, 0xa0, 0xc9, 0xa0, 0xfb, 0xae);

//---------------------------------------------------------------------------
//  CID_IFXModifierChain
//
//  This component supports the following interfaces:  IFXUnknown,
//  IFXSubject and IFXModifierChain.  It is not a singleton.
//---------------------------------------------------------------------------
// {543A4F04-65AB-48fc-A013-42119E042BB7}
IFXDEFINE_GUID(CID_IFXModifierChain,
0x543a4f04, 0x65ab, 0x48fc, 0xa0, 0x13, 0x42, 0x11, 0x9e, 0x4, 0x2b, 0xb7);

//---------------------------------------------------------------------------
//  CID_IFXDidRegistry
//
//  This component supports the following interfaces:  IFXUnknown,
//  IFXSubject and IFXDidRegistry.  It is a singleton.
//---------------------------------------------------------------------------
// {EA0098EE-1BD1-4594-AED8-E0BAD6CBB49D}
IFXDEFINE_GUID(CID_IFXDidRegistry,
0xea0098ee, 0x1bd1, 0x4594, 0xae, 0xd8, 0xe0, 0xba, 0xd6, 0xcb, 0xb4, 0x9d);

//---------------------------------------------------------------------------
//  CID_IFXMotionResource
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXMotionResource.  It is not a singleton.
//---------------------------------------------------------------------------
// {5CC4FF81-1794-11d4-8ED6-009027A6D7B4}
IFXDEFINE_GUID(CID_IFXMotionResource,
0x5cc4ff81, 0x1794, 0x11d4, 0x8e, 0xd6, 0x0, 0x90, 0x27, 0xa6, 0xd7, 0xb4);

//---------------------------------------------------------------------------
//  CID_IFXShaderList
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXDecoder.  It is not a singleton.
//---------------------------------------------------------------------------
// {4E245D74-19D8-411E-BA22-AE450207DA9C}
IFXDEFINE_GUID(CID_IFXShaderList,
0x4E245D74, 0x19D8, 0x411E, 0xBA, 0x22, 0xAE, 0x45, 0x02, 0x07, 0xDA, 0x9C);

//---------------------------------------------------------------------------
//  CID_IFXGlyph2DModifier
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXGlyph2DModifier.  It is not a singleton.
//---------------------------------------------------------------------------
// {51F08BB1-1BC0-11d4-A121-00A0C9A0F93B}
IFXDEFINE_GUID(CID_IFXGlyph2DModifier,
0x51f08bb1, 0x1bc0, 0x11d4, 0xa1, 0x21, 0x0, 0xa0, 0xc9, 0xa0, 0xf9, 0x3b);


//---------------------------------------------------------------------------
//  CID_IFXPalette
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXPalette.  It is not a singleton.
//---------------------------------------------------------------------------
// {D12EC0A0-ADD7-11d3-A0E9-00A0C9A0F93B}
IFXDEFINE_GUID(CID_IFXPalette,
0xd12ec0a0, 0xadd7, 0x11d3, 0xa0, 0xe9, 0x0, 0xa0, 0xc9, 0xa0, 0xf9, 0x3b);

//---------------------------------------------------------------------------
//  CID_IFXPerformanceTimer
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXPerformanceTimer.  It is a singleton.
//---------------------------------------------------------------------------
// {33ACB9A0-9772-11d3-A886-00A0C9779AE4}
IFXDEFINE_GUID(CID_IFXPerformanceTimer,
0x33acb9a0, 0x9772, 0x11d3, 0xa8, 0x86, 0x0, 0xa0, 0xc9, 0x77, 0x9a, 0xe4);

//---------------------------------------------------------------------------
//  CID_IFXPickObject
//
//  This component supports the following interfaces:  IFXUnknown
//---------------------------------------------------------------------------
// {B52C7F57-4664-49c8-BBBB-AC03687339AF}
IFXDEFINE_GUID(CID_IFXPickObject,
0xb52c7f57, 0x4664, 0x49c8, 0xbb, 0xbb, 0xac, 0x3, 0x68, 0x73, 0x39, 0xaf);

//---------------------------------------------------------------------------
//  CID_IFXRenderPass
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXObserver.  It is not a singleton.
//---------------------------------------------------------------------------
// {8EAB7828-3A6A-4bfa-B24D-D03EE72CC028}
IFXDEFINE_GUID(CID_IFXRenderPass,
0x8eab7828, 0x3a6a, 0x4bfa, 0xb2, 0x4d, 0xd0, 0x3e, 0xe7, 0x2c, 0xc0, 0x28);

//---------------------------------------------------------------------------
//  CID_IFXSceneGraph
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXSceneGraph.  It is not a singleton.
//---------------------------------------------------------------------------
// {4A89D430-996F-11d3-8469-00A0C939B104}
IFXDEFINE_GUID(CID_IFXSceneGraph,
0x4a89d430, 0x996f, 0x11d3, 0x84, 0x69, 0x0, 0xa0, 0xc9, 0x39, 0xb1, 0x4);

//---------------------------------------------------------------------------
//  CID_IFXSimpleCollection
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXSimpleCollection.  It is not a singleton.
//---------------------------------------------------------------------------
// {74FBCC9B-E324-4f07-BAA4-456F8C42D0E0}
IFXDEFINE_GUID(CID_IFXSimpleCollection,
0x74fbcc9b, 0xe324, 0x4f07, 0xba, 0xa4, 0x45, 0x6f, 0x8c, 0x42, 0xd0, 0xe0);

//---------------------------------------------------------------------------
//  CID_IFXAnimationModifier
//
//  This component supports the following interfaces:  IFXUnknown
//  and IFXAnimationModifier.  It is not a singleton.
//---------------------------------------------------------------------------
// {58EEA3D4-A384-4372-BB3C-5A599096F4A6}
IFXDEFINE_GUID(CID_IFXAnimationModifier,
0x58eea3d4, 0xa384, 0x4372, 0xbb, 0x3c, 0x5a, 0x59, 0x90, 0x96, 0xf4, 0xa6);

//---------------------------------------------------------------------------
//  CID_IFXAuthorPointSet
//
//  This component supports the following interfaces:  IFXUnknown and
//  CID_IFXAuthorPointSet.  It is not a singleton.
//---------------------------------------------------------------------------
// {09F6CB19-38B7-4910-8FC0-75BF16B16B5F}
IFXDEFINE_GUID(CID_IFXAuthorPointSet,
0x9f6cb19, 0x38b7, 0x4910, 0x8f, 0xc0, 0x75, 0xbf, 0x16, 0xb1, 0x6b, 0x5f);

// {FDC847A4-52B2-4876-93C4-EC0CD94AB9EC}
IFXDEFINE_GUID(CID_IFXAuthorPointSetResource,
0xfdc847a4, 0x52b2, 0x4876, 0x93, 0xc4, 0xec, 0xc, 0xd9, 0x4a, 0xb9, 0xec);


//---------------------------------------------------------------------------
//  CID_IFXAuthorLineSet
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXAuthorLineSet.  It is not a singleton.
//---------------------------------------------------------------------------

// {E30E304C-0E1F-4fc1-B4E7-FF6D9F4C85B5}
IFXDEFINE_GUID(CID_IFXAuthorLineSet,
0xe30e304c, 0xe1f, 0x4fc1, 0xb4, 0xe7, 0xff, 0x6d, 0x9f, 0x4c, 0x85, 0xb5);


//---------------------------------------------------------------------------
//  CID_IFXAuthorLineSetResource
//
//  This component supports the following interfaces:  IFXUnknown,
//  IFXAuthorLineSetResource, IFXModifier, IFXMarkerX, IFXGenerator
//  It is not a singleton.
//---------------------------------------------------------------------------

// {77DE98FD-87D4-49c4-900D-9BD67E8B5782}
IFXDEFINE_GUID(CID_IFXAuthorLineSetResource,
0x77de98fd, 0x87d4, 0x49c4, 0x90, 0xd, 0x9b, 0xd6, 0x7e, 0x8b, 0x57, 0x82);

//---------------------------------------------------------------------------
//  CID_IFXSimpleHash
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXSimpleHash.  It is not a singleton.
//---------------------------------------------------------------------------
// {98BE00B2-D8DA-4311-B733-DFA00ECB5492}
IFXDEFINE_GUID(CID_IFXSimpleHash,
0x98be00b2, 0xd8da, 0x4311, 0xb7, 0x33, 0xdf, 0xa0, 0xe, 0xcb, 0x54, 0x92);

//---------------------------------------------------------------------------
//  CID_IFXSimpleList
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXSimpleList.  It is not a singleton.
//---------------------------------------------------------------------------
// {139C63D5-109C-11d4-A120-00A0C9A0F93B}
IFXDEFINE_GUID(CID_IFXSimpleList,
0x139c63d5, 0x109c, 0x11d4, 0xa1, 0x20, 0x0, 0xa0, 0xc9, 0xa0, 0xf9, 0x3b);

//---------------------------------------------------------------------------
//  CID_IFXSimpleObject
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXSimpleObject.  It is not a singleton.
//---------------------------------------------------------------------------
// {1D4FCA71-9699-4e20-92C5-65D0CA67919A}
IFXDEFINE_GUID(CID_IFXSimpleObject,
0x1d4fca71, 0x9699, 0x4e20, 0x92, 0xc5, 0x65, 0xd0, 0xca, 0x67, 0x91, 0x9a);

//---------------------------------------------------------------------------
//  CID_IFXSpatialSubdiv
//
//  This component supports the following interfaces:  IFXUnknown, IFXCollection
//  IFXSpatialSubdiv.  It is not a singleton.
//---------------------------------------------------------------------------
// {178F5B83-599D-477c-91FE-639E49E1D85D}
IFXDEFINE_GUID(CID_IFXSpatialSubdiv,
0x178f5b83, 0x599d, 0x477c, 0x91, 0xfe, 0x63, 0x9e, 0x49, 0xe1, 0xd8, 0x5d);

//---------------------------------------------------------------------------
//  CID_IFXSubdivModifier
//
//  This component supports the following interfaces:  IFXUnknown, IFXMarker
//  IFXSubdivModifier.  It is not a singleton.
//---------------------------------------------------------------------------
// {B37553B6-1C5E-11d4-94DC-00A0C9A0FBAE}
IFXDEFINE_GUID(CID_IFXSubdivModifier,
0xb37553b6, 0x1c5e, 0x11d4, 0x94, 0xdc, 0x0, 0xa0, 0xc9, 0xa0, 0xfb, 0xae);

//---------------------------------------------------------------------------
//  CID_IFXTextureImageTools
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXTextureImageTools.  It is not a singleton.
//---------------------------------------------------------------------------
// {120B0030-6D8C-4bdf-86EF-E47B4003EEAB}
IFXDEFINE_GUID(CID_IFXTextureImageTools,
0x120b0030, 0x6d8c, 0x4bdf, 0x86, 0xef, 0xe4, 0x7b, 0x40, 0x3, 0xee, 0xab);

//---------------------------------------------------------------------------
//  CID_IFXTextureObject
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXTextureObject.  It is not a singleton.
//---------------------------------------------------------------------------
// {33ACB9A8-9772-11d3-A886-00A0C9779AE4}
IFXDEFINE_GUID(CID_IFXTextureObject,
0x33acb9a8, 0x9772, 0x11d3, 0xa8, 0x86, 0x0, 0xa0, 0xc9, 0x77, 0x9a, 0xe4);

//---------------------------------------------------------------------------
//  CID_IFXUVGenerator
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXUVGenerator.  It is not a singleton.
//---------------------------------------------------------------------------
// {E1969930-B25D-11d3-A0EB-00A0C9A0F93B}
IFXDEFINE_GUID(CID_IFXUVGenerator,
0xe1969930, 0xb25d, 0x11d3, 0xa0, 0xeb, 0x0, 0xa0, 0xc9, 0xa0, 0xf9, 0x3b);

//---------------------------------------------------------------------------
//  CID_IFXUVMapperCylindrical
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXUVMapperCylindrical.  It is not a singleton.
//---------------------------------------------------------------------------
// {A5F0E2C1-188F-11d4-A121-00A0C9A0F93B}
IFXDEFINE_GUID(CID_IFXUVMapperCylindrical,
0xa5f0e2c1, 0x188f, 0x11d4, 0xa1, 0x21, 0x0, 0xa0, 0xc9, 0xa0, 0xf9, 0x3b);

//---------------------------------------------------------------------------
//  CID_IFXUVMapperNone
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXUVMapper.  It is not a singleton.
//---------------------------------------------------------------------------
// {377EBFD4-8326-4a14-B9D1-0CD9A6CDAE6A}
IFXDEFINE_GUID(CID_IFXUVMapperNone,
0x377ebfd4, 0x8326, 0x4a14, 0xb9, 0xd1, 0xc, 0xd9, 0xa6, 0xcd, 0xae, 0x6a);

//---------------------------------------------------------------------------
//  CID_IFXUVMapperPlanar
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXUVMapperPlanar.  It is not a singleton.
//---------------------------------------------------------------------------
// {A5F0E2C2-188F-11d4-A121-00A0C9A0F93B}
IFXDEFINE_GUID(CID_IFXUVMapperPlanar,
0xa5f0e2c2, 0x188f, 0x11d4, 0xa1, 0x21, 0x0, 0xa0, 0xc9, 0xa0, 0xf9, 0x3b);

//---------------------------------------------------------------------------
//  CID_IFXUVMapperSpherical
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXUVMapperSpherical.  It is not a singleton.
//---------------------------------------------------------------------------
// {A5F0E2C3-188F-11d4-A121-00A0C9A0F93B}
IFXDEFINE_GUID(CID_IFXUVMapperSpherical,
0xa5f0e2c3, 0x188f, 0x11d4, 0xa1, 0x21, 0x0, 0xa0, 0xc9, 0xa0, 0xf9, 0x3b);

//---------------------------------------------------------------------------
//  CID_IFXUVMapperReflection
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXUVMapperReflection.  It is not a singleton.
//---------------------------------------------------------------------------
// {83CFAB26-1564-4923-9055-2E0386D5118D}
IFXDEFINE_GUID(CID_IFXUVMapperReflection,
0x83cfab26, 0x1564, 0x4923, 0x90, 0x55, 0x2e, 0x3, 0x86, 0xd5, 0x11, 0x8d);

//---------------------------------------------------------------------------
//  CID_IFXView
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXView.  It is not a singleton.
//---------------------------------------------------------------------------
// {1BF87865-4A17-4392-AF00-167DE7443F03}
IFXDEFINE_GUID(CID_IFXView,
0x1bf87865, 0x4a17, 0x4392, 0xaf, 0x0, 0x16, 0x7d, 0xe7, 0x44, 0x3f, 0x3);

//---------------------------------------------------------------------------
//  CID_IFXVoidWrapper
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXVoidWrapper.  It is not a singleton.
//---------------------------------------------------------------------------
// {EF8653E1-1ACE-4996-8ED7-C2F8FF1048BA}
IFXDEFINE_GUID(CID_IFXVoidWrapper,
0xef8653e1, 0x1ace, 0x4996, 0x8e, 0xd7, 0xc2, 0xf8, 0xff, 0x10, 0x48, 0xba);

//---------------------------------------------------------------------------
//  CID_IFXShadingModifier
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXShadingModifier.  It is not a singleton.
//---------------------------------------------------------------------------
// {FEA5D3C8-4BF4-4735-AD2B-A94ECB5EA44A}
IFXDEFINE_GUID(CID_IFXShadingModifier,
0xfea5d3c8, 0x4bf4, 0x4735, 0xad, 0x2b, 0xa9, 0x4e, 0xcb, 0x5e, 0xa4, 0x4a);

//---------------------------------------------------------------------------
// CID_IFXDummyModifier
//---------------------------------------------------------------------------
// {93EB6589-7EFD-4d78-9497-FCFDB6697300}
IFXDEFINE_GUID(CID_IFXDummyModifier, 
0x93eb6589, 0x7efd, 0x4d78, 0x94, 0x97, 0xfc, 0xfd, 0xb6, 0x69, 0x73, 0x0);


//---------------------------------------------------------------------------
//  CID_IFXShaderLitTexture
//
//  This component supports the following interfaces:  IFXUnknown and
//  IFXShaderLitTexture.  It is not a singleton.
//---------------------------------------------------------------------------
// {7a298616-8b01-11d3-8467-00a0c939b104}
IFXDEFINE_GUID(CID_IFXShaderLitTexture,
0x7a298616, 0x8b01, 0x11d3, 0x84, 0x67, 0x0, 0xa0, 0xc9, 0x39, 0xb1, 0x4);


#endif
