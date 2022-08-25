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
//  This program tests the XMLPlatformUtils::Initialize/Terminate() pair
//  by calling the pair a number of times.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/sax/SAXParseException.hpp>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/dom/DOMException.hpp>
#include "InitTermTest.hpp"

#include <string.h>
#include <stdlib.h>

#if defined(XERCES_NEW_IOSTREAMS)
#include <fstream>
#else
#include <fstream.h>
#endif
#include <limits.h>


// ---------------------------------------------------------------------------
//  Local Enum to switch theState
// ---------------------------------------------------------------------------
enum Teststate {
    Once,
    Multiple,
    UnEven,
    Limit,
    ExceedLimit
};

// ---------------------------------------------------------------------------
//  Declare functions
// ---------------------------------------------------------------------------
int TestInit4DOM(const char* xmlFile, bool gDoNamespaces, bool gDoSchema, bool gSchemaFullChecking, Teststate theState);
int TestInit4SAX(const char* xmlFile, bool gDoNamespaces, bool gDoSchema, bool gSchemaFullChecking, Teststate theState);
int TestInit4SAX2(const char* xmlFile, bool gDoNamespaces, bool gDoSchema, bool gSchemaFullChecking, Teststate theState);

// ---------------------------------------------------------------------------
//  Define macro
// ---------------------------------------------------------------------------
#define TESTINITPRE                                               \
    long times = 1;                                               \
    switch (theState) {                                           \
        case Multiple:                                            \
        case UnEven:                                              \
            times = 100;                                           \
            break;                                                \
        case Limit:                                               \
        case ExceedLimit:                                         \
            times = LONG_MAX;                                     \
            break;                                                \
        case Once:                                                \
        default:                                                  \
            times = 1;                                            \
    }                                                             \
    long i = 0;                                                   \
    for (i = 0; i < times; i++) {                                 \
        try                                                       \
        {                                                         \
            XMLPlatformUtils::Initialize();                       \
        }                                                         \
                                                                  \
        catch (const XMLException& toCatch)                       \
        {                                                         \
             XERCES_STD_QUALIFIER cerr << "Error during initialization! :\n"           \
                  << StrX(toCatch.getMessage()) << XERCES_STD_QUALIFIER endl;          \
             return 1;                                            \
        }                                                         \
    }                                                             \
                                                                  \
    if (theState == ExceedLimit) {                                \
        try                                                       \
        {                                                         \
            XMLPlatformUtils::Initialize();                       \
        }                                                         \
                                                                  \
        catch (const XMLException& toCatch)                       \
        {                                                         \
             XERCES_STD_QUALIFIER cerr << "Error during initialization! :\n"           \
                  << StrX(toCatch.getMessage()) << XERCES_STD_QUALIFIER endl;          \
             return 1;                                            \
        }                                                         \
    }


#define TESTINITPOST                                              \
    InitTermTestErrorHandler errorHandler;                        \
    parser->setErrorHandler(&errorHandler);                       \
    bool   errorOccurred = false;                                 \
                                                                  \
    errorHandler.resetErrors();                                   \
                                                                  \
    try                                                           \
    {                                                             \
        parser->parse(xmlFile);                                   \
    }                                                             \
    catch (const OutOfMemoryException&)                           \
    {                                                             \
        XERCES_STD_QUALIFIER cerr << "OutOfMemoryException" << XERCES_STD_QUALIFIER endl; \
        errorOccurred = true;                                     \
    }                                                             \
    catch (const XMLException& toCatch)                           \
    {                                                             \
        XERCES_STD_QUALIFIER cerr << "\nError during parsing: '" << xmlFile << "'\n"   \
             << "Exception message is:  \n"                       \
             << StrX(toCatch.getMessage()) << "\n" << XERCES_STD_QUALIFIER endl;       \
        errorOccurred = true;                                     \
    }                                                             \
    catch (const DOMException& toCatch)                           \
    {                                                             \
        XERCES_STD_QUALIFIER cerr << "\nDOM Error during parsing: '" << xmlFile        \
             << "\nDOMException code is:  \n"                     \
             << toCatch.code << "\n" << XERCES_STD_QUALIFIER endl;                     \
        errorOccurred = true;                                     \
    }                                                             \
    catch (...)                                                   \
    {                                                             \
        XERCES_STD_QUALIFIER cerr << "\nUnexpected exception during parsing: '"        \
        << xmlFile << "'\n";                                      \
        errorOccurred = true;                                     \
    }                                                             \
                                                                  \
    if (errorHandler.getSawErrors())                              \
    {                                                             \
        XERCES_STD_QUALIFIER cout << "\nErrors occurred, no output available\n" << XERCES_STD_QUALIFIER endl;\
        errorOccurred = true;                                     \
    }                                                             \
                                                                  \
    delete parser;                                                \
                                                                  \
    for (i = 0; i < times; i++) {                                 \
        XMLPlatformUtils::Terminate();                            \
    }                                                             \
                                                                  \
    if (theState == ExceedLimit || theState == UnEven) {          \
        XMLPlatformUtils::Terminate();                            \
    }                                                             \
                                                                  \
    if (errorOccurred)                                            \
        return 4;                                                 \
    else                                                          \
        return 0;

