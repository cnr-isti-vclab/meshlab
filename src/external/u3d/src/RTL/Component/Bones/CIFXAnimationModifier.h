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
	@file CIFXAnimationModifier.h
*/

#ifndef CIFXANIMATIONMODIFIER_H
#define CIFXANIMATIONMODIFIER_H

#include "IFXDids.h"
#include "IFXModifierDataPacket.h"

#include "CIFXModifier.h"
#include "IFXMixerQueueImpl.h"
#include "IFXAnimationModifier.h"

#include "IFXMixerConstruct.h"
#include "IFXMotionResource.h"
#include "IFXSkeletonDataElement.h"

class CIFXAnimationModifier :	public CIFXModifier,
								virtual public IFXMixerQueueImpl,
								virtual public IFXAnimationModifier,
								virtual public IFXMarkerX
{
private:
	
	class CIFXMotionResourceManager: public IFXMotionManager
	{
	public:
		
		CIFXMotionResourceManager(void):
		  m_pMotionResourcePalette(NULL)
		  {
			  m_pBonesModifier=NULL;
			  m_pMotionResourcePalette=NULL;
			  m_pMixerResourcePalette=NULL;
		  }
		  virtual	IFXRESULT IFXAPI 	Reset(void)
		  {
			  IFXRELEASE(m_pMotionResourcePalette);
			  m_pMotionResourcePalette=NULL;
			  return IFX_OK;
		  }
		  virtual				~CIFXMotionResourceManager(void)
		  {
			  IFXRELEASE(m_pMotionResourcePalette);
			  IFXRELEASE(m_pMixerResourcePalette);
		  }
		  virtual	IFXRESULT IFXAPI 	FindMotion(const IFXString& rName,U32 *motionid);
		  virtual	IFXRESULT IFXAPI 	GetMotion(I32 motionId,IFXMotion **ppMotion,
			  IFXString *pSourcename);
		  virtual	IFXRESULT IFXAPI 	PopulateMotionMixer(const IFXString& rMixerName,
			  IFXMotionMixer *pMotionMixer);
		  
		  void		SetBonesModifier(IFXAnimationModifier *set)
		  {	m_pBonesModifier=set; };
		  void		SetPalettes(IFXPalette *motionPalette,
			  IFXPalette *mixerPalette)
		  {
			  IFXRELEASE(m_pMotionResourcePalette);
			  IFXRELEASE(m_pMixerResourcePalette);
			  
			  //* assumes ref already added
			  m_pMotionResourcePalette=motionPalette;
			  m_pMixerResourcePalette=mixerPalette;
		  };
		  IFXPalette	*GetMotionPalette(void)
		  {	return m_pMotionResourcePalette; };
		  IFXPalette	*GetMixerPalette(void)
		  {	return m_pMixerResourcePalette; };

	private:
		IFXRESULT	InitializeMotionMixer(IFXMotionMixer *pMotionMixer);
		IFXRESULT	MapHierarchy(IFXBonesManager *pBonesManager,
			IFXMotionMixer *pMotionMixer,
			U32 motionId,const IFXString& rBoneName);
		IFXRESULT	MapConstruct(IFXBonesManager *pBonesManager,
			IFXMotionMixer *pMotionMixer,
			IFXMixerConstruct *pMixCon, const IFXString& rBoneName);
		
		IFXAnimationModifier	*m_pBonesModifier;
		IFXPalette			*m_pMotionResourcePalette;
		IFXPalette			*m_pMixerResourcePalette;
	};
	
	// constructor determines type of class
	CIFXAnimationModifier( void );
	virtual				~CIFXAnimationModifier(void);
	
	friend	IFXRESULT	IFXAPI_CALLTYPE CIFXAnimationModifier_Factory(IFXREFIID iid, void** ppv);
	
	public:
		//* IFXUnknown
		U32 IFXAPI 			AddRef (void);
		U32 IFXAPI 			Release (void);
		IFXRESULT IFXAPI 	QueryInterface (IFXREFIID riid, void **ppv);
		
		//* IFXMarkerX
		void IFXAPI 		GetEncoderX (IFXEncoderX*& rpEncoderX);
		
		//* IFXModifier
		IFXRESULT IFXAPI 	GetOutputs (
			IFXGUID**&	rpOutOutputs, 
			U32&		rOutNumberOfOutputs,
			U32*&		rpOutOutputDepAttrs );
		
		IFXRESULT IFXAPI 	GetDependencies (
			IFXGUID*	pInOutputDID,
			IFXGUID**&	pOutInputDependencies,
			U32&		uOutNumberInputDependencies,
			IFXGUID**&	pOutOutputDependencies,
			U32&		uOutNumberOfOutputDependencies,
			U32*&		puOutDepAttrs );
		
		IFXRESULT IFXAPI 	GenerateOutput (U32 inOutputDataElementIndex,
			void*& rpOutData, BOOL& rNeedRelease );
		
		IFXRESULT IFXAPI 	SetDataPacket (IFXModifierDataPacket* pInInputDataPacket,
			IFXModifierDataPacket* pInDataPacket );
		
		IFXRESULT IFXAPI 	Notify (IFXModifierMessage eInMessage,
			void* pMessageContext );
		
		//* IFXAnimationModifier
		void		Setup(void);
		
		virtual	IFXBonesManager* IFXAPI GetBonesManagerNR(void);

		void  IFXAPI    SetAsKeyframe();
		void  IFXAPI    SetAsBones();
		BOOL  IFXAPI 	IsKeyframe();
		
	protected:
		
		virtual	IFXRESULT IFXAPI 	SubmitEvent(bool startevent, const IFXString& rName,F32 localTime);
		
	private:
		void		SetBonesManager(IFXBonesManager *pBonesManager);
		IFXRESULT	InitBonesManager( void );
		
		//* IFXUnknown
		U32							m_uRefCount;
		
		BOOL						m_bIsKeyframe;
		BOOL						m_bInitialized;
		
		//* IFXModifier
		U32							m_uMeshGroupDataElementIndex;
		U32							m_uSkeletonDataElementIndex;
		U32							m_uBonesManagerDataElementIndex;
		U32							m_uBoneWeightsDataElementIndex;
		U32							m_uTransformDataElementIndex;
		U32							m_uSimulationTimeDataElementIndex;
		
		U32							m_uLastMeshGroupChangeCount;
		U32							m_uBonesManagerChangeCount;
		U32							m_uBoneWeightsChangeCount;
		
		//* IFXAnimationModifier
		IFXTransform				m_outputTransform;
		
		CIFXMotionResourceManager	m_motionManager;
		
		IFXBonesManager				*m_pBonesManager;
		IFXCharacter				*m_pCharacter;
		
		IFXMeshGroup				*m_pOutputMeshGroup;
		IFXSkeletonDataElement		*m_pSkeletonOutputElement;

		IFXArray<IFXMatrix4x4>	m_RootTransform;
};

#endif
