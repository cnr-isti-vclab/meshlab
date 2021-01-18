//***************************************************************************
//
//  Copyright (c) 2002 - 2006 Intel Corporation
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
//
//  CIFXGlyphGeneratorEncoder.cpp
//
//  DESCRIPTION:
//    Implementation of the CIFXGlyphGeneratorEncoder.
//    The CIFXGlyphGeneratorEncoder contains 2d glyph modifier encoding
//    functionality that is used by the write manager.
//
//*****************************************************************************


#include "CIFXGlyphModifierEncoder.h"
#include "IFXBlockTypes.h"
#include "IFXCheckX.h"
#include "IFXCoreCIDs.h"
#include "IFXException.h"
#include "IFXGlyph2DModifier.h"

// constructor
CIFXGlyphGeneratorEncoder::CIFXGlyphGeneratorEncoder() :
IFXDEFINEMEMBER(m_pModifier)
{
	m_bInitialized = FALSE;
	m_pBitStream = NULL;
	m_pCoreServices = NULL;
	m_pObject = NULL;
	m_uRefCount = 0;
}

// destructor
CIFXGlyphGeneratorEncoder::~CIFXGlyphGeneratorEncoder()
{
	IFXRELEASE( m_pBitStream );
	IFXRELEASE( m_pCoreServices );
	IFXRELEASE( m_pObject );
}


// IFXUnknown
U32 CIFXGlyphGeneratorEncoder::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXGlyphGeneratorEncoder::Release()
{
	if ( !( --m_uRefCount ) )
	{
		delete this;

		// This second return point is used so that the deleted object's
		// reference count isn't referenced after the memory is released.
		return 0;
	}

	return m_uRefCount;
}

IFXRESULT CIFXGlyphGeneratorEncoder::QueryInterface( IFXREFIID  interfaceId,
													void**   ppInterface )
{
	IFXRESULT rc = IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXEncoderX )
		{
			*ppInterface = ( IFXEncoderX* ) this;
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
	}
	else
		rc = IFX_E_INVALID_POINTER;

	IFXRETURN(rc);
}


