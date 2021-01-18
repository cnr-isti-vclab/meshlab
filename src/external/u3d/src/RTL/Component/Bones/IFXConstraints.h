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
	@file IFXConstraints.h
*/

#ifndef IFXCONSTRAINTS_H
#define IFXCONSTRAINTS_H

#include "IFXEuler.h"

class IFXConstraints
{
public:
	IFXConstraints(void)
	{
		m_world_relative=true;

		m_active[0]=true;
		m_active[1]=true;
		m_active[2]=true;

		m_limited[0]=false;
		m_limited[1]=false;
		m_limited[2]=false;

		m_bias[0]=2;
		m_bias[1]=2;
		m_bias[2]=2;
		m_lastEuler.Reset();

		m_min.Reset();
		m_max.Reset();
	};

	void            SetActive(bool x,bool y,bool z)
	{
		m_active[0]=x;
		m_active[1]=y;
		m_active[2]=z;
	};
	void            SetLimited(bool x,bool y,bool z)
	{
		m_limited[0]=x;
		m_limited[1]=y;
		m_limited[2]=z;
	};
	void            SetMin(F32 x,F32 y,F32 z)
	{ m_min.Set(x,y,z); };
	void            SetMax(F32 x,F32 y,F32 z)
	{ m_max.Set(x,y,z); };

	bool            &WorldRelative(void)    { return m_world_relative; };
	const   bool            &WorldRelativeConst(void) const
	{ return m_world_relative; };

	bool            *Active(void)                   { return m_active; };
	const   bool            *ActiveConst(void) const        { return m_active; };
	bool            *Limited(void)                  { return m_limited; };
	const   bool            *LimitedConst(void) const       { return m_limited; };

	char            *Bias(void)                     { return m_bias; };
	IFXEuler        &LastEuler(void)                { return m_lastEuler; };

	IFXVector3      &Min(void)                      { return m_min; };
	const   IFXVector3      &MinConst(void) const           { return m_min; };
	IFXVector3      &Max(void)                      { return m_max; };
	const   IFXVector3      &MaxConst(void) const           { return m_max; };

private:
	bool            m_world_relative;
	bool            m_active[3];
	bool            m_limited[3];
	IFXVector3      m_min;
	IFXVector3      m_max;

	char            m_bias[3];      /// dynamic preference to stablize IK
	// trinary: 0=min 1=max 2=don't care
	IFXEuler        m_lastEuler;    /// history
};

#endif
