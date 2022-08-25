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

//---------------------------------------------------------------------
//
//  This test program is used, in conjunction with a set of test data files,
//  to verify support for different character encodings in XML.
//
//---------------------------------------------------------------------


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include <xercesc/framework/XMLBuffer.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLException.hpp>

#include <xercesc/sax/SAXException.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SAXParseException.hpp>

#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <stdio.h>

XERCES_CPP_NAMESPACE_USE

static int gTestsFailed = 0;
static int gTestsRun    = 0;
static XercesDOMParser* parser = 0;


//-----------------------------------------------------------------------
//
//  ErrorHandler.   The DOM Parser will report any parsing errors by means
//                  of call-backs to the methods of this class.
//                  This is just necessary boilerplate, as far as this
//                  program is concerned.
//
//-----------------------------------------------------------------------

class  ParseErrorHandler: public ErrorHandler
{
public:
    void warning(const SAXParseException& e);
    void error(const SAXParseException& e);
    void fatalError(const SAXParseException& e);
    void resetErrors() {};

};

void ParseErrorHandler::error(const SAXParseException& e)
{
    char* systemId = XMLString::transcode(e.getSystemId());
    char* message = XMLString::transcode(e.getMessage());

    fprintf(stderr, "\nError at file \"%s\", line %llu, char %llu:  %s\n",
            systemId, (unsigned long long) e.getLineNumber(),
            (unsigned long long) e.getColumnNumber(), message);

    XMLString::release(&systemId);
    XMLString::release(&message);
    throw e;

}

void ParseErrorHandler::fatalError(const SAXParseException& e)
{
    char* systemId = XMLString::transcode(e.getSystemId());
    char* message = XMLString::transcode(e.getMessage());

    fprintf(stderr, "\nFatal Error at file \"%s\", line %llu, char %llu:  %s\n",
            systemId, (unsigned long long) e.getLineNumber(),
            (unsigned long long) e.getColumnNumber(), message);

    XMLString::release(&systemId);
    XMLString::release(&message);
    throw e;
}

void ParseErrorHandler::warning(const SAXParseException& e)
{
    char* systemId = XMLString::transcode(e.getSystemId());
    char* message = XMLString::transcode(e.getMessage());

    fprintf(stderr, "\nWarning at file \"%s\", line %llu, char %llu:  %s\n",
            systemId, (unsigned long long) e.getLineNumber(),
            (unsigned long long) e.getColumnNumber(), message);

    XMLString::release(&systemId);
    XMLString::release(&message);
    throw e;

}


//------------------------------------------------------------------------
//
//   parseFile  - a simpler to use function for just parsing an XML file
//                and getting the DOM Document back.
//
//------------------------------------------------------------------------
static DOMDocument* parseFile(char *fileName)
{
    ParseErrorHandler eh;
    if (!parser)
        parser = new XercesDOMParser;
    parser->setValidationScheme(AbstractDOMParser::Val_Never);
    parser->setErrorHandler(&eh);
    try
    {
        parser->parse(fileName);
    }
    catch (const OutOfMemoryException&)
    {
	    fprintf(stderr, "OutOfMemoryException during parsing: %s\n", fileName);
	    return 0;
    }
    catch (const XMLException& e )
    {
		fprintf(stderr, "Exception Occurred \"%s\".  \n",
			XMLString::transcode(e.getMessage()));
		fprintf(stderr, "File being parsed is \"%s\".\n", fileName);
        return 0;  // A null document.
    }

	catch (...)
	{
		fprintf(stderr, "Unexpected Exception thrown during parse of file \"%s\".\n",
		                 fileName);
		return 0;
	}
    return parser->getDocument();
}


//------------------------------------------------------------------------
//
//  writeUData - Write out a udata xml element for a XMLCh* contents.
//
//------------------------------------------------------------------------
static void writeUData(const XMLCh* s)
{
    unsigned int i;
    printf("<udata>\n");
    size_t len = XMLString::stringLen(s);
    for (i=0; i<len; i++)
    {
        if (i % 16 == 0)
            printf("\n");
        XMLCh c = s[i];
        printf("%4x ", c);
    }
    printf("\n</udata>\n");
}



