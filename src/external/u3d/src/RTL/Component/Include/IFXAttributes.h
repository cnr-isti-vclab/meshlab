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
	IFXAttributes.h

     Declaration of IFXAttributes template
*/

#ifndef __IFXATTRIBUTES_H__
#define __IFXATTRIBUTES_H__

#include <string.h>

template <unsigned int NUM_ATTRIBUTES>
class IFXAttributes
{
	bool flags[NUM_ATTRIBUTES];

public:
	IFXAttributes()
	{
		memset(flags, 0, sizeof(flags));
	}
	IFXAttributes(IFXAttributes<NUM_ATTRIBUTES> const& src)
	{
		memcpy(flags, src.flags, sizeof(flags));
	}
	IFXAttributes(unsigned int const i)
	{
		memset(flags, 0, sizeof(flags));
		if (i < NUM_ATTRIBUTES) flags[i] = true;
	}
	IFXAttributes<NUM_ATTRIBUTES>& operator= (IFXAttributes<NUM_ATTRIBUTES> const& src)
	{
		memcpy(flags, src.flags, sizeof(flags));
		return *this;
	}
	IFXAttributes<NUM_ATTRIBUTES>& operator= (unsigned int const i)
	{
		memset(flags, 0, sizeof(flags));
		if (i < NUM_ATTRIBUTES) flags[i] = true;
		return *this;
	}
	IFXAttributes<NUM_ATTRIBUTES> operator& (unsigned int const i) const
	{
		IFXAttributes<NUM_ATTRIBUTES> a;
		if (i < NUM_ATTRIBUTES) a.flags[i] = flags[i];
		return a;
	}
	IFXAttributes<NUM_ATTRIBUTES> operator& (IFXAttributes<NUM_ATTRIBUTES> const& src) const
	{
		IFXAttributes<NUM_ATTRIBUTES> a(*this);
		unsigned int i = 0;
		for (; i < NUM_ATTRIBUTES; i++)
			a.flags[i] &= src.flags[i];
		return a;
	}
	IFXAttributes<NUM_ATTRIBUTES>& operator&= (unsigned int const i)
	{
		unsigned int j = 0;
		for (; j < NUM_ATTRIBUTES; j++)
			if (i != j) flags[i] = false;
		return *this;
	}
	IFXAttributes<NUM_ATTRIBUTES> operator| (unsigned int const i) const
	{
		IFXAttributes<NUM_ATTRIBUTES> a(*this);
		if (i < NUM_ATTRIBUTES) a.flags[i] = true;
		return a;
	}
	IFXAttributes<NUM_ATTRIBUTES> operator| (IFXAttributes<NUM_ATTRIBUTES> const& src) const
	{
		IFXAttributes<NUM_ATTRIBUTES> a(*this);
		unsigned int i = 0;
		for (; i < NUM_ATTRIBUTES; i++)
			a.flags[i] |= src.flags[i];
		return a;
	}
	IFXAttributes<NUM_ATTRIBUTES>& operator|= (unsigned int const i)
	{
		if (i < NUM_ATTRIBUTES) flags[i] = true;
		return *this;
	}
	IFXAttributes<NUM_ATTRIBUTES> operator^ (unsigned int const i) const
	{
		IFXAttributes<NUM_ATTRIBUTES> a(*this);
		if (i < NUM_ATTRIBUTES) a.flags[i] = !a.flags[i];
		return a;
	}
	IFXAttributes<NUM_ATTRIBUTES> operator^ (IFXAttributes<NUM_ATTRIBUTES> const& src) const
	{
		IFXAttributes<NUM_ATTRIBUTES> a(*this);
		unsigned int i = 0;
		for (; i < NUM_ATTRIBUTES; i++)
			a.flags[i] ^= src.flags[i];
		return a;
	}
	IFXAttributes<NUM_ATTRIBUTES>& operator^= (unsigned int const i)
	{
		if (i < NUM_ATTRIBUTES) flags[i] = !flags[i];
		return *this;
	}
	IFXAttributes<NUM_ATTRIBUTES> operator~ () const
	{
		IFXAttributes<NUM_ATTRIBUTES> a(*this);
		unsigned int i = 0;
		for (; i < NUM_ATTRIBUTES; i++)
			a.flags[i] = !a.flags[i];
		return a;
	}
	IFXAttributes<NUM_ATTRIBUTES> operator>> (unsigned int const i) const
	{
		IFXAttributes<NUM_ATTRIBUTES> a(*this);
		int j = 0;
		for (; (unsigned)j < NUM_ATTRIBUTES-i; j++)
			a.flags[j] = a.flags[j+i];
		for (; (unsigned)j < NUM_ATTRIBUTES; j++)
			a.flags[j] = false;
		return a;
	}
	IFXAttributes<NUM_ATTRIBUTES> operator<< (unsigned int const i) const
	{
		IFXAttributes<NUM_ATTRIBUTES> a(*this);
		int j = NUM_ATTRIBUTES-1;
		for (; (unsigned)j >= i; j--)
			a.flags[j] = a.flags[j-i];
		for (; j >= 0; j--)
			a.flags[j] = false;
		return a;
	}
	bool operator== (IFXAttributes<NUM_ATTRIBUTES> const& src) const
	{
		return (memcmp(flags, src.flags, sizeof(flags)) == 0);
	}
	bool operator!= (IFXAttributes<NUM_ATTRIBUTES> const& src) const
	{
		return (memcmp(flags, src.flags, sizeof(flags)) != 0);
	}
	bool operator[] (unsigned int const i) const
	{
		return flags[i];
	}
	bool toBool() const
	{
		bool q;
		unsigned int i;
		for (q = false, i = 0; i < NUM_ATTRIBUTES; i++)
			q |= flags[i];
		return q;
	}
	bool isSet(unsigned int const i) const
	{
		return (i < NUM_ATTRIBUTES) ? flags[i] : false;
	}
	void set(unsigned int const i)
	{
		if (i < NUM_ATTRIBUTES) flags[i] = true;
	}
	void reset(unsigned int const i)
	{
		if (i < NUM_ATTRIBUTES) flags[i] = false;
	}
	void clear()
	{
		memset(flags, 0, sizeof(flags));
	}
};

#endif
