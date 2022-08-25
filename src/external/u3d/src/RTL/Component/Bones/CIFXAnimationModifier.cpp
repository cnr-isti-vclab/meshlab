//***************************************************************************
//
//  Copyright (c) 2000 - 2006 Intel Corporation
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
	@file CIFXAnimationModifier.cpp
*/

#include "CIFXAnimationModifier.h"
#include "IFXNotificationManager.h"
#include "IFXReadBuffer.h"
#include "IFXSceneGraph.h"
#include "IFXModifierChain.h"
#include "IFXModel.h"

#include "IFXSchedulingCIDs.h" // CID_IFXNotificationInfo
#include "IFXExportingCIDs.h"
#include "IFXCoreCIDs.h"
#include "IFXAutoRelease.h"

#include "IFXMeshGroup.h"
#include "IFXBonesManagerImpl.h"

/** 
	@warning    All this seem to assume a single bonesmanager for non-keyframe.
				This won't work for shallow cloning. 
*/

CIFXAnimationModifier::CIFXAnimationModifier()
{
	m_uRefCount = 0;
	m_uSimulationTimeDataElementIndex = (U32)-1;
	m_uBonesManagerDataElementIndex = (U32)-1;
	m_uBoneWeightsDataElementIndex = (U32)-1;
	m_uTransformDataElementIndex = (U32)-1;
	m_uMeshGroupDataElementIndex = (U32)-1;
	m_uSkeletonDataElementIndex = (U32)-1;
	m_uLastMeshGroupChangeCount = (U32)-1;
	m_uBonesManagerChangeCount = (U32)-1;
	m_uBoneWeightsChangeCount = (U32)-1;

	m_pBonesManager = NULL;
	m_pCharacter = NULL;
	m_motionManager.SetBonesModifier( this );

	AssociateWithMotionManager( &m_motionManager );

	m_pOutputMeshGroup = NULL;
	m_pSkeletonOutputElement = NULL;

	m_bInitialized = FALSE;
	m_bIsKeyframe = FALSE;

	m_RootTransform.Clear();
}


CIFXAnimationModifier::~CIFXAnimationModifier()
{
	IFXRELEASE( m_pOutputMeshGroup );

	if (m_pSkeletonOutputElement)
	{
		IFXDELETE_ARRAY(m_pSkeletonOutputElement->pBoneArray);
		m_pSkeletonOutputElement->uNumBones = 0;
		IFXDELETE(m_pSkeletonOutputElement);
	}

	if(m_pBonesManager)
	{
		if( m_bIsKeyframe )
		{
			delete (IFXBonesManagerImpl *)m_pBonesManager;
			m_pBonesManager = NULL;
		}
		else
			m_pBonesManager->ResetUnshared();
	}
}


IFXRESULT IFXAPI_CALLTYPE CIFXAnimationModifier_Factory( IFXREFIID iid, void** ppv )
{
	IFXRESULT ir;

	if( ppv )
	{
		// Create the CIFXAnimationModifier component.
		CIFXAnimationModifier *pComponent = new CIFXAnimationModifier();

		if( pComponent )
		{
			// Perform a temporary AddRef for our usage of the component.
			pComponent->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			ir = pComponent->QueryInterface( iid, ppv );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pComponent->Release();
		}
		else
			ir = IFX_E_OUT_OF_MEMORY;
	}
	else
		ir = IFX_E_INVALID_POINTER;

	IFXRETURN(ir);
}

// IFXUnknown
U32 CIFXAnimationModifier::AddRef(void)
{
	return ++m_uRefCount;
}


U32 CIFXAnimationModifier::Release(void)
{
	if( 1 == m_uRefCount )
	{
		PreDestruct();
		delete this;
		return 0;
	}

	return --m_uRefCount;
}


IFXRESULT CIFXAnimationModifier::QueryInterface(IFXREFIID interfaceId,
												void** ppInterface )
{
	IFXRESULT   ir  = IFX_OK;

	if( ppInterface )
	{
		if( interfaceId == IID_IFXUnknown )
			*ppInterface = ( IFXUnknown* ) this;
		else if( interfaceId == IID_IFXModifier )
			*ppInterface = ( IFXModifier* ) this;
		else if( interfaceId == IID_IFXSubject )
			*ppInterface = ( IFXSubject* ) this;
		else if( interfaceId == IID_IFXMarker )
			*ppInterface = ( IFXMarker* ) this;
		else if( interfaceId == IID_IFXMarkerX )
			*ppInterface = (IFXMarkerX*) this;
		else if( interfaceId == IID_IFXAnimationModifier )
			*ppInterface = ( IFXAnimationModifier* ) this;
		else if (interfaceId == IID_IFXMetaDataX)
			*ppInterface = ( IFXMetaDataX* ) this;
		else
		{
			*ppInterface = NULL;
			ir = IFX_E_UNSUPPORTED;
		}

		if(IFXSUCCESS(ir))
			AddRef();
	}
	else
	{
		ir = IFX_E_INVALID_POINTER;
	}

	IFXRETURN(ir);
}


