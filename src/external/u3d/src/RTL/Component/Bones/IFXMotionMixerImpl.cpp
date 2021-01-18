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
	@file IFXMotionMixerImpl.cpp
*/

#include <string.h>
#include "IFXMotionMixerImpl.h"
#include "IFXMotionManager.h"
#include "IFXCharacter.h"
#include "IFXBoneNode.h"

#define IFXMOTIONMXR_CHECKBOUNDS    TRUE    // _DEBUG
#define IFXMOTIONMXR_CHECKNULL      TRUE
#define IFXMOTIONMXR_PELVISPATCH    FALSE   // disallow pelvis/spine motion

#define IFXMOTIONMXR_DEFMOTION      L"DefaultMotion"


IFXMotionMixerImpl::IFXMotionMixerImpl(void)
{
	m_uRefCount = 1;
	Reset();
}

IFXMotionMixerImpl::~IFXMotionMixerImpl(void)
{
}

U32 IFXMotionMixerImpl::AddRef( void )
{
	return m_uRefCount++;
}

U32 IFXMotionMixerImpl::Release( void )
{
	if ( 1 == m_uRefCount ) 
	{
		delete this;

		// This second return point is used so that the deleted object's
		// reference count isn't referenced after the memory is released.
		return 0;
	}
	
	return --m_uRefCount;
}


IFXRESULT IFXMotionMixerImpl::Reset(void)
	{
	m_motionmanager=NULL;
	m_timeOffset=0.0f;
	m_customlength=0;
	m_motionarray=NULL;
	m_character=NULL;
	m_readerarray.Clear();

	m_queueOffset=0.0f;
	m_localStartTime= 0.0f;
	m_localEndTime= -0.001f;
	m_timeScale=1.0f;
	m_loop=false;
	m_sync=false;
	m_running=false;
	m_delaymap=false;
	m_started=false;
	m_defaultOffset=false;
	m_primaryMotion="<no motion>";
	m_eventState=IFXSX_WaitStart;

	return IFX_OK;
	}


IFXRESULT IFXMotionMixerImpl::GetMotionReader(I32 boneid,
													IFXMotionReader **reader)
	{
	IFXASSERT(boneid>=0);

#if IFXMOTIONMXR_CHECKBOUNDS
	if(boneid<0)
		return IFX_E_INVALID_HANDLE;
#endif

	*reader=&m_readerarray[boneid];
	return IFX_OK;
	}


IFXRESULT IFXMotionMixerImpl::AssociateWithMotionManager(
												IFXMotionManager *set)
	{
#if IFXMOTIONMXR_CHECKNULL
	if(!set)
		return IFX_E_INVALID_POINTER;
#endif

	IFXASSERT(!m_motionarray);
	if(m_motionarray)
		return IFX_E_ALREADY_INITIALIZED;

	IFXASSERT(m_motionmanager==NULL || m_motionmanager==set);
	if(m_motionmanager && m_motionmanager!=set)
		return IFX_E_ALREADY_INITIALIZED;

	m_motionmanager=set;
	return IFX_OK;
	}


IFXRESULT IFXMotionMixerImpl::UseCustomMotionArray(I32 length,
														IFXMotion *motion)
	{
#if IFXMOTIONMXR_CHECKNULL
	if(!motion)
		return IFX_E_INVALID_POINTER;
#endif

	IFXASSERT(m_motionmanager==NULL);
	if(m_motionmanager)
		return IFX_E_ALREADY_INITIALIZED;

	m_customlength=length;
	m_motionarray=motion;
	return IFX_OK;
	}


IFXMotion *IFXMotionMixerImpl::LookupMotion(I32 motionid)
	{
#if IFXMOTIONMXR_CHECKBOUNDS
	if(motionid<0)
		return NULL;
#endif

	if(m_motionmanager)
		{
		IFXMotion *motion=NULL;
		IFXRESULT result=m_motionmanager->GetMotion(motionid,&motion,NULL);
		if(result!=IFX_OK)
			return NULL;
		return motion;
		}
	else if(m_motionarray)
		{
		if((U32)motionid>=m_customlength)
			return NULL;

		return &m_motionarray[motionid];
		}

	return NULL;
	}


