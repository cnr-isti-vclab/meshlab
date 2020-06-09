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
//
//  CIFXSceneGraph.h
//
//	DESCRIPTION
//
//	NOTES
//
//***************************************************************************
#ifndef __CIFXSCENEGRAPH_H__
#define __CIFXSCENEGRAPH_H__
#include "IFXSceneGraph.h"
#include "IFXPalette.h"
#include "IFXRenderContext.h"
#include "CIFXSubject.h"
#include "CIFXMarker.h"
#include "IFXCoreCIDs.h"

class CIFXSceneGraph : public CIFXSubject,
               virtual public IFXSceneGraph
{
	CIFXSceneGraph();
	virtual ~CIFXSceneGraph();
	friend 
	IFXRESULT IFXAPI_CALLTYPE CIFXSceneGraph_Factory(IFXREFIID iid, void** ppv);

public:
	// IFXUnknown
	U32 IFXAPI        AddRef ();
	U32 IFXAPI        Release ();
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID riid, void** ppv);

	IFXRESULT  IFXAPI 	 Mark(void);
	IFXRESULT  IFXAPI 	 Marked(BOOL* pbOutMarked);
	void      IFXAPI SetPriority( U32 uInPriority, BOOL bRecursive, BOOL bPromotionOnly );

	// IFXSceneGraph
	IFXRESULT  IFXAPI 	 GetPalette(EIFXPalette eInPalette, IFXPalette** ppOutPalette);

	IFXRESULT  IFXAPI 	 GetDebugFlags(U32* puOutDebugFlags);
	IFXRESULT  IFXAPI 	 SetDebugFlags(U32 uInDebugFlags);

	U32 IFXAPI CurrentMark(void);
	IFXRESULT  IFXAPI 	 UnmarkAll(void);
	IFXRESULT  IFXAPI 	 Initialize(IFXCoreServices *pCS);

	IFXRESULT  IFXAPI 	 GetCoreServices(IFXCoreServices **ppCS);

	IFXRESULT  IFXAPI 	 GetAnimationEnabledState( BOOL* pState );
    IFXRESULT  IFXAPI 	 SetAnimationEnabledState( BOOL state );

	IFXRESULT  IFXAPI 	 GetSimClockSubject(IFXSubject** ppOutClockSubject);
#if 0
	IFXRESULT SetSimClockSubject(IFXSubject* pInClockSubject);
#endif

private:
	// IFXUnknown
	U32 m_uRefCount;

	// IFXMarker
	U32 m_uRunningMarker;
	U32 m_uMark;
	U32 m_uPriority;
	U32 m_uUserData;

	// IFXSceneGraph
	BOOL m_bInitialized;
	IFXPalette* m_pPalettes[NUMBER_OF_PALETTES];
	U32 m_uDebugFlags;

	IFXSubject		*m_pClockSubject;

	IFXCoreServices *m_pCS;

	BOOL m_bAnimationEnabledState; // BOOL that if TRUE indicates that animation is globally enabled.  If FALSE, animation is globally disabled regardless of local settings.
};


#endif 
