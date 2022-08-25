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
#include <xercesc/parsers/SAXParser.hpp>
#include "SAXPrint.hpp"
#include <xercesc/util/OutOfMemoryException.hpp>

// ---------------------------------------------------------------------------
//  Local data
//
//  doNamespaces
//      Indicates whether namespace processing should be enabled or not.
//      Defaults to disabled.
//
//  doSchema
//      Indicates whether schema processing should be enabled or not.
//      Defaults to disabled.
//
//  schemaFullChecking
//      Indicates whether full schema constraint checking should be enabled or not.
//      Defaults to disabled.
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
// ---------------------------------------------------------------------------
static bool                     doNamespaces        = false;
static bool                     doSchema            = false;
static bool                     schemaFullChecking  = false;
static const char*              encodingName    = "LATIN1";
static XMLFormatter::UnRepFlags unRepFlags      = XMLFormatter::UnRep_CharRef;
static char*                    xmlFile         = 0;
static SAXParser::ValSchemes    valScheme       = SAXParser::Val_Auto;



// ---------------------------------------------------------------------------
//  Local helper methods
// ---------------------------------------------------------------------------
static void usage()
{
    XERCES_STD_QUALIFIER cout << "\nUsage:\n"
            "    SAXPrint [options] <XML file>\n\n"
            "This program invokes the SAX Parser, and then prints the\n"
            "data returned by the various SAX handlers for the specified\n"
            "XML file.\n\n"
            "Options:\n"
             "    -u=xxx      Handle unrepresentable chars [fail | rep | ref*].\n"
             "    -v=xxx      Validation scheme [always | never | auto*].\n"
             "    -n          Enable namespace processing.\n"
             "    -s          Enable schema processing.\n"
             "    -f          Enable full schema constraint checking.\n"
             "    -x=XXX      Use a particular encoding for output (LATIN1*).\n"
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
                valScheme = SAXParser::Val_Never;
            else if (!strcmp(parm, "auto"))
                valScheme = SAXParser::Val_Auto;
            else if (!strcmp(parm, "always"))
                valScheme = SAXParser::Val_Always;
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
            doNamespaces = true;
        }
         else if (!strcmp(argV[parmInd], "-s")
              ||  !strcmp(argV[parmInd], "-S"))
        {
            doSchema = true;
        }
         else if (!strcmp(argV[parmInd], "-f")
              ||  !strcmp(argV[parmInd], "-F"))
        {
            schemaFullChecking = true;
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
    int errorCount = 0;

    //
    //  Create a SAX parser object. Then, according to what we were told on
    //  the command line, set it to validate or not.
    //
    SAXParser* parser = new SAXParser;
    parser->setValidationScheme(valScheme);
    parser->setDoNamespaces(doNamespaces);
    parser->setDoSchema(doSchema);
    parser->setHandleMultipleImports (true);
    parser->setValidationSchemaFullChecking(schemaFullChecking);

    //
    //  Create the handler object and install it as the document and error
    //  handler for the parser-> Then parse the file and catch any exceptions
    //  that propogate out
    //
    int errorCode = 0;
    try
    {
        SAXPrintHandlers handler(encodingName, unRepFlags);
        parser->setDocumentHandler(&handler);
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
    delete parser;

    // And call the termination method
    XMLPlatformUtils::Terminate();

    if (errorCount > 0)
        return 4;
    else
        return 0;
}