IFXRESULT IFXMotionMixerImpl::AssociateWithBonesManager(
												IFXBonesManager *set)
	{
#if IFXMOTIONMXR_CHECKNULL
	if(!set)
		return IFX_E_INVALID_POINTER;
#endif

	IFXCharacter *character;
	set->GetCharacter(&character);
	return VerifyCharacter(character);
	}


IFXRESULT IFXMotionMixerImpl::VerifyCharacter(IFXVariant vset)
	{
	IFXCharacter *set=NULL;
#ifdef _DEBUG
	IFXRESULT result=
#endif
	vset.CopyPointerTo(&set);
	IFXASSERT(result == IFX_OK);

	IFXASSERT(m_character==NULL || m_character==set);
	if(m_character && m_character!=set)
		return IFX_E_ALREADY_INITIALIZED;

	m_character=set;
	return IFX_OK;
	}


IFXRESULT IFXMotionMixerImpl::MapFullMotionToCharacter(I32 motionid)
	{
	IFXASSERT(m_character);

#if IFXMOTIONMXR_CHECKNULL
	if(!m_character)
		return IFX_E_NOT_INITIALIZED;
#endif

#if IFXMOTIONMXR_CHECKBOUNDS
	if(motionid<0)
		return IFX_E_INVALID_RANGE;
#endif

	IFXMotion *motion=LookupMotion(motionid);
	if(!motion)
		return IFX_E_NOT_INITIALIZED;

	//* total default motion
	if( motion->GetNameConst() == IFXMOTIONMXR_DEFMOTION )
		{
		I32 m,number=m_character->GetBoneTableSize();
		ResizeReaderArray(number);
		for(m=0;m<number;m++)
			{
			m_readerarray[m].Reset();
			m_readerarray[m].SetReference(true);
			}
		return IFX_OK;
		}

	//* normal mapping
	I32 m,number=motion->GetTracks().GetNumberElements();
	for(m=0;m<number;m++)
		{
		const IFXKeyTrack &keytrack=motion->GetTrackConst(m);
	/*  const IFXCHAR *trackname8=keytrack.GetNameConst().Raw();

#if IFXMOTIONMXR_PELVISPATCH
		/// @todo investigate disallowing movement of Pelvis/Spine because of exporter bug
		if(!trackname8[6].Compare(&trackname8[6],L"Pelvis") ||
										!&trackname8[6].Compare(L"Spine"))
			continue;
#endif
	*/
		IFXCoreNode *node=m_character->FindDescendentByName(
													keytrack.GetNameConst());
		if(!node || !node->IsBone())
			continue;

		MapMotionTrackToBone(motionid,keytrack.GetNameConst(),(
										(IFXBoneNode *)node)->GetBoneIndex());
		}

	return IFX_OK;
	}


struct IFXMMI_MapState
	{
	IFXMotionMixerImpl  *mixer;
	I32                 motionid;
	IFXRESULT           iResult;
	};


IFXRESULT IFXMotionMixerImpl::SubMapMotionToCharacter(I32 motionid,
															I32 fromboneid)
	{
	IFXASSERT(m_character);

#if IFXMOTIONMXR_CHECKNULL
	if(!m_character)
		return IFX_E_NOT_INITIALIZED;
#endif

	IFXCoreNode *fromnode=m_character;
	if(fromboneid>=0)
		fromnode=m_character->LookupBoneIndex(fromboneid);

#if IFXMOTIONMXR_CHECKNULL
	if(!fromnode)
		return IFX_E_INVALID_HANDLE;
#endif

	struct IFXMMI_MapState mapstate;
	mapstate.mixer=this;
	mapstate.motionid=motionid;
	mapstate.iResult=IFX_OK;

	IFXMotionMixerImpl::MapBone(*fromnode,&mapstate);   // base, too
	m_character->ForEachNode2(IFXPARENTFIRST,*fromnode,
									&IFXMotionMixerImpl::MapBone,&mapstate);

	return mapstate.iResult;
	}


