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
	@file	IFXSchedulerTypes.h
			This module defines all scheduler-related things that are not
			specifically part of some class or another.
*/

#ifndef __IFXSchedulerTypes_H__
#define __IFXSchedulerTypes_H__

#include "IFXResult.h"

//-------------------------------------------------------------------
// Data Types
//-------------------------------------------------------------------

/**
	A handle that uniquely identifies a task and an object registered
	with one of the Scheduler's various managers.  It may be used to
	perform an unregistration.  It should not be used as a pointer.
*/
typedef U32 IFXTaskHandle;

/**
	This value denotes an invalid task handle.
*/
#define IFXTASK_HANDLE_INVALID 0

//-------------------------------------------------------------------
// Return Codes
//-------------------------------------------------------------------

#define IFX_E_VIEW_NEEDED             MAKE_IFXRESULT_FAIL(IFXRESULT_COMPONENT_SCHEDULER, 0x7000)
#define IFX_E_OBJECT_MOUSE_EVENT      MAKE_IFXRESULT_FAIL(IFXRESULT_COMPONENT_SCHEDULER, 0x7001)
#define IFX_E_OBJECT_JOYSTICK_EVENT   MAKE_IFXRESULT_FAIL(IFXRESULT_COMPONENT_SCHEDULER, 0x7001)
#define IFX_E_TIME_PERIOD             MAKE_IFXRESULT_FAIL(IFXRESULT_COMPONENT_SCHEDULER, 0x7002)
#define IFX_E_TIME_START              MAKE_IFXRESULT_FAIL(IFXRESULT_COMPONENT_SCHEDULER, 0x7003)
#define IFX_E_TIME_UNITS              MAKE_IFXRESULT_FAIL(IFXRESULT_COMPONENT_SCHEDULER, 0x7004)
#define IFX_E_ROOT_NODE               MAKE_IFXRESULT_FAIL(IFXRESULT_COMPONENT_SCHEDULER, 0x7005)
#define IFX_E_NO_ATTRIBUTE            MAKE_IFXRESULT_FAIL(IFXRESULT_COMPONENT_SCHEDULER, 0x7006)
#define IFX_E_INVALID_FLAGS           MAKE_IFXRESULT_FAIL(IFXRESULT_COMPONENT_SCHEDULER, 0x7007)
#define IFX_E_NO_SCRIPT_HANDLER_FOUND MAKE_IFXRESULT_FAIL(IFXRESULT_COMPONENT_SCHEDULER, 0x7009)
#define IFX_E_NO_SCRIPT_OBJECT_FOUND  MAKE_IFXRESULT_FAIL(IFXRESULT_COMPONENT_SCHEDULER, 0x700a)
#define IFX_E_ATTRIBUTE_COMPONENT     MAKE_IFXRESULT_FAIL(IFXRESULT_COMPONENT_SCHEDULER, 0x700b)

#endif