// IFXModifierX
void CIFXAnimationModifier::GetEncoderX(IFXEncoderX*& rpEncoderX)
{
	CIFXMarker::GetEncoderX( CID_IFXAnimationModifierEncoder, rpEncoderX );
}


static const IFXGUID* s_scpSingleTrackDIDs[] =
{
	&DID_IFXTransform
};

static const IFXGUID* s_scpMultiTrackDIDs[] =
{
	&DID_IFXTransform,
	&DID_IFXRenderableGroup,
	&DID_IFXSkeleton
};

static const IFXGUID* s_scpSkeletonIntraDIDs[] =
{
	&DID_IFXRenderableGroup,
};

static const IFXGUID* s_scpTransformInputDIDs[] =
{
	&DID_IFXTransform,
	&DID_IFXSimulationTime
};

static const IFXGUID* s_scpMeshGroupInputDIDs[] =
{
	&DID_IFXSimulationTime,
	&DID_IFXBonesManager,
	&DID_IFXBoneWeights,
	&DID_IFXRenderableGroup
};

static const U32 s_scuMultiTrackUnchangedAttrs[] =
{
	0,// change everything for transform input change
	// don't change everything for MeshGroup input change - just pos & norm..
	IFX_MESHGROUP_CB_ALL ^ ( IFX_MESHGROUP_CB_POSITIONS | IFX_MESHGROUP_CB_NORMALS )
};

// IFXModifier
IFXRESULT CIFXAnimationModifier::GetOutputs(IFXGUID**& rpOutOutputs,
										U32& rOutNumberOfOutputs,
										U32*& rpOutOutputDepAttrs )
{
	if( m_bIsKeyframe )
	{
		rOutNumberOfOutputs = sizeof(s_scpSingleTrackDIDs)/sizeof(IFXGUID*);
		rpOutOutputs = (IFXGUID**)&s_scpSingleTrackDIDs;
		rpOutOutputDepAttrs = NULL;
	}
	else
	{
		rOutNumberOfOutputs = sizeof(s_scpMultiTrackDIDs)/sizeof(IFXGUID*);
		rpOutOutputs = (IFXGUID**)&s_scpMultiTrackDIDs;
		rpOutOutputDepAttrs = (U32*)s_scuMultiTrackUnchangedAttrs;
	}

	return IFX_OK;
}


IFXRESULT CIFXAnimationModifier::GetDependencies(
								IFXGUID*    pInOutputDID,
								IFXGUID**&	rppOutInputDependencies,
								U32&		rOutNumberInputDependencies,
								IFXGUID**&	rppOutOutputDependencies,
								U32&		rOutNumberOutputDependencies,
								U32*&		rpOutOutputDepAttrs )
{
	if( pInOutputDID == &DID_IFXTransform )
	{
		rppOutInputDependencies = (IFXGUID**)s_scpTransformInputDIDs;
		rOutNumberInputDependencies = IFXARRAY_SIZE(s_scpTransformInputDIDs);
		rppOutOutputDependencies = NULL;
		rOutNumberOutputDependencies = 0;
		rpOutOutputDepAttrs = NULL;
	}
	else if( pInOutputDID == &DID_IFXRenderableGroup)
	{
		IFXASSERT( !m_bIsKeyframe );

		rppOutInputDependencies = (IFXGUID**)s_scpMeshGroupInputDIDs;
		rOutNumberInputDependencies = IFXARRAY_SIZE(s_scpMeshGroupInputDIDs);
		rppOutOutputDependencies = NULL;
		rOutNumberOutputDependencies = 0;
		rpOutOutputDepAttrs = NULL;
	}
	else if( pInOutputDID == &DID_IFXSkeleton)
	{
		rppOutInputDependencies = NULL;
		rOutNumberInputDependencies = 0;
		rppOutOutputDependencies = (IFXGUID**)s_scpSkeletonIntraDIDs;
		rOutNumberOutputDependencies = IFXARRAY_SIZE(s_scpSkeletonIntraDIDs);
		rpOutOutputDepAttrs = NULL;
	}
	else
	{
		IFXASSERTBOX(1,
				"CIFXAnimationModifier::GetDependencies() called with unknown output!" );
		return IFX_E_UNDEFINED;
	}

	return IFX_OK;
}

