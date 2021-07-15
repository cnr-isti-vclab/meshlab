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

//
//  Various DOM tests.
//     This is NOT a complete test of DOM functionality.
//

/*
 * $Id$
 */

#include <stdio.h>
#include <string.h>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUniDefs.hpp>


XERCES_CPP_NAMESPACE_USE

bool errorOccurred = false;

#define UNUSED(x) { if(x!=0){} }

#define TASSERT(c) tassert((c), __FILE__, __LINE__)

void tassert(bool c, const char *file, int line)
{
    if (!c) {
        printf("Failure.  Line %d,   file %s\n", line, file);
        errorOccurred = true;
    }
}


#define EXCEPTION_TEST(operation, expected_exception)               \
{                                                                   \
    try {                                                           \
    operation;                                                      \
    printf(" Error: no exception thrown at line %d\n", __LINE__);   \
    errorOccurred = true;                                           \
    }                                                               \
    catch (DOMException &e) {                                       \
    if (e.code != expected_exception) {                             \
        printf(" Wrong exception code: %d at line %d\n", e.code, __LINE__); \
        errorOccurred = true;                                       \
    }                                                               \
    }                                                               \
    catch (...)   {                                                 \
        printf(" Wrong exception thrown at line %d\n", __LINE__);   \
        errorOccurred = true;                                       \
    }                                                               \
}

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

