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
//  This test program is used to verify support for C++11 char16_t as
//  XMLCh, along with direct use of u"" UTF-16 string literals with the
//  DOM API.
//
//  This test requires C++11.
//
//---------------------------------------------------------------------

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>

#include <xercesc/dom/DOM.hpp>

#include <cassert>
#include <iostream>
#include <string>

int main() {
    try
    {
        xercesc::XMLPlatformUtils::Initialize();
    }
    catch (const xercesc::XMLException& toCatch)
    {
      std::cerr << "Error during initialization of xerces-c: "
                << toCatch.getMessage()
                << std::endl;
        return 1;
    }

    const char16_t *ns = u"https://example.com/schema/char16";

    int errorCode = 0;

    xercesc::DOMImplementation* impl = xercesc::DOMImplementationRegistry::getDOMImplementation(u"LS");
    if(impl)
    {
        try
        {
            xercesc::DOMDocument* doc = impl->createDocument
                (ns, // root element namespace URI.
                 u"catalogue", // root element name
                 nullptr);

            xercesc::DOMElement* rootElem = doc->getDocumentElement();

            xercesc::DOMElement*  prodElem = doc->createElementNS(ns, u"produit");
            rootElem->appendChild(prodElem);

            xercesc::DOMText*    prodDataVal = doc->createTextNode(u"Xerces-C");
            prodElem->appendChild(prodDataVal);

            xercesc::DOMElement*  catElem = doc->createElementNS(ns, u"catégorie");
            rootElem->appendChild(catElem);

            catElem->setAttribute(u"idée", u"génial");

            xercesc::DOMText*    catDataVal = doc->createTextNode(u"produit");
            catElem->appendChild(catDataVal);

            xercesc::DOMElement*  devByElem = doc->createElementNS(ns, u"développéPar");
            rootElem->appendChild(devByElem);

            xercesc::DOMText*    devByDataVal = doc->createTextNode(u"Fondation du logiciel Apache");
            devByElem->appendChild(devByDataVal);

            assert(std::u16string(u"produit") == prodElem->getTagName());
            assert(std::u16string(u"catégorie") == catElem->getTagName());
            assert(std::u16string(u"génial") == catElem->getAttribute(u"idée"));
            assert(std::u16string(u"développéPar") == devByElem->getTagName());
        }
        catch (const xercesc::OutOfMemoryException&)
        {
            std::cerr << "OutOfMemoryException" << std::endl;
            errorCode = 5;
        }
        catch (const xercesc::DOMException& e)
        {
            std::cerr << "DOMException code is:  " << e.code << std::endl;
            errorCode = 2;
        }
        catch (...)
        {
            std::cerr << "An error occurred creating the document" << std::endl;
            errorCode = 3;
        }
    }
    else
    {
        std::cerr << "Requested DOM implementation is not supported" << std::endl;
        errorCode = 4;
    }

    xercesc::XMLPlatformUtils::Terminate();
    return errorCode;
}