IFXRESULT CIFXAnimationModifier::GenerateOutput(U32 inOutputDataElementIndex,
											void*& rpOutData, BOOL& rNeedRelease)
{
	IFXRESULT result = IFX_OK;

	// BonesPlayer gets bones manager from mod chain, Keyframe player creates its own
	result = InitBonesManager();

	if( m_pBonesManager )
	{
		if( inOutputDataElementIndex == m_uTransformDataElementIndex )
		{
			IFXMatrix4x4* pInputMatrix = NULL;
			U32 NumberElements = 0;
			IFXArray<IFXMatrix4x4> *pAInputMatrix;
			if( IFXSUCCESS(result) )
				result = m_pInputDataPacket->GetDataElement(
						m_uTransformDataElementIndex, (void**)&pAInputMatrix);
			if (IFXSUCCESS(result))
				NumberElements = pAInputMatrix->GetNumberElements();

			U32 simTime = 0;
			if( IFXSUCCESS(result) )
				result = m_pInputDataPacket->GetDataElement(
						m_uSimulationTimeDataElementIndex, (void**)&simTime);

			if( IFXSUCCESS(result) )
			{
				F32 worldTime = simTime*0.001f;
				IFXASSERT(m_pBonesManager);
				m_pBonesManager->SetFloat(IFXBonesManagerImpl::Time, worldTime);

				result=Advance();
				if (result == IFX_E_RESOURCE_NOT_AVAILABLE) {
					result = IFX_W_READING_NOT_COMPLETE;
					rpOutData = pAInputMatrix;
				} else {
					m_pBonesManager->SetBool( IFXBonesManagerImpl::NoDeform, TRUE);
					m_pBonesManager->UpdateMesh( NULL, NULL );

					IFXTransform clearance;
					m_pBonesManager->GetRootClearance( &clearance );

					IFXMatrix4x4* matrixconst;
					U32 i;
					for ( i = 0; i < NumberElements; i++) {
						pInputMatrix = &pAInputMatrix->GetElement(i);

						RootTransform() = *pInputMatrix;
						RootTransform().Multiply( clearance );

						if (m_RootTransform.GetNumberElements() < i+1)
							matrixconst = &m_RootTransform.CreateNewElement();
						else
							matrixconst = &m_RootTransform.GetElement(i);
						*matrixconst = RootTransform().MatrixConst();
					}
					rpOutData=(void*)( &m_RootTransform );
				}
			}
			IFXASSERT( IFXSUCCESS( result ) );
		}
		else if( inOutputDataElementIndex == m_uMeshGroupDataElementIndex )
		{
			IFXDECLARELOCAL(IFXUnknown, pMeshGroupUnknown);
			if( IFXSUCCESS(result) )
				result = m_pInputDataPacket->GetDataElement(
						m_uMeshGroupDataElementIndex, (void**)&pMeshGroupUnknown );

			U32 uMeshGroupChangeCount = 0;
			if( IFXSUCCESS(result) )
				result = m_pInputDataPacket->GetDataElementChangeCount(
						m_uMeshGroupDataElementIndex, uMeshGroupChangeCount );

			IFXDECLARELOCAL(IFXMeshGroup, pInputMeshGroup);
			if( IFXSUCCESS(result) )
				result = pMeshGroupUnknown->QueryInterface(IID_IFXMeshGroup,
														(void**)&pInputMeshGroup);

			if( m_uLastMeshGroupChangeCount != uMeshGroupChangeCount &&
															IFXSUCCESS(result) )
			{
				result = m_pBonesManager->InitMesh( pInputMeshGroup, NULL );
				m_uLastMeshGroupChangeCount = uMeshGroupChangeCount;
			}

			if(IFXSUCCESS(result))
			{
				m_pBonesManager->SetBool(IFXBonesManagerImpl::NoDeform, FALSE);

				m_pCharacter->GrabSkin();

				result = m_pBonesManager->UpdateMesh( pInputMeshGroup,
															&m_pOutputMeshGroup);
				// Leave a reference to output mesh group in this object:
				m_pOutputMeshGroup->QueryInterface(IID_IFXUnknown, &rpOutData);
				rNeedRelease = TRUE;
			}

			IFXASSERT(rpOutData);
			IFXASSERT( IFXSUCCESS( result ) );
		}
		else if( inOutputDataElementIndex == m_uSkeletonDataElementIndex )
		{
			U32                 numBones = 0;
			U32                 i;
			IFXCurrentBoneData  *pBoneArray = NULL;

			IFXASSERT(m_pCharacter);

			numBones = m_pCharacter->GetBoneTableSize();

			// Allocate the skeleton data element if necessary, or
			// clean out the previous contents of the data element if the number
			// of bones has changed.
			if (m_pSkeletonOutputElement)
			{
				if (m_pSkeletonOutputElement->uNumBones != numBones)
				{
					IFXDELETE_ARRAY(m_pSkeletonOutputElement->pBoneArray);
					m_pSkeletonOutputElement->uNumBones = numBones;
					m_pSkeletonOutputElement->pBoneArray = new IFXCurrentBoneData[numBones];
				}
			}
			else
			{
				m_pSkeletonOutputElement = new IFXSkeletonDataElement;
				m_pSkeletonOutputElement->uNumBones = numBones;
				m_pSkeletonOutputElement->pBoneArray = new IFXCurrentBoneData[numBones];
			}

			IFXASSERT(m_pSkeletonOutputElement);
			IFXASSERT(m_pSkeletonOutputElement->pBoneArray);

			pBoneArray = (IFXCurrentBoneData *)m_pSkeletonOutputElement->pBoneArray;

			// Now fill it up with data from the bones
			for ( i=0; i< numBones; i++)
			{
				IFXBoneNode *pBonenode=m_pCharacter->LookupBoneIndex(i);
				IFXBoneNode *pParentNode = NULL;

				if(!pBonenode)
				{
					pBoneArray[i].isValid = 0;
					continue;
				}
				else
					pBoneArray[i].isValid = 1;

				// Get basic bone information
				pBoneArray[i].boneLength = pBonenode->GetLength();
				pBoneArray[i].boneTransform = (IFXMatrix4x4)pBonenode->StoredTransform().MatrixConst();
				pBoneArray[i].boneDisplacement = pBonenode->DisplacementConst();

				// Get the index of our parent
				pParentNode = (IFXBoneNode *)pBonenode->ParentConst();
				pBoneArray[i].iParentIndex = pParentNode->GetBoneIndex();
			}

			rpOutData=(void*)m_pSkeletonOutputElement;

		}
		else
			IFXASSERTBOX(1,"CIFXAnimationModifier::GenerateOutput() index invalid.");
	}

	return result;
}