//---------------------------------------------------------------------------------------
//
//   DOMBasicTests    Basic DOM Level 1 tests
//
//---------------------------------------------------------------------------------------
void DOMBasicTests()
{
    //
    //  Test Doc01      Create a new empty document
    //
    {
        //  First precondition, so that lazily created strings do not appear
        //  as memory leaks.
        DOMDocument*   doc;
        doc = DOMImplementationRegistry::getDOMImplementation(X("Core"))->createDocument();
        doc->release();
    }

    //
    //  Test Doc02      Create one of each kind of node using the
    //                  document createXXX methods.
    //                  Watch for memory leaks.
    //
    {
        //  Do all operations in a preconditioning step, to force the
        //  creation of implementation objects that are set up on first use.
        //  Don't watch for leaks in this block (no  / )
        DOMDocument* doc = DOMImplementationRegistry::getDOMImplementation(X("Core"))->createDocument();

        DOMElement*   el = doc->createElement(X("Doc02Element"));
        UNUSED(el); // silence warning

        DOMDocumentFragment* frag = doc->createDocumentFragment ();
        UNUSED(frag); // silence warning

        DOMText*  text = doc->createTextNode(X("Doc02TextNode"));
        UNUSED(text); // silence warning

        DOMComment* comment = doc->createComment(X("Doc02Comment"));
        UNUSED(comment); // silence warning

        DOMCDATASection*  cdataSec = doc->createCDATASection(X("Doc02CDataSection"));
        UNUSED(cdataSec); // silence warning

        DOMDocumentType*  docType = doc->createDocumentType(X("Doc02DocumentType"));
        UNUSED(docType); // silence warning

        DOMNotation* notation = doc->createNotation(X("Doc02Notation"));
        UNUSED(notation); // silence warning

        DOMProcessingInstruction* pi = doc->createProcessingInstruction(X("Doc02PITarget"), X("Doc02PIData"));
        UNUSED(pi); // silence warning

        DOMNodeList*    nodeList = doc->getElementsByTagName(X("*"));
        UNUSED(nodeList); // silence warning

        doc->release();
    }



    //
    //  Doc03 - Create a small document tree
    //

    {
        DOMDocument*   doc = DOMImplementationRegistry::getDOMImplementation(X("Core"))->createDocument();

        DOMElement*   rootEl = doc->createElement(X("Doc03RootElement"));

        doc->appendChild(rootEl);

        DOMText*       textNode = doc->createTextNode(X("Doc03 text stuff"));

        rootEl->appendChild(textNode);

        DOMNodeList*    nodeList = doc->getElementsByTagName(X("*"));
        UNUSED(nodeList); // silence warning
        doc->release();
    };



    //
    //  Attr01
    //
    {
        DOMDocument*   doc = DOMImplementationRegistry::getDOMImplementation(X("Core"))->createDocument();

        DOMElement*   rootEl  = doc->createElement(X("RootElement"));

        doc->appendChild(rootEl);
        {
            DOMAttr*        attr01  = doc->createAttribute(X("Attr01"));
            DOMNode* rem = rootEl->setAttributeNode(attr01);
            if (rem)
                rem->release();

        }


        {
            DOMAttr* attr02 = doc->createAttribute(X("Attr01"));
            DOMNode* rem = rootEl->setAttributeNode(attr02);
            if (rem)
                rem->release();
        }
        doc->release();

    };

    //
    //  Attr02
    //

    {
        DOMDocument*   doc = DOMImplementationRegistry::getDOMImplementation(X("Core"))->createDocument();

        DOMElement*   rootEl  = doc->createElement(X("RootElement"));

        doc->appendChild(rootEl);

        DOMAttr*        attr01  = doc->createAttribute(X("Attr02"));

        DOMNode* rem = rootEl->setAttributeNode(attr01);
        if (rem)
            rem->release();


        DOMAttr*        attr02 = doc->createAttribute(X("Attr02"));

        rem = rootEl->setAttributeNode(attr02);
        if (rem)
            rem->release();
        doc->release();
    }



    //
    //  Attr03
    //

    {
        DOMDocument*   doc = DOMImplementationRegistry::getDOMImplementation(X("Core"))->createDocument();

        DOMElement*   rootEl  = doc->createElement(X("RootElement"));

        doc->appendChild(rootEl);

        DOMAttr*        attr01  = doc->createAttribute(X("Attr03"));

        DOMNode* rem = rootEl->setAttributeNode(attr01);
        if (rem)
            rem->release();


        attr01->setValue(X("Attr03Value1"));

        attr01->setValue(X("Attr03Value2"));
        doc->release();
    }



    //
    //  Attr04
    //

    {
        DOMDocument*   doc = DOMImplementationRegistry::getDOMImplementation(X("Core"))->createDocument();

        DOMElement*   rootEl  = doc->createElement(X("RootElement"));

        doc->appendChild(rootEl);

        DOMAttr*        attr01  = doc->createAttribute(X("Attr04"));

        DOMNode* rem = rootEl->setAttributeNode(attr01);
        if (rem)
            rem->release();

        attr01->setValue(X("Attr04Value1"));

        DOMNode* value = attr01->getFirstChild();
        UNUSED(value); // silence warning
        doc->release();
    }




    //
    //  Text01
    //

    {
        DOMDocument*   doc = DOMImplementationRegistry::getDOMImplementation(X("Core"))->createDocument();

        DOMElement*   rootEl  = doc->createElement(X("RootElement"));

        doc->appendChild(rootEl);

        DOMText*        txt1 = doc->createTextNode(X("Hello Goodbye"));
        rootEl->appendChild(txt1);

        txt1->splitText(6);
        rootEl->normalize();
        doc->release();

    }



    //
    //  Notation01
    //

    {
        DOMDocument*       doc = DOMImplementationRegistry::getDOMImplementation(X("Core"))->createDocument();

        DOMDocumentType*   dt  = doc->createDocumentType(X("DocType_for_Notation01"));

        doc->appendChild(dt);


        DOMNamedNodeMap* notationMap = dt->getNotations();

        DOMNotation*    nt1 = doc->createNotation(X("Notation01"));

        DOMNode* rem = notationMap->setNamedItem (nt1);
        if (rem)
            rem->release();

        DOMNode*  abc1 = notationMap->getNamedItem(X("Notation01"));

        DOMNotation*    nt2 = (DOMNotation*) abc1;
        TASSERT(nt1==nt2);
        nt2 = 0;
        nt1 = 0;

        DOMNode* abc6 = notationMap->getNamedItem(X("Notation01"));

        nt2 = (DOMNotation*) abc6;
        doc->release();
    }



    //
    //  NamedNodeMap01 - comparison operators.
    //

    {
        DOMNamedNodeMap*    nnm = 0;
        TASSERT(nnm == 0);

        DOMDocument*       doc = DOMImplementationRegistry::getDOMImplementation(X("Core"))->createDocument();
        nnm = doc->getAttributes();    // Should be null, because node type
                                      //   is not Element.
        TASSERT(nnm == 0);
        TASSERT(!(nnm != 0));

        DOMElement* el = doc->createElement(X("NamedNodeMap01"));

        DOMNamedNodeMap* nnm2 = el->getAttributes();    // Should be an empty, but non-null map.
        TASSERT(nnm2 != 0);
        TASSERT(nnm != nnm2);
        nnm = nnm2;
        TASSERT(nnm == nnm2);
        doc->release();
    }



    //
    //  importNode quick test
    //

    {
        DOMDocument*   doc1 = DOMImplementationRegistry::getDOMImplementation(X("Core"))->createDocument();
        DOMDocument*   doc2 = DOMImplementationRegistry::getDOMImplementation(X("Core"))->createDocument();

        DOMElement*   el1  = doc1->createElement(X("abc"));

        doc1->appendChild(el1);
        TASSERT(el1->getParentNode() != 0);
        DOMNode*        el2  = doc2->importNode(el1, true);
        TASSERT(el2->getParentNode() == 0);
        const XMLCh*       tagName = el2->getNodeName();

        TASSERT(!XMLString::compareString(tagName, X("abc")));

        TASSERT(el2->getOwnerDocument() == doc2);
        TASSERT(doc1 != doc2);
        doc1->release();
        doc2->release();
    }


    //
    //  getLength() tests.  Both Node CharacterData and NodeList implement
    //                  getLength().  Early versions of the DOM had a clash
    //                  between the two, originating in the implementation class
    //                  hirearchy, which has NodeList as a (distant) base class
    //                  of CharacterData.  This is a regression test to verify
    //                  that the problem stays fixed.
    //

    {
        DOMDocument*    doc = DOMImplementationRegistry::getDOMImplementation(X("Core"))->createDocument();

        DOMText*          tx = doc->createTextNode(X("Hello"));

        DOMElement*     el = doc->createElement(X("abc"));

        el->appendChild(tx);

        XMLSize_t textLength = tx->getLength();
        TASSERT(textLength == 5);

        DOMNodeList*      nl = tx->getChildNodes();
        XMLSize_t nodeListLen = nl->getLength();
        TASSERT(nodeListLen == 0);

        nl = el->getChildNodes();
        nodeListLen = nl->getLength();
        TASSERT(nodeListLen == 1);
        doc->release();
    }


    //
    //  NodeList - comparison operators, basic operation.
    //

    {
        DOMNodeList*    nl = 0;
        DOMNodeList*    nl2 = 0;
        TASSERT(nl == 0);
        TASSERT(!(nl != 0));
        TASSERT(nl == nl2);

        DOMDocument*       doc = DOMImplementationRegistry::getDOMImplementation(X("Core"))->createDocument();
        nl = doc->getChildNodes();    // Should be non-null, but empty

        TASSERT(nl != 0);
        XMLSize_t len = nl->getLength();
        TASSERT(len == 0);

        DOMElement* el = doc->createElement(X("NodeList01"));

        doc->appendChild(el);
        len = nl->getLength();
        TASSERT(len == 1);
        TASSERT(nl != nl2);
        nl2 = nl;
        TASSERT(nl == nl2);
        doc->release();
    }




    //
    //  Name validity checking.
    //

    {
         DOMDocument*       doc = DOMImplementationRegistry::getDOMImplementation(X("Core"))->createDocument();
         try
         {
             DOMElement* el = doc->createElement(X("!@@ bad element name"));
             UNUSED(el); // silence warning
             TASSERT(false);  // Exception above should prevent us reaching here.
         }
         catch ( DOMException e)
         {
             TASSERT(e.code == DOMException::INVALID_CHARACTER_ERR);
         }
         catch (...)
         {
             TASSERT(false);  // Wrong exception thrown.
         }
         doc->release();
    }



    //
    //  Assignment ops return value
    //

    {
        DOMDocument*       doc = DOMImplementationRegistry::getDOMImplementation(X("Core"))->createDocument();

        DOMElement* el = doc->createElement(X("NodeList01"));

        doc->appendChild(el);

        DOMElement* n1, *n2, *n3;

        n1 = n2 = n3 = el;
        TASSERT(n1 == n2);
        TASSERT(n1 == n3);
        TASSERT(n1 == el);
        TASSERT(n1 != 0);
        n1 = n2 = n3 = 0;
        TASSERT(n1 == 0);
        doc->release();
    }



    //
    //  Cloning of a node with attributes. Regression test for a ref counting
    //  bug in attributes of cloned nodes that occured when the "owned" flag
    //  was not set in the clone.
    //

    {
        DOMDocument*   doc = DOMImplementationRegistry::getDOMImplementation(X("Core"))->createDocument();

        DOMElement*   root = doc->createElement(X("CTestRoot"));

        root->setAttribute(X("CTestAttr"), X("CTestAttrValue"));

        const XMLCh* s = root->getAttribute(X("CTestAttr"));
        TASSERT(!XMLString::compareString(s, X("CTestAttrValue")));

        DOMNode* abc2 = root->cloneNode(true);
        DOMElement*   cloned = (DOMElement*) abc2;
        DOMAttr* a = cloned->getAttributeNode(X("CTestAttr"));
        TASSERT(a != 0);
        s = a->getValue();
        TASSERT(!XMLString::compareString(s, X("CTestAttrValue")));
        a = 0;

        a = cloned->getAttributeNode(X("CTestAttr"));
        TASSERT(a != 0);
        s = a->getValue();
        TASSERT(!XMLString::compareString(s, X("CTestAttrValue")));
        doc->release();

    }



    //
    //  splitText()
    //     Regression test for a bug from Tinny Ng
    //

    {
        DOMDocument* doc;
        doc = DOMImplementationRegistry::getDOMImplementation(X("Core"))->createDocument();
        DOMText* tn, *tn1, *tn2;

        tn = doc->createTextNode (X("0123456789"));

        tn1 = tn->splitText(5);

        TASSERT(!XMLString::compareString(tn->getNodeValue(), X("01234")));

        TASSERT(!XMLString::compareString(tn1->getNodeValue(), X("56789")));


        tn2 = tn->splitText(5);
        TASSERT(!XMLString::compareString(tn->getNodeValue(), X("01234")));

        TASSERT(!XMLString::compareString(tn2->getNodeValue(), XMLUni::fgZeroLenString));

        EXCEPTION_TEST(tn->splitText(6), DOMException::INDEX_SIZE_ERR);
        doc->release();
    }



}


