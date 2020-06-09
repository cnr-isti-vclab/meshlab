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
//
//  CIFXShader.h
//
//	DESCRIPTION
//		This class implements common Shader functionality. It is not
//		ment to be instantiated.  
//
//	NOTES
//		The intent is to simplify the development of new Shaders by 
//		inheriting and forwarding calls to this implementation. 
//
//***************************************************************************
#ifndef __CIFXSHADER_H__
#define __CIFXSHADER_H__

#include "IFXSceneGraph.h"
#include "IFXShader.h"
#include "CIFXMarker.h"
#include "CIFXSubject.h"

class CIFXShader : protected CIFXMarker, 
                   protected CIFXSubject,
           virtual public     IFXShader
{
public:
	IFXList<IFXShadedElement>& IFXAPI ShadedElementList() {return m_shadedElementList;}

	U32 IFXAPI SetRenderPassFlags(U32 uFlags) { return m_uRenderPassFlags = uFlags; }
	U32 IFXAPI GetRenderPassFlags() const { return m_uRenderPassFlags; }

protected:
	CIFXShader();
	virtual ~CIFXShader();

	IFXList<IFXShadedElement> m_shadedElementList;

	U32	m_uRenderPassFlags;
};

#endif
