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
	@file IFXBonesManagerImpl.cpp 
*/

#include "IFXBonesManagerImpl.h"
#include "IFXMotionMixerImpl.h"
#include "IFXBoneNodeList.h"

#define IFXBONESMGR_CHECKBOUNDS   _DEBUG
#define IFXBONESMGR_CHECKNULL   TRUE ///< @note FALSE will break streaming!!!

IFXBonesManagerImpl::IFXBonesManagerImpl(IFXBonesManager *pCloneOf)
{
	IFXMeshGroup_Character *pMGChar=NULL;
	if(pCloneOf)
	{
		IFXCharacter *pCharacter=NULL;
		#ifdef _DEBUG        
			IFXRESULT result = 
		#endif      
				pCloneOf->GetCharacter(&pCharacter);
		IFXASSERT(result==IFX_OK);
		IFXASSERT(pCharacter);

		pMGChar=(IFXMeshGroup_Character *)pCharacter;
	}

	m_pLocalCharacter=NULL;
	ReplaceCharacter(new IFXMeshGroup_Character(pMGChar));
	m_pLocalCharacter=m_pCharacter;

	CreateCircle();
	m_pMeshFactory=NULL;
	m_getTimerCB=NULL;
	m_rootTransform.Reset();
	Reset();
}


IFXBonesManagerImpl::~IFXBonesManagerImpl(void)
{
	IFXDELETE(m_pLocalCharacter);
}

IFXRESULT IFXBonesManagerImpl::UpdateReferencePositions()
{
	IFXRESULT res = IFX_OK;

	if(NULL == m_pCharacter)
		res = IFX_E_UNDEFINED;
	else
		m_pCharacter->StoreReferencePositions();

	IFXRETURN(res);
}

IFXMotionMixer* IFXBonesManagerImpl::MotionMixer(void)
{
	// check for corruption
	IFXASSERT(!m_pMotionMixer ||
		0==m_pMotionMixer->Loop() || 1==m_pMotionMixer->Loop());
	
	return m_pMotionMixer;
}


IFXMotionMixer* IFXBonesManagerImpl::LastMixer(void)
{
	// check for corruption
	IFXASSERT(!m_pLastMixer ||
		m_pLastMixer->Loop()==0 || m_pLastMixer->Loop()==1);
	
	return m_pLastMixer;
}


void IFXBonesManagerImpl::ReplaceCharacter(IFXMeshGroup_Character *pSet)
{
	IFXDELETE(m_pLocalCharacter);

	IFXASSERT(pSet);
	m_pCharacter=pSet;

	// cast off const
	m_pCharacter->SetBonesManager((IFXBonesManager *)this);
	m_pCharacter->SetTimeProgressCB(IFXBonesManagerImpl::TimeProgress);
}

IFXRESULT IFXBonesManagerImpl::ResetUnshared(void)
{
	m_auto_translate.Reset();
	m_last_translate.Reset();
	m_auto_rotate.MakeIdentity();
	m_last_rotate.MakeIdentity();
	m_auto_scale.Set(1.0f,1.0f,1.0f);
	m_last_scale.Set(1.0f,1.0f,1.0f);

	m_pMotionMixer=NULL;
	m_pLastMixer=NULL;

	IFXRETURN(IFX_OK); ;
}


IFXRESULT IFXBonesManagerImpl::Reset(void)
{
	ResetUnshared();

	m_joints_calced = false;

	I32 m;
	for(m=0;m<IFXBonesManagerImpl::MaxBoolean;m++)
		m_bools[m]=false;
	for(m=0;m<IFXBonesManagerImpl::MaxInteger;m++)
		m_integers[m]=0;
	for(m=0;m<IFXBonesManagerImpl::MaxFloat;m++)
		m_floats[m]=0.0f;
	for(m=0;m<IFXBonesManagerImpl::NumTimedItems;m++)
	{
		m_frames=0;
		m_lasttimer[m]=0.0f;
		m_sumtimer[m]=0.0f;
		m_lastperiod[m]=0.0f;
	}

	m_floats[IFXBonesManagerImpl::BoneWidthScale]=1.0f/8.0f;
	m_floats[IFXBonesManagerImpl::BoneMaxWidth]=2.0f;
	m_floats[IFXBonesManagerImpl::AxisLength]=1.0f;
	m_floats[IFXBonesManagerImpl::WorldAxisLength]=36.0f;
	m_floats[IFXBonesManagerImpl::GridSize]=12.0f;
	m_bools[IFXBonesManagerImpl::AutoBlend]=true;
	m_bools[IFXBonesManagerImpl::LimitAngVelocity]=false;
	m_floats[IFXBonesManagerImpl::MaxAngVelocity]=1.0f;
	m_bools[IFXBonesManagerImpl::IKIncremental]=true;
	m_bools[IFXBonesManagerImpl::CLODLimited]=true;
	m_integers[IFXBonesManagerImpl::IKIterations]=1;

	m_unlinked_weights.Clear();

	IFXRETURN(IFX_OK);
}

IFXRESULT IFXBonesManagerImpl::GetBoneIndex(IFXString boneName, I32 *pBoneId)
{
	IFXRESULT res = IFX_OK;
	
	if (pBoneId)
	{
		*pBoneId=-1;

		IFXCoreNode *pNode = m_pCharacter->FindDescendentByName(boneName);

		if(pNode && pNode->IsBone())
			*pBoneId=((IFXBoneNode *)pNode)->GetBoneIndex();
		else
			res = IFX_E_CANNOT_FIND;    
	}
	else
		res = IFX_E_INVALID_POINTER;

	IFXRETURN(res);
}


