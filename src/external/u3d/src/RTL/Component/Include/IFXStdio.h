//***************************************************************************
//
//  Copyright (c) 2000 - 2006 Intel Corporation
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
	@file IFXStdio.h
	Declaration of IFXStdio interface.  This interface is used to open and close files using stdio.
*/

//*****************************************************************************
#ifndef IFXSTDIO_H__
#define IFXSTDIO_H__

#include "IFXUnknown.h"

// {4AC57844-EB76-11d3-9519-00A0C90AB136}
IFXDEFINE_GUID(IID_IFXStdio,
0x4ac57844, 0xeb76, 0x11d3, 0x95, 0x19, 0x0, 0xa0, 0xc9, 0xa, 0xb1, 0x36);

/// IFXStdio is used to open and close files using stdio.
class IFXStdio : virtual public IFXUnknown
{
public:
	/// Opens the file
	/// @param pFilename The name of the file to be opened.
	/** @note Opening for reading or writing or appending,
		opening in binary or text mode,
		or creating are all undefined and implementation specific.
		Refer to the documentation of any class implementing IFXStdio for details.
	*/
	virtual IFXRESULT IFXAPI  Open(const IFXCHAR *pFilename) = 0;
	/// Closes the file.
	virtual IFXRESULT IFXAPI  Close() = 0;
};

#endif
