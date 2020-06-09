//***************************************************************************
//
//  Copyright (c) 2001 - 2006 Intel Corporation
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
	@file	CIFXGlyphGeneratorDecoder.cpp

			The CIFXGlyphGeneratorDecoder is used by the CIFXLoadManager to load
			glyph primitive generators into the scene graph.
			CIFXGlyphGeneratorDecoder exposes an IFXDecoderX interface to the
			CIFXLoadManager for this purpose.
*/

#include "IFXCoreCIDs.h"
#include "IFXGlyph2DModifier.h"
#include "CIFXGlyphModifierDecoder.h"
#include "IFXBlockTypes.h"
#include "IFXCheckX.h"
#include "IFXBitStreamX.h"

CIFXGlyphGeneratorDecoder::CIFXGlyphGeneratorDecoder() :
	m_unitsScale(1.0f)
{
}

CIFXGlyphGeneratorDecoder::~CIFXGlyphGeneratorDecoder()
{
}

U32 CIFXGlyphGeneratorDecoder::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXGlyphGeneratorDecoder::Release()
{
	if ( 1 == m_uRefCount ) {
		delete this;
		return 0;
	}
	return --m_uRefCount;
}

IFXRESULT CIFXGlyphGeneratorDecoder::QueryInterface( IFXREFIID  interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) {
		if ( interfaceId == IID_IFXDecoderX )
		{
			*ppInterface = ( IFXDecoderX* ) this;
			this->AddRef();
		}
		else if ( interfaceId == IID_IFXUnknown )
		{
			*ppInterface = ( IFXUnknown* ) this;
			this->AddRef();
		}
		else
		{
			*ppInterface = NULL;
			rc = IFX_E_UNSUPPORTED;
		}
	} else {
		rc = IFX_E_INVALID_POINTER;
	}

	return rc;
}

// Initialize and get a reference to the core services object
void CIFXGlyphGeneratorDecoder::InitializeX(const IFXLoadConfig &lc)
{
	if( lc.m_units > 0.0f )
		m_unitsScale = lc.m_units;

	IFXModifierBaseDecoder::InitializeX(lc);
}

// Provide next block of data to the loader
void CIFXGlyphGeneratorDecoder::PutNextBlockX( IFXDataBlockX &rDataBlockX )
{
	if(NULL == m_pDataBlockQueueX) {
		IFXCHECKX(IFX_E_NOT_INITIALIZED);
	}

	m_pDataBlockQueueX->AppendBlockX( rDataBlockX );
	CreateObjectX(rDataBlockX, CID_IFXGlyph2DModifier);
}

void CIFXGlyphGeneratorDecoder::TransferX(IFXRESULT &rWarningPartialTransfer)
{
	if(NULL == m_pCoreServices || NULL == m_pObject) {
		IFXCHECKX(IFX_E_NOT_INITIALIZED);
	}

	// For each data block in the list
	BOOL bDone = FALSE;
	while (FALSE == bDone) {
		// Get the next data block
		IFXDECLARELOCAL(IFXDataBlockX,pDataBlockX);
		m_pDataBlockQueueX->GetNextBlockX( pDataBlockX, bDone);

		if(pDataBlockX) {

			// Determine the block type
			U32 uBlockType = 0;
			pDataBlockX->GetBlockTypeX( uBlockType );

			// Process the data block
			switch ( uBlockType ) {
	  case BlockType_Modifier2DGlyphU3D:
		  ProcessGlyph2DBlockX( *pDataBlockX );
		  break;
	  default:
		  break; /// @todo: consider generating error condition here...
			} // end switch (uBlockType)

		}
	}

	rWarningPartialTransfer = IFX_OK;
}

