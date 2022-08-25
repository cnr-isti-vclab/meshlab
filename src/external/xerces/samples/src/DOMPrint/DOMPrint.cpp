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
//  This sample program invokes the XercesDOMParser to build a DOM tree for
//  the specified input file. It then invokes DOMLSSerializer::write() to
//  serialize the resultant DOM tree back to XML stream.
//
//  Note:
//  Application needs to provide its own implementation of
//		   DOMErrorHandler (in this sample, the DOMPrintErrorHandler),
//		   if it would like to receive notification from the serializer
//		   in the case any error occurs during the serialization.
//
//  Application needs to provide its own implementation of
//		   DOMLSSerializerFilter (in this sample, the DOMPrintFilter),
//		   if it would like to filter out certain part of the DOM
//		   representation, but must be aware that thus may render the
//		   resultant XML stream invalid.
//
//  Application may choose any combination of characters as the
//		   end of line sequence to be used in the resultant XML stream,
//		   but must be aware that thus may render the resultant XML
//		   stream ill formed.
//
//  Application may choose a particular encoding name in which
//		   the output XML stream would be, but must be aware that if
//		   characters, unrepresentable in the encoding specified, appearing
//		   in markups, may force the serializer to terminate serialization
//		   prematurely, and thus no complete serialization would be done.
//
//  Application shall query the serializer first, before set any
//           feature/mode(true, false), or be ready to catch exception if this
//           feature/mode is not supported by the serializer.
//
//  Application needs to clean up the filter, error handler and
//		   format target objects created for the serialization.
//
//   Limitations:
//      1.  The encoding="xxx" clause in the XML header should reflect
//          the system local code page, but does not.
//      2.  Cases where the XML data contains characters that can not
//          be represented in the system local code page are not handled.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include <xercesc/util/PlatformUtils.hpp>

#include <xercesc/dom/DOM.hpp>

#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>

#include "DOMTreeErrorReporter.hpp"
#include "DOMPrintFilter.hpp"
#include "DOMPrintErrorHandler.hpp"
#include <xercesc/util/OutOfMemoryException.hpp>

#include <string.h>
#include <stdlib.h>

// ---------------------------------------------------------------------------
//  Local data
//
//  gXmlFile
//      The path to the file to parser. Set via command line.
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
//  gDoCreate
//      Indicates whether entity reference nodes needs to be created or not
//      Defaults to false
//
//  gOutputEncoding
//      The encoding we are to output in. If not set on the command line,
//      then it is defaults to the encoding of the input XML file.
//
//  gSplitCdataSections
//      Indicates whether split-cdata-sections is to be enabled or not.
//
//  gDiscardDefaultContent
//      Indicates whether default content is discarded or not.
//
//  gUseFilter
//      Indicates if user wants to plug in the DOMPrintFilter.
//
//  gValScheme
//      Indicates what validation scheme to use. It defaults to 'auto', but
//      can be set via the -v= command.
//
// ---------------------------------------------------------------------------
static char*                    gXmlFile               = 0;
static bool                     gDoNamespaces          = false;
static bool                     gDoSchema              = false;
static bool                     gSchemaFullChecking    = false;
static bool                     gDoCreate              = false;

static char*                    goutputfile            = 0;
static char*                    gXPathExpression       = 0;

// options for DOMLSSerializer's features
static XMLCh*                   gOutputEncoding        = 0;

static bool                     gSplitCdataSections    = true;
static bool                     gDiscardDefaultContent = true;
static bool                     gUseFilter             = false;
static bool                     gFormatPrettyPrint     = false;
static bool                     gXMLDeclaration        = true;
static bool                     gWriteBOM              = false;

static XercesDOMParser::ValSchemes    gValScheme       = XercesDOMParser::Val_Auto;


//	Prototypes for internally used functions
void usage();


