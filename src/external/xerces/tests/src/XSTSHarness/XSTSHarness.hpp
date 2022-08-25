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

#ifndef XSTS_HARNESS_HPP
#define XSTS_HARNESS_HPP

// ---------------------------------------------------------------------------
//  Includes for all the program files to see
// ---------------------------------------------------------------------------

#include <xercesc/util/PlatformUtils.hpp>
#include <stdlib.h>
#include <string.h>
#if defined(XERCES_NEW_IOSTREAMS)
#include <iostream>
#else
#include <iostream.h>
#endif
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLURL.hpp>
#include <xercesc/util/XMLEntityResolver.hpp>

XERCES_CPP_NAMESPACE_USE

typedef enum
{
    unknown,
    invalid,
    valid
} ValidityOutcome;

extern const XMLCh dummy[];

// ---------------------------------------------------------------------------
//  This is a simple class that lets us do easy (though not terribly efficient)
//  trancoding of XMLCh data to local code page for display.
// ---------------------------------------------------------------------------
class StrX
{
public :
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    StrX(const XMLCh* const toTranscode)
    {
        // Call the private transcoding method
        fLocalForm = XMLString::transcode(toTranscode);
    }

    ~StrX()
    {
        XMLString::release(&fLocalForm);
    }

    // -----------------------------------------------------------------------
    //  Getter methods
    // -----------------------------------------------------------------------
    const char* localForm() const
    {
        return fLocalForm;
    }

private :
    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  fLocalForm
    //      This is the local code page form of the string.
    // -----------------------------------------------------------------------
    char*   fLocalForm;
};

inline XERCES_STD_QUALIFIER ostream& operator<<(XERCES_STD_QUALIFIER ostream& target, const StrX& toDump)
{
    target << toDump.localForm();
    return target;
}

class BaseHarnessHandlers : public DefaultHandler
{
public:
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    BaseHarnessHandlers(const XMLCh* baseURL);

    unsigned int getTotalTests() const
    {
        return fTests;
    }
    unsigned int getFailedTests() const
    {
        return fFailures;
    }

    bool getSawErrors() const
    {
        return fSawErrors;
    }

    void printFile(XMLURL& url);

    // -----------------------------------------------------------------------
    //  Handlers for the SAX ErrorHandler interface
    // -----------------------------------------------------------------------
	void warning(const SAXParseException& exc);
    void error(const SAXParseException& exc);
    void fatalError(const SAXParseException& exc);
    void resetErrors()
    {
        fSawErrors = false;
    }

protected:
    XMLURL              fBaseURL;
    bool                fSawErrors;
    unsigned int        fFailures, fTests;
};

class BaseErrorHandler : public ErrorHandler
{
public:
    BaseErrorHandler() : fSawErrors(false) {}

    bool getSawErrors() const
    {
        return fSawErrors;
    }
    const XMLCh* getErrorText()
    {
        return fErrorText.getRawBuffer();
    }

    // -----------------------------------------------------------------------
    //  Handlers for the SAX ErrorHandler interface
    // -----------------------------------------------------------------------
    void warning(const SAXParseException& /* exc */)      {}
    void error(const SAXParseException& exc);
    void fatalError(const SAXParseException& exc);
    void resetErrors()                              { fSawErrors=false; fErrorText.reset(); }

private:
    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  fSawErrors
    //      This is set by the error handlers, and is queryable later to
    //      see if any errors occured.
    // -----------------------------------------------------------------------
    bool            fSawErrors;
    XMLBuffer       fErrorText;
};

class BaseEntityResolver : public XMLEntityResolver
{
public:
    BaseEntityResolver() {};

    virtual InputSource* resolveEntity(XMLResourceIdentifier* resourceIdentifier);

};

#endif
