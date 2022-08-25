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
	@file IFXLong3.h
*/

#ifndef IFXLONG3_H
#define IFXLONG3_H

#include "IFXList.h"

/**
	this could be made into a big deal, but it's not used much
*/
class IFXLong3
{
public:
	IFXLong3(void)
	{
		m_data[0]=0;
		m_data[1]=0;
		m_data[2]=0;
	}

	I32& operator[](I32 index)
	{ return m_data[index]; }

	void Set(I32 a, I32 b, I32 c)
	{
		m_data[0] = a;
		m_data[1] = b;
		m_data[2] = c;
	}

private:
	I32 m_data[3];
};

class IFXLong3List: public IFXList<IFXLong3>
{
public:
	IFXLong3List(void)
	{
		SetAutoDestruct(true);
	};
};

#endif
