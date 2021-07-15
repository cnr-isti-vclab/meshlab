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
//  Includes
// ---------------------------------------------------------------------------
#include "Xlat.hpp"



// ---------------------------------------------------------------------------
//  MsgCatFormatter: Constructors and Destructor
// ---------------------------------------------------------------------------
MsgCatFormatter::MsgCatFormatter() :

    fOutFl(0)
    , fTranscoder(0)
{
    //
    //  Try to create a transcoder for the format that we were told
    //  to output in.
    //
    //  <TBD> Right now we are just using an LCP transcoding, which is not
    //  really the right thing to do!
    //
    fTranscoder = XMLPlatformUtils::fgTransService->makeNewLCPTranscoder(XMLPlatformUtils::fgMemoryManager);
    if (!fTranscoder)
    {
        wprintf(L"Could not create LCP transcoder\n");
        throw ErrReturn_NoTranscoder;
    }
}

MsgCatFormatter::~MsgCatFormatter()
{
	delete fTranscoder;
}


// ---------------------------------------------------------------------------
//  MsgCatFormatter: Implementation of the formatter interface
// ---------------------------------------------------------------------------
void MsgCatFormatter::endDomain(const   XMLCh* const    domainName
                                , const unsigned int    msgCount)
{
    fwprintf(fOutFl, L"\n\n");
}


void MsgCatFormatter::endMsgType(const MsgTypes type)
{
}


void MsgCatFormatter::endOutput()
{
    fwprintf
    (
        fOutHpp
        , L"\nXERCES_CPP_NAMESPACE_END\n\n"
    );

    // Close the output files
    fclose(fOutFl);
    fclose(fOutHpp);
}

void
MsgCatFormatter::nextMessage(const  XMLCh* const            msgText
                            , const XMLCh* const            msgId
                            , const unsigned int            messageId
                            , const unsigned int            curId)
{
    //
    //  We have to transcode the message text to UTF-8 in order to be able
    //  to write it out to a message catalog (which is not Unicode enabled.)
    //  If the text is just US-ASCII, this won't have any effect, but don't
    //  bother checking, just do it simple and stupid.
    //
    //
    // on hp, it is required that message line shall start with number
    //        w/o leading space.
    //
    fwprintf(fOutFl, L"%d  %s\n", curId, xmlStrToPrintable(msgText));
    releasePrintableStr
}


void MsgCatFormatter::startDomain(  const   XMLCh* const    domainName
                                    , const XMLCh* const    nameSpace)
{
    // Output a constant to the header file
    fwprintf(fOutHpp, L"const unsigned int CatId_%s = %d;\n", xmlStrToPrintable(nameSpace), fSeqId);
    releasePrintableStr

    //
    //  Output the leading part of the array declaration. Its just an
    //  array of pointers to Unicode chars.
    //
    fwprintf(fOutFl, L"$set %u\n", fSeqId);

    // And bump the sequence id
    fSeqId++;
}


void MsgCatFormatter::startMsgType(const MsgTypes type)
{
}


void MsgCatFormatter::startOutput(  const   XMLCh* const    locale
                                    , const XMLCh* const    outPath)
{
    //
    //  Ok, lets try to open the the output file. All of the messages
    //  for all the domains are put into a single Msg file, which can be
    //  compiled into the program.
    //
    //  CppErrMsgs_xxxx.Msg
    //
    //  where xxx is the locale suffix passed in.
    //
    const unsigned int bufSize = 4095;
    XMLCh *tmpBuf = new XMLCh[bufSize + 1];
    tmpBuf[0] = 0;
    XMLCh *tmpXMLStr = XMLString::transcode("XercesMessages_");
    XMLCh *tmpXMLStr2 = XMLString::transcode(".Msg");

    XMLString::catString(tmpBuf, outPath);
    XMLString::catString(tmpBuf, tmpXMLStr );
    XMLString::catString(tmpBuf, locale);
    XMLString::catString(tmpBuf, tmpXMLStr2 );
    XMLString::release(&tmpXMLStr);
    XMLString::release(&tmpXMLStr2);
    char *tmpStr = XMLString::transcode(tmpBuf);
    fOutFl = fopen(tmpStr, "wt");
    XMLString::release(&tmpStr);
    if ((!fOutFl) ||(fwide(fOutFl, 1) < 0))
    {        
        wprintf(L"Could not open the output file: %s\n\n", xmlStrToPrintable(tmpBuf) );        
        releasePrintableStr
        XMLString::release(&tmpBuf);
        throw ErrReturn_OutFileOpenFailed;
    }

    // Set the message delimiter
    fwprintf(fOutFl, L"$quote \"\n");


    delete tmpBuf;
    tmpBuf = new XMLCh[bufSize + 1];
    tmpBuf[0] = 0;
    tmpXMLStr = XMLString::transcode("XMLMsgCat_Ids.hpp");
    XMLString::catString(tmpBuf, outPath);
    XMLString::catString(tmpBuf, tmpXMLStr );
    XMLString::release(&tmpXMLStr);
    tmpStr = XMLString::transcode(tmpBuf);
    fOutHpp = fopen(tmpStr, "wt");
    XMLString::release(&tmpStr);
    if ((!fOutHpp) || (fwide(fOutHpp,1) < 0))
    {        
        wprintf(L"Could not open the output file: %s\n\n", xmlStrToPrintable(tmpBuf) );        
        releasePrintableStr
        XMLString::release(&tmpBuf);
        throw ErrReturn_OutFileOpenFailed;
    }

    fwprintf
    (
        fOutHpp
        , L"// ----------------------------------------------------------------\n"
          L"//  This file was generated from the XML error message source.\n"
          L"//  so do not edit this file directly!!\n"
          L"// ----------------------------------------------------------------\n\n"
          L"#include <xercesc/util/XercesDefs.hpp>\n\n"
          L"XERCES_CPP_NAMESPACE_BEGIN\n\n"
    );

    // Reset the sequence id
    fSeqId = 1;
    delete tmpBuf;
}
