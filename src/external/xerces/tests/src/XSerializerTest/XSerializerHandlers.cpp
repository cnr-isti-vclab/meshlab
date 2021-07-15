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
#include "XSerializerTest.hpp"

#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/SAXException.hpp>

// ---------------------------------------------------------------------------
//  XSerializerHandlers: Constructors and Destructor
// ---------------------------------------------------------------------------
XSerializerHandlers::XSerializerHandlers() :

    fAttrCount(0)
    , fCharacterCount(0)
    , fElementCount(0)
    , fSpaceCount(0)
    , fSawErrors(false)
{
}

XSerializerHandlers::~XSerializerHandlers()
{
}

// ---------------------------------------------------------------------------
//  XSerializerHandlers: Implementation of the SAX DocumentHandler interface
// ---------------------------------------------------------------------------
void XSerializerHandlers::startElement(const XMLCh* const /*uri*/
                                   , const XMLCh* const /*localname*/
                                   , const XMLCh* const /*qname*/
                                   , const Attributes& attrs)
{
    fElementCount++;
    fAttrCount += attrs.getLength();
}

void XSerializerHandlers::characters(  const   XMLCh* const  /*chars*/
                                     , const XMLSize_t       length)
{
    fCharacterCount += length;
}

void XSerializerHandlers::ignorableWhitespace( const   XMLCh* const /*chars*/
                                             , const XMLSize_t length)
{
    fSpaceCount += length;
}

void XSerializerHandlers::resetDocument()
{
    fAttrCount = 0;
    fCharacterCount = 0;
    fElementCount = 0;
    fSpaceCount = 0;
}


// ---------------------------------------------------------------------------
//  XSerializerHandlers: Overrides of the SAX ErrorHandler interface
// ---------------------------------------------------------------------------
void XSerializerHandlers::error(const SAXParseException& e)
{
    fSawErrors = true;
    XERCES_STD_QUALIFIER cerr << "\nError at file " << StrX(e.getSystemId())
		 << ", line " << e.getLineNumber()
		 << ", char " << e.getColumnNumber()
         << "\n  Message: " << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;
}

void XSerializerHandlers::fatalError(const SAXParseException& e)
{
    fSawErrors = true;
    XERCES_STD_QUALIFIER cerr << "\nFatal Error at file " << StrX(e.getSystemId())
		 << ", line " << e.getLineNumber()
		 << ", char " << e.getColumnNumber()
         << "\n  Message: " << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;
}

void XSerializerHandlers::warning(const SAXParseException& e)
{
    XERCES_STD_QUALIFIER cerr << "\nWarning at file " << StrX(e.getSystemId())
		 << ", line " << e.getLineNumber()
		 << ", char " << e.getColumnNumber()
         << "\n  Message: " << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;
}

void XSerializerHandlers::resetErrors()
{
    fSawErrors = false;
}