// IFXEncoderX
void CIFXGlyphGeneratorEncoder::EncodeX( IFXString& rName, IFXDataBlockQueueX& rDataBlockQueue, F64 units )
{
	IFXGlyph2DModifier*  pGlyph2DModifier = NULL;
	IFXSimpleList*      pGlyphList = NULL;
	IFXUnknown*       pUnk = NULL;
	IFXGlyph2DCommands*   pGlyphCommand = NULL;
	IFXGlyphCurveToBlock* pGlyphCurveTo = NULL;
	IFXGlyphLineToBlock*  pGlyphLineTo = NULL;
	IFXGlyphMoveToBlock*  pGlyphMoveTo = NULL;
	IFXGlyphTagBlock*  pGlyphTag = NULL;

	U32           uAttributes = 0; // start with all 'FALSE'.
	IFXMatrix4x4 offsetMatrix;

	try
	{
		// check for initialization
		if ( FALSE == m_bInitialized )
			throw IFXException( IFX_E_NOT_INITIALIZED );
		if ( NULL == m_pObject )
			throw IFXException( IFX_E_CANNOT_FIND );

		IFXCHECKX( m_pObject->QueryInterface( IID_IFXGlyph2DModifier, (void**)&pGlyph2DModifier ) );

		//    CIFXModifierBaseEncoder::SetName( rName );

		// The following elements are common to all Modifier blocks
		// and are encoded in EncodeCommonElements():
		// 1. ModelGenerator Name
		// 2. Index for modifier chain (U32)

		m_pBitStream->WriteIFXStringX( rName );

		U32 uTemp = 0;
		IFXCHECKX( m_pModifier->GetModifierChainIndex( uTemp ) );
		m_pBitStream->WriteU32X( uTemp );

		// Glyph2D description blocks have the following sections:

		// 1. Attributes (boolean bit field)
		// 2. ExtruderDepth
		// 3. ExtruderSteps
		// 4. ExtruderBevelDepth
		// 5. ExtruderBevelType
		// 6. CombinerFrontCapFacing
		// 7. CombinerBackCapFacing
		// 8. CombinerCurveSteps
		// 9. Glyph command history.

		// The following elements are common to all resource/generator blocks
		// and are encoded in EncodeCommonGeneratorElements():
		// 1. Style Count
		// 2. Mesh Count
		// 3. Style Name (per style)
		// 4. Shader Name (per mesh, one list of shaders per style)


		//-------------------------------------------------------------------------
		// 1. Attributes (boolean bit field)
		IFXCHECKX( pGlyph2DModifier->GetAttributes( &uAttributes ) );
		m_pBitStream->WriteU32X( uAttributes );

		//-------------------------------------------------------------------------
		// 2. Glyph command history
		U32 i = 0;
		U32 j = 0;
		U32 uListCount = 0;
		U32 uCommandType = 0;
		U32 attr = 0;
		F64 fDoubleArray[6] = {0.0};

		IFXCHECKX( pGlyph2DModifier->GetGlyphCommandList( &pGlyphList ) );
		// must Release() pGlyphList.

		if ( NULL != pGlyphList )
		{
			IFXCHECKX( pGlyphList->GetCount( &uListCount ) );
			m_pBitStream->WriteU32X( uListCount );

			for (i=0; (i < uListCount); i++)
			{
				IFXCHECKX( pGlyphList->Get( i, &pUnk ) );

				if ( NULL != pUnk )
				{
					// find out what kind of object it is:
					IFXCHECKX( pUnk->QueryInterface( IID_IFXGlyph2DCommands, (void**)&pGlyphCommand ) );
					IFXCHECKX( pGlyphCommand->GetType( (IFXGlyph2DCommands::EGLYPH_TYPE*)&uCommandType ) );
					IFXCHECKX( pGlyphCommand->GetAttributes( &attr ) );
					m_pBitStream->WriteU32X( uCommandType );

					switch( uCommandType )
					{
					case IFXGlyph2DCommands::IGG_TYPE_STARTGLYPHSTRING:
					case IFXGlyph2DCommands::IGG_TYPE_STARTGLYPH:
					case IFXGlyph2DCommands::IGG_TYPE_STARTPATH:
					case IFXGlyph2DCommands::IGG_TYPE_ENDPATH:
					case IFXGlyph2DCommands::IGG_TYPE_ENDGLYPHSTRING:
						break;

					case IFXGlyph2DCommands::IGG_TYPE_MOVETO:
						{
							// get the MoveTo handle:
							IFXCHECKX( pGlyphCommand->QueryInterface( IID_IFXGlyphMoveToBlock,
								(void**)&pGlyphMoveTo ) );
							IFXCHECKX( pGlyphMoveTo->GetData( &fDoubleArray[0],
								&fDoubleArray[1] ) );

							IFXRELEASE( pGlyphMoveTo );

							m_pBitStream->WriteF32X( (F32)fDoubleArray[0] );
							m_pBitStream->WriteF32X( (F32)fDoubleArray[1] );
						} // end case IGG_TYPE_MOVETO
						break;

					case IFXGlyph2DCommands::IGG_TYPE_LINETO:
						{
							// get the LineTo handle:

							IFXCHECKX( pGlyphCommand->QueryInterface( IID_IFXGlyphLineToBlock,
								(void**)&pGlyphLineTo ) );
							IFXCHECKX( pGlyphLineTo->GetData( &fDoubleArray[0],
								&fDoubleArray[1] ) );

							IFXRELEASE( pGlyphLineTo );

							m_pBitStream->WriteF32X( (F32)fDoubleArray[0] );
							m_pBitStream->WriteF32X( (F32)fDoubleArray[1] );
						} // end case IGG_TYPE_LINETO
						break;

					case IFXGlyph2DCommands::IGG_TYPE_CURVETO:
						{
							// get the CurveTo handle:
							IFXCHECKX( pGlyphCommand->QueryInterface( IID_IFXGlyphCurveToBlock,
								(void**)&pGlyphCurveTo ) );
							IFXCHECKX( pGlyphCurveTo->GetData( &fDoubleArray[0],
								&fDoubleArray[1],
								&fDoubleArray[2],
								&fDoubleArray[3],
								&fDoubleArray[4],
								&fDoubleArray[5],
								&uTemp ) ); // not needed.
							IFXRELEASE( pGlyphCurveTo );

							for (j=0; (j <= 5); j++)
								m_pBitStream->WriteF32X( (F32)fDoubleArray[j] );
						} // end case IGG_TYPE_CURVETO
						break;

					case IFXGlyph2DCommands::IGG_TYPE_ENDGLYPH:
						{
							// get the EndGlyph handle:
							IFXCHECKX( pGlyphCommand->QueryInterface( IID_IFXGlyphTagBlock,
								(void**)&pGlyphTag ) );
							IFXCHECKX( pGlyphTag->GetData( &fDoubleArray[0],
								&fDoubleArray[1] ) );

							IFXRELEASE( pGlyphTag );

							m_pBitStream->WriteF32X( (F32)fDoubleArray[0] );
							m_pBitStream->WriteF32X( (F32)fDoubleArray[1] );
						} // end case IGG_TYPE_ENDGLYPH
						break ;

					default:
						IFXCHECKX(IFX_E_UNDEFINED);
						break ;
					} // end switch( uCommandType )

					IFXRELEASE( pGlyphCommand );
				} // end if ( we got the next entry in the list)

				IFXRELEASE( pUnk );
			} // end for (each element in the simple list)
		} // end if (we got the glyph command list)

		IFXRELEASE( pGlyphList );


		// Write the transform
		offsetMatrix = pGlyph2DModifier->GetTransform();

		// apply units scaling factor
		F32 recUnits = 1.0f/(F32)units;
		offsetMatrix.Scale( IFXVector3(recUnits,recUnits,recUnits) );

		F32* pElement = offsetMatrix.Raw();
		U32 ii;
		for ( ii = 0; ii < 16; ii++)
			m_pBitStream->WriteF32X(pElement[ii]);

		//-------------------------------------------------------------------------
		// Done with Glyph specific parameters.
		//-------------------------------------------------------------------------

		IFXDECLARELOCAL(IFXDataBlockX, pDataBlock);

		// Get a data block from the bitstream
		m_pBitStream->GetDataBlockX(pDataBlock);

		// Set the data block type
		pDataBlock->SetBlockTypeX(BlockType_Modifier2DGlyphU3D);

		// Set the data block priority
		pDataBlock->SetPriorityX(0);

		// set metadata
		IFXDECLARELOCAL(IFXMetaDataX, pBlockMD);
		IFXDECLARELOCAL(IFXMetaDataX, pObjectMD);
		pDataBlock->QueryInterface(IID_IFXMetaDataX, (void**)&pBlockMD);
		m_pModifier->QueryInterface(IID_IFXMetaDataX, (void**)&pObjectMD);
		pBlockMD->AppendX(pObjectMD);

		// Put the data block on the list
		rDataBlockQueue.AppendBlockX(*pDataBlock);

		// clean up
		IFXRELEASE( pGlyph2DModifier );
	}
	catch ( ... )
	{
		IFXRELEASE( pGlyph2DModifier );
		IFXRELEASE( pGlyphList );
		IFXRELEASE( pUnk );
		IFXRELEASE( pGlyphCommand );
		IFXRELEASE( pGlyphCurveTo );
		IFXRELEASE( pGlyphLineTo );
		IFXRELEASE( pGlyphMoveTo );
		throw;
	}
}

