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
	@file CIFXSkeleton.cpp
*/

#include "IFXCoreCIDs.h"
#include "IFXExportingCIDs.h"
#include "CIFXSkeleton.h"
#include "IFXBonesManagerImpl.h"

IFXRESULT IFXAPI_CALLTYPE CIFXSkeleton_Factory( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT rc = IFX_OK;
	if (ppInterface)
	{
		CIFXSkeleton* pPtr = new CIFXSkeleton;
		if (pPtr)
		{
			rc = pPtr->Construct();
			if (IFXFAILURE(rc))
				IFXDELETE(pPtr);
		}
		else
		{
			rc = IFX_E_OUT_OF_MEMORY;
		}
		if (IFXSUCCESS(rc))
		{
			pPtr->AddRef();
			rc = pPtr->QueryInterface(interfaceId, ppInterface);
			pPtr->Release();
		}
	}
	else
	{
		rc = IFX_E_INVALID_POINTER;
	}
	IFXRETURN(rc);
}

CIFXSkeleton::CIFXSkeleton()
{
	m_uRefCount = 0;

	m_uBlockTypeBase = 0;
}

CIFXSkeleton::~CIFXSkeleton()
{
	if(m_pBonesMgr)
	{
		delete (IFXBonesManagerImpl *)m_pBonesMgr;
		m_pBonesMgr = NULL;
	}
}

IFXRESULT CIFXSkeleton::Construct()
{
	IFXRESULT rc = IFX_OK;

	m_uBonesManagerIndex = 0;
	m_pBonesMgr = 0;

	m_pBonesMgr = new IFXBonesManagerImpl(); // was: IFXCreateBonesManager();

	if(0 == m_pBonesMgr)
	{
		rc = IFX_E_OUT_OF_MEMORY;
	}
	else
	{
		m_pBonesMgr->ResetUnshared();
	}

	if(IFXSUCCESS(rc))
	{
		IFXCharacter* pCharacter = 0;
		rc = m_pBonesMgr->GetCharacter(&pCharacter);
	}

	IFXRETURN(rc);
}


// IFXUnknown
U32 CIFXSkeleton::AddRef()
{
	return ++m_uRefCount;
}

U32 CIFXSkeleton::Release()
{
	if( 1 == m_uRefCount)
	{
		delete this;
		return 0;
	}

	return --m_uRefCount;
}

IFXRESULT CIFXSkeleton::QueryInterface(IFXREFIID riid, void** ppv)
{
	IFXRESULT rc = IFX_OK;

	if(ppv)
	{
		*ppv = 0;

		if(riid == IID_IFXUnknown)
			*ppv = (IFXUnknown*)this;
		else if(riid == IID_IFXSkeleton)
			*ppv = (IFXSkeleton*)this;

		if(*ppv)
			AddRef();
		else
			rc = IFX_E_UNSUPPORTED;
	}
	else
		rc = IFX_E_INVALID_POINTER;

	IFXRETURN(rc);
}