// ---------------------------------------------------------------------------
//
//  Usage()
//
// ---------------------------------------------------------------------------
void usage()
{
    XERCES_STD_QUALIFIER cout << "\nUsage:\n"
            "    DOMPrint [options] <XML file>\n\n"
            "This program invokes the DOM parser, and builds the DOM tree.\n"
            "It then asks the DOMLSSerializer to serialize the DOM tree.\n"
            "Options:\n"
            "    -e          create entity reference nodes. Default is no expansion.\n"
            "    -v=xxx      Validation scheme [always | never | auto*].\n"
            "    -n          Enable namespace processing. Default is off.\n"
            "    -s          Enable schema processing. Default is off.\n"
            "    -f          Enable full schema constraint checking. Defaults is off.\n"
            "    -wenc=XXX   Use a particular encoding for output. Default is\n"
            "                the same encoding as the input XML file. UTF-8 if\n"
            "                input XML file has not XML declaration.\n"
            "    -wfile=xxx  Write to a file instead of stdout.\n"
            "    -wscs=xxx   Enable/Disable split-cdata-sections.      Default on\n"
            "    -wddc=xxx   Enable/Disable discard-default-content.   Default on\n"
            "    -wflt=xxx   Enable/Disable filtering.                 Default off\n"
            "    -wfpp=xxx   Enable/Disable format-pretty-print.       Default off\n"
            "    -wfdecl=xxx Enable/Disable xml-declaration.           Default on\n"
            "    -wbom=xxx   Enable/Disable write Byte-Order-Mark      Default off\n"
            "    -xpath=xxx  Prints only the nodes matching the given XPath.\n"
            "    -?          Show this help.\n\n"
            "  * = Default if not provided explicitly.\n\n"
            "The parser has intrinsic support for the following encodings:\n"
            "    UTF-8, US-ASCII, ISO8859-1, UTF-16[BL]E, UCS-4[BL]E,\n"
            "    WINDOWS-1252, IBM1140, IBM037, IBM1047.\n"
          <<  XERCES_STD_QUALIFIER endl;
}

