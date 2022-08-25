/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * $Id$
 */

// ---------------------------------------------------------------------------
//  Some globally used types
// ---------------------------------------------------------------------------
enum MsgTypes
{
    MsgType_Warning
    , MsgType_Error
    , MsgType_FatalError

    , MsgTypes_Count
};


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>

#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include "Xlat_ErrHandler.hpp"
#include "Xlat_Types.hpp"
#include "Xlat_Formatter.hpp"
#include "Xlat_CppSrc.hpp"
#include "Xlat_Win32RC.hpp"
#include "Xlat_MsgCatalog.hpp"
#include "Xlat_ICUResourceBundle.hpp"

XERCES_CPP_NAMESPACE_USE


// ---------------------------------------------------------------------------
//  Some const global data
// ---------------------------------------------------------------------------
extern XMLCh* typePrefixes[MsgTypes_Count];


// this ugly hack is needed because cygwin/linux and Windows (MSVC++) 
// have irreconcileable differences about what to do with chars, wchar_t and XMLCh
// in wfprintf.  Windows thinks that XMLCh * is fine here whereas 
// char * is not; gcc will allow XMLCh to be cast to wchar_t but happily
// prints out gobbledygook in this case; it only seems happy when 
// the native transcoder is used to convert the XMLCh to a char *
#if defined(__linux__) || defined(__CYGWIN__)
    extern char *fTmpStr;
    #define xmlStrToPrintable(xmlStr) \
        (fTmpStr = XMLString::transcode(xmlStr))
    #define releasePrintableStr \
        XMLString::release(&fTmpStr);
    #define longChars(str) str
#elif defined(_WIN32) || defined(WIN32) || defined(__WINDOWS__)
    extern wchar_t *longChars(const char *str);
    #define xmlStrToPrintable(xmlStr) xmlStr
    #define releasePrintableStr 
#else
    #error Code requires port to host OS!
#endif

