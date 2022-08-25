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
#if HAVE_CONFIG_H
#	include <config.h>
#endif
// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include <xercesc/framework/XMLGrammarPoolImpl.hpp>
#include <xercesc/internal/MemoryManagerImpl.hpp>
#include <xercesc/internal/XSerializationException.hpp>
#include <xercesc/internal/BinMemOutputStream.hpp>
#include <xercesc/util/BinMemInputStream.hpp>

#include "XSerializerTest.hpp"

#if defined(XERCES_NEW_IOSTREAMS)
#include <fstream>
#else
#include <fstream.h>
#endif

// ---------------------------------------------------------------------------
//  command line option variables
// ---------------------------------------------------------------------------
static const char*                  xmlFile            = 0;
static SAX2XMLReader::ValSchemes    valScheme          = SAX2XMLReader::Val_Auto;
static bool                         doNamespaces       = true;
static bool                         doSchema           = true;
static bool                         schemaFullChecking = false;
static bool                         doList             = false;
static bool                         namespacePrefixes  = false;
static bool                         errorOccurred      = false;
static bool                         recognizeNEL       = false;

static char                         localeStr[64];

// ---------------------------------------------------------------------------
//  parsing components
// ---------------------------------------------------------------------------
static XSerializerHandlers*     handler     = 0;

/***
*
*   This program is a variation of SAX2Count.
*
*   Whenever a file is served, it does the following:
*
*   . parses the file
*   . caches the grammar without issuing any error message with regards to the parsing
*   . serializes(store) the grammar cached to a BinOutputStream
*   . deserialize(load) the grammar from the BinInputStream
*   . parses the instance document a second time
*   . validates the instance against the deserialized grammar if validation is on.
*
***/

#include <stdio.h>

static
void parseCase(const char* const xmlFile);

static
bool parseOne(BinOutputStream*    outStream
            , const char* const  xmlFile);

static
void parseTwo(BinInputStream*     inStream
            , const char* const  xmlFile);

static
void parseFile(SAX2XMLReader* const parser
             , const char* const xmlFile);

static
SAX2XMLReader* getParser(XMLGrammarPool* const theGramPool
                       , bool                  setHandler);

// ---------------------------------------------------------------------------
//  Local helper methods
// ---------------------------------------------------------------------------
void usage()
{
    XERCES_STD_QUALIFIER cout << "\nUsage:\n"
            "    XSerializerTest [options] <XML file | List file>\n\n"
            "This program invokes the SAX2XMLReader, and then prints the\n"
            "number of elements, attributes, spaces and characters found\n"
            "in each XML file, using SAX2 API.\n\n"
            "Options:\n"
            "    -l          Indicate the input file is a List File that has a list of xml files.\n"
            "                Default to off (Input file is an XML file).\n"
            "    -v=xxx      Validation scheme [always | never | auto*].\n"
            "    -f          Enable full schema constraint checking processing. Defaults to off.\n"
            "    -p          Enable namespace-prefixes feature. Defaults to off.\n"
            "    -n          Disable namespace processing. Defaults to on.\n"
            "                NOTE: THIS IS OPPOSITE FROM OTHER SAMPLES.\n"
            "    -s          Disable schema processing. Defaults to on.\n"
            "                NOTE: THIS IS OPPOSITE FROM OTHER SAMPLES.\n"
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
                valScheme = SAX2XMLReader::Val_Never;
            else if (!strcmp(parm, "auto"))
                valScheme = SAX2XMLReader::Val_Auto;
            else if (!strcmp(parm, "always"))
                valScheme = SAX2XMLReader::Val_Always;
            else
            {
                XERCES_STD_QUALIFIER cerr << "Unknown -v= value: " << parm << XERCES_STD_QUALIFIER endl;
                return 2;
            }
        }
         else if (!strcmp(argV[argInd], "-n")
              ||  !strcmp(argV[argInd], "-N"))
        {
            doNamespaces = false;
        }
         else if (!strcmp(argV[argInd], "-s")
              ||  !strcmp(argV[argInd], "-S"))
        {
            doSchema = false;
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
         else if (!strcmp(argV[argInd], "-p")
              ||  !strcmp(argV[argInd], "-P"))
        {
            namespacePrefixes = true;
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

        parseCase(xmlFile);
    }

    if (doList)
        fin.close();

    // And call the termination method
    XMLPlatformUtils::Terminate();

    if (errorOccurred)
        return 4;
    else
        return 0;

}

// ---------------------------------------------------------------------------
//  Serialization/Deserialization
// ---------------------------------------------------------------------------
static const int BufSize = 1024;

static void parseCase(const char* const xmlFile)
{
    //
    //  Create our SAX handler object and install it on the parser, as the
    //  document and error handler.
    //
    if (!handler)
        handler = new XSerializerHandlers();

    BinOutputStream* myOut = new BinMemOutputStream(BufSize);
    Janitor<BinOutputStream> janOut(myOut);

    if (!parseOne(myOut, xmlFile))
        return;

    BinInputStream*  myIn  = new BinMemInputStream(
                                                   ((BinMemOutputStream*)myOut)->getRawBuffer()
                                                 , (XMLSize_t)((BinMemOutputStream*)myOut)->getSize()
                                                 , BinMemInputStream::BufOpt_Reference
                                                  );
    Janitor<BinInputStream> janIn(myIn);

    parseTwo(myIn, xmlFile);

}