BOOL IFXMotionMixerImpl::MapBone(IFXCoreNode &node,IFXVariant state)
	{
	if(!node.IsBone())
		return false;

	struct IFXMMI_MapState *mapstate=NULL;
	IFXRESULT result=state.CopyPointerTo(&mapstate);
	IFXASSERT(result==IFX_OK);
	IFXASSERT(mapstate);

	if(!IFXSUCCESS(mapstate->iResult))
		return false;

	IFXBoneNode *bonenode=(IFXBoneNode *)&node;

	result=mapstate->mixer->MapMotionTrackToBone(mapstate->motionid,
									bonenode->Name(),bonenode->GetBoneIndex());

	if(!IFXSUCCESS(result))
		mapstate->iResult=result;
	return false;
	}


IFXRESULT IFXMotionMixerImpl::MapMotionTrackToBone(I32 motionid,
											const IFXString& rTrackName,I32 boneid)
	{
	IFXASSERT(m_character);

#if IFXMOTIONMXR_CHECKNULL
	if(!m_character)
		return IFX_E_NOT_INITIALIZED;
#endif

#if IFXMOTIONMXR_CHECKBOUNDS
	if(motionid<0)
		return IFX_E_INVALID_RANGE;

	// NOTE not currently supporting motion on root m_character node
	if(boneid<0)
		return IFX_E_INVALID_HANDLE;
#endif

	IFXMotion *motion=LookupMotion(motionid);
	if(!motion)
		return IFX_E_NOT_INITIALIZED;

	//* set reader data in table
	ResizeReaderArray(boneid+1);
	m_readerarray[boneid].Reset();

	bool ok=false;
	if( motion && motion->GetNameConst() == IFXMOTIONMXR_DEFMOTION )
		{
		m_readerarray[boneid].SetReference(true);
		ok=true;
		}
	else if(rTrackName==L"")
		ok=m_readerarray[boneid].AssignByTrackID(motion,0);
	else
		ok=m_readerarray[boneid].AssignByTrackName(motion,rTrackName);

	return (ok)? IFX_OK: IFX_E_CANNOT_FIND;
	}


//* only expands, does not shrink
void IFXMotionMixerImpl::ResizeReaderArray(I32 size)
	{
	I32 m,elements=m_readerarray.GetNumberElements();
	if(size<=elements)
		return;

	m_readerarray.ResizeToAtLeast(size);
	for(m=elements;m<size;m++)
		m_readerarray[m].Reset();
	}


IFXRESULT IFXMotionMixerImpl::GetMotionTimeLimits(F32 *min,F32 *max)
{
	IFXASSERT(min);
	IFXASSERT(max);

#if IFXMOTIONMXR_CHECKNULL
	if(!min || !max)
		return IFX_E_INVALID_POINTER;
#endif

	F32 limits[2];
	bool found=false;

	*min=  1e30f;
	*max= -1e30f;

	I32 m,readers=m_readerarray.GetNumberElements();
	for(m=0;m<readers;m++)
	{
		IFXMotionReader &reader=m_readerarray[m];
		if(!reader.IsValid())
			continue;

		found=true;
		limits[0]=0.0f;
		limits[1]=0.0f;

		reader.GetTimeLimits(&limits[0],&limits[1]);

		if(*min>limits[0])
			*min=limits[0];
		if(*max<limits[1])
			*max=limits[1];

	}

	if(!found)
	{
		*min=0.0f;
		*max=0.0f;
	}

	return IFX_OK;
}


//* location, rotation, and/or scale may be NULL
IFXRESULT IFXMotionMixerImpl::SetPositionExplicitly(I32 boneid,
												const IFXVector3 *location,
												const IFXQuaternion *rotation,
												const IFXVector3 *scale)
	{
	IFXASSERT(m_character);

#if IFXMOTIONMXR_CHECKNULL
	if(!m_character)
		return IFX_E_NOT_INITIALIZED;
#endif

	IFXASSERT(boneid>=0);

#if IFXMOTIONMXR_CHECKBOUNDS
	if(boneid<0)
		return IFX_E_INVALID_HANDLE;
#endif

	ResizeReaderArray(boneid+1);

	bool ik=m_readerarray[boneid].GetIK();
	m_readerarray[boneid].Reset();
	m_readerarray[boneid].SetIK(ik);

	IFXBoneNode *bonenode=m_character->LookupBoneIndex(boneid);
	if(!bonenode)
		return IFX_E_INVALID_HANDLE;

	if(location)
		{
		if(ik)
			bonenode->Effector()= *location;
		else
			bonenode->Displacement()= *location;
		}

	if(rotation)
		bonenode->Rotation()= *rotation;

	if(scale)
		bonenode->Scale()= *scale;

	return IFX_OK;
	}