//---------------------------------------------------------------------------------------
//
//   DOMNSTests    DOM Name Space tests
//
//---------------------------------------------------------------------------------------
void DOMNSTests()
{

    //
    //  DOM Level 2 tests.  These should be split out as a separate test.
    //


    //
    // hasFeature.  The set of supported options tested here is for Xerces 1.1
    //              Note: because the implementation lazily creates some of the comprison
    //                    strings within the implementation, this test must be pre-flighted
    //                    outside of the TESPROLOG/ macros to avoid spurious
    //                    reports of memory leaks.
    //
    // Also test the case-insensitive
    //
    {

        DOMImplementation*  impl = DOMImplementationRegistry::getDOMImplementation(X("Core"));

        TASSERT(impl->hasFeature(X("XmL"), X("2.0")) == true);

        TASSERT(impl->hasFeature(X("xML"), 0) == true);
        TASSERT(impl->hasFeature(X("xML"), XMLUni::fgZeroLenString) == true);

        TASSERT(impl->hasFeature(X("XMl"), X("1.0"))  == true);

        TASSERT(impl->hasFeature(X("xMl"), X("3.0"))  == false);

        TASSERT(impl->hasFeature(X("TrAveRsal"), 0)  == true);
        TASSERT(impl->hasFeature(X("TrAveRsal"), XMLUni::fgZeroLenString)  == true);
    }



    {
        DOMImplementation*  impl = DOMImplementationRegistry::getDOMImplementation(X("Core"));

        TASSERT(impl->hasFeature(X("XmL"), 0)   == true);
        TASSERT(impl->hasFeature(X("XmL"), X("1.0"))    == true);
        TASSERT(impl->hasFeature(X("XmL"), X("2.0"))   == true);
        TASSERT(impl->hasFeature(X("XmL"), X("3.0"))   == false);

        TASSERT(impl->hasFeature(X("Core"), 0)   == true);

        TASSERT(impl->hasFeature(X("coRe"), X("1.0"))    == true);

        TASSERT(impl->hasFeature(X("core"), X("2.0"))   == true);

        TASSERT(impl->hasFeature(X("cORe"), X("3.0"))   == true);
        TASSERT(impl->hasFeature(X("cORe"), X("4.0"))   == false);

        TASSERT(impl->hasFeature(X("Traversal"), XMLUni::fgZeroLenString)   == true);

        TASSERT(impl->hasFeature(X("traversal"), X("1.0"))    == false);

        TASSERT(impl->hasFeature(X("TraVersal"), X("2.0"))   == true);

        TASSERT(impl->hasFeature(X("Range"), 0)   == true);

        TASSERT(impl->hasFeature(X("raNge"), X("1.0"))    == false);

        TASSERT(impl->hasFeature(X("RaNge"), X("2.0"))   == true);


        TASSERT(impl->hasFeature(X("HTML"), 0)   == false);

        TASSERT(impl->hasFeature(X("Views"), XMLUni::fgZeroLenString)   == false);

        TASSERT(impl->hasFeature(X("StyleSheets"), 0)   == false);

        TASSERT(impl->hasFeature(X("CSS"), XMLUni::fgZeroLenString)   == false);

        TASSERT(impl->hasFeature(X("CSS2"), 0)   == false);

        TASSERT(impl->hasFeature(X("Events"), 0)   == false);

        TASSERT(impl->hasFeature(X("UIEvents"), 0)   == false);

        TASSERT(impl->hasFeature(X("MouseEvents"), 0)   == false);

        TASSERT(impl->hasFeature(X("MutationEvents"), 0)   == false);

        TASSERT(impl->hasFeature(X("HTMLEvents"), 0)   == false);
    }



    //
    // isSupported test (similar to hasFeature)
    //

    {
        DOMDocument* doc;
        doc = DOMImplementationRegistry::getDOMImplementation(X("Core"))->createDocument();


        TASSERT(doc->isSupported(X("XmL"), 0)   == true);
        TASSERT(doc->isSupported(X("XmL"), X("1.0"))    == true);
        TASSERT(doc->isSupported(X("XmL"), X("2.0"))   == true);
        TASSERT(doc->isSupported(X("XmL"), X("3.0"))   == false);

        TASSERT(doc->isSupported(X("Core"), 0)   == true);
        TASSERT(doc->isSupported(X("Core"), XMLUni::fgZeroLenString)   == true);

        TASSERT(doc->isSupported(X("coRe"), X("1.0"))    == true);

        TASSERT(doc->isSupported(X("core"), X("2.0"))   == true);

        TASSERT(doc->isSupported(X("cORe"), X("3.0"))   == true);
        TASSERT(doc->isSupported(X("cORe"), X("4.0"))   == false);

        TASSERT(doc->isSupported(X("Traversal"), 0)   == true);

        TASSERT(doc->isSupported(X("traversal"), X("1.0"))    == false);

        TASSERT(doc->isSupported(X("TraVersal"), X("2.0"))   == true);

        TASSERT(doc->isSupported(X("Range"), XMLUni::fgZeroLenString)   == true);

        TASSERT(doc->isSupported(X("raNge"), X("1.0"))    == false);

        TASSERT(doc->isSupported(X("RaNge"), X("2.0"))   == true);

        TASSERT(doc->isSupported(X("HTML"), 0)   == false);

        TASSERT(doc->isSupported(X("Views"), 0)   == false);

        TASSERT(doc->isSupported(X("StyleSheets"), 0)   == false);

        TASSERT(doc->isSupported(X("CSS"), 0)   == false);

        TASSERT(doc->isSupported(X("CSS2"), XMLUni::fgZeroLenString)   == false);
        TASSERT(doc->isSupported(X("Events"), 0)   == false);

        TASSERT(doc->isSupported(X("UIEvents"), 0)   == false);

        TASSERT(doc->isSupported(X("MouseEvents"), 0)   == false);

        TASSERT(doc->isSupported(X("MutationEvents"), XMLUni::fgZeroLenString)   == false);

        TASSERT(doc->isSupported(X("HTMLEvents"), 0)   == false);
        doc->release();
    }


    //
    // CreateDocumentType
    //

    {
        DOMImplementation*  impl = DOMImplementationRegistry::getDOMImplementation(X("Core"));


        DOMDocumentType* dt = impl->createDocumentType(X("foo:docName"), X("pubId"), X("http://sysId"));

        TASSERT(dt != 0);
        TASSERT(dt->getNodeType() == DOMNode::DOCUMENT_TYPE_NODE);
        TASSERT(!XMLString::compareString(dt->getNodeName(), X("foo:docName")));
        TASSERT(dt->getNamespaceURI() == 0);
        TASSERT(dt->getPrefix() == 0);
        TASSERT(dt->getLocalName() == 0);
        TASSERT(!XMLString::compareString(dt->getPublicId(), X("pubId")));
        TASSERT(!XMLString::compareString(dt->getSystemId(), X("http://sysId")));
        TASSERT(dt->getInternalSubset() == 0);
        TASSERT(dt->getOwnerDocument() == 0);

        DOMNamedNodeMap* nnm = dt->getEntities();
        TASSERT(nnm->getLength() == 0);
        nnm = dt->getNotations();
        TASSERT(nnm->getLength() == 0);

        // release the documentType (dt) which is an orphaned node (does not have the owner)
        dt->release();

        //
        // Qualified name without prefix should also work.
        //
        dt = impl->createDocumentType(X("docName"), X("pubId"), X("http://sysId"));

        TASSERT(dt != 0);
        TASSERT(dt->getNodeType() == DOMNode::DOCUMENT_TYPE_NODE);
        TASSERT(!XMLString::compareString(dt->getNodeName(), X("docName")));
        TASSERT(dt->getNamespaceURI() == 0);
        TASSERT(dt->getPrefix() == 0);
        TASSERT(dt->getLocalName() == 0);
        TASSERT(!XMLString::compareString(dt->getPublicId(), X("pubId")));
        TASSERT(!XMLString::compareString(dt->getSystemId(), X("http://sysId")));
        TASSERT(dt->getInternalSubset() == 0);
        TASSERT(dt->getOwnerDocument() == 0);

        // Creating a DocumentType with invalid or malformed qName should fail.
        EXCEPTION_TEST(impl->createDocumentType(X("<docName"), X("pubId"), X("http://sysId")), DOMException::INVALID_CHARACTER_ERR);

        EXCEPTION_TEST(impl->createDocumentType(X(":docName"), X("pubId"), X("http://sysId")), DOMException::NAMESPACE_ERR);

        EXCEPTION_TEST(impl->createDocumentType(X("docName:"), X("pubId"), X("http://sysId")), DOMException::NAMESPACE_ERR);

        EXCEPTION_TEST(impl->createDocumentType(X("doc::Name"), X("pubId"), X("http://sysId")), DOMException::NAMESPACE_ERR);

        EXCEPTION_TEST(impl->createDocumentType(X("doc:N:ame"), X("pubId"), X("http://sysId")), DOMException::NAMESPACE_ERR);

        // release the documentType (dt) which is an orphaned node (does not have the owner)
        dt->release();

    }



    //
    //  DOMImplementation::CreateDocument
    {

        DOMImplementation*   impl = DOMImplementationRegistry::getDOMImplementation(X("Core"));
        DOMDocumentType*       dt = 0;

        DOMDocument*           doc = impl->createDocument(XMLUni::fgZeroLenString, X("a"), dt);

        doc->getNodeName();
        doc->release();
    }

    //

    {

        DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(X("Core"));



        DOMDocumentType* dt = impl->createDocumentType(X("foo:docName"), X("pubId"), X("http://sysId"));

        DOMDocument* doc = impl->createDocument(X("http://document.namespace"), X("foo:docName"), dt);

        TASSERT(dt->getOwnerDocument() == doc);
        TASSERT(doc->getOwnerDocument() == 0);

        TASSERT(doc->getNodeType() == DOMNode::DOCUMENT_NODE);
        TASSERT(doc->getDoctype() == dt);

        TASSERT(!XMLString::compareString(doc->getNodeName(), X("#document")));

        TASSERT(doc->getNodeValue() == 0);
        TASSERT(doc->getNamespaceURI() == 0);
        TASSERT(doc->getPrefix() == 0);
        TASSERT(doc->getLocalName() == 0);

        DOMElement* el = doc->getDocumentElement();

        TASSERT(!XMLString::compareString(el->getLocalName(), X("docName")));

        TASSERT(!XMLString::compareString(el->getNamespaceURI(), X("http://document.namespace")));
        TASSERT(!XMLString::compareString(el->getNodeName(), X("foo:docName")));
        TASSERT(el->getOwnerDocument() == doc);
        TASSERT(el->getParentNode() == doc);

        TASSERT(!XMLString::compareString(el->getPrefix(), X("foo")));

        TASSERT(!XMLString::compareString(el->getTagName(), X("foo:docName")));
        TASSERT(el->hasChildNodes() == false);


        //
        // Creating a second document with the same docType object should fail.
        //
        try
        {
            DOMDocument* doc2 = impl->createDocument(X("pubId"), X("foo:docName"), dt);
            UNUSED(doc2); // silence warning
            TASSERT(false);  // should not reach here.
        }
        catch ( DOMException &e)
        {
            TASSERT(e.code == DOMException::WRONG_DOCUMENT_ERR);
        }
        catch (...)
        {
            TASSERT(false);  // Wrong exception thrown.
        }

        // release the document, the documentType (dt) still has the owner, and thus no need to release
        doc->release();

        // Creating a document with null NamespaceURI and DocumentType
        doc = impl->createDocument(X("pubId"), X("foo:docName"), 0);
        doc->release();

        // Namespace tests of createDocument are covered by createElementNS below
    }





    //
    //  CreateElementNS methods
    //

    {

        // Set up an initial (root element only) document.
        //
        DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(X("Core"));


        DOMDocumentType* dt = impl->createDocumentType(X("foo:docName"), X("pubId"), X("http://sysId"));


        DOMDocument* doc = impl->createDocument(X("http://document.namespace"), X("foo:docName"), dt);
        DOMElement* rootEl = doc->getDocumentElement();

        //
        // CreateElementNS
        //
        DOMElement* ela = doc->createElementNS(X("http://nsa"), X("a:ela"));  // prefix and URI

        TASSERT(!XMLString::compareString(ela->getNodeName(), X("a:ela")));
        TASSERT(!XMLString::compareString(ela->getNamespaceURI(), X("http://nsa")));
        TASSERT(!XMLString::compareString(ela->getPrefix(), X("a")));
        TASSERT(!XMLString::compareString(ela->getLocalName(), X("ela")));
        TASSERT(!XMLString::compareString(ela->getTagName(), X("a:ela")));

        DOMElement* elb = doc->createElementNS(X("http://nsb"), X("elb"));    //  URI, no prefix.

        TASSERT(!XMLString::compareString(elb->getNodeName(), X("elb")));
        TASSERT(!XMLString::compareString(elb->getNamespaceURI(), X("http://nsb")));
        TASSERT(!XMLString::compareString(elb->getPrefix(), XMLUni::fgZeroLenString));
        TASSERT(!XMLString::compareString(elb->getLocalName(), X("elb")));
        TASSERT(!XMLString::compareString(elb->getTagName(), X("elb")));

        DOMElement* elc = doc->createElementNS(XMLUni::fgZeroLenString, X("elc"));              // No URI, no prefix.

        TASSERT(!XMLString::compareString(elc->getNodeName(), X("elc")));
        TASSERT(!XMLString::compareString(elc->getNamespaceURI(), XMLUni::fgZeroLenString));
        TASSERT(!XMLString::compareString(elc->getPrefix(), XMLUni::fgZeroLenString));
        TASSERT(!XMLString::compareString(elc->getLocalName(), X("elc")));
        TASSERT(!XMLString::compareString(elc->getTagName(), X("elc")));

        rootEl->appendChild(ela);
        rootEl->appendChild(elb);
        rootEl->appendChild(elc);

        // Badly formed qualified name
        EXCEPTION_TEST(doc->createElementNS(X("http://nsa"), X("<a")), DOMException::INVALID_CHARACTER_ERR);

        EXCEPTION_TEST(doc->createElementNS(X("http://nsa"), X(":a")), DOMException::NAMESPACE_ERR);

        EXCEPTION_TEST(doc->createElementNS(X("http://nsa"), X("a:")), DOMException::NAMESPACE_ERR);

        EXCEPTION_TEST(doc->createElementNS(X("http://nsa"), X("a::a")), DOMException::NAMESPACE_ERR);

        EXCEPTION_TEST(doc->createElementNS(X("http://nsa"), X("a:a:a")), DOMException::NAMESPACE_ERR);

        // xml:a must have namespaceURI == "http://www.w3.org/XML/1998/namespace"
        TASSERT(!XMLString::compareString(doc->createElementNS(X("http://www.w3.org/XML/1998/namespace"), X("xml:a"))->getNamespaceURI(), X("http://www.w3.org/XML/1998/namespace")));
        EXCEPTION_TEST(doc->createElementNS(X("http://nsa"), X("xml:a")), DOMException::NAMESPACE_ERR);
        EXCEPTION_TEST(doc->createElementNS(XMLUni::fgZeroLenString, X("xml:a")), DOMException::NAMESPACE_ERR);
        EXCEPTION_TEST(doc->createElementNS(0,  X("xml:a")), DOMException::NAMESPACE_ERR);

        //unlike Attribute, xmlns (no different from foo) can have any namespaceURI for Element
        TASSERT(!XMLString::compareString(doc->createElementNS(X("http://nsa"), X("xmlns"))->getNamespaceURI(), X("http://nsa")));

        TASSERT(!XMLString::compareString(doc->createElementNS(X("http://www.w3.org/XML/1998/namespace"), X("xmlns"))->getNamespaceURI(), X("http://www.w3.org/XML/1998/namespace")));
        TASSERT(!XMLString::compareString(doc->createElementNS(XMLUni::fgZeroLenString, X("xmlns"))->getNamespaceURI(), XMLUni::fgZeroLenString));
        TASSERT(!XMLString::compareString(doc->createElementNS(0,  X("xmlns"))->getNamespaceURI(), XMLUni::fgZeroLenString));

        //unlike Attribute, xmlns:a (no different from foo:a) can have any namespaceURI for Element
        //except "" and null
        TASSERT(!XMLString::compareString(doc->createElementNS(X("http://nsa"), X("xmlns:a"))->getNamespaceURI(), X("http://nsa")));
        TASSERT(!XMLString::compareString(doc->createElementNS(X("http://www.w3.org/XML/1998/namespace"), X("xmlns:a"))->getNamespaceURI(), X("http://www.w3.org/XML/1998/namespace")));
        EXCEPTION_TEST(doc->createElementNS(XMLUni::fgZeroLenString, X("xmlns:a")), DOMException::NAMESPACE_ERR);
        EXCEPTION_TEST(doc->createElementNS(0, X("xmlns:a")), DOMException::NAMESPACE_ERR);

        //In fact, any prefix != null should have a namespaceURI != 0 or != ""
        TASSERT(!XMLString::compareString(doc->createElementNS(X("http://nsa"), X("foo:a"))->getNamespaceURI(), X("http://nsa")));
        EXCEPTION_TEST(doc->createElementNS(XMLUni::fgZeroLenString, X("foo:a")), DOMException::NAMESPACE_ERR);
        EXCEPTION_TEST(doc->createElementNS(0,  X("foo:a")), DOMException::NAMESPACE_ERR);

        //Change prefix
        DOMElement* elem = doc->createElementNS(X("http://nsa"), X("foo:a"));
        elem->setPrefix(X("bar"));
        TASSERT(!XMLString::compareString(elem->getNodeName(), X("bar:a")));
        TASSERT(!XMLString::compareString(elem->getNamespaceURI(), X("http://nsa")));
        TASSERT(!XMLString::compareString(elem->getPrefix(), X("bar")));
        TASSERT(!XMLString::compareString(elem->getLocalName(), X("a")));
        TASSERT(!XMLString::compareString(elem->getTagName(), X("bar:a")));
        //The spec does not prevent us from setting prefix to a node without prefix
        elem = doc->createElementNS(X("http://nsa"), X("a"));
        TASSERT(!XMLString::compareString(elem->getPrefix(), XMLUni::fgZeroLenString));
        elem->setPrefix(X("bar"));
        TASSERT(!XMLString::compareString(elem->getNodeName(), X("bar:a")));
        TASSERT(!XMLString::compareString(elem->getNamespaceURI(), X("http://nsa")));
        TASSERT(!XMLString::compareString(elem->getPrefix(), X("bar")));
        TASSERT(!XMLString::compareString(elem->getLocalName(), X("a")));
        TASSERT(!XMLString::compareString(elem->getTagName(), X("bar:a")));

        //Special case for xml:a where namespaceURI must be xmlURI
        elem = doc->createElementNS(X("http://www.w3.org/XML/1998/namespace"), X("foo:a"));
        elem->setPrefix(X("xml"));
        elem = doc->createElementNS(X("http://nsa"), X("foo:a"));
        EXCEPTION_TEST(elem->setPrefix(X("xml")), DOMException::NAMESPACE_ERR);
        //However, there is no restriction on prefix xmlns
        elem->setPrefix(X("xmlns"));
        //Also an element can not have a prefix with namespaceURI == null or ""
        elem = doc->createElementNS(0, X("a"));
        EXCEPTION_TEST(elem->setPrefix(X("foo")), DOMException::NAMESPACE_ERR);
        elem = doc->createElementNS(XMLUni::fgZeroLenString, X("a"));
        EXCEPTION_TEST(elem->setPrefix(X("foo")), DOMException::NAMESPACE_ERR);

        //Only prefix of Element and Attribute can be changed
        EXCEPTION_TEST(doc->setPrefix(X("foo")), DOMException::NAMESPACE_ERR);

        //Prefix of readonly Element can not be changed.
        //However, there is no way to create such DOMElement* for testing yet.

        // release the document, the documentType (dt) still has the owner, and thus no need to release
        doc->release();
    }




    //
    //  CreateAttributeNS methods
    //

    {

        // Set up an initial (root element only) document.
        //
        DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(X("Core"));



        DOMDocumentType* dt = impl->createDocumentType(X("foo:docName"), X("pubId"), X("http://sysId"));

        DOMDocument* doc = impl->createDocument(X("http://document.namespace"), X("foo:docName"), dt);
        DOMElement* rootEl = doc->getDocumentElement();
        UNUSED(rootEl); // silence warning

        //
        // CreateAttributeNS
        //


        DOMAttr* attra = doc->createAttributeNS(X("http://nsa"), X("a:attra"));       // prefix and URI
        TASSERT(!XMLString::compareString(attra->getNodeName(), X("a:attra")));
        TASSERT(!XMLString::compareString(attra->getNamespaceURI(), X("http://nsa")));

        TASSERT(!XMLString::compareString(attra->getPrefix(), X("a")));

        TASSERT(!XMLString::compareString(attra->getLocalName(), X("attra")));
        TASSERT(!XMLString::compareString(attra->getName(), X("a:attra")));
        TASSERT(attra->getOwnerElement() == 0);

        DOMAttr* attrb = doc->createAttributeNS(X("http://nsb"), X("attrb"));         //  URI, no prefix.
        TASSERT(!XMLString::compareString(attrb->getNodeName(), X("attrb")));
        TASSERT(!XMLString::compareString(attrb->getNamespaceURI(), X("http://nsb")));
        TASSERT(!XMLString::compareString(attrb->getPrefix(), XMLUni::fgZeroLenString));
        TASSERT(!XMLString::compareString(attrb->getLocalName(), X("attrb")));
        TASSERT(!XMLString::compareString(attrb->getName(), X("attrb")));
        TASSERT(attrb->getOwnerElement() == 0);



        DOMAttr* attrc = doc->createAttributeNS(XMLUni::fgZeroLenString, X("attrc"));
        TASSERT(!XMLString::compareString(attrc->getNodeName(), X("attrc")));
        TASSERT(!XMLString::compareString(attrc->getNamespaceURI(), XMLUni::fgZeroLenString));
        TASSERT(!XMLString::compareString(attrc->getPrefix(), XMLUni::fgZeroLenString));
        TASSERT(!XMLString::compareString(attrc->getLocalName(), X("attrc")));
        TASSERT(!XMLString::compareString(attrc->getName(), X("attrc")));
        TASSERT(attrc->getOwnerElement() == 0);

        // Badly formed qualified name
        EXCEPTION_TEST(doc->createAttributeNS(X("http://nsa"), X("<a")), DOMException::INVALID_CHARACTER_ERR);

        EXCEPTION_TEST(doc->createAttributeNS(X("http://nsa"), X(":a")), DOMException::NAMESPACE_ERR);

        EXCEPTION_TEST(doc->createAttributeNS(X("http://nsa"), X("a:")), DOMException::NAMESPACE_ERR);

        EXCEPTION_TEST(doc->createAttributeNS(X("http://nsa"), X("a::a")), DOMException::NAMESPACE_ERR);

        EXCEPTION_TEST(doc->createAttributeNS(X("http://nsa"), X("a:a:a")), DOMException::NAMESPACE_ERR);

        // xml:a must have namespaceURI == "http://www.w3.org/XML/1998/namespace"
        TASSERT(!XMLString::compareString(doc->createAttributeNS(X("http://www.w3.org/XML/1998/namespace"), X("xml:a"))->getNamespaceURI(), X("http://www.w3.org/XML/1998/namespace")));
        EXCEPTION_TEST(doc->createAttributeNS(X("http://nsa"), X("xml:a")), DOMException::NAMESPACE_ERR);
        EXCEPTION_TEST(doc->createAttributeNS(XMLUni::fgZeroLenString, X("xml:a")), DOMException::NAMESPACE_ERR);
        EXCEPTION_TEST(doc->createAttributeNS(0,  X("xml:a")), DOMException::NAMESPACE_ERR);

        //unlike Element, xmlns must have namespaceURI == "http://www.w3.org/2000/xmlns/"
        TASSERT(!XMLString::compareString(doc->createAttributeNS(X("http://www.w3.org/2000/xmlns/"), X("xmlns"))->getNamespaceURI(), X("http://www.w3.org/2000/xmlns/")));
        EXCEPTION_TEST(doc->createAttributeNS(X("http://nsa"), X("xmlns")), DOMException::NAMESPACE_ERR);
        EXCEPTION_TEST(doc->createAttributeNS(X("http://www.w3.org/XML/1998/namespace"), X("xmlns")), DOMException::NAMESPACE_ERR);
        EXCEPTION_TEST(doc->createAttributeNS(XMLUni::fgZeroLenString, X("xmlns")), DOMException::NAMESPACE_ERR);
        EXCEPTION_TEST(doc->createAttributeNS(0,  X("xmlns")), DOMException::NAMESPACE_ERR);

        //unlike Element, xmlns:a must have namespaceURI == "http://www.w3.org/2000/xmlns/"
        TASSERT(!XMLString::compareString(doc->createAttributeNS(X("http://www.w3.org/2000/xmlns/"), X("xmlns:a"))->getNamespaceURI(), X("http://www.w3.org/2000/xmlns/")));
        EXCEPTION_TEST(doc->createAttributeNS(X("http://nsa"), X("xmlns:a")), DOMException::NAMESPACE_ERR);
        EXCEPTION_TEST(doc->createAttributeNS(X("http://www.w3.org/XML/1998/namespace"), X("xmlns:a")), DOMException::NAMESPACE_ERR);
        EXCEPTION_TEST(doc->createAttributeNS(XMLUni::fgZeroLenString, X("xmlns:a")), DOMException::NAMESPACE_ERR);
        EXCEPTION_TEST(doc->createAttributeNS(0,  X("xmlns:a")), DOMException::NAMESPACE_ERR);

        //In fact, any prefix != null should have a namespaceURI != 0 or != ""
        TASSERT(!XMLString::compareString(doc->createAttributeNS(X("http://nsa"), X("foo:a"))->getNamespaceURI(), X("http://nsa")));
        EXCEPTION_TEST(doc->createAttributeNS(XMLUni::fgZeroLenString, X("foo:a")), DOMException::NAMESPACE_ERR);
        EXCEPTION_TEST(doc->createAttributeNS(0,  X("foo:a")), DOMException::NAMESPACE_ERR);

        //Change prefix
        DOMAttr* attr = doc->createAttributeNS(X("http://nsa"), X("foo:a"));
        attr->setPrefix(X("bar"));

        TASSERT(!XMLString::compareString(attr->getNodeName(), X("bar:a")));
        TASSERT(!XMLString::compareString(attr->getNamespaceURI(), X("http://nsa")));
        TASSERT(!XMLString::compareString(attr->getPrefix(), X("bar")));
        TASSERT(!XMLString::compareString(attr->getName(), X("bar:a")));
        //The spec does not prevent us from setting prefix to a node without prefix
        TASSERT(!XMLString::compareString(attr->getLocalName(), X("a")));
        attr = doc->createAttributeNS(X("http://nsa"), X("a"));
        TASSERT(!XMLString::compareString(attr->getPrefix(), XMLUni::fgZeroLenString));
        attr->setPrefix(X("bar"));
        TASSERT(!XMLString::compareString(attr->getNodeName(), X("bar:a")));
        TASSERT(!XMLString::compareString(attr->getNamespaceURI(), X("http://nsa")));
        TASSERT(!XMLString::compareString(attr->getPrefix(), X("bar")));
        TASSERT(!XMLString::compareString(attr->getLocalName(), X("a")));
        TASSERT(!XMLString::compareString(attr->getName(), X("bar:a")));


        //Special case for xml:a where namespaceURI must be xmlURI
        attr = doc->createAttributeNS(X("http://www.w3.org/XML/1998/namespace"), X("foo:a"));
        attr->setPrefix(X("xml"));
        attr = doc->createAttributeNS(X("http://nsa"), X("foo:a"));
        EXCEPTION_TEST(attr->setPrefix(X("xml")), DOMException::NAMESPACE_ERR);
        //Special case for xmlns:a where namespaceURI must be xmlURI
        attr = doc->createAttributeNS(X("http://www.w3.org/2000/xmlns/"), X("foo:a"));

        attr->setPrefix(X("xmlns"));
        attr = doc->createAttributeNS(X("http://nsa"), X("foo:a"));
        EXCEPTION_TEST(attr->setPrefix(X("xmlns")), DOMException::NAMESPACE_ERR);
        //Special case for xmlns where no prefix can be set
        attr = doc->createAttributeNS(X("http://www.w3.org/2000/xmlns/"), X("xmlns"));

        EXCEPTION_TEST(attr->setPrefix(X("foo")), DOMException::NAMESPACE_ERR);
        EXCEPTION_TEST(attr->setPrefix(X("xmlns")), DOMException::NAMESPACE_ERR);
        //Also an attribute can not have a prefix with namespaceURI == null or ""
        attr = doc->createAttributeNS(XMLUni::fgZeroLenString, X("a"));
        EXCEPTION_TEST(attr->setPrefix(X("foo")), DOMException::NAMESPACE_ERR);
        attr = doc->createAttributeNS(0, X("a"));
        EXCEPTION_TEST(attr->setPrefix(X("foo")), DOMException::NAMESPACE_ERR);

        //Only prefix of Element and Attribute can be changed
        EXCEPTION_TEST(doc->setPrefix(X("foo")), DOMException::NAMESPACE_ERR);

        //Prefix of readonly Attribute can not be changed.
        //However, there is no way to create such DOMAttribute for testing yet.

        // release the document, the documentType (dt) still has the owner, and thus no need to release
        doc->release();
    }



    //
    //  getElementsByTagName*
    //

    {

        // Set up an initial (root element only) document.
        //
        DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(X("Core"));
        DOMDocumentType* dt = impl->createDocumentType(X("foo:docName"), X("pubId"), X("http://sysId"));

        DOMDocument* doc = impl->createDocument(X("http://document.namespace"), X("foo:docName"), dt);

        DOMElement* rootEl = doc->getDocumentElement();

        //
        // Populate the document
        //

        DOMElement* ela = doc->createElementNS(X("http://nsa"), X("a:ela"));
        rootEl->appendChild(ela);

        DOMElement* elb = doc->createElementNS(X("http://nsb"), X("elb"));
        rootEl->appendChild(elb);

        DOMElement* elc = doc->createElementNS(XMLUni::fgZeroLenString, X("elc"));
        rootEl->appendChild(elc);

        DOMElement* eld = doc->createElementNS(X("http://nsa"), X("d:ela"));
        rootEl->appendChild(eld);

        DOMElement* ele = doc->createElementNS(X("http://nse"), X("elb"));
        rootEl->appendChild(ele);


        //
        // Access with DOM Level 1 getElementsByTagName
        //

        DOMNodeList* nl;

        nl = doc->getElementsByTagName(X("a:ela"));
        TASSERT(nl->getLength() == 1);
        TASSERT(nl->item(0) == ela);

        nl = doc->getElementsByTagName(X("elb"));
        TASSERT(nl->getLength() == 2);
        TASSERT(nl->item(0) == elb);
        TASSERT(nl->item(1) == ele);

        nl = doc->getElementsByTagName(X("d:ela"));
        TASSERT(nl->getLength() == 1);
        TASSERT(nl->item(0) == eld);

        //
        //  Access with DOM Level 2 getElementsByTagNameNS
        //

        nl = doc->getElementsByTagNameNS(XMLUni::fgZeroLenString, X("elc"));
        TASSERT(nl->getLength() == 1);
        TASSERT(nl->item(0) == elc);

        nl = doc->getElementsByTagNameNS(0, X("elc"));
        TASSERT(nl->getLength() == 1);
        TASSERT(nl->item(0) == elc);

        nl = doc->getElementsByTagNameNS(X("http://nsa"), X("ela"));
        TASSERT(nl->getLength() == 2);
        TASSERT(nl->item(0) == ela);
        TASSERT(nl->item(1) == eld);

        nl = doc->getElementsByTagNameNS(XMLUni::fgZeroLenString, X("elb"));
        TASSERT(nl->getLength() == 0);

        nl = doc->getElementsByTagNameNS(X("http://nsb"), X("elb"));
        TASSERT(nl->getLength() == 1);
        TASSERT(nl->item(0) == elb);

        nl = doc->getElementsByTagNameNS(X("*"), X("elb"));
        TASSERT(nl->getLength() == 2);
        TASSERT(nl->item(0) == elb);
        TASSERT(nl->item(1) == ele);

        nl = doc->getElementsByTagNameNS(X("http://nsa"), X("*"));
        TASSERT(nl->getLength() == 2);
        TASSERT(nl->item(0) == ela);
        TASSERT(nl->item(1) == eld);

        nl = doc->getElementsByTagNameNS(X("*"), X("*"));
        TASSERT(nl->getLength() == 6);     // Gets the document root element, plus 5 more

        TASSERT(nl->item(6) == 0);
        // TASSERT(nl->item(-1) == 0);

        nl = rootEl->getElementsByTagNameNS(X("*"), X("*"));
        TASSERT(nl->getLength() == 5);


        nl = doc->getElementsByTagNameNS(X("http://nsa"), X("d:ela"));
        TASSERT(nl->getLength() == 0);


        //
        // Node lists are Live
        //

        nl = doc->getElementsByTagNameNS(X("*"), X("*"));
        DOMNodeList* nla = ela->getElementsByTagNameNS(X("*"), X("*"));

        TASSERT(nl->getLength() == 6);
        TASSERT(nla->getLength() == 0);

        DOMNode* rem = rootEl->removeChild(elc);
        rem->release();
        TASSERT(nl->getLength() == 5);
        TASSERT(nla->getLength() == 0);

        ela->appendChild(elc);
        TASSERT(nl->getLength() == 6);
        TASSERT(nla->getLength() == 1);

        // release the document, the documentType (dt) still has the owner, and thus no need to release
        doc->release();
    }



   //
    // Attributes and NamedNodeMaps.
    //

    {

        // Set up an initial (root element only) document.
        //
        DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(X("Core"));

        DOMDocumentType* dt = impl->createDocumentType(X("foo:docName"), X("pubId"), X("http://sysId"));

        DOMDocument* doc = impl->createDocument(X("http://document.namespace"), X("foo:docName"), dt);
        DOMElement* rootEl = doc->getDocumentElement();

        //
        // Create a set of attributes and hang them on the root element.
        //

        DOMAttr* attra = doc->createAttributeNS(X("http://nsa"), X("a:attra"));
        DOMNode* rem = rootEl->setAttributeNodeNS(attra);
        if (rem)
            rem->release();

        //
        // Check that the attribute nodes were created with the correct properties.
        //
        TASSERT(!XMLString::compareString(attra->getNodeName(), X("a:attra")));
        TASSERT(!XMLString::compareString(attra->getNamespaceURI(), X("http://nsa")));

        TASSERT(!XMLString::compareString(attra->getLocalName(), X("attra")));
        TASSERT(!XMLString::compareString(attra->getName(), X("a:attra")));
        TASSERT(attra->getNodeType() == DOMNode::ATTRIBUTE_NODE);
        TASSERT(!XMLString::compareString(attra->getNodeValue(), XMLUni::fgZeroLenString));

        TASSERT(!XMLString::compareString(attra->getPrefix(), X("a")));
        TASSERT(attra->getSpecified() == true);
        TASSERT(!XMLString::compareString(attra->getValue(), XMLUni::fgZeroLenString));

        //
        // Create a set of attributes and hang them on the root element.
        //

        DOMAttr* attrb = doc->createAttributeNS(X("http://nsb"), X("attrb"));
        rem = rootEl->setAttributeNodeNS(attrb);
        if (rem)
            rem->release();

        DOMAttr* attrc = doc->createAttributeNS(XMLUni::fgZeroLenString, X("attrc"));
        rem = rootEl->setAttributeNodeNS(attrc);
        if (rem)
            rem->release();

        // this one will replace the attra
        DOMAttr* attrd = doc->createAttributeNS(X("http://nsa"), X("d:attra"));
        rem = rootEl->setAttributeNodeNS(attrd);
        TASSERT(attra->getOwnerElement() == 0);
        if (rem)
            rem->release();

        DOMAttr* attre = doc->createAttributeNS(X("http://nse"), X("attrb"));
        rem = rootEl->setAttributeNodeNS(attre);
        if (rem)
            rem->release();

        // Test methods of NamedNodeMap
        DOMNamedNodeMap* nnm = rootEl->getAttributes();
        TASSERT(nnm->getLength() == 4);


        TASSERT(nnm->getNamedItemNS(X("http://nsa"), X("attra")) == attrd);
        TASSERT(nnm->getNamedItemNS(X("http://nsb"), X("attrb")) == attrb);
        TASSERT(nnm->getNamedItemNS(XMLUni::fgZeroLenString, X("attra")) == 0);
        TASSERT(nnm->getNamedItemNS(X("http://nsa"), X("attrb")) == 0);

        TASSERT(nnm->getNamedItemNS(X("http://nse"), X("attrb")) == attre);

        TASSERT(nnm->getNamedItemNS(XMLUni::fgZeroLenString, X("attrc")) == attrc);

        // Test hasAttributes, hasAttribute, hasAttributeNS
        TASSERT(doc->hasAttributes() ==  false);
        TASSERT(attrc->hasAttributes() == false);
        TASSERT(rootEl->hasAttributes() == true);
        TASSERT(rootEl->hasAttribute(X("attrc")) == true);

        TASSERT(rootEl->hasAttribute(X("wrong")) == false);

        TASSERT(rootEl->hasAttributeNS(X("http://nsa"), X("attra")) == true);
        TASSERT(rootEl->hasAttributeNS(X("http://nsa"), X("wrong")) == false);

        // release the document, the documentType (dt) still has the owner, and thus no need to release
        doc->release();
    }


    //
    //
    //

}


