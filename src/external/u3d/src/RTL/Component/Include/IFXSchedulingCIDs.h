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
  @file IFXSchedulingCIDs.h

      This header defines all of the CIDs that identify the various
      components contained in the Scheduling DL.
*/

#ifndef IFXSchedulingCIDs_H
#define IFXSchedulingCIDs_H

//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXGUID.h"

//***************************************************************************
//  Component identifiers
//***************************************************************************

//---------------------------------------------------------------------------
//	CID_IFXSimulationInfo
//
//	This component supports the following interfaces:  IFXUnknown, IFXTaskData and
//	IFXSimulationInfo.  It is not a singleton.
//---------------------------------------------------------------------------
// {CD489FA7-A633-11d5-9AE3-00D0B73FB755}
IFXDEFINE_GUID(CID_IFXSimulationInfo,
0xcd489fa7, 0xa633, 0x11d5, 0x9a, 0xe3, 0x0, 0xd0, 0xb7, 0x3f, 0xb7, 0x55);

//---------------------------------------------------------------------------
//	CID_IFXNotificationInfo
//
//	This component supports the following interfaces:  IFXUnknown, IFXTaskData and
//	IFXNotificationInfo.  It is not a singleton.
//---------------------------------------------------------------------------
// {CD489FA5-A633-11d5-9AE3-00D0B73FB755}
IFXDEFINE_GUID(CID_IFXNotificationInfo,
0xcd489fa5, 0xa633, 0x11d5, 0x9a, 0xe3, 0x0, 0xd0, 0xb7, 0x3f, 0xb7, 0x55);

//---------------------------------------------------------------------------
//	CID_IFXNotificationManager
//
//	This component supports the following interfaces:  IFXUnknown, IFXTask and
//	IFXNotificationManager.  It is not a singleton.
//---------------------------------------------------------------------------
// {AD4A0941-B21A-11d5-9AE4-00D0B73FB755}
IFXDEFINE_GUID(CID_IFXNotificationManager,
0xad4a0941, 0xb21a, 0x11d5, 0x9a, 0xe4, 0x0, 0xd0, 0xb7, 0x3f, 0xb7, 0x55);

//---------------------------------------------------------------------------
//	CID_IFXSchedulerInfo
//
//	This component supports the following interfaces:  IFXUnknown, IFXTaskData and
//	IFXSchedulerInfo.  It is not a singleton.
//---------------------------------------------------------------------------
// {CD489FA6-A633-11d5-9AE3-00D0B73FB755}
IFXDEFINE_GUID(CID_IFXSchedulerInfo,
0xcd489fa6, 0xa633, 0x11d5, 0x9a, 0xe3, 0x0, 0xd0, 0xb7, 0x3f, 0xb7, 0x55);

//---------------------------------------------------------------------------
//	CID_IFXSimulationManager
//
//	This component supports the following interfaces:  IFXUnknown, IFXTask and
//	IFXSimulationManager.  It is not a singleton.
//---------------------------------------------------------------------------
// {CD489FA0-A633-11d5-9AE3-00D0B73FB755}
IFXDEFINE_GUID(CID_IFXSimulationManager,
0xcd489fa0, 0xa633, 0x11d5, 0x9a, 0xe3, 0x0, 0xd0, 0xb7, 0x3f, 0xb7, 0x55);

//---------------------------------------------------------------------------
//	CID_IFXSystemManager
//
//	This component supports the following interfaces:  IFXUnknown, IFXTask and
//	IFXSystemManager.  It is not a singleton.
//---------------------------------------------------------------------------
// {AD4A0945-B21A-11d5-9AE4-00D0B73FB755}
IFXDEFINE_GUID(CID_IFXSystemManager,
0xad4a0945, 0xb21a, 0x11d5, 0x9a, 0xe4, 0x0, 0xd0, 0xb7, 0x3f, 0xb7, 0x55);

//---------------------------------------------------------------------------
//	CID_IFXSystemInfo
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXSystemInfo.  It is a singleton.
//---------------------------------------------------------------------------
// {33ACB9A3-9772-11d3-A886-00A0C9779AE4}
//IFXDEFINE_GUID(CID_IFXSystemInfo,
//0x33acb9a3, 0x9772, 0x11d3, 0xa8, 0x86, 0x0, 0xa0, 0xc9, 0x77, 0x9a, 0xe4);

