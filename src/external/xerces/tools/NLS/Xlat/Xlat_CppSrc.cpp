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
//  CppSrcFormatter: Implementation of the formatter interface
// ---------------------------------------------------------------------------
void CppSrcFormatter::endDomain(const   XMLCh* const    domainName
                                , const unsigned int    msgCount)
{
    // And close out the array declaration
    fwprintf(fOutFl, L"\n};\n");

    // Output the const size value
    fwprintf(fOutFl, L"const unsigned int %s%s = %d;\n\n", xmlStrToPrintable(fCurDomainName), longChars("Size"), msgCount);
    releasePrintableStr
}


void CppSrcFormatter::endMsgType(const MsgTypes type)
{
    if (fFirst)
    {
        fwprintf(fOutFl, L"    { ");
        fFirst = false;
    }
     else
    {
        fwprintf(fOutFl, L"  , { ");
    }

    XMLCh* rawData = typePrefixes[type];
    while (*rawData)
        fwprintf(fOutFl, L"0x%04lX,", *rawData++);

    XMLCh* tmpXMLStr = rawData = XMLString::transcode("End");
    while (*rawData)
        fwprintf(fOutFl, L"0x%04lX,", *rawData++);
    XMLString::release(&tmpXMLStr);
    fwprintf(fOutFl, L"0x00 }\n");
}


void CppSrcFormatter::endOutput()
{
    fwprintf
    (
        fOutFl,
          L"XERCES_CPP_NAMESPACE_END\n\n"
    );
    // Close the output file
    fclose(fOutFl);
}

void
CppSrcFormatter::nextMessage(const  XMLCh* const            msgText
                            , const XMLCh* const            msgId
                            , const unsigned int            messageId
                            , const unsigned int            curId)
{
    //
    //  We do not transcode to the output format in this case. Instead we
    //  just store the straight Unicode format. Because we cannot assume 'L'
    //  type prefix support, we have to put them out as numeric character
    //  values.
    //
    const XMLCh* rawData = msgText;
    if (fFirst)
    {
        fwprintf(fOutFl, L"    { ");
        fFirst = false;
    }
     else
    {
        fwprintf(fOutFl, L"  , { ");
    }

    unsigned int i = 0;
    while (*rawData) {
        fwprintf(fOutFl, L"0x%04lX,", *rawData++);
        if (++i == 35) {
            i = 0;
            fwprintf(fOutFl, L"\n");
            fwprintf(fOutFl, L"      ");
        }
    }
    fwprintf(fOutFl, L"0x00 }\n");
}


void CppSrcFormatter::startDomain(  const   XMLCh* const    domainName
                                    , const XMLCh* const)
{
    //
    //  We have a different array name for each domain, so store that for
    //  later use and for use below.
    //
    XMLString::release(&fCurDomainName);
    if (!XMLString::compareString(XMLUni::fgXMLErrDomain, domainName))
    {
        fCurDomainName = XMLString::transcode("gXMLErrArray");
    }
     else if (!XMLString::compareString(XMLUni::fgExceptDomain, domainName))
    {
        fCurDomainName = XMLString::transcode("gXMLExceptArray");
    }
     else if (!XMLString::compareString(XMLUni::fgValidityDomain, domainName))
    {
        fCurDomainName = XMLString::transcode("gXMLValidityArray");
    }
     else if (!XMLString::compareString(XMLUni::fgXMLDOMMsgDomain, domainName))
    {
        fCurDomainName = XMLString::transcode("gXMLDOMMsgArray");
    }
     else
    {
        wprintf(L"Unknown message domain: %s\n", domainName);
        throw ErrReturn_SrcFmtError;
    }

    //
    //  Output the leading part of the array declaration. Its just an
    //  array of pointers to Unicode chars.
    //
    fwprintf(fOutFl, L"const XMLCh %s[][128] = \n{\n", xmlStrToPrintable(fCurDomainName));
    releasePrintableStr

    // Reset the first message trigger
    fFirst = true;
}


void CppSrcFormatter::startMsgType(const MsgTypes type)
{
    if (fFirst)
    {
        fwprintf(fOutFl, L"    { ");
        fFirst = false;
    }
     else
    {
        fwprintf(fOutFl, L"  , { ");
    }

    XMLCh* rawData = typePrefixes[type];
    while (*rawData)
        fwprintf(fOutFl, L"0x%04lX,", *rawData++);

    XMLCh *tmpXMLStr = rawData = XMLString::transcode("Start");
    while (*rawData)
        fwprintf(fOutFl, L"0x%04lX,", *rawData++);
    XMLString::release(&tmpXMLStr);

    fwprintf(fOutFl, L"0x00 }\n");
}


void CppSrcFormatter::startOutput(  const   XMLCh* const    locale
                                    , const XMLCh* const    outPath)
{
    //
    //  Ok, lets try to open the the output file. All of the messages
    //  for all the domains are put into a single Cpp file, which can be
    //  compiled into the program.
    //
    //  CppErrMsgs_xxxx.cpp
    //
    //  where xxx is the locale suffix passed in.
    //
    const unsigned int bufSize = 4095;
    XMLCh tmpBuf[bufSize + 1];
    tmpBuf[0] = 0;
    XMLCh *tmpXMLStr = XMLString::transcode("XercesMessages_");
    XMLCh *tmpXMLStr2 = XMLString::transcode(".hpp");

    XMLString::catString(tmpBuf, outPath); 
    XMLString::catString(tmpBuf, tmpXMLStr );
    XMLString::catString(tmpBuf, locale);
    XMLString::catString(tmpBuf, tmpXMLStr2 );
    XMLString::release(&tmpXMLStr);
    XMLString::release(&tmpXMLStr2);
    char *tmpStr = XMLString::transcode(tmpBuf);
    fOutFl = fopen(tmpStr, "wt");
    XMLString::release(&tmpStr);
    if ((!fOutFl) || (fwide(fOutFl, 1) <  0))
    {
        wprintf(L"Could not open the output file: %s\n\n", tmpBuf);
        throw ErrReturn_OutFileOpenFailed;
    }

    //
    //  Ok, lets output the grunt data at the start of the file. We put out a
    //  comment that indicates its a generated file, and the title string.
    //
    fwprintf
    (
        fOutFl
        , L"// ----------------------------------------------------------------\n"
          L"//  This file was generated from the XML error message source.\n"
          L"//  so do not edit this file directly!!\n"
          L"// ----------------------------------------------------------------\n\n"
          L"#include <xercesc/util/XercesDefs.hpp>\n\n"
          L"XERCES_CPP_NAMESPACE_BEGIN\n\n"
    );
}