IFXRESULT CIFXAnimationModifier::SetDataPacket(
								IFXModifierDataPacket* pInInputDataPacket,
								IFXModifierDataPacket* pInOutputDataPacket )
{
	IFXRESULT result = IFX_OK;

	IFXRELEASE( m_pInputDataPacket );
	IFXRELEASE( m_pModifierDataPacket );

	if( pInOutputDataPacket && pInOutputDataPacket )
	{
		pInOutputDataPacket->AddRef();
		m_pModifierDataPacket = pInOutputDataPacket;
		pInInputDataPacket->AddRef();
		m_pInputDataPacket = pInInputDataPacket;

		result = m_pInputDataPacket->GetDataElementIndex(
					DID_IFXTransform, m_uTransformDataElementIndex );

		result = m_pInputDataPacket->GetDataElementIndex(
					DID_IFXSimulationTime, m_uSimulationTimeDataElementIndex );

		m_uMeshGroupDataElementIndex = (U32)-1;
		m_uBonesManagerDataElementIndex = (U32)-1;
		m_uBoneWeightsDataElementIndex = (U32)-1;

		if( !m_bIsKeyframe )
		{
			result |= m_pInputDataPacket->GetDataElementIndex(
						DID_IFXRenderableGroup, m_uMeshGroupDataElementIndex );

			IFXASSERT( IFXSUCCESS( result ) );

			result |= m_pInputDataPacket->GetDataElementIndex(
						DID_IFXBonesManager, m_uBonesManagerDataElementIndex );

			IFXASSERT( IFXSUCCESS( result ) );

			result |= m_pModifierDataPacket->GetDataElementIndex(
						DID_IFXSkeleton, m_uSkeletonDataElementIndex );

			IFXASSERT( IFXSUCCESS( result ) );

			result |= m_pModifierDataPacket->GetDataElementIndex(
						DID_IFXBoneWeights, m_uBoneWeightsDataElementIndex );

			IFXASSERT( IFXSUCCESS( result ) );
		}

		Setup();
	}
	else
	{
		m_uTransformDataElementIndex = (U32)-1;
		m_uSimulationTimeDataElementIndex = (U32)-1;
		m_uMeshGroupDataElementIndex = (U32)-1;
		m_uBonesManagerDataElementIndex = (U32)-1;
		m_uBoneWeightsDataElementIndex = (U32)-1;
		m_uSkeletonDataElementIndex = (U32)-1;
	}

	return result;
}


IFXRESULT CIFXAnimationModifier::Notify(IFXModifierMessage eInMessage,
									void* pMessageContext)
{
	IFXRESULT result = IFX_OK;

	IFXRETURN(result);
}

