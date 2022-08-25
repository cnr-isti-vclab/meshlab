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
#include <xercesc/util/TransService.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include "SAX2Print.hpp"
#include <xercesc/util/OutOfMemoryException.hpp>
#include "SAX2FilterHandlers.hpp"

// ---------------------------------------------------------------------------
//  Local data
//
//  encodingName
//      The encoding we are to output in. If not set on the command line,
//      then it is defaulted to LATIN1.
//
//  xmlFile
//      The path to the file to parser. Set via command line.
//
//  valScheme
//      Indicates what validation scheme to use. It defaults to 'auto', but
//      can be set via the -v= command.
//
//	expandNamespaces
//		Indicates if the output should expand the namespaces Alias with
//		their URI's, defaults to false, can be set via the command line -e
// ---------------------------------------------------------------------------
static const char*              encodingName    = "LATIN1";
static XMLFormatter::UnRepFlags unRepFlags      = XMLFormatter::UnRep_CharRef;
static char*                    xmlFile         = 0;
static SAX2XMLReader::ValSchemes valScheme      = SAX2XMLReader::Val_Auto;
static bool					        expandNamespaces= false ;
static bool                     doNamespaces    = true;
static bool                     doSchema        = true;
static bool                     schemaFullChecking = false;
static bool                     namespacePrefixes = false;
static bool                     sortAttributes  = false;


// ---------------------------------------------------------------------------
//  Local helper methods
// ---------------------------------------------------------------------------
static void usage()
{
    XERCES_STD_QUALIFIER cout << "\nUsage:\n"
            "    SAX2Print [options] <XML file>\n\n"
            "This program invokes the SAX2XMLReader, and then prints the\n"
            "data returned by the various SAX2 handlers for the specified\n"
            "XML file.\n\n"
            "Options:\n"
             "    -u=xxx      Handle unrepresentable chars [fail | rep | ref*].\n"
             "    -v=xxx      Validation scheme [always | never | auto*].\n"
             "    -e          Expand Namespace Alias with URI's. Defaults to off.\n"
             "    -x=XXX      Use a particular encoding for output (LATIN1*).\n"
             "    -f          Enable full schema constraint checking processing. Defaults to off.\n"
             "    -p          Enable namespace-prefixes feature. Defaults to off.\n"
             "    -n          Disable namespace processing. Defaults to on.\n"
             "                NOTE: THIS IS OPPOSITE FROM OTHER SAMPLES.\n"
             "    -s          Disable schema processing. Defaults to on.\n"
             "                NOTE: THIS IS OPPOSITE FROM OTHER SAMPLES.\n"
             "    -sa         Print the attributes in alphabetic order. Defaults to off.\n"
             "    -?          Show this help.\n\n"
             "  * = Default if not provided explicitly.\n\n"
             "The parser has intrinsic support for the following encodings:\n"
             "    UTF-8, US-ASCII, ISO8859-1, UTF-16[BL]E, UCS-4[BL]E,\n"
             "    WINDOWS-1252, IBM1140, IBM037, IBM1047.\n"
         <<  XERCES_STD_QUALIFIER endl;
}



