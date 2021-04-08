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
//  This is a simple abstract API that each formatter must provide. This is
//  how the main program logic calls out with all of the needed information
//  and events needed to create the output.
// ---------------------------------------------------------------------------
class XlatFormatter
{
public :
    // -----------------------------------------------------------------------
    //  Public Constructors and Destructor
    // -----------------------------------------------------------------------
    virtual ~XlatFormatter()
    {
    }


    // -----------------------------------------------------------------------
    //  Virtual formatter interface
    // -----------------------------------------------------------------------
    virtual void endDomain
    (
        const   XMLCh* const    domainName
        , const unsigned int    msgCount
    ) = 0;

    virtual void endMsgType
    (
        const   MsgTypes        type
    ) = 0;

    virtual void endOutput() = 0;

    virtual void nextMessage
    (
        const   XMLCh* const    msgText
        , const XMLCh* const    msgId
        , const unsigned int    messageId
        , const unsigned int    curId
    ) = 0;

    virtual void startDomain
    (
        const   XMLCh* const    domainName
        , const XMLCh* const    nameSpace
    ) = 0;

    virtual void startMsgType
    (
        const   MsgTypes        type
    ) = 0;

    virtual void startOutput
    (
        const   XMLCh* const locale
        , const XMLCh* const outPath
    ) = 0;


protected :
    // -----------------------------------------------------------------------
    //  Hidden constructors
    // -----------------------------------------------------------------------
    XlatFormatter()
    {
    }


private :
    // -----------------------------------------------------------------------
    //  Unimplemented constructors and operators
    // -----------------------------------------------------------------------
    XlatFormatter(const XlatFormatter&);
    void operator=(const XlatFormatter&);
};
