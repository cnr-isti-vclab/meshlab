
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

#ifndef __IFXInstant_h__
#define __IFXInstant_h__

#include "IFXQuaternion.h"

/**
	This class represents an instantaneous state in time including location,
	rotation, and scale.
*/
class IFXInstant
	{
	public:
						IFXInstant(void)	{ Reset(); };

						/// Resets data to identity values.
		void			Reset(void)
							{
							m_location.Reset();
							m_rotation.MakeIdentity();
							m_scale.Set(1.0f,1.0f,1.0f);
							};

						/// Standard copy constructor.
						IFXInstant(const IFXInstant &operand)
							{ operator=(operand); };

						/// Accesses the location component.
		IFXVector3		&Location(void)					{ return m_location; };
						/// Accesses the location component disallowing changes.
const	IFXVector3		&LocationConst(void)	const	{ return m_location; };

						/// Accesses the rotation component.
		IFXQuaternion	&Rotation(void)					{ return m_rotation; };
						/// Accesses the rotation component disallowing changes.
const	IFXQuaternion	&RotationConst(void)	const	{ return m_rotation; };

						/// Accesses the scale component.
		IFXVector3		&Scale(void)					{ return m_scale; };
						/// Accesses the scale component disallowing changes.
const	IFXVector3		&ScaleConst(void)		const	{ return m_scale; };

						/// Standard deep copy.
		IFXInstant		&operator=(const IFXInstant &operand)
							{
							m_location=operand.m_location;
							m_rotation=operand.m_rotation;
							m_scale=operand.m_scale;

							return *this;
							};

	private:
		IFXVector3		m_location;
		IFXQuaternion	m_rotation;
		IFXVector3		m_scale;
	};


#endif
