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
	@file IFXBonesManagerImpl.h
*/

#ifndef IFXBONESMANAGERIMPL_H
#define IFXBONESMANAGERIMPL_H

#include "IFXTransform.h"
#include "IFXBonesManager.h"
#include "IFXVertexWeights.h"
#include "IFXBlendParam.h"
#include "IFXBoneNode.h"
#include "IFXMeshGroup_Character.h"

/** 
	@note You need to init mesh at full resolution
*/
class IFXBonesManagerImpl : public IFXBonesManager
{
public:
			IFXBonesManagerImpl(IFXBonesManager *pCloneOf = NULL);
virtual     ~IFXBonesManagerImpl(void);

virtual IFXRESULT IFXAPI    Reset(void);
virtual IFXRESULT IFXAPI    ResetUnshared(void);

virtual IFXRESULT IFXAPI    UpdateReferencePositions();

virtual IFXRESULT IFXAPI    SetBool(BooleanProperty prop,bool set);
virtual IFXRESULT IFXAPI    GetBool(BooleanProperty prop,bool *pGet);
virtual IFXRESULT IFXAPI    SetInteger(IntegerProperty prop,I32 set);
virtual IFXRESULT IFXAPI    GetInteger(IntegerProperty prop,I32 *pGet);
virtual IFXRESULT IFXAPI    SetFloat(FloatProperty prop,F32 set);
virtual IFXRESULT IFXAPI    GetFloat(FloatProperty prop,F32 *pGet);
virtual IFXRESULT IFXAPI    GetTimedPeriod(TimedItem prop,F32 *pGet);
virtual IFXRESULT IFXAPI    SetMeshFactory(IFXMeshFactory *pSet)
{
	m_pMeshFactory=pSet;
	return IFX_OK;
}

virtual IFXRESULT IFXAPI    InitMesh(
									 IFXMeshGroup *pMeshGrp,
									 IFXVertexMapGroup *pVertexMapGroup
									 );

virtual IFXRESULT IFXAPI    UpdateMesh(IFXMeshGroup *pIn,IFXMeshGroup **ppOut);
virtual IFXRESULT IFXAPI    DeformMesh(IFXMeshGroup *pIn,IFXMeshGroup **ppOut);

virtual IFXRESULT IFXAPI    PrepareBones(void);

virtual IFXRESULT IFXAPI    GetBoneIndex(IFXString boneName,I32 *pBoneId);
virtual IFXRESULT IFXAPI    GetBoneName(I32 boneId, IFXString *pBoneName);

virtual IFXRESULT IFXAPI    SelectMotionMixer(IFXMotionMixer *pSelect);
virtual IFXRESULT IFXAPI    BlendToMotionMixer(IFXMotionMixer *pNext,
												const IFXBlendParam *pParam);

virtual IFXRESULT IFXAPI    GetRootClearance(IFXTransform *pTransform)
{
	*pTransform=m_rootTransform;
	return IFX_OK;
};

virtual IFXRESULT IFXAPI    CreateIKChain(I32 baseBoneId,I32 endBoneId);

virtual IFXRESULT IFXAPI    GetCharacter(IFXCharacter **ppGet)
{
	IFXRESULT res = IFX_OK;

	if(NULL == ppGet)
		res = IFX_E_INVALID_POINTER;
	else
		*ppGet=(IFXCharacter *)m_pCharacter;
	
	return res;
};

virtual IFXRESULT IFXAPI SetProgressCallback(IFXProgressCB progressCB)
{
	m_pCharacter->SetProgressCB(progressCB);
	return IFX_OK;
};

virtual IFXRESULT IFXAPI SetGetTimerCallback(IFXGetTimerCB set)
{
	m_getTimerCB=set;
	return IFX_OK;
};

virtual IFXRESULT IFXAPI SetAutoTranslate(const IFXVector3 &vector,bool last);
virtual IFXRESULT IFXAPI SetAutoRotate(const IFXQuaternion &quaternion,bool last);
virtual IFXRESULT IFXAPI SetAutoScale(const IFXVector3 &vector,bool last);
virtual IFXRESULT IFXAPI AddToAutoTranslate(const IFXVector3 &addVector,
											bool last);
virtual IFXRESULT IFXAPI AddToAutoRotate(const IFXQuaternion &addQuaternion,
																	bool last);
virtual IFXRESULT IFXAPI AddToAutoScale(const IFXVector3 &addVector,bool last);

virtual IFXRESULT IFXAPI GetMotionMixer(U32 index,
									   IFXMotionMixer **ppMotionMixer)
{
	IFXRESULT res = IFX_OK;

	if(NULL == ppMotionMixer)
		res = IFX_E_INVALID_POINTER;
	else
		*ppMotionMixer=index? m_pLastMixer: m_pMotionMixer;
	
	return res;
};

virtual void     IFXAPI     ReplaceCharacter(IFXMeshGroup_Character *pSet);

static  void        TimeProgress(IFXCharacter *pCharacter, U32 index, bool on);

private:

	static const U32 IFX_BONECIRCLE_SIDES = 64;

	void ApplyAutoTransition(
		const IFXVector3 &rLocation1,
		const IFXVector3 &rLocation2,
		const IFXQuaternion &rRotation1,
		const IFXQuaternion &rRotation2
		);

	void        CreateCircle(void);
	void        CalcJoints(void);

	IFXBoneNode *GetBoneByIndex(I32 index);

	IFXMotionMixer  *MotionMixer(void);
	IFXMotionMixer  *LastMixer(void);


	IFXMeshGroup_Character  *m_pCharacter,*m_pLocalCharacter;
	IFXVertexWeights        m_unlinked_weights; // don't need if no save

	IFXMeshFactory          *m_pMeshFactory;///< @todo [5/2/2006] Need check: do we need this?

	bool                    m_bools[MaxBoolean];
	I32                     m_integers[MaxInteger];
	F32                     m_floats[MaxFloat];
	bool                    m_joints_calced;

	/// @todo 04/26/06 Investigate, what do we need this code for (m_circledata)?
	IFXVector3              m_circledata[IFX_BONECIRCLE_SIDES+1];

	IFXGetTimerCB           m_getTimerCB;
	U32                     m_frames;
	F32                     m_lasttimer[NumTimedItems];
	F32                     m_sumtimer[NumTimedItems];
	F32                     m_lastperiod[NumTimedItems];


	/// @note not sharable
	IFXMotionMixer  *m_pMotionMixer;

	// blending parameters
	IFXMotionMixer  *m_pLastMixer;
	IFXBlendParam   m_blendparam;
	F32             m_lasttime;
	F32             m_blendscale;
	IFXTransform    m_rootTransform;
	IFXVector3      m_auto_translate;
	IFXVector3      m_last_translate;
	IFXQuaternion   m_auto_rotate;
	IFXQuaternion   m_last_rotate;
	IFXVector3      m_auto_scale;
	IFXVector3      m_last_scale;
};

#endif
