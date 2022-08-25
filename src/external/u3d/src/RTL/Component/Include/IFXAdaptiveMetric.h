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
	@file  IFXAdaptiveMetric.h

			The header file that defines the IFXAdaptiveMetric interface.
*/

#ifndef IFXADAPTIVEMETRIC_DOT_H
#define IFXADAPTIVEMETRIC_DOT_H

class IFXTQTTriangle;

/**
	This is the main interface for IFXAdaptiveMetric.
*/
class IFXAdaptiveMetric
{
public:
	virtual ~IFXAdaptiveMetric() {}
	/**
		Action - An enumerated type definition.\n

		- @b Subdivide 
		- @b Consolidate 
		- @b Sustain 
	*/
    enum Action {Subdivide, Consolidate, Sustain};

	/**
		This method evaluates the triangle and determines which action to take.

		@param	pTriangle	A pointer to an IFXTQTTriangle object.
		@param	pAction		A pointer to the Action to take for the triangle.\n

		
		@return void
	*/
	virtual void EvaluateTriangle (IFXTQTTriangle *pTriangle, Action *pAction) = 0;
};

#endif
