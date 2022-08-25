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
//  This sample program demonstrates the progressive parse capabilities of
//  the parser system. It allows you to do a scanFirst() call followed by
//  a loop which calls scanNext(). You can drop out when you've found what
//  ever it is you want. In our little test, our event handler looks for
//  16 new elements then sets a flag to indicate its found what it wants.
//  At that point, our progressive parse loop below exits.
//
//  The parameters are:
//
//      [-?]            - Show usage and exit
//      [-v=xxx]        - Validation scheme [always | never | auto*]
//      [-n]            - Enable namespace processing
//      [-s]            - Enable schema processing
//      [-f]            - Enable full schema constraint checking
//      filename        - The path to the XML file to parse
//
//  * = Default if not provided explicitly
//  These are non-case sensitive
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/XMLPScanToken.hpp>
#include <xercesc/parsers/SAXParser.hpp>
#include "PParse.hpp"
#include <xercesc/util/OutOfMemoryException.hpp>

// ---------------------------------------------------------------------------
//  Local data
//
//  xmlFile
//      The path to the file to parser. Set via command line.
//
//  doNamespaces
//      Indicates whether namespace processing should be done.
//
//  doSchema
//      Indicates whether schema processing should be done.
//
//  schemaFullChecking
//      Indicates whether full schema constraint checking should be done.
//
//  valScheme
//      Indicates what validation scheme to use. It defaults to 'auto', but
//      can be set via the -v= command.
// ---------------------------------------------------------------------------
static char*	 xmlFile         = 0;
static bool     doNamespaces       = false;
static bool     doSchema           = false;
static bool     schemaFullChecking = false;
static SAXParser::ValSchemes    valScheme       = SAXParser::Val_Auto;



// ---------------------------------------------------------------------------
//  Local helper methods
// ---------------------------------------------------------------------------
static void usage()
{
    XERCES_STD_QUALIFIER cout << "\nUsage:\n"
            "    PParse [options] <XML file>\n\n"
            "This program demonstrates the progressive parse capabilities of\n"
	         "the parser system. It allows you to do a scanFirst() call followed by\n"
            "a loop which calls scanNext(). You can drop out when you've found what\n"
            "ever it is you want. In our little test, our event handler looks for\n"
            "16 new elements then sets a flag to indicate its found what it wants.\n"
            "At that point, our progressive parse loop exits.\n\n"
            "Options:\n"
            "      -v=xxx        - Validation scheme [always | never | auto*].\n"
            "      -n            - Enable namespace processing [default is off].\n"
            "      -s            - Enable schema processing [default is off].\n"
            "      -f            - Enable full schema constraint checking [default is off].\n"
            "      -?            - Show this help.\n\n"
            "  * = Default if not provided explicitly.\n"
         <<  XERCES_STD_QUALIFIER endl;
}



// ---------------------------------------------------------------------------
//  Program entry point
// ---------------------------------------------------------------------------
int main(int argC, char* argV[])
{
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
    //  Create a SAX parser object to use and create our SAX event handlers
    //  and plug them in.
    //
    SAXParser* parser = new SAXParser;
    PParseHandlers handler;
    parser->setDocumentHandler(&handler);
    parser->setErrorHandler(&handler);
    parser->setValidationScheme(valScheme);
    parser->setDoNamespaces(doNamespaces);
    parser->setDoSchema(doSchema);
    parser->setHandleMultipleImports (true);
    parser->setValidationSchemaFullChecking(schemaFullChecking);

    //
    //  Ok, lets do the progressive parse loop. On each time around the
    //  loop, we look and see if the handler has found what its looking
    //  for. When it does, we fall out then.
    //
    unsigned long duration;
    int errorCode = 0;
    try
    {
        // Create a progressive scan token
        XMLPScanToken token;

        const unsigned long startMillis = XMLPlatformUtils::getCurrentMillis();
        if (!parser->parseFirst(xmlFile, token))
        {
            XERCES_STD_QUALIFIER cerr << "scanFirst() failed\n" << XERCES_STD_QUALIFIER endl;
            XMLPlatformUtils::Terminate();
            return 1;
        }

        //
        //  We started ok, so lets call scanNext() until we find what we want
        //  or hit the end.
        //
        bool gotMore = true;
        while (gotMore && !parser->getErrorCount())
            gotMore = parser->parseNext(token);

        const unsigned long endMillis = XMLPlatformUtils::getCurrentMillis();
        duration = endMillis - startMillis;

        errorCount = parser->getErrorCount();
        //
        //  Reset the parser-> In this simple progrma, since we just exit
        //  now, its not technically required. But, in programs which
        //  would remain open, you should reset after a progressive parse
        //  in case you broke out before the end of the file. This insures
        //  that all opened files, sockets, etc... are closed.
        //
        parser->parseReset(token);
    }
    catch (const OutOfMemoryException&)
    {
        XERCES_STD_QUALIFIER cerr << "OutOfMemoryException" << XERCES_STD_QUALIFIER endl;
        errorCode = 5;
    }
    catch (const XMLException& toCatch)
    {
        XERCES_STD_QUALIFIER cerr << "\nAn error occurred: '" << xmlFile << "'\n"
             << "Exception message is: \n"
             << StrX(toCatch.getMessage())
             << "\n" << XERCES_STD_QUALIFIER endl;
        errorCode = 4;
    }

    if(errorCode) {
        XMLPlatformUtils::Terminate();
        return errorCode;
    }

    if (!errorCount) {
        XERCES_STD_QUALIFIER cout << xmlFile << ": " << duration << " ms ("
            << handler.getElementCount() << " elems, "
            << handler.getAttrCount() << " attrs, "
            << handler.getSpaceCount() << " spaces, "
            << handler.getCharacterCount() << " chars)" << XERCES_STD_QUALIFIER endl;
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