IFXRESULT IFXBonesManagerImpl::GetBoneName(I32 boneId, IFXString *pBoneName)
{
	IFXRESULT res = IFX_OK;

	if(!pBoneName)
		res = IFX_E_INVALID_POINTER;
	else
	{
		IFXBoneNode *pBoneNode = GetBoneByIndex(boneId);

		if(!pBoneNode)
			res = IFX_E_INVALID_HANDLE;
		else
			*pBoneName=pBoneNode->NameConst();
	}

	IFXRETURN(res);
}


IFXRESULT IFXBonesManagerImpl::SetBool(BooleanProperty prop,bool set)
{
	IFXRESULT res = IFX_OK;
	
	#if IFXBONESMGR_CHECKBOUNDS
		if(prop<0 || prop>=IFXBonesManagerImpl::MaxBoolean)
			res = IFX_E_INVALID_RANGE;
	#endif

	if(IFXBonesManagerImpl::TimerUpdate == prop)
		res = IFX_E_CANNOT_CHANGE;

	if(IFXSUCCESS(res))
	{
		m_bools[prop]=set;

		if(!m_joints_calced && set && (prop==IFXBonesManagerImpl::ShowJoints ||
			prop==IFXBonesManagerImpl::ShowCylinders) )
		{
			CalcJoints();
			m_joints_calced=true;
		}

		if(prop>=IFXBonesManagerImpl::AutoJoint &&
			m_integers[IFXBonesManagerImpl::SelectBone]>=0)
		{
			IFXBoneNode *pSelectBone=GetBoneByIndex(
				m_integers[IFXBonesManagerImpl::SelectBone]);

			if(pSelectBone)
				switch(prop)
			{
				case IFXBonesManagerImpl::AutoLink:
					pSelectBone->AutoLink()=m_bools[prop];
					break;
				case IFXBonesManagerImpl::AutoJoint:
					pSelectBone->AutoJoint()=m_bools[prop];
					break;
				case IFXBonesManagerImpl::IsInfluential:
					pSelectBone->Influential()=m_bools[prop];
					break;
				default:
					break;
			}
		}
	}
	IFXRETURN(res); 
}

IFXRESULT IFXBonesManagerImpl::GetBool(BooleanProperty prop,bool *pGet)
{
	#if IFXBONESMGR_CHECKNULL
		if(!pGet)
			return IFX_E_INVALID_POINTER;
	#else
		IFXASSERT(pGet);
	#endif

	#if IFXBONESMGR_CHECKBOUNDS
		if(prop<0 || prop>=IFXBonesManagerImpl::MaxBoolean)
			return IFX_E_INVALID_RANGE;
	#endif

	if(prop>=IFXBonesManagerImpl::AutoJoint &&
		m_integers[IFXBonesManagerImpl::SelectBone]>=0)
	{
		IFXBoneNode *pSelectBone=GetBoneByIndex(
			m_integers[IFXBonesManagerImpl::SelectBone]);

		if(pSelectBone)
			switch(prop)
		{
			case IFXBonesManagerImpl::AutoLink:
				m_bools[prop]=(pSelectBone->IsAutoLink() != 0);
				break;
			case IFXBonesManagerImpl::AutoJoint:
				m_bools[prop]=(pSelectBone->IsAutoJoint() != 0);
				break;
			case IFXBonesManagerImpl::IsInfluential:
				m_bools[prop]=(pSelectBone->IsInfluential() != 0);
				break;
			default:
				break;
		}
	}

	*pGet=m_bools[prop];

	// cleared when read
	if(prop==IFXBonesManagerImpl::TimerUpdate)
		m_bools[prop]=false;

	return IFX_OK;
}


IFXRESULT IFXBonesManagerImpl::SetInteger(IntegerProperty prop,I32 set)
{
	IFXRESULT res = IFX_OK;

	#if IFXBONESMGR_CHECKBOUNDS
		if(prop<0 || prop>=IFXBonesManagerImpl::MaxInteger)
			return IFX_E_INVALID_RANGE;
	#endif

	if(prop==IFXBonesManagerImpl::NumberOfBones)
		res =  IFX_E_CANNOT_CHANGE;
	else if (prop==IFXBonesManagerImpl::SelectBone &&
		(set< -1 || set>=m_pCharacter->GetBoneTableSize()) )
		res = IFX_E_INVALID_RANGE;
	else 
	{
		m_integers[prop] = set;

		if(prop==IFXBonesManagerImpl::NumBoneLinks &&
			m_integers[IFXBonesManagerImpl::SelectBone]>=0)
		{
			IFXBoneNode *pSelectBone=GetBoneByIndex(
				m_integers[IFXBonesManagerImpl::SelectBone]);

			if(pSelectBone)
				pSelectBone->BoneLinks().SetNumberLinks(m_integers[prop]);
		}
	}

	IFXRETURN(res);
}


IFXRESULT IFXBonesManagerImpl::GetInteger(IntegerProperty prop, I32 *pGet)
{
	#if IFXBONESMGR_CHECKNULL
		if(!pGet)
			return IFX_E_INVALID_POINTER;
	#else
		IFXASSERT(pGet);
	#endif

	#if IFXBONESMGR_CHECKBOUNDS
		if(prop<0 || prop>=IFXBonesManagerImpl::MaxInteger)
			return IFX_E_INVALID_RANGE;
	#endif

	if(prop==IFXBonesManagerImpl::NumberOfBones)
		m_integers[prop]=m_pCharacter->GetBoneTableSize();

	if(prop==IFXBonesManagerImpl::NumBoneLinks &&
		m_integers[IFXBonesManagerImpl::SelectBone]>=0)
	{
		IFXBoneNode *selectbone=GetBoneByIndex(
			m_integers[IFXBonesManagerImpl::SelectBone]);

		if(selectbone)
			m_integers[prop]=selectbone->BoneLinks().GetNumberLinks();
	}

	*pGet = m_integers[prop];
	return IFX_OK;
}