// ---------------------------------------------------------------------------
//  DOM Parser
// ---------------------------------------------------------------------------
int TestInit4DOM(const char* xmlFile, bool gDoNamespaces, bool gDoSchema, bool gSchemaFullChecking, Teststate theState)
{
    TESTINITPRE;
    XercesDOMParser* parser = new XercesDOMParser;
    parser->setDoNamespaces(gDoNamespaces);
    parser->setDoSchema(gDoSchema);
    parser->setHandleMultipleImports (true);
    parser->setValidationSchemaFullChecking(gSchemaFullChecking);
    TESTINITPOST;
}

// ---------------------------------------------------------------------------
//  SAX Parser
// ---------------------------------------------------------------------------
int TestInit4SAX(const char* xmlFile, bool gDoNamespaces, bool gDoSchema, bool gSchemaFullChecking, Teststate theState)
{
    TESTINITPRE;
    SAXParser* parser = new SAXParser;
    parser->setDoNamespaces(gDoNamespaces);
    parser->setDoSchema(gDoSchema);
    parser->setHandleMultipleImports (true);
    parser->setValidationSchemaFullChecking(gSchemaFullChecking);
    TESTINITPOST;
}

// ---------------------------------------------------------------------------
//  SAX2 XML Reader
// ---------------------------------------------------------------------------
int TestInit4SAX2(const char* xmlFile, bool gDoNamespaces, bool gDoSchema, bool gSchemaFullChecking, Teststate theState)
{
    TESTINITPRE;
    SAX2XMLReader* parser = XMLReaderFactory::createXMLReader();

    XMLCh* doNamespaceFeature = XMLString::transcode("http://xml.org/sax/features/namespaces");
    parser->setFeature(doNamespaceFeature, gDoNamespaces);

    XMLCh* doSchemaFeature = XMLString::transcode("http://apache.org/xml/features/validation/schema");
    parser->setFeature(doSchemaFeature, gDoSchema);

    XMLCh* handleMultipleImportsFeature = XMLString::transcode("http://apache.org/xml/features/validation/schema/handle-multiple-imports");
    parser->setFeature(handleMultipleImportsFeature, true);

    XMLCh* fullSchemaCheckFeature = XMLString::transcode("http://apache.org/xml/features/validation/schema-full-checking");
    parser->setFeature(fullSchemaCheckFeature, gSchemaFullChecking);

    XMLString::release(&doNamespaceFeature);
    XMLString::release(&doSchemaFeature);
    XMLString::release(&handleMultipleImportsFeature);
    XMLString::release(&fullSchemaCheckFeature);

    TESTINITPOST;
}

// ---------------------------------------------------------------------------
//
//  Usage()
//
// ---------------------------------------------------------------------------
void usage()
{
    XERCES_STD_QUALIFIER cout << "\nUsage:\n"
            "    InitTermTest [options] <XML file>\n\n"
            "This program tests the XMLPlatformUtils::Initialize()/Terminate()\n"
            "pair by calling it a number of times.\n"
            "Options:\n"
            "    -n          Enable namespace processing. Default is off.\n"
            "    -s          Enable schema processing. Default is off.\n"
            "    -f          Enable full schema constraint checking. Defaults to off.\n"
		      "    -?          Show this help.\n"
          <<  XERCES_STD_QUALIFIER endl;
}

