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

#include "Normalizer.hpp"
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/XMLBuffer.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>

#if defined(XERCES_NEW_IOSTREAMS)
#include <iostream>
#else
#include <iostream.h>
#endif

#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include "xercesc/dom/impl/DOMConfigurationImpl.hpp"
#include "xercesc/dom/impl/DOMDocumentImpl.hpp"
#include "xercesc/dom/impl/DOMEntityImpl.hpp"
#include "xercesc/dom/impl/DOMEntityReferenceImpl.hpp"

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

#define StrX(str) StrX(str).localForm()



Normalizer::Normalizer() {
    try
    {
        XMLPlatformUtils::Initialize();
    }

    catch(const XMLException &toCatch)
    {
        XERCES_STD_QUALIFIER cerr << "Error during Xerces-c Initialization.\n"
             << "  Exception message:"
             << StrX(toCatch.getMessage()) << XERCES_STD_QUALIFIER endl;
    }
    parser = 0;

}

Normalizer::~Normalizer() {
    XMLPlatformUtils::Terminate();
}

void Normalizer::printEntityRefNodes(DOMElement *ele) {

    DOMNode *child = ele->getFirstChild();

    while(child != 0) {
        if(child->getNodeType() == DOMNode::ENTITY_REFERENCE_NODE) {
            XERCES_STD_QUALIFIER cout << "start of entity ref node" << XERCES_STD_QUALIFIER endl;
            DOMNode *entChild = ((DOMEntityReference*)child)->getFirstChild();
            while(entChild != 0) {
                serializeNode(entChild);
                entChild = entChild->getNextSibling();
            }
            XERCES_STD_QUALIFIER cout << "\nend of entity ref node\n\n" << XERCES_STD_QUALIFIER endl;

        }

        if(child->getNodeType() == DOMNode::ELEMENT_NODE) {
            printEntityRefNodes((DOMElement*)child);
        }

        child = child->getNextSibling();
    }

}


bool Normalizer::handleError(const DOMError& domError)
{
    // Display whatever error message passed from the serializer
    if (domError.getSeverity() == DOMError::DOM_SEVERITY_WARNING)
        XERCES_STD_QUALIFIER cerr << "\nWarning Message: ";
    else if (domError.getSeverity() == DOMError::DOM_SEVERITY_ERROR)
        XERCES_STD_QUALIFIER cerr << "\nError Message: ";
    else
        XERCES_STD_QUALIFIER cerr << "\nFatal Message: ";

    char *msg = XMLString::transcode(domError.getMessage());
    XERCES_STD_QUALIFIER cerr<< msg <<XERCES_STD_QUALIFIER endl;
    XMLString::release(&msg);

    XERCES_STD_QUALIFIER cerr << "Related data ";

    msg = XMLString::transcode(((DOMNode*)domError.getRelatedData())->getNodeName());
    XERCES_STD_QUALIFIER cerr << msg <<XERCES_STD_QUALIFIER endl;
    XMLString::release(&msg);



    // continue serialization if possible.
    return true;
}



DOMDocument* Normalizer::createDocument() {
    XMLCh coreStr[100];
    XMLString::transcode("Core",coreStr,99);

    DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(coreStr);
    return impl->createDocument();
}

void Normalizer::serializeNode(const DOMNode * const node) {
    XMLCh tempStr[100];
    XMLString::transcode("LS", tempStr, 99);
    DOMImplementation *impl          = DOMImplementationRegistry::getDOMImplementation(tempStr);
    DOMLSSerializer   *theSerializer = ((DOMImplementationLS*)impl)->createLSSerializer();
    DOMLSOutput       *theOutput     = ((DOMImplementationLS*)impl)->createLSOutput();
    theSerializer->getDomConfig()->setParameter(X("format-pretty-print"), true);
    XMLFormatTarget *myFormTarget;
    myFormTarget = new StdOutFormatTarget();

    theOutput->setByteStream(myFormTarget);
    theSerializer->write(node,theOutput);
    delete myFormTarget;
    theSerializer->release();
    theOutput->release();
}

