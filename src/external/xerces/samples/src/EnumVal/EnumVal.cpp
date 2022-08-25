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
#include <xercesc/util/NameIdPool.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/XMLValidator.hpp>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/validators/DTD/DTDValidator.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>    
#if defined(XERCES_NEW_IOSTREAMS)
#include <iostream>
#else
#include <iostream.h>
#endif
#include <stdlib.h>
#include <string.h>


XERCES_CPP_NAMESPACE_USE

// ---------------------------------------------------------------------------
//  Forward references
// ---------------------------------------------------------------------------
static void usage();



// ---------------------------------------------------------------------------
//  This is a simple class that lets us do easy (though not terribly efficient)
//  trancoding of XMLCh data to local code page for display.
// ---------------------------------------------------------------------------
class StrX
{
public :
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    StrX(const XMLCh* const toTranscode)
    {
        // Call the private transcoding method
        fLocalForm = XMLString::transcode(toTranscode);
    }

    ~StrX()
    {
        XMLString::release(&fLocalForm);
    }


    // -----------------------------------------------------------------------
    //  Getter methods
    // -----------------------------------------------------------------------
    const char* localForm() const
    {
        return fLocalForm;
    }


private :
    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  fLocalForm
    //      This is the local code page form of the string.
    // -----------------------------------------------------------------------
    char*   fLocalForm;
};

inline XERCES_STD_QUALIFIER ostream& operator<<(XERCES_STD_QUALIFIER ostream& target, const StrX& toDump)
{
    target << toDump.localForm();
    return target;
}


// ---------------------------------------------------------------------------
//  Local helper methods
// ---------------------------------------------------------------------------
static void usage()
{
    XERCES_STD_QUALIFIER cout << "\nUsage:\n"
            "    EnumVal <XML file>\n\n"
            "This program parses the specified XML file, then shows how to\n"
            "enumerate the contents of the DTD Grammar. Essentially,\n"
            "shows how one can access the DTD information stored in internal\n"
            "data structures.\n"
         << XERCES_STD_QUALIFIER endl;
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
         XERCES_STD_QUALIFIER cerr   << "Error during initialization! Message:\n"
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

    // We only have one required parameter, which is the file to process
    if ((argC != 2) || (*(argV[1]) == '-'))
    {
        usage();
        XMLPlatformUtils::Terminate();
        return 1;
    }

    const char*              xmlFile   = argV[1];
    SAXParser::ValSchemes    valScheme = SAXParser::Val_Auto;

    //
    //  Create a DTD validator to be used for our validation work. Then create
    //  a SAX parser object and pass it our validator. Then, according to what
    //  we were told on the command line, set it to validate or not. He owns
    //  the validator, so we have to allocate it.
    //
    int errorCount = 0;
    DTDValidator* valToUse = new DTDValidator;
    SAXParser* parser = new SAXParser(valToUse);
    parser->setValidationScheme(valScheme);

    //
    //  Get the starting time and kick off the parse of the indicated
    //  file. Catch any exceptions that might propogate out of it.
    //
    int errorCode = 0;
    try
    {
        parser->parse(xmlFile);
        errorCount = parser->getErrorCount();
    }
    catch (const OutOfMemoryException&)
    {
        XERCES_STD_QUALIFIER cerr << "OutOfMemoryException" << XERCES_STD_QUALIFIER endl;
        errorCode = 5;
    }
    catch (const XMLException& e)
    {
        XERCES_STD_QUALIFIER cerr << "\nError during parsing: '" << xmlFile << "'\n"
             << "Exception message is:  \n"
             << StrX(e.getMessage()) << "\n" << XERCES_STD_QUALIFIER endl;
        errorCode = 4;
    }
    if(errorCode) {
        XMLPlatformUtils::Terminate();
        return errorCode;
    }

    if (!errorCount) {
        //
        //  Now we will get an enumerator for the element pool from the validator
        //  and enumerate the elements, printing them as we go. For each element
        //  we get an enumerator for its attributes and print them also.
        //
        DTDGrammar* grammar = (DTDGrammar*) valToUse->getGrammar();
        NameIdPoolEnumerator<DTDElementDecl> elemEnum = grammar->getElemEnumerator();
        if (elemEnum.hasMoreElements())
        {
            XERCES_STD_QUALIFIER cout << "\nELEMENTS:\n----------------------------\n";
            while(elemEnum.hasMoreElements())
            {
                const DTDElementDecl& curElem = elemEnum.nextElement();
                XERCES_STD_QUALIFIER cout << "  Name: " << StrX(curElem.getFullName()) << "\n";

                XERCES_STD_QUALIFIER cout << "  Content Model: "
                     << StrX(curElem.getFormattedContentModel())
                     << "\n";

                // Get an enumerator for this guy's attributes if any
                if (curElem.hasAttDefs())
                {
                    XERCES_STD_QUALIFIER cout << "  Attributes:\n";
                    XMLAttDefList& attList = curElem.getAttDefList();
                    for (unsigned int i=0; i<attList.getAttDefCount(); i++)
                    {
                        const XMLAttDef& curAttDef = attList.getAttDef(i);
                        XERCES_STD_QUALIFIER cout << "    Name:" << StrX(curAttDef.getFullName())
                             << ", Type: ";

                        // Get the type and display it
                        const XMLAttDef::AttTypes type = curAttDef.getType();
                        switch(type)
                        {
                            case XMLAttDef::CData :
                                XERCES_STD_QUALIFIER cout << "CDATA";
                                break;

                            case XMLAttDef::ID :
                                XERCES_STD_QUALIFIER cout << "ID";
                                break;

                            case XMLAttDef::IDRef :
                            case XMLAttDef::IDRefs :
                                XERCES_STD_QUALIFIER cout << "IDREF(S)";
                                break;

                            case XMLAttDef::Entity :
                            case XMLAttDef::Entities :
                                XERCES_STD_QUALIFIER cout << "ENTITY(IES)";
                                break;

                            case XMLAttDef::NmToken :
                            case XMLAttDef::NmTokens :
                                XERCES_STD_QUALIFIER cout << "NMTOKEN(S)";
                                break;

                            case XMLAttDef::Notation :
                                XERCES_STD_QUALIFIER cout << "NOTATION";
                                break;

                            case XMLAttDef::Enumeration :
                                XERCES_STD_QUALIFIER cout << "ENUMERATION";
                                break;
                            default:
                                break;
                        }

                        XERCES_STD_QUALIFIER cout << "\n";
                    }
                }
                XERCES_STD_QUALIFIER cout << XERCES_STD_QUALIFIER endl;
            }
        }
         else
        {
            XERCES_STD_QUALIFIER cout << "The validator has no elements to display\n" << XERCES_STD_QUALIFIER endl;
        }
    }
    else
        XERCES_STD_QUALIFIER cout << "\nErrors occurred, no output available\n" << XERCES_STD_QUALIFIER endl;

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