// ---------------------------------------------------------------------------
//
//  main
//
// ---------------------------------------------------------------------------
int main(int argC, char* argV[])
{
    int retval = 0;

    // Initialize the XML4C2 system
    try
    {
        XMLPlatformUtils::Initialize();
    }

    catch(const XMLException &toCatch)
    {
        XERCES_STD_QUALIFIER cerr << "Error during Xerces-c Initialization.\n"
             << "  Exception message:"
             << StrX(toCatch.getMessage()) << XERCES_STD_QUALIFIER endl;
        return 1;
    }

    // Check command line and extract arguments.
    if (argC < 2)
    {
        usage();
        XMLPlatformUtils::Terminate();
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
            XMLPlatformUtils::Terminate();
            return 2;
        }
         else if (!strncmp(argV[parmInd], "-v=", 3)
              ||  !strncmp(argV[parmInd], "-V=", 3))
        {
            const char* const parm = &argV[parmInd][3];

            if (!strcmp(parm, "never"))
                gValScheme = XercesDOMParser::Val_Never;
            else if (!strcmp(parm, "auto"))
                gValScheme = XercesDOMParser::Val_Auto;
            else if (!strcmp(parm, "always"))
                gValScheme = XercesDOMParser::Val_Always;
            else
            {
                XERCES_STD_QUALIFIER cerr << "Unknown -v= value: " << parm << XERCES_STD_QUALIFIER endl;
                XMLPlatformUtils::Terminate();
                return 2;
            }
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
         else if (!strcmp(argV[parmInd], "-e")
              ||  !strcmp(argV[parmInd], "-E"))
        {
            gDoCreate = true;
        }
         else if (!strncmp(argV[parmInd], "-wenc=", 6))
        {
             // Get out the encoding name
             gOutputEncoding = XMLString::transcode( &(argV[parmInd][6]) );
        }
         else if (!strncmp(argV[parmInd], "-wfile=", 7))
        {
             goutputfile =  &(argV[parmInd][7]);
        }
         else if (!strncmp(argV[parmInd], "-wddc=", 6))
        {
            const char* const parm = &argV[parmInd][6];

            if (!strcmp(parm, "on"))
				gDiscardDefaultContent = true;
            else if (!strcmp(parm, "off"))
				gDiscardDefaultContent = false;
            else
            {
                XERCES_STD_QUALIFIER cerr << "Unknown -wddc= value: " << parm << XERCES_STD_QUALIFIER endl;
                XMLPlatformUtils::Terminate();
                return 2;
            }

        }
         else if (!strncmp(argV[parmInd], "-wscs=", 6))
        {
            const char* const parm = &argV[parmInd][6];

            if (!strcmp(parm, "on"))
				gSplitCdataSections = true;
			else if (!strcmp(parm, "off"))
				gSplitCdataSections = false;
            else
            {
                XERCES_STD_QUALIFIER cerr << "Unknown -wscs= value: " << parm << XERCES_STD_QUALIFIER endl;
                XMLPlatformUtils::Terminate();
                return 2;
            }
        }
         else if (!strncmp(argV[parmInd], "-wflt=", 6))
        {
            const char* const parm = &argV[parmInd][6];

            if (!strcmp(parm, "on"))
				gUseFilter = true;
			else if (!strcmp(parm, "off"))
				gUseFilter = false;
            else
            {
                XERCES_STD_QUALIFIER cerr << "Unknown -wflt= value: " << parm << XERCES_STD_QUALIFIER endl;
                XMLPlatformUtils::Terminate();
                return 2;
            }
        }
         else if (!strncmp(argV[parmInd], "-wfpp=", 6))
        {
            const char* const parm = &argV[parmInd][6];

            if (!strcmp(parm, "on"))
				gFormatPrettyPrint = true;
			else if (!strcmp(parm, "off"))
				gFormatPrettyPrint = false;
            else
            {
                XERCES_STD_QUALIFIER cerr << "Unknown -wfpp= value: " << parm << XERCES_STD_QUALIFIER endl;
                XMLPlatformUtils::Terminate();
                return 2;
            }
        }
         else if (!strncmp(argV[parmInd], "-wfdecl=", 8))
        {
             const char* const parm = &argV[parmInd][8];

             if (!strcmp(parm, "on"))
                 gXMLDeclaration = true;
             else if (!strcmp(parm, "off"))
                 gXMLDeclaration = false;
             else
             {
                 XERCES_STD_QUALIFIER cerr << "Unknown -wfdecl= value: " << parm << XERCES_STD_QUALIFIER endl;
                 XMLPlatformUtils::Terminate();
                 return 2;
             }
        }
         else if (!strncmp(argV[parmInd], "-wbom=", 6))
        {
            const char* const parm = &argV[parmInd][6];

            if (!strcmp(parm, "on"))
                gWriteBOM = true;
            else if (!strcmp(parm, "off"))
                gWriteBOM = false;
            else
            {
                XERCES_STD_QUALIFIER cerr << "Unknown -wbom= value: " << parm << XERCES_STD_QUALIFIER endl;
                XMLPlatformUtils::Terminate();
                return 2;
            }
        }
         else if (!strncmp(argV[parmInd], "-xpath=", 7))
        {
             gXPathExpression = &(argV[parmInd][7]);
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
        XMLPlatformUtils::Terminate();
        return 1;
    }
    gXmlFile = argV[parmInd];

    //
    //  Create our parser, then attach an error handler to the parser.
    //  The parser will call back to methods of the ErrorHandler if it
    //  discovers errors during the course of parsing the XML document.
    //
    XercesDOMParser *parser = new XercesDOMParser;
    parser->setValidationScheme(gValScheme);
    parser->setDoNamespaces(gDoNamespaces);
    parser->setDoSchema(gDoSchema);
    parser->setHandleMultipleImports (true);
    parser->setValidationSchemaFullChecking(gSchemaFullChecking);
    parser->setCreateEntityReferenceNodes(gDoCreate);

    DOMTreeErrorReporter *errReporter = new DOMTreeErrorReporter();
    parser->setErrorHandler(errReporter);

    //
    //  Parse the XML file, catching any XML exceptions that might propogate
    //  out of it.
    //
    bool errorsOccured = false;
    try
    {
        parser->parse(gXmlFile);
    }
    catch (const OutOfMemoryException&)
    {
        XERCES_STD_QUALIFIER cerr << "OutOfMemoryException" << XERCES_STD_QUALIFIER endl;
        errorsOccured = true;
    }
    catch (const XMLException& e)
    {
        XERCES_STD_QUALIFIER cerr << "An error occurred during parsing\n   Message: "
             << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;
        errorsOccured = true;
    }

    catch (const DOMException& e)
    {
        const unsigned int maxChars = 2047;
        XMLCh errText[maxChars + 1];

        XERCES_STD_QUALIFIER cerr << "\nDOM Error during parsing: '" << gXmlFile << "'\n"
             << "DOMException code is:  " << e.code << XERCES_STD_QUALIFIER endl;

        if (DOMImplementation::loadDOMExceptionMsg(e.code, errText, maxChars))
             XERCES_STD_QUALIFIER cerr << "Message is: " << StrX(errText) << XERCES_STD_QUALIFIER endl;

        errorsOccured = true;
    }

    catch (...)
    {
        XERCES_STD_QUALIFIER cerr << "An error occurred during parsing\n " << XERCES_STD_QUALIFIER endl;
        errorsOccured = true;
    }

    // If the parse was successful, output the document data from the DOM tree
    if (!errorsOccured && !errReporter->getSawErrors())
    {
        DOMPrintFilter   *myFilter = 0;

        try
        {
            // get a serializer, an instance of DOMLSSerializer
            XMLCh tempStr[3] = {chLatin_L, chLatin_S, chNull};
            DOMImplementation *impl          = DOMImplementationRegistry::getDOMImplementation(tempStr);
            DOMLSSerializer   *theSerializer = ((DOMImplementationLS*)impl)->createLSSerializer();
            DOMLSOutput       *theOutputDesc = ((DOMImplementationLS*)impl)->createLSOutput();

            // set user specified output encoding
            theOutputDesc->setEncoding(gOutputEncoding);

            // plug in user's own filter
            if (gUseFilter)
            {
                // even we say to show attribute, but the DOMLSSerializer
                // will not show attribute nodes to the filter as
                // the specs explicitly says that DOMLSSerializer shall
                // NOT show attributes to DOMLSSerializerFilter.
                //
                // so DOMNodeFilter::SHOW_ATTRIBUTE has no effect.
                // same DOMNodeFilter::SHOW_DOCUMENT_TYPE, no effect.
                //
                myFilter = new DOMPrintFilter(DOMNodeFilter::SHOW_ELEMENT   |
                                              DOMNodeFilter::SHOW_ATTRIBUTE |
                                              DOMNodeFilter::SHOW_DOCUMENT_TYPE);
                theSerializer->setFilter(myFilter);
            }

            // plug in user's own error handler
            DOMErrorHandler *myErrorHandler = new DOMPrintErrorHandler();
            DOMConfiguration* serializerConfig=theSerializer->getDomConfig();
            serializerConfig->setParameter(XMLUni::fgDOMErrorHandler, myErrorHandler);

            // set feature if the serializer supports the feature/mode
            if (serializerConfig->canSetParameter(XMLUni::fgDOMWRTSplitCdataSections, gSplitCdataSections))
                serializerConfig->setParameter(XMLUni::fgDOMWRTSplitCdataSections, gSplitCdataSections);

            if (serializerConfig->canSetParameter(XMLUni::fgDOMWRTDiscardDefaultContent, gDiscardDefaultContent))
                serializerConfig->setParameter(XMLUni::fgDOMWRTDiscardDefaultContent, gDiscardDefaultContent);

            if (serializerConfig->canSetParameter(XMLUni::fgDOMWRTFormatPrettyPrint, gFormatPrettyPrint))
                serializerConfig->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, gFormatPrettyPrint);

            if (serializerConfig->canSetParameter(XMLUni::fgDOMWRTBOM, gWriteBOM))
                serializerConfig->setParameter(XMLUni::fgDOMWRTBOM, gWriteBOM);
            
            if (serializerConfig->canSetParameter(XMLUni::fgDOMXMLDeclaration, gXMLDeclaration))
                serializerConfig->setParameter(XMLUni::fgDOMXMLDeclaration, gXMLDeclaration);

            //
            // Plug in a format target to receive the resultant
            // XML stream from the serializer.
            //
            // StdOutFormatTarget prints the resultant XML stream
            // to stdout once it receives any thing from the serializer.
            //
            XMLFormatTarget *myFormTarget;
            if (goutputfile)
                myFormTarget=new LocalFileFormatTarget(goutputfile);
            else
                myFormTarget=new StdOutFormatTarget();
            theOutputDesc->setByteStream(myFormTarget);

            // get the DOM representation
            DOMDocument *doc = parser->getDocument();

            //
            // do the serialization through DOMLSSerializer::write();
            //
            if(gXPathExpression!=NULL)
            {
                XMLCh* xpathStr=XMLString::transcode(gXPathExpression);
                DOMElement* root = doc->getDocumentElement();
                try
                {
                    DOMXPathNSResolver* resolver=doc->createNSResolver(root);
                    DOMXPathResult* result=doc->evaluate(
                      xpathStr,
                      root,
                      resolver,
                      DOMXPathResult::ORDERED_NODE_SNAPSHOT_TYPE,
                      NULL);

                    XMLSize_t nLength = result->getSnapshotLength();
                    for(XMLSize_t i = 0; i < nLength; i++)
                    {
                      result->snapshotItem(i);
                      theSerializer->write(result->getNodeValue(), theOutputDesc);
                    }

                    result->release();
                    resolver->release ();
                }
                catch(const DOMXPathException& e)
                {
                    XERCES_STD_QUALIFIER cerr << "An error occurred during processing of the XPath expression. Msg is:"
                        << XERCES_STD_QUALIFIER endl
                        << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;
                    retval = 4;
                }
                catch(const DOMException& e)
                {
                    XERCES_STD_QUALIFIER cerr << "An error occurred during processing of the XPath expression. Msg is:"
                        << XERCES_STD_QUALIFIER endl
                        << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;
                    retval = 4;
                }
                XMLString::release(&xpathStr);
            }
            else
                theSerializer->write(doc, theOutputDesc);

            theOutputDesc->release();
            theSerializer->release();

            //
            // Filter, formatTarget and error handler
            // are NOT owned by the serializer.
            //
            delete myFormTarget;
            delete myErrorHandler;

            if (gUseFilter)
                delete myFilter;

        }
        catch (const OutOfMemoryException&)
        {
            XERCES_STD_QUALIFIER cerr << "OutOfMemoryException" << XERCES_STD_QUALIFIER endl;
            retval = 5;
        }
        catch (const DOMLSException& e)
        {
            XERCES_STD_QUALIFIER cerr << "An error occurred during serialization of the DOM tree. Msg is:"
                << XERCES_STD_QUALIFIER endl
                << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;
            retval = 4;
        }
        catch (const XMLException& e)
        {
            XERCES_STD_QUALIFIER cerr << "An error occurred during creation of output transcoder. Msg is:"
                << XERCES_STD_QUALIFIER endl
                << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;
            retval = 4;
        }

    }
    else
        retval = 4;

    //
    //  Clean up the error handler. The parser does not adopt handlers
    //  since they could be many objects or one object installed for multiple
    //  handlers.
    //
    delete errReporter;

    //
    //  Delete the parser itself.  Must be done prior to calling Terminate, below.
    //
    delete parser;

    XMLString::release(&gOutputEncoding);

    // And call the termination method
    XMLPlatformUtils::Terminate();

    return retval;
}
