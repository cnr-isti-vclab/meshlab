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
#include "MemParse.hpp"
#include <string.h>
#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/SAXException.hpp>



// ---------------------------------------------------------------------------
//  MemParseHandlers: Constructors and Destructor
// ---------------------------------------------------------------------------
MemParseHandlers::MemParseHandlers() :
    fAttrCount(0)
    , fCharacterCount(0)
    , fElementCount(0)
    , fSpaceCount(0)
{
}

MemParseHandlers::~MemParseHandlers()
{
}


// ---------------------------------------------------------------------------
//  MemParseHandlers: Implementation of the SAX DocumentHandler interface
// ---------------------------------------------------------------------------
void MemParseHandlers::startElement(const   XMLCh* const    /* name */
                                    ,       AttributeList&  attributes)
{
    fElementCount++;
    fAttrCount += attributes.getLength();
}

void MemParseHandlers::characters(  const   XMLCh* const    /* chars */
                                    , const XMLSize_t    length)
{
    fCharacterCount += length;
}

void MemParseHandlers::ignorableWhitespace( const   XMLCh* const /* chars */
                                            , const XMLSize_t length)
{
    fSpaceCount += length;
}

void MemParseHandlers::resetDocument()
{
    fAttrCount = 0;
    fCharacterCount = 0;
    fElementCount = 0;
    fSpaceCount = 0;
}



// ---------------------------------------------------------------------------
//  MemParseHandlers: Overrides of the SAX ErrorHandler interface
// ---------------------------------------------------------------------------
void MemParseHandlers::error(const SAXParseException& e)
{
    XERCES_STD_QUALIFIER cerr << "\nError at (file " << StrX(e.getSystemId())
		 << ", line " << e.getLineNumber()
		 << ", char " << e.getColumnNumber()
         << "): " << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;
}

void MemParseHandlers::fatalError(const SAXParseException& e)
{
    XERCES_STD_QUALIFIER cerr << "\nFatal Error at (file " << StrX(e.getSystemId())
		 << ", line " << e.getLineNumber()
		 << ", char " << e.getColumnNumber()
         << "): " << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;
}

void MemParseHandlers::warning(const SAXParseException& e)
{
    XERCES_STD_QUALIFIER cerr << "\nWarning at (file " << StrX(e.getSystemId())
		 << ", line " << e.getLineNumber()
		 << ", char " << e.getColumnNumber()
         << "): " << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;
}