//------------------------------------------------------------------------
//
//  eatWhiteSpace -  XMLCh*s are kind of short on utility functions :-(
//
//------------------------------------------------------------------------
static void eatWhiteSpace(XMLCh* s, unsigned int &i)
{
    size_t len = XMLString::stringLen(s);
    while (i < len)
    {
    XMLCh c = s[i];
    if (!(c == 0x20 ||           // These are the official XML space characters,
        c == 0x09 ||             //   expressed as Unicode constants.
        c == 0x0A))
        break;
    i++;
    }
}

//------------------------------------------------------------------------
//
//   convertHexValue     if the XMLCh* contains a hex number at position i,
//                       convert it and return it, and update i to index the
//                       first char not in the string.
//                       return 0 if string[i] didn't have a hex digit.
//                       0 return is ambiguous, but it doesn't matter for XML,
//                       where 0 is not a valid character.
//
//------------------------------------------------------------------------
static int convertHexValue(XMLCh* s, unsigned int &i)
{
    int value = 0;

                                   // For reference, the digits  0-9 are Unicode 0x30-39
                                   //                the letters A-F are Unicode 0x41-0x46
                                   //                the letters a-f are Unicode 0x61-66
                                   // We can't use character literals - we might be
                                   //  building on an EBCDIC machine.
    size_t len = XMLString::stringLen(s);
    while (i < len)
    {
        XMLCh c = s[i];
        if (c >= 0x61 && c <= 0x66)     // Uppercase a-f to A-F.
            c -= 0x20;

        if (c < 0x30 || c >0x46)        // Stop if not a hex digit
            break;
        if (c > 0x39 && c <0x41)
            break;

        value = value << 4;             // Append this digit to accumulating value
        if (c <= 0x39)
            value += c-0x30;
        else
            value += 0xA + c - 0x41;

        i++;
    }
    return value;
}