// ---------------------------------------------------------------------------
//  Main
// ---------------------------------------------------------------------------
int main(int argC, char* argV[]) {

    // ---------------------------------------------------------------------------
    //  Local data
    //
    //  gDoNamespaces
    //      Indicates whether namespace processing should be done.
    //
    //  gDoSchema
    //      Indicates whether schema processing should be done.
    //
    //  gSchemaFullChecking
    //      Indicates whether full schema constraint checking should be done.
    //
    // ---------------------------------------------------------------------------
    bool gDoNamespaces          = false;
    bool gDoSchema              = false;
    bool gSchemaFullChecking    = false;

    // Check command line and extract arguments.
    if (argC < 2)
    {
        usage();
        return 1;
    }

    // See if non validating dom parser configuration is requested.
    int parmInd;
    for (parmInd = 1; parmInd < argC; parmInd++)
    {
        // Break out on first parm not starting with a dash
        if (argV[parmInd][0] != '-')
            break;

        // Watch for special case help request
        if (!strcmp(argV[parmInd], "-?"))
        {
            usage();
            return 2;
        }
         else if (!strcmp(argV[parmInd], "-n")
              ||  !strcmp(argV[parmInd], "-N"))
        {
            gDoNamespaces = true;
        }
         else if (!strcmp(argV[parmInd], "-s")
              ||  !strcmp(argV[parmInd], "-S"))
        {
            gDoSchema = true;
        }
         else if (!strcmp(argV[parmInd], "-f")
              ||  !strcmp(argV[parmInd], "-F"))
        {
            gSchemaFullChecking = true;
        }
         else
        {
            XERCES_STD_QUALIFIER cerr << "Unknown option '" << argV[parmInd]
                 << "', ignoring it.\n" << XERCES_STD_QUALIFIER endl;
        }
    }

    //
    //  And now we have to have only one parameter left and it must be
    //  the file name.
    //
    if (parmInd + 1 != argC)
    {
        usage();
        return 1;
    }

    char* xmlFile = argV[parmInd];
    bool error = false;

    //
    // Calling Init/Term pair a number of times
    //
    unsigned int i = 0;
    for (i=0; i < 100; i++) {
        if (error) {
            XERCES_STD_QUALIFIER cout << "Test Failed" << XERCES_STD_QUALIFIER endl;
            return 4;
        }

        if (TestInit4DOM(xmlFile, gDoNamespaces, gDoSchema, gSchemaFullChecking, Once))
            error = true;
        if (TestInit4SAX(xmlFile, gDoNamespaces, gDoSchema, gSchemaFullChecking, Once))
            error = true;
        if (TestInit4SAX2(xmlFile, gDoNamespaces, gDoSchema, gSchemaFullChecking, Once))
            error = true;
    }

    if (error || TestInit4DOM(xmlFile, gDoNamespaces, gDoSchema, gSchemaFullChecking, Multiple))
        error = true;
    if (error || TestInit4SAX(xmlFile, gDoNamespaces, gDoSchema, gSchemaFullChecking, Multiple))
        error = true;
    if (error || TestInit4SAX2(xmlFile, gDoNamespaces, gDoSchema, gSchemaFullChecking, Multiple))
        error = true;
/*
 * The following Limit test is a stress test that can run a long time
 * Commented out for regular sanity test
 */
/*
    if (error || TestInit4DOM(xmlFile, gDoNamespaces, gDoSchema, gSchemaFullChecking, Limit))
        error = true;
    if (error || TestInit4SAX(xmlFile, gDoNamespaces, gDoSchema, gSchemaFullChecking, Limit))
        error = true;
    if (error || TestInit4SAX2(xmlFile, gDoNamespaces, gDoSchema, gSchemaFullChecking, Limit))
        error = true;

    if (error || TestInit4DOM(xmlFile, gDoNamespaces, gDoSchema, gSchemaFullChecking, ExceedLimit))
        error = true;
    if (error || TestInit4SAX(xmlFile, gDoNamespaces, gDoSchema, gSchemaFullChecking, ExceedLimit))
        error = true;
    if (error || TestInit4SAX2(xmlFile, gDoNamespaces, gDoSchema, gSchemaFullChecking, ExceedLimit))
        error = true;
*/

    if (error || TestInit4DOM(xmlFile, gDoNamespaces, gDoSchema, gSchemaFullChecking, UnEven))
        error = true;
    if (error || TestInit4SAX(xmlFile, gDoNamespaces, gDoSchema, gSchemaFullChecking, UnEven))
        error = true;
    if (error || TestInit4SAX2(xmlFile, gDoNamespaces, gDoSchema, gSchemaFullChecking, UnEven))
        error = true;

    if (error) {
        XERCES_STD_QUALIFIER cout << "Test Failed" << XERCES_STD_QUALIFIER endl;
        return 4;
    }

    XERCES_STD_QUALIFIER cout << "Test Run Successfully" << XERCES_STD_QUALIFIER endl;
    return 0;
}

// ---------------------------------------------------------------------------
//  InitTermTestErrorHandler
// ---------------------------------------------------------------------------
InitTermTestErrorHandler::InitTermTestErrorHandler() :

    fSawErrors(false)
{
}

InitTermTestErrorHandler::~InitTermTestErrorHandler()
{
}

void InitTermTestErrorHandler::error(const SAXParseException& e)
{
    fSawErrors = true;
    XERCES_STD_QUALIFIER cerr << "\nError at file " << StrX(e.getSystemId())
         << ", line " << e.getLineNumber()
         << ", char " << e.getColumnNumber()
         << "\n  Message: " << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;
}

void InitTermTestErrorHandler::fatalError(const SAXParseException& e)
{
    fSawErrors = true;
    XERCES_STD_QUALIFIER cerr << "\nFatal Error at file " << StrX(e.getSystemId())
         << ", line " << e.getLineNumber()
         << ", char " << e.getColumnNumber()
         << "\n  Message: " << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;
}

void InitTermTestErrorHandler::warning(const SAXParseException& e)
{
    XERCES_STD_QUALIFIER cerr << "\nWarning at file " << StrX(e.getSystemId())
         << ", line " << e.getLineNumber()
         << ", char " << e.getColumnNumber()
         << "\n  Message: " << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;
}

void InitTermTestErrorHandler::resetErrors()
{
    fSawErrors = false;
}