IFXRESULT IFXBonesManagerImpl::SetFloat(FloatProperty prop,F32 set)
{
	#if IFXBONESMGR_CHECKBOUNDS
		if(prop<0 || prop>=IFXBonesManagerImpl::MaxFloat)
			return IFX_E_INVALID_RANGE;
	#endif

	if(prop==IFXBonesManagerImpl::ReachingLimit)
		return IFX_E_CANNOT_CHANGE;

	m_floats[prop] = set;

	if(prop==IFXBonesManagerImpl::BlendStartTime)
	{
		m_lasttime+=m_floats[prop]-m_blendparam.GetStartTimeConst();
		m_blendparam.SetStartTime(m_floats[prop]);
	}

	if(prop>=IFXBonesManagerImpl::LinkLength &&
		m_integers[IFXBonesManagerImpl::SelectBone]>=0)
	{
		IFXBoneNode *pSelectBone=GetBoneByIndex(
			m_integers[IFXBonesManagerImpl::SelectBone]);

		bool tip=m_bools[IFXBonesManagerImpl::ForTip];

		if(pSelectBone)
			switch(prop)
		{
			case IFXBonesManagerImpl::LinkLength:
				pSelectBone->BoneLinks().SetLinkLength(m_floats[prop]);
				break;
			case IFXBonesManagerImpl::JointCenterY:
				pSelectBone->CylindricalBounds()
					.GetCenter(tip)[1]=m_floats[prop];
				break;
			case IFXBonesManagerImpl::JointCenterZ:
				pSelectBone->CylindricalBounds()
					.GetCenter(tip)[2]=m_floats[prop];
				break;
			case IFXBonesManagerImpl::JointScaleY:
				pSelectBone->CylindricalBounds()
					.GetScale(tip)[1]=m_floats[prop];
				break;
			case IFXBonesManagerImpl::JointScaleZ:
				pSelectBone->CylindricalBounds()
					.GetScale(tip)[2]=m_floats[prop];
				break;
			default:
				break;
		}
	}

	return IFX_OK;
}

IFXRESULT IFXBonesManagerImpl::GetFloat(FloatProperty prop,F32 *pGet)
{
	#if IFXBONESMGR_CHECKNULL
		if(!pGet)
			return IFX_E_INVALID_POINTER;
	#else
		IFXASSERT(pGet);
	#endif

	#if IFXBONESMGR_CHECKBOUNDS
		if(prop<0 || prop>=IFXBonesManagerImpl::MaxFloat)
			return IFX_E_INVALID_RANGE;
	#endif

	IFXASSERT(m_pCharacter);
	if(prop==IFXBonesManagerImpl::ReachingLimit)
		m_floats[prop]=m_pCharacter->CalcReachingLimit();

	if(prop==IFXBonesManagerImpl::MinTime || prop==IFXBonesManagerImpl::MaxTime)
	{
		if(MotionMixer())
			MotionMixer()->GetMotionTimeLimits(
			&m_floats[IFXBonesManagerImpl::MinTime],
			&m_floats[IFXBonesManagerImpl::MaxTime]);
	}

	if(prop>=IFXBonesManagerImpl::LinkLength &&
		m_integers[IFXBonesManagerImpl::SelectBone]>=0)
	{
		IFXBoneNode *pSelectBone=GetBoneByIndex(
			m_integers[IFXBonesManagerImpl::SelectBone]);

		bool tip=m_bools[IFXBonesManagerImpl::ForTip];

		if(pSelectBone)
			switch(prop)
		{
			case IFXBonesManagerImpl::LinkLength:
				m_floats[prop]=pSelectBone->BoneLinks().GetLinkLength();
				break;
			case IFXBonesManagerImpl::JointCenterY:
				m_floats[prop]=pSelectBone->CylindricalBounds()
					.GetCenter(tip)[1];
				break;
			case IFXBonesManagerImpl::JointCenterZ:
				m_floats[prop]=pSelectBone->CylindricalBounds()
					.GetCenter(tip)[2];
				break;
			case IFXBonesManagerImpl::JointScaleY:
				m_floats[prop]=pSelectBone->CylindricalBounds()
					.GetScale(tip)[1];
				break;
			case IFXBonesManagerImpl::JointScaleZ:
				m_floats[prop]=pSelectBone->CylindricalBounds()
					.GetScale(tip)[2];
				break;
			default:
				break;
		}
	}

	if(prop==IFXBonesManagerImpl::BlendStartTime)
		m_floats[prop]=m_blendparam.GetStartTimeConst();

	*pGet=m_floats[prop];
	return IFX_OK;
}


IFXRESULT IFXBonesManagerImpl::GetTimedPeriod(TimedItem prop,F32 *pGet)
{
	#if IFXBONESMGR_CHECKNULL
		if(!pGet)
			return IFX_E_INVALID_POINTER;
	#else
		IFXASSERT(pGet);
	#endif

	#if IFXBONESMGR_CHECKBOUNDS
		if(prop<0 || prop>=IFXBonesManagerImpl::NumTimedItems)
			return IFX_E_INVALID_RANGE;
	#endif

	*pGet=m_lastperiod[prop];
	return IFX_OK;
}