//------------------------------------------------------------------------
//
//  processTestFile   Given the file name of an encoding test xml file,
//                    run it.
//
//------------------------------------------------------------------------
static bool  processTestFile(const XMLCh* fileName)
{
    //
    //  Send the input file through the parse, create a DOM document for it.
    //
    char cFileName[4000];
    XMLString::transcode(fileName, cFileName, 3999);
    DOMDocument* testDoc = parseFile(cFileName);
    if (testDoc == 0)
        return false;    // parse errors in the source xml.

    //
    //  Pull the "data" element out of the document.
    //
    XMLCh tempStr[4000];
    XMLString::transcode("data", tempStr, 3999);
    DOMNodeList* nl = testDoc->getElementsByTagName(tempStr);
    if (nl->getLength() != 1) {
        fprintf(stderr, "Test file \"%s\" must have exactly one \"data\" element.\n", cFileName);
        return false;
    };
    DOMNode* tmpNode = nl->item(0);
    DOMElement* data = (DOMElement*) tmpNode;


    //
    //  Build up a string containing the character data contents of the data element.
    //
    DOMNode* child;
    XMLBuffer elData;
    for (child=data->getFirstChild(); child != 0; child= child->getNextSibling())
    {
		if (child->getNodeType() == DOMNode::COMMENT_NODE)
			continue;
        if (! (child->getNodeType() == DOMNode::TEXT_NODE ||
               child->getNodeType() == DOMNode::CDATA_SECTION_NODE ||
               child->getNodeType() == DOMNode::ENTITY_REFERENCE_NODE))
        {
               fprintf(stderr, "Test file \"%s\": data element contains unexpected children.",
                    cFileName);
               return false;
        }
        elData.append(((DOMCharacterData *)child)->getData());
    };

    //
    //  Pull the "udata" element out of the document
    //
    XMLString::transcode("udata", tempStr, 3999);
    nl = testDoc->getElementsByTagName(tempStr);
    if (nl->getLength() != 1) {
        fprintf(stderr, "Test file \"%s\" must have exactly one \"udata\" element.\n", cFileName);
        return false;
    };
    DOMNode* tmpNode1 = nl->item(0);
    DOMElement* udata = (DOMElement*) tmpNode1;

    //
    //  Build up a string containing the character data contents of the udata element.
    //  This will consist of a whole bunch hex numbers, still in string from
    //

    XMLBuffer rawUData;
    for (child=udata->getFirstChild(); child != 0; child= child->getNextSibling())
    {
        if (child->getNodeType() == DOMNode::COMMENT_NODE)
            continue;
        if (! (child->getNodeType() == DOMNode::TEXT_NODE ||
            child->getNodeType() == DOMNode::CDATA_SECTION_NODE ||
            child->getNodeType() == DOMNode::ENTITY_REFERENCE_NODE))
        {
            fprintf(stderr, "Test file \"%s\": udata element contains unexpected children.",
                cFileName);
            return false;
        }
        rawUData.append(((DOMCharacterData *)child)->getData());
    };


    //
    // Convert the raw (hex numbers)  form of the udata to the corresponding string.
    //
    XMLBuffer uData;
    unsigned int rawIndex = 0;

    while (rawIndex < rawUData.getLen())
    {
        eatWhiteSpace(rawUData.getRawBuffer(), rawIndex);
        XMLCh c = convertHexValue(rawUData.getRawBuffer(), rawIndex);
        if (c > 0)
            uData.append(c);
        else
            if (rawIndex < rawUData.getLen())
            {
                fprintf(stderr, "Test file \"%s\": Bad hex number in udata element.  "
                        "Data character number %llu\n", cFileName,
                        (unsigned long long) uData.getLen());
                return false;
            }
    }


    //
    // Compare the two strings.
    //
    unsigned int i;
    for (i=0; i< elData.getLen(); i++)
    {
        XMLCh* elDataRaw = elData.getRawBuffer();
        XMLCh* uDataRaw = uData.getRawBuffer();
        if (i >= uData.getLen())
        {
            fprintf(stderr, "Test file \"%s\": udata element shorter than data at char number %d\n",
                cFileName, i);
            writeUData(elDataRaw);
            return false;
        }
        if (uDataRaw[i] != elDataRaw[i])
        {
            fprintf(stderr, "Test file \"%s\": comparison failure at character number %d\n",
                cFileName, i);
            writeUData(elDataRaw);
            return false;
        };
    }

    if (elData.getLen() != uData.getLen())
    {
        fprintf(stderr, "Test file \"%s\": udata element longer than data at char number %d\n",
            cFileName, i);
        writeUData(elData.getRawBuffer());
        return false;
    }

    return true;
}


int main(int argc, char ** argv) {

   //
    // Initialize the Xerces-c environment
    //
	try
    {
        XMLPlatformUtils::Initialize();
    }

    catch (const XMLException& toCatch)
    {
        fprintf(stderr, "Error during initialization of xerces-c: %s\n",
            XMLString::transcode(toCatch.getMessage()));
         return 1;
    }

    //
    // Parse the command line, which should specify exactly one file, which is an
    //   xml file containing the list of test files to be processed.
    //
    if (argc != 2) {
        printf("usage: %s file_name \n"
               "   where file name is the xml file specifying the list of test files.", argv[0]);
        return 1;
    }
    DOMDocument* fileListDoc = parseFile(argv[1]);
    if (fileListDoc == 0) return 1;


    //
    // Iterate over the list of files, running each as a test.
    //
    XMLCh tempStr[4000];
    XMLString::transcode("testFile", tempStr, 3999);
    DOMNodeList* list = fileListDoc->getElementsByTagName(tempStr);
    XMLSize_t i;
    XMLSize_t numFiles = list->getLength();
    for (i=0; i<numFiles; i++)
    {
        ++gTestsRun;
        DOMNode* tmpNode3 = list->item(i);
        XMLString::transcode("name", tempStr, 3999);
        const XMLCh* fileName = ((DOMElement*) tmpNode3)->getAttribute(tempStr);
        if (processTestFile(fileName) == false)
            ++gTestsFailed;
    };



    //
    // We are done.  Print out a summary of the results
    //
    printf("Encoding Tests Results Summary: \n"
           "   %d encoding tests run.\n"
           "   %d tests passed,\n"
           "   %d tests failed\n", gTestsRun, gTestsRun-gTestsFailed, gTestsFailed);

    delete parser;
    parser = 0;
   return 0;
}