void CIFXAnimationModifier::SetAsBones()
{
	if( !m_bInitialized )
	{
		m_bIsKeyframe = FALSE;
		m_bInitialized = TRUE;
	}
}

void CIFXAnimationModifier::SetAsKeyframe()
{
	if( !m_bInitialized )
	{
		m_bIsKeyframe = TRUE;
		m_bInitialized = TRUE;
		SetBonesManager( new IFXBonesManagerImpl() );
	}
}

BOOL CIFXAnimationModifier::IsKeyframe()
{
	return m_bIsKeyframe;
}

IFXBonesManager *CIFXAnimationModifier::GetBonesManagerNR(void)
{
	if( !m_pBonesManager )
	{
		InitBonesManager();
	}

	return m_pBonesManager;
}

IFXRESULT CIFXAnimationModifier::InitBonesManager( void )
{
	IFXRESULT result = IFX_OK;

	if( m_pInputDataPacket )
	{
		if( !m_bIsKeyframe ) // execute this only for bones animation and when BM isn't init-ed
		{
			U32 uBoneWeightsChangeCount = 0;

			result = m_pInputDataPacket->GetDataElementChangeCount(
				m_uBoneWeightsDataElementIndex, uBoneWeightsChangeCount );

			if( IFXSUCCESS( result ) )
			{
				if( m_uBoneWeightsChangeCount != uBoneWeightsChangeCount )
				{
					IFXUnknown *pUnk = NULL;

					result = m_pInputDataPacket->GetDataElement(
						m_uBoneWeightsDataElementIndex, (void**)&pUnk );

					m_uBoneWeightsChangeCount = uBoneWeightsChangeCount;
				}
			}

			if( IFXSUCCESS( result ) )
			{
				U32 uBonesManagerChangeCount = 0;

				result = m_pInputDataPacket->GetDataElementChangeCount(
					m_uBonesManagerDataElementIndex, uBonesManagerChangeCount );

				IFXASSERT( IFX_OK == result );

				if( m_uBonesManagerChangeCount != uBonesManagerChangeCount )
				{
					IFXBonesManager *pBonesManager = NULL;

					result = m_pInputDataPacket->GetDataElement(
						m_uBonesManagerDataElementIndex, (void**)&pBonesManager );

					IFXASSERT( IFX_OK == result );

					SetBonesManager( pBonesManager );

					m_uBonesManagerChangeCount = uBonesManagerChangeCount;
				}
			}
		}
	}
	else
	{
		IFXASSERT(0);
	}

	return result;
}

void CIFXAnimationModifier::SetBonesManager(IFXBonesManager *pBonesManager)
{
	if( m_bIsKeyframe && m_pBonesManager)
	{
		delete (IFXBonesManagerImpl *)m_pBonesManager;
		m_pBonesManager = NULL;
	}

	m_pBonesManager = pBonesManager;

	m_pCharacter = NULL;

	pBonesManager->GetCharacter(&m_pCharacter);

	IFXASSERT(m_pCharacter);
	pBonesManager->SetBool(IFXBonesManagerImpl::RootClearTranslate,TRUE);
	pBonesManager->SetBool(IFXBonesManagerImpl::RootClearRotate,TRUE);

	AssociateWithBonesManager(pBonesManager);

	if( m_bIsKeyframe )
	{
		//* make a root bone for keyframing
		IFXCharacter *pCharacter = NULL;
		pBonesManager->GetCharacter(&pCharacter);
		IFXASSERT(pCharacter);

		pCharacter->Reset();

		IFXBoneNode *pBonenode=new IFXBoneNode(pCharacter);
		pCharacter->SetBoneAtIndex(0,pBonenode);
		pCharacter->AppendChild(pBonenode);
		pBonenode->SetParent(m_pCharacter);
		pBonenode->SetName("key");
		pBonenode->SetBoneIndex(0);
	}
}


void CIFXAnimationModifier::Setup(void)
{
	IFXRESULT result = IFX_OK;

	// Get the motion palette
	if(!m_motionManager.GetMotionPalette())
	{
		IFXPalette* pMotionResourcePalette = NULL;
		IFXPalette* pMixerResourcePalette = NULL;
		if(IFXSUCCESS(result))
		{
			result = m_pSceneGraph->GetPalette(IFXSceneGraph::MOTION,
												&pMotionResourcePalette);
			IFXASSERT(pMotionResourcePalette);

			result = m_pSceneGraph->GetPalette(IFXSceneGraph::MIXER,
												&pMixerResourcePalette);
			IFXASSERT(pMixerResourcePalette);

			m_motionManager.SetPalettes(pMotionResourcePalette,
										pMixerResourcePalette);
		}
	}
}