// ---------------------------------------------------------------------------
//  Program entry point
// ---------------------------------------------------------------------------
int main(int argC, char* argV[])
{
    // Initialize the XML4C2 system
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

    // Check command line and extract arguments.
    if (argC < 2)
    {
        usage();
        XMLPlatformUtils::Terminate();
        return 1;
    }

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
                valScheme = SAX2XMLReader::Val_Never;
            else if (!strcmp(parm, "auto"))
                valScheme = SAX2XMLReader::Val_Auto;
            else if (!strcmp(parm, "always"))
                valScheme = SAX2XMLReader::Val_Always;
            else
            {
                XERCES_STD_QUALIFIER cerr << "Unknown -v= value: " << parm << XERCES_STD_QUALIFIER endl;
                XMLPlatformUtils::Terminate();
                return 2;
            }
        }
         else if (!strcmp(argV[parmInd], "-e")
              ||  !strcmp(argV[parmInd], "-E"))
        {
            expandNamespaces = true;
        }
         else if (!strncmp(argV[parmInd], "-x=", 3)
              ||  !strncmp(argV[parmInd], "-X=", 3))
        {
            // Get out the encoding name
            encodingName = &argV[parmInd][3];
        }
         else if (!strncmp(argV[parmInd], "-u=", 3)
              ||  !strncmp(argV[parmInd], "-U=", 3))
        {
            const char* const parm = &argV[parmInd][3];

            if (!strcmp(parm, "fail"))
                unRepFlags = XMLFormatter::UnRep_Fail;
            else if (!strcmp(parm, "rep"))
                unRepFlags = XMLFormatter::UnRep_Replace;
            else if (!strcmp(parm, "ref"))
                unRepFlags = XMLFormatter::UnRep_CharRef;
            else
            {
                XERCES_STD_QUALIFIER cerr << "Unknown -u= value: " << parm << XERCES_STD_QUALIFIER endl;
                XMLPlatformUtils::Terminate();
                return 2;
            }
        }
         else if (!strcmp(argV[parmInd], "-n")
              ||  !strcmp(argV[parmInd], "-N"))
        {
            doNamespaces = false;
        }
         else if (!strcmp(argV[parmInd], "-s")
              ||  !strcmp(argV[parmInd], "-S"))
        {
            doSchema = false;
        }
         else if (!strcmp(argV[parmInd], "-f")
              ||  !strcmp(argV[parmInd], "-F"))
        {
            schemaFullChecking = true;
        }
         else if (!strcmp(argV[parmInd], "-p")
              ||  !strcmp(argV[parmInd], "-P"))
        {
            namespacePrefixes = true;
        }
         else if (!strcmp(argV[parmInd], "-sa"))
        {
            sortAttributes = true;
        }
         else
        {
            XERCES_STD_QUALIFIER cerr << "Unknown option '" << argV[parmInd]
                 << "', ignoring it\n" << XERCES_STD_QUALIFIER endl;
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
    xmlFile = argV[parmInd];

    //
    //  Create a SAX parser object. Then, according to what we were told on
    //  the command line, set it to validate or not.
    //
    SAX2XMLReader* parser;
    SAX2XMLReader* reader = XMLReaderFactory::createXMLReader();
    SAX2XMLReader* filter = NULL;
    if(sortAttributes)
    {
        filter=new SAX2SortAttributesFilter(reader);
        parser=filter;
    }
    else
        parser=reader;

    //
    //  Then, according to what we were told on
    //  the command line, set it to validate or not.
    //
    if (valScheme == SAX2XMLReader::Val_Auto)
    {
        parser->setFeature(XMLUni::fgSAX2CoreValidation, true);
        parser->setFeature(XMLUni::fgXercesDynamic, true);
    }

    if (valScheme == SAX2XMLReader::Val_Never)
    {
        parser->setFeature(XMLUni::fgSAX2CoreValidation, false);
    }

    if (valScheme == SAX2XMLReader::Val_Always)
    {
        parser->setFeature(XMLUni::fgSAX2CoreValidation, true);
        parser->setFeature(XMLUni::fgXercesDynamic, false);
    }

    parser->setFeature(XMLUni::fgSAX2CoreNameSpaces, doNamespaces);
    parser->setFeature(XMLUni::fgXercesSchema, doSchema);
    parser->setFeature(XMLUni::fgXercesHandleMultipleImports, true);
    parser->setFeature(XMLUni::fgXercesSchemaFullChecking, schemaFullChecking);
    parser->setFeature(XMLUni::fgSAX2CoreNameSpacePrefixes, namespacePrefixes);

    //
    //  Create the handler object and install it as the document and error
    //  handler for the parser. Then parse the file and catch any exceptions
    //  that propogate out
    //

	XMLSize_t errorCount = 0;
    int errorCode = 0;
    try
    {
        SAX2PrintHandlers handler(encodingName, unRepFlags, expandNamespaces);
        parser->setContentHandler(&handler);
        parser->setErrorHandler(&handler);
        parser->parse(xmlFile);
        errorCount = parser->getErrorCount();
    }
    catch (const OutOfMemoryException&)
    {
        XERCES_STD_QUALIFIER cerr << "OutOfMemoryException" << XERCES_STD_QUALIFIER endl;
        errorCode = 5;
    }
    catch (const XMLException& toCatch)
    {
        XERCES_STD_QUALIFIER cerr << "\nAn error occurred\n  Error: "
             << StrX(toCatch.getMessage())
             << "\n" << XERCES_STD_QUALIFIER endl;
        errorCode = 4;
    }

    if(errorCode) {
        XMLPlatformUtils::Terminate();
        return errorCode;
    }

    //
    //  Delete the parser itself.  Must be done prior to calling Terminate, below.
    //
    delete reader;
    if(filter)
        delete filter;

    // And call the termination method
    XMLPlatformUtils::Terminate();

    if (errorCount > 0)
        return 4;
    else
        return 0;
}