//---------------------------------------------------------------------------
//	CID_IFXScheduler
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXScheduler.  It is not a singleton.
//---------------------------------------------------------------------------
// {973D75E1-9B9B-11d3-A924-00A0C977EB2F}
IFXDEFINE_GUID(CID_IFXScheduler,
0x973d75e1, 0x9b9b, 0x11d3, 0xa9, 0x24, 0x0, 0xa0, 0xc9, 0x77, 0xeb, 0x2f);

//---------------------------------------------------------------------------
//	CID_IFXTaskData
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXTaskData.  It is not a singleton.
//---------------------------------------------------------------------------
// {CD489FA3-A633-11d5-9AE3-00D0B73FB755}
IFXDEFINE_GUID(CID_IFXTaskData,
0xcd489fa3, 0xa633, 0x11d5, 0x9a, 0xe3, 0x0, 0xd0, 0xb7, 0x3f, 0xb7, 0x55);

//---------------------------------------------------------------------------
//	CID_IFXTaskManager
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXTaskManager.  It is not a singleton.
//---------------------------------------------------------------------------
// {AD4A0949-B21A-11d5-9AE4-00D0B73FB755}
IFXDEFINE_GUID(CID_IFXTaskManager,
0xad4a0949, 0xb21a, 0x11d5, 0x9a, 0xe4, 0x0, 0xd0, 0xb7, 0x3f, 0xb7, 0x55);

//---------------------------------------------------------------------------
//	CID_IFXTaskManagerNode
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXTaskManagerNode.  It is not a singleton.
//---------------------------------------------------------------------------
// {5D9997B0-BC3A-11d5-9AE6-00D0B73FB755}
IFXDEFINE_GUID(CID_IFXTaskManagerNode,
0x5d9997b0, 0xbc3a, 0x11d5, 0x9a, 0xe6, 0x0, 0xd0, 0xb7, 0x3f, 0xb7, 0x55);

//---------------------------------------------------------------------------
//	CID_IFXTaskManagerView
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXTaskManagerView.  It is not a singleton.
//---------------------------------------------------------------------------
// {5D9997B2-BC3A-11d5-9AE6-00D0B73FB755}
IFXDEFINE_GUID(CID_IFXTaskManagerView,
0x5d9997b2, 0xbc3a, 0x11d5, 0x9a, 0xe6, 0x0, 0xd0, 0xb7, 0x3f, 0xb7, 0x55);

//---------------------------------------------------------------------------
//	CID_IFXTimeManager
//
//	This component supports the following interfaces:  IFXUnknown, IFXTask and
//	IFXTimeManager.  It is not a singleton.
//---------------------------------------------------------------------------
// {AD4A0943-B21A-11d5-9AE4-00D0B73FB755}
IFXDEFINE_GUID(CID_IFXTimeManager,
0xad4a0943, 0xb21a, 0x11d5, 0x9a, 0xe4, 0x0, 0xd0, 0xb7, 0x3f, 0xb7, 0x55);

//---------------------------------------------------------------------------
//	CID_IFXErrorInfo
//
//	This component supports the following interfaces:  IFXUnknown, IFXTaskData and
//	IFXErrorInfo.  It is not a singleton.
//---------------------------------------------------------------------------
// {CD489FA4-A633-11d5-9AE3-00D0B73FB755}
IFXDEFINE_GUID(CID_IFXErrorInfo,
0xcd489fa4, 0xa633, 0x11d5, 0x9a, 0xe3, 0x0, 0xd0, 0xb7, 0x3f, 0xb7, 0x55);

//---------------------------------------------------------------------------
//	CID_IFXClock
//
//	This component supports the following interfaces:  IFXUnknown and
//	IFXClock.  It is not a singleton.
//---------------------------------------------------------------------------
// {AD4A0947-B21A-11d5-9AE4-00D0B73FB755}
IFXDEFINE_GUID(CID_IFXClock,
0xad4a0947, 0xb21a, 0x11d5, 0x9a, 0xe4, 0x0, 0xd0, 0xb7, 0x3f, 0xb7, 0x55);

//---------------------------------------------------------------------------
/**
	This component supports the following interfaces:  IFXUnknown, IFXTask 
	and IFXTaskCallback.  It is not a singleton.

	@note	The GUID string is {217AB4BD-EFD2-49ac-9D7E-50D60F59AE18}.
*/
IFXDEFINE_GUID(CID_IFXTaskCallback, 
0x217ab4bd, 0xefd2, 0x49ac, 0x9d, 0x7e, 0x50, 0xd6, 0xf, 0x59, 0xae, 0x18);


#endif