IFXRESULT CIFXAnimationModifier::CIFXMotionResourceManager::GetMotion(
					I32 motionId,IFXMotion **ppMotion,IFXString *pSourcename)
{
	if(pSourcename)
		*pSourcename="Palette";

	if(!m_pMotionResourcePalette)
		return IFX_E_NOT_INITIALIZED;

	IFXMotionResource* pMotionResource = NULL;
	IFXRESULT result=m_pMotionResourcePalette->GetResourcePtr(motionId,
							IID_IFXMotionResource,(void**)&pMotionResource );

	if(ppMotion)
		*ppMotion=(IFXSUCCESS(result)) ? pMotionResource->GetMotionRef() : NULL;

	if( NULL == *ppMotion ) // use default motion
	{
		result = m_pMotionResourcePalette->GetResourcePtr( 0, IID_IFXMotionResource, (void**)&pMotionResource );
		if(ppMotion)
			*ppMotion= (IFXSUCCESS(result)) ? pMotionResource->GetMotionRef() : NULL;
	}

	IFXRELEASE(pMotionResource);

	return result;
}


IFXRESULT CIFXAnimationModifier::CIFXMotionResourceManager::FindMotion(
										const IFXString& rMotionName, U32* pMotionId)
{
	IFXRESULT result=IFX_OK;

	IFXASSERT(m_pBonesModifier);
	m_pBonesModifier->GetBonesManagerNR();  //* flush through if necessary

	IFXASSERT(m_pMotionResourcePalette);

#if TRUE && defined(_DEBUG)
	U32 m,paletteSize;
	IFXString paletteEntryName;
	IFXString name;
	result=GetMotionPalette()->GetPaletteSize(&paletteSize);
	for(m=0;m<paletteSize;m++)
	{
		result=GetMotionPalette()->GetName(m,&paletteEntryName);
		name=paletteEntryName;
		IFXTRACE_GENERIC(L"FindMotion %d: \"%ls\"\n",m,name.Raw());
	}
#endif

	*pMotionId=0;
	result = GetMotionPalette()->Find(&rMotionName,pMotionId);
	if( result == IFX_E_CANNOT_FIND )
		result = IFX_OK;

	return result;
}

IFXRESULT CIFXAnimationModifier::CIFXMotionResourceManager::InitializeMotionMixer(
												IFXMotionMixer *pMotionMixer)
{
	IFXRESULT result = IFX_OK;

	if( !m_pBonesModifier->GetBonesManagerNR() )
	{
		IFXASSERT( m_pBonesModifier->IsKeyframe() == FALSE );
	}

	if(IFXSUCCESS(result))
		result = pMotionMixer->AssociateWithMotionManager(this);

	if(IFXSUCCESS(result))
		result = pMotionMixer->AssociateWithBonesManager(
									m_pBonesModifier->GetBonesManagerNR() );
	return result;
}



IFXRESULT CIFXAnimationModifier::CIFXMotionResourceManager::MapHierarchy(
				IFXBonesManager* pBonesManager,IFXMotionMixer* pMotionMixer,
				U32 motionId, const IFXString& rBoneName)
{
	IFXRESULT result = IFX_OK;

	IFXASSERT(pBonesManager);
	I32 numberBones=0;
	pBonesManager->GetInteger(IFXBonesManagerImpl::NumberOfBones,&numberBones);

	if(numberBones==1)
	{
		//* singletrack
		result=pMotionMixer->MapMotionTrackToBone(motionId,rBoneName,0);
	}
	else
	{
		I32 boneID=0;
		result=pBonesManager->GetBoneIndex(rBoneName, &boneID);
		if(IFXSUCCESS(result))
			result=pMotionMixer->SubMapMotionToCharacter(motionId,boneID);
		else if(!rBoneName.Length())
			result=pMotionMixer->MapFullMotionToCharacter(motionId);
	}

	return result;
}