// IFXSkeleton interface...
IFXRESULT CIFXSkeleton::SetBoneInfo( U32 uBoneID, IFXBoneInfo *pBoneInfo )
{
	IFXRESULT rc = IFX_OK;
	IFXCharacter* pCharacter = 0;
	IFXBoneNode *pBoneNode=0;

	if(0 == pBoneInfo)
	{
		rc = IFX_E_INVALID_POINTER;
	}

	if(IFXSUCCESS(rc))
		rc = m_pBonesMgr->GetCharacter(&pCharacter);

	if(IFXSUCCESS(rc))
	{
		if(uBoneID < (U32)pCharacter->GetBoneTableSize() )
		{
			pBoneNode = pCharacter->LookupBoneIndex(uBoneID);
		}
		
		if(!pBoneNode)
		{
			// Create bone if needed
			pBoneNode = new IFXBoneNode(pCharacter);
			pCharacter->SetBoneAtIndex(uBoneID, pBoneNode);
			
			// attach 
			if(pBoneInfo->iParentBoneID < 0)
			{
				pCharacter->AppendChild(pBoneNode);
				pBoneNode->SetParent(pCharacter);
			}
			else
			{
				IFXBoneNode *parent = pCharacter
					->LookupBoneIndex(pBoneInfo->iParentBoneID);
				if(!parent)
					rc = IFX_E_ABORTED;
				else
				{
					parent->AppendChild(pBoneNode);
					pBoneNode->SetParent(parent);
				}
			}
		}
	}

	// set bones from struct
	if(IFXSUCCESS(rc))
	{
		pBoneNode->SetName(pBoneInfo->stringBoneName);

		pBoneNode->SetBoneIndex(uBoneID);
		pBoneNode->SetLength(pBoneInfo->fBoneLength);
		pBoneNode->Displacement()=pBoneInfo->v3BoneDisplacement;
		pBoneNode->Rotation()=pBoneInfo->v4BoneRotation;
		pBoneNode->ReferenceDisplacement()=pBoneInfo->v3BoneDisplacement;
		pBoneNode->LocalReferenceRotation()=pBoneInfo->v4BoneRotation;

		IFXConstraints &rot=pBoneNode->RotationalConstraints();

		rot.Min().Raw()[0]=pBoneInfo->fRotationConstraintXMin;
		rot.Min().Raw()[1]=pBoneInfo->fRotationConstraintYMin;
		rot.Min().Raw()[2]=pBoneInfo->fRotationConstraintZMin;

		rot.Max().Raw()[0]=pBoneInfo->fRotationConstraintXMax;
		rot.Max().Raw()[1]=pBoneInfo->fRotationConstraintYMax;
		rot.Max().Raw()[2]=pBoneInfo->fRotationConstraintZMax;

		rot.Active()[0]= (0 !=
			(pBoneInfo->uBoneAttributes & IFXSKELETON_ROTATIONCONSTRAINTACTIVE_X) );
		rot.Active()[1]= (0 !=
			(pBoneInfo->uBoneAttributes & IFXSKELETON_ROTATIONCONSTRAINTACTIVE_Y) );
		rot.Active()[2]= (0 !=
			(pBoneInfo->uBoneAttributes & IFXSKELETON_ROTATIONCONSTRAINTACTIVE_Z) );

		rot.Limited()[0]= (0 !=
			(pBoneInfo->uBoneAttributes & IFXSKELETON_ROTATIONCONSTRAINTLIMITED_X));
		rot.Limited()[1]= (0 !=
			(pBoneInfo->uBoneAttributes & IFXSKELETON_ROTATIONCONSTRAINTLIMITED_Y));
		rot.Limited()[2]= (0 !=
			(pBoneInfo->uBoneAttributes & IFXSKELETON_ROTATIONCONSTRAINTLIMITED_Z));

		IFXCylinder &bounds=pBoneNode->CylindricalBounds();

		bounds.GetCenter(0)=pBoneInfo->v2StartJointCenter;
		bounds.GetScale(0)=pBoneInfo->v2StartJointScale;
		bounds.GetCenter(1)=pBoneInfo->v2EndJointCenter;
		bounds.GetScale(1)=pBoneInfo->v2EndJointScale;

		pBoneNode->AutoLink()= (0 ==
				(pBoneInfo->uBoneAttributes & IFXSKELETON_LINKPRESENT) );
		pBoneNode->AutoJoint()= (0 ==
				(pBoneInfo->uBoneAttributes & IFXSKELETON_JOINTPRESENT) );
	}
	
	IFXRETURN(rc);
}

