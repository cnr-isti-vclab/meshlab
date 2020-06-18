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
	@file IFXBoneLinks.h
*/

#ifndef IFXBONELINKS_H
#define IFXBONELINKS_H

#include "IFXList.h"

/******************************************************************************
class IFXBoneLinks
	link length is for display purposes
*/
class IFXBoneLinks
{
public:
	IFXBoneLinks(void)
	{
		m_numberlinks=0;
		m_linklength=1.0f;
		//                          ResetSamples();
	};

	IFXBoneNode     *GetLink(I32 index) { return m_linkarray[index]; };
	// if index<0, enter into next NULL location
	void            SetLink(I32 index,IFXBoneNode *set)
	{
		if(index>=0)
		{
			m_linkarray[index]=set;
			return;
		}

		U32 m;
		for(m=0;m<(U32)m_numberlinks;m++)
			if(!m_linkarray[m])
			{
				m_linkarray[m]=set;
				return;
			}
			IFXASSERT(false);   // more links than m_numberlinks
	};

	void            SetNumberLinks(I32 set) { m_numberlinks=set; };
	I32             &GetNumberLinks(void)   { return m_numberlinks; };

	void            AllocateArray(void)
	{
		m_linkarray.ResizeToExactly(m_numberlinks);
		U32 m;
		for(m=0;m<(U32)m_numberlinks;m++)
			m_linkarray[m]=NULL;
	};
	I32             GetArraySize(void)
	{ return m_linkarray.GetNumberElements(); };

	F32             &GetLinkLength(void)    { return m_linklength; };
	void            SetLinkLength(F32 set)
	{
		m_linklength=set;
		//                          m_samples= -1;
	};

#if FALSE
	void            ResetSamples(void)
	{
		m_linklength=0.0f;
		m_samples=0;
	};
	void            SampleLength(F32 sample)
	{
		if(m_samples<0)
			return;

		// IFXTRACE_GENERIC("sample=%.6G length=%.6G m_samples=%d\n",sample,m_linklength,m_samples);

		m_linklength=(m_linklength*m_samples+sample)/
			(F32)(m_samples+1);
		m_samples++;

		// IFXTRACE_GENERIC("            length=%.6G m_samples=%d\n",m_linklength,m_samples);
	};
#endif

private:

	I32                     m_numberlinks;
	IFXArray<IFXBoneNode *> m_linkarray;
	F32                     m_linklength;

	//      I32                     m_samples;
};

#endif