/**
@note assumes m_pCharacter is a IFXMeshGroup_Character
*/
void IFXBonesManagerImpl::TimeProgress(IFXCharacter *pCharacter,
									   U32 index, bool on)
{
	IFXMeshGroup_Character *pMGC=(IFXMeshGroup_Character *)pCharacter;
	IFXBonesManagerImpl *pManager=(IFXBonesManagerImpl *)pMGC->GetBonesManager();
	if(!pManager)
		return;

	if(!pManager->m_getTimerCB)
		return;

	F32 timer=pManager->m_getTimerCB();

	if(on)
		pManager->m_lasttimer[index]=timer;
	else if(timer>pManager->m_lasttimer[index])
		pManager->m_sumtimer[index]+=timer-pManager->m_lasttimer[index];

	// when full frame accumulates to threshold, update all timers
	if(index==IFXBonesManagerImpl::TimeFrame && !on)
	{
		++pManager->m_frames;
		if(pManager->m_sumtimer[index] > 1000.0f)
		{
			I32 m, timeditems=IFXBonesManagerImpl::NumTimedItems;
			for(m=0; m < timeditems; ++m)
			{
				pManager->m_lastperiod[m]=pManager->m_sumtimer[m] /
					(F32)pManager->m_frames;
				pManager->m_sumtimer[m] = 0;
			}
			pManager->m_frames = 0;
			pManager->m_bools[IFXBonesManagerImpl::TimerUpdate] = true;
		}
	}
}

IFXRESULT IFXBonesManagerImpl::InitMesh(IFXMeshGroup *pMeshGrp,
										IFXVertexMapGroup *pVertexMapGroup)
{
	#if IFXBONESMGR_CHECKNULL
		//  if(!pMeshGrp || !pVertexMapGroup)
		if(!pMeshGrp)
			return IFX_E_INVALID_POINTER;
	#else
		IFXASSERT(pMeshGrp);
		//  IFXASSERT(pVertexMapGroup);
	#endif

	IFXRESULT result=m_pCharacter->AdoptMeshGroup(pMeshGrp,pVertexMapGroup);
	m_pCharacter->SetLastInput(pMeshGrp);

	return result;
}


