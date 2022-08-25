
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

#ifndef __IFXKeyFrame_h__
#define __IFXKeyFrame_h__

#include "IFXInstant.h"

/**
A derived class of IFXInstant including the time it represents.
*/
class IFXKeyFrame : public IFXInstant
	{
	public:
					IFXKeyFrame(void)
						{
						m_time=0.0f;
						};

					/// Accesses the time.
		F32			&Time(void)				{ return m_time; };
					/// Accesses the time disallowing changes.
const	F32			&TimeConst(void) const	{ return m_time; };

					/// Deprecated.
		void		SetTime(float set)		{ m_time=set; };

					/// Standard deep copy.
		IFXKeyFrame	&operator=(const IFXKeyFrame &operand)
						{
						IFXInstant::operator=(operand);
						m_time=operand.m_time;

						return *this;
						};

	private:
		F32	m_time;
	};



#endif
