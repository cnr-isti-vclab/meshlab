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
	@file	CIFXMixerConstruct.h

			Implementation of IFXMixerConstruct interface.
			The IFXMixerConstruct interface is used to get to animation data.
			The animation data is stored in separate tracks.
			Each track generally corresponds to the animation of one bone.
*/

#ifndef CIFXMixerConstruct_H
#define CIFXMixerConstruct_H

#include "IFXMixerConstruct.h"
#include "CIFXSubject.h"
#include "CIFXMarker.h"
#include "IFXCoreCIDs.h"

class IFXSceneGraph;
class IFXMotionResource;

class CIFXMixerConstruct : private CIFXSubject,
						   private CIFXMarker,
                   virtual public  IFXMixerConstruct
{
public:
// IFXMarkerX
	void IFXAPI 		GetEncoderX (IFXEncoderX*& rpEncoderX);

// IFXMixerConstruct	
	void                  IFXAPI SetMotionResource( 
	                                   IFXMotionResource* pIFXMotionResource );
	IFXMotionResource*    IFXAPI GetMotionResource();
	void                  IFXAPI AddMapping(  IFXMixerConstruct* pMixer, 
											  IFXString*        pBoneName );
	IFXINLINE
	IFXList<IFXMapEntry>* IFXAPI GetEntryList() {return &m_EntryList;}
	F32                   IFXAPI GetDuration();
	MotionType            IFXAPI GetType();

private:
	IFXMotionResource*   m_pIFXMotionResource;
	IFXList<IFXMapEntry> m_EntryList;
	F32                  m_Duration;
	MotionType           m_type;


// IFXUnknown
public:
	U32 IFXAPI        AddRef ();
	U32 IFXAPI        Release ();
	IFXRESULT IFXAPI  QueryInterface ( IFXREFIID riid, void** ppv );

private:
	U32       m_refCount;
	
	IFXSceneGraph* m_pSceneGraph;

	          CIFXMixerConstruct();
	virtual  ~CIFXMixerConstruct();
	friend 
	IFXRESULT IFXAPI_CALLTYPE CIFXMixerConstruct_Factory( IFXREFIID interfaceId, 
	                                      void**    ppInterface );
};


#endif
