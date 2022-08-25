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
	@file IFXBoneCache.h
*/

#ifndef IFXBONECACHE_H
#define IFXBONECACHE_H

#include "IFXMatrix3x4.h"
#include "IFXTransform.h"

#define IFXBONECACHE_NORMALS TRUE // only set up for IFXMesh

/**
	contains minimal data for a each bone required for deformation
*/
class IFXBoneCache
{
public:

	IFXMatrix3x4& GetGlobalMatrix(void)
	{ return m_globalmatrix; };
	
	const IFXMatrix3x4& GetGlobalMatrixConst(void) const
	{ return m_globalmatrix; };

	#if !IFXBONECACHE_NORMALS
		IFXTransform    &GetGlobalDeltaTransform(void)
		{ return m_tglobaldelta; };
	#endif

private:
	IFXMatrix3x4    m_globalmatrix;

	#if !IFXBONECACHE_NORMALS
		IFXTransform    m_tglobaldelta;
	#endif
};

#endif