int main(int /*argc*/, char ** /*argv*/) {

    Normalizer *normalizer = new Normalizer();

    DOMDocument *doc = normalizer->createDocument();
    bool *tmpTrue = new bool(true);
    bool *tmpFalse = new bool(false);

    DOMElement* docFirstElement = doc->createElementNS(X("http://www.test.com"),X("docEle"));
    doc->appendChild(docFirstElement);
    DOMElement* docFirstElementChild = doc->createElementNS(X("http://www.test2.com"),X("docEleChild"));
    docFirstElement->appendChild(docFirstElementChild);

    //create default ns
    doc->normalizeDocument();
    normalizer->serializeNode(doc);
    XERCES_STD_QUALIFIER cout << "\n\n";

    //add in binding
    docFirstElement->setPrefix(X("po"));
    doc->normalizeDocument();
    normalizer->serializeNode(doc);
    XERCES_STD_QUALIFIER cout << "\n\n";

    //use default
    DOMElement* docFirstElementChildChild = doc->createElementNS(X("http://www.test2.com"),X("docEleChildChild"));
    docFirstElementChild->appendChild(docFirstElementChildChild);
    doc->normalizeDocument();
    normalizer->serializeNode(doc);
    XERCES_STD_QUALIFIER cout << "\n\n";

    // this block is needed to destroy the XMLBuffer
    {
        //use a binding
        XMLBuffer buf;
        buf.set(XMLUni::fgXMLNSString);
        buf.append(chColon);
        buf.append(X("po2"));
        docFirstElementChild->removeAttributeNS(XMLUni::fgXMLNSURIName, XMLUni::fgXMLNSString);
        docFirstElement->removeAttributeNS(XMLUni::fgXMLNSURIName, XMLUni::fgXMLNSString);
        docFirstElement->setAttributeNS(XMLUni::fgXMLNSURIName, buf.getRawBuffer(), X("http://www.test2.com"));
        docFirstElementChild->setPrefix(X("po2"));
        doc->normalizeDocument();
        normalizer->serializeNode(doc);
        XERCES_STD_QUALIFIER cout << "\n\n";
    }

    //some siblngs to ensure the scope stacks are working
    docFirstElementChildChild = doc->createElementNS(X("http://www.test3.com"),X("docEleChildChild2"));
    docFirstElementChild->appendChild(docFirstElementChildChild);
    docFirstElementChildChild = doc->createElementNS(X("http://www.test4.com"),X("po4:docEleChildChild3"));
    docFirstElementChild->appendChild(docFirstElementChildChild);
    docFirstElementChildChild = doc->createElementNS(X("http://www.test4.com"),X("po4:docEleChildChild4"));
    docFirstElementChild->appendChild(docFirstElementChildChild);
    doc->normalizeDocument();
    normalizer->serializeNode(doc);
    XERCES_STD_QUALIFIER cout << "\n\n";

    //conflicting prefix
    docFirstElementChildChild->setAttributeNS(XMLUni::fgXMLNSURIName, X("po4"), X("conflict"));
    doc->normalizeDocument();
    normalizer->serializeNode(doc);
    XERCES_STD_QUALIFIER cout << "\n\n";

    //conflicting default
    docFirstElementChildChild = doc->createElementNS(X("http://www.test4.com"),X("docEleChildChild5"));
    docFirstElementChild->appendChild(docFirstElementChildChild);
    docFirstElementChildChild->setAttributeNS(XMLUni::fgXMLNSURIName, XMLUni::fgXMLNSString, X("conflict"));
    doc->normalizeDocument();
    normalizer->serializeNode(doc);
    XERCES_STD_QUALIFIER cout << "\n\n";

    //set the xmlns to ""
    DOMElement *noNamespaceEle = doc->createElementNS(X(""),X("noNamespace"));
    docFirstElementChildChild->appendChild(noNamespaceEle);
    doc->normalizeDocument();
    normalizer->serializeNode(doc);
    XERCES_STD_QUALIFIER cout << "\n\n";


    //now lets do a bit off attribute testing on the doc ele
    docFirstElement->setAttributeNS(X("http://testattr.com"), X("attr1"), X("value"));
    docFirstElement->setAttributeNS(X("http://testattr.com"), X("attr2"), X("value"));
    docFirstElement->setAttributeNS(X("http://testattr2.com"), X("attr3"), X("value"));
    docFirstElement->setAttributeNS(X("http://www.test.com"), X("attr4"), X("value"));
    docFirstElement->setAttributeNS(X("http://testattr2.com"), X("po:attr5"), X("value"));
    docFirstElement->setAttributeNS(X("http://testattr2.com"), X("poFake:attr6"), X("value"));
    docFirstElement->setAttributeNS(X("http://testattr3.com"), X("po3:attr7"), X("value"));
    doc->normalizeDocument();
    normalizer->serializeNode(doc);
    XERCES_STD_QUALIFIER cout << "\n\n";

    //and now on one of its children
    docFirstElementChildChild->setAttributeNS(X("http://testattr.com"), X("attr1"), X("value"));
    docFirstElementChildChild->setAttributeNS(X("http://testattr.com"), X("attr2"), X("value"));
    docFirstElementChildChild->setAttributeNS(X("http://testattr2.com"), X("attr3"), X("value"));
    docFirstElementChildChild->setAttributeNS(X("http://www.test.com"), X("attr4"), X("value"));
    docFirstElementChildChild->setAttributeNS(X("http://testattr2.com"), X("po:attr5"), X("value"));
    docFirstElementChildChild->setAttributeNS(X("http://testattr2.com"), X("poFake:attr6"), X("value"));
    docFirstElementChildChild->setAttributeNS(X("http://testattr3.com"), X("po3:attr7"), X("value"));
    docFirstElementChildChild->setAttributeNS(X("http://testattr4.com"), X("po4:attr8"), X("value"));


    //test for a clash with our NSx attrs
    docFirstElementChildChild->setAttributeNS(X("http://testclash.com"), X("NS1:attr9"), X("value"));
    docFirstElementChildChild->setAttributeNS(XMLUni::fgXMLNSURIName, X("xmlns:NS1"), X("http://testclash.com"));

    //clash with standard prefix
    docFirstElementChildChild->setAttributeNS(X("http://testattr5.com"), X("po:attr10"), X("value"));

    doc->normalizeDocument();
    normalizer->serializeNode(doc);
    XERCES_STD_QUALIFIER cout << "\n\n";


    //2 prefix with the same uri
    docFirstElementChildChild = doc->createElementNS(X("http://www.uri1.com"),X("docEleChildChild6"));
    docFirstElementChild->appendChild(docFirstElementChildChild);
    docFirstElementChildChild->setAttributeNS(XMLUni::fgXMLNSURIName, X("xmlns:uri1"), X("http://www.uri1.com"));
    docFirstElementChildChild->setAttributeNS(XMLUni::fgXMLNSURIName, X("xmlns:uri1b"), X("http://www.uri1.com"));
    docFirstElementChildChild->setAttributeNS(X("http://www.uri1.com"), X("uri1:attr1"), X("value"));
    docFirstElementChildChild->setAttributeNS(X("http://www.uri1.com"), X("uri1b:attr2"), X("value"));
    doc->normalizeDocument();
    normalizer->serializeNode(doc);
    XERCES_STD_QUALIFIER cout << "\n\n";

    //check to see we use the nearest binding and for more inheritence
    DOMElement *docFirstElementChildChildChild = doc->createElementNS(X("http://www.uri1.com"),X("docEleChildChildChild"));
    docFirstElementChildChild->appendChild(docFirstElementChildChildChild);
    docFirstElementChildChild->setAttributeNS(XMLUni::fgXMLNSURIName, X("xmlns:nearerThanPo"), X("http://www.test.com"));
    docFirstElementChildChildChild->setAttributeNS(X("http://testattr.com"), X("attr2"), X("value"));
    docFirstElementChildChildChild->setAttributeNS(X("http://www.test.com"), X("attr1"), X("value"));
    doc->normalizeDocument();
    normalizer->serializeNode(doc);
    XERCES_STD_QUALIFIER cout << "\n\n";


    //NS1.1 stuff

    //test creating default prefix when NS1 has been set to ""
    noNamespaceEle->setAttributeNS(XMLUni::fgXMLNSURIName, X("xmlns:NS1"), X(""));
    DOMElement *noNamespaceChild = doc->createElementNS(X("http://testclash.com"),X("testing1.1Stuff"));
    noNamespaceEle->appendChild(noNamespaceChild);
    doc->normalizeDocument();
    normalizer->serializeNode(doc);

    noNamespaceChild = doc->createElementNS(X("http://testclash.com"),X("NS1:testing1.1Stuff"));
    noNamespaceEle->appendChild(noNamespaceChild);

    noNamespaceChild->setAttributeNS(X("http://www.someRandomUri.com"), X("attr"), X("value"));
    doc->normalizeDocument();
    normalizer->serializeNode(doc);


    //check error conditions
    XERCES_STD_QUALIFIER cout << "error conditions" << XERCES_STD_QUALIFIER endl;

    DOMConfiguration *conf = doc->getDOMConfig();
    conf->setParameter(XMLUni::fgDOMErrorHandler, normalizer);
    conf->setParameter(XMLUni::fgDOMNamespaces, true);

    DOMElement *level1Node = doc->createElement(X("level1Node"));
    docFirstElement->appendChild(level1Node);
    doc->normalizeDocument();

    docFirstElement->removeChild(level1Node);
    docFirstElement->setAttribute(X("level1Attr"), X("level1"));
    doc->normalizeDocument();
    docFirstElement->removeAttribute(X("level1Attr"));

    //cant check this as Xerces does not let us do it
    //    noNamespaceChild->setAttributeNS(X("http://www.someRandomUri.com"), X("xmlns"), X("value"));
    //    doc->normalizeDocument();



    //lets do a sanity test on a comment
    DOMComment *comment = doc->createComment(X("some comment"));
    docFirstElement->appendChild(comment);
    doc->normalizeDocument();
    normalizer->serializeNode(doc);

    conf->setParameter(XMLUni::fgDOMComments, false);
    docFirstElement->appendChild(comment);
    doc->normalizeDocument();
    normalizer->serializeNode(doc);


    //and on a CDATA
    DOMCDATASection *cData = doc->createCDATASection(X("some cdata"));
    docFirstElement->appendChild(cData);
    doc->normalizeDocument();
    normalizer->serializeNode(doc);

    conf->setParameter(XMLUni::fgDOMCDATASections, false);
    docFirstElement->appendChild(cData);
    doc->normalizeDocument();
    normalizer->serializeNode(doc);

    delete normalizer;
    delete tmpTrue;
    delete tmpFalse;

    return 0;
}
