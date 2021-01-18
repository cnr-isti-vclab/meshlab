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
	@file IFXIKModes.h
*/

#ifndef IFXIKMODES_H
#define IFXIKMODES_H

class IFXIKModes
{
public:
	IFXIKModes(void)
	{ Reset(); };

	void Reset(void)
	{
		m_limitangvelocity=FALSE;
		m_maxangvelocity=0.0f;
		m_incremental=TRUE;
		m_iterations=1;
	};

	bool    GetLimitAngVelocity(void)       { return m_limitangvelocity; };
	void    SetLimitAngVelocity(bool set)   { m_limitangvelocity=set; };

	F32     GetMaxAngVelocity(void)         { return m_maxangvelocity; };
	void    SetMaxAngVelocity(F32 set)      { m_maxangvelocity=set; };

	bool    GetIncremental(void)            { return m_incremental; };
	void    SetIncremental(bool set)        { m_incremental=set; };

	U32     GetIterations(void)             { return m_iterations; };
	void    SetIterations(U32 set)          { m_iterations=set; };

private:
	bool    m_limitangvelocity;
	F32     m_maxangvelocity;
	bool    m_incremental;
	U32     m_iterations;
};

#endif
