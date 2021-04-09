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
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMImplementationRegistry.hpp>
#include <xercesc/dom/DOMLSParser.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMError.hpp>
#include <xercesc/dom/DOMLocator.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <xercesc/dom/DOMAttr.hpp>
#include "DOMCount.hpp"
#include <string.h>
#include <stdlib.h>

#if defined(XERCES_NEW_IOSTREAMS)
#include <fstream>
#else
#include <fstream.h>
#endif


// ---------------------------------------------------------------------------
//  This is a simple program which invokes the DOMParser to build a DOM
//  tree for the specified input file. It then walks the tree and counts
//  the number of elements. The element count is then printed.
// ---------------------------------------------------------------------------
static void usage()
{
    XERCES_STD_QUALIFIER cout << "\nUsage:\n"
            "    DOMCount [options] <XML file | List file>\n\n"
            "This program invokes the DOMLSParser, builds the DOM tree,\n"
            "and then prints the number of elements found in each XML file.\n\n"
            "Options:\n"
            "    -l          Indicate the input file is a List File that has a list of xml files.\n"
            "                Default to off (Input file is an XML file).\n"
            "    -v=xxx      Validation scheme [always | never | auto*].\n"
            "    -n          Enable namespace processing. Defaults to off.\n"
            "    -s          Enable schema processing. Defaults to off.\n"
            "    -f          Enable full schema constraint checking. Defaults to off.\n"
            "    -d          Disallow DOCTYPE. Defaults to false.\n"
            "    -locale=ll_CC specify the locale, default: en_US.\n"
            "    -p          Print out names of elements and attributes encountered.\n"
		    "    -?          Show this help.\n\n"
            "  * = Default if not provided explicitly.\n"
         << XERCES_STD_QUALIFIER endl;
}



// ---------------------------------------------------------------------------
//
//  Recursively Count up the total number of child Elements under the specified Node.
//  Process attributes of the node, if any.
//
// ---------------------------------------------------------------------------
static int countChildElements(DOMNode *n, bool printOutEncounteredEles)
{
    DOMNode *child;
    int count = 0;
    if (n) {
        if (n->getNodeType() == DOMNode::ELEMENT_NODE)
		{
            if(printOutEncounteredEles) {
                char *name = XMLString::transcode(n->getNodeName());
                XERCES_STD_QUALIFIER cout <<"----------------------------------------------------------"<<XERCES_STD_QUALIFIER endl;
                XERCES_STD_QUALIFIER cout <<"Encountered Element : "<< name << XERCES_STD_QUALIFIER endl;

                XMLString::release(&name);

                if(n->hasAttributes()) {
                    // get all the attributes of the node
                    DOMNamedNodeMap *pAttributes = n->getAttributes();
                    const XMLSize_t nSize = pAttributes->getLength();
                    XERCES_STD_QUALIFIER cout <<"\tAttributes" << XERCES_STD_QUALIFIER endl;
                    XERCES_STD_QUALIFIER cout <<"\t----------" << XERCES_STD_QUALIFIER endl;
                    for(XMLSize_t i=0;i<nSize;++i) {
                        DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
                        // get attribute name
                        char *name = XMLString::transcode(pAttributeNode->getName());

                        XERCES_STD_QUALIFIER cout << "\t" << name << "=";
                        XMLString::release(&name);

                        // get attribute type
                        name = XMLString::transcode(pAttributeNode->getValue());
                        XERCES_STD_QUALIFIER cout << name << XERCES_STD_QUALIFIER endl;
                        XMLString::release(&name);
                    }
                }
            }
			++count;
		}
        for (child = n->getFirstChild(); child != 0; child=child->getNextSibling())
            count += countChildElements(child, printOutEncounteredEles);
    }
    return count;
}

