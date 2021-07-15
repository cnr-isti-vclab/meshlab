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
#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/SAXException.hpp>
#include "StdInParse.hpp"


// ---------------------------------------------------------------------------
//  StdInParseHandlers: Constructors and Destructor
// ---------------------------------------------------------------------------
StdInParseHandlers::StdInParseHandlers() :

    fAttrCount(0)
    , fCharacterCount(0)
    , fElementCount(0)
    , fSpaceCount(0)
{
}

StdInParseHandlers::~StdInParseHandlers()
{
}


// ---------------------------------------------------------------------------
//  StdInParseHandlers: Implementation of the SAX DocumentHandler interface
// ---------------------------------------------------------------------------
void StdInParseHandlers::endElement(const XMLCh* const /* name */)
{
}

void
StdInParseHandlers::startElement(   const   XMLCh* const    /* name */
                                    ,       AttributeList&  attributes)
{
    fElementCount++;
    fAttrCount += attributes.getLength();
}

void StdInParseHandlers::characters(const   XMLCh* const    /* chars */
								    , const XMLSize_t      length)
{
    fCharacterCount += length;
}

void StdInParseHandlers::ignorableWhitespace(const  XMLCh* const /* chars */
										    , const XMLSize_t    length)
{
    fSpaceCount += length;
}

void StdInParseHandlers::resetDocument()
{
    fAttrCount = 0;
    fCharacterCount = 0;
    fElementCount = 0;
    fSpaceCount = 0;
}



// ---------------------------------------------------------------------------
//  StdInParseHandlers: Overrides of the SAX ErrorHandler interface
// ---------------------------------------------------------------------------
void StdInParseHandlers::error(const SAXParseException& e)
{
    XERCES_STD_QUALIFIER cerr << "\nError at (file " << StrX(e.getSystemId())
		 << ", line " << e.getLineNumber()
		 << ", char " << e.getColumnNumber()
         << "): " << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;
}

void StdInParseHandlers::fatalError(const SAXParseException& e)
{
    XERCES_STD_QUALIFIER cerr << "\nFatal Error at (file " << StrX(e.getSystemId())
		 << ", line " << e.getLineNumber()
		 << ", char " << e.getColumnNumber()
         << "): " << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;
}

void StdInParseHandlers::warning(const SAXParseException& e)
{
    XERCES_STD_QUALIFIER cerr << "\nWarning at (file " << StrX(e.getSystemId())
		 << ", line " << e.getLineNumber()
		 << ", char " << e.getColumnNumber()
         << "): " << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;
}
