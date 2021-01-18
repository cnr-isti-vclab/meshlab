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
	@file	IFXBoundingBox.h

			This interface is used to compute the axis-aligned bounding box
			of a set of 3D points.
*/

#ifndef IFXBOUNDINGBOX_H
#define IFXBOUNDINGBOX_H

#include <float.h>
#include "IFXDataTypes.h"
#include "IFXVector3.h"

/** This class is used to compute the axis-aligned bounding box 
of a set of 3D points. */
class IFXBoundingBox
{
public:
	/** 
		Initialize the min to the largest float value,
		initalize the max to the largest negative float value.
	*/
	IFXBoundingBox();

	/**
		Adds a 3D point to the bounding box computation.

		@param pVector A pointer to the added 3D point.
	*/
	void AddVector(IFXVector3* pVector);

	/**
		Gets the diagonal of the axis-aligned box that bounds the
		set of 3D points specified by the AddVector() method.

		@param pMin A pointer to the output value that is a min diagonal pointer.
		@param pMax A pointer to the output value that is a max diagonal pointer.
	*/
	void GetBox(IFXVector3* pMin, IFXVector3* pMax);

private:
	IFXVector3 m_min;		///< Current min x,y,z values
	IFXVector3 m_max;		///< Current max x,y,z values
};

//
//  Inline methods
//

IFXINLINE IFXBoundingBox::IFXBoundingBox() :
	m_min(FLT_MAX, FLT_MAX, FLT_MAX),
	m_max(-FLT_MAX, -FLT_MAX, -FLT_MAX)
{
}


IFXINLINE void IFXBoundingBox::AddVector(IFXVector3* pVector)
{
	F32 a;
	I32 i;

	IFXASSERT(pVector);

	if (NULL != pVector)
	{
		for ( i = 0; i < 3; ++i)
		{
			a = pVector->Value(i);
			if (a < m_min.Value(i))
				m_min.Value(i) = a;
			if (a > m_max.Value(i))
				m_max.Value(i) = a;
		}
	}
}

IFXINLINE void IFXBoundingBox::GetBox(IFXVector3* pMin, 
									  IFXVector3* pMax)
{
	*pMin = m_min;
	*pMax = m_max;
}

#endif