IFXRESULT CIFXAnimationModifier::CIFXMotionResourceManager::PopulateMotionMixer(
							const IFXString& rMixerName,IFXMotionMixer *pMotionMixer)
{
	IFXRESULT result=IFX_OK;

	//* flush through if necessary
	IFXASSERT(m_pBonesModifier);
	IFXBonesManager *pBonesManager = m_pBonesModifier->GetBonesManagerNR();

	IFXASSERT(m_pMotionResourcePalette);
	IFXASSERT(m_pMixerResourcePalette);

	// NOTE pMotionMixer already allocated, just filling in data

	result = InitializeMotionMixer( pMotionMixer );

	// List all
#if TRUE && defined(_DEBUG)
	U32 m,n,paletteSize=0;
	IFXString paletteEntryName;
	IFXString name;
	if(IFXSUCCESS(result))
		result=GetMotionPalette()->GetPaletteSize(&paletteSize);
	for(m=0;m<paletteSize;m++)
	{
		result=GetMotionPalette()->GetName(m,&paletteEntryName);
		name=paletteEntryName;

		IFXDECLARELOCAL(IFXMotionResource, pResource);
		result=GetMotionPalette()->GetResourcePtr(m,
									IID_IFXMotionResource,(void **)&pResource);

		IFXTRACE_GENERIC(L"Motion %d: %p \"%ls\"\n",m,pResource,name.Raw());

		IFXMotion *pMotion;
		GetMotion(m,&pMotion,&name);
		U32 tracks;
		if(pMotion && (tracks=pMotion->GetNumberTracks()))
		{
			for(n=0;n<2 && n<tracks;n++)
			{
				IFXTRACE_GENERIC(L"Track %d:%d \"%ls\"\n",m,n,
										pMotion->GetTrack(n).GetName().Raw());
				IFXTRACE_GENERIC(L"\n");
			}
		}
	}

	if(IFXSUCCESS(result))
		result=GetMixerPalette()->GetPaletteSize(&paletteSize);
	for(m=0;m<paletteSize;m++)
	{
		result=GetMixerPalette()->GetName(m,&paletteEntryName);
		name=paletteEntryName;

		IFXDECLARELOCAL(IFXMixerConstruct, pMixCon);
		result=GetMixerPalette()->GetResourcePtr(m,
									IID_IFXMixerConstruct,(void **)&pMixCon);

		IFXTRACE_GENERIC(L"Mixer %d: %p \"%ls\"\n",m,pMixCon,name.Raw());
	}
#endif

	U32 mixConId = 0;

	if( IFXSUCCESS(result) )
		result=GetMixerPalette()->Find(&rMixerName,&mixConId);

	if( !IFXSUCCESS(result) )
		return IFX_E_CANNOT_FIND;

	IFXMixerConstruct *pMixCon = NULL;
	result=GetMixerPalette()->GetResourcePtr(mixConId,
									IID_IFXMixerConstruct,(void **)&pMixCon);

	if( IFX_E_PALETTE_NULL_RESOURCE_POINTER == result )
		result = IFX_E_NOT_INITIALIZED;

	// build mixer recursively
	if( IFXSUCCESS(result) )
	{
		if( pMixCon )
		{
			result=MapConstruct(pBonesManager,pMotionMixer,pMixCon,L"");
			IFXRELEASE(pMixCon);
		}
		else
		{
			//* NULL pMixCon ok -> go directly to motion palette

			U32 motionID = 0;
			result=FindMotion(rMixerName,&motionID);

			if(IFXSUCCESS(result))
				result=MapHierarchy(pBonesManager,pMotionMixer,motionID,"");

			if(!IFXSUCCESS(result))
				result=IFX_E_NOT_INITIALIZED;
		}
	}

	return result;
}


