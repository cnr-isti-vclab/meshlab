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
//  ICUResBundFormatter: Constructors and Destructor
// ---------------------------------------------------------------------------
ICUResBundFormatter::ICUResBundFormatter()
:fOutFl(0)
,fTranscoder(0)
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

ICUResBundFormatter::~ICUResBundFormatter()
{
	delete fTranscoder;
}

// ---------------------------------------------------------------------------
//  ICUResBundFormatter: Implementation of the formatter interface
// ---------------------------------------------------------------------------
void ICUResBundFormatter::endDomain(const   XMLCh*        const    domainName
                                  , const   unsigned int           msgCount)
{
    fwprintf(fOutFl, L"\t\t} \n\n");
}


void ICUResBundFormatter::endMsgType(const MsgTypes type)
{
#ifndef ICU_RESBUNDLE_IN_TABLE_FORM
     fwprintf(fOutFl, L"\t\t\"%s End \" ,\n", xmlStrToPrintable(typePrefixes[type]) );
     releasePrintableStr
#endif
}


void ICUResBundFormatter::endOutput()
{
    // Close the output files
    fwprintf(fOutFl, L"} \n\n");
    fclose(fOutFl);
}

void
ICUResBundFormatter::nextMessage(const  XMLCh* const            msgText
                               , const  XMLCh* const            msgId
                               , const  unsigned int            messageId
                               , const  unsigned int            curId)
{
    //
    //  We have to transcode the message text to UTF-8 in order to be able
    //  to write it out to a message catalog (which is not Unicode enabled.)
    //  If the text is just US-ASCII, this won't have any effect, but don't
    //  bother checking, just do it simple and stupid.
    //

    //
	//  array_name {
	//               "xxx" ,
	//               "yyy" ,
	//  }
	//
	//
	//  table_name {
	//       AB { "xxx" }
	//       CD { "yyy" }
	//  }
	//

#ifdef ICU_RESBUNDLE_IN_TABLE_FORM
    fwprintf(fOutFl, L"\t\t %3d { \"%s\" } \n", curId, xmlStrToPrintable(msgText));
    releasePrintableStr
	// need to print leading 0 if less than 100, not tested yet
#else
    fwprintf(fOutFl, L"\t\t\"%s\" ,\n", xmlStrToPrintable(msgText));
    releasePrintableStr
	// need a space between the last character and the closing "
#endif

}


void ICUResBundFormatter::startDomain(const   XMLCh* const    domainName
                                    , const   XMLCh* const    nameSpace)
{
    //
    //  Output:
    //           // an array
    //           domainName {
	//
	int index = XMLString::lastIndexOf(domainName, chForwardSlash);

#ifdef ICU_RESBUNDLE_IN_TABLE_FORM
    fwprintf(fOutFl, L"\n\t // a table \n");
#else
    fwprintf(fOutFl, L"\n\t // an array \n");
#endif

    fwprintf(fOutFl, L"\t%s { \n" , xmlStrToPrintable(&(domainName[index+1])));
    releasePrintableStr

}


void ICUResBundFormatter::startMsgType(const MsgTypes type)
{
#ifndef ICU_RESBUNDLE_IN_TABLE_FORM
    fwprintf(fOutFl, L"\t\t\"%s Start \" , \n", xmlStrToPrintable(typePrefixes[type]) );
    releasePrintableStr
#endif
}


void ICUResBundFormatter::startOutput(const   XMLCh* const    msgLocale
                                    , const   XMLCh* const    outPath)
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
    XMLCh tmpBuf[bufSize + 1];
    tmpBuf[0] = 0;
    XMLCh *tmpXMLStr = XMLString::transcode(".txt");

    // ICU Resource Bundles now uses "root" as locale
    XMLCh* locale = XMLString::transcode ("root");
    XMLString::catString(tmpBuf, outPath);
    XMLString::catString(tmpBuf, locale);
    XMLString::catString(tmpBuf, tmpXMLStr );
    XMLString::release(&tmpXMLStr);
    char *tmpStr = XMLString::transcode(tmpBuf);
    fOutFl = fopen(tmpStr, "wt");
    XMLString::release(&tmpStr);
    if ((!fOutFl) || (fwide(fOutFl, 1) < 0))
    {        
        wprintf(L"Could not open the output file: %s\n\n", xmlStrToPrintable(tmpBuf) );        
        releasePrintableStr
        throw ErrReturn_OutFileOpenFailed;
    }

    // Set the message delimiter
    fwprintf(fOutFl, L"%s { \n", xmlStrToPrintable(locale) );
    releasePrintableStr
    XMLString::release(&locale);
}
