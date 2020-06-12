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
	@file	CIFXAuthorLineSetAnalyzer.h

			Declaration of helper class to analyze connectivity of 
			IFXAuthorLineSet
*/

#ifndef __CIFXAUTHORLINESETANALYZER_H__
#define __CIFXAUTHORLINESETANALYZER_H__

#include "IFXAuthorLineSet.h"
#include "IFXLine.h"
#include "IFXArray.h"
#include "IFXAuthorLineSetAnalyzer.h"

struct IFXPositionConnectivity 
{
	IFXPositionConnectivity () 
	{
		m_uLineCount	= 0;
		m_LineIndex.Clear();
		m_EndPosIndex.Clear();
	};

	U32		m_uLineCount;
	IFXArray <U32> m_LineIndex;
	IFXArray <U32> m_EndPosIndex;
};

class CIFXAuthorLineSetAnalyzer : virtual public IFXAuthorLineSetAnalyzer
{
public:
	// IFXUnknown methods
	U32 IFXAPI  AddRef ();
	U32 IFXAPI  Release ();
	IFXRESULT IFXAPI QueryInterface ( IFXREFIID interfaceId, void** ppInterface );

	IFXRESULT IFXAPI Initialize(IFXAuthorLineSet* pLineSet);
	BOOL IFXAPI   IsInitialized() {  return m_initialized; };

	IFXRESULT IFXAPI Update();

	/// returns line indexes and their end positions connected with specified positions
	IFXRESULT IFXAPI GetLines(
						U32 pointInd, 
						IFXArray <U32>& lines, 
						IFXArray <U32>& endPoints );

	void IFXAPI DumpAuthorLineSet();

private:
	CIFXAuthorLineSetAnalyzer();
	virtual ~CIFXAuthorLineSetAnalyzer();

	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXAuthorLineSetAnalyzer_Factory( 
													IFXREFIID	interfaceId, 
													void**		ppInterface );
	IFXRESULT GenerateConnectivity();

	// Reference count for IFXUnknown
	U32				m_uRefCount;		

	IFXAuthorLineSet*			m_pLineSet;
	IFXAuthorLineSetDesc		m_LineSetDesc;
	U32							m_uMaxPositionCount;
	BOOL						m_initialized;

	IFXPositionConnectivity* m_pPosInfo;
};

#endif