IFXRESULT IFXBonesManagerImpl::UpdateMesh(IFXMeshGroup *pIn, IFXMeshGroup **ppOut)
{
	IFXRESULT res = IFX_OK;
	
	if(!pIn || !ppOut)
		res = IFX_E_INVALID_POINTER;

	TIME_PROGRESS(m_pCharacter,IFXBonesManagerImpl::TimeOutside,false);

	if(m_bools[IFXBonesManagerImpl::ReferenceLock])
	{
		m_pCharacter->ResetToReference();
		m_rootTransform.Reset();
	}
	else if(MotionMixer())
	{
		F32 difftime=m_floats[IFXBonesManagerImpl::Time]-
			m_blendparam.GetStartTimeConst();
		F32 weight=1.0f;
		bool incremental=false;
		F32 modtime=m_floats[IFXBonesManagerImpl::Time];

		TIME_PROGRESS(m_pCharacter, IFXBonesManagerImpl::TimeLastMotion,true);
		if(LastMixer())
		{
			if(m_bools[IFXBonesManagerImpl::AutoBlend])
			{
				if(m_blendparam.GetBlendTimeConst()>0.0f)
					weight=difftime/m_blendparam.GetBlendTimeConst();

				m_floats[IFXBonesManagerImpl::BlendFraction]=weight;
			}
			else
			{
				weight=m_floats[IFXBonesManagerImpl::BlendFraction];

				// fit a reasonable blendtime
				if(weight<=0.0f)
					m_blendparam.SetBlendTime(1.0f);
				else
					m_blendparam.SetBlendTime(difftime/weight);
			}

			if(weight<1.0f)
			{
				if(weight<0.0f || !m_blendparam.GetIncrementalConst())
				{
					// compute blendposition for last mixer
					F32 othertime=m_lasttime;

					if(!m_blendparam.GetStopLastConst() || difftime<0.0f)
					{
						if(difftime>0.0f)
							othertime+=difftime*m_blendscale;
						else
							othertime+=difftime;
					}

					LastMixer()->ApplyMotion(othertime,true);
				}

				incremental=(weight>0.0f && m_blendparam.GetIncrementalConst());

				modtime = m_blendparam.GetBlendTimeConst()-difftime;
				modtime *= m_blendscale;
				modtime = m_blendparam.GetBlendTimeConst()-modtime;

				modtime += m_blendparam.GetStartTimeConst();
			}
		}
		else
			m_floats[IFXBonesManagerImpl::BlendFraction]=1.0f;

		TIME_PROGRESS(m_pCharacter,IFXBonesManagerImpl::TimeLastMotion,false);

		TIME_PROGRESS(m_pCharacter,IFXBonesManagerImpl::TimeMotion,true);
		if(weight>0.0f)
			MotionMixer()->ApplyMotion(modtime,incremental);

		{
			IFXListContext context;
			IFXBoneNode *rootbone = NULL;
			IFXBoneNodeList &rootlist=m_pCharacter->Children();

			if(!m_bools[IFXBonesManagerImpl::AutoTranslationX])
			{
				m_last_translate[0]=0.0f;
				m_auto_translate[0]=0.0f;
			}
			if(!m_bools[IFXBonesManagerImpl::AutoTranslationY])
			{
				m_last_translate[1]=0.0f;
				m_auto_translate[1]=0.0f;
			}
			if(!m_bools[IFXBonesManagerImpl::AutoTranslationZ])
			{
				m_last_translate[2]=0.0f;
				m_auto_translate[2]=0.0f;
			}
			if(   !m_bools[IFXBonesManagerImpl::AutoRotationX]  &&
				!m_bools[IFXBonesManagerImpl::AutoRotationY]  &&
				!m_bools[IFXBonesManagerImpl::AutoRotationZ]  )
			{
				m_last_rotate.MakeIdentity();
				m_auto_rotate.MakeIdentity();
			}
			else if(!m_bools[IFXBonesManagerImpl::AutoRotationX]  ||
				!m_bools[IFXBonesManagerImpl::AutoRotationY]  ||
				!m_bools[IFXBonesManagerImpl::AutoRotationZ]  )
			{
				IFXEuler lasteuler=m_last_rotate;
				IFXEuler euler=m_auto_rotate;

				if(!m_bools[IFXBonesManagerImpl::AutoRotationX])
				{
					lasteuler[0]=0.0f;
					euler[0]=0.0f;
				}
				if(!m_bools[IFXBonesManagerImpl::AutoRotationY])
				{
					lasteuler[1]=0.0f;
					euler[1]=0.0f;
				}
				if(!m_bools[IFXBonesManagerImpl::AutoRotationZ])
				{
					lasteuler[2]=0.0f;
					euler[2]=0.0f;
				}

				euler.ConvertToQuaternion(m_auto_rotate);
				lasteuler.ConvertToQuaternion(m_last_rotate);
			}
			if(!m_bools[IFXBonesManagerImpl::AutoScaleX])
			{
				m_last_scale[0]=1.0f;
				m_auto_scale[0]=1.0f;
			}
			if(!m_bools[IFXBonesManagerImpl::AutoScaleY])
			{
				m_last_scale[1]=1.0f;
				m_auto_scale[1]=1.0f;
			}
			if(!m_bools[IFXBonesManagerImpl::AutoScaleZ])
			{
				m_last_scale[2]=1.0f;
				m_auto_scale[2]=1.0f;
			}

			rootlist.ToHead(context);
			while((rootbone=rootlist.PostIncrement(context)) != NULL)
			{
				IFXVector3 rotated;
				if(incremental)
				{
					/// @todo: update this one like below

					m_auto_rotate.RotateVector(
						rootbone->BlendDisplacement(),rotated);
					rootbone->BlendDisplacement().Add(rotated,m_auto_translate);
					rootbone->BlendRotation().Rotate(m_auto_rotate);
					rootbone->BlendScale().Multiply(m_auto_scale);
				}
				else
				{
					IFXTransform xauto,xlast,xroot,xblend;
					xauto.MakeTRS(m_auto_translate,m_auto_rotate,m_auto_scale);
					xlast.MakeTRS(m_last_translate,m_last_rotate,m_last_scale);
					xroot.MakeTRS(rootbone->DisplacementConst(),
						rootbone->RotationConst(),rootbone->ScaleConst());
					xblend.MakeTRS(rootbone->BlendDisplacement(),
						rootbone->BlendRotation(),rootbone->BlendScale());

					xauto.Multiply(xroot);
					xlast.Multiply(xblend);
					xauto.CalcTRS(&rootbone->Displacement(),
						&rootbone->Rotation(),&rootbone->Scale());
					xlast.CalcTRS(&rootbone->BlendDisplacement(),
						&rootbone->BlendRotation(),&rootbone->BlendScale());
				}
			}
		}
		TIME_PROGRESS(m_pCharacter,IFXBonesManagerImpl::TimeMotion,false);

		TIME_PROGRESS(m_pCharacter,IFXBonesManagerImpl::TimeIK,true);
		IFXIKModes &ikmodes=m_pCharacter->GetIKModes();
		ikmodes.SetLimitAngVelocity(
			m_bools[IFXBonesManagerImpl::LimitAngVelocity]);
		ikmodes.SetMaxAngVelocity(
			m_floats[IFXBonesManagerImpl::MaxAngVelocity]);
		ikmodes.SetIncremental(m_bools[IFXBonesManagerImpl::IKIncremental]);
		ikmodes.SetIterations(m_integers[IFXBonesManagerImpl::IKIterations]);

		if(m_bools[IFXBonesManagerImpl::ApplyIK])
			MotionMixer()->ApplyIK();
		TIME_PROGRESS(m_pCharacter,IFXBonesManagerImpl::TimeIK,false);

		TIME_PROGRESS(m_pCharacter,IFXBonesManagerImpl::TimeBlend,true);

		if(LastMixer() && weight<1.0f)
		{
			// weighted average on all bones

			if(weight>0.0f)
			{
				weight=(F32)IFXPOW(weight,m_blendparam.GetExponentConst());

				if(incremental)
					weight=1.0f-weight;
			}
			m_pCharacter->BlendBones(weight);
		}
		else
		{
			m_pLastMixer=NULL;
		}

		TIME_PROGRESS(m_pCharacter,IFXBonesManagerImpl::TimeBlend,false);
	}
	else
		m_pLastMixer=NULL;

	TIME_PROGRESS(m_pCharacter,IFXBonesManagerImpl::TimeMotion,true);
	if(m_bools[IFXBonesManagerImpl::RootClearTranslate] ||
		m_bools[IFXBonesManagerImpl::RootClearRotate])
	{
		IFXListContext context;
		IFXBoneNode *rootbone;
		IFXBoneNodeList &rootlist=m_pCharacter->Children();

		rootbone=rootlist.GetHead();
		if(rootbone)
		{
			IFXVector3 lastTranslation=m_rootTransform.TranslationConst();
			m_rootTransform.Reset();

			if(m_bools[IFXBonesManagerImpl::RootClearTranslate])
			{
				if(m_bools[IFXBonesManagerImpl::RootLock])
				{
					IFXVector3 advance;
					advance.Subtract(rootbone->Displacement(),lastTranslation);
					m_auto_translate.Subtract(advance);
					m_last_translate.Subtract(advance);

					m_rootTransform.SetTranslation(lastTranslation);
				}
				else
					m_rootTransform.SetTranslation(rootbone->Displacement());
			}

			if(m_bools[IFXBonesManagerImpl::RootClearRotate])
			{
				m_rootTransform.Rotate(rootbone->RotationConst());
				m_rootTransform.Scale(rootbone->ScaleConst());
			}
		}

		rootlist.ToHead(context);
		while((rootbone=rootlist.PostIncrement(context)) != NULL)
		{
			if(m_bools[IFXBonesManagerImpl::RootClearTranslate])
				rootbone->Displacement().Reset();
			else if(m_bools[IFXBonesManagerImpl::RootLock])
				rootbone->Displacement()=rootbone->ReferenceDisplacement();

			if(m_bools[IFXBonesManagerImpl::RootClearRotate])
			{
				rootbone->Rotation().MakeIdentity();
				rootbone->Scale().Set(1.0f,1.0f,1.0f);
			}
		}
	}
	else if(m_bools[IFXBonesManagerImpl::RootLock])
	{
		//old style rootlock: reset to displacement
		IFXListContext context;
		IFXBoneNode *rootbone;
		IFXBoneNodeList &rootlist=m_pCharacter->Children();

		rootlist.ToHead(context);
		while((rootbone=rootlist.PostIncrement(context)) != NULL)
			rootbone->Displacement()=rootbone->ReferenceDisplacement();
	}
	TIME_PROGRESS(m_pCharacter,IFXBonesManagerImpl::TimeMotion,false);

	TIME_PROGRESS(m_pCharacter,IFXBonesManagerImpl::TimeLinkRecalc,true);

	if(m_bools[IFXBonesManagerImpl::CreateBoneLinks])
		m_pCharacter->RecalcLinks();

	TIME_PROGRESS(m_pCharacter,IFXBonesManagerImpl::TimeLinkRecalc,false);

	TIME_PROGRESS(m_pCharacter,IFXBonesManagerImpl::TimeMotion,true);

	m_pCharacter->ForEachNodeTransformed(IFXPARENTFIRST|IFXSTORE_XFORM,NULL);

	TIME_PROGRESS(m_pCharacter,IFXBonesManagerImpl::TimeMotion,false);

	if (IFXSUCCESS(res))
	{
		if(!m_bools[IFXBonesManagerImpl::NoDeform])
			res = DeformMesh(pIn, ppOut);
	}

	TIME_PROGRESS(m_pCharacter,IFXBonesManagerImpl::TimeAllAnim, false);
	TIME_PROGRESS(m_pCharacter,IFXBonesManagerImpl::TimeOutside, true);

	return res;
}


