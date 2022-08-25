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
//  Data types
//
//  ErrReturns
//      These are the values returned from main when an error occurs. It is
//      also used to throw errors out to the main for return.
//
//  OutFormts
//      The available output formats. This is mapped to from the /OutFmt=
//      command line parameter.
// ---------------------------------------------------------------------------
enum ErrReturns
{
    ErrReturn_Success               = 0
    , ErrReturn_BadParameters       = 1
    , ErrReturn_OutFileOpenFailed   = 4
    , ErrReturn_ParserInit          = 5
    , ErrReturn_ParseErr            = 6
    , ErrReturn_LocaleErr           = 7
    , ErrReturn_NoTranscoder        = 8
    , ErrReturn_SrcFmtError         = 9
    , ErrReturn_UnknownDomain       = 10
    , ErrReturn_Internal            = 9999
};

enum OutFormats
{
    OutFormat_Unknown
    , OutFormat_CppSrc
    , OutFormat_ResBundle
    , OutFormat_Win32RC
    , OutFormat_MsgCatalog
};