void CIFXGlyphGeneratorEncoder::InitializeX( IFXCoreServices& rCoreServices )
{
	try
	{
		// latch onto the core services object passed in
		IFXRELEASE( m_pCoreServices )
			m_pCoreServices = &rCoreServices;
		m_pCoreServices->AddRef();

		// create a bitstream
		IFXRELEASE( m_pBitStream );
		IFXCreateComponent( CID_IFXBitStreamX, IID_IFXBitStreamX, (void**)&m_pBitStream );

		m_bInitialized = TRUE;
	}
	catch ( ... )
	{
		IFXRELEASE( m_pCoreServices );
		IFXRELEASE( m_pBitStream );
		throw;
	}
}

void CIFXGlyphGeneratorEncoder::SetObjectX( IFXUnknown& rObject )
{
	IFXModifier* pModifier= NULL;

	try
	{
		// set the object
		IFXRELEASE( m_pObject );
		m_pObject = &rObject;
		m_pObject->AddRef();


		m_pObject->QueryInterface( IID_IFXModifier, (void**)&pModifier );

		//    CIFXModifierBaseEncoder::SetModifier( *pModifier );
		pModifier->AddRef();
		IFXRELEASE( m_pModifier );
		m_pModifier = pModifier;

		IFXRELEASE( pModifier );
	}
	catch ( ... )
	{
		IFXRELEASE( m_pObject ); // release the member variable, not the input parameter
		IFXRELEASE( pModifier );
		throw;
	}
}


// Factory friend
IFXRESULT IFXAPI_CALLTYPE CIFXGlyphGeneratorEncoder_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;


	if ( ppInterface )
	{
		// Create the CIFXLoadManager component.
		CIFXGlyphGeneratorEncoder *pComponent = new CIFXGlyphGeneratorEncoder;

		if ( pComponent )
		{
			// Perform a temporary AddRef for our usage of the component.
			pComponent->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			rc = pComponent->QueryInterface( interfaceId, ppInterface );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pComponent->Release();
		}
		else
			rc = IFX_E_OUT_OF_MEMORY;
	}
	else
		rc = IFX_E_INVALID_POINTER;

	IFXRETURN( rc );
}
