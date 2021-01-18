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
	@file IFXCylinder.h
*/

#ifndef IFXCYLINDER_H
#define IFXCYLINDER_H

/**
	Tapered Cylinder (non-right)

	Consists of center and scale at base and end points.
	Convention dictates that X is the longitudinal axis.
*/
class IFXCylinder
{
public:
	IFXCylinder(void)
	{
		m_center[0].Reset();
		m_center[0].Reset();
		m_scale[1].Reset();
		m_scale[1].Reset();
	}

	IFXVector3  &GetCenter(I32 index)
	{
		IFXASSERT(index==0 || index==1);
		return m_center[index];
	}

	IFXVector3  &GetScale(I32 index)
	{
		IFXASSERT(index==0 || index==1);
		return m_scale[index];
	}

private:
	IFXVector3 m_center[2];
	IFXVector3 m_scale[2];
};

#endif
