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

#ifndef __IFXRay_H__
#define __IFXRay_H__

#include "IFXVector3.h"

/**
    Class for a ray, represented by position and direction.

	This representation also supports methods for treating a ray as a line
	as well as a plane(implicitly as a perpendicular).
*/
class IFXRay 
{
public:
	            // No default clear
	            IFXRay() {}; 
	            IFXRay( IFXVector3 position, IFXVector3 direction );

	void        Reset();
	void        Set( const IFXVector3& position, const IFXVector3& direction );
	const	IFXVector3& GetPosition() const;
	const	IFXVector3& GetDirection() const;
			F32 DistanceFromLine( const IFXVector3& point ) const;
			F32 DistanceFromLine( const IFXVector3& point, 
										F32&        parametricValue ) const;

	IFXVector3 position;
	IFXVector3 direction;
};


IFXINLINE IFXRay::IFXRay( IFXVector3 vInPosition, IFXVector3 vInDirection )
{ 
	position = vInPosition;
	direction = vInDirection;
}

IFXINLINE void IFXRay::Reset()
{
	position.Set(0,0,0);
	direction.Set(0,0,0);
}

IFXINLINE const IFXVector3& IFXRay::GetPosition() const
{
	return position;
}

IFXINLINE const IFXVector3& IFXRay::GetDirection() const
{
	return direction;
}


IFXINLINE void IFXRay::Set( const IFXVector3& vInPosition, const IFXVector3& vInDirection )
{
	position = vInPosition;
	direction = vInDirection;
}


IFXINLINE F32 IFXRay::DistanceFromLine( const IFXVector3& point ) const 
{
	IFXVector3 v, dir = direction;
	v.Subtract(point,position);
	dir.Scale( dir.DotProduct(v)/dir.DotProduct(dir));
	v.Subtract(dir);
	return v.CalcMagnitude();
}


IFXINLINE F32 IFXRay::DistanceFromLine( const IFXVector3& point, 
	                                                F32&        parametricValue ) const 
{
	IFXVector3 v, dir = direction;
	v.Subtract(point,position);
	parametricValue = dir.DotProduct(v)/dir.DotProduct(dir);
	dir.Scale( parametricValue);
	v.Subtract(dir);
	return v.CalcMagnitude();
}


#endif
