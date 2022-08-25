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
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/AbstractDOMParser.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>

#include "XInclude.hpp"

// ---------------------------------------------------------------------------
//  This is a simple program which tests the different XInclude mechanisms prototyped
//  for my dissertation. Based largely on the DOMCount example provided with the
//  Xerces C++ project.
//  Simon Rowland 2006
// ---------------------------------------------------------------------------
static void usage()
{
    XERCES_STD_QUALIFIER cout << "\nUsage:\n"
			"    XInclude [-h] InputFile OutputFile\n"
			"  -h :             Prints this help message and exits.\n"
         << XERCES_STD_QUALIFIER endl;
}

// ---------------------------------------------------------------------------
//
//   main
//
// ---------------------------------------------------------------------------
int main(int argC, char* argV[])
{
	char						*testFileName;
	char						*outputFileName;

    for (int argInd = 1; argInd < argC; argInd++)
	{
        if (!strcmp(argV[argInd], "-?") || !strcmp(argV[argInd], "-h"))
        {
			/* print help and exit */
            usage();
            return 2;
        }
    }

	if (argC < 3){
		usage();
		return 2;
	}

	testFileName = argV[argC-2];
	outputFileName = argV[argC-1];

    // Initialize the XML4C system
    try
    {
        XMLPlatformUtils::Initialize();
    }

    catch (const XMLException& toCatch)
    {
         XERCES_STD_QUALIFIER cerr << "Error during initialization! :\n"
              << StrX(toCatch.getMessage()) << XERCES_STD_QUALIFIER endl;
         return 1;
    }

	//============================================================================
	// Instantiate the DOM parser to use for the source documents
	//============================================================================
    static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
    DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(gLS);
    DOMLSParser       *parser = ((DOMImplementationLS*)impl)->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
    DOMConfiguration  *config = parser->getDomConfig();

    config->setParameter(XMLUni::fgDOMNamespaces, true);
    config->setParameter(XMLUni::fgXercesSchema, true);
    config->setParameter(XMLUni::fgXercesHandleMultipleImports, true);
    config->setParameter(XMLUni::fgXercesSchemaFullChecking, true);

    if(config->canSetParameter(XMLUni::fgXercesDoXInclude, true)){
      config->setParameter(XMLUni::fgXercesDoXInclude, true);
    }

    // enable datatype normalization - default is off
    //config->setParameter(XMLUni::fgDOMDatatypeNormalization, true);

    // And create our error handler and install it
    XIncludeErrorHandler errorHandler;
    config->setParameter(XMLUni::fgDOMErrorHandler, &errorHandler);

    XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc = 0;

    try
    {
        // load up the test source document
		XERCES_STD_QUALIFIER cerr << "Parse " << testFileName << " in progress ...";
        parser->resetDocumentPool();
		doc = parser->parseURI(testFileName);
		XERCES_STD_QUALIFIER cerr << " finished." << XERCES_STD_QUALIFIER endl;
    }
    catch (const XMLException& toCatch)
    {
        XERCES_STD_QUALIFIER cerr << "\nError during parsing: '" << testFileName << "'\n"
                << "Exception message is:  \n"
                << StrX(toCatch.getMessage()) << "\n" << XERCES_STD_QUALIFIER endl;
    }
    catch (const DOMException& toCatch)
    {
        const unsigned int maxChars = 2047;
        XMLCh errText[maxChars + 1];

        XERCES_STD_QUALIFIER cerr << "\nDOM Error during parsing: '" << testFileName << "'\n"
                << "DOMException code is:  " << toCatch.code << XERCES_STD_QUALIFIER endl;

        if (DOMImplementation::loadDOMExceptionMsg(toCatch.code, errText, maxChars))
                XERCES_STD_QUALIFIER cerr << "Message is: " << StrX(errText) << XERCES_STD_QUALIFIER endl;

    }
    catch (...)
    {
        XERCES_STD_QUALIFIER cerr << "\nUnexpected exception during parsing: '" << testFileName << "'\n";
    }

    if (!errorHandler.getSawErrors() && doc) {
	    DOMLSSerializer	*writer = ((DOMImplementationLS*)impl)->createLSSerializer();
	    DOMLSOutput     *theOutputDesc = ((DOMImplementationLS*)impl)->createLSOutput();

		try {
			// write out the results
			XERCES_STD_QUALIFIER cerr << "Writing result to: " << outputFileName << XERCES_STD_QUALIFIER endl;

			XMLFormatTarget *myFormTarget = new LocalFileFormatTarget(outputFileName);
			theOutputDesc->setByteStream(myFormTarget);
			writer->write(doc, theOutputDesc);
            delete myFormTarget;
		}
		catch (const XMLException& toCatch)
		{
		    XERCES_STD_QUALIFIER cerr << "\nXMLException during writing: '" << testFileName << "'\n"
				<< "Exception message is:  \n"
				<< StrX(toCatch.getMessage()) << "\n" << XERCES_STD_QUALIFIER endl;
		}
		catch (const DOMException& toCatch)
		{
			const unsigned int maxChars = 2047;
			XMLCh errText[maxChars + 1];

			XERCES_STD_QUALIFIER cerr << "\nDOM Error during writing: '" << testFileName << "'\n"
				<< "DOMException code is:  " << toCatch.code << XERCES_STD_QUALIFIER endl;

			if (DOMImplementation::loadDOMExceptionMsg(toCatch.code, errText, maxChars))
				XERCES_STD_QUALIFIER cerr << "Message is: " << StrX(errText) << XERCES_STD_QUALIFIER endl;
		}
		catch (...)
		{
			XERCES_STD_QUALIFIER cerr << "\nUnexpected exception during writing: '" << testFileName << "'\n";
		}
        writer->release();
        theOutputDesc->release();
    }

    //
    //  Delete the parser itself.  Must be done prior to calling Terminate, below.
    //
    parser->release();
	// And call the termination method
    XMLPlatformUtils::Terminate();

    return 0;
}

XIncludeErrorHandler::XIncludeErrorHandler() :
    fSawErrors(false)
{
}

XIncludeErrorHandler::~XIncludeErrorHandler()
{
}


// ---------------------------------------------------------------------------
//  XIncludeHandlers: Overrides of the DOM ErrorHandler interface
// ---------------------------------------------------------------------------
bool XIncludeErrorHandler::handleError(const DOMError& domError)
{
	bool continueParsing = true;
    if (domError.getSeverity() == DOMError::DOM_SEVERITY_WARNING)
        XERCES_STD_QUALIFIER cerr << "\nWarning at file ";
    else if (domError.getSeverity() == DOMError::DOM_SEVERITY_ERROR)
    {
        XERCES_STD_QUALIFIER cerr << "\nError at file ";
        fSawErrors = true;
    }
	else {
        XERCES_STD_QUALIFIER cerr << "\nFatal Error at file ";
		continueParsing = false;
        fSawErrors = true;
	}

    XERCES_STD_QUALIFIER cerr << StrX(domError.getLocation()->getURI())
         << ", line " << domError.getLocation()->getLineNumber()
         << ", char " << domError.getLocation()->getColumnNumber()
         << "\n  Message: " << StrX(domError.getMessage()) << XERCES_STD_QUALIFIER endl;

    return continueParsing;
}

void XIncludeErrorHandler::resetErrors()
{
    fSawErrors = false;
}