IFXRESULT CIFXSkeleton::GetBoneInfo( U32 uBoneID, IFXBoneInfo *pBoneInfo )
{
	IFXRESULT rc = IFX_OK;

	IFXCharacter* pCharacter = 0;
	IFXBoneNode* pBoneNode = 0;
	
	if(pBoneInfo == 0)
	{
		rc = IFX_E_INVALID_POINTER;
	}

	if(IFXSUCCESS(rc))
	{
		rc = m_pBonesMgr->GetCharacter(&pCharacter);
	}

	if(IFXSUCCESS(rc))
	{
		pBoneNode = pCharacter->LookupBoneIndex(uBoneID);
		if(!pBoneNode)
		{
			rc = IFX_E_INVALID_RANGE;
		}
	}

	if(IFXSUCCESS(rc))
	{
		const IFXCoreNode *pNode = pBoneNode->ParentConst();
		if(pNode->IsBone())
			pBoneInfo->iParentBoneID = ((IFXBoneNode *)pNode)->BoneIndex();
		else
			pBoneInfo->iParentBoneID = -1;

		pBoneInfo->stringBoneName = pBoneNode->NameConst();

		if( m_pBonesMgr && pBoneInfo->iParentBoneID != -1 )
		{
			m_pBonesMgr->GetBoneName( pBoneInfo->iParentBoneID, &pBoneInfo->stringParentName );
		}

		pBoneInfo->fBoneLength=pBoneNode->GetLength();
		pBoneInfo->v3BoneDisplacement=pBoneNode->ReferenceDisplacement();
		pBoneInfo->v4BoneRotation=pBoneNode->LocalReferenceRotation().RawConst();

		const IFXConstraints &rot=pBoneNode->RotationalConstraintsConst();

		pBoneInfo->fRotationConstraintXMin=rot.MinConst().RawConst()[0];
		pBoneInfo->fRotationConstraintYMin=rot.MinConst().RawConst()[1];
		pBoneInfo->fRotationConstraintZMin=rot.MinConst().RawConst()[2];

		pBoneInfo->fRotationConstraintXMax=rot.MaxConst().RawConst()[0];
		pBoneInfo->fRotationConstraintYMax=rot.MaxConst().RawConst()[1];
		pBoneInfo->fRotationConstraintZMax=rot.MaxConst().RawConst()[2];

		pBoneInfo->uBoneAttributes=0;

		if(rot.ActiveConst()[0])
			pBoneInfo->uBoneAttributes |= IFXSKELETON_ROTATIONCONSTRAINTACTIVE_X;
		if(rot.ActiveConst()[1])
			pBoneInfo->uBoneAttributes |= IFXSKELETON_ROTATIONCONSTRAINTACTIVE_Y;
		if(rot.ActiveConst()[2])
			pBoneInfo->uBoneAttributes |= IFXSKELETON_ROTATIONCONSTRAINTACTIVE_Z;

		if(rot.LimitedConst()[0])
			pBoneInfo->uBoneAttributes |= IFXSKELETON_ROTATIONCONSTRAINTLIMITED_X;
		if(rot.LimitedConst()[1])
			pBoneInfo->uBoneAttributes |= IFXSKELETON_ROTATIONCONSTRAINTLIMITED_Y;
		if(rot.LimitedConst()[2])
			pBoneInfo->uBoneAttributes |= IFXSKELETON_ROTATIONCONSTRAINTLIMITED_Z;

		IFXCylinder &bounds=pBoneNode->CylindricalBounds();

		pBoneInfo->v2StartJointCenter=bounds.GetCenter(0).RawConst();
		pBoneInfo->v2StartJointScale=bounds.GetScale(0).RawConst();
		pBoneInfo->v2EndJointCenter=bounds.GetCenter(1).RawConst();
		pBoneInfo->v2EndJointScale=bounds.GetScale(1).RawConst();

		if(!pBoneNode->AutoLink())
			pBoneInfo->uBoneAttributes |= IFXSKELETON_LINKPRESENT;

		if(!pBoneNode->AutoJoint())
			pBoneInfo->uBoneAttributes |= IFXSKELETON_JOINTPRESENT;
	}

	IFXRETURN(rc);
}

IFXRESULT CIFXSkeleton::GetNumBones( U32& uNumBones )
{
	IFXRESULT rc = IFX_OK;

	rc = m_pBonesMgr->GetInteger(IFXBonesManagerImpl::NumberOfBones, (I32*)&uNumBones );

	IFXRETURN(rc);
}


IFXBonesManager *CIFXSkeleton::GetBonesManagerNR(void)
{
	return m_pBonesMgr;
}


IFXRESULT CIFXSkeleton::SetBlockTypeBase( U32 uBlockType )
{
	m_uBlockTypeBase = uBlockType;
	return IFX_OK;
}


IFXRESULT CIFXSkeleton::GetBlockTypeBase( U32* pBlockType )
{
	*pBlockType = m_uBlockTypeBase;
	return IFX_OK;
}