//* WARNING doesn't handle partial map of a construct that isn't a pure motion
IFXRESULT CIFXAnimationModifier::CIFXMotionResourceManager::MapConstruct(
		IFXBonesManager *pBonesManager,IFXMotionMixer *pMotionMixer,
		IFXMixerConstruct *pMixCon,const IFXString& rBoneName)
{
	// NOTE recursive
	IFXRESULT result=IFX_OK;

	IFXASSERT(pMixCon);
	const IFXList<IFXMapEntry> *pConList=pMixCon->GetEntryList();
	IFXMapEntry *pConMap;
	U32 motionId=0;

	// NOTE if leaf (no submappings) and no resource, default to motionid 0
	IFXMotionResource *motionResource=pMixCon->GetMotionResource();
	if(motionResource || !pConList->GetNumberElements())
	{
		if(pConList->GetNumberElements())
			IFXTRACE_GENERIC(L"Note: construct %p has resource AND %d submaps\n",
									pMixCon,pConList->GetNumberElements());
		if(!motionResource)
			IFXTRACE_GENERIC(L"Note: construct %p has NULL resource AND no submaps\n",
									pMixCon);

		//* find motion palette
		motionId=0;
		if(motionResource)
			result = GetMotionPalette()->Find(
					&motionResource->GetMotionRef()->GetNameConst(),&motionId);
			if( result == IFX_E_CANNOT_FIND )
				result = IFX_OK;

		IFXTRACE_GENERIC(L"find name=\"%ls\" resource=%p result==x%x id=%d\n",
				motionResource->GetMotionRef()->GetNameConst().Raw(),
				motionResource,result,motionId);

		IFXString cMotionName;
		result=GetMotionPalette()->GetName(motionId,&cMotionName);
		IFXString motionname = cMotionName;

		IFXTRACE_GENERIC(L"Use Motion \"%ls\" \"%ls\"\n",motionname.Raw(),rBoneName.Raw());

		result = GetMotionPalette()->Find(&motionname,&motionId);
		if( result == IFX_E_CANNOT_FIND )
			result = IFX_OK;

		if(IFXSUCCESS(result))
			result=MapHierarchy(pBonesManager,pMotionMixer,motionId,rBoneName);

		if(motionResource)
			IFXRELEASE(motionResource);
	}

	IFXCharacter *pCharacter=NULL;
	pBonesManager->GetCharacter(&pCharacter);
	IFXASSERT(pCharacter);
	IFXCoreNode *bonecore=pCharacter->FindDescendentByName(rBoneName);
	IFXBoneNode *bonenode= (bonecore && bonecore->IsBone())?
										(IFXBoneNode *)bonecore: NULL;

	IFXListContext context;
	pConList->ToHead(context);
	while( IFXSUCCESS(result) &&
							(pConMap=pConList->PostIncrement(context)) != NULL)
	{
		IFXMixerConstruct *pOtherCon=pConMap->m_pMixerConstruct;
		IFXString othername=pConMap->m_boneName;

		IFXTRACE_GENERIC(L"Construct %p \"%ls\"\n",pOtherCon,othername.Raw());

		if(!pOtherCon)
			continue;

		//* check if previous hier subsets this hier or is entirely disjoint
		if(bonenode)
		{
			IFXCoreNode *othercore=pCharacter->FindDescendentByName(othername);
			IFXBoneNode *othernode= (othercore && othercore->IsBone())?
											(IFXBoneNode *)othercore: NULL;

			if(!othernode || bonenode->HasAncestor(othernode))
				othername=rBoneName;
			else if(!othernode->HasAncestor(bonenode))
				continue;
		}

		result=MapConstruct(pBonesManager,pMotionMixer,pOtherCon,othername);
	}

	return result;
}


IFXRESULT CIFXAnimationModifier::SubmitEvent(
										bool startevent,
										const IFXString& rName,
										F32 localTime)
{
	IFXRESULT result=IFX_OK;

	IFXModifierChain *pModifierChain=NULL;
	IFXModifier *pModifier=NULL;
	IFXModel *pModel=NULL;

	if( IFXSUCCESS(result) )
		result = this->GetModifierChain(&pModifierChain);
	if( IFXSUCCESS(result) )
		result = pModifierChain->GetModifier(0,pModifier);
	if( IFXSUCCESS(result) )
		result = pModifier->QueryInterface(IID_IFXModel,(void**)&pModel);
	if(!IFXSUCCESS(result))
	{
		IFXRELEASE(pModifierChain);
		IFXRELEASE(pModifier);
		return result;
	}

	IFXUnknown *pAffectedObject=NULL;
	pModel->QueryInterface(IID_IFXUnknown,(void**)&pAffectedObject);

	IFXNotificationInfo *pEvent;
	IFXCreateComponent(CID_IFXNotificationInfo,IID_IFXNotificationInfo,
															(void**)&pEvent);

	IFXCoreServices *pCoreServices=NULL;

	IFXASSERT(m_pSceneGraph);
	m_pSceneGraph->GetCoreServices(&pCoreServices);
	IFXASSERT(pCoreServices);

	pEvent->Initialize(pCoreServices);

	if(startevent)
		pEvent->SetId(IFXNotificationId_AnimationStarted);
	else
		pEvent->SetId(IFXNotificationId_AnimationEnded);

	pEvent->SetType(IFXNotificationType_Animation);
	pEvent->SetTime(U32(localTime*1000));
	pEvent->SetName(rName);
	pEvent->SetObjectFilter(pAffectedObject);

	IFXNotificationManager *pNotificationManager=NULL;
	pCoreServices->GetNotificationManager(IID_IFXNotificationManager,
												(void**)&pNotificationManager);
	IFXASSERT(pNotificationManager);

#if TRUE
	IFXTRACE_GENERIC(L"SubmitEvent(%ls%ls,%.6G) at %.6G\n",
							startevent? L"startAnim,": L"endAnim,  ",
							rName.Raw(),localTime,GetWorldTime());
#endif

	// submitevent will keep a COPY of event,
	// and will addref anything it needs (if it is aware of them)
	result=pNotificationManager->SubmitEvent(pEvent);

	IFXRELEASE(pEvent);

	IFXRELEASE(pAffectedObject);

	IFXRELEASE(pNotificationManager);

	return result;
}