//---------------------------------------------------------------------------------------
//
//   DOMReleaseTests    Test if the release() function
//
//---------------------------------------------------------------------------------------
void DOMReleaseTests()
{
    XMLCh tempStr[4000];
    XMLCh tempStr2[4000];
    XMLCh tempStr3[4000];
    XMLString::transcode("status", tempStr, 3999);
    XMLString::transcode("true", tempStr2, 3999);
    XMLString::transcode("root", tempStr3, 3999);

    //create document
    DOMDocument*  cpXMLDocument;
    cpXMLDocument = DOMImplementation::getImplementation()->createDocument();
    //create root element
    DOMElement*   cpRoot = cpXMLDocument->createElement(tempStr3);
    //create status attribute
    cpRoot->setAttribute(tempStr,tempStr2);
    DOMAttr* pAttr = cpRoot->getAttributeNode(tempStr);

    //simulate setting the attribute value
    //   The setValue and setAttribute should call release internally so that
    //   the overall memory usage is not increased
    int i = 0;
    for(i=0;i<20000;i++)
    {
        pAttr->setValue(tempStr2);
    }
    for(i=0;i<20000;i++)
    {
        //same problem
        cpRoot->removeAttribute(tempStr);
        cpRoot->setAttribute(tempStr,tempStr2);
    }

    //simulate changing node value
    //   the overall memory usage is not increased
    char tempchar[4000];
    for(i=0;i<20000;i++)
    {
        sprintf(tempchar, "time is %lu\n",XMLPlatformUtils::getCurrentMillis());
        XMLSize_t len = strlen(tempchar), j;
        for (j = len; j < 4000-len; j++)
            tempchar[j] = 'a';
        tempchar[j]=0;
        pAttr->setNodeValue(X(tempchar));
    }

    DOMText*  text = cpXMLDocument->createTextNode(tempStr3);
    for(i=0;i<20000;i++)
    {
        sprintf(tempchar, "time is %lu\n",XMLPlatformUtils::getCurrentMillis());
        XMLSize_t len = strlen(tempchar), j;
        for (j = len; j < 4000-len; j++)
            tempchar[j] = 'a';
        tempchar[j]=0;
        text->setNodeValue(X(tempchar));
    }

    cpXMLDocument->release();

}


//---------------------------------------------------------------------------------------
//
//   main
//
//---------------------------------------------------------------------------------------
int  mymain()
{
    try {
        XMLPlatformUtils::Initialize();
    }
    catch (const XMLException& toCatch) {
        char *pMessage = XMLString::transcode(toCatch.getMessage());
        fprintf(stderr, "Error during XMLPlatformUtils::Initialize(). \n"
            "  Message is: %s\n", pMessage);
        XMLString::release(&pMessage);
        return -1;
    }

    DOMBasicTests();
    DOMNSTests();
    DOMReleaseTests();

    //
    //  Print Final allocation stats for full set of tests
    //
    XMLPlatformUtils::Terminate();

    return 0;

}

int  main() {
   for (int i = 0; i<3; i++)
        mymain();

    if (errorOccurred) {
        printf("Test Failed\n");
        return 4;
    }

    printf("Test Run Successfully\n");

    return 0;
}