IFXRESULT IFXBonesManagerImpl::DeformMesh(IFXMeshGroup *pIn,IFXMeshGroup **ppOut)
{
	#if IFXBONESMGR_CHECKNULL
		if(!pIn || !ppOut)
			return IFX_E_INVALID_POINTER;
	#else
		IFXASSERT(pIn);
		IFXASSERT(ppOut);
	#endif

	TIME_PROGRESS(m_pCharacter,IFXBonesManagerImpl::TimeFrame,false);
	TIME_PROGRESS(m_pCharacter,IFXBonesManagerImpl::TimeFrame,true);
	TIME_PROGRESS(m_pCharacter,IFXBonesManagerImpl::TimeAllAnim,true);

	IFXRESULT result=IFX_OK;
	if(m_pCharacter->GetLastInput()!=pIn)
		result=m_pCharacter->AdoptMeshGroup(pIn,NULL);
	m_pCharacter->SetLastInput(pIn);
	if(result!=IFX_OK)
		return result;

	//IFXMeshGroup *img=m_pCharacter->GetInputMeshGroup();
	IFXMeshGroup *pOMG=m_pCharacter->GetOutputMeshGroup();

	m_pCharacter->GetSkin()->DeformMesh(
		m_bools[IFXBonesManagerImpl::Renormalize],
		m_bools[IFXBonesManagerImpl::CLODLimited]);

	// Update position and normal versions on the output mesh
	U32 i;
	for( i = 0; i < pOMG->GetNumMeshes(); i++)
	{
		IFXMesh *pMesh = 0;
		pOMG->GetMesh(i, pMesh);
		if(pMesh)
		{
			pMesh->UpdateVersionWord(IFX_MESH_POSITION);
			pMesh->UpdateVersionWord(IFX_MESH_NORMAL);
		}
		IFXRELEASE(pMesh);
	}
	if(ppOut)
		*ppOut=pOMG;

	return IFX_OK;
}


