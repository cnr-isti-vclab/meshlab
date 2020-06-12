//***************************************************************************
//
//  Copyright (c) 2001 - 2006 Intel Corporation
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
// CIFXOpenGL.h

#ifndef CIFX_OPENGL_H
#define CIFX_OPENGL_H

#include "IFXOpenGL.h"

class CIFXOpenGL : virtual public IFXOpenGL, virtual public IFXUnknown
{
public:
	IFXRESULT IFXAPI LoadExtensions();

protected:
	CIFXOpenGL() {}
	virtual ~CIFXOpenGL() {}

	IFXRESULT IFXAPI Construct();

	virtual IFXOGLPROC IFXAPI GetOGLExtensionFunc(const char* szFuncName);
};

#endif
