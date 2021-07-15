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

/*
 * This sample illustrates how you can create a DOM tree in memory.
 * It then prints the count of elements in the tree.
 */


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>

#if defined(XERCES_NEW_IOSTREAMS)
#include <iostream>
#else
#include <iostream.h>
#endif


XERCES_CPP_NAMESPACE_USE

// ---------------------------------------------------------------------------
//  This is a simple class that lets us do easy (though not terribly efficient)
//  trancoding of char* data to XMLCh data.
// ---------------------------------------------------------------------------
class XStr
{
public :
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    XStr(const char* const toTranscode)
    {
        // Call the private transcoding method
        fUnicodeForm = XMLString::transcode(toTranscode);
    }

    ~XStr()
    {
        XMLString::release(&fUnicodeForm);
    }


    // -----------------------------------------------------------------------
    //  Getter methods
    // -----------------------------------------------------------------------
    const XMLCh* unicodeForm() const
    {
        return fUnicodeForm;
    }

private :
    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  fUnicodeForm
    //      This is the Unicode XMLCh format of the string.
    // -----------------------------------------------------------------------
    XMLCh*   fUnicodeForm;
};

#define X(str) XStr(str).unicodeForm()


// ---------------------------------------------------------------------------
//  main
// ---------------------------------------------------------------------------

int main(int argC, char*[])
{
    // Initialize the XML4C2 system.
    try
    {
        XMLPlatformUtils::Initialize();
    }

    catch(const XMLException& toCatch)
    {
        char *pMsg = XMLString::transcode(toCatch.getMessage());
        XERCES_STD_QUALIFIER cerr << "Error during Xerces-c Initialization.\n"
             << "  Exception message:"
             << pMsg;
        XMLString::release(&pMsg);
        return 1;
    }

    // Watch for special case help request
    int errorCode = 0;
    if (argC > 1)
    {
        XERCES_STD_QUALIFIER cout << "\nUsage:\n"
                "    CreateDOMDocument\n\n"
                "This program creates a new DOM document from scratch in memory.\n"
                "It then prints the count of elements in the tree.\n"
             << XERCES_STD_QUALIFIER endl;
        errorCode = 1;
    }
    if(errorCode) {
        XMLPlatformUtils::Terminate();
        return errorCode;
    }

   {
       //  Nest entire test in an inner block.
       //  The tree we create below is the same that the XercesDOMParser would
       //  have created, except that no whitespace text nodes would be created.

       // <company>
       //     <product>Xerces-C</product>
       //     <category idea='great'>XML Parsing Tools</category>
       //     <developedBy>Apache Software Foundation</developedBy>
       // </company>

       DOMImplementation* impl =  DOMImplementationRegistry::getDOMImplementation(X("Core"));

       if (impl != NULL)
       {
           try
           {
               DOMDocument* doc = impl->createDocument(
                           0,                    // root element namespace URI.
                           X("company"),         // root element name
                           0);                   // document type object (DTD).

               DOMElement* rootElem = doc->getDocumentElement();

               DOMElement*  prodElem = doc->createElement(X("product"));
               rootElem->appendChild(prodElem);

               DOMText*    prodDataVal = doc->createTextNode(X("Xerces-C"));
               prodElem->appendChild(prodDataVal);

               DOMElement*  catElem = doc->createElement(X("category"));
               rootElem->appendChild(catElem);

               catElem->setAttribute(X("idea"), X("great"));

               DOMText*    catDataVal = doc->createTextNode(X("XML Parsing Tools"));
               catElem->appendChild(catDataVal);

               DOMElement*  devByElem = doc->createElement(X("developedBy"));
               rootElem->appendChild(devByElem);

               DOMText*    devByDataVal = doc->createTextNode(X("Apache Software Foundation"));
               devByElem->appendChild(devByDataVal);

               //
               // Now count the number of elements in the above DOM tree.
               //

               const XMLSize_t elementCount = doc->getElementsByTagName(X("*"))->getLength();
               XERCES_STD_QUALIFIER cout << "The tree just created contains: " << elementCount
                    << " elements." << XERCES_STD_QUALIFIER endl;

               doc->release();
           }
           catch (const OutOfMemoryException&)
           {
               XERCES_STD_QUALIFIER cerr << "OutOfMemoryException" << XERCES_STD_QUALIFIER endl;
               errorCode = 5;
           }
           catch (const DOMException& e)
           {
               XERCES_STD_QUALIFIER cerr << "DOMException code is:  " << e.code << XERCES_STD_QUALIFIER endl;
               errorCode = 2;
           }
           catch (...)
           {
               XERCES_STD_QUALIFIER cerr << "An error occurred creating the document" << XERCES_STD_QUALIFIER endl;
               errorCode = 3;
           }
       }  // (inpl != NULL)
       else
       {
           XERCES_STD_QUALIFIER cerr << "Requested implementation is not supported" << XERCES_STD_QUALIFIER endl;
           errorCode = 4;
       }
   }

   XMLPlatformUtils::Terminate();
   return errorCode;
}
