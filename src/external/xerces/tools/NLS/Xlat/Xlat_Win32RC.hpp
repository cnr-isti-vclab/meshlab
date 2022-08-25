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


class Win32RCFormatter : public XlatFormatter
{
public :
    // -----------------------------------------------------------------------
    //  Public Constructors and Destructor
    // -----------------------------------------------------------------------
    Win32RCFormatter();
    virtual ~Win32RCFormatter();


    // -----------------------------------------------------------------------
    //  Implementation of the formatter interface
    // -----------------------------------------------------------------------
    virtual void endDomain
    (
        const   XMLCh* const    domainName
        , const unsigned int    msgCount
    );

    virtual void endMsgType
    (
        const   MsgTypes        type
    );

    virtual void endOutput();

    virtual void nextMessage
    (
        const   XMLCh* const    msgText
        , const XMLCh* const    msgId
        , const unsigned int    messageId
        , const unsigned int    curId
    );

    virtual void startDomain
    (
        const   XMLCh* const    domainName
        , const XMLCh* const    nameSpace
    );

    virtual void startMsgType
    (
        const   MsgTypes        type
    );

    virtual void startOutput
    (
        const   XMLCh* const locale
        , const XMLCh* const outPath
    );


private :
    // -----------------------------------------------------------------------
    //  Unimplemented constructors and operators
    // -----------------------------------------------------------------------
    Win32RCFormatter(const Win32RCFormatter&);
    void operator=(const Win32RCFormatter&);


    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  fCurDomainName
    //      This is the short name for the current domain being processed.
    //      This is used to create the names of information we write to the
    //      file.
    //
    //  fMsgOffset
    //      Since we are going to put all the messages into a single message
    //      table, we have to provide an offset for each domain (since they
    //      each think that they have a unique numbering range.) So we just
    //      offset each new domain by 0x2000 to keep them separate. When the
    //      numbers are passed into the Win32RCMsgLoader, it will offset the
    //      incoming message ids by the appropriate amount.
    //
    //  fOutFl
    //      This is the current output file. Its created when a new domain
    //      is begun via startDomain() and closed when endDomain is called.
    // -----------------------------------------------------------------------
    XMLCh*          fCurDomainName;
    unsigned int    fMsgOffset;
    FILE*           fOutFl;
};
