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


#include "IFXVector3.h"

IFXString IFXVector3::Out(BOOL verbose) const
{
	char buffer[128];

	if(verbose)
		sprintf(buffer,"Vector3 {%.6G %.6G %.6G}",
										m_value[0],m_value[1],m_value[2]);
	else
		sprintf(buffer,"%.6G %.6G %.6G",m_value[0],m_value[1],m_value[2]);

	IFXString result((U8*)buffer);
	return result;
}


/******************************************************************************
	simple linear interpolation

******************************************************************************/
void IFXVector3::Interpolate(F32 t,const IFXVector3 &from,
														const IFXVector3 &to)
{
	F32 diff=1.0f-t;

	m_value[0]=from.m_value[0]*diff+to.m_value[0]*t;
	m_value[1]=from.m_value[1]*diff+to.m_value[1]*t;
	m_value[2]=from.m_value[2]*diff+to.m_value[2]*t;
}

// eof
