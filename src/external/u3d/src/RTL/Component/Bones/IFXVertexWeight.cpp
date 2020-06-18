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
	@file IFXVertexWeight.cpp
*/

#include <wchar.h>
#include "IFXVertexWeight.h"

#if __MINGW32__
#define swprintf _snwprintf
#endif

/**
	Generates a string representing the contents of this VertexWeight. 

	@todo check: verbose - unused parameter?
*/
IFXString IFXVertexWeight::Out(bool verbose) const
{
	const int IFXBONES_MAX_STRING_LEN = 256;

	IFXCHAR buffer[ IFXBONES_MAX_STRING_LEN ];

	swprintf(
		buffer,
		IFXBONES_MAX_STRING_LEN, L"%d:%d %d %.6G %ls %ls",
		m_meshid, m_vertexid, m_boneid, m_weight,m_offset.Out().Raw(),
		m_normalOffset.Out().Raw());

	IFXString result(buffer);

	return result;
}
