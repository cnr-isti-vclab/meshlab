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
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include "Redirect.hpp"
#include <string.h>


// ---------------------------------------------------------------------------
//  Local constant data
//
//  gFileToTrap
//      This is the file that we are looking for in the entity handler, to
//      redirect to another file.
//
//  gRedirectToFile
//      This is the file that we are going to redirect the parser to.
// ---------------------------------------------------------------------------
static const XMLCh  gFileToTrap[] =
{
        chLatin_p, chLatin_e, chLatin_r, chLatin_s, chLatin_o, chLatin_n
    ,   chLatin_a, chLatin_l, chPeriod,  chLatin_d, chLatin_t, chLatin_d, chNull
};

static const XMLCh  gRedirectToFile[] =
{
        chLatin_r, chLatin_e, chLatin_d, chLatin_i, chLatin_r, chLatin_e
    ,   chLatin_c, chLatin_t, chPeriod,  chLatin_d, chLatin_t, chLatin_d, chNull
};


// ---------------------------------------------------------------------------
//  RedirectHandlers: Constructors and Destructor
// ---------------------------------------------------------------------------
RedirectHandlers::RedirectHandlers() :

    fAttrCount(0)
    , fCharacterCount(0)
    , fElementCount(0)
    , fSpaceCount(0)
{
}

RedirectHandlers::~RedirectHandlers()
{
}


// ---------------------------------------------------------------------------
//  RedirectHandlers: Implementation of the SAX DocumentHandler interface
// ---------------------------------------------------------------------------
void RedirectHandlers::startElement(const   XMLCh* const    /* name */
                                    ,       AttributeList&  attributes)
{
    fElementCount++;
    fAttrCount += attributes.getLength();
}

void RedirectHandlers::characters(  const   XMLCh* const    /* chars */
                                    , const XMLSize_t    length)
{
    fCharacterCount += length;
}

void RedirectHandlers::ignorableWhitespace( const   XMLCh* const /* chars */
                                            , const XMLSize_t length)
{
    fSpaceCount += length;
}

void RedirectHandlers::resetDocument()
{
    fAttrCount = 0;
    fCharacterCount = 0;
    fElementCount = 0;
    fSpaceCount = 0;
}



// ---------------------------------------------------------------------------
//  RedirectHandlers: Overrides of the SAX ErrorHandler interface
// ---------------------------------------------------------------------------
void RedirectHandlers::error(const SAXParseException& e)
{
    XERCES_STD_QUALIFIER cerr << "\nError at (file " << StrX(e.getSystemId())
		 << ", line " << e.getLineNumber()
		 << ", char " << e.getColumnNumber()
         << "): " << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;
}

void RedirectHandlers::fatalError(const SAXParseException& e)
{
    XERCES_STD_QUALIFIER cerr << "\nFatal Error at (file " << StrX(e.getSystemId())
		 << ", line " << e.getLineNumber()
		 << ", char " << e.getColumnNumber()
         << "): " << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;
}

void RedirectHandlers::warning(const SAXParseException& e)
{
    XERCES_STD_QUALIFIER cerr << "\nWarning at (file " << StrX(e.getSystemId())
		 << ", line " << e.getLineNumber()
		 << ", char " << e.getColumnNumber()
         << "): " << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;
}


// This is the SAX2 resolveEntity interface...
// -----------------------------------------------------------------------
//  Handlers for the SAX EntityResolver interface
// -----------------------------------------------------------------------
InputSource* RedirectHandlers::resolveEntity(const   XMLCh* const    /* publicId */
                                             , const XMLCh* const    systemId)
{
    //
    //  If it's our file, then create a new URL input source for the file that
    //  we want to really be used. Otherwise, just return NULL to let the
    //  default action occur.
    if (XMLString::compareString(gFileToTrap, systemId) != 0)
    {
        return NULL;
    }
    else
    {
        // They were equal, so redirect to our other file
        return new LocalFileInputSource(gRedirectToFile);
    }
}

// -----------------------------------------------------------------------
//  Handlers for the XMLEntityResolver interface
// -----------------------------------------------------------------------

InputSource* RedirectHandlers::resolveEntity(XMLResourceIdentifier* resourceIdentifier)
{
    // Call the SAX2 version.
    return resolveEntity(NULL, resourceIdentifier->getSystemId());
}