static
bool parseOne(BinOutputStream*    outStream
            , const char* const   xmlFile)
{
    //we don't use janitor here
    MemoryManager*  theMemMgr   = new MemoryManagerImpl();
    XMLGrammarPool* theGramPool = new XMLGrammarPoolImpl(theMemMgr);
    SAX2XMLReader*  theParser   = getParser(theGramPool, false);  //don't emit error
    bool            retVal      = true;

    theParser->setFeature(XMLUni::fgXercesCacheGrammarFromParse, true);

    //scan instance document and cache grammar
    try
    {
        theParser->parse(xmlFile);
    }
    catch (...)
    {
        //do nothing, it could be an invalid instance document, but the grammar is fine
    }

    //serialize the grammar pool
    try
    {
        theGramPool->serializeGrammars(outStream);
    }
    catch (const XSerializationException& e)
    {
        //do emit error here so that we know serialization failure
        XERCES_STD_QUALIFIER cerr << "An error occurred during serialization\n   Message: "
            << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;

        retVal = false;
    }

    catch (...)
    {
        //do emit error here so that we know serialization failure
        XERCES_STD_QUALIFIER cerr << "An error occurred during serialization\n" << XERCES_STD_QUALIFIER endl;

        retVal = false;
    }

    //the order is important
    delete theParser;
    delete theGramPool;
    delete theMemMgr;

    return retVal;
}

static
void parseTwo(BinInputStream*     inStream
            , const char* const   xmlFile)
{
    //we don't use janitor here
    MemoryManager*  theMemMgr   = new MemoryManagerImpl();
    XMLGrammarPool* theGramPool = new XMLGrammarPoolImpl(theMemMgr);
    bool            errorSeen   = false;

    //de-serialize grammar pool
    try
    {
        theGramPool->deserializeGrammars(inStream);
    }

    catch(const XSerializationException& e)
    {
        XERCES_STD_QUALIFIER cerr << "An error occurred during de-serialization\n   Message: "
            << StrX(e.getMessage()) << XERCES_STD_QUALIFIER endl;

        errorSeen = true;
    }

    catch (...)
    {
        //do emit error here so that we know serialization failure
        XERCES_STD_QUALIFIER cerr << "An error occurred during de-serialization\n" << XERCES_STD_QUALIFIER endl;

        errorSeen = true;
    }

    if (!errorSeen)
    {
        SAX2XMLReader*  theParser   = getParser(theGramPool, true); //set the handler

        theParser->setFeature(XMLUni::fgXercesUseCachedGrammarInParse, true);
        parseFile(theParser, xmlFile);
        delete theParser;
    }

    //the order is important
    delete theGramPool;
    delete theMemMgr;

    return;
}

static SAX2XMLReader* getParser(XMLGrammarPool* const theGramPool
                              , bool                  setHandler)
{
    SAX2XMLReader* parser = XMLReaderFactory::createXMLReader(theGramPool->getMemoryManager(), theGramPool);

    parser->setFeature(XMLUni::fgSAX2CoreNameSpaces, doNamespaces);
    parser->setFeature(XMLUni::fgXercesSchema, doSchema);
    parser->setFeature(XMLUni::fgXercesHandleMultipleImports, true);
    parser->setFeature(XMLUni::fgXercesSchemaFullChecking, schemaFullChecking);
    parser->setFeature(XMLUni::fgSAX2CoreNameSpacePrefixes, namespacePrefixes);

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

    if (setHandler)
    {
        parser->setContentHandler(handler);
        parser->setErrorHandler(handler);
    }
    else
    {
        parser->setContentHandler(0);
        parser->setErrorHandler(0);
    }

    return parser;
}

static void parseFile(SAX2XMLReader* const parser
                    , const char* const xmlFile)
{
    //
    //  Get the starting time and kick off the parse of the indicated
    //  file. Catch any exceptions that might propogate out of it.
    //
    unsigned long duration;

    //reset error count first
    handler->resetErrors();

    try
    {
        const unsigned long startMillis = XMLPlatformUtils::getCurrentMillis();
        parser->parse(xmlFile);
        const unsigned long endMillis = XMLPlatformUtils::getCurrentMillis();
        duration = endMillis - startMillis;
    }
    catch (const XMLException& e)
    {
        XERCES_STD_QUALIFIER cerr << "\nError during parsing: '" << xmlFile << "'\n"
            << "Exception message is:  \n"
            << StrX(e.getMessage()) << "\n" << XERCES_STD_QUALIFIER endl;
        errorOccurred = true;
    }
    catch (...)
    {
        XERCES_STD_QUALIFIER cerr << "\nUnexpected exception during parsing: '" << xmlFile << "'\n";
        errorOccurred = true;
    }

    // Print out the stats that we collected and time taken
    if (!handler->getSawErrors())
    {
        XERCES_STD_QUALIFIER cout << xmlFile << ": " << duration << " ms ("
            << handler->getElementCount() << " elems, "
            << handler->getAttrCount() << " attrs, "
            << handler->getSpaceCount() << " spaces, "
            << handler->getCharacterCount() << " chars)" << XERCES_STD_QUALIFIER endl;
    }
    else
        errorOccurred = true;

}
