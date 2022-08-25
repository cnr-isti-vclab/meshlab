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

//REVISIT
/*
 * $Id$
 */


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include "PSVIWriter.hpp"
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/parsers/SAX2XMLReaderImpl.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>

#if defined(XERCES_NEW_IOSTREAMS)
#include <fstream>
#else
#include <fstream.h>
#endif
// ---------------------------------------------------------------------------
//  Local helper methods
// ---------------------------------------------------------------------------
void usage()
{
	XERCES_STD_QUALIFIER cout << "\nUsage:\n"
			"    PSVIWriter [options] <XML file | List file>\n\n"
			"This program invokes the SAX2XMLReaderImpl, and then exposes the\n"
			"underlying PSVI of each parsed XML file, using SAX2 API.\n\n"
			"Options:\n"
			"    -f          Enable full schema constraint checking processing. Defaults to off.\n"
			"    -o=xxx      Output PSVI to file xxx (default is stdout)\n"
			"    -e=xxx      Output errors to file xxx (default is stdout)\n"
			"    -u=xxx      Handle unrepresentable chars [fail | rep | ref*].\n"
			"    -x=XXX      Use a particular encoding for output (UTF8*).\n"
            "    -l          Indicate the input file is a List File that has a list of xml files.\n"
            "                Default to off (Input file is an XML file).\n"
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

    static const char* encodingName		= "UTF8";
    static XMLFormatter::UnRepFlags unRepFlags = XMLFormatter::UnRep_CharRef;
    const char*						xmlFile				= 0;
    bool							doList				= false; //REVISIT
    bool							schemaFullChecking	= false;
    bool							errorOccurred		= false;
    const char*						psviOut				= 0;
    const char*						errorOut			= 0;
    XMLFormatTarget* 				psviTarget 			= 0;
    XMLFormatTarget* 				errorTarget 		= 0;
    XMLFormatter* 					psviFormatter 		= 0;
    XMLFormatter* 					errorFormatter 		= 0;
    char							fileName[80]		="";


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
         else if (!strcmp(argV[argInd], "-l")
              ||  !strcmp(argV[argInd], "-L"))
        {
            doList = true;
        }
         else if (!strcmp(argV[argInd], "-f")
              ||  !strcmp(argV[argInd], "-F"))
        {
            schemaFullChecking = true;
        }
         else if (!strncmp(argV[argInd], "-o=", 3)
              ||  !strncmp(argV[argInd], "-O=", 3))
        {
            psviOut = &argV[argInd][3];
        }
         else if (!strncmp(argV[argInd], "-e=", 3)
              ||  !strncmp(argV[argInd], "-E=", 3))
        {
            errorOut = &argV[argInd][3];
        }
         else if (!strncmp(argV[argInd], "-x=", 3)
              ||  !strncmp(argV[argInd], "-X=", 3))
        {
            // Get out the encoding name
            encodingName = &argV[argInd][3];
        }
         else if (!strncmp(argV[argInd], "-u=", 3)
              ||  !strncmp(argV[argInd], "-U=", 3))
        {
            const char* const parm = &argV[argInd][3];

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

    // Initialize the XML4C2 system
    try
    {
        XMLPlatformUtils::Initialize();
    } catch (const XMLException& toCatch) {
        XERCES_STD_QUALIFIER cerr << "Error during initialization! Message:\n"
            << StrX(toCatch.getMessage()) << XERCES_STD_QUALIFIER endl;
        return 1;
    }

    //
    //  Create a SAX parser object, then set it to validate or not.
    //
    SAX2XMLReaderImpl* parser = new SAX2XMLReaderImpl();
    parser->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);
    parser->setFeature(XMLUni::fgXercesSchema, true);
    parser->setFeature(XMLUni::fgXercesHandleMultipleImports, true);
    parser->setFeature(XMLUni::fgXercesSchemaFullChecking, schemaFullChecking);
    parser->setFeature(XMLUni::fgSAX2CoreNameSpacePrefixes, false);
    parser->setFeature(XMLUni::fgSAX2CoreValidation, true);
    parser->setFeature(XMLUni::fgXercesDynamic, true);

	//
	//	Based on commandline arguments, create XMLFormatters for PSVI output and errors
	//
	if (!doList) {
	    if (psviOut==0) {
	    	psviTarget = new StdOutFormatTarget();
	    } else {
	    	psviTarget = new LocalFileFormatTarget(psviOut);
	    }
		psviFormatter = new XMLFormatter(encodingName, psviTarget, XMLFormatter::NoEscapes, unRepFlags);
    }

    if (errorOut==0) {
    	errorTarget = new StdOutFormatTarget();
    } else {
    	errorTarget = new LocalFileFormatTarget(errorOut);
    }
	errorFormatter = new XMLFormatter(encodingName, errorTarget, XMLFormatter::NoEscapes, unRepFlags);

  	//
    //  Create our SAX handler object and install it as the handlers
    //

   	PSVIWriterHandlers* handler;
   	if (doList)
   		handler = new PSVIWriterHandlers(0, errorFormatter);
   	else
   		handler = new PSVIWriterHandlers(psviFormatter, errorFormatter);

    PSVIAdvancedHandler* advancedHandler = new PSVIAdvancedHandler(handler);
    parser->installAdvDocHandler(advancedHandler);

	parser->setPSVIHandler(handler);
    parser->setContentHandler(handler);
    parser->setLexicalHandler(handler);
	parser->setXMLEntityResolver(handler);
    parser->setErrorHandler(handler);

    bool more = true;
    XERCES_STD_QUALIFIER ifstream fin;

    // the input is a list file
    if (doList) //REVISIT
        fin.open(argV[argInd]);

    if (fin.fail()) {
        XERCES_STD_QUALIFIER cerr <<"Cannot open the list file: " << argV[argInd] << XERCES_STD_QUALIFIER endl;
        return 2;
    }

    while (more) //REVISIT
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
                    xmlFile =fURI;
                    XMLString::trim((char*)xmlFile);
                    XERCES_STD_QUALIFIER cerr << "==Parsing== \"" << xmlFile << "\"" << XERCES_STD_QUALIFIER endl;
                }

				if (psviOut==0) {
					if (psviTarget==0 && psviFormatter==0) {
			    		psviTarget = new StdOutFormatTarget();
						psviFormatter = new XMLFormatter(encodingName, psviTarget, XMLFormatter::NoEscapes, unRepFlags);
						handler->resetPSVIFormatter(psviFormatter);
			    	}
			    } else {
					strcpy(fileName, psviOut);
					if (strrchr(xmlFile, '\\')>strrchr(xmlFile, '/')) {
						strcat(fileName, strrchr(xmlFile, '\\'));
					} else {
                        if (strrchr(xmlFile, '/'))
                            strcat(fileName, strrchr(xmlFile, '/'));
					}
	                if (psviFormatter)
	                	delete psviFormatter;
	                if (psviTarget)
	                	delete psviTarget;
			    	psviTarget = new LocalFileFormatTarget(fileName);
					psviFormatter = new XMLFormatter(encodingName, psviTarget, XMLFormatter::NoEscapes, unRepFlags);
					handler->resetPSVIFormatter(psviFormatter);
			    }
            }
            else
                break;
        }
        else {
            xmlFile = argV[argInd];
            more = false; //REVISIT
        }

        //reset error count first
        handler->resetErrors();

        try
        {
            parser->parse(xmlFile);
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
            XERCES_STD_QUALIFIER cerr << "\nUnexpected exception during parsing: '" << xmlFile << "'\n" << XERCES_STD_QUALIFIER endl;;
            errorOccurred = true;
            continue;
        }
    }

    if (doList) //REVISIT
        fin.close();

    //
    //  Delete the parser itself.  Must be done prior to calling Terminate, below.
    //
    delete parser;
    delete advancedHandler;
    delete handler;
    delete psviFormatter;
    delete errorFormatter;
    delete psviTarget;
    delete errorTarget;

    // And call the termination method
    XMLPlatformUtils::Terminate();

    if (errorOccurred)
        return 4;
    else
        return 0;

}