IFXRESULT IFXBonesManagerImpl::SelectMotionMixer(
	IFXMotionMixer *pSelect)
{
	IFXRESULT result=IFX_OK;

	if(pSelect)
		result=((IFXMotionMixerImpl *)pSelect)
		->VerifyCharacter((IFXCharacter *)m_pCharacter);
	if(result!=IFX_OK)
		pSelect=NULL;

	if(pSelect)
	{
		if(MotionMixer())
		{
			IFXVector3 location,location2;
			IFXQuaternion rotation,rotation2;

			pSelect->CalcBonePositionAtTime(0,0.0f,&location2,&rotation2);

			MotionMixer()->CalcBonePositionAtTime(0,
				m_floats[IFXBonesManagerImpl::Time],
				&location,&rotation);
		}
		else
		{
			m_auto_translate.Reset();
			m_auto_rotate.MakeIdentity();
			m_auto_scale.Set(1.0f,1.0f,1.0f);
		}
	}

	m_last_translate.Reset();
	m_last_rotate.MakeIdentity();
	m_last_scale.Set(1.0f,1.0f,1.0f);

	m_pMotionMixer=pSelect;
	m_pLastMixer=NULL;

	// TEMP test for corruption
	MotionMixer();

	if(pSelect)
		m_pCharacter->ResetToReference();

	IFXRETURN(result); 
}


void IFXBonesManagerImpl::ApplyAutoTransition(
	const IFXVector3 &rLocation1,
	const IFXVector3 &rLocation2,
	const IFXQuaternion &rRotation1,
	const IFXQuaternion &rRotation2
	)
{
	/// @todo: remove or implement this function
}


IFXRESULT IFXBonesManagerImpl::BlendToMotionMixer(IFXMotionMixer *pNext,
												  const IFXBlendParam *pParam)
{
	IFXMotionMixer *pStoremixer=MotionMixer();

	if(MotionMixer() && LastMixer())
	{
		IFXTRACE_GENERIC(L"BlendToMotionMixer() triple-blending not implemented ");
		IFXTRACE_GENERIC(L"%p -> %p + %p\n",LastMixer(),MotionMixer(),pNext);
	}

	bool store_auto[9];
	store_auto[0]=m_bools[IFXBonesManagerImpl::AutoTranslationX];
	store_auto[1]=m_bools[IFXBonesManagerImpl::AutoTranslationY];
	store_auto[2]=m_bools[IFXBonesManagerImpl::AutoTranslationZ];
	store_auto[3]=m_bools[IFXBonesManagerImpl::AutoRotationX];
	store_auto[4]=m_bools[IFXBonesManagerImpl::AutoRotationY];
	store_auto[5]=m_bools[IFXBonesManagerImpl::AutoRotationZ];
	store_auto[6]=m_bools[IFXBonesManagerImpl::AutoScaleX];
	store_auto[7]=m_bools[IFXBonesManagerImpl::AutoScaleY];
	store_auto[8]=m_bools[IFXBonesManagerImpl::AutoScaleZ];

	m_bools[IFXBonesManagerImpl::AutoTranslationX]=false;
	m_bools[IFXBonesManagerImpl::AutoTranslationY]=false;
	m_bools[IFXBonesManagerImpl::AutoTranslationZ]=false;
	m_bools[IFXBonesManagerImpl::AutoRotationX]=false;
	m_bools[IFXBonesManagerImpl::AutoRotationY]=false;
	m_bools[IFXBonesManagerImpl::AutoRotationZ]=false;
	m_bools[IFXBonesManagerImpl::AutoScaleX]=false;
	m_bools[IFXBonesManagerImpl::AutoScaleY]=false;
	m_bools[IFXBonesManagerImpl::AutoScaleZ]=false;

	SelectMotionMixer(pNext);

	m_bools[IFXBonesManagerImpl::AutoTranslationX]=store_auto[0];
	m_bools[IFXBonesManagerImpl::AutoTranslationY]=store_auto[1];
	m_bools[IFXBonesManagerImpl::AutoTranslationZ]=store_auto[2];
	m_bools[IFXBonesManagerImpl::AutoRotationX]=store_auto[3];
	m_bools[IFXBonesManagerImpl::AutoRotationY]=store_auto[4];
	m_bools[IFXBonesManagerImpl::AutoRotationZ]=store_auto[5];
	m_bools[IFXBonesManagerImpl::AutoScaleX]=store_auto[6];
	m_bools[IFXBonesManagerImpl::AutoScaleY]=store_auto[7];
	m_bools[IFXBonesManagerImpl::AutoScaleZ]=store_auto[8];

	m_lasttime=m_floats[IFXBonesManagerImpl::Time];
	m_pLastMixer=pStoremixer;
	m_blendparam=*pParam;
	m_last_translate=m_auto_translate;
	m_last_rotate=m_auto_rotate;
	m_last_scale=m_auto_scale;

	if(pParam->GetBlendTimeConst()>0.0f)
		m_blendscale=(pParam->GetBlendTimeConst()+pParam->GetRealignmentConst())
		/ pParam->GetBlendTimeConst();
	else
		m_blendscale=1.0f;

	// auto displacement
	if(MotionMixer() && LastMixer())
	{
		IFXVector3 location,location2;
		IFXQuaternion rotation,rotation2;

		MotionMixer()->CalcBonePositionAtTime(0,
			pParam->GetStartTimeConst(),&location2,&rotation2);

		LastMixer()->CalcBonePositionAtTime(0,
			m_lasttime+pParam->GetRealignmentConst(),
			&location,&rotation);
	}

	return IFX_OK;
}



IFXRESULT IFXBonesManagerImpl::SetAutoTranslate(const IFXVector3 &rVector,
												bool last)
{
	IFXTRACE_GENERIC(L"AutoT%d %ls ->%ls\n",last,
		(last? m_last_translate: m_auto_translate).Out().Raw(),
		rVector.Out().Raw());

	//IFXVector3 rotated;

	if(last)
		m_last_translate=rVector;
	else
	{
		m_auto_rotate.RotateVector(rVector,m_auto_translate);
		m_auto_translate.Multiply(m_auto_scale);
	}
	return IFX_OK;
}



