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
#include "XSTSHarness.hpp"
#include "XSTSHarnessHandlers.hpp"
#include "XMLHarnessHandlers.hpp"
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/util/BinInputStream.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#if defined(XERCES_NEW_IOSTREAMS)
#include <fstream>
#else
#include <fstream.h>
#endif
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

static XMLCh sz_XMLTestSuiteRoot[]={ chLatin_T, chLatin_E, chLatin_S, chLatin_T, chLatin_S, chLatin_U, chLatin_I, chLatin_T, chLatin_E, chNull };
const XMLCh dummy[]={ chLatin_f, chLatin_i, chLatin_l, chLatin_e, chColon, chForwardSlash, chForwardSlash,
                       chLatin_d, chLatin_u, chLatin_m, chLatin_m, chLatin_y, chForwardSlash, chNull };
const XMLCh* g_scanner = XMLUni::fgIGXMLScanner;

// ---------------------------------------------------------------------------
//  Local helper methods
// ---------------------------------------------------------------------------
void usage()
{
    XERCES_STD_QUALIFIER cout <<
            "\n"
            "Usage:\n"
            "    XSTSHarness <testSet>\n"
            "\n"
            "This program runs the tests listed in the XMLSchema Test Suite, available at\n"
            "http://www.w3.org/XML/2004/xml-schema-test-suite/xmlschema2006-11-06/xsts-2007-06-20.tar.gz\n"
            "Run this executable against the suite.xml file found in the top directory\n"
            "\n"
            "It also runs the tests listed in the XML Test Suite, available at\n"
            "http://www.w3.org/XML/Test/xmlts20080827.zip\n"
            "Run this executable against the xmlconf.xml file found in the top directory\n"
            "\n"
         << XERCES_STD_QUALIFIER endl;
}

BaseHarnessHandlers::BaseHarnessHandlers(const XMLCh* baseURL) :
    fBaseURL(baseURL),
    fSawErrors(false),
    fFailures(0),
    fTests(0)
{
}

void BaseHarnessHandlers::error(const SAXParseException& e)
{
    fSawErrors = true;
    XERCES_STD_QUALIFIER cout << "\nError at file " << StrX(e.getSystemId())
		 << ", line " << e.getLineNumber()
		 << ", char " << e.getColumnNumber()
         << "\n  Message: " << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;
}

void BaseHarnessHandlers::fatalError(const SAXParseException& e)
{
    fSawErrors = true;
    XERCES_STD_QUALIFIER cout << "\nFatal Error at file " << StrX(e.getSystemId())
		 << ", line " << e.getLineNumber()
		 << ", char " << e.getColumnNumber()
         << "\n  Message: " << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;
}

void BaseHarnessHandlers::warning(const SAXParseException& e)
{
    XERCES_STD_QUALIFIER cout << "\nWarning at file " << StrX(e.getSystemId())
		 << ", line " << e.getLineNumber()
		 << ", char " << e.getColumnNumber()
         << "\n  Message: " << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;
}

void BaseHarnessHandlers::printFile(XMLURL& url)
{
    if(XMLString::equals(url.getURLText(), dummy))
        return;
    BinInputStream* stream=url.makeNewStream();
    if(stream==NULL)
    {
        XERCES_STD_QUALIFIER cout << "File " << StrX(url.getURLText()) << " is missing" << XERCES_STD_QUALIFIER endl;
        return;
    }
    XERCES_STD_QUALIFIER cout << "Content of file " << StrX(url.getURLText()) << XERCES_STD_QUALIFIER endl;
    XMLByte buffer[256];
    XMLSize_t nRead;
    while((nRead=stream->readBytes(buffer, 255)) >0)
    {
        buffer[nRead]=0;
        // sending data containing \n\r to cout generates \n\n\r, so strip any \r
        XMLSize_t idx=0;
        while(true)
        {
            int cr=XMLString::indexOf((const char*)buffer, '\r', idx);
            if(cr==-1)
                break;
            memmove(&buffer[cr], &buffer[cr+1], XMLString::stringLen((const char*)&buffer[cr+1])+1);
            idx=cr;
            if(buffer[idx]==0)
                break;
        }
        XERCES_STD_QUALIFIER cout << (const char*)buffer;
    }
    XERCES_STD_QUALIFIER cout << XERCES_STD_QUALIFIER endl;
    delete stream;
}