//* location, rotation, and/or scale may be NULL
IFXRESULT IFXMotionMixerImpl::GetBonePosition(I32 boneid,
												IFXVector3 *location,
												IFXQuaternion *rotation,
												IFXVector3 *scale)
{
	IFXASSERT(m_character);

	#if IFXMOTIONMXR_CHECKNULL
		if(!m_character)
			return IFX_E_NOT_INITIALIZED;
	#endif

	IFXCoreNode *node=m_character;
	if(boneid>=0)
	{
		IFXBoneNode *bonenode=m_character->LookupBoneIndex(boneid);
		if(!bonenode)
			return IFX_E_INVALID_HANDLE;

		node=bonenode;
	}

	if(location)
		*location=node->DisplacementConst();
	if(rotation)
		*rotation=node->RotationConst();
	if(scale)
		*scale=node->ScaleConst();

	return IFX_OK;
}


IFXRESULT IFXMotionMixerImpl::GetAbsoluteBoneMatrix(I32 boneid,
														IFXMatrix4x4 *pMatrix)
	{
	IFXASSERT(m_character);

#if IFXMOTIONMXR_CHECKNULL
	if(!m_character)
		return IFX_E_NOT_INITIALIZED;
	if(!pMatrix)
		return IFX_E_INVALID_POINTER;
#endif

	IFXCoreNode *node=m_character;
	if(boneid>=0)
		{
		IFXBoneNode *bonenode=m_character->LookupBoneIndex(boneid);
		if(!bonenode)
			return IFX_E_INVALID_HANDLE;

		node=bonenode;
		}

	*pMatrix=node->StoredTransform().MatrixConst();

	return IFX_OK;
	}


IFXRESULT IFXMotionMixerImpl::CalcBonePositionAtTime(
	I32 boneid,
	F32 time,
	IFXVector3 *location,
	IFXQuaternion *rotation,
	IFXVector3 *scale)
{
	IFXASSERT(m_character);

	#if IFXMOTIONMXR_CHECKNULL
		if(!m_character)
			return IFX_E_NOT_INITIALIZED;
	#endif

	//* adjust for local offset
	time -= m_timeOffset;

	// IFXCoreNode *node=m_character;
	IFXBoneNode *bonenode=NULL;
	if(boneid>=0)
	{
		bonenode=m_character->LookupBoneIndex(boneid);
		if(!bonenode)
			return IFX_E_INVALID_RANGE;

		// node=bonenode;
	}

	IFXMotionReader *reader=NULL;

	if((U32)boneid>=m_readerarray.GetNumberElements() ||
		!(reader=&m_readerarray[boneid])->IsValid())
	{
		//* if not mapped, return reference
		if(location)
			*location=bonenode->GetReferenceDisplacement();
		if(rotation)
			*rotation=bonenode->LocalReferenceRotationConst();
		if(scale)
			scale->Set(1.0f,1.0f,1.0f);
		return IFX_OK;
	}

	IFXInstant instant;
	reader->Read(time,&instant);

	// Same code in ApplyMotion
	if(location)
	{
		IFXVector3 tempv;
		*location=tempv.Add(instant.LocationConst(),bonenode->GetReferenceDisplacement()); //!!!
	}
	if(rotation)
	{
		IFXQuaternion tempq;

		*rotation=tempq.Multiply(instant.RotationConst(), bonenode->LocalReferenceRotationConst());
	}
	if(scale)
		*scale=instant.ScaleConst();

	return IFX_OK;
}


IFXRESULT IFXMotionMixerImpl::ActivateIK(I32 boneid,bool on)
	{
	IFXASSERT(boneid>=0);

#if IFXMOTIONMXR_CHECKBOUNDS
	if(boneid<0)
		return IFX_E_INVALID_HANDLE;
#endif

	ResizeReaderArray(boneid+1);
	m_readerarray[boneid].SetIK(on);

	return IFX_OK;
	}


