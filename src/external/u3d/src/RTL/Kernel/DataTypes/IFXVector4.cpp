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


#include "IFXVector4.h"


IFXString IFXVector4::Out(BOOL verbose) const
{
	IFXString result;
    char mstr[40];

	if(verbose)
		result = L"Vector4 {";
	sprintf( mstr, "%G", m_value[0]);
	result += ( (U8*)mstr );
	result += ( L" " );
	sprintf( mstr, "%G", m_value[1]);
	result += ( (U8*)mstr );
	result += ( L" " );
	sprintf( mstr, "%G", m_value[2]);
	result += ( (U8*)mstr );
	result += ( L" " );
	sprintf( mstr, "%G", m_value[3]);
	result += ( (U8*)mstr );
	if( verbose )
		result += L"}";

	return result;
}


/******************************************************************************
	IFXVector4 is a four element F32 array: x, y, z, h.  The fourth,
	usually considered a homogenious component, is used here to represent the
	radius of a sphere, centered at x, y, and z.
	Description:
	Updates the spherical representation so that the new state will encompass
	the previous sphere and the incomming sphere.  In essence, "incorporate"
	the incomming bounding sphere into the bound.
******************************************************************************/
void IFXVector4::IncorporateSphere(const IFXVector4 &bound)
{
	//* In addition to guarding against NULL incomming bounds, we need to
	//* check that the incomming bound is valid(radius != -1).
	if(bound.Radius() >= 0.0)
	{
		//* If THIS bound is uninitialized, then simply assimilate the values
		//* from the incomming sphere's bound.
		if(Radius()<0.0f)
		{
			*this=bound;
		}
		else
		{
			//* If initialized, determine the correct bounding sphere.

			//* First determine the vector between the spheres centers.
			IFXVector4 vectorBetweenCenters;
			vectorBetweenCenters.Subtract(*this,bound);

			//* distance between the spheres' centers
			F32 distanceBetweenCenters=vectorBetweenCenters.CalcMagnitude3();

			//* If this bound does not already encompass the incoming sphere.
			if(Radius() < bound.Radius()+distanceBetweenCenters)
			{
				//* If the incoming sphere encompasses this sphere, then
				//* simply assimilate the values from the incomming sphere's
				//* bound.
				if(bound.Radius() > Radius()+distanceBetweenCenters)
				{
					*this=bound;
				}
				//* Guard against divide by zero.
				else if(distanceBetweenCenters != 0.0f)
				{
					//* A new sphere must be calculated.

					F32 d=1.0f/distanceBetweenCenters;
					d*=Radius()-bound.Radius();
					vectorBetweenCenters.Scale3(0.5f+0.5f*d);
					this->Add(bound,vectorBetweenCenters);
					m_value[3]=0.5f*
							(Radius()+bound.Radius()+distanceBetweenCenters);
				}
			}
		}
	}
}

void IFXVector4::IncorporateSphere(const IFXVector4 * pBound)
{
	//* In addition to guarding against NULL incomming bounds, we need to
	//* check that the incomming bound is valid(radius != -1).
/*	IFXVector4 left, right;
	right.set(pBound->X(),pBound->Y(),pBound->Z(),pBound->Radius());
	left.set(X(),Y(),Z(),Radius());
	left.Incorporate(&right);
	m_value[0] = left.X();
	m_value[1] = left.Y();
	m_value[2] = left.Z();
	m_value[3] = left.Radius();

*/
	if(pBound->Radius() >= 0.0)
	{
		//* If THIS bound is uninitialized, then simply assimilate the values
		//* from the incomming sphere's bound.
		if(Radius()<0.0f)
		{
			*this=*pBound;
		}
		else
		{
			//* If initialized, determine the correct bounding sphere.

			//* First determine the vector between the spheres centers.
			IFXVector4 vectorBetweenCenters;
			vectorBetweenCenters.Subtract(*this,*pBound);

			//* distance between the spheres' centers
			F32 distanceBetweenCenters=vectorBetweenCenters.CalcMagnitude3();

			//* If this bound does not already encompass the incoming sphere.
			if(Radius() < pBound->Radius()+distanceBetweenCenters)
			{
				//* If the incoming sphere encompasses this sphere, then
				//* simply assimilate the values from the incomming sphere's
				//* bound.
				if(pBound->Radius() > Radius()+distanceBetweenCenters)
				{
					*this=*pBound;
				}
				//* Guard against divide by zero.
				else if(distanceBetweenCenters != 0.0f)
				{
					//* A new sphere must be calculated.

					F32 d=1.0f/distanceBetweenCenters;
					d*=Radius()-pBound->Radius();
					vectorBetweenCenters.Scale3(0.5f+0.5f*d);
					this->Add(*pBound,vectorBetweenCenters);
					m_value[3]=0.5f*
							(Radius()+pBound->Radius()+distanceBetweenCenters);
				}
			}
		}
	}
}