/////////////////////////////////////////////////////////////////////

void BaseErrorHandler::error(const SAXParseException& exc)
{
    fSawErrors=true;
    fErrorText.append(exc.getMessage());
    fErrorText.append(chLF);
}

void BaseErrorHandler::fatalError(const SAXParseException& exc)
{
    fSawErrors=true;
    fErrorText.append(exc.getMessage());
    fErrorText.append(chLF);
}

/////////////////////////////////////////////////////////////////////

InputSource* BaseEntityResolver::resolveEntity(XMLResourceIdentifier* resourceIdentifier)
{
    if(XMLString::equals(resourceIdentifier->getNameSpace(), XMLUni::fgXMLURIName))
    {
        static const char* xmlXsd=  "<?xml version='1.0'?>"
                                    "<xs:schema targetNamespace='http://www.w3.org/XML/1998/namespace' "
                                    "  xmlns:xs='http://www.w3.org/2001/XMLSchema' "
                                    "  xml:lang='en'>"
                                    ""
                                    " <xs:attribute name='lang'>"
                                    "  <xs:simpleType>"
                                    "   <xs:union memberTypes='xs:language'>"
                                    "    <xs:simpleType>"
                                    "     <xs:restriction base='xs:string'>"
                                    "      <xs:enumeration value=''/>"
                                    "     </xs:restriction>"
                                    "    </xs:simpleType>"
                                    "   </xs:union>"
                                    "  </xs:simpleType>"
                                    " </xs:attribute>"
                                    ""
                                    " <xs:attribute name='space'>"
                                    "  <xs:simpleType>"
                                    "   <xs:restriction base='xs:NCName'>"
                                    "    <xs:enumeration value='default'/>"
                                    "    <xs:enumeration value='preserve'/>"
                                    "   </xs:restriction>"
                                    "  </xs:simpleType>"
                                    " </xs:attribute>"
                                    ""
                                    " <xs:attribute name='base' type='xs:anyURI'>"
                                    " </xs:attribute>"
                                    ""
                                    " <xs:attribute name='id' type='xs:ID'>"
                                    " </xs:attribute>"
                                    ""
                                    " <xs:attributeGroup name='specialAttrs'>"
                                    "  <xs:attribute ref='xml:base'/>"
                                    "  <xs:attribute ref='xml:lang'/>"
                                    "  <xs:attribute ref='xml:space'/>"
                                    "  <xs:attribute ref='xml:id'/>"
                                    " </xs:attributeGroup>"
                                    ""
                                    "</xs:schema>";
        return new MemBufInputSource((XMLByte*)xmlXsd, strlen(xmlXsd), "");
    }
    return 0;
}

/////////////////////////////////////////////////////////////////////

class RootExtractor : public DefaultHandler
{
public:
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    RootExtractor()
    {
        fIsXMLSuite=false;
    }

    // -----------------------------------------------------------------------
    //  Getter methods
    // -----------------------------------------------------------------------
    bool isXMLSuite() const
    {
        return fIsXMLSuite;
    }

    // -----------------------------------------------------------------------
    //  Handlers for the SAX DocumentHandler interface
    // -----------------------------------------------------------------------
    void startElement(const XMLCh* const /* uri */, const XMLCh* const /* localname */, const XMLCh* const qname, const Attributes& /* attrs */)
    {
        fIsXMLSuite=XMLString::equals(qname, sz_XMLTestSuiteRoot);
        throw fIsXMLSuite;
    }

protected:
    bool fIsXMLSuite;
};

