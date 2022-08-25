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

#ifndef IFXEULER_H
#define IFXEULER_H

#include "IFXDataTypes.h"
#include "IFXQuaternion.h"

/**
	@file	IFXEuler.h

	@brief	Set of 3 orthogonal rotations.

			The primary intent of this object is for conversions to and from
			IFXQuaternion.

			In this context, an Euler is defined as three angles about the the
			axes X, Y, and Z.  The order of rotations are Z->Y->X.

			So given a IFXQuaternion quaternion1,

			@code
				IFXEuler euler=quaternion1;
			@endcode

			creates an euler whose three angle values can be used like

			@code
				IFXQuaternion quaternion2;
				quaternion2.MakeRotation(euler[0],Z_AXIS);
				quaternion2.Rotate(euler[1],Y_AXIS);
				quaternion2.Rotate(euler[2],X_AXIS);
			@endcode

			where quaternion2 == quaternion1.

			Note that there can be more than one Euler solution for any 
			quaternion. An euler->quaternion->euler conversion may not result 
			in the same euler angles.

			But a quaternion->euler->quaternion conversion will always return
			to the same quaternion, since there is only one quaternion solution
			for any aribitrary-axis rotation.  (Exceptional cases may occur
			around Euler problem areas.)

			Note that every quaternion has an exactly equivalent twin,
			with every element of opposite sign.   This pair is referred to
			as one solution.

			Note that some cases of 90 degree rotations can create Gimbal Lock
			in Euler angles by aligning rotation axes.  Quaternions are not
			suceptable to this, but conversions may not be accurate in these 
			cases.

			Specifically, the Y result of a quaternion->euler conversion is 
			limited to +-90 degrees.  Solution near the limits of Y are very
			unstable in X and Z.
*/

/**
This interface is used for conversions to and from IFXQuaternion.
*/
class IFXEuler
{
	public:

						/** Default constructor.  This does not initialize
							the angles. */
						IFXEuler(void)					{ }
						/** Copies constructor.*/
						IFXEuler(const IFXEuler& rOperand)
													{ operator=(rOperand); }
						/** Constructs by converting from a quaternion.*/
						IFXEuler(const IFXQuaternion& rOperand)
													{ operator=(rOperand); }

						/** Resets the Euler to three angles of 0.*/
		void			Reset(void);

						/** Sets the three axial angles, in radians.*/
		void			Set(F32 x,F32 y,F32 z);

						/** Copies from another Euler.*/
		IFXEuler&		operator=(const IFXEuler& rOperand);

						/** Converts into this Euler from a quaternion.
						This is a step-by-step conceptual conversion.
						
						This could probably be made quite a bit short
						if someone could create a reduced mathematical 
						conversion.*/
		IFXEuler&		operator=(const IFXQuaternion& rOperand);

						/** Compares with another Euler using zero tolerance.*/
		bool			operator==(const IFXEuler& rOperand) const;

						/** Converts this euler to a quaternion, outputing
							into the operand. */
		void			ConvertToQuaternion(IFXQuaternion& rOperand) const;

						/** Accesses an indexed element (0 to 2).*/
		F32&			operator[](const int x)		{ return m_data[x]; }

						/** Returns the three angles as an array of F32.*/
		F32*			Raw(void)						{ return m_data; }

						/** Returns the three angles as a const array of F32.*/
const	F32*			RawConst(void) const			{ return m_data; }

	private:
		F32				m_data[3];
};

IFXINLINE void IFXEuler::Set(F32 x,F32 y,F32 z)
{
	m_data[0]=x;
	m_data[1]=y;
	m_data[2]=z;
}

IFXINLINE void IFXEuler::Reset(void)
{
	m_data[0]=0.0f;
	m_data[1]=0.0f;
	m_data[2]=0.0f;
}

IFXINLINE IFXEuler& IFXEuler::operator=(const IFXEuler& rOperand)
{
	if (this != &rOperand)
	{
		m_data[0]=rOperand.m_data[0];
		m_data[1]=rOperand.m_data[1];
		m_data[2]=rOperand.m_data[2];
	}

	return *this;
}

IFXINLINE bool IFXEuler::operator==(const IFXEuler& rOperand) const
{
	return (	m_data[0]==rOperand.m_data[0]	&&
				m_data[1]==rOperand.m_data[1]	&&
				m_data[2]==rOperand.m_data[2]	);
}

#endif