// ---------------------------------------------------------------------------
//
//   main
//
// ---------------------------------------------------------------------------
int main(int argC, char* argV[])
{

    // Check command line and extract arguments.
    if (argC < 2)
    {
        usage();
        return 1;
    }

    const char*                xmlFile = 0;
    AbstractDOMParser::ValSchemes valScheme = AbstractDOMParser::Val_Auto;
    bool                       doNamespaces       = false;
    bool                       doSchema           = false;
    bool                       schemaFullChecking = false;
    bool                       disallowDoctype    = false;
    bool                       doList = false;
    bool                       errorOccurred = false;
    bool                       recognizeNEL = false;
    bool                       printOutEncounteredEles = false;
    char                       localeStr[64];
    memset(localeStr, 0, sizeof localeStr);

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
         else if (!strncmp(argV[argInd], "-v=", 3)
              ||  !strncmp(argV[argInd], "-V=", 3))
        {
            const char* const parm = &argV[argInd][3];

            if (!strcmp(parm, "never"))
                valScheme = AbstractDOMParser::Val_Never;
            else if (!strcmp(parm, "auto"))
                valScheme = AbstractDOMParser::Val_Auto;
            else if (!strcmp(parm, "always"))
                valScheme = AbstractDOMParser::Val_Always;
            else
            {
                XERCES_STD_QUALIFIER cerr << "Unknown -v= value: " << parm << XERCES_STD_QUALIFIER endl;
                return 2;
            }
        }
         else if (!strcmp(argV[argInd], "-n")
              ||  !strcmp(argV[argInd], "-N"))
        {
            doNamespaces = true;
        }
         else if (!strcmp(argV[argInd], "-s")
              ||  !strcmp(argV[argInd], "-S"))
        {
            doSchema = true;
        }
         else if (!strcmp(argV[argInd], "-f")
              ||  !strcmp(argV[argInd], "-F"))
        {
            schemaFullChecking = true;
        }
         else if (!strcmp(argV[argInd], "-l")
              ||  !strcmp(argV[argInd], "-L"))
        {
            doList = true;
        }
         else if (!strcmp(argV[argInd], "-d")
              ||  !strcmp(argV[argInd], "-D"))
        {
            disallowDoctype = true;
        }
         else if (!strcmp(argV[argInd], "-special:nel"))
        {
            // turning this on will lead to non-standard compliance behaviour
            // it will recognize the unicode character 0x85 as new line character
            // instead of regular character as specified in XML 1.0
            // do not turn this on unless really necessary

             recognizeNEL = true;
        }
         else if (!strcmp(argV[argInd], "-p")
              ||  !strcmp(argV[argInd], "-P"))
        {
            printOutEncounteredEles = true;
        }
         else if (!strncmp(argV[argInd], "-locale=", 8))
        {
             // Get out the end of line
             strcpy(localeStr, &(argV[argInd][8]));
        }
         else
        {
            XERCES_STD_QUALIFIER cerr << "Unknown option '" << argV[argInd]
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

    // Initialize the XML4C system
    try
    {
        if (strlen(localeStr))
        {
            XMLPlatformUtils::Initialize(localeStr);
        }
        else
        {
            XMLPlatformUtils::Initialize();
        }

        if (recognizeNEL)
        {
            XMLPlatformUtils::recognizeNEL(recognizeNEL);
        }
    }

    catch (const XMLException& toCatch)
    {
         XERCES_STD_QUALIFIER cerr << "Error during initialization! :\n"
              << StrX(toCatch.getMessage()) << XERCES_STD_QUALIFIER endl;
         return 1;
    }

    // Instantiate the DOM parser.
    static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
    DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(gLS);
    DOMLSParser       *parser = ((DOMImplementationLS*)impl)->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
    DOMConfiguration  *config = parser->getDomConfig();

    config->setParameter(XMLUni::fgDOMNamespaces, doNamespaces);
    config->setParameter(XMLUni::fgXercesSchema, doSchema);
    config->setParameter(XMLUni::fgXercesHandleMultipleImports, true);
    config->setParameter(XMLUni::fgXercesSchemaFullChecking, schemaFullChecking);
    config->setParameter(XMLUni::fgDOMDisallowDoctype, disallowDoctype);

    if (valScheme == AbstractDOMParser::Val_Auto)
    {
        config->setParameter(XMLUni::fgDOMValidateIfSchema, true);
    }
    else if (valScheme == AbstractDOMParser::Val_Never)
    {
        config->setParameter(XMLUni::fgDOMValidate, false);
    }
    else if (valScheme == AbstractDOMParser::Val_Always)
    {
        config->setParameter(XMLUni::fgDOMValidate, true);
    }

    // enable datatype normalization - default is off
    config->setParameter(XMLUni::fgDOMDatatypeNormalization, true);

    // And create our error handler and install it
    DOMCountErrorHandler errorHandler;
    config->setParameter(XMLUni::fgDOMErrorHandler, &errorHandler);

    //
    //  Get the starting time and kick off the parse of the indicated
    //  file. Catch any exceptions that might propogate out of it.
    //
    unsigned long duration;

    bool more = true;
    XERCES_STD_QUALIFIER ifstream fin;

    // the input is a list file
    if (doList)
        fin.open(argV[argInd]);

    if (fin.fail()) {
        XERCES_STD_QUALIFIER cerr <<"Cannot open the list file: " << argV[argInd] << XERCES_STD_QUALIFIER endl;
        return 2;
    }

    while (more)
    {
        char fURI[1000];
        //initialize the array to zeros
        memset(fURI,0,sizeof(fURI));

        if (doList) {
            if (! fin.eof() ) {
                fin.getline (fURI, sizeof(fURI));
                if (!*fURI)
                    continue;
                else {
                    xmlFile = fURI;
                    XERCES_STD_QUALIFIER cerr << "==Parsing== " << xmlFile << XERCES_STD_QUALIFIER endl;
                }
            }
            else
                break;
        }
        else {
            xmlFile = argV[argInd];
            more = false;
        }

        //reset error count first
        errorHandler.resetErrors();

        XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc = 0;

        try
        {
            // reset document pool
            parser->resetDocumentPool();

            const unsigned long startMillis = XMLPlatformUtils::getCurrentMillis();
            doc = parser->parseURI(xmlFile);
            const unsigned long endMillis = XMLPlatformUtils::getCurrentMillis();
            duration = endMillis - startMillis;
        }

        catch (const XMLException& toCatch)
        {
            XERCES_STD_QUALIFIER cerr << "\nError during parsing: '" << xmlFile << "'\n"
                 << "Exception message is:  \n"
                 << StrX(toCatch.getMessage()) << "\n" << XERCES_STD_QUALIFIER endl;
            errorOccurred = true;
            continue;
        }
        catch (const DOMException& toCatch)
        {
            const unsigned int maxChars = 2047;
            XMLCh errText[maxChars + 1];

            XERCES_STD_QUALIFIER cerr << "\nDOM Error during parsing: '" << xmlFile << "'\n"
                 << "DOMException code is:  " << toCatch.code << XERCES_STD_QUALIFIER endl;

            if (DOMImplementation::loadDOMExceptionMsg(toCatch.code, errText, maxChars))
                 XERCES_STD_QUALIFIER cerr << "Message is: " << StrX(errText) << XERCES_STD_QUALIFIER endl;

            errorOccurred = true;
            continue;
        }
        catch (...)
        {
            XERCES_STD_QUALIFIER cerr << "\nUnexpected exception during parsing: '" << xmlFile << "'\n";
            errorOccurred = true;
            continue;
        }

        //
        //  Extract the DOM tree, get the list of all the elements and report the
        //  length as the count of elements.
        //
        if (errorHandler.getSawErrors())
        {
            XERCES_STD_QUALIFIER cout << "\nErrors occurred, no output available\n" << XERCES_STD_QUALIFIER endl;
            errorOccurred = true;
        }
         else
        {
            unsigned int elementCount = 0;
            if (doc) {
                elementCount = countChildElements((DOMNode*)doc->getDocumentElement(), printOutEncounteredEles);
                // test getElementsByTagName and getLength
                XMLCh xa[] = {chAsterisk, chNull};
                if (elementCount != doc->getElementsByTagName(xa)->getLength()) {
                    XERCES_STD_QUALIFIER cout << "\nErrors occurred, element count is wrong\n" << XERCES_STD_QUALIFIER endl;
                    errorOccurred = true;
                }
            }

            // Print out the stats that we collected and time taken.
            XERCES_STD_QUALIFIER cout << xmlFile << ": " << duration << " ms ("
                 << elementCount << " elems)." << XERCES_STD_QUALIFIER endl;
        }
    }

    //
    //  Delete the parser itself.  Must be done prior to calling Terminate, below.
    //
    parser->release();

    // And call the termination method
    XMLPlatformUtils::Terminate();

    if (doList)
        fin.close();

    if (errorOccurred)
        return 4;
    else
        return 0;
}





DOMCountErrorHandler::DOMCountErrorHandler() :

    fSawErrors(false)
{
}

DOMCountErrorHandler::~DOMCountErrorHandler()
{
}


// ---------------------------------------------------------------------------
//  DOMCountHandlers: Overrides of the DOM ErrorHandler interface
// ---------------------------------------------------------------------------
bool DOMCountErrorHandler::handleError(const DOMError& domError)
{
    fSawErrors = true;
    if (domError.getSeverity() == DOMError::DOM_SEVERITY_WARNING)
        XERCES_STD_QUALIFIER cerr << "\nWarning at file ";
    else if (domError.getSeverity() == DOMError::DOM_SEVERITY_ERROR)
        XERCES_STD_QUALIFIER cerr << "\nError at file ";
    else
        XERCES_STD_QUALIFIER cerr << "\nFatal Error at file ";

    XERCES_STD_QUALIFIER cerr << StrX(domError.getLocation()->getURI())
         << ", line " << domError.getLocation()->getLineNumber()
         << ", char " << domError.getLocation()->getColumnNumber()
         << "\n  Message: " << StrX(domError.getMessage()) << XERCES_STD_QUALIFIER endl;

    return true;
}

void DOMCountErrorHandler::resetErrors()
{
    fSawErrors = false;
}