void CIFXGlyphGeneratorDecoder::ProcessGlyph2DBlockX( IFXDataBlockX &rDataBlockX )
{
	U32  uAttributes  = 0;
	IFXMatrix4x4 matrix;

	// Note: calling Initialize() on a Modifier generator causes the
	// Modifier to regenerate itself.  Thus, at this point, the
	// Modifier will have default values for its parameters, and
	// have a valid meshgroup, etc..  Thus, we don't have to cache
	// all the parms that come out of the datablock, we can just
	// stick them right into the Modifier component.

	// grab the IFXGlyph2DModifier handle to the current modifier component:
	IFXDECLARELOCAL(IFXGlyph2DModifier,pGlyph2DModifier);
	IFXCHECKX(m_pObject->QueryInterface( IID_IFXGlyph2DModifier,(void**)&pGlyph2DModifier ) );

	// The following elements are common to all Modifier blocks
	// and are decoded in DecodeCommonElements():
	// 1. ModelName
	// 2. Chain type
	// 3. Index for modifier chain

	// Glyph2D description blocks have the following sections:
	// 1. Attributes (boolean bit field)
	// 2. ExtruderDepth
	// 3. ExtruderSteps
	// 4. ExtruderBevelDepth
	// 5. ExtruderBevelType
	// 6. CombinerFrontCapFacing
	// 7. CombinerBackCapFacing
	// 8. CombinerCurveSteps
	// 9. Glyph command history (buckets o'stuff here...)

	// The following elements are handled in DecodeCommonGeneratorElements():
	// 1. StyleCount
	// 2. Style Name (per style)
	// 3. Shader Name (per mesh, one list of shaders per style)

	// do common modifier stuff:
	// set metadata
	IFXDECLARELOCAL(IFXMetaDataX, pBlockMD);
	IFXDECLARELOCAL(IFXMetaDataX, pObjectMD);
	rDataBlockX.QueryInterface(IID_IFXMetaDataX, (void**)&pBlockMD);
	m_pObject->QueryInterface(IID_IFXMetaDataX, (void**)&pObjectMD);
	pObjectMD->AppendX(pBlockMD);

	// Create the bitstream for reading from the data block
	IFXDECLARELOCAL( IFXBitStreamX, pBitStreamX );
	{
		IFXCHECKX(IFXCreateComponent( CID_IFXBitStreamX, IID_IFXBitStreamX, (void**)&pBitStreamX ));
		pBitStreamX->SetDataBlockX( rDataBlockX );
	}

	// 1. Model Name
	pBitStreamX->ReadIFXStringX(m_stringObjectName);
	IFXDECLARELOCAL(IFXNameMap, pNameMap);
	m_pCoreServices->GetNameMap(IID_IFXNameMap, (void**)&pNameMap);
	IFXCHECKX(pNameMap->Map(m_uLoadId, m_ePalette, m_stringObjectName));

	// 2. Index for modifier chain
	pBitStreamX->ReadU32X( m_uChainPosition );

	// 1. Attributes (boolean bit field)
	// Read out the attributes field:
	pBitStreamX->ReadU32X( uAttributes );
	IFXCHECKX(pGlyph2DModifier->SetAttributes( uAttributes ) );

	//-------------------------------------------------------------------------
	// 2. Glyph command history
	// the command history consists of all of the commands that had been
	// submitted to the ModifierGlyphGen in order to build the 3D text
	// meshes.  Each command consists of a type, and depending upon the
	// type, some number of additional arguments.  As we read each command
	// out, we'll just submit it right away to pGlyph2DModifier, to
	// avoid having to hang onto any of it.

	U32 i       = 0;
	U32 j       = 0;
	U32 uListCount    = 0;
	U32 uCommandType  = 0;
	F32 fArray[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

	// First, read out the number of commands:
	pBitStreamX->ReadU32X( uListCount );

	for (i=0; i<uListCount; i++) {
		// read out the type:
		pBitStreamX->ReadU32X( uCommandType );
		//    pBitStreamX->ReadU32X( attr );

		switch ( uCommandType ) {
	case IFXGlyph2DCommands::IGG_TYPE_STARTGLYPHSTRING:
		IFXCHECKX(pGlyph2DModifier->StartGlyphString() );
		break ;

	case IFXGlyph2DCommands::IGG_TYPE_STARTGLYPH:
		IFXCHECKX(pGlyph2DModifier->StartGlyph() );
		break ;

	case IFXGlyph2DCommands::IGG_TYPE_STARTPATH:
		IFXCHECKX(pGlyph2DModifier->StartPath() );
		break ;

	case IFXGlyph2DCommands::IGG_TYPE_ENDPATH:
		IFXCHECKX(pGlyph2DModifier->EndPath() );
		break ;

	case IFXGlyph2DCommands::IGG_TYPE_ENDGLYPHSTRING:
		IFXCHECKX(pGlyph2DModifier->EndGlyphString() );
		break ;

	case IFXGlyph2DCommands::IGG_TYPE_MOVETO:
		{
			pBitStreamX->ReadF32X( fArray[0] );
			pBitStreamX->ReadF32X( fArray[1] );
			//        pBitStreamX->ReadU32X( uTemp);
			//        if(0 == uTemp) {
			//          bTemp = FALSE;
			//        } else {
			//          bTemp = TRUE;
			//        }
			IFXCHECKX(pGlyph2DModifier->MoveTo( (F64)fArray[0],(F64)fArray[1] ) );
		}
		break ;

	case IFXGlyph2DCommands::IGG_TYPE_LINETO:
		{
			pBitStreamX->ReadF32X( fArray[0] );
			pBitStreamX->ReadF32X( fArray[1] );
			IFXCHECKX(pGlyph2DModifier->LineTo( (F64)fArray[0],(F64)fArray[1] ) );
		}
		break ;

	case IFXGlyph2DCommands::IGG_TYPE_CURVETO:
		{
			for (j=0; j <= 5; j++) {
				pBitStreamX->ReadF32X( fArray[j] );
			}

			IFXCHECKX(pGlyph2DModifier->CurveTo( (F64)fArray[0],
				(F64)fArray[1],
				(F64)fArray[2],
				(F64)fArray[3],
				(F64)fArray[4],
				(F64)fArray[5] ) );
		}
		break;

	case IFXGlyph2DCommands::IGG_TYPE_ENDGLYPH:
		{
			pBitStreamX->ReadF32X( fArray[0] );
			pBitStreamX->ReadF32X( fArray[1] );

			IFXCHECKX(pGlyph2DModifier->EndGlyph( (F64)fArray[0],  (F64)fArray[1] ) );
		}
		break ;

	default:
		IFXCHECKX(IFX_E_UNDEFINED);
		break ;
		} // end switch ( uCommandType )
	}

	// Decode the transform
	F32* pElement = matrix.Raw();

	U32 ii;
	for ( ii = 0; ii < 16 ; ii++)
		pBitStreamX->ReadF32X(pElement[ii]);

	// apply units scaling factor
	matrix.Scale( IFXVector3((F32)m_unitsScale, (F32)m_unitsScale, (F32)m_unitsScale) );

	IFXCHECKX(pGlyph2DModifier->SetTransform(matrix));
	//-------------------------------------------------------------------------
	// Done with Glyph2D specific parameters.
	//-------------------------------------------------------------------------

	// now that we have all GlyphGen specific parameters, we need
	// to make the Modifier regenerate itself so that we have
	// an accurate num_mesh count.
	// NOTE: This may not be necessary for GlyphGen.

}

IFXRESULT IFXAPI_CALLTYPE CIFXGlyphGeneratorDecoder_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface ) {
		// Create the CIFXLoadManager component.
		CIFXGlyphGeneratorDecoder *pComponent = new CIFXGlyphGeneratorDecoder;

		if ( pComponent ) {
			// Perform a temporary AddRef for our usage of the component.
			pComponent->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			rc = pComponent->QueryInterface( interfaceId, ppInterface );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pComponent->Release();
		} else {
			rc = IFX_E_OUT_OF_MEMORY;
		}
	} else {
		rc = IFX_E_INVALID_POINTER;
	}

	IFXRETURN(rc);
}