IFXRESULT IFXMotionMixerImpl::ApplyMotion(F32 time,bool toblend)
{
	IFXASSERT(m_character);

#if IFXMOTIONMXR_CHECKNULL
	if(!m_character)
		return IFX_E_NOT_INITIALIZED;
#endif

#if FALSE
	IFXTRACE_GENERIC("%p ApplyMotion(%.6G,%d) offset %.6G as %.6G\n",
				this,time,(I32)toblend,m_timeOffset,time-m_timeOffset);
#endif

	//* adjust for local offset
	time-=m_timeOffset;

	I32 m,bones=m_character->GetBoneTableSize();
	I32 readers=m_readerarray.GetNumberElements();
	for(m=0;m<bones;m++)
	{
		IFXBoneNode *bonenode=m_character->LookupBoneIndex(m);
		if(!bonenode)
			continue;
		if(m<readers && m_readerarray[m].GetIK() )
			bonenode->SetEffected(true);
		else
			bonenode->SetEffected(false);
	}

	for(m=0;m<readers;m++)
	{
		IFXBoneNode *bonenode=m_character->LookupBoneIndex(m);
		if(bonenode==NULL)
			continue;

		IFXMotionReader &reader=m_readerarray[m];
		if(reader.GetReference())
		{
			if(toblend)
			{
				bonenode->BlendDisplacement()=bonenode->ReferenceDisplacement();
				bonenode->BlendRotation()=bonenode->LocalReferenceRotation();
				bonenode->BlendScale().Set(1.0f,1.0f,1.0f);
			}
			else
			{
				bonenode->Displacement()=bonenode->ReferenceDisplacement();
				bonenode->Rotation()=bonenode->LocalReferenceRotation();
				bonenode->Scale().Set(1.0f,1.0f,1.0f);
			}
		}
		if(!reader.IsValid())
			continue;

		IFXInstant instant;
		reader.Read(time,&instant);

		if(reader.GetIK())
		{
			bonenode->Effector()=instant.LocationConst();
		}
		else if(toblend)
		{
			// put into blend buffers
			bonenode->BlendDisplacement()=instant.LocationConst();
			bonenode->BlendRotation()=instant.RotationConst();
			bonenode->BlendScale()=instant.ScaleConst();
		}
		else
		{
			IFXVector3 tempv;
			IFXQuaternion tempq;

			// explicit node rotation
			bonenode->Displacement()=tempv.Add(instant.LocationConst(),bonenode->GetReferenceDisplacement()); //!!!!
			bonenode->Rotation()=tempq.Multiply(instant.RotationConst(), bonenode->LocalReferenceRotationConst());
			bonenode->Scale()=instant.ScaleConst();
		}

#if FALSE
		// what was this for?
		bonenode->GetStoredTransform().MakeIdentity();
		bonenode->GetStoredTransform().SetTranslation(
			instant.LocationConst());
#endif
	}

	return IFX_OK;
}


//  NOTE should the real code be imported in?
IFXRESULT IFXMotionMixerImpl::ApplyIK(void)
	{
	m_character->ApplyIK();

	return IFX_OK;
	}


IFXRESULT IFXMotionMixerImpl::GetRelativeTransform(U32 boneID,F32 *pMatrix16)
	{
	IFXVector3 location;
	IFXQuaternion rotation;
	IFXVector3 scale;

	IFXRESULT iResult=GetBonePosition(boneID,&location,&rotation,&scale);
	if(!IFXSUCCESS(iResult))
		IFXRETURN(iResult);

	IFXMatrix4x4 matrix=rotation;
	matrix.SetTranslation(location);
	matrix.Scale3x4(scale);

	I32 m;
	for(m=0;m<16;m++)
		pMatrix16[m]=matrix[m];

	return IFX_OK;
	}



IFXRESULT IFXMotionMixerImpl::SetRelativeTransform(U32 boneID,F32 *pMatrix16)
	{
	IFXMatrix4x4 matrix(pMatrix16);

	IFXVector3 location;
#if FALSE
	IFXQuaternion rotation=matrix;

	matrix.CopyTranslation(location);
#else
	IFXQuaternion rotation=matrix;
	IFXVector3 scale;

	matrix.CalcTRS(&location,&rotation,&scale);
#endif

	return SetPositionExplicitly(boneID,&location,&rotation,&scale);
	}