// ---------------------------------------------------------------------------
//  Program entry point
// ---------------------------------------------------------------------------
int main(int argC, char* argV[])
{

    // Check command line and extract arguments.
    if (argC < 2)
    {
        usage();
        return 1;
    }

    try
    {
        XMLPlatformUtils::Initialize();
    }

    catch (const XMLException& toCatch)
    {
        XERCES_STD_QUALIFIER cout << "Error during initialization! Message:\n"
            << StrX(toCatch.getMessage()) << XERCES_STD_QUALIFIER endl;
        return 1;
    }

    int argInd;
    for (argInd = 1; argInd < argC; argInd++)
    {
        // Break out on first parm not starting with a dash
        if (argV[argInd][0] != '-')
            break;

        // Watch for special case help request
        if (!strcmp(argV[argInd], "-?"))
        {
            usage();
            return 2;
        }
        // TODO: add option to generate the XML summarizing the result
        else if (!strncmp(argV[argInd], "-scanner=", 9))
        {
            g_scanner = XMLString::transcode(argV[argInd]+9);
        }
        else
        {
            XERCES_STD_QUALIFIER cout << "Unknown option '" << argV[argInd]
                << "', ignoring it\n" << XERCES_STD_QUALIFIER endl;
        }
    }

    //
    //  There should be only one and only one parameter left, and that
    //  should be the file name.
    //
    if (argInd != argC - 1)
    {
        usage();
        return 1;
    }

    //
    //  Create a SAX parser object.
    //
    SAX2XMLReader* parser = XMLReaderFactory::createXMLReader();
    parser->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);
    parser->setFeature(XMLUni::fgSAX2CoreValidation, false);

    const char* xmlFile = argV[argInd];

    // Discover if the test suite is the XML or XMLSchema one
    RootExtractor rootExtractor;
    parser->setContentHandler(&rootExtractor);
    try
    {
        parser->parse(xmlFile);
    }
    catch (...)
    {
    }

    XMLCh* uniFile = XMLString::transcode(xmlFile);
    XMLCh* uri = new XMLCh[XMLString::stringLen(xmlFile) + 9];
    XMLString::fixURI(uniFile, uri);
    BaseHarnessHandlers* handler=NULL;
    if(rootExtractor.isXMLSuite())
    {
        // XML Test Suite
        handler=new XMLHarnessHandlers(uri, g_scanner);
    }
    else
    {
        // XMLSchema Test Suite
        handler=new XSTSHarnessHandlers(uri, g_scanner);
    }
    XMLString::release(&uniFile);
    delete [] uri;
    parser->setContentHandler(handler);
    parser->setErrorHandler(handler);

    bool errorOccurred=false;
    const unsigned long startMillis = XMLPlatformUtils::getCurrentMillis();
    //
    //  Get the starting time and kick off the parse of the indicated
    //  file. Catch any exceptions that might propagate out of it.
    //
    try
    {
        parser->parse(xmlFile);
    }
    catch (const OutOfMemoryException&)
    {
        XERCES_STD_QUALIFIER cout << "OutOfMemoryException" << XERCES_STD_QUALIFIER endl;
        errorOccurred = true;
    }
    catch (const XMLException& e)
    {
        XERCES_STD_QUALIFIER cout << "\nError during parsing: '" << xmlFile << "'\n"
            << "Exception message is:  \n"
            << StrX(e.getMessage()) << "\n" << XERCES_STD_QUALIFIER endl;
        errorOccurred = true;
    }
    catch (...)
    {
        XERCES_STD_QUALIFIER cout << "\nUnexpected exception during parsing: '" << xmlFile << "'\n";
        errorOccurred = true;
    }

    const unsigned long endMillis = XMLPlatformUtils::getCurrentMillis();
    unsigned long duration = endMillis - startMillis;

    if (handler->getSawErrors())
        errorOccurred = true;

    XERCES_STD_QUALIFIER cout << "Scanner: " << StrX(g_scanner) << XERCES_STD_QUALIFIER endl;
    XERCES_STD_QUALIFIER cout << "  Total tests: " << handler->getTotalTests() << XERCES_STD_QUALIFIER endl;
    XERCES_STD_QUALIFIER cout << "  Failed tests: " << handler->getFailedTests() << XERCES_STD_QUALIFIER endl;
    XERCES_STD_QUALIFIER cout << "  Success rate: " << ((double)(handler->getTotalTests()-handler->getFailedTests()))/(double)handler->getTotalTests()*100 << "%" << XERCES_STD_QUALIFIER endl;
    XERCES_STD_QUALIFIER cout << "  Duration: ";
    if(duration > 60000)
    {
        XERCES_STD_QUALIFIER cout << duration/60000 << ":";
        duration=duration % 60000;
    }
    if(duration/1000 < 10)
        XERCES_STD_QUALIFIER cout << "0";
    XERCES_STD_QUALIFIER cout << duration/1000 << "." << duration % 1000 << XERCES_STD_QUALIFIER endl;

    //
    //  Delete the parser itself.  Must be done prior to calling Terminate, below.
    //
    delete parser;
    delete handler;

    // And call the termination method
    XMLPlatformUtils::Terminate();

    if (errorOccurred)
        return 4;
    else
        return 0;

}