IFXRESULT IFXBonesManagerImpl::SetAutoRotate(const IFXQuaternion &rQuaternion,
											 bool last)
{
	if(last)
		m_last_rotate=rQuaternion;
	else
		m_auto_rotate=rQuaternion;
	return IFX_OK;
}


IFXRESULT IFXBonesManagerImpl::SetAutoScale(const IFXVector3 &rVector,
											bool last)
{
	if(last)
		m_last_scale=rVector;
	else
		m_auto_scale=rVector;
	return IFX_OK;
}



IFXRESULT IFXBonesManagerImpl::AddToAutoTranslate(const IFXVector3 &rAddVector,
												  bool last)
{
	IFXVector3 rotated;

	if(last)
	{
		m_last_translate.Add(rAddVector);
	}
	else
	{
		m_auto_rotate.RotateVector(rAddVector,rotated);
		rotated.Multiply(m_auto_scale);
		m_auto_translate.Add(rotated);
	}
	return IFX_OK;
}



IFXRESULT IFXBonesManagerImpl::AddToAutoRotate(
	const IFXQuaternion &addQuaternion,bool last)
{
	if(last)
		m_last_rotate.Multiply(addQuaternion);
	else
		m_auto_rotate.Multiply(addQuaternion);
	return IFX_OK;
}

IFXRESULT IFXBonesManagerImpl::AddToAutoScale( const IFXVector3 &rAddVector,
											  bool last)
{
	if(last)
		m_last_scale.Multiply(rAddVector);
	else
		m_auto_scale.Multiply(rAddVector);
	return IFX_OK;
}


IFXRESULT IFXBonesManagerImpl::PrepareBones(void)
{
	IFXRESULT result=m_pCharacter->CleanupWeights(
		m_bools[IFXBonesManagerImpl::RegenerateWeights],
		m_bools[IFXBonesManagerImpl::RemoveRogueWeights],
		m_bools[IFXBonesManagerImpl::SmoothWeights],
		m_floats[IFXBonesManagerImpl::SmoothThreshold],
		m_floats[IFXBonesManagerImpl::WeldMax],
		m_floats[IFXBonesManagerImpl::ModelSize]);

	m_joints_calced=false;

	if(result != IFX_OK)
		return result;

	// store pre-linked weights
	if(m_bools[IFXBonesManagerImpl::PrepareForSave])
		m_unlinked_weights.CopyFrom(m_pCharacter->GetVertexWeights());

	if(m_bools[IFXBonesManagerImpl::CreateBoneLinks])
	{
		if(!m_joints_calced)
		{
			CalcJoints();
			m_joints_calced=true;
		}
		m_pCharacter->StateProgress("Create BoneLinks");
		m_pCharacter->GenerateBoneLinks(
			m_integers[IFXBonesManagerImpl::DefaultBoneLinks]);
	}

	m_pCharacter->GetSkin()->PackVertexWeights();
	if(!m_bools[IFXBonesManagerImpl::PrepareForSave])
		m_pCharacter->GetSkin()->GetVertexWeights().Clear();

	m_pCharacter->StateProgress("");

	return IFX_OK;
}


void IFXBonesManagerImpl::CalcJoints(void)
{
	m_pCharacter->StateProgress("Final Calc Joints");
	IFXTRACE_GENERIC(L"Calc Joints\n");

	m_pCharacter->ResetToReference();
	m_pCharacter->ForEachNodeTransformed(IFXPARENTFIRST|IFXSTORE_XFORM, NULL);

	m_pCharacter->GetSkin()->CalculateJointCrossSections(false,true);
}


IFXRESULT IFXBonesManagerImpl::CreateIKChain(I32 baseBoneId, I32 endBoneId)
{
	IFXRESULT res = IFX_OK;

	if(endBoneId<0 || endBoneId>=m_pCharacter->GetBoneTableSize() ||
		baseBoneId >= m_pCharacter->GetBoneTableSize() )
	{
		res = IFX_E_INVALID_HANDLE;
	}
	else 
	{
		IFXBoneNode *endNode=m_pCharacter->LookupBoneIndex(endBoneId);
		IFXBoneNode *baseNode=NULL;

		if(baseBoneId>=0)
			baseNode=m_pCharacter->LookupBoneIndex(baseBoneId);
		
		if(!baseNode || !endNode)
			res = IFX_E_INVALID_HANDLE;
		else
			endNode->CreateEffections(baseNode);
	}

	IFXRETURN(res); 
}


IFXBoneNode* IFXBonesManagerImpl::GetBoneByIndex(I32 index)
{
	IFXBoneNode *pSelectBone = NULL;

	if(index >= 0 && index<m_pCharacter->GetBoneTableSize())
		pSelectBone=m_pCharacter->LookupBoneIndex(index);
	
	return pSelectBone;
}

void IFXBonesManagerImpl::CreateCircle(void)
{
	U32 m;

	for(m=0; m < IFX_BONECIRCLE_SIDES; ++m)
	{
		F32 angle = IFXPI * 2.0f * m/(F32)IFX_BONECIRCLE_SIDES;

		m_circledata[m].Set(0.0f,IFXCOS(angle),IFXSIN(angle));
	}

	m_circledata[IFX_BONECIRCLE_SIDES] = m_circledata[0];
}
