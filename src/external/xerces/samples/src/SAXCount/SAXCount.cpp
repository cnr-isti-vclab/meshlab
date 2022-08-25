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
#include "SAXCount.hpp"
#if defined(XERCES_NEW_IOSTREAMS)
#include <fstream>
#else
#include <fstream.h>
#endif
#include <xercesc/util/OutOfMemoryException.hpp>

// ---------------------------------------------------------------------------
//  Local helper methods
// ---------------------------------------------------------------------------
void usage()
{
    XERCES_STD_QUALIFIER cout << "\nUsage:\n"
            "    SAXCount [options] <XML file | List file>\n\n"
            "This program invokes the SAX Parser, and then prints the\n"
            "number of elements, attributes, spaces and characters found\n"
            "in each XML file, using SAX API.\n\n"
            "Options:\n"
            "    -l          Indicate the input file is a List File that has a list of xml files.\n"
            "                Default to off (Input file is an XML file).\n"
            "    -v=xxx      Validation scheme [always | never | auto*].\n"
            "    -n          Enable namespace processing. Defaults to off.\n"
            "    -s          Enable schema processing. Defaults to off.\n"
            "    -f          Enable full schema constraint checking. Defaults to off.\n"
            "    -locale=ll_CC specify the locale, default: en_US.\n"
		    "    -?          Show this help.\n\n"
            "  * = Default if not provided explicitly.\n"
         << XERCES_STD_QUALIFIER endl;
}


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

    const char*              xmlFile = 0;
    SAXParser::ValSchemes    valScheme = SAXParser::Val_Auto;
    bool                     doNamespaces       = false;
    bool                     doSchema           = false;
    bool                     schemaFullChecking = false;
    bool                     doList = false;
    bool                     errorOccurred = false;
    bool                     recognizeNEL = false;
    char                     localeStr[64];
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
                valScheme = SAXParser::Val_Never;
            else if (!strcmp(parm, "auto"))
                valScheme = SAXParser::Val_Auto;
            else if (!strcmp(parm, "always"))
                valScheme = SAXParser::Val_Always;
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
         else if (!strcmp(argV[argInd], "-special:nel"))
        {
            // turning this on will lead to non-standard compliance behaviour
            // it will recognize the unicode character 0x85 as new line character
            // instead of regular character as specified in XML 1.0
            // do not turn this on unless really necessary
             recognizeNEL = true;
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
    //  There should at least one parameter left, and that
    //  should be the file name(s).
    //
    if (argInd == argC)
    {
        usage();
        return 1;
    }

    // Initialize the XML4C2 system
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
        XERCES_STD_QUALIFIER cerr << "Error during initialization! Message:\n"
            << StrX(toCatch.getMessage()) << XERCES_STD_QUALIFIER endl;
        return 1;
    }

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
    //  Create our SAX handler object and install it on the parser, as the
    //  document and error handler.
    //
    SAXCountHandlers handler;
    parser->setDocumentHandler(&handler);
    parser->setErrorHandler(&handler);


    //
    //  Get the starting time and kick off the parse of the indicated
    //  file. Catch any exceptions that might propogate out of it.
    //
    unsigned long duration;

    XERCES_STD_QUALIFIER ifstream fin;

    // the input is a list file
    if (doList)
        fin.open(argV[argInd]);

    if (fin.fail()) {
        XERCES_STD_QUALIFIER cerr <<"Cannot open the list file: " << argV[argInd] << XERCES_STD_QUALIFIER endl;
        return 2;
    }

    while (true)
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
            if (argInd < argC)
            {
                 xmlFile = argV[argInd];
                 argInd++;
            }
            else
                break;
        }

        //reset error count first
        handler.resetErrors();

        try
        {
            const unsigned long startMillis = XMLPlatformUtils::getCurrentMillis();
            parser->parse(xmlFile);
            const unsigned long endMillis = XMLPlatformUtils::getCurrentMillis();
            duration = endMillis - startMillis;
        }
        catch (const OutOfMemoryException&)
        {
            XERCES_STD_QUALIFIER cerr << "OutOfMemoryException" << XERCES_STD_QUALIFIER endl;
            errorOccurred = true;
            continue;
        }
        catch (const XMLException& e)
        {
            XERCES_STD_QUALIFIER cerr << "\nError during parsing: '" << xmlFile << "'\n"
                << "Exception message is:  \n"
                << StrX(e.getMessage()) << "\n" << XERCES_STD_QUALIFIER endl;
            errorOccurred = true;
            continue;
        }

        catch (...)
        {
            XERCES_STD_QUALIFIER cerr << "\nUnexpected exception during parsing: '" << xmlFile << "'\n";
            errorOccurred = true;
            continue;
        }


        // Print out the stats that we collected and time taken
        if (!handler.getSawErrors())
        {
            XERCES_STD_QUALIFIER cout << xmlFile << ": " << duration << " ms ("
                << handler.getElementCount() << " elems, "
                << handler.getAttrCount() << " attrs, "
                << handler.getSpaceCount() << " spaces, "
                << handler.getCharacterCount() << " chars)" << XERCES_STD_QUALIFIER endl;
        }
        else
            errorOccurred = true;
    }

    if (doList)
        fin.close();

    //
    //  Delete the parser itself.  Must be done prior to calling Terminate, below.
    //
    delete parser;

    // And call the termination method
    XMLPlatformUtils::Terminate();

    if (errorOccurred)
        return 4;
    else
        return 0;

}
