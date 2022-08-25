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
	@file	IFXAuthorLineSetAnalyzer.h

			Declaration of helper interface to analyze connectivity of 
			IFXAuthorLineSet
*/


#ifndef __IFXAUTHOLINESETANALYZER_H__
#define __IFXAUTHOLINESETANALYZER_H__

#include "IFXUnknown.h"
#include "IFXArray.h"
#include "IFXAuthorLineSet.h"

// {8A5107D4-7F2F-4518-B6DA-551B79C4266D}
IFXDEFINE_GUID(IID_IFXAuthorLineSetAnalyzer,
0x8a5107d4, 0x7f2f, 0x4518, 0xb6, 0xda, 0x55, 0x1b, 0x79, 0xc4, 0x26, 0x6d);


class IFXAuthorLineSetAnalyzer : public IFXUnknown
{
public:
	virtual	IFXRESULT IFXAPI Initialize(IFXAuthorLineSet* pLineSet) = 0;
	
	virtual BOOL IFXAPI IsInitialized() = 0;
	
	virtual IFXRESULT IFXAPI Update() = 0;		

	virtual IFXRESULT IFXAPI GetLines(U32 pointInd, IFXArray <U32>& lines, IFXArray <U32>& endPoints) = 0;
	
	virtual void IFXAPI DumpAuthorLineSet() = 0;
};

#endif
