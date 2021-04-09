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



/**
 * This class tests methods for XML DOM implementation
 * DOMException errors are tested by calls to DOMExceptionsTest from: Main, docBuilder...
 *
 */

#include <stdio.h>
#include "DTest.h"
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/BinInputStream.hpp>
#include <xercesc/util/regx/Match.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOMLSException.hpp>
#include <xercesc/dom/DOMLSParserFilter.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/validators/common/CMStateSet.hpp>

#define UNUSED(x) { if(x!=0){} }

#define EXCEPTIONSTEST(operation, expectedException, resultFlag, testNum) \
    {                                                               \
        try                                                         \
        {                                                           \
            operation;                                              \
            fprintf(stderr, "Exceptions Test # %d: no Exception thrown->\n", testNum); \
        }                                                           \
        catch (DOMException &e)                                 \
        {                                                           \
            if (e.code != expectedException) {                      \
                fprintf(stderr, "Exceptions Test # %d: wrong DOMException thrown->\n", \
                    testNum);                                       \
                resultFlag = false;                                 \
            }                                                       \
        }                                                           \
        catch (...)                                                 \
        {                                                           \
            fprintf(stderr, "Exceptions Test # %d: unknown exception thrown->\n",    \
                testNum);                                           \
            resultFlag = false;                                     \
        }                                                           \
    }

#define USERDATAHANDLERTEST(userhandler, uoperation, ukey, udata, usrc, udst, uline) \
    if (userhandler.getCurrentType() != uoperation) {\
        fprintf(stderr, "DOMUserDataHandler::handler's operationType does not work in line %i\n", uline); \
        OK = false; \
    } \
    if (XMLString::compareString(userhandler.getCurrentKey(), ukey)) {\
        fprintf(stderr, "DOMUserDataHandler::handler's key does not work in line %i\n", uline); \
        OK = false; \
    } \
    if (userhandler.getCurrentData() != udata) {\
        fprintf(stderr, "DOMUserDataHandler::handler's data does not work in line %i\n", uline); \
        OK = false; \
    } \
    if (userhandler.getCurrentSrc() != usrc) {\
        fprintf(stderr, "DOMUserDataHandler::handler's src does not work in line %i\n", uline); \
        OK = false; \
    } \
    if (userhandler.getCurrentDst() != udst) {\
        fprintf(stderr, "DOMUserDataHandler::handler's dst does not work in line %i\n", uline); \
        OK = false; \
    }


#define LOOKUPDEFAULTNSTEST(thisNode, uri, pass, line) \
    if(thisNode->isDefaultNamespace(uri)) { \
        if(!pass) { \
            fprintf(stderr, "DOMNode::isDefaultNamespace returned true in line %i\n", line); \
            OK = false; \
        } \
    } \
    else { \
        if(pass) { \
            fprintf(stderr, "DOMNode::isDefaultNamespace returned false in line %i\n", line); \
            OK = false; \
        } \
    }


#define LOOKUPNSTEST(thisNode, prefix, uri, pass, line) \
    prefixResult = XMLString::compareString(thisNode->lookupPrefix(uri), prefix); \
    uriResult = XMLString::compareString(thisNode->lookupNamespaceURI(prefix), uri); \
    if(pass) { \
        if(prefixResult != 0) { \
        fprintf(stderr, "DOMNode::lookupNamespacePrefix does not work in line %i\n", line); \
        OK = false; \
        } \
        if(uriResult != 0) { \
        fprintf(stderr, "DOMNode::lookupNamespaceURI does not work in line %i\n", line); \
        OK = false;\
            } \
        } \
    else { \
        if(prefixResult == 0) { \
        fprintf(stderr, "DOMNode::lookupNamespacePrefix does not work in line %i\n", line); \
        OK = false; \
        } \
        if(uriResult == 0) { \
        fprintf(stderr, "DOMNode::lookupNamespaceURI does not work in line %i\n", line); \
        OK = false; \
            } \
        } \

int prefixResult;
int uriResult;

#define COMPARETREEPOSITIONTEST(thisNode, otherNode, position, line) \
    myposition = thisNode->compareDocumentPosition(otherNode); \
    if ((myposition & position) == 0) {\
        fprintf(stderr, "DOMNode::compareDocumentPosition does not work in line %i\n", line); \
        OK = false; \
    }

// temp position for compareDocumentPosition
short myposition;

//temp XMLCh String Buffer
XMLCh tempStr[4000];
XMLCh tempStr2[4000];
XMLCh tempStr3[4000];
XMLCh tempStr4[4000];
XMLCh tempStr5[4000];

//DOMUserDataHandler
myUserDataHandler userhandler;

DOMElement*                 DOMTest::testElementNode;
DOMAttr*                    DOMTest::testAttributeNode;
DOMText*                    DOMTest::testTextNode;
DOMCDATASection*            DOMTest::testCDATASectionNode;
DOMEntityReference*         DOMTest::testEntityReferenceNode;
DOMEntity*                  DOMTest::testEntityNode;
DOMProcessingInstruction*   DOMTest::testProcessingInstructionNode;
DOMComment*                 DOMTest::testCommentNode;
DOMDocument*                DOMTest::testDocumentNode;
DOMDocumentType*            DOMTest::testDocumentTypeNode;
DOMDocumentFragment*        DOMTest::testDocumentFragmentNode;
DOMNotation*                DOMTest::testNotationNode;

/**
 *
 *
 */

DOMTest::DOMTest()
{
}


/**
 *
 * @return DOMDocument
 *
 */
DOMDocument* DOMTest::createDocument() {
    XMLCh coreStr[100];
    XMLString::transcode("Core",coreStr,99);

    DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(coreStr);
    return impl->createDocument();
}


/**
 *
 * @return DOMDocumentType
 * @param name XMLCh*
 *
 */
DOMDocumentType* DOMTest::createDocumentType(DOMDocument* doc, XMLCh* name) {
    return doc->createDocumentType(name);    //Replace with a DOMDocumentType* creator
}


/**
 *
 * @return org.w3c.dom.DOMEntity
 * @param doc org.w3c.dom.DOMDocument
 * @param name XMLCh*
 *
 */
DOMEntity* DOMTest::createEntity(DOMDocument* doc, XMLCh* name) {
    return doc->createEntity(name);
}



/**
 *
 * @return org.w3c.dom.DOMNotation
 * @param doc org.w3c.dom.DOMDocument
 * @param name XMLCh*
 *
 */
DOMNotation* DOMTest::createNotation(DOMDocument* doc, XMLCh* name) {
    return doc->createNotation(name);
}


/**
 * This method builds test documents for the XML DOM implementation
 * @param document org.w3c.dom.DOMDocument
 * @param name document's name
 * @param type document's type
 *
 */
bool DOMTest::docBuilder(DOMDocument* document, XMLCh* nameIn)
{
    XMLCh* name = XMLString::replicate(nameIn);

    DOMDocument* doc = document;
    bool OK = true;

    //name + "FirstElement"
    XMLString::transcode("FirstElement", tempStr2, 3999);
    XMLString::copyString(tempStr, name);
    XMLString::catString(tempStr, tempStr2);

    DOMElement* docFirstElement = doc->createElement(tempStr);
    doc->appendChild(docFirstElement);

    //name + "FirstElement", name + "firstElement"
    XMLString::catString(tempStr, name);
    XMLString::transcode("FirstElement", tempStr2, 3999);
    XMLString::catString(tempStr, tempStr2);
    XMLString::catString(tempStr2, name);
    XMLString::transcode("firstElement", tempStr3, 3999);
    XMLString::catString(tempStr2, tempStr3);
    docFirstElement->setAttribute(tempStr, tempStr2);
    DOMAttr* docFirstElementAttr = docFirstElement->getAttributeNode(tempStr);

    //name + "TargetProcessorChannel" + "This is " + doc->getNodeName() + "'s processing instruction");
    XMLString::copyString(tempStr, name);
    XMLString::transcode("TargetProcessorChannel", tempStr2, 3999);
    XMLString::catString(tempStr, tempStr2);
    XMLString::transcode("This is ", tempStr2, 3999);
    XMLString::catString(tempStr2, doc->getNodeName());
    XMLString::transcode("'s processing instruction", tempStr3, 3999);
    XMLString::catString(tempStr2, tempStr3);

    DOMProcessingInstruction* docProcessingInstruction = doc->createProcessingInstruction(tempStr, tempStr2);
    docFirstElement->appendChild(docProcessingInstruction);

    //name + "TestBody"
    XMLString::copyString(tempStr, name);
    XMLString::transcode("TestBody", tempStr2, 3999);
    XMLString::catString(tempStr, tempStr2);
    DOMElement* docBody = doc->createElement(tempStr);
    docFirstElement->appendChild(docBody);

    //name + "BodyLevel21"
    XMLString::copyString(tempStr, name);
    XMLString::transcode("BodyLevel21", tempStr2, 3999);
    XMLString::catString(tempStr, tempStr2);
    DOMElement* docBodyLevel21 = doc->createElement(tempStr);

    //name + "BodyLevel22"
    XMLString::copyString(tempStr, name);
    XMLString::transcode("BodyLevel22", tempStr2, 3999);
    XMLString::catString(tempStr, tempStr2);
    DOMElement* docBodyLevel22 = doc->createElement(tempStr);

    //name + "BodyLevel23"
    XMLString::copyString(tempStr, name);
    XMLString::transcode("BodyLevel23", tempStr2, 3999);
    XMLString::catString(tempStr, tempStr2);
    DOMElement* docBodyLevel23 = doc->createElement(tempStr);

    //name + "BodyLevel24"
    XMLString::copyString(tempStr, name);
    XMLString::transcode("BodyLevel24", tempStr2, 3999);
    XMLString::catString(tempStr, tempStr2);
    DOMElement* docBodyLevel24 = doc->createElement(tempStr);

    docBody->appendChild(docBodyLevel21);
    docBody->appendChild(docBodyLevel22);
    docBody->appendChild(docBodyLevel23);
    docBody->appendChild(docBodyLevel24);

    //name + "BodyLevel31"
    XMLString::copyString(tempStr, name);
    XMLString::transcode("BodyLevel31", tempStr2, 3999);
    XMLString::catString(tempStr, tempStr2);
    DOMElement* docBodyLevel31 = doc->createElement(tempStr);

    //name + "BodyLevel32"
    XMLString::copyString(tempStr, name);
    XMLString::transcode("BodyLevel32", tempStr2, 3999);
    XMLString::catString(tempStr, tempStr2);
    DOMElement* docBodyLevel32 = doc->createElement(tempStr);

    //name + "BodyLevel33"
    XMLString::copyString(tempStr, name);
    XMLString::transcode("BodyLevel33", tempStr2, 3999);
    XMLString::catString(tempStr, tempStr2);
    DOMElement* docBodyLevel33 = doc->createElement(tempStr);

    //name + "BodyLevel34"
    XMLString::copyString(tempStr, name);
    XMLString::transcode("BodyLevel34", tempStr2, 3999);
    XMLString::catString(tempStr, tempStr2);
    DOMElement* docBodyLevel34 = doc->createElement(tempStr);

    docBodyLevel21->appendChild(docBodyLevel31);
    docBodyLevel21->appendChild(docBodyLevel32);
    docBodyLevel22->appendChild(docBodyLevel33);
    docBodyLevel22->appendChild(docBodyLevel34);

    //name + "BodyLevel31'sChildTextNode11"
    XMLString::copyString(tempStr, name);
    XMLString::transcode("BodyLevel31'sChildTextNode11", tempStr2, 3999);
    XMLString::catString(tempStr, tempStr2);
    DOMText* docTextNode11 = doc->createTextNode(tempStr);

    //name + "BodyLevel31'sChildTextNode12"
    XMLString::copyString(tempStr, name);
    XMLString::transcode("BodyLevel31'sChildTextNode12", tempStr2, 3999);
    XMLString::catString(tempStr, tempStr2);
    DOMText* docTextNode12 = doc->createTextNode(tempStr);

    //name + "BodyLevel31'sChildTextNode13"
    XMLString::copyString(tempStr, name);
    XMLString::transcode("BodyLevel31'sChildTextNode13", tempStr2, 3999);
    XMLString::catString(tempStr, tempStr2);
    DOMText* docTextNode13 = doc->createTextNode(tempStr);

    //name + "TextNode2"
    XMLString::copyString(tempStr, name);
    XMLString::transcode("TextNode2", tempStr2, 3999);
    XMLString::catString(tempStr, tempStr2);
    DOMText* docTextNode2 = doc->createTextNode(tempStr);

    //name + "TextNode3"
    XMLString::copyString(tempStr, name);
    XMLString::transcode("TextNode3", tempStr2, 3999);
    XMLString::catString(tempStr, tempStr2);
    DOMText* docTextNode3 = doc->createTextNode(tempStr);

    //name + "TextNode4"
    XMLString::copyString(tempStr, name);
    XMLString::transcode("TextNode4", tempStr2, 3999);
    XMLString::catString(tempStr, tempStr2);
    DOMText* docTextNode4 = doc->createTextNode(tempStr);

    docBodyLevel31->appendChild(docTextNode11);
    docBodyLevel31->appendChild(docTextNode12);
    docBodyLevel31->appendChild(docTextNode13);
    docBodyLevel32->appendChild(docTextNode2);
    docBodyLevel33->appendChild(docTextNode3);
    docBodyLevel34->appendChild(docTextNode4);

    //"<![CDATA[<greeting>Hello, world!</greeting>]]>"
    XMLString::transcode("<![CDATA[<greeting>Hello, world!</greeting>]]>", tempStr, 3999);
    DOMCDATASection* docCDATASection = doc->createCDATASection(tempStr);
    docBodyLevel23->appendChild(docCDATASection);

    //"This should be a comment of some kind "
    XMLString::transcode("This should be a comment of some kind ", tempStr, 3999);
    DOMComment* docComment = doc->createComment(tempStr);

    //Test compareDocumentPosition before append
    COMPARETREEPOSITIONTEST(docFirstElementAttr, docComment, DOMNode::DOCUMENT_POSITION_DISCONNECTED, __LINE__);

    docBodyLevel23->appendChild(docComment);

    //"ourEntityNode"
    XMLString::transcode("ourEntityNode", tempStr, 3999);
    DOMEntityReference* docReferenceEntity = doc->createEntityReference(tempStr);
    docBodyLevel24->appendChild(docReferenceEntity);

    DOMTest make;

    //"ourNotationNode"
    XMLString::transcode("ourNotationNode", tempStr, 3999);
    DOMNotation* docNotation = make.createNotation(doc, tempStr);
    DOMNode*  abc1 = doc->getFirstChild();
    DOMDocumentType* docType = (DOMDocumentType*) abc1;
    DOMNode* rem = docType->getNotations()->setNamedItem(docNotation);
    if (rem)
        rem->release();


//***********Do some quick compareDocumentPosition tests
//The tree now looks like
//
// docFirstElement (has docFirstElementAttr)
//      |
//      |_ docProcessInstruction
//      |
//      |_ docBody
//            |
//            |_ docBodyLevel21
//            |         |
//            |         |_ docBodyLevel31
//            |         |        |
//            |         |        |_ docTextNode11
//            |         |        |
//            |         |        |_ docTextNode12
//            |         |        |
//            |         |        |_ docTextNode13
//            |         |
//            |         |_ docBodyLevel32
//            |                  |
//            |                  |_ docTextNode2
//            |
//            |_ docBodyLevel22
//            |         |
//            |         |_ docBodyLevel33
//            |         |        |
//            |         |        |_ docTextNode3
//            |         |
//            |         |
//            |         |_ docBodyLevel34
//            |                  |
//            |                  |_ docTextNode4
//            |
//            |_ docBodyLevel23
//            |         |
//            |         |_ docCDATASection
//            |         |
//            |         |_ docComment
//            |
//            |_ docBodyLevel24
//                      |
//                      |_ docReferenceEntity
//

    COMPARETREEPOSITIONTEST(docProcessingInstruction, docBody, DOMNode::DOCUMENT_POSITION_FOLLOWING, __LINE__);
    COMPARETREEPOSITIONTEST(docBodyLevel24, docProcessingInstruction, DOMNode::DOCUMENT_POSITION_PRECEDING, __LINE__);
    COMPARETREEPOSITIONTEST(docBodyLevel23, docBodyLevel21, DOMNode::DOCUMENT_POSITION_PRECEDING, __LINE__);
    COMPARETREEPOSITIONTEST(docBodyLevel21, docTextNode11, DOMNode::DOCUMENT_POSITION_CONTAINED_BY, __LINE__);
    COMPARETREEPOSITIONTEST(docCDATASection, docFirstElement, DOMNode::DOCUMENT_POSITION_CONTAINS, __LINE__);
    COMPARETREEPOSITIONTEST(docReferenceEntity, docFirstElement, DOMNode::DOCUMENT_POSITION_CONTAINS, __LINE__);
    COMPARETREEPOSITIONTEST(docFirstElementAttr, docFirstElement, DOMNode::DOCUMENT_POSITION_PRECEDING, __LINE__);
    COMPARETREEPOSITIONTEST(docFirstElementAttr, docProcessingInstruction, DOMNode::DOCUMENT_POSITION_FOLLOWING, __LINE__);
    COMPARETREEPOSITIONTEST(docProcessingInstruction, docFirstElementAttr, DOMNode::DOCUMENT_POSITION_PRECEDING, __LINE__);
    COMPARETREEPOSITIONTEST(docFirstElementAttr, doc, DOMNode::DOCUMENT_POSITION_PRECEDING, __LINE__);
    COMPARETREEPOSITIONTEST(doc, docFirstElementAttr, DOMNode::DOCUMENT_POSITION_FOLLOWING, __LINE__);
    COMPARETREEPOSITIONTEST(docBodyLevel21, docBodyLevel22, DOMNode::DOCUMENT_POSITION_FOLLOWING, __LINE__);

    COMPARETREEPOSITIONTEST(docNotation, docFirstElement, DOMNode::DOCUMENT_POSITION_FOLLOWING, __LINE__);

    myposition = docFirstElement->compareDocumentPosition(docFirstElement);
    if (myposition != 0) {
        fprintf(stderr, "DOMNode::compareDocumentPosition does not work in line %i\n", __LINE__);
        OK = false;
    }

    //now do some lookupNamespaceURI and lookupNamespacePrefix
    //first lets add some attributes
    XMLString::transcode("http://www.w3.org/2000/xmlns/", tempStr, 3999);
    XMLString::transcode("xmlns:pre1", tempStr2, 3999);
    XMLString::transcode("pre1URI", tempStr3, 3999);
    XMLString::transcode("pre1", tempStr4, 3999);

    DOMAttr *attr1 = doc->createAttributeNS(tempStr, tempStr2);
    attr1->setValue(tempStr3);
    docFirstElement->setAttributeNodeNS(attr1);


    LOOKUPNSTEST(docProcessingInstruction, tempStr4, tempStr3, true, __LINE__);
    LOOKUPNSTEST(docBodyLevel24, tempStr4, tempStr3, true, __LINE__);
    LOOKUPNSTEST(docBodyLevel23, tempStr4, tempStr3, true, __LINE__);
    LOOKUPNSTEST(docBodyLevel21, tempStr4, tempStr3, true, __LINE__);
    LOOKUPNSTEST(docBodyLevel31, tempStr4, tempStr3, true, __LINE__);
    LOOKUPNSTEST(docBodyLevel32, tempStr4, tempStr3, true, __LINE__);
    LOOKUPNSTEST(docCDATASection, tempStr4, tempStr3, true, __LINE__);
    LOOKUPNSTEST(docFirstElement, tempStr4, tempStr3, true, __LINE__);
    LOOKUPNSTEST(docReferenceEntity, tempStr4, tempStr3, true, __LINE__);
    LOOKUPNSTEST(docFirstElementAttr, tempStr4, tempStr3, true, __LINE__);
    LOOKUPNSTEST(doc, tempStr4, tempStr3, true, __LINE__);
    LOOKUPNSTEST(docNotation, tempStr4, tempStr3, false, __LINE__);
    LOOKUPNSTEST(docTextNode2, tempStr4, tempStr3, true, __LINE__);
    LOOKUPNSTEST(docTextNode4, tempStr4, tempStr3, true, __LINE__);
    LOOKUPNSTEST(docComment, tempStr4, tempStr3, true, __LINE__);

    XMLString::transcode("xmlns:pre2", tempStr2, 3999);
    XMLString::transcode("pre2URI", tempStr3, 3999);
    XMLString::transcode("pre2", tempStr4, 3999);

    DOMAttr *attr2 = doc->createAttributeNS(tempStr, tempStr2);
    attr2->setValue(tempStr3);
    docBodyLevel21->setAttributeNodeNS(attr2);


    LOOKUPNSTEST(docProcessingInstruction, tempStr4, tempStr3, false, __LINE__);
    LOOKUPNSTEST(docBodyLevel24, tempStr4, tempStr3, false, __LINE__);
    LOOKUPNSTEST(docBodyLevel23, tempStr4, tempStr3, false, __LINE__);
    LOOKUPNSTEST(docBodyLevel21, tempStr4, tempStr3, true, __LINE__);
    LOOKUPNSTEST(docBodyLevel31, tempStr4, tempStr3, true, __LINE__);
    LOOKUPNSTEST(docBodyLevel32, tempStr4, tempStr3, true, __LINE__);
    LOOKUPNSTEST(docCDATASection, tempStr4, tempStr3, false, __LINE__);
    LOOKUPNSTEST(docFirstElement, tempStr4, tempStr3, false, __LINE__);
    LOOKUPNSTEST(docReferenceEntity, tempStr4, tempStr3, false, __LINE__);
    LOOKUPNSTEST(docFirstElementAttr, tempStr4, tempStr3, false, __LINE__);
    LOOKUPNSTEST(doc, tempStr4, tempStr3, false, __LINE__);
    LOOKUPNSTEST(docNotation, tempStr4, tempStr3, false, __LINE__);
    LOOKUPNSTEST(docComment, tempStr4, tempStr3, false, __LINE__);
    LOOKUPNSTEST(docTextNode2, tempStr4, tempStr3, true, __LINE__);
    LOOKUPNSTEST(docTextNode4, tempStr4, tempStr3, false, __LINE__);


    XMLString::transcode("xmlns", tempStr2, 3999);
    XMLString::transcode("default", tempStr3, 3999);
    XMLString::transcode("", tempStr4, 3999);


    DOMAttr *attr3 = doc->createAttributeNS(tempStr, tempStr2);
    attr3->setValue(tempStr3);
    docFirstElement->setAttributeNodeNS(attr3);

    LOOKUPNSTEST(docProcessingInstruction, 0, tempStr3, true, __LINE__);
    LOOKUPNSTEST(docBodyLevel24, 0, tempStr3, true, __LINE__);
    LOOKUPNSTEST(docBodyLevel23, 0, tempStr3, true, __LINE__);
    LOOKUPNSTEST(docBodyLevel21, 0, tempStr3, true, __LINE__);
    LOOKUPNSTEST(docBodyLevel31, 0, tempStr3, true, __LINE__);
    LOOKUPNSTEST(docBodyLevel32, 0, tempStr3, true, __LINE__);
    LOOKUPNSTEST(docCDATASection, 0, tempStr3, true, __LINE__);
    LOOKUPNSTEST(docFirstElement, 0, tempStr3, true, __LINE__);
    LOOKUPNSTEST(docReferenceEntity, 0, tempStr3, true, __LINE__);
    LOOKUPNSTEST(docFirstElementAttr, 0, tempStr3, true, __LINE__);
    LOOKUPNSTEST(doc, 0, tempStr3, true, __LINE__);
    LOOKUPNSTEST(docComment, 0, tempStr3, true, __LINE__);
    LOOKUPNSTEST(docTextNode2, 0, tempStr3, true, __LINE__);
    LOOKUPNSTEST(docTextNode4, 0, tempStr3, true, __LINE__);

    //this has to be done separately because negative lookup is the same as default ns lookup!!!

    prefixResult = XMLString::compareString(docNotation->lookupPrefix(tempStr3), 0);

    uriResult = XMLString::compareString(docNotation->lookupNamespaceURI(0), 0);
    if(prefixResult != 0) {
        fprintf(stderr, "DOMNode::lookupNamespacePrefix does not work in line %i\n", __LINE__);
        OK = false;
    }
    if(uriResult != 0) {
        fprintf(stderr, "DOMNode::lookupNamespacePrefix does not work in line %i\n", __LINE__);
        OK = false;
    }

    XMLString::transcode("notset", tempStr3, 3999);

    LOOKUPDEFAULTNSTEST(docProcessingInstruction, tempStr3, false, __LINE__);
    LOOKUPDEFAULTNSTEST(docBodyLevel24, tempStr3, false, __LINE__);
    LOOKUPDEFAULTNSTEST(docBodyLevel23, tempStr3, false, __LINE__);
    LOOKUPDEFAULTNSTEST(docBodyLevel21, tempStr3, false, __LINE__);
    LOOKUPDEFAULTNSTEST(docBodyLevel31, tempStr3, false, __LINE__);
    LOOKUPDEFAULTNSTEST(docBodyLevel32, tempStr3, false, __LINE__);
    LOOKUPDEFAULTNSTEST(docCDATASection, tempStr3, false, __LINE__);
    LOOKUPDEFAULTNSTEST(docFirstElement, tempStr3, false, __LINE__);
    LOOKUPDEFAULTNSTEST(docReferenceEntity, tempStr3, false, __LINE__);
    LOOKUPDEFAULTNSTEST(docFirstElementAttr, tempStr3, false, __LINE__);
    LOOKUPDEFAULTNSTEST(doc, tempStr3, false, __LINE__);
    LOOKUPDEFAULTNSTEST(docNotation, tempStr3, false, __LINE__);
    LOOKUPDEFAULTNSTEST(docComment, tempStr3, false, __LINE__);
    LOOKUPDEFAULTNSTEST(docTextNode2, tempStr3, false, __LINE__);
    LOOKUPDEFAULTNSTEST(docTextNode4, tempStr3, false, __LINE__);


    XMLString::transcode("default", tempStr3, 3999);

    LOOKUPDEFAULTNSTEST(docProcessingInstruction, tempStr3, false, __LINE__);
    LOOKUPDEFAULTNSTEST(docBodyLevel24, tempStr3, false, __LINE__);
    LOOKUPDEFAULTNSTEST(docBodyLevel23, tempStr3, false, __LINE__);
    LOOKUPDEFAULTNSTEST(docBodyLevel21, tempStr3, false, __LINE__);
    LOOKUPDEFAULTNSTEST(docBodyLevel31, tempStr3, false, __LINE__);
    LOOKUPDEFAULTNSTEST(docBodyLevel32, tempStr3, false, __LINE__);
    LOOKUPDEFAULTNSTEST(docCDATASection, tempStr3, false, __LINE__);
    LOOKUPDEFAULTNSTEST(docFirstElement, tempStr3, false, __LINE__);
    LOOKUPDEFAULTNSTEST(docReferenceEntity, tempStr3, false, __LINE__);
    LOOKUPDEFAULTNSTEST(docFirstElementAttr, tempStr3, false, __LINE__);
    LOOKUPDEFAULTNSTEST(doc, tempStr3, false, __LINE__);
    LOOKUPDEFAULTNSTEST(docNotation, tempStr3, false, __LINE__);
    LOOKUPDEFAULTNSTEST(docComment, tempStr3, false, __LINE__);
    LOOKUPDEFAULTNSTEST(docTextNode2, tempStr3, false, __LINE__);
    LOOKUPDEFAULTNSTEST(docTextNode4, tempStr3, false, __LINE__);

    //remove the xmlns attr
    docFirstElement->removeAttributeNode(attr3);

    LOOKUPDEFAULTNSTEST(docProcessingInstruction, 0, true, __LINE__);
    LOOKUPDEFAULTNSTEST(docBodyLevel24, 0, true, __LINE__);
    LOOKUPDEFAULTNSTEST(docBodyLevel23, 0, true, __LINE__);
    LOOKUPDEFAULTNSTEST(docBodyLevel21, 0, true, __LINE__);
    LOOKUPDEFAULTNSTEST(docBodyLevel31, 0, true, __LINE__);
    LOOKUPDEFAULTNSTEST(docBodyLevel32, 0, true, __LINE__);
    LOOKUPDEFAULTNSTEST(docCDATASection, 0, true, __LINE__);
    LOOKUPDEFAULTNSTEST(docFirstElement, 0, true, __LINE__);
    LOOKUPDEFAULTNSTEST(docReferenceEntity, 0, true, __LINE__);
    LOOKUPDEFAULTNSTEST(docFirstElementAttr, 0, true, __LINE__);
    LOOKUPDEFAULTNSTEST(doc, 0, true, __LINE__);
    LOOKUPDEFAULTNSTEST(docNotation, 0, false, __LINE__);
    LOOKUPDEFAULTNSTEST(docComment, 0, true, __LINE__);
    LOOKUPDEFAULTNSTEST(docTextNode2, 0, true, __LINE__);
    LOOKUPDEFAULTNSTEST(docTextNode4, 0, true, __LINE__);

    XMLString::transcode("someSpecialURI", tempStr3, 3999);
    XMLString::transcode("newNameForEle", tempStr4, 3999);
    DOMElement *ele = doc->createElementNS(tempStr3, tempStr4);
    docFirstElement->insertBefore(ele, docFirstElement->getFirstChild());

    // test for bug# 26919
    docFirstElement->insertBefore(docFirstElement->getFirstChild(), docFirstElement->getFirstChild());

    //a test for lookup when xmlns is not set so we take the fact that there is no prefix to be confimation
    LOOKUPDEFAULTNSTEST(ele, tempStr3, true, __LINE__);

    docFirstElement->removeAttributeNode(attr1);
    docBodyLevel21->removeAttributeNode(attr2);
    docFirstElement->removeChild(ele);

//***********Following are for errorTests
    DOMDocumentFragment* docDocFragment = doc->createDocumentFragment();
    UNUSED(docDocFragment); // silence warning

    //name + "docTextNode3"
    XMLString::copyString(tempStr, name);
    XMLString::transcode("docTextNode3", tempStr2, 3999);
    XMLString::catString(tempStr, tempStr2);
    DOMText* docNode3 = doc->createTextNode(tempStr);

    //name + "docTextNode4"
    XMLString::copyString(tempStr, name);
    XMLString::transcode("docTextNode4", tempStr2, 3999);
    XMLString::catString(tempStr, tempStr2);
    DOMText* docNode4 = doc->createTextNode(tempStr);

    //"ourEntityNode"
    XMLString::transcode("ourEntityNode", tempStr, 3999);
    DOMNode*   abc2 =  doc->getDoctype()->getEntities()->getNamedItem(tempStr);  // Get the DOMEntity* node
    DOMEntity* docEntity = (DOMEntity*) abc2;
    UNUSED(docEntity); // silence warning
    DOMNode*   abc3 = doc->getFirstChild(); // Get the DOMDocumentType* node
    DOMDocumentType* docDocType = (DOMDocumentType*) abc3;
    UNUSED(docDocType); // silence warning
    DOMNode*   abc4 = doc->getLastChild()->getLastChild()->getLastChild()->getFirstChild();
    DOMEntityReference* entityReferenceText = (DOMEntityReference*) abc4;
    UNUSED(entityReferenceText); // silence warning

    //"entityReferenceText information"
    XMLString::transcode("entityReferenceText information", tempStr, 3999);
    DOMText* entityReferenceText2 = doc->createTextNode(tempStr);
//************************************************ ERROR TESTS
    DOMTest tests;

    EXCEPTIONSTEST(document->appendChild(docBody), DOMException::HIERARCHY_REQUEST_ERR, OK,  1);

    EXCEPTIONSTEST(document->appendChild(docBody), DOMException::HIERARCHY_REQUEST_ERR, OK, 2);
    EXCEPTIONSTEST(docNode3->appendChild(docNode4), DOMException::HIERARCHY_REQUEST_ERR, OK, 3);
    // EXCEPTIONSTEST(doc->insertBefore(docEntity, docFirstElement), DOMException::HIERARCHY_REQUEST_ERR, OK, 4);
    EXCEPTIONSTEST(doc->replaceChild(docCDATASection, docFirstElement), DOMException::HIERARCHY_REQUEST_ERR, OK, 5);

    //"This shouldn't work!"
    XMLString::transcode("entityReferenceText information", tempStr, 3999);

    // The following setNodeValue tests are not invalid
    // According to DOM spec, if the node value is defined to be null in the DOM spec, setting it has no effect.
    // Only those node type that are supposed to have node value, exception will be raised if the node is readonly.
    // EXCEPTIONSTEST(docFirstElement->setNodeValue(tempStr), DOMException::NO_MODIFICATION_ALLOWED_ERR, OK, 6);
    // EXCEPTIONSTEST(docReferenceEntity->setNodeValue(tempStr), DOMException::NO_MODIFICATION_ALLOWED_ERR, OK, 7);
    // EXCEPTIONSTEST(docEntity->setNodeValue(tempStr), DOMException::NO_MODIFICATION_ALLOWED_ERR, OK, 8);
    // EXCEPTIONSTEST(doc->setNodeValue(tempStr), DOMException::NO_MODIFICATION_ALLOWED_ERR, OK, 9);
    // EXCEPTIONSTEST(docDocType->setNodeValue(tempStr), DOMException::NO_MODIFICATION_ALLOWED_ERR, OK, 10);
    // EXCEPTIONSTEST(docDocFragment->setNodeValue(tempStr), DOMException::NO_MODIFICATION_ALLOWED_ERR, OK, 11);
    // EXCEPTIONSTEST(docNotation->setNodeValue(tempStr), DOMException::NO_MODIFICATION_ALLOWED_ERR, OK, 12);
    EXCEPTIONSTEST(docReferenceEntity->appendChild(entityReferenceText2 ), DOMException::NO_MODIFICATION_ALLOWED_ERR , OK, 13);
    EXCEPTIONSTEST(docBodyLevel32->insertBefore(docTextNode11,docBody ), DOMException::NOT_FOUND_ERR, OK, 14);
    EXCEPTIONSTEST(docBodyLevel32->removeChild(docFirstElement), DOMException::NOT_FOUND_ERR, OK, 15);
    EXCEPTIONSTEST(docBodyLevel32->replaceChild(docTextNode11,docFirstElement ), DOMException::NOT_FOUND_ERR, OK, 16);

    XMLString::release(&name);

    return OK;


//!! Throws a NOT_FOUND_ERR ********

     // docBodyLevel32->getAttributes()->removeNamedItem(testAttribute->getName());    16  // To test removeNamedItem

}  //END OF DOCBUILDER



/**
 * @param document org.w3c.dom.DOMDocument
 */
void DOMTest::findTestNodes(DOMDocument* document) {
    DOMNode* node = document;
    int nodeCount = 0;

    // Walk the tree until you find and assign all node types needed that exist.
    while (node != 0 && nodeCount < 12)
    {

        switch (node->getNodeType())
    {
        case DOMNode::ELEMENT_NODE :
            if (testElementNode == 0) {testElementNode = (DOMElement*)node; nodeCount++;}
            break;
        case DOMNode::ATTRIBUTE_NODE :
            if (testAttributeNode == 0) {testAttributeNode = (DOMAttr*)node; nodeCount++;}
            break;
        case DOMNode::TEXT_NODE :
            if (testTextNode == 0) {testTextNode = (DOMText*)node; nodeCount++;}
            break;
        case DOMNode::CDATA_SECTION_NODE :
            if (testCDATASectionNode == 0) {testCDATASectionNode = (DOMCDATASection*)node; nodeCount++;}
            break;
        case DOMNode::ENTITY_REFERENCE_NODE :
            if (testEntityReferenceNode == 0) {testEntityReferenceNode = (DOMEntityReference*)node; nodeCount++;}
            break;
        case DOMNode::ENTITY_NODE :
            if (testEntityNode == 0) {testEntityNode = (DOMEntity*)node; nodeCount++;}
            break;
        case DOMNode::PROCESSING_INSTRUCTION_NODE :
            if (testProcessingInstructionNode == 0) {testProcessingInstructionNode = (DOMProcessingInstruction*)node; nodeCount++;}
            break;
        case DOMNode::COMMENT_NODE :
            if (testCommentNode == 0) {testCommentNode = (DOMComment*)node; nodeCount++;}
            break;
        case DOMNode::DOCUMENT_TYPE_NODE :
            if (testDocumentTypeNode == 0) {testDocumentTypeNode = (DOMDocumentType*)node; nodeCount++;}
            break;
        case DOMNode::DOCUMENT_FRAGMENT_NODE :
            if (testDocumentFragmentNode == 0) {testDocumentFragmentNode = (DOMDocumentFragment*)node; nodeCount++;}
            break;
        case DOMNode::NOTATION_NODE :
            if (testNotationNode == 0) {testNotationNode = (DOMNotation*)node; nodeCount++;}
            break;
        case DOMNode::DOCUMENT_NODE :
            if (testDocumentNode == 0) {testDocumentNode = (DOMDocument*)node; nodeCount++;}
            break;
        default:
            ;
    }// End of switch

    }   // End of while
}


/**
 * @param document org.w3c.dom.DOMDocument
 */
void DOMTest::findTestNodes(DOMNode* node) {
    DOMTest test;
    DOMNode*  kid;
    // Walk the tree until you find and assign all node types needed that exist.


    if (node->getFirstChild() != 0)
    {
        kid = node->getFirstChild();
        test.findTestNodes(kid);
    }


    if (node->getNextSibling() != 0)
    {
        kid = node->getNextSibling();
        test.findTestNodes(kid);
    }


    switch (node->getNodeType())
    {
        case DOMNode::ELEMENT_NODE :
            if (testElementNode == 0) {testElementNode = (DOMElement*)node; }
            break;
        case DOMNode::ATTRIBUTE_NODE :
            if (testAttributeNode == 0) {testAttributeNode = (DOMAttr*)node; }
            break;
        case DOMNode::TEXT_NODE :
            if (testTextNode == 0) {testTextNode = (DOMText*)node; }
            break;
        case DOMNode::CDATA_SECTION_NODE :
            if (testCDATASectionNode == 0) {testCDATASectionNode = (DOMCDATASection*)node; }
            break;
        case DOMNode::ENTITY_REFERENCE_NODE :
            if (testEntityReferenceNode == 0) {testEntityReferenceNode = (DOMEntityReference*)node;}
            break;
        case DOMNode::ENTITY_NODE :
            if (testEntityNode == 0) {testEntityNode = (DOMEntity*)node;}
            break;
        case DOMNode::PROCESSING_INSTRUCTION_NODE :
            if (testProcessingInstructionNode == 0) {testProcessingInstructionNode = (DOMProcessingInstruction*)node;}
            break;
        case DOMNode::COMMENT_NODE :
            if (testCommentNode == 0) {testCommentNode = (DOMComment*)node;}
            break;
        case DOMNode::DOCUMENT_TYPE_NODE :
            if (testDocumentTypeNode == 0) {testDocumentTypeNode = (DOMDocumentType*)node; }
            break;
        case DOMNode::DOCUMENT_FRAGMENT_NODE :
            if (testDocumentFragmentNode == 0) {testDocumentFragmentNode = (DOMDocumentFragment*)node;}
            break;
        case DOMNode::NOTATION_NODE :
            if (testNotationNode == 0) {testNotationNode = (DOMNotation*)node;}
            break;
        case DOMNode::DOCUMENT_NODE :
            if (testDocumentNode == 0) {testDocumentNode = (DOMDocument*)node;}
            break;
        default:
            ;
    }// End of switch
}//End of class

/**
 *
 *
 */
int main(int /*argc*/, char ** /*argv*/)
 {
     bool OK = true;

     {
         //  Nest entire test in an inner block.
         //     Reference counting should recover all document
         //     storage when this block exits.

         DOMTest test;
         try {
             XMLPlatformUtils::Initialize();
         }
         catch (const XMLException& toCatch) {
             char *pMessage = XMLString::transcode(toCatch.getMessage());
             fprintf(stderr, "Error during initialization! \n  %s \n", pMessage);
             XMLString::release(&pMessage);
             return -1;
         }

         // for (int i=0; i< 1000; i++)
         // {
         // AH Revisit  //  startTime = System.currentTimeMillis();
         //     if(!OK)
         //     break;

         DOMDocument* d = test.createDocument();

         XMLString::transcode("testDocument1", tempStr, 3999);
         DOMDocumentType* docDocType = test.createDocumentType(d,tempStr);
         d->appendChild(docDocType);

         XMLString::transcode("ourEntityNode", tempStr, 3999);
         DOMEntity* docEntity = test.createEntity( d, tempStr);
         UNUSED(docEntity); // silence warning
         //Build a branch for entityReference tests
         // DOMText* entityChildText = d.createTextNode("entityChildText information"); //
         // docEntity->appendChild(entityChildText);
         // docDocType->getEntities()->setNamedItem(docEntity);

         XMLString::transcode("d", tempStr3, 3999);
         OK &= test.docBuilder(d, tempStr3);

         test.findTestNodes((DOMNode*)d);

         OK &= test.testAttr(d);
         OK &= test.testCDATASection(d);
         OK &= test.testCharacterData(d);
         OK &= test.testChildNodeList(d);
         OK &= test.testComment(d);
         OK &= test.testDeepNodeList(d);
         OK &= test.testDocument(d);
         OK &= test.testDocumentFragment(d);
         OK &= test.testDocumentType(d);
         OK &= test.testDOMImplementation(d);
         OK &= test.testElement(d);
//         OK &= test.testEntity(d);  // Can not test entities;  only parser can create them.
         OK &= test.testEntityReference(d);
         OK &= test.testNode(d);
         OK &= test.testNotation(d);
         OK &= test.testPI(d);
         OK &= test.testText(d);
         OK &= test.testDOMerrors(d);
         OK &= test.testXPath(d);
         OK &= test.testRegex();

         // Null out the static object references in class DOMTest,
         // which will recover their storage.
         DOMTest::testElementNode = 0;
         DOMTest::testAttributeNode = 0;
         DOMTest::testTextNode = 0;
         DOMTest::testCDATASectionNode = 0;
         DOMTest::testEntityReferenceNode = 0;
         DOMTest::testEntityNode = 0;
         DOMTest::testProcessingInstructionNode = 0;
         DOMTest::testCommentNode = 0;
         DOMTest::testDocumentNode = 0;
         DOMTest::testDocumentTypeNode = 0;
         DOMTest::testDocumentFragmentNode = 0;
         DOMTest::testNotationNode = 0;

        // test release of attributes
        DOMElement* elt = d->createElement(tempStr3);
        elt->setAttribute(tempStr3, tempStr3);
        elt->release();
        
        elt = d->createElement(tempStr3);
        DOMAttr *attr = d->createAttribute(tempStr3);
        attr->setValue(tempStr3);
        elt->setAttributeNode(attr);
        elt->setIdAttributeNode(attr, true);

        DOMElement *eleTest = d->getElementById(tempStr3);
        if(eleTest==NULL)
        {
            fprintf(stderr, "getElementById test failed at line %i\n", __LINE__);
            OK = false;
        }

        //all good until here
        elt->release();
        eleTest = d->getElementById(tempStr3);
        if(eleTest!=NULL)
        {
            fprintf(stderr, "getElementById test failed at line %i\n", __LINE__);
            OK = false;
        }

        // we couldn't really test the user data handler call as the userhandler is already
        // deleted when the release() is done, but still set it to test the code internally
        d->setUserData(tempStr, (void*) tempStr, &userhandler);
        d->release();

        // Test baseURI. BaseURI is set on nodes at parse time so we
        // cannot use the docBuilder document above

        //Setup parser

        XercesDOMParser *parser = new XercesDOMParser;
        parser->setValidationScheme(XercesDOMParser::Val_Never);
        parser->setDoNamespaces(true);
        parser->setDoSchema(true);
        parser->setCreateEntityReferenceNodes(true);

        OK &= test.testBaseURI(parser);

        parser->setCreateEntityReferenceNodes(false);
        OK &= test.testBaseURI(parser);

        parser->setDoNamespaces(false);
        parser->setDoSchema(false);
        OK &= test.testBaseURI(parser);

        parser->setCreateEntityReferenceNodes(true);
        OK &= test.testBaseURI(parser);

		OK &= test.testWholeText(parser);
        OK &= test.testScanner(parser);
        delete parser;

        OK &= test.testLSExceptions();

        OK &= test.testElementTraversal();

        OK &= test.testUtilFunctions();
    }

    XMLPlatformUtils::Terminate();

    if (!OK) {
        printf("Test Failed\n");
        return 4;
    }

    printf("Test Run Successfully\n");

    return 0;
}


/**
 * This method tests DOMAttr* methods for the XML DOM implementation
 * @param document org.w3c.dom.DOMDocument
 *
 */
bool DOMTest::testAttr(DOMDocument* document)
{
    DOMNode* node;
    DOMAttr* attributeNode;
    bool T = true;
    bool OK = true;
// For debugging*****   printf("\n          testAttr's outputs:\n\n");

    XMLString::transcode("testAttribute", tempStr, 3999);
    DOMAttr* testAttribute = document->createAttribute(tempStr);

    XMLString::transcode("testAttribute's value", tempStr, 3999);
    testAttribute->setValue(tempStr);
    node = document->getDocumentElement(); // node gets first element

    // ((DOMElement*)node)->setAttributeNode(testAttribute);
    // attributeNode = ((DOMElement*)node)->getAttributeNode("testAttribute");
    DOMElement* el = (DOMElement*)node;
    DOMNode* rem = el->setAttributeNode(testAttribute);
    if (rem)
        rem->release();

    XMLString::transcode("testAttribute", tempStr, 3999);
    attributeNode = el->getAttributeNode(tempStr);

    //Test compareDocumentPosition, the equivalent case here
    XMLString::transcode("dFirstElementdFirstElement", tempStr2, 3999);
    DOMAttr* docFirstElementAttr = el->getAttributeNode(tempStr2);

    COMPARETREEPOSITIONTEST(docFirstElementAttr, attributeNode, DOMNode::DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC, __LINE__);

    // Test the name and other data
    if (XMLString::compareString(tempStr, attributeNode->getName()))
    {
        fprintf(stderr, "Warning!!! DOMAttr's 'getName' method failed to work properly!\n");
        OK = false;
    }

    XMLString::transcode("testAttribute's value", tempStr, 3999);
    if (XMLString::compareString(tempStr, attributeNode->getNodeValue()))
    {
        fprintf(stderr, "Warning!!! DOMAttr's 'getNodeValue' method failed to work properly!\n");
        OK = false;
    }
    if (! T ==attributeNode->getSpecified())
    {
        fprintf(stderr, "Warning!!! DOMAttr's 'getSpecified' method failed to work properly!\n");
        OK = false;
    }

    if (XMLString::compareString(tempStr, attributeNode->getValue()))
    {
        fprintf(stderr, "Warning!!! DOMAttr's 'getValue' method failed to work properly!\n");
        OK = false;
    }


    XMLString::transcode("Reset Value", tempStr, 3999);
    attributeNode->setNodeValue(tempStr);   /// LEAK!!!!!
    if (XMLString::compareString(tempStr, attributeNode->getNodeValue()))
    {
        fprintf(stderr, "Warning!!! DOMAttr's 'setNodeValue' method failed to work properly!\n");
        OK = false;
    }

    attributeNode->setValue(XMLUni::fgZeroLenString);
    if (XMLString::compareString(XMLUni::fgZeroLenString, attributeNode->getValue()))
    {
        fprintf(stderr, "Warning!!! DOMAttr's 'setValue' to '0' method failed to work properly!\n");
        OK = false;
    }

    XMLString::transcode("Another value ", tempStr, 3999);
    attributeNode->setValue(tempStr);
    if (XMLString::compareString(tempStr, attributeNode->getValue()))
    {
        fprintf(stderr, "Warning!!! DOMAttr's 'setValue' method failed to work properly!");
        OK = false;
    }

    node = attributeNode->cloneNode(T);

    // Check nodes for equality, both their name and value or lack thereof
    bool cloneOK = true;
    if (XMLString::compareString(node->getNodeName(), attributeNode->getNodeName()))
        cloneOK = false;
    if (node->getNodeValue() == 0 &&
        attributeNode->getNodeValue() != 0)
    {
        cloneOK = false;
    }

    if (node->getNodeValue() != 0 && attributeNode->getNodeValue() == 0)
    {
        cloneOK = false;
    }

    if (node->getNodeValue() != 0 && attributeNode->getNodeValue() != 0)
    {
        if (XMLString::compareString(node->getNodeValue(),attributeNode->getNodeValue()))
            cloneOK = false;
    }


/*
    if (! (node->getNodeName(), attributeNode->getNodeName()) &&         // Compares node names for equality
          (node->getNodeValue() != 0 && attributeNode->getNodeValue() != 0)  // Checks to make sure each node has a value node
        ?  node->getNodeValue(), attributeNode->getNodeValue())          // If both have value nodes test those value nodes for equality
        : (node->getNodeValue() == 0 && attributeNode->getNodeValue() == 0)))// If one node doesn't have a value node make sure both don't
*/
    if (cloneOK == false)
        {
            fprintf(stderr, "'cloneNode' did not clone the Attribute node correctly\n");
            OK = false;
        }
        // Deep clone test comparison is in testNode & testDocument

//************************************************* ERROR TESTS
    DOMTest tests;
//!! Throws HIERARCHY_REQUEST_ERR ****************
    //  doc->getDocumentElement()->appendChild(attributeNode);

//!! Throws a NOT_FOUND_ERR ********
    //  attribute2 = doc->createAttribute("testAttribute2");
    //  doc->getDocumentElement()->removeAttributeNode(attribute2);

//!! Throws an INUSE_ATTRIBUTE_ERR ******
    //  DOMElement* element = (DOMElement*)doc->getLastChild()->getLastChild();
    //  element->setAttributeNode(testAttribute );// Tests setNamedItem which generates error through justSetNamedItem.


    // Test the user data
    // Test simple set and get
    DOMAttr* userTest = testAttribute;
    DOMElement*  userFirst = el;
    XMLCh* userSecond = tempStr2;
    XMLString::transcode("first", tempStr, 3999);

    XMLString::transcode("document", tempStr2, 3999);
    userTest->setUserData(tempStr2, (void*) document, 0);
    void* mydocument = userTest->getUserData(tempStr2);
    if (document != mydocument) {
        fprintf(stderr, "'set/getUserData' in line %i does not work\n", __LINE__);
        OK = false;
    }

    userTest->setUserData(tempStr, (void*) userFirst, 0);
    void* myFirst = userTest->getUserData(tempStr);
    if (userFirst != myFirst) {
        fprintf(stderr, "'set/getUserData' in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test overwrite
    void* myFirst2 = userTest->setUserData(tempStr, (void*) userSecond, 0);
    void* mySecond = userTest->getUserData(tempStr);
    if (userSecond != mySecond) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (userFirst != myFirst2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (userFirst == mySecond) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test null
    // test non-exist key
    XMLString::transcode("not-exist", tempStr3, 3999);
    if (userTest->getUserData(tempStr3)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }

    // use a node that does not have user data set before
    if (userFirst->getUserData(tempStr)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test reset
    void* mySecond2 = userTest->setUserData(tempStr, (void*) 0, 0);
    void* myNull = userTest->getUserData(tempStr);
    if (userSecond != mySecond2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (myNull) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }

    void* mydocument2 = userTest->setUserData(tempStr2, (void*) 0, 0);
    void* myNull2 = userTest->getUserData(tempStr2);
    if (mydocument != mydocument2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (myNull2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }

    //the userTest user data table should be null now
    if (userTest->getUserData(tempStr)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (userTest->getUserData(tempStr2)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test DOMUserDataHandler
    // test clone
    userTest->setUserData(tempStr2, (void*) document, &userhandler);
    DOMNode* mycloned = userTest->cloneNode(true);
    USERDATAHANDLERTEST(userhandler, DOMUserDataHandler::NODE_CLONED, tempStr2, document, userTest, mycloned, __LINE__);

    // test import
    document->setUserData(tempStr2, (void*) document, &userhandler);
    DOMNode* myimport = document->importNode(userTest,true);
    USERDATAHANDLERTEST(userhandler, DOMUserDataHandler::NODE_IMPORTED, tempStr2, document, userTest, myimport, __LINE__);

    // test delete
    myimport->setUserData(tempStr2, (void*) userTest, &userhandler);
    myimport->release();
    USERDATAHANDLERTEST(userhandler, DOMUserDataHandler::NODE_DELETED, tempStr2, userTest, 0, 0, __LINE__);

    // Test isSameNode
    if (!userTest->isSameNode(userTest)) {
        fprintf(stderr, "isSameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (userTest->isSameNode(userFirst)) {
        fprintf(stderr, "isSameNode failed in line %i\n", __LINE__);
        OK = false;
    }

    // Test isEqualNode
    if (!userTest->isEqualNode(mycloned)) {
        fprintf(stderr, "isEqualNode failed in line %i\n", __LINE__);
        OK = false;
    }

    if (!userTest->isEqualNode(userTest)) {
        fprintf(stderr, "isEqualNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (userTest->isEqualNode(userFirst)) {
        fprintf(stderr, "isEqualNode failed in line %i\n", __LINE__);
        OK = false;
    }

    // Test renameNode
    XMLString::transcode("http://nsa", tempStr4, 3999);
    XMLString::transcode("aa", tempStr5, 3999);
    XMLString::transcode("pnsa:aa", tempStr3, 3999);

    // create the attribute
    DOMAttr* renameTestAttribute = document->createAttribute(tempStr5);
    DOMAttr* renameTestAttributeNS = document->createAttributeNS(tempStr4, tempStr3);

    // create the owner element and append the attribute node
    DOMElement* renameTestElement = document->createElement(tempStr5);
    renameTestElement->setAttributeNode(renameTestAttribute);
    renameTestElement->setAttributeNode(renameTestAttributeNS);

    // set up the userdata
    renameTestAttribute->setUserData(tempStr5, (void*) document, &userhandler);
    renameTestAttributeNS->setUserData(tempStr4, (void*) document, 0);

    // set up the node value
    renameTestAttribute->setNodeValue(tempStr5);
    renameTestAttributeNS->setNodeValue(tempStr4);

    XMLString::transcode("http://nsb", tempStr, 3999);
    XMLString::transcode("bb", tempStr2, 3999);
    XMLString::transcode("pnsb:bb", tempStr3, 3999);

    // start the rename tests
    // rename the NS Attribute
    DOMAttr* renameTest = (DOMAttr*) document->renameNode(renameTestAttributeNS, tempStr, tempStr3);
    // test the name
    if (XMLString::compareString(tempStr, renameTest->getNamespaceURI()))
    {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (XMLString::compareString(tempStr2, renameTest->getLocalName()))
    {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (XMLString::compareString(tempStr3, renameTest->getNodeName()))
    {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    // test the child / nodevalue
    if (XMLString::compareString(tempStr4, renameTest->getNodeValue()))
    {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (XMLString::compareString(tempStr4, renameTest->getFirstChild()->getNodeValue()))
    {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    // test the owner element
    if (!renameTestElement->getAttributeNodeNS(tempStr, tempStr2)) {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (renameTestElement->getAttributeNodeNS(tempStr4, tempStr5)) {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    // test user data
    void* renamedocument = renameTest->getUserData(tempStr4);
    if (document != renamedocument) {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    // test isSame and isEqual
    if (!renameTestAttributeNS->isEqualNode(renameTest)) {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (!renameTestAttributeNS->isSameNode(renameTest)) {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }


    // rename the Attribute (null namespace)
    renameTest = (DOMAttr*) document->renameNode(renameTestAttribute, 0, tempStr2);
    // test the name
    if (renameTest->getNamespaceURI())
    {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (renameTest->getLocalName())
    {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (XMLString::compareString(tempStr2, renameTest->getNodeName()))
    {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    // test the child / nodevalue
    if (XMLString::compareString(tempStr5, renameTest->getNodeValue()))
    {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (XMLString::compareString(tempStr5, renameTest->getFirstChild()->getNodeValue()))
    {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    // test the owner element
    if (!renameTestElement->getAttributeNode(tempStr2)) {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (renameTestElement->getAttributeNode(tempStr5)) {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    // test user data
    renamedocument = renameTest->getUserData(tempStr5);
    if (document != renamedocument) {
        fprintf(stderr, "'set/getUserData' in line %i does not work\n", __LINE__);
        OK = false;
    }
    // test isSame and isEqual
    if (!renameTestAttribute->isEqualNode(renameTest)) {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (!renameTestAttribute->isSameNode(renameTest)) {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }


    // rename the Attribute (with namespace)
    renameTest = (DOMAttr*) document->renameNode(renameTestAttribute, tempStr, tempStr3);
    // test the name
    if (XMLString::compareString(tempStr, renameTest->getNamespaceURI()))
    {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (XMLString::compareString(tempStr2, renameTest->getLocalName()))
    {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (XMLString::compareString(tempStr3, renameTest->getNodeName()))
    {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    // test the child / nodevalue
    if (XMLString::compareString(tempStr5, renameTest->getNodeValue()))
    {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (XMLString::compareString(tempStr5, renameTest->getFirstChild()->getNodeValue()))
    {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (renameTestAttribute->getFirstChild())
    {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    // test the owner element
    if (!renameTestElement->getAttributeNodeNS(tempStr, tempStr2)) {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (renameTestElement->getAttributeNodeNS(0, tempStr2)) {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (renameTestElement->getAttributeNode(tempStr2)) {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    // test user data
    renamedocument = renameTest->getUserData(tempStr5);
    if (document != renamedocument) {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    // test userdatahandler
    USERDATAHANDLERTEST(userhandler, DOMUserDataHandler::NODE_RENAMED, tempStr5, document, renameTestAttribute, renameTest, __LINE__);
    // test isSame and isEqual
    // a new node is created here, so both isSame and isEqual are not compared
    if (renameTestAttribute->isEqualNode(renameTest)) {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (renameTestAttribute->isSameNode(renameTest)) {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }



    //isID tests

    XMLString::transcode("http://nsa", tempStr4, 3999);
    XMLString::transcode("aa", tempStr5, 3999);

    DOMAttr *idAtt = document->createAttributeNS(tempStr4, tempStr5);
    testElementNode->setAttributeNode(idAtt);


    if(idAtt->isId()) {
        fprintf(stderr, "isID failed in line %i\n", __LINE__);
        OK = false;
    }

    testElementNode->setIdAttributeNode(idAtt,true);

    if(!idAtt->isId()) {
        fprintf(stderr, "isID failed in line %i\n", __LINE__);
        OK = false;
    }

    testElementNode->setIdAttributeNode(idAtt,false);

    if(idAtt->isId()) {
        fprintf(stderr, "isID failed in line %i\n", __LINE__);
        OK = false;
    }
    //clean up
    testElementNode->removeAttributeNode(idAtt);

    if (! OK)
        printf("\n*****The DOMAttr* method calls listed above failed, all others worked correctly.*****\n");
    return OK;

}




/**
 * This method tests DOMCDATASection* methods for the XML DOM implementation
 * @param document org.w3c.dom.DOMDocument
 *
 */
bool DOMTest::testCDATASection(DOMDocument* document)
{
    DOMNode* node;
    DOMNode* node2;
    bool T = true;
    bool OK = true;
// For debugging*****   printf("\n          testCDATASection's outputs:\n");
    XMLString::transcode("dBodyLevel23", tempStr, 3999);
    node = document->getDocumentElement()->getElementsByTagName(tempStr)->item(0)->getFirstChild(); // node gets DOMCDATASection* node

    node2 = node->cloneNode(T);//*****?

    // Check nodes for equality, both their name and value or lack thereof
    if (!(!XMLString::compareString(node->getNodeName(), node2->getNodeName()) &&        // Compares node names for equality
          (node->getNodeValue() != 0 && node2->getNodeValue() != 0)     // Checks to make sure each node has a value node
        ? !XMLString::compareString(node->getNodeValue(), node2->getNodeValue())         // If both have value nodes test those value nodes for equality
        : (node->getNodeValue() == 0 && node2->getNodeValue() == 0)))   // If one node doesn't have a value node make sure both don't
    {
        fprintf(stderr, "'cloneNode' did not clone the DOMCDATASection* node correctly\n");
        OK = false;
    }
    // Deep clone test comparison is in testNode & testDocument


    // Test the user data
    // Test simple set and get
    DOMCDATASection* userTest = (DOMCDATASection*) node;
    DOMCDATASection*  userFirst = (DOMCDATASection*) node2;
    XMLCh* userSecond = tempStr2;
    XMLString::transcode("first", tempStr, 3999);

    XMLString::transcode("document", tempStr2, 3999);
    userTest->setUserData(tempStr2, (void*) document, 0);
    void* mydocument = userTest->getUserData(tempStr2);
    if (document != mydocument) {
        fprintf(stderr, "'set/getUserData' in line %i does not work\n", __LINE__);
        OK = false;
    }

    userTest->setUserData(tempStr, (void*) userFirst, 0);
    void* myFirst = userTest->getUserData(tempStr);
    if (userFirst != myFirst) {
        fprintf(stderr, "'set/getUserData' in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test overwrite
    void* myFirst2 = userTest->setUserData(tempStr, (void*) userSecond, 0);
    void* mySecond = userTest->getUserData(tempStr);
    if (userSecond != mySecond) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (userFirst != myFirst2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (userFirst == mySecond) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test null
    // test non-exist key
    XMLString::transcode("not-exist", tempStr3, 3999);
    if (userTest->getUserData(tempStr3)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }

    // use a node that does not have user data set before
    if (userFirst->getUserData(tempStr)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test reset
    void* mySecond2 = userTest->setUserData(tempStr, (void*) 0, 0);
    void* myNull = userTest->getUserData(tempStr);
    if (userSecond != mySecond2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (myNull) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }

    void* mydocument2 = userTest->setUserData(tempStr2, (void*) 0, 0);
    void* myNull2 = userTest->getUserData(tempStr2);
    if (mydocument != mydocument2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (myNull2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }

    //the userTest user data table should be null now
    if (userTest->getUserData(tempStr)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (userTest->getUserData(tempStr2)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test DOMUserDataHandler
    // test clone
    userTest->setUserData(tempStr2, (void*) document, &userhandler);
    DOMNode* mycloned = userTest->cloneNode(true);
    USERDATAHANDLERTEST(userhandler, DOMUserDataHandler::NODE_CLONED, tempStr2, document, userTest, mycloned, __LINE__);

    // test import
    document->setUserData(tempStr2, (void*) document, &userhandler);
    DOMNode* myimport = document->importNode(userTest,true);
    USERDATAHANDLERTEST(userhandler, DOMUserDataHandler::NODE_IMPORTED, tempStr2, document, userTest, myimport, __LINE__);

    // test delete
    myimport->setUserData(tempStr2, (void*) userTest, &userhandler);
    myimport->release();
    USERDATAHANDLERTEST(userhandler, DOMUserDataHandler::NODE_DELETED, tempStr2, userTest, 0, 0, __LINE__);

    // Test isSameNode
    if (!userTest->isSameNode(userTest)) {
        fprintf(stderr, "isSameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (userTest->isSameNode(userFirst)) {
        fprintf(stderr, "isSameNode failed in line %i\n", __LINE__);
        OK = false;
    }

    // Test isEqualNode
    if (!userTest->isEqualNode(mycloned)) {
        fprintf(stderr, "isEqualNode failed in line %i\n", __LINE__);
        OK = false;
    }

    if (!userTest->isEqualNode(userTest)) {
        fprintf(stderr, "isEqualNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (userTest->isEqualNode(document)) {
        fprintf(stderr, "isEqualNode failed in line %i\n", __LINE__);
        OK = false;
    }

    // Test rename, should throw exception
    EXCEPTIONSTEST(document->renameNode(userTest, 0, tempStr2), DOMException::NOT_SUPPORTED_ERR, OK, 204 );

    if (! OK)
        printf("\n*****The DOMCDATASection* method calls listed above failed, all others worked correctly.*****\n");
    return OK;
}



/**
 * This method tests DOMCharacterData methods for the XML DOM implementation
 * @param document org.w3c.dom.DOMDocument
 *
 */
bool DOMTest::testCharacterData(DOMDocument* document)
{
    DOMCharacterData* charData;
    XMLCh resetData[3999];
    bool OK = true;
// For debugging*****   printf("\n          testCharacterData's outputs:\n");
    XMLString::transcode("dBodyLevel31", tempStr, 3999);
    DOMNode*  abc1 = document->getDocumentElement()->getElementsByTagName(tempStr)->item(0)->getFirstChild(); // charData gets textNode11

    charData = (DOMCharacterData *) abc1;

    XMLString::transcode("dBodyLevel31'sChildTextNode11", tempStr, 3999);
    if (XMLString::compareString(tempStr, charData->getData()))
    {
        fprintf(stderr, "Warning!!! DOMCharacterData's 'getData' failed to work properly!\n This may corrupt other DOMCharacterData tests!!!*****\n");
        OK = false;
    }

    XMLString::copyString(resetData,charData->getData());
    //  printf("This node's original data is: " + charData->getData());

    XMLString::transcode(" This is new data for this node", tempStr2, 3999);

    XMLString::copyString(tempStr, charData->getData());
    XMLString::catString(tempStr, tempStr2);

    charData->appendData(tempStr2);


    if (XMLString::compareString(tempStr, charData->getData()))
    {
        fprintf(stderr, "Warning!!! DOMCharacterData's 'appendData' failed to work properly!\n");
        OK = false;
    }
    //  printf("This node's appended data is: " + charData->getData());

    XMLString::transcode("dBodyLevel", tempStr, 3999);
    charData->deleteData(10, 100);
    if (XMLString::compareString(tempStr, charData->getData()))
    {
        fprintf(stderr, "Warning!!! DOMCharacterData's 'deleteData' failed to work properly!\n");
        OK = false;
    }
    //  printf("This node's partially deleted data is: " + charData->getData());

    XMLSize_t length = 10;
    if (!(length == charData->getLength()))
    {
        fprintf(stderr, "Warning!!! DOMCharacterData's 'getLength' failed to work properly!\n");
        OK = false;
    }
    //  printf("This node's data length is: " + charData->getLength());

    XMLString::transcode("dBody' This is data inserted into this node'Level", tempStr, 3999);
    XMLString::transcode("' This is data inserted into this node'", tempStr2, 3999);
    charData->insertData(5, tempStr2);
    if (XMLString::compareString(tempStr, charData->getData()))
    {
        fprintf(stderr, "Warning!!! DOMCharacterData's 'insertData' failed to work properly!\n");
        OK = false;
    }
    //  printf("This node's updated with insert data is: " + charData->getData());

    XMLString::transcode("dBody' This is ' replacement data'ted into this node'Level", tempStr, 3999);
    XMLString::transcode("' replacement data'", tempStr2, 3999);
    charData->replaceData(15, 10, tempStr2);
    if (XMLString::compareString(tempStr, charData->getData()))
    {
        fprintf(stderr, "Warning!!! DOMCharacterData's 'replaceData' failed to work properly!\n");
        OK = false;
    }
    //  printf("This node's updated with replacement data is: " +charData->getData());

    XMLString::transcode("New data A123456789B123456789C123456789D123456789E123456789", tempStr, 3999);
    charData->setData(tempStr);
    if (XMLString::compareString(tempStr, charData->getData()))
    {
        fprintf(stderr, "Warning!!! DOMCharacterData's 'setData' failed to work properly!");
        OK = false;
    }
    //  printf("This node's new data via setData: " + charData->getData());

    XMLString::transcode("123456789D123456789E123456789", tempStr, 3999);
    if (XMLString::compareString(tempStr, charData->substringData(30, 30)))
    {
        fprintf(stderr, "Warning!!! DOMCharacterData's 'substringData' failed to work properly!\n");
        OK = false;
    }
    //  printf("Using subString 30,30 you get:");  charData->substringData(30,30)).print();

    XMLString::transcode("New data A123456789B12345", tempStr, 3999);
    if (XMLString::compareString(tempStr, charData->substringData(0, 25)))
    {
        fprintf(stderr, "Warning!!! DOMCharacterData's 'substringData' failed to work properly!\n");
        OK = false;
    }
    //  printf("Using subString 0,25 you get: ");   charData->substringData(0,25)).print();

//************************************************* ERROR TESTS
    DOMTest tests;   // What is this for?  'tests' is never used.

//!! Throws INDEX_SIZE_ERR ********************
    EXCEPTIONSTEST(charData->deleteData((XMLSize_t)-1, 5), DOMException::INDEX_SIZE_ERR, OK, 101 );
    // Test 102 is not an error because the -1 parameter is an unsigned value, and counts
    //   that exceed the length of the string are allowed.
//    EXCEPTIONSTEST(charData->deleteData(2, -1), DOMException::INDEX_SIZE_ERR, OK, 102 );
    EXCEPTIONSTEST(charData->deleteData(100, 5), DOMException::INDEX_SIZE_ERR, OK,103 );

//can't set negative XMLSize_t in c++ compiler

  //  EXCEPTIONSTEST(charData->insertData(-1, "Stuff inserted"), DOMException::INDEX_SIZE_ERR, OK, 104 );
    XMLString::transcode("Stuff inserted", tempStr, 3999);
    EXCEPTIONSTEST(charData->insertData(100, tempStr), DOMException::INDEX_SIZE_ERR, OK, 105 );

  //  EXCEPTIONSTEST(charData->replaceData(-1, 5, "Replacement stuff") , DOMException::INDEX_SIZE_ERR, OK, 106 );
    XMLString::transcode("Replacement stuff", tempStr, 3999);
    EXCEPTIONSTEST(charData->replaceData(100, 5 ,tempStr), DOMException::INDEX_SIZE_ERR, OK, 107 );
  //  EXCEPTIONSTEST(charData->replaceData(2, -1, "Replacement stuff"), DOMException::INDEX_SIZE_ERR,  OK, 108 );

    EXCEPTIONSTEST(charData->substringData((XMLSize_t)-1, 5), DOMException::INDEX_SIZE_ERR, OK, 109 );
    EXCEPTIONSTEST(charData->substringData(100, 5), DOMException::INDEX_SIZE_ERR, OK, 110 );
 //   EXCEPTIONSTEST(charData->substringData(2, -1), DOMException::INDEX_SIZE_ERR, OK, 111 );


    // Test the user data
    // Test simple set and get
    DOMCharacterData* userTest = charData;
    DOMDocument*  userFirst = document;
    XMLCh* userSecond = tempStr2;
    XMLString::transcode("first", tempStr, 3999);

    XMLString::transcode("document", tempStr2, 3999);
    userTest->setUserData(tempStr2, (void*) document, 0);
    void* mydocument = userTest->getUserData(tempStr2);
    if (document != mydocument) {
        fprintf(stderr, "'set/getUserData' in line %i does not work\n", __LINE__);
        OK = false;
    }

    userTest->setUserData(tempStr, (void*) userFirst, 0);
    void* myFirst = userTest->getUserData(tempStr);
    if (userFirst != myFirst) {
        fprintf(stderr, "'set/getUserData' in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test overwrite
    void* myFirst2 = userTest->setUserData(tempStr, (void*) userSecond, 0);
    void* mySecond = userTest->getUserData(tempStr);
    if (userSecond != mySecond) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (userFirst != myFirst2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (userFirst == mySecond) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test null
    // test non-exist key
    XMLString::transcode("not-exist", tempStr3, 3999);
    if (userTest->getUserData(tempStr3)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }

    // use a node that does not have user data set before
    if (userFirst->getUserData(tempStr)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test reset
    void* mySecond2 = userTest->setUserData(tempStr, (void*) 0, 0);
    void* myNull = userTest->getUserData(tempStr);
    if (userSecond != mySecond2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (myNull) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }

    void* mydocument2 = userTest->setUserData(tempStr2, (void*) 0, 0);
    void* myNull2 = userTest->getUserData(tempStr2);
    if (mydocument != mydocument2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (myNull2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }

    //the userTest user data table should be null now
    if (userTest->getUserData(tempStr)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (userTest->getUserData(tempStr2)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test DOMUserDataHandler
    // test clone
    userTest->setUserData(tempStr2, (void*) document, &userhandler);
    DOMNode* mycloned = userTest->cloneNode(true);
    USERDATAHANDLERTEST(userhandler, DOMUserDataHandler::NODE_CLONED, tempStr2, document, userTest, mycloned, __LINE__);

    // test import
    document->setUserData(tempStr2, (void*) document, &userhandler);
    DOMNode* myimport = document->importNode(userTest,true);
    USERDATAHANDLERTEST(userhandler, DOMUserDataHandler::NODE_IMPORTED, tempStr2, document, userTest, myimport, __LINE__);

    // test delete
    myimport->setUserData(tempStr2, (void*) userTest, &userhandler);
    myimport->release();
    USERDATAHANDLERTEST(userhandler, DOMUserDataHandler::NODE_DELETED, tempStr2, userTest, 0, 0, __LINE__);

    // Test isSameNode
    if (!userTest->isSameNode(userTest)) {
        fprintf(stderr, "isSameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (userTest->isSameNode(userFirst)) {
        fprintf(stderr, "isSameNode failed in line %i\n", __LINE__);
        OK = false;
    }

    // Test isEqualNode
    if (!userTest->isEqualNode(mycloned)) {
        fprintf(stderr, "isEqualNode failed in line %i\n", __LINE__);
        OK = false;
    }

    if (!userTest->isEqualNode(userTest)) {
        fprintf(stderr, "isEqualNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (userTest->isEqualNode(userFirst)) {
        fprintf(stderr, "isEqualNode failed in line %i\n", __LINE__);
        OK = false;
    }

    // Test rename, should throw exception
    EXCEPTIONSTEST(document->renameNode(userTest, 0, tempStr2), DOMException::NOT_SUPPORTED_ERR, OK, 205 );

    if (!OK)
        printf("\n*****The DOMCharacterData method calls listed above failed, all others worked correctly.*****\n");
    charData->setData(resetData); // reset node to original data
    return OK;
}




/**
 * This method tests ChildNodeList methods for the XML DOM implementation
 * @param document org.w3c.dom.DOMDocument
 *
 */
bool DOMTest::testChildNodeList(DOMDocument* document)
{
    DOMNode* node;
    DOMNode* node2;
    bool OK = true;
// For debugging*****   printf("\n          testChildNodeList's outputs:\n");
    node = document->getDocumentElement()->getLastChild(); // node gets doc's testBody element

    if (!(node->getChildNodes()->getLength()== 4))
        OK = false;
    node2 = node->getChildNodes()->item(2);
    XMLString::transcode("dBodyLevel23", tempStr, 3999);
    if (XMLString::compareString(tempStr, node2->getNodeName()))
        OK = false;

    if (!OK)
        printf("\n*****The ChildNodeList method calls listed above failed, all others worked correctly.*****\n");
    return OK;
}



/**
 * This method tests DOMComment* methods for the XML DOM implementation
 * @param document org.w3c.dom.DOMDocument
 *
 */
bool DOMTest::testComment(DOMDocument* document)
{
    DOMNode* node;
    DOMNode* node2;
    bool T = true;
    bool OK = true;
// For debugging*****   printf("\n          testComment's outputs:\n");
    XMLString::transcode("dBodyLevel31", tempStr, 3999);
    node = document->getDocumentElement()->getElementsByTagName(tempStr)->item(0)->getFirstChild(); // node gets textNode11
    node2 = node->cloneNode(T);
    // Check nodes for equality, both their name and value or lack thereof
    if (!(!XMLString::compareString(node->getNodeName(), node2->getNodeName()) &&        // Compares node names for equality
          (node->getNodeValue() != 0 && node2->getNodeValue() != 0)     // Checks to make sure each node has a value node
        ? !XMLString::compareString(node->getNodeValue(), node2->getNodeValue())         // If both have value nodes test those value nodes for equality
        : (node->getNodeValue() == 0 && node2->getNodeValue() == 0)))   // If one node doesn't have a value node make sure both don't
    {
        fprintf(stderr, "'cloneNode' did not clone the DOMComment* node correctly\n");
        OK = false;
    }
    // Deep clone test comparison is in testNode & testDocument

    // Test the user data
    // Test simple set and get
    DOMComment* userTest = (DOMComment*) node;
    DOMComment*  userFirst = (DOMComment*) node2;
    XMLCh* userSecond = tempStr2;
    XMLString::transcode("first", tempStr, 3999);

    XMLString::transcode("document", tempStr2, 3999);
    userTest->setUserData(tempStr2, (void*) document, 0);
    void* mydocument = userTest->getUserData(tempStr2);
    if (document != mydocument) {
        fprintf(stderr, "'set/getUserData' in line %i does not work\n", __LINE__);
        OK = false;
    }

    userTest->setUserData(tempStr, (void*) userFirst, 0);
    void* myFirst = userTest->getUserData(tempStr);
    if (userFirst != myFirst) {
        fprintf(stderr, "'set/getUserData' in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test overwrite
    void* myFirst2 = userTest->setUserData(tempStr, (void*) userSecond, 0);
    void* mySecond = userTest->getUserData(tempStr);
    if (userSecond != mySecond) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (userFirst != myFirst2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (userFirst == mySecond) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test null
    // test non-exist key
    XMLString::transcode("not-exist", tempStr3, 3999);
    if (userTest->getUserData(tempStr3)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }

    // use a node that does not have user data set before
    if (userFirst->getUserData(tempStr)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test reset
    void* mySecond2 = userTest->setUserData(tempStr, (void*) 0, 0);
    void* myNull = userTest->getUserData(tempStr);
    if (userSecond != mySecond2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (myNull) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }

    void* mydocument2 = userTest->setUserData(tempStr2, (void*) 0, 0);
    void* myNull2 = userTest->getUserData(tempStr2);
    if (mydocument != mydocument2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (myNull2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }

    //the userTest user data table should be null now
    if (userTest->getUserData(tempStr)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (userTest->getUserData(tempStr2)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test DOMUserDataHandler
    // test clone
    userTest->setUserData(tempStr2, (void*) document, &userhandler);
    DOMNode* mycloned = userTest->cloneNode(true);
    USERDATAHANDLERTEST(userhandler, DOMUserDataHandler::NODE_CLONED, tempStr2, document, userTest, mycloned, __LINE__);

    // test import
    document->setUserData(tempStr2, (void*) document, &userhandler);
    DOMNode* myimport = document->importNode(userTest,true);
    USERDATAHANDLERTEST(userhandler, DOMUserDataHandler::NODE_IMPORTED, tempStr2, document, userTest, myimport, __LINE__);

    // test delete
    myimport->setUserData(tempStr2, (void*) userTest, &userhandler);
    myimport->release();
    USERDATAHANDLERTEST(userhandler, DOMUserDataHandler::NODE_DELETED, tempStr2, userTest, 0, 0, __LINE__);

    // Test isSameNode
    if (!userTest->isSameNode(userTest)) {
        fprintf(stderr, "isSameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (userTest->isSameNode(userFirst)) {
        fprintf(stderr, "isSameNode failed in line %i\n", __LINE__);
        OK = false;
    }

    // Test isEqualNode
    if (!userTest->isEqualNode(mycloned)) {
        fprintf(stderr, "isEqualNode failed in line %i\n", __LINE__);
        OK = false;
    }

    if (!userTest->isEqualNode(userTest)) {
        fprintf(stderr, "isEqualNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (userTest->isEqualNode(document)) {
        fprintf(stderr, "isEqualNode failed in line %i\n", __LINE__);
        OK = false;
    }

    // Test rename, should throw exception
    EXCEPTIONSTEST(document->renameNode(userTest, 0, tempStr2), DOMException::NOT_SUPPORTED_ERR, OK, 206 );

    if (!OK)
        printf("\n*****The DOMComment* method calls listed above failed, all others worked correctly.*****\n");
    return OK;
}



/**
 * This method tests DeepNodeList methods for the XML DOM implementation
 * @param document org.w3c.dom.DOMDocument
 *
 */
bool DOMTest::testDeepNodeList(DOMDocument* document)
{
    DOMNode* node;
    DOMNode* node2;
    bool OK = true;
// For debugging*****   printf("\n          testDeepNodeList's outputs:\n\n");
    node = document->getLastChild()->getLastChild(); // node gets docBody element
//  DOMElement* el = (DOMElement*)node;
//  DOMNodeList nl = el->getElementsByTagName("*");
//  int len = nl->getLength();
//  if (len != 8)
    XMLString::transcode("*",tempStr, 3999);
    if (!(8 == ((DOMElement*) node)->getElementsByTagName(tempStr)->getLength()))
        {
            printf ("Warning!!! DeepNodeList's 'getLength' failed to work properly!\n");
            OK = false;
        }
    node2 = ((DOMElement*) node)->getElementsByTagName(tempStr)->item(2); //This also runs through 'nextMatchingElementAfter"

    XMLString::transcode("dBodyLevel32", tempStr, 3999);
    if (XMLString::compareString(tempStr, node2->getNodeName()))
        {
            printf ("Warning!!! DeepNodeList's 'item' (or DOMElement's 'getElementsBy TagName)failed to work properly!\n");
            OK = false;
        }
    node2 = document->getLastChild();
    XMLString::transcode("dTestBody", tempStr, 3999);
    if (XMLString::compareString(tempStr, ((DOMElement*) node2)->getElementsByTagName(tempStr)->item(0)->getNodeName()))//This also runs through 'nextMatchingElementAfter"
        {
            printf ("Warning!!! DeepNodeList's 'item' (or DOMElement's 'getElementsBy TagName)failed to work properly!\n");
            OK = false;
        }

    if (!OK)
        printf("\n*****The DeepNodeList method calls listed above failed, all others worked correctly.*****\n");
    return OK;
}



/**
 * This method tests DOMDocument* methods for the XML DOM implementation
 * @param document org.w3c.dom.DOMDocument
 *
 *
 **** ALL DOMDocument* create methods are run in docBuilder except createAttribute which is in testAttribute**
 */
bool DOMTest::testDocument(DOMDocument* document)
{
    DOMTest make;
    DOMDocumentFragment* docFragment, *docFragment2;
    DOMElement* newElement;
    DOMNode* node, *node2;

    const char* elementNames[] =  {"dFirstElement", "dTestBody", "dBodyLevel21","dBodyLevel31","dBodyLevel32",
                   "dBodyLevel22","dBodyLevel33","dBodyLevel34","dBodyLevel23","dBodyLevel24"};
    const char* newElementNames[] = {"dFirstElement", "dTestBody", "dBodyLevel22","dBodyLevel33","dBodyLevel34","dBodyLevel23"};


    bool result;
    bool OK = true;
// For debugging*****   printf("\n          testDocument's outputs:\n \n");

    XMLString::transcode("testDocument1", tempStr, 3999);
    DOMDocumentType* checkDocType =  make.createDocumentType(document,tempStr);
    DOMDocumentType* docType = document->getDoctype();

    if (XMLString::compareString(checkDocType->getNodeName(),docType->getNodeName() ))
    {
        fprintf(stderr, "Warning!!! DOMDocument's 'getDocType method failed!\n" );
        OK = false;
    }

    if (XMLString::compareString(checkDocType->getNodeValue(), docType->getNodeValue()))
    {
        fprintf(stderr, "Warning!!! DOMDocument's 'getDocType method failed!\n" );
        OK = false;
    }

    /*
    if (! (checkDocType->getNodeName(), docType->getNodeName()) &&      // Compares node names for equality
          (checkDocType->getNodeValue() != 0 && docType->getNodeValue() != 0)   // Checks to make sure each node has a value node
        ?  checkDocType->getNodeValue(), docType->getNodeValue())       // If both have value nodes test those value nodes for equality
        : (checkDocType->getNodeValue() == 0 && docType->getNodeValue() == 0))) // If one node doesn't have a value node make sure both don't
    {
        fprintf(stderr, "Warning!!! DOMDocument's 'getDocType method failed!\n" );
        OK = false;
    }
    */

    DOMNode*  rootElement = document->getLastChild();

    bool check = (rootElement->getNodeValue() && document->getDocumentElement()->getNodeValue())   // Checks to make sure each node has a value node
        ?  !XMLString::compareString(rootElement->getNodeValue(), document->getDocumentElement()->getNodeValue())      // If both have value nodes test those value nodes for equality
        : (rootElement->getNodeValue() == 0 && document->getDocumentElement()->getNodeValue() == 0);    // If one node doesn't have a value node make sure both don't
    if (!(!XMLString::compareString(rootElement->getNodeName(), document->getDocumentElement()->getNodeName()) &&        // Compares node names for equality
         check))
    {
        fprintf(stderr, "Warning!!! DOMDocument's 'getDocumentElement' method failed!\n" );
        OK = false;
    }

    XMLString::transcode("*", tempStr, 3999);
    DOMNodeList* docElements = document->getElementsByTagName(tempStr);
    XMLSize_t docSize = docElements->getLength();
    XMLSize_t i;
    for (i = 0; i < docSize; i++)
    {
        DOMNode*  n = (DOMNode*) docElements->item(i);
        XMLString::transcode(elementNames[i], tempStr, 3999);
        if (XMLString::compareString(tempStr, n->getNodeName()))
        {
            fprintf(stderr, "Comparison of this document's elements failed at element number %llu at line %i \n",
                    (unsigned long long) i, __LINE__);
            OK = false;
            break;
        }
    }

    // What is this supposed to be doing?
    //
    //if (document->equals(document->getImplementation()))
    //{
    //  fprintf(stderr, "Warning!!! DOMDocument's 'getImplementation' method failed!\n" );
    //  OK = false;
    //}

    XMLString::transcode("NewElementTestsInsertBefore", tempStr, 3999);
    newElement = document->createElement(tempStr);
    //  doc->insertBefore(newElement,0);//!! Throws a HIERARCHY_REQUEST_ERR   *******
    //  doc->removeChild(docElements->item(9));//!! Throws a NOT_FOUND_ERR  ********

    docFragment = document->createDocumentFragment();
    //Tests removeChild and stores removed branch for tree reconstruction
    docFragment->appendChild(docElements->item(1)->removeChild(docElements->item(9)));
    docFragment2 = document->createDocumentFragment();
    //Tests removeChild and stores removed branch for tree reconstruction
    docFragment2->appendChild(docElements->item(1)->removeChild(docElements->item(2)));
    docSize = docElements->getLength();
    for (i = 0; i < docSize; i++)
    {
        DOMNode*  n = (DOMNode*) docElements->item(i);
        XMLString::transcode(newElementNames[i], tempStr, 3999);
        if (XMLString::compareString(tempStr, n->getNodeName()))
        {
            fprintf(stderr, "Comparison of new document's elements failed at element number %llu at line %i \n",
                    (unsigned long long) i, __LINE__);
            OK = false;
            break;
        }
    }
    docElements->item(1)->insertBefore(docFragment, 0); //Reattaches removed branch to restore tree to the original
                                                // AH Revist.  Note: insertBefore should be able to take
                                                //   a 0 as its second parameter.
    docElements->item(1)->insertBefore(docFragment2, docElements->item(2)); //Reattaches removed branch to restore tree to the original

    //  printf(docElements->item(2)->getNodeName());

    docSize = docElements->getLength();
    for (i = 0; i < docSize; i++)
    {
        DOMNode*  n = (DOMNode*) docElements->item(i);
        XMLString::transcode(elementNames[i], tempStr, 3999);
        if (XMLString::compareString(tempStr, n->getNodeName()))
        {
            fprintf(stderr, "Comparison of restored document's elements failed at element number %llu at line %i \n",
                    (unsigned long long) i, __LINE__);
            OK = false;
            break;
        }
    }

    DOMTest tests;


//  DOMDocument* z = tests.createDocument();
//  tests.docBuilder(z, "z");

//!! Throws WRONG_DOCUMENT_ERR **********
//  EXCEPTIONSTEST(z->appendChild(
    //  z->appendChild(d.createComment("Test doc d comment"));// Tries to append z document with document d comment
    //  d->getDocumentElement()->appendChild(z.createElement("newZdocElement"));// Tries to append d document with document z DOMElement
    //  d->getLastChild()->getLastChild()->insertBefore(z.createElement("newZdocElement"),d->getLastChild()->getLastChild()->getFirstChild());// Tries to insert into d document with document z DOMElement
    //  d->replaceChild(z.createElement("newZdocElement"),d->getLastChild()->getLastChild()->getFirstChild());  // Tries to replace in d document with document z DOMElement

    //  doc->setNodeValue("This shouldn't work");//!! Throws a NO_MODIFICATION_ALLOWED_ERR ********

    node = document;
    node2 = document->cloneNode(true);
    result = treeCompare(node, node2); // Deep clone test comparison of document cloneNode
    if (!result)
    {
        fprintf(stderr, "Warning!!! Deep clone of the document failed!\n");
        OK = false;
    }
    node2->release();

    // Deep clone test comparison is also in testNode

    // Test the user data
    // Test simple set and get
    DOMDocumentFragment* userTest = docFragment;
    DOMDocumentFragment*  userFirst = docFragment2;
    XMLCh* userSecond = tempStr2;
    XMLString::transcode("first", tempStr, 3999);

    XMLString::transcode("document", tempStr2, 3999);
    userTest->setUserData(tempStr2, (void*) document, 0);
    void* mydocument = userTest->getUserData(tempStr2);
    if (document != mydocument) {
        fprintf(stderr, "'set/getUserData' in line %i does not work\n", __LINE__);
        OK = false;
    }

    userTest->setUserData(tempStr, (void*) userFirst, 0);
    void* myFirst = userTest->getUserData(tempStr);
    if (userFirst != myFirst) {
        fprintf(stderr, "'set/getUserData' in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test overwrite
    void* myFirst2 = userTest->setUserData(tempStr, (void*) userSecond, 0);
    void* mySecond = userTest->getUserData(tempStr);
    if (userSecond != mySecond) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (userFirst != myFirst2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (userFirst == mySecond) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test null
    // test non-exist key
    XMLString::transcode("not-exist", tempStr3, 3999);
    if (userTest->getUserData(tempStr3)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }

    // use a node that does not have user data set before
    if (userFirst->getUserData(tempStr)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test reset
    void* mySecond2 = userTest->setUserData(tempStr, (void*) 0, 0);
    void* myNull = userTest->getUserData(tempStr);
    if (userSecond != mySecond2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (myNull) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }

    void* mydocument2 = userTest->setUserData(tempStr2, (void*) 0, 0);
    void* myNull2 = userTest->getUserData(tempStr2);
    if (mydocument != mydocument2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (myNull2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }

    //the userTest user data table should be null now
    if (userTest->getUserData(tempStr)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (userTest->getUserData(tempStr2)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test DOMUserDataHandler
    // test clone
    userTest->setUserData(tempStr2, (void*) document, &userhandler);
    DOMNode* mycloned = userTest->cloneNode(true);
    USERDATAHANDLERTEST(userhandler, DOMUserDataHandler::NODE_CLONED, tempStr2, document, userTest, mycloned, __LINE__);

    // test import
    document->setUserData(tempStr2, (void*) document, &userhandler);
    DOMNode* myimport = document->importNode(userTest,true);
    USERDATAHANDLERTEST(userhandler, DOMUserDataHandler::NODE_IMPORTED, tempStr2, document, userTest, myimport, __LINE__);

    // test delete
    myimport->setUserData(tempStr2, (void*) userTest, &userhandler);
    myimport->release();
    USERDATAHANDLERTEST(userhandler, DOMUserDataHandler::NODE_DELETED, tempStr2, userTest, 0, 0, __LINE__);

    // Test isSameNode
    if (!userTest->isSameNode(userTest)) {
        fprintf(stderr, "isSameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (userTest->isSameNode(userFirst)) {
        fprintf(stderr, "isSameNode failed in line %i\n", __LINE__);
        OK = false;
    }

    // Test isEqualNode
    if (!userTest->isEqualNode(mycloned)) {
        fprintf(stderr, "isEqualNode failed in line %i\n", __LINE__);
        OK = false;
    }

    if (!userTest->isEqualNode(userTest)) {
        fprintf(stderr, "isEqualNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (userTest->isEqualNode(document)) {
        fprintf(stderr, "isEqualNode failed in line %i\n", __LINE__);
        OK = false;
    }

    // Test rename, should throw exception
    EXCEPTIONSTEST(document->renameNode(userTest, 0, tempStr2), DOMException::NOT_SUPPORTED_ERR, OK, 207 );

    if (!OK)
        printf("\n*****The DOMDocument* method calls listed above failed, all others worked correctly.*****\n");
    return OK;
}



/**
 * This method tests DOMDocumentFragment* methods for the XML DOM implementation
 * @param document org.w3c.dom.DOMDocument
 *
 *
 *
 ********This really isn't needed, only exists to throw NO_MODIFICATION_ALLOWED_ERR ********
 */
bool DOMTest::testDocumentFragment(DOMDocument* document)
{
    bool OK = true;
// For debugging*****   printf("\n          testDocumentFragment's outputs:\n");
    DOMDocumentFragment* testDocFragment = document->createDocumentFragment();

    //  testDocFragment->setNodeValue("This is a document fragment!");//!! Throws a NO_MODIFICATION_ALLOWED_ERR ********

    // Test isSameNode
    DOMDocumentFragment* userTest = testDocFragment;
    DOMNode* mycloned = testDocFragment->cloneNode(true);
    DOMDocument* userFirst = document;

    if (!userTest->isSameNode(userTest)) {
        fprintf(stderr, "isSameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (userTest->isSameNode(userFirst)) {
        fprintf(stderr, "isSameNode failed in line %i\n", __LINE__);
        OK = false;
    }

    // Test isEqualNode
    if (!userTest->isEqualNode(mycloned)) {
        fprintf(stderr, "isEqualNode failed in line %i\n", __LINE__);
        OK = false;
    }

    if (!userTest->isEqualNode(userTest)) {
        fprintf(stderr, "isEqualNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (userTest->isEqualNode(userFirst)) {
        fprintf(stderr, "isEqualNode failed in line %i\n", __LINE__);
        OK = false;
    }

    // Test rename, should throw exception
    EXCEPTIONSTEST(document->renameNode(userTest, 0, tempStr2), DOMException::NOT_SUPPORTED_ERR, OK, 208 );

    if (!OK)
        printf("\n*****The DOMDocumentFragment* method calls listed above failed, all others worked correctly.*****\n");
    return OK;
}



/**
 * This method tests DOMDocumentType* methods for the XML DOM implementation
 * @param document org.w3c.dom.DOMDocument
 *
 */
bool DOMTest::testDocumentType(DOMDocument* document)
{
    DOMTest test;
    DOMDocumentType* docType, *holdDocType;
    DOMNamedNodeMap* docNotationMap;
    DOMNode* node, *node2;
    bool OK = true;
// For debugging*****   printf("\n          testDocumentType's outputs:\n");
    XMLString::transcode("TestDocument", tempStr, 3999);
    DOMDocumentType* newDocumentType =  test.createDocumentType(document, tempStr);
    node = document->getFirstChild(); // node gets doc's docType node
    node2 = node->cloneNode(true);
    // Check nodes for equality, both their name and value or lack thereof
    if (!(!XMLString::compareString(node->getNodeName(), node2->getNodeName()) &&        // Compares node names for equality
          (node->getNodeValue() != 0 && node2->getNodeValue() != 0)     // Checks to make sure each node has a value node
        ? !XMLString::compareString(node->getNodeValue(), node2->getNodeValue())         // If both have value nodes test those value nodes for equality
        : (node->getNodeValue() == 0 && node2->getNodeValue() == 0)))   // If one node doesn't have a value node make sure both don't
    {
        fprintf(stderr, "'cloneNode' did not clone the DOMDocumentType* node correctly\n");
        OK = false;
    }
     // Deep clone test comparison is in testNode & testDocument

    DOMNode*   abc9 = document->getFirstChild();
    docType = (DOMDocumentType*) abc9;

    XMLString::transcode("ourNotationNode", tempStr, 3999);
    docNotationMap = docType->getNotations();
    if (XMLString::compareString(tempStr, docNotationMap->item(0)->getNodeName()))
    {
        fprintf(stderr, "Warning!!! DOMDocumentType's 'getNotations' failed!\n");
        OK = false;
    }
    //  doc->appendChild(newDocumentTypeImpl);//!! Throws a HIERARCHY_REQUEST_ERR    *******
    DOMNode*  abc10 = document->removeChild(document->getFirstChild()); //Tests removeChild and stores removed branch for tree reconstruction
    holdDocType = (DOMDocumentType*) abc10;
    document->insertBefore(newDocumentType, document->getDocumentElement());
    //** Other aspects of insertBefore are tested in docBuilder through appendChild*

    DOMNode* rem = document->removeChild(document->getFirstChild()); //Removes newDocumentType for tree restoral
    rem->release();
    document->insertBefore(holdDocType, document->getFirstChild()); //Reattaches removed branch to restore tree to the original

    // Test the user data
    // Test simple set and get
    DOMDocumentType* userTest = docType;
    DOMNamedNodeMap*  userFirst = docNotationMap;
    XMLCh* userSecond = tempStr2;
    XMLString::transcode("first", tempStr, 3999);

    XMLString::transcode("document", tempStr2, 3999);
    userTest->setUserData(tempStr2, (void*) document, 0);
    void* mydocument = userTest->getUserData(tempStr2);
    if (document != mydocument) {
        fprintf(stderr, "'set/getUserData' in line %i does not work\n", __LINE__);
        OK = false;
    }

    userTest->setUserData(tempStr, (void*) userFirst, 0);
    void* myFirst = userTest->getUserData(tempStr);
    if (userFirst != myFirst) {
        fprintf(stderr, "'set/getUserData' in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test overwrite
    void* myFirst2 = userTest->setUserData(tempStr, (void*) userSecond, 0);
    void* mySecond = userTest->getUserData(tempStr);
    if (userSecond != mySecond) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (userFirst != myFirst2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (userFirst == mySecond) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test null
    // test non-exist key
    XMLString::transcode("not-exist", tempStr3, 3999);
    if (userTest->getUserData(tempStr3)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }

    // use a node that does not have user data set before
    if (node2->getUserData(tempStr)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test reset
    void* mySecond2 = userTest->setUserData(tempStr, (void*) 0, 0);
    void* myNull = userTest->getUserData(tempStr);
    if (userSecond != mySecond2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (myNull) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }

    void* mydocument2 = userTest->setUserData(tempStr2, (void*) 0, 0);
    void* myNull2 = userTest->getUserData(tempStr2);
    if (mydocument != mydocument2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (myNull2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }

    //the userTest user data table should be null now
    if (userTest->getUserData(tempStr)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (userTest->getUserData(tempStr2)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test DOMUserDataHandler
    // test clone
    userTest->setUserData(tempStr2, (void*) document, &userhandler);
    DOMNode* mycloned = userTest->cloneNode(true);
    USERDATAHANDLERTEST(userhandler, DOMUserDataHandler::NODE_CLONED, tempStr2, document, userTest, mycloned, __LINE__);

    // document type cannot be imported, so no test import, rather test the exception
    document->setUserData(tempStr2, (void*) document, &userhandler);
    EXCEPTIONSTEST(document->importNode(userTest,true), DOMException::NOT_SUPPORTED_ERR, OK, 203 );

    // Test isSameNode
    if (!userTest->isSameNode(userTest)) {
        fprintf(stderr, "isSameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (userTest->isSameNode(document)) {
        fprintf(stderr, "isSameNode failed in line %i\n", __LINE__);
        OK = false;
    }

    // Test isEqualNode
    if (!userTest->isEqualNode(mycloned)) {
        fprintf(stderr, "isEqualNode failed in line %i\n", __LINE__);
        OK = false;
    }

    if (!userTest->isEqualNode(userTest)) {
        fprintf(stderr, "isEqualNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (userTest->isEqualNode(document)) {
        fprintf(stderr, "isEqualNode failed in line %i\n", __LINE__);
        OK = false;
    }

    // Test rename, should throw exception
    EXCEPTIONSTEST(document->renameNode(userTest, 0, tempStr2), DOMException::NOT_SUPPORTED_ERR, OK, 209 );

    if (!OK)
        printf("\n*****The DOMDocumentType* method calls listed above failed, all others worked correctly.*****\n");
    return OK;
}



/**
 * @param document org.w3c.dom.DOMDocument
 */
bool DOMTest::testDOMerrors(DOMDocument* document) {
    bool OK = true;

    DOMTest tests;

    EXCEPTIONSTEST(document->appendChild(testElementNode), DOMException::HIERARCHY_REQUEST_ERR, OK, 201 );
    EXCEPTIONSTEST(testTextNode->appendChild(testTextNode), DOMException::HIERARCHY_REQUEST_ERR, OK, 202 );
    return OK;
}

#define TEST_VALID_XPATH(xpath, expected, line)   \
    try \
    { \
        XMLCh xpathStr[100]; \
        XMLString::transcode(xpath,xpathStr,99); \
        DOMXPathResult* result=(DOMXPathResult*)document->evaluate(xpathStr, document->getDocumentElement(), NULL, DOMXPathResult::ORDERED_NODE_SNAPSHOT_TYPE, NULL); \
        if(result->getSnapshotLength() != expected) {  \
            fprintf(stderr, "DOMDocument::evaluate does not work in line %i (%llu nodes instead of %d)\n", line, \
                    (unsigned long long) result->getSnapshotLength(), expected); \
            OK = false; \
        }   \
        result->release(); \
    }   \
    catch(DOMException&) \
    {   \
        fprintf(stderr, "DOMDocument::evaluate failed at line %i\n", line); \
        OK = false; \
    }

#define TEST_VALID_XPATH_NS(xpath, resolver, expected, line)   \
    try \
    { \
        XMLCh xpathStr[100]; \
        XMLString::transcode(xpath,xpathStr,99); \
        DOMXPathResult* result=(DOMXPathResult*)document->evaluate(xpathStr, document->getDocumentElement(), resolver, DOMXPathResult::ORDERED_NODE_SNAPSHOT_TYPE, NULL); \
        if(result->getSnapshotLength() != expected) {  \
            fprintf(stderr, "DOMDocument::evaluate does not work in line %i (%llu nodes instead of %d)\n", line, \
                    (unsigned long long) result->getSnapshotLength(), expected); \
            OK = false; \
        }   \
        result->release(); \
    }   \
    catch(DOMException&) \
    {   \
        fprintf(stderr, "DOMDocument::evaluate failed at line %i\n", line); \
        OK = false; \
    }

#define TEST_INVALID_XPATH(xpath, line)   \
    try \
    { \
        XMLCh xpathStr[100]; \
        XMLString::transcode(xpath,xpathStr,99); \
        DOMXPathResult* result=(DOMXPathResult*)document->evaluate(xpathStr, document->getDocumentElement(), NULL, DOMXPathResult::ORDERED_NODE_SNAPSHOT_TYPE, NULL); \
        fprintf(stderr, "DOMDocument::evaluate does not work in line %i (invalid XPath)\n", line);  \
        OK = false; \
        result->release(); \
    }   \
    catch(DOMException& ) \
    {   \
    }

#define TEST_VALID_XPATH_SINGLE(xpath, line)   \
    try \
    { \
        XMLCh xpathStr[100]; \
        XMLString::transcode(xpath,xpathStr,99); \
        DOMXPathResult* result=(DOMXPathResult*)document->evaluate(xpathStr, document->getDocumentElement(), NULL, DOMXPathResult::FIRST_ORDERED_NODE_TYPE, NULL); \
        if(result->getNodeValue() == NULL) {  \
            fprintf(stderr, "DOMDocument::evaluate does not work in line %i (single node not found)\n", line);  \
            OK = false; \
        }   \
        result->release(); \
    }   \
    catch(DOMException& ) \
    {   \
        fprintf(stderr, "DOMDocument::evaluate failed at line %i\n", line); \
        OK = false; \
    }

#include <xercesc/framework/StdOutFormatTarget.hpp>

bool DOMTest::testXPath(DOMDocument* document) {
    bool OK = true;

#if 0
            XMLCh tempLS[3] = {chLatin_L, chLatin_S, chNull};
            DOMImplementation *impl          = DOMImplementationRegistry::getDOMImplementation(tempLS);
            DOMLSSerializer   *theSerializer = ((DOMImplementationLS*)impl)->createLSSerializer();
            DOMLSOutput       *theOutputDesc = ((DOMImplementationLS*)impl)->createLSOutput();
            StdOutFormatTarget myFormTarget;
            theOutputDesc->setByteStream(&myFormTarget);
            theSerializer->getDomConfig()->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true);
            theSerializer->write(document, theOutputDesc);

            theOutputDesc->release();
            theSerializer->release();
#endif

    TEST_VALID_XPATH("*", 1, __LINE__);
    TEST_VALID_XPATH("dTestBody/dBodyLevel24", 1, __LINE__);
    TEST_VALID_XPATH("//dBodyLevel34", 1, __LINE__);
    TEST_VALID_XPATH("/*", 1, __LINE__);
    TEST_VALID_XPATH("/dFirstElement/dTestBody/dBodyLevel24", 1, __LINE__);
    TEST_INVALID_XPATH("/dFirstElement//dBodyLevel34", __LINE__);  // the "//" can only be at the beginning
    TEST_INVALID_XPATH("/dFirstElement/@dFirstElementdFirstElement", __LINE__);   // cannot select attributes
    TEST_VALID_XPATH("//*", 10, __LINE__);
    TEST_VALID_XPATH_SINGLE("//*", __LINE__);
    TEST_INVALID_XPATH("//ns:node", __LINE__);  // "ns" prefix is undefined

    XMLCh tempStr[100];
    XMLString::transcode("xmlns:ns",tempStr,99);
    DOMAttr* attr=document->createAttributeNS(XMLUni::fgXMLNSURIName, tempStr);
    attr->setNodeValue(XMLUni::fgXSAXMLScanner);
    document->getDocumentElement()->setAttributeNodeNS(attr);
    const DOMXPathNSResolver* resolver=document->createNSResolver(document->getDocumentElement());
    TEST_VALID_XPATH_NS("//ns:node", resolver, 0, __LINE__);
    document->getDocumentElement()->removeAttributeNode(attr);

    return OK;
}

/**
 * This method tests DOMImplementation methods for the XML DOM implementation
 * @param document org.w3c.dom.DOMDocument
 *
 */
bool DOMTest::testDOMImplementation(DOMDocument* document)
{

    DOMImplementation* implementation;
    bool result = false;
    bool OK = true;
// For debugging*****   printf("\n          testDOMImplementation's outputs:\n");
    implementation = document->getImplementation(); //Uses getDOMImplementation to obtain implementation

    XMLString::transcode("XML", tempStr, 3999);
    XMLString::transcode("1.0", tempStr2, 3999);
    result = implementation->hasFeature(tempStr, tempStr2);
    if(!result)
    {
        fprintf(stderr, "Warning!!! DOMImplementation's 'hasFeature' that should be 'true' failed!");
        OK = false;
    }

    XMLString::transcode("HTML", tempStr, 3999);
    XMLString::transcode("4.0", tempStr2, 3999);
    result = implementation->hasFeature(tempStr, tempStr2);
    if(result)
    {
        fprintf(stderr, "Warning!!! DOMImplementation's 'hasFeature' that should be 'false' failed!");
        OK = false;
    }


    if (!OK)
        fprintf(stderr, "\n*****The DOMImplementation method calls listed above failed, all others worked correctly.*****\n");
    return OK;
}



/**
 * This method tests DOMElement* methods for the XML DOM implementation
 * @param document org.w3c.dom.DOMDocument
 *
 */
bool DOMTest::testElement(DOMDocument* document)
{
    DOMAttr* attributeNode, *newAttributeNode;
    DOMElement* element, *element2;
    DOMNode* node, *node2;

    const char* elementNames[] =  {"dFirstElement", "dTestBody", "dBodyLevel21","dBodyLevel31","dBodyLevel32",
                   "dBodyLevel22","dBodyLevel33","dBodyLevel34","dBodyLevel23","dBodyLevel24"};
    const char* textCompare[] = {"dBodyLevel31'sChildTextNode11",
                                "dBodyLevel31'sChildTextNode12",
                                "dBodyLevel31'sChildTextNode13"};

    DOMNamedNodeMap* nodeMap;
    bool OK = true;
    node = document->getDocumentElement(); // node gets doc's firstElement
    node2 = node->cloneNode(true);
    // Check nodes for equality, both their name and value or lack thereof
    if (!(!XMLString::compareString(node->getNodeName(), node2->getNodeName()) &&        // Compares node names for equality
          (node->getNodeValue() != 0 && node2->getNodeValue() != 0)     // Checks to make sure each node has a value node
        ? !XMLString::compareString(node->getNodeValue(), node2->getNodeValue())         // If both have value nodes test those value nodes for equality
        : (node->getNodeValue() == 0 && node2->getNodeValue() == 0)))   // If one node doesn't have a value node make sure both don't
    {
        fprintf(stderr, "'cloneNode' did not clone the DOMElement* node correctly.\n");
        OK = false;
    }
    // Deep clone test comparison is in testNode & testDocument

    element = document->getDocumentElement(); // element gets doc's firstElement

    XMLString::copyString(tempStr, document->getNodeValue());
    XMLString::transcode("'s test attribute", tempStr2, 3999);
    XMLString::catString(tempStr, tempStr2);
    if (XMLString::compareString(XMLUni::fgZeroLenString, element->getAttribute(tempStr)))
    {
        fprintf(stderr, "Warning!!! DOMElement's 'getAttribute' failed!\n");
        OK = false;
    }

    XMLString::copyString(tempStr, document->getNodeValue());
    XMLString::transcode("FirstElement", tempStr2, 3999);
    XMLString::catString(tempStr, tempStr2);
    attributeNode = element->getAttributeNode(tempStr);
    if(! (attributeNode == 0))
    {
        fprintf(stderr, "Warning!!! DOMElement's 'getAttributeNode' failed! It should have returned '0' here!\n");
        OK = false;
    }


    XMLString::transcode("AnotherFirstElementAttribute", tempStr, 3999);
    newAttributeNode = document->createAttribute(tempStr);

    XMLString::transcode("A new attribute which helps test calls in DOMElement", tempStr, 3999);
    newAttributeNode->setValue(tempStr);
    // This test is incorrect.  It assumes that there is a defined ordering of the entries
    //  in a nodeMap, but there is no ordering required.
#ifdef TheFollowingCheckIsInvalid
    DOMNode* rem2 = element->setAttributeNode(newAttributeNode);
    if (rem2)
        rem2->release();
    nodeMap = element->getAttributes();
    XMLSize_t size = nodeMap->getLength();
    XMLSize_t k;
    for (k = 0; k < size; k++)
    {
        DOMNode*  n = (DOMNode) nodeMap->item(k);
        XMLString::transcode(attributeCompare[k], tempStr, 3999);
        if (XMLString::compareString(tempStr, n->getNodeName())))
        {
            fprintf(stderr, "Warning!!! Comparison of firstElement's attributes failed at line %i.\n", __LINE__);
            fprintf(stderr, "   This failure can be a result of DOMElement's 'setValue' and/or 'setAttributeNode' and/or 'getAttributes' failing.\n");
            OK = false;
            break;
        }
    //  printf("firstElement's attribute number " + k + " : " + n->getNodeName());
    }
#endif

    nodeMap = element->getAttributes();
    XMLSize_t size = nodeMap->getLength();
    if (size != 2)
    {
        fprintf(stderr, "DOMElement* Tests Failure 001\n");
        OK = false;
    }
    DOMNode* rem = element->setAttributeNode(newAttributeNode);
    if (rem)
        rem->release();
    size = nodeMap->getLength();
    if (size != 3)
    {
        fprintf(stderr, "DOMElement* Tests Failure 002\n");
        OK = false;
    }

    // Fetch the newly added attribute node back out of from the named node map,
    //  and check that we are returned the same node that we put in->
    XMLString::transcode("AnotherFirstElementAttribute", tempStr, 3999);
    DOMNode*  abc12 = nodeMap->getNamedItem(tempStr);
    DOMAttr* fetchedAttr = (DOMAttr*) abc12;
    if (fetchedAttr != newAttributeNode)
    {
        fprintf(stderr, "DOMElement* Tests Failure 003\n");
        OK = false;
    }

    // Fetch the newly added attribute back out directly from the element itself.
    XMLString::transcode("AnotherFirstElementAttribute", tempStr, 3999);
    fetchedAttr = element->getAttributeNode(tempStr);
    if (fetchedAttr != newAttributeNode)
    {
        fprintf(stderr, "DOMElement* Tests Failure 004\n");
        OK = false;
    }



    XMLString::transcode("*",tempStr, 3999);
    DOMNodeList* docElements = document->getElementsByTagName(tempStr);
    XMLSize_t docSize = docElements->getLength();
    XMLSize_t i;
    for (i = 0; i < docSize; i++)
    {
        DOMNode*  n = docElements->item(i);
        XMLString::transcode(elementNames[i], tempStr, 3999);
        if (XMLString::compareString(tempStr, n->getNodeName()))
        {
            fprintf(stderr, "Warning!!! Comparison of DOMElement's 'getElementsByTagName' "
                            "and/or 'item' failed at element number %llu at line %i \n",
                    (unsigned long long) i, __LINE__ );
            fprintf(stderr, "\n");
            OK = false;
            break;
        }
    //  printf("docElement's number " + i + " is: " + n->getNodeName());
    }
    XMLString::transcode("dBodyLevel21", tempStr, 3999);
    DOMNode*  abc15 = document->getElementsByTagName(tempStr)->item(0); // element gets DOMElement* test BodyLevel21
    element = (DOMElement*) abc15;

    XMLString::transcode("dBodyLevel31", tempStr, 3999);
    DOMNode*  abc16 = document->getElementsByTagName(tempStr)->item(0); // element2 gets DOMElement* test BodyLevel31
    element2 = (DOMElement*) abc16;
    DOMNodeList* text = ((DOMNode*  &) element2)->getChildNodes();
    XMLSize_t textSize = text->getLength();
    XMLSize_t j;
    static bool firstTime = true;
    if (firstTime)
    {
        firstTime = false;      // Temporary fix.  Subsequent tests alter the doc, causing
                                //   this test to fail on all but the first time through.
        for (j = 0; j < textSize; j++)
        {
            DOMNode*  n = text->item(j);
            XMLString::transcode(textCompare[j], tempStr, 3999);
            if (XMLString::compareString(tempStr, n->getNodeValue()))
            {
                fprintf(stderr, "Warning!!! Comparison of original text nodes via DOMNode*  'getChildNodes' & DOMNodeList 'item'\n"
                        "     failed at text node: #%llu at line %i \n     ",
                        (unsigned long long) j, __LINE__ );
                OK = false;
                break;
            }
            //  printf("DOMElement* testBodyLevel31's child text node " + j + " is: " + n->getNodeValue());
        }
    }

    element = document->getDocumentElement(); // element gets doc's firstElement
    element->normalize();        // Concatenates all adjacent text nodes in this element's subtree
    DOMNodeList* text2 = ((DOMNode*) element2)->getChildNodes();
    XMLString::transcode("dBodyLevel31'sChildTextNode11dBodyLevel31'sChildTextNode12dBodyLevel31'sChildTextNode13", tempStr, 3999);
    DOMNode*  n = text2->item(0);
    if (XMLString::compareString(tempStr, n->getNodeValue()))
    {
        fprintf(stderr, "Warning!!! Comparison of concatenated text nodes created by DOMElement's 'normalize' failed!\n");
        OK = false;
    }

    XMLString::transcode("FirstElementLastAttribute", tempStr, 3999);
    XMLString::transcode("More attribute stuff for firstElement!!", tempStr2, 3999);
    element->setAttribute(tempStr, tempStr2);

    XMLString::transcode("FirstElementLastAttribute", tempStr, 3999);
    element->removeAttribute(tempStr);
    rem = element->removeAttributeNode(newAttributeNode);
    if (rem)
        rem->release();

    //  doc->getLastChild()->setNodeValue("This shouldn't work");//!! Throws a NO_MODIFICATION_ALLOWED_ERR***

    // Test the user data
    // Test simple set and get
    DOMElement* userTest = element;
    DOMAttr*  userFirst = newAttributeNode;
    XMLCh* userSecond = tempStr2;
    XMLString::transcode("first", tempStr, 3999);

    XMLString::transcode("document", tempStr2, 3999);
    userTest->setUserData(tempStr2, (void*) document, 0);
    void* mydocument = userTest->getUserData(tempStr2);
    if (document != mydocument) {
        fprintf(stderr, "'set/getUserData' in line %i does not work\n", __LINE__);
        OK = false;
    }

    userTest->setUserData(tempStr, (void*) userFirst, 0);
    void* myFirst = userTest->getUserData(tempStr);
    if (userFirst != myFirst) {
        fprintf(stderr, "'set/getUserData' in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test overwrite
    void* myFirst2 = userTest->setUserData(tempStr, (void*) userSecond, 0);
    void* mySecond = userTest->getUserData(tempStr);
    if (userSecond != mySecond) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (userFirst != myFirst2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (userFirst == mySecond) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test null
    // test non-exist key
    XMLString::transcode("not-exist", tempStr3, 3999);
    if (userTest->getUserData(tempStr3)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }

    // use a node that does not have user data set before
    if (userFirst->getUserData(tempStr)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test reset
    void* mySecond2 = userTest->setUserData(tempStr, (void*) 0, 0);
    void* myNull = userTest->getUserData(tempStr);
    if (userSecond != mySecond2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (myNull) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }

    void* mydocument2 = userTest->setUserData(tempStr2, (void*) 0, 0);
    void* myNull2 = userTest->getUserData(tempStr2);
    if (mydocument != mydocument2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (myNull2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }

    //the userTest user data table should be null now
    if (userTest->getUserData(tempStr)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (userTest->getUserData(tempStr2)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test DOMUserDataHandler
    // test clone
    userTest->setUserData(tempStr2, (void*) document, &userhandler);
    DOMNode* mycloned = userTest->cloneNode(true);
    USERDATAHANDLERTEST(userhandler, DOMUserDataHandler::NODE_CLONED, tempStr2, document, userTest, mycloned, __LINE__);

    // test import
    document->setUserData(tempStr2, (void*) document, &userhandler);
    DOMNode* myimport = document->importNode(userTest,true);
    USERDATAHANDLERTEST(userhandler, DOMUserDataHandler::NODE_IMPORTED, tempStr2, document, userTest, myimport, __LINE__);

    // test delete
    myimport->setUserData(tempStr2, (void*) userTest, &userhandler);
    myimport->release();
    USERDATAHANDLERTEST(userhandler, DOMUserDataHandler::NODE_DELETED, tempStr2, userTest, 0, 0, __LINE__);

    // Test isSameNode
    if (!userTest->isSameNode(userTest)) {
        fprintf(stderr, "isSameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (userTest->isSameNode(userFirst)) {
        fprintf(stderr, "isSameNode failed in line %i\n", __LINE__);
        OK = false;
    }

    // Test isEqualNode
    if (!userTest->isEqualNode(mycloned)) {
        fprintf(stderr, "isEqualNode failed in line %i\n", __LINE__);
        OK = false;
    }

    if (!userTest->isEqualNode(userTest)) {
        fprintf(stderr, "isEqualNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (userTest->isEqualNode(userFirst)) {
        fprintf(stderr, "isEqualNode failed in line %i\n", __LINE__);
        OK = false;
    }

    // Test renameNode
    XMLString::transcode("http://nsa", tempStr4, 3999);
    XMLString::transcode("aa", tempStr5, 3999);
    XMLString::transcode("pnsa:aa", tempStr3, 3999);

    // create the element
    DOMElement* renameTestElement = document->createElement(tempStr5);
    DOMElement* renameTestElementNS = document->createElementNS(tempStr4, tempStr3);

    // create the parent
    DOMElement* renameTestParent = document->createElement(tempStr5);
    renameTestParent->appendChild(renameTestElement);
    renameTestParent->appendChild(renameTestElementNS);

    // set up the userdata
    renameTestElement->setUserData(tempStr5, (void*) document, &userhandler);
    renameTestElementNS->setUserData(tempStr4, (void*) document, 0);

    // append a text node as child
    DOMText* renameTestText = document->createTextNode(tempStr5);
    DOMText* renameTestTextNS = document->createTextNode(tempStr4);
    renameTestElement->appendChild(renameTestText);
    renameTestElementNS->appendChild(renameTestTextNS);

    XMLString::transcode("http://nsb", tempStr, 3999);
    XMLString::transcode("bb", tempStr2, 3999);
    XMLString::transcode("pnsb:bb", tempStr3, 3999);

    // set up some attributes
    DOMAttr* renameTestAttribute = document->createAttribute(tempStr5);
    DOMAttr* renameTestAttributeNS = document->createAttributeNS(tempStr4, tempStr3);
    renameTestElement->setAttributeNode(renameTestAttribute);
    renameTestElementNS->setAttributeNodeNS(renameTestAttributeNS);

    //Test compareDocumentPosition first before testing rename
    // renameTestParent
    //  |
    //  |_ renameTestElement (has renameTestAttribute)
    //  |          |
    //  |          |_ renameTestText
    //  |
    //  |_ renameTestElementNS (has renameTestAttributeNS)
    //  |          |
    //  |          |_ renameTestTextNS
    //
    COMPARETREEPOSITIONTEST(renameTestAttribute, renameTestAttributeNS, DOMNode::DOCUMENT_POSITION_FOLLOWING, __LINE__);
    COMPARETREEPOSITIONTEST(renameTestAttribute, renameTestElement, DOMNode::DOCUMENT_POSITION_PRECEDING, __LINE__);
    COMPARETREEPOSITIONTEST(renameTestAttribute, renameTestText, DOMNode::DOCUMENT_POSITION_FOLLOWING, __LINE__);
    COMPARETREEPOSITIONTEST(renameTestAttribute, renameTestTextNS, DOMNode::DOCUMENT_POSITION_FOLLOWING, __LINE__);
    COMPARETREEPOSITIONTEST(renameTestAttribute, renameTestParent, DOMNode::DOCUMENT_POSITION_PRECEDING, __LINE__);

    COMPARETREEPOSITIONTEST(renameTestAttributeNS, renameTestAttribute, DOMNode::DOCUMENT_POSITION_PRECEDING, __LINE__);
    COMPARETREEPOSITIONTEST(renameTestElement, renameTestAttributeNS, DOMNode::DOCUMENT_POSITION_FOLLOWING, __LINE__);
    COMPARETREEPOSITIONTEST(renameTestAttributeNS, renameTestText, DOMNode::DOCUMENT_POSITION_PRECEDING, __LINE__);
    COMPARETREEPOSITIONTEST(renameTestTextNS, renameTestAttributeNS, DOMNode::DOCUMENT_POSITION_PRECEDING, __LINE__);

    // start the rename tests
    // rename the NS Element
    DOMElement* renameTest = (DOMElement*) document->renameNode(renameTestElementNS, tempStr, tempStr3);
    // test the name
    if (XMLString::compareString(tempStr, renameTest->getNamespaceURI()))
    {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (XMLString::compareString(tempStr2, renameTest->getLocalName()))
    {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (XMLString::compareString(tempStr3, renameTest->getNodeName()))
    {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    // test the child / nodevalue
    if (XMLString::compareString(tempStr4, renameTest->getFirstChild()->getNodeValue()))
    {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    // test the attribute
    if (!renameTest->getAttributeNodeNS(tempStr4, tempStr2))
    {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    // test the owner element
    if (renameTestParent->getElementsByTagNameNS(tempStr, tempStr2)->getLength() != 1) {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (renameTestParent->getElementsByTagNameNS(tempStr4, tempStr5)->getLength() != 0) {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    // test user data
    void* renamedocument = renameTest->getUserData(tempStr4);
    if (document != renamedocument) {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    // test isSame and isEqual
    if (!renameTestElementNS->isEqualNode(renameTest)) {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (!renameTestElementNS->isSameNode(renameTest)) {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }


    // rename the Element (null namespace)
    renameTest = (DOMElement*) document->renameNode(renameTestElement, 0, tempStr2);
    // test the name
    if (renameTest->getNamespaceURI())
    {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (renameTest->getLocalName())
    {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (XMLString::compareString(tempStr2, renameTest->getNodeName()))
    {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    // test the child / nodevalue
    if (XMLString::compareString(tempStr5, renameTest->getFirstChild()->getNodeValue()))
    {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    // test the attribute
    if (!renameTest->getAttributeNode(tempStr5))
    {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    // test the owner element
    if (renameTestParent->getElementsByTagName(tempStr2)->getLength() != 1) {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (renameTestParent->getElementsByTagName(tempStr5)->getLength() != 0) {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    // test user data
    renamedocument = renameTest->getUserData(tempStr5);
    if (document != renamedocument) {
        fprintf(stderr, "'set/getUserData' in line %i does not work\n", __LINE__);
        OK = false;
    }
    // test isSame and isEqual
    if (!renameTestElement->isEqualNode(renameTest)) {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (!renameTestElement->isSameNode(renameTest)) {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }


    // rename the Element (with namespace)
    renameTest = (DOMElement*) document->renameNode(renameTestElement, tempStr, tempStr3);
    // test the name
    if (XMLString::compareString(tempStr, renameTest->getNamespaceURI()))
    {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (XMLString::compareString(tempStr2, renameTest->getLocalName()))
    {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (XMLString::compareString(tempStr3, renameTest->getNodeName()))
    {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    // test the child / nodevalue
    if (XMLString::compareString(tempStr5, renameTest->getFirstChild()->getNodeValue()))
    {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (renameTestElement->getFirstChild())
    {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    // test the attribute
    if (!renameTest->getAttributeNode(tempStr5))
    {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (renameTestElement->getAttributeNode(tempStr5))
    {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    // test the owner element
    // the nodelist should be 2 items as we have to count the renameTestElementNS as well
    if (renameTestParent->getElementsByTagNameNS(tempStr, tempStr2)->getLength() != 2) {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (renameTestParent->getElementsByTagNameNS(0, tempStr2)->getLength() != 0) {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (renameTestParent->getElementsByTagName(tempStr2)->getLength() != 0) {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    // test user data
    renamedocument = renameTest->getUserData(tempStr5);
    if (document != renamedocument) {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    // test userdatahandler
    USERDATAHANDLERTEST(userhandler, DOMUserDataHandler::NODE_RENAMED, tempStr5, document, renameTestElement, renameTest, __LINE__);
    // test isSame and isEqual
    // a new node is created here, so both isSame and isEqual are not compared
    if (renameTestElement->isEqualNode(renameTest)) {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (renameTestElement->isSameNode(renameTest)) {
        fprintf(stderr, "renameNode failed in line %i\n", __LINE__);
        OK = false;
    }


    //setIdAttribute tests

    XMLString::transcode("http://nsa", tempStr4, 3999);
    XMLString::transcode("aa", tempStr5, 3999);

    DOMAttr *idAtt = document->createAttributeNS(tempStr4, tempStr5);

    //tests for node not being on testElementNode
    EXCEPTIONSTEST(testElementNode->setIdAttribute(tempStr4, true), DOMException::NOT_FOUND_ERR, OK,  1000);
    EXCEPTIONSTEST(testElementNode->setIdAttributeNS(tempStr4, tempStr5, true), DOMException::NOT_FOUND_ERR, OK,  1001);
    EXCEPTIONSTEST(testElementNode->setIdAttributeNode(idAtt, true), DOMException::NOT_FOUND_ERR, OK,  1002);

    //should test NO_MODIFICATION_ALLOWED_ERR but dont know how to without direct access to DOMAttrImpl.

    idAtt = document->createAttributeNS(tempStr4, tempStr5);
    idAtt->setValue(tempStr3);
    testElementNode->setAttributeNode(idAtt);
    testElementNode->setIdAttributeNode(idAtt, true);

    if(!idAtt->isId()) {
        fprintf(stderr, "setIdAttributeNode failed in line %i\n", __LINE__);
        OK = false;
    }

    DOMElement *idEle = document->getElementById(tempStr3);

    if(!idEle || !idEle->isSameNode(testElementNode)) {
        fprintf(stderr, "setIdAttributeNode failed in line %i\n", __LINE__);
        OK = false;
    }

    testElementNode->setIdAttributeNode(idAtt, false);

    if(idAtt->isId()) {
        fprintf(stderr, "setIdAttributeNode failed in line %i\n", __LINE__);
        OK = false;
    }

    idEle = document->getElementById(tempStr3);

    if(idEle) {
        fprintf(stderr, "setIdAttributeNode failed in line %i\n", __LINE__);
        OK = false;
    }

    testElementNode->removeAttributeNode(idAtt);


    XMLString::transcode("someval", tempStr3, 3999);
    idAtt = document->createAttributeNS(tempStr4, tempStr5);
    idAtt->setValue(tempStr3);
    testElementNode->setAttributeNode(idAtt);
    testElementNode->setIdAttributeNS(tempStr4, tempStr5, true);

    if(!idAtt->isId()) {
        fprintf(stderr, "setIdAttributeNS failed in line %i\n", __LINE__);
        OK = false;
    }

    idEle = document->getElementById(tempStr3);

    if(!idEle || !idEle->isSameNode(testElementNode)) {
        fprintf(stderr, "setIdAttributeNS failed in line %i\n", __LINE__);
        OK = false;
    }

    testElementNode->setIdAttributeNS(tempStr4, tempStr5, false);

    if(idAtt->isId()) {
        fprintf(stderr, "setIdAttributeNS failed in line %i\n", __LINE__);
        OK = false;
    }

    idEle = document->getElementById(tempStr3);

    if(idEle) {
        fprintf(stderr, "setIdAttributeNS failed in line %i\n", __LINE__);
        OK = false;
    }

    testElementNode->removeAttributeNode(idAtt);
    idAtt->release();


    XMLString::transcode("somevalDif", tempStr3, 3999);
    idAtt = document->createAttribute(tempStr5);
    idAtt->setValue(tempStr3);
    testElementNode->setAttributeNode(idAtt);
    testElementNode->setIdAttribute(tempStr5, true);

    if(!idAtt->isId()) {
        fprintf(stderr, "setIdAttribute failed in line %i\n", __LINE__);
        OK = false;
    }

    idEle = document->getElementById(tempStr3);

    if(!idEle || !idEle->isSameNode(testElementNode)) {
        fprintf(stderr, "setIdAttribute failed in line %i\n", __LINE__);
        OK = false;
    }

    testElementNode->setIdAttribute(tempStr5, false);

    if(idAtt->isId()) {
        fprintf(stderr, "setIdAttribute failed in line %i\n", __LINE__);
        OK = false;
    }

    idEle = document->getElementById(tempStr3);

    if(idEle) {
        fprintf(stderr, "setIdAttribute failed in line %i\n", __LINE__);
        OK = false;
    }

    testElementNode->removeAttributeNode(idAtt);
    idAtt->release();

    if (!OK)
        printf("\n*****The DOMElement* method calls listed above failed, all others worked correctly.*****\n");
    return OK;
}



/**
 * This method tests DOMEntity* methods for the XML DOM implementation
 * @param document org.w3c.dom.DOMDocument
 *
 */
bool DOMTest::testEntity(DOMDocument* document)
{
    DOMEntity* entity;
    DOMNode* node, *node2;
    bool OK = true;
// For debugging*****   printf("\n          testEntity's outputs:\n\n");
    XMLString::transcode("ourEntityNode", tempStr, 3999);
    DOMNode*  abc20 = document->getDoctype()->getEntities()->getNamedItem(tempStr);
    entity = (DOMEntity*) abc20;
    node = entity;
    node2 = entity->cloneNode(true);
    // Check nodes for equality, both their name and value or lack thereof
    if (!(!XMLString::compareString(node->getNodeName(), node2->getNodeName()) &&        // Compares node names for equality
          (node->getNodeValue() != 0 && node2->getNodeValue() != 0)     // Checks to make sure each node has a value node
        ? !XMLString::compareString(node->getNodeValue(), node2->getNodeValue())         // If both have value nodes test those value nodes for equality
        : (node->getNodeValue() == 0 && node2->getNodeValue() == 0)))   // If one node doesn't have a value node make sure both don't
    {
        fprintf(stderr, "'cloneNode' did not clone the DOMEntity* node correctly");
        OK = false;
    }
    // Deep clone test comparison is in testNode & testDocument

    // Test the user data
    // Test simple set and get
    DOMEntity* userTest = entity;
    DOMNode*  userFirst = node;
    XMLCh* userSecond = tempStr2;
    XMLString::transcode("first", tempStr, 3999);

    XMLString::transcode("document", tempStr2, 3999);
    userTest->setUserData(tempStr2, (void*) document, 0);
    void* mydocument = userTest->getUserData(tempStr2);
    if (document != mydocument) {
        fprintf(stderr, "'set/getUserData' in line %i does not work\n", __LINE__);
        OK = false;
    }

    userTest->setUserData(tempStr, (void*) userFirst, 0);
    void* myFirst = userTest->getUserData(tempStr);
    if (userFirst != myFirst) {
        fprintf(stderr, "'set/getUserData' in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test overwrite
    void* myFirst2 = userTest->setUserData(tempStr, (void*) userSecond, 0);
    void* mySecond = userTest->getUserData(tempStr);
    if (userSecond != mySecond) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (userFirst != myFirst2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (userFirst == mySecond) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test null
    // test non-exist key
    XMLString::transcode("not-exist", tempStr3, 3999);
    if (userTest->getUserData(tempStr3)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }

    // use a node that does not have user data set before
    if (userFirst->getUserData(tempStr)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test reset
    void* mySecond2 = userTest->setUserData(tempStr, (void*) 0, 0);
    void* myNull = userTest->getUserData(tempStr);
    if (userSecond != mySecond2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (myNull) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }

    void* mydocument2 = userTest->setUserData(tempStr2, (void*) 0, 0);
    void* myNull2 = userTest->getUserData(tempStr2);
    if (mydocument != mydocument2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (myNull2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }

    //the userTest user data table should be null now
    if (userTest->getUserData(tempStr)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (userTest->getUserData(tempStr2)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test DOMUserDataHandler
    // test clone
    userTest->setUserData(tempStr2, (void*) document, &userhandler);
    DOMNode* mycloned = userTest->cloneNode(true);
    USERDATAHANDLERTEST(userhandler, DOMUserDataHandler::NODE_CLONED, tempStr2, document, userTest, mycloned, __LINE__);

    // test import
    document->setUserData(tempStr2, (void*) document, &userhandler);
    DOMNode* myimport = document->importNode(userTest,true);
    USERDATAHANDLERTEST(userhandler, DOMUserDataHandler::NODE_IMPORTED, tempStr2, document, userTest, myimport, __LINE__);

    // test delete
    myimport->setUserData(tempStr2, (void*) userTest, &userhandler);
    myimport->release();
    USERDATAHANDLERTEST(userhandler, DOMUserDataHandler::NODE_DELETED, tempStr2, userTest, 0, 0, __LINE__);

    // Test isSameNode
    if (!userTest->isSameNode(userTest)) {
        fprintf(stderr, "isSameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (userTest->isSameNode(userFirst)) {
        fprintf(stderr, "isSameNode failed in line %i\n", __LINE__);
        OK = false;
    }

    // Test isEqualNode
    if (!userTest->isEqualNode(mycloned)) {
        fprintf(stderr, "isEqualNode failed in line %i\n", __LINE__);
        OK = false;
    }

    if (!userTest->isEqualNode(userTest)) {
        fprintf(stderr, "isEqualNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (userTest->isEqualNode(userFirst)) {
        fprintf(stderr, "isEqualNode failed in line %i\n", __LINE__);
        OK = false;
    }

    // Test rename, should throw exception
    EXCEPTIONSTEST(document->renameNode(userTest, 0, tempStr2), DOMException::NOT_SUPPORTED_ERR, OK, 210 );

    if (!OK)
        printf("\n*****The DOMEntity* method calls listed above failed, all others worked correctly.*****\n");
    return OK;
}


/**
 * This method tests DOMEntityReference* methods for the XML DOM implementation
 * @param document org.w3c.dom.DOMDocument
 *
 */
bool DOMTest::testEntityReference(DOMDocument* document)
{
    DOMEntityReference* entityReference;
    DOMNode* node, *node2;
    bool OK = true;
// For debugging*****   printf("\n          testEntityReference's outputs:\n");
    DOMNode*  abc30 = document->getLastChild()->getLastChild()->getLastChild()->getFirstChild();
    entityReference = (DOMEntityReference*) abc30;
    node = entityReference;
    node2 = node->cloneNode(true);
    // Check nodes for equality, both their name and value or lack thereof
    if (!(!XMLString::compareString(node->getNodeName(), node2->getNodeName()) &&        // Compares node names for equality
          (node->getNodeValue() != 0 && node2->getNodeValue() != 0)     // Checks to make sure each node has a value node
        ? !XMLString::compareString(node->getNodeValue(), node2->getNodeValue())         // If both have value nodes test those value nodes for equality
        : (node->getNodeValue() == 0 && node2->getNodeValue() == 0)))   // If one node doesn't have a value node make sure both don't
    {
        fprintf(stderr, "'cloneNode' did not clone the DOMEntityReference* node correctly\n");
        OK = false;
    }
    // Deep clone test comparison is in testNode & testDocument

    //  entityReference->setNodeValue("This shouldn't work");//!! Throws a NO_MODIFICATION_ALLOWED_ERR ********

    // Test the user data
    // Test simple set and get
    DOMEntityReference* userTest = entityReference;
    DOMNode*  userFirst = node2;
    XMLCh* userSecond = tempStr2;
    XMLString::transcode("first", tempStr, 3999);

    XMLString::transcode("document", tempStr2, 3999);
    userTest->setUserData(tempStr2, (void*) document, 0);
    void* mydocument = userTest->getUserData(tempStr2);
    if (document != mydocument) {
        fprintf(stderr, "'set/getUserData' in line %i does not work\n", __LINE__);
        OK = false;
    }

    userTest->setUserData(tempStr, (void*) userFirst, 0);
    void* myFirst = userTest->getUserData(tempStr);
    if (userFirst != myFirst) {
        fprintf(stderr, "'set/getUserData' in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test overwrite
    void* myFirst2 = userTest->setUserData(tempStr, (void*) userSecond, 0);
    void* mySecond = userTest->getUserData(tempStr);
    if (userSecond != mySecond) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (userFirst != myFirst2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (userFirst == mySecond) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test null
    // test non-exist key
    XMLString::transcode("not-exist", tempStr3, 3999);
    if (userTest->getUserData(tempStr3)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }

    // use a node that does not have user data set before
    if (node2->getUserData(tempStr)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test reset
    void* mySecond2 = userTest->setUserData(tempStr, (void*) 0, 0);
    void* myNull = userTest->getUserData(tempStr);
    if (userSecond != mySecond2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (myNull) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }

    void* mydocument2 = userTest->setUserData(tempStr2, (void*) 0, 0);
    void* myNull2 = userTest->getUserData(tempStr2);
    if (mydocument != mydocument2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (myNull2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }

    //the userTest user data table should be null now
    if (userTest->getUserData(tempStr)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (userTest->getUserData(tempStr2)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test DOMUserDataHandler
    // test clone
    userTest->setUserData(tempStr2, (void*) document, &userhandler);
    DOMNode* mycloned = userTest->cloneNode(true);
    USERDATAHANDLERTEST(userhandler, DOMUserDataHandler::NODE_CLONED, tempStr2, document, userTest, mycloned, __LINE__);

    // test import
    document->setUserData(tempStr2, (void*) document, &userhandler);
    DOMNode* myimport = document->importNode(userTest,true);
    USERDATAHANDLERTEST(userhandler, DOMUserDataHandler::NODE_IMPORTED, tempStr2, document, userTest, myimport, __LINE__);

    // test delete
    myimport->setUserData(tempStr2, (void*) userTest, &userhandler);
    myimport->release();
    USERDATAHANDLERTEST(userhandler, DOMUserDataHandler::NODE_DELETED, tempStr2, userTest, 0, 0, __LINE__);

    // Test isSameNode
    if (!userTest->isSameNode(userTest)) {
        fprintf(stderr, "isSameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (userTest->isSameNode(userFirst)) {
        fprintf(stderr, "isSameNode failed in line %i\n", __LINE__);
        OK = false;
    }

    // Test isEqualNode
    if (!userTest->isEqualNode(mycloned)) {
        fprintf(stderr, "isEqualNode failed in line %i\n", __LINE__);
        OK = false;
    }

    if (!userTest->isEqualNode(userTest)) {
        fprintf(stderr, "isEqualNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (userTest->isEqualNode(document)) {
        fprintf(stderr, "isEqualNode failed in line %i\n", __LINE__);
        OK = false;
    }

    // Test rename, should throw exception
    EXCEPTIONSTEST(document->renameNode(userTest, 0, tempStr2), DOMException::NOT_SUPPORTED_ERR, OK, 211 );

    if (!OK)
        printf("\n*****The DOMEntityReference* method calls listed above failed, all others worked correctly.*****\n");
    return OK;
}



/**
 * This method tests DOMNode*  methods for the XML DOM implementation
 * @param document org.w3c.dom.DOMDocument
 *
 *
 *
 ********* This is only for a test of cloneNode "deep"*******
 ********* And for error tests*********
 */
bool DOMTest::testNode(DOMDocument* document)
{
    DOMNode* node, *node2;
    bool result;
    bool OK = true;
// For debugging*****   printf("\n          testNode's outputs:\n");
    node = document->getDocumentElement();
    node2 = node->cloneNode(true);
    result = treeCompare(node, node2); // Deep clone test of cloneNode
    if (result)
    {
        //printf("'cloneNode' successfully cloned this whole node tree (deep)!\n");
    }
    else
    {
        fprintf(stderr, "'cloneNode' did not successfully clone this whole node tree (deep)!\n");
        OK = false;
    }
    //!! The following gives a did not clone successfully message*********
    node = document->getDocumentElement();
    node2 = node->getFirstChild();
    result = treeCompare(node, node2);
    if (!result)
    {
        //fprintf(stderr, "'cloneNode' did not successfully clone this whole node tree (deep)!\n");
    }
    else
    {
        fprintf(stderr, "'cloneNode' was supposed to fail here, either it or 'treeCompare' failed!!!\n");
        OK = false;
    }
    // Deep clone test also in testDocument

    // Test the user data
    // Test simple set and get
    DOMNode* userTest = node;
    DOMNode*  userFirst = node2;
    XMLCh* userSecond = tempStr2;
    XMLString::transcode("first", tempStr, 3999);

    XMLString::transcode("document", tempStr2, 3999);
    userTest->setUserData(tempStr2, (void*) document, 0);
    void* mydocument = userTest->getUserData(tempStr2);
    if (document != mydocument) {
        fprintf(stderr, "'set/getUserData' in line %i does not work\n", __LINE__);
        OK = false;
    }

    userTest->setUserData(tempStr, (void*) userFirst, 0);
    void* myFirst = userTest->getUserData(tempStr);
    if (userFirst != myFirst) {
        fprintf(stderr, "'set/getUserData' in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test overwrite
    void* myFirst2 = userTest->setUserData(tempStr, (void*) userSecond, 0);
    void* mySecond = userTest->getUserData(tempStr);
    if (userSecond != mySecond) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (userFirst != myFirst2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (userFirst == mySecond) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test null
    // test non-exist key
    XMLString::transcode("not-exist", tempStr3, 3999);
    if (userTest->getUserData(tempStr3)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }

    // use a node that does not have user data set before
    if (userFirst->getUserData(tempStr)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test reset
    void* mySecond2 = userTest->setUserData(tempStr, (void*) 0, 0);
    void* myNull = userTest->getUserData(tempStr);
    if (userSecond != mySecond2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (myNull) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }

    void* mydocument2 = userTest->setUserData(tempStr2, (void*) 0, 0);
    void* myNull2 = userTest->getUserData(tempStr2);
    if (mydocument != mydocument2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (myNull2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }

    //the userTest user data table should be null now
    if (userTest->getUserData(tempStr)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (userTest->getUserData(tempStr2)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test DOMUserDataHandler
    // test clone
    userTest->setUserData(tempStr2, (void*) document, &userhandler);
    DOMNode* mycloned = userTest->cloneNode(true);
    USERDATAHANDLERTEST(userhandler, DOMUserDataHandler::NODE_CLONED, tempStr2, document, userTest, mycloned, __LINE__);

    // test import
    document->setUserData(tempStr2, (void*) document, &userhandler);
    DOMNode* myimport = document->importNode(userTest,true);
    USERDATAHANDLERTEST(userhandler, DOMUserDataHandler::NODE_IMPORTED, tempStr2, document, userTest, myimport, __LINE__);

    // test delete
    myimport->setUserData(tempStr2, (void*) userTest, &userhandler);
    myimport->release();
    USERDATAHANDLERTEST(userhandler, DOMUserDataHandler::NODE_DELETED, tempStr2, userTest, 0, 0, __LINE__);

    // Test isSameNode
    if (!userTest->isSameNode(userTest)) {
        fprintf(stderr, "isSameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (userTest->isSameNode(userFirst)) {
        fprintf(stderr, "isSameNode failed in line %i\n", __LINE__);
        OK = false;
    }

    // Test isEqualNode
    if (!userTest->isEqualNode(mycloned)) {
        fprintf(stderr, "isEqualNode failed in line %i\n", __LINE__);
        OK = false;
    }

    if (!userTest->isEqualNode(userTest)) {
        fprintf(stderr, "isEqualNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (userTest->isEqualNode(document)) {
        fprintf(stderr, "isEqualNode failed in line %i\n", __LINE__);
        OK = false;
    }

    if (!OK)
        printf("\n*****The DOMNode*  method calls listed above failed, all others worked correctly.*****\n");
    return OK;
}



/**
 * This method tests DOMNotation* methods for the XML DOM implementation
 * @param document org.w3c.dom.DOMDocument
 *
 */
bool DOMTest::testNotation(DOMDocument* document)
{
    DOMNode* node, *node2;
    DOMNotation* notation;
    bool OK = true;
// For debugging*****   printf("\n          testNotation's outputs:\n");
    XMLString::transcode("ourNotationNode", tempStr, 3999);
	DOMNode*  abc40 = document->getDoctype()->getNotations()->getNamedItem(tempStr);
    notation = (DOMNotation*) abc40;
    node = notation;
    node2 = notation->cloneNode(true);//*****?
    // Check nodes for equality, both their name and value or lack thereof
    if (!(!XMLString::compareString(node->getNodeName(), node2->getNodeName()) &&        // Compares node names for equality
          (node->getNodeValue() != 0 && node2->getNodeValue() != 0)     // Checks to make sure each node has a value node
        ? !XMLString::compareString(node->getNodeValue(), node2->getNodeValue())         // If both have value nodes test those value nodes for equality
        : (node->getNodeValue() == 0 && node2->getNodeValue() == 0)))   // If one node doesn't have a value node make sure both don't
    {
        fprintf(stderr, "'cloneNode' did not clone the DOMNotation* node correctly");
        OK = false;
    }
    // Deep clone test comparison is in testNode & testDocument

    //  notation->setNodeValue("This shouldn't work");//!! Throws a NO_MODIFICATION_ALLOWED_ERR ********

    // Test the user data
    // Test simple set and get
    DOMNotation* userTest = notation;
    DOMNode*  userFirst = node2;
    XMLCh* userSecond = tempStr2;
    XMLString::transcode("first", tempStr, 3999);

    XMLString::transcode("document", tempStr2, 3999);
    userTest->setUserData(tempStr2, (void*) document, 0);
    void* mydocument = userTest->getUserData(tempStr2);
    if (document != mydocument) {
        fprintf(stderr, "'set/getUserData' in line %i does not work\n", __LINE__);
        OK = false;
    }

    userTest->setUserData(tempStr, (void*) userFirst, 0);
    void* myFirst = userTest->getUserData(tempStr);
    if (userFirst != myFirst) {
        fprintf(stderr, "'set/getUserData' in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test overwrite
    void* myFirst2 = userTest->setUserData(tempStr, (void*) userSecond, 0);
    void* mySecond = userTest->getUserData(tempStr);
    if (userSecond != mySecond) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (userFirst != myFirst2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (userFirst == mySecond) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test null
    // test non-exist key
    XMLString::transcode("not-exist", tempStr3, 3999);
    if (userTest->getUserData(tempStr3)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }

    // use a node that does not have user data set before
    if (userFirst->getUserData(tempStr)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test reset
    void* mySecond2 = userTest->setUserData(tempStr, (void*) 0, 0);
    void* myNull = userTest->getUserData(tempStr);
    if (userSecond != mySecond2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (myNull) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }

    void* mydocument2 = userTest->setUserData(tempStr2, (void*) 0, 0);
    void* myNull2 = userTest->getUserData(tempStr2);
    if (mydocument != mydocument2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (myNull2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }

    //the userTest user data table should be null now
    if (userTest->getUserData(tempStr)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (userTest->getUserData(tempStr2)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test DOMUserDataHandler
    // test clone
    userTest->setUserData(tempStr2, (void*) document, &userhandler);
    DOMNode* mycloned = userTest->cloneNode(true);
    USERDATAHANDLERTEST(userhandler, DOMUserDataHandler::NODE_CLONED, tempStr2, document, userTest, mycloned, __LINE__);

    // test import
    document->setUserData(tempStr2, (void*) document, &userhandler);
    DOMNode* myimport = document->importNode(userTest,true);
    USERDATAHANDLERTEST(userhandler, DOMUserDataHandler::NODE_IMPORTED, tempStr2, document, userTest, myimport, __LINE__);

    // test delete
    myimport->setUserData(tempStr2, (void*) userTest, &userhandler);
    myimport->release();
    USERDATAHANDLERTEST(userhandler, DOMUserDataHandler::NODE_DELETED, tempStr2, userTest, 0, 0, __LINE__);

    // Test isSameNode
    if (!userTest->isSameNode(userTest)) {
        fprintf(stderr, "isSameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (userTest->isSameNode(userFirst)) {
        fprintf(stderr, "isSameNode failed in line %i\n", __LINE__);
        OK = false;
    }

    // Test isEqualNode
    if (!userTest->isEqualNode(mycloned)) {
        fprintf(stderr, "isEqualNode failed in line %i\n", __LINE__);
        OK = false;
    }

    if (!userTest->isEqualNode(userTest)) {
        fprintf(stderr, "isEqualNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (userTest->isEqualNode(document)) {
        fprintf(stderr, "isEqualNode failed in line %i\n", __LINE__);
        OK = false;
    }

    // Test rename, should throw exception
    EXCEPTIONSTEST(document->renameNode(userTest, 0, tempStr2), DOMException::NOT_SUPPORTED_ERR, OK, 212 );

    if (!OK)
        printf("\n*****The DOMNotation* method calls listed above failed, all others worked correctly.*****\n");
    return OK;
}



/**
 * This method tests DOMProcessingInstruction* methods for the XML DOM implementation
 * @param document org.w3c.dom.DOMDocument
 *
 */
bool DOMTest::testPI(DOMDocument* document)
{
    DOMProcessingInstruction* pI, *pI2;
    bool OK = true;
// For debugging*****   printf("\n          testPI's outputs:\n");
	DOMNode*   abc50 = document->getDocumentElement()->getFirstChild();// Get doc's DOMProcessingInstruction
    pI  = (DOMProcessingInstruction*) abc50;
	DOMNode*   abc51 = pI->cloneNode(true);//*****?
    pI2 = (DOMProcessingInstruction*) abc51;
    // Check nodes for equality, both their name and value or lack thereof
    if (!(!XMLString::compareString(pI->getNodeName(), pI2->getNodeName()) &&         // Compares node names for equality
         (pI->getNodeValue() != 0 && pI2->getNodeValue() != 0)  // Checks to make sure each node has a value node
        ? !XMLString::compareString(pI->getNodeValue(), pI2->getNodeValue())      // If both have value nodes test those value nodes for equality
        :(pI->getNodeValue() == 0 && pI2->getNodeValue() == 0)))// If one node doesn't have a value node make sure both don't
    {
        fprintf(stderr, "'cloneNode' did not clone the DOMEntity* node correctly\n");
        OK = false;
    }
    // Deep clone test comparison is in testNode & testDocument
    // compare = "This is [#document: 0]'s processing instruction";  // AH Revisit.  Where id
    //                  this ": 0]" stuff come from in the Java version??  I don' think that it is right.
    XMLString::transcode("This is #document's processing instruction", tempStr, 3999);
    if (XMLString::compareString(tempStr, pI->getData()))
    {
        fprintf(stderr, "Warning!!! PI's 'getData' failed!\n");
        OK = false;
    }

    XMLString::transcode("PI's reset data", tempStr, 3999);
    pI->setData(tempStr);
    if (XMLString::compareString(tempStr, pI->getData()))
    {
        fprintf(stderr, "Warning!!! PI's 'setData' failed!\n");
        OK = false;
    }
    XMLString::transcode("dTargetProcessorChannel", tempStr, 3999);
    if (XMLString::compareString(tempStr, pI->getTarget()))
    {
        fprintf(stderr, "Warning!!! PI's 'getTarget' failed!\n");
        OK = false;
    }


    // Restore original PI data.
    XMLString::transcode("This is #document's processing instruction", tempStr, 3999);
    pI->setData(tempStr);

    // Test the user data
    // Test simple set and get
    DOMProcessingInstruction* userTest = pI;
    DOMNode*  userFirst = abc51;
    XMLCh* userSecond = tempStr2;
    XMLString::transcode("first", tempStr, 3999);

    XMLString::transcode("document", tempStr2, 3999);
    userTest->setUserData(tempStr2, (void*) document, 0);
    void* mydocument = userTest->getUserData(tempStr2);
    if (document != mydocument) {
        fprintf(stderr, "'set/getUserData' in line %i does not work\n", __LINE__);
        OK = false;
    }

    userTest->setUserData(tempStr, (void*) userFirst, 0);
    void* myFirst = userTest->getUserData(tempStr);
    if (userFirst != myFirst) {
        fprintf(stderr, "'set/getUserData' in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test overwrite
    void* myFirst2 = userTest->setUserData(tempStr, (void*) userSecond, 0);
    void* mySecond = userTest->getUserData(tempStr);
    if (userSecond != mySecond) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (userFirst != myFirst2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (userFirst == mySecond) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test null
    // test non-exist key
    XMLString::transcode("not-exist", tempStr3, 3999);
    if (userTest->getUserData(tempStr3)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }

    // use a node that does not have user data set before
    if (abc51->getUserData(tempStr)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test reset(reset)
    void* mySecond2 = userTest->setUserData(tempStr, (void*) 0, 0);
    void* myNull = userTest->getUserData(tempStr);
    if (userSecond != mySecond2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (myNull) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }

    void* mydocument2 = userTest->setUserData(tempStr2, (void*) 0, 0);
    void* myNull2 = userTest->getUserData(tempStr2);
    if (mydocument != mydocument2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (myNull2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }

    //the userTest user data table should be null now
    if (userTest->getUserData(tempStr)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (userTest->getUserData(tempStr2)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test DOMUserDataHandler
    // test clone
    userTest->setUserData(tempStr2, (void*) document, &userhandler);
    DOMNode* mycloned = userTest->cloneNode(true);
    USERDATAHANDLERTEST(userhandler, DOMUserDataHandler::NODE_CLONED, tempStr2, document, userTest, mycloned, __LINE__);

    // test import
    document->setUserData(tempStr2, (void*) document, &userhandler);
    DOMNode* myimport = document->importNode(userTest,true);
    USERDATAHANDLERTEST(userhandler, DOMUserDataHandler::NODE_IMPORTED, tempStr2, document, userTest, myimport, __LINE__);

    // test delete
    myimport->setUserData(tempStr2, (void*) userTest, &userhandler);
    myimport->release();
    USERDATAHANDLERTEST(userhandler, DOMUserDataHandler::NODE_DELETED, tempStr2, userTest, 0, 0, __LINE__);

    // Test isSameNode
    if (!userTest->isSameNode(userTest)) {
        fprintf(stderr, "isSameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (userTest->isSameNode(userFirst)) {
        fprintf(stderr, "isSameNode failed in line %i\n", __LINE__);
        OK = false;
    }

    // Test isEqualNode
    if (!userTest->isEqualNode(mycloned)) {
        fprintf(stderr, "isEqualNode failed in line %i\n", __LINE__);
        OK = false;
    }

    if (!userTest->isEqualNode(userTest)) {
        fprintf(stderr, "isEqualNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (userTest->isEqualNode(document)) {
        fprintf(stderr, "isEqualNode failed in line %i\n", __LINE__);
        OK = false;
    }

    // Test rename, should throw exception
    EXCEPTIONSTEST(document->renameNode(userTest, 0, tempStr2), DOMException::NOT_SUPPORTED_ERR, OK, 213 );

    if (!OK)
        printf("\n*****The PI method calls listed above failed, all others worked correctly.*****\n");

    return OK;
}



/**
 * This method tests DOMText* methods for the XML DOM implementation
 * @param document org.w3c.dom.DOMDocument
 *
 */
bool DOMTest::testText(DOMDocument* document)
{
    DOMNode* node, *node2;
    DOMText* text;
    bool OK = true;
// For debugging*****   printf("\n          testText's outputs:\n");
    XMLString::transcode("dBodyLevel31", tempStr, 3999);
    DOMNode*  abc70 = document->getDocumentElement()->getElementsByTagName(tempStr)->item(0);
    DOMElement* elem = (DOMElement*) abc70;
    node = elem->getFirstChild(); // charData gets textNode11
    text = (DOMText*) node;
    node2 = node->cloneNode(true);//*****?
    // Check nodes for equality, both their name and value or lack thereof
    if (!(!XMLString::compareString(node->getNodeName(), node2->getNodeName()) &&        // Compares node names for equality
          (node->getNodeValue() != 0 && node2->getNodeValue() != 0)     // Checks to make sure each node has a value node
        ? !XMLString::compareString(node->getNodeValue(), node2->getNodeValue())         // If both have value nodes test those value nodes for equality
        : (node->getNodeValue() == 0 && node2->getNodeValue() == 0)))   // If one node doesn't have a value node make sure both don't
    {
        fprintf(stderr, "'cloneNode' did not clone the DOMText* node correctly\n");
        OK = false;
    }
    // Deep clone test comparison is in testNode & testDocument

    text->splitText(25);
    // Three original text nodes were concatenated by 'normalize' in testElement
    XMLString::transcode("dBodyLevel31'sChildTextNo", tempStr, 3999);
    if (XMLString::compareString(tempStr, text->getNodeValue()))
        {
            fprintf(stderr, "First part of DOMText's split text failed!\n" );
            OK = false;
        }
    // Three original text nodes were concatenated by 'normalize' in testElement
    XMLString::transcode("de11dBodyLevel31'sChildTextNode12dBodyLevel31'sChildTextNode13", tempStr, 3999);
    if (XMLString::compareString(tempStr, text->getNextSibling()->getNodeValue()))
        {
            fprintf(stderr, "The second part of DOMText's split text failed!\n") ;
            OK = false;
        }

    // Re-normalize the text nodes under elem, so that this test can be rerun->
    elem->normalize();


//************************************************* ERROR TESTS
    DOMTest tests;
    //!! Throws INDEX_SIZE_ERR ********************
    //  text.splitText(-1);
    //  text.splitText(100);

    // Test the user data
    // Test simple set and get
    DOMText* userTest = text;
    DOMNode*  userFirst = node2;
    XMLCh* userSecond = tempStr2;
    XMLString::transcode("first", tempStr, 3999);

    XMLString::transcode("document", tempStr2, 3999);
    userTest->setUserData(tempStr2, (void*) document, 0);
    void* mydocument = userTest->getUserData(tempStr2);
    if (document != mydocument) {
        fprintf(stderr, "'set/getUserData' in line %i does not work\n", __LINE__);
        OK = false;
    }

    userTest->setUserData(tempStr, (void*) userFirst, 0);
    void* myFirst = userTest->getUserData(tempStr);
    if (userFirst != myFirst) {
        fprintf(stderr, "'set/getUserData' in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test overwrite
    void* myFirst2 = userTest->setUserData(tempStr, (void*) userSecond, 0);
    void* mySecond = userTest->getUserData(tempStr);
    if (userSecond != mySecond) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (userFirst != myFirst2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (userFirst == mySecond) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test null
    // test non-exist key
    XMLString::transcode("not-exist", tempStr3, 3999);
    if (userTest->getUserData(tempStr3)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }

    // use a node that does not have user data set before
    if (userFirst->getUserData(tempStr)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test reset
    void* mySecond2 = userTest->setUserData(tempStr, (void*) 0, 0);
    void* myNull = userTest->getUserData(tempStr);
    if (userSecond != mySecond2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (myNull) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }

    void* mydocument2 = userTest->setUserData(tempStr2, (void*) 0, 0);
    void* myNull2 = userTest->getUserData(tempStr2);
    if (mydocument != mydocument2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (myNull2) {
        fprintf(stderr, "overwrite userdata with same key in line %i does not work\n", __LINE__);
        OK = false;
    }

    //the userTest user data table should be null now
    if (userTest->getUserData(tempStr)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }
    if (userTest->getUserData(tempStr2)) {
        fprintf(stderr, "get non-exist user data in line %i does not work\n", __LINE__);
        OK = false;
    }

    // Test DOMUserDataHandler
    // test clone
    userTest->setUserData(tempStr2, (void*) document, &userhandler);
    DOMNode* mycloned = userTest->cloneNode(true);
    USERDATAHANDLERTEST(userhandler, DOMUserDataHandler::NODE_CLONED, tempStr2, document, userTest, mycloned, __LINE__);

    // test import
    document->setUserData(tempStr2, (void*) document, &userhandler);
    DOMNode* myimport = document->importNode(userTest,true);
    USERDATAHANDLERTEST(userhandler, DOMUserDataHandler::NODE_IMPORTED, tempStr2, document, userTest, myimport, __LINE__);

    // test delete
    myimport->setUserData(tempStr2, (void*) userTest, &userhandler);
    myimport->release();
    USERDATAHANDLERTEST(userhandler, DOMUserDataHandler::NODE_DELETED, tempStr2, userTest, 0, 0, __LINE__);

    // Test isSameNode
    if (!userTest->isSameNode(userTest)) {
        fprintf(stderr, "isSameNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (userTest->isSameNode(userFirst)) {
        fprintf(stderr, "isSameNode failed in line %i\n", __LINE__);
        OK = false;
    }

    // Test isEqualNode
    if (!userTest->isEqualNode(mycloned)) {
        fprintf(stderr, "isEqualNode failed in line %i\n", __LINE__);
        OK = false;
    }

    if (!userTest->isEqualNode(userTest)) {
        fprintf(stderr, "isEqualNode failed in line %i\n", __LINE__);
        OK = false;
    }
    if (userTest->isEqualNode(abc70)) {
        fprintf(stderr, "isEqualNode failed in line %i\n", __LINE__);
        OK = false;
    }

    // Test rename, should throw exception
    EXCEPTIONSTEST(document->renameNode(userTest, 0, tempStr2), DOMException::NOT_SUPPORTED_ERR, OK, 214 );

    if (!OK)
        printf("\n*****The DOMText* method calls listed above failed, all others worked correctly.*****\n");

    return OK;
}


/**
 * This method tests setting the DOM Level 3 baseURI attribute at
 * parse time on nodes from the document personal-schema.xml.xml
 *
 */

bool DOMTest::testBaseURI(XercesDOMParser* parser) {

    bool OK = true;

    try {
        // this one assumes executing in samples/data where personal-schema.xml resides
        // please modify if this is not correct
        parser->parse("personal-schema.xml");
    }
    catch (const OutOfMemoryException&)
    {
	    fprintf(stderr, "OutOfMemoryException.\n");
	    return false;
    }
    catch (...) {
        fprintf(stderr, "parsing personal-schema.xml failed at line %i\n", __LINE__);
        return false;
    }

    // test only if there is no error
    if (!parser->getErrorCount()) {

        //Setup testing strings
        XMLCh *fileSchema = XMLString::transcode("file://");
        XMLCh *filePath = XMLString::transcode("samples/data/personal-schema.xml");

        //Test document baseURI
        DOMDocument *document = parser->getDocument();

        //The baseURI should contain `file://' and `samples/data/personal-schema.xml'
        const XMLCh *docBaseURI = document->getBaseURI();

        if(XMLString::patternMatch(docBaseURI, fileSchema) == -1) {
            OK = false;
            fprintf(stderr, "checking baseURI failed at line %i\n", __LINE__);
        }

        if(XMLString::patternMatch(docBaseURI, filePath) == -1) {
            OK = false;
            fprintf(stderr, "checking baseURI failed at line %i\n", __LINE__);
        }

        //Create relative paths from document baseURI

        XMLCh *docBaseURIRoot = new XMLCh [ XMLString::stringLen(docBaseURI) ];
        XMLString::copyNString(docBaseURIRoot, docBaseURI, XMLString::lastIndexOf(docBaseURI, chForwardSlash) + 1);

        XMLCh *base_foobar = new XMLCh [ XMLString::stringLen(docBaseURIRoot) + 8];
        XMLString::copyString(base_foobar, docBaseURIRoot);
        XMLCh *foobar = XMLString::transcode("foo/bar");
        XMLString::catString(base_foobar, foobar);

        XMLCh *base_foobarbar = new XMLCh [ XMLString::stringLen(docBaseURIRoot) + 12];
        XMLString::copyString(base_foobarbar, docBaseURIRoot);
        XMLCh *foobarbar = XMLString::transcode("foo/bar/bar");
        XMLString::catString(base_foobarbar, foobarbar);

        XMLCh *base_foocarbar = new XMLCh [ XMLString::stringLen(docBaseURIRoot) + 12];
        XMLString::copyString(base_foocarbar, docBaseURIRoot);
        XMLCh *foocarbar = XMLString::transcode("foo/car/bar");
        XMLString::catString(base_foocarbar, foocarbar);

        XMLCh *file_autobar = XMLString::transcode("file:///auto/bar");
        XMLCh *file_carfoo = XMLString::transcode("file:///car/foo/");
        XMLCh *file_carfoobarbar = XMLString::transcode("file:///car/foo/bar/bar");

        XMLCh *http_carcar = XMLString::transcode("http://www.example.com/car/car");
        XMLCh *http_barfoo = XMLString::transcode("http://www.example.com/bar/foo/");
        XMLCh *http_barfoofoobar = XMLString::transcode("http://www.example.com/bar/foo/foo/bar");

        //Processing instruction before Document Element (has document baseURI)

        DOMNode *node = document->getFirstChild();
        while(node->getNodeType() != DOMNode::PROCESSING_INSTRUCTION_NODE)
            node = node->getNextSibling();

        if(XMLString::compareString(node->getBaseURI(), docBaseURI) != 0) {
            OK = false;
            fprintf(stderr, "checking baseURI failed at line %i\n",  __LINE__);
        }

        //Document Element baseURI (same as document)

        node = document->getDocumentElement();

        if(XMLString::compareString(node->getBaseURI(), docBaseURI) != 0) {
            OK = false;
            fprintf(stderr, "checking baseURI, failed at line %i\n", __LINE__);
        }

        // <level 1>

        node = node->getFirstChild();
        while(node->getNodeType() != DOMNode::ELEMENT_NODE)
            node = node->getNextSibling();

        DOMNode *level1 = node;

        // <one>

        node = node->getFirstChild();
        while(node->getNodeType() != DOMNode::ELEMENT_NODE)
            node = node->getNextSibling();

        if(XMLString::compareString(node->getBaseURI(), base_foobar) != 0) {
            OK = false;
            fprintf(stderr, "checking baseURI failed at line %i\n",  __LINE__);
        }

        // <inner1>

        node = node->getFirstChild();
        while(node->getNodeType() != DOMNode::ELEMENT_NODE)
            node = node->getNextSibling();

        if(XMLString::compareString(node->getBaseURI(), base_foobarbar) != 0) {
            OK = false;
            fprintf(stderr, "checking baseURI failed at line %i\n",  __LINE__);
        }

        // <inner2>

        node = node->getNextSibling();
        while(node->getNodeType() != DOMNode::ELEMENT_NODE)
            node = node->getNextSibling();

        if(XMLString::compareString(node->getBaseURI(), base_foocarbar) != 0) {
            OK = false;
            fprintf(stderr, "checking baseURI failed at line %i\n",  __LINE__);
        }

        // <?proc-inst-2?>

        node = node->getNextSibling();
        while(node->getNodeType() != DOMNode::PROCESSING_INSTRUCTION_NODE)
            node = node->getNextSibling();

        if(XMLString::compareString(node->getBaseURI(), base_foobar) != 0) {
            OK = false;
            fprintf(stderr, "checking baseURI failed at line %i\n",  __LINE__);
        }

        // <level2>

        node = level1->getNextSibling();
        while(node->getNodeType() != DOMNode::ELEMENT_NODE)
            node = node->getNextSibling();

        DOMNode *level2 = node;

        if(XMLString::compareString(node->getBaseURI(), file_autobar) != 0) {
            OK = false;
            fprintf(stderr, "checking baseURI failed at line %i\n",  __LINE__);
        }

        // <two>

        node = node->getFirstChild();
        while(node->getNodeType() != DOMNode::ELEMENT_NODE)
            node = node->getNextSibling();

        if(XMLString::compareString(node->getBaseURI(), file_carfoo) != 0) {
            OK = false;
            fprintf(stderr, "checking baseURI failed at line %i\n",  __LINE__);
        }

        // <inner1>

        node = node->getFirstChild();
        while(node->getNodeType() != DOMNode::ELEMENT_NODE)
            node = node->getNextSibling();

        if(XMLString::compareString(node->getBaseURI(), file_carfoobarbar) != 0) {
            OK = false;
            fprintf(stderr, "checking baseURI failed at line %i\n",  __LINE__);
        }

        // <level3>

        node = level2->getNextSibling();
        while(node->getNodeType() != DOMNode::ELEMENT_NODE)
            node = node->getNextSibling();

        if(XMLString::compareString(node->getBaseURI(), http_carcar) != 0) {
            OK = false;
            fprintf(stderr, "checking baseURI failed at line %i\n",  __LINE__);
        }

        // <three>

        node = node->getFirstChild();
        while(node->getNodeType() != DOMNode::ELEMENT_NODE)
            node = node->getNextSibling();

        if(XMLString::compareString(node->getBaseURI(), http_barfoo) != 0) {
            OK = false;
            fprintf(stderr, "checking baseURI failed at line %i\n",  __LINE__);
        }

        // <inner1>

        node = node->getFirstChild();
        while(node->getNodeType() != DOMNode::ELEMENT_NODE)
            node = node->getNextSibling();

        if(XMLString::compareString(node->getBaseURI(), http_barfoofoobar) != 0) {
            OK = false;
            fprintf(stderr, "checking baseURI failed at line %i\n",  __LINE__);
        }
    }
    else {
        printf("baseURI test was not carried out\n");
    }

    return OK;
}

bool DOMTest::testWholeText(XercesDOMParser* parser) {
	const char* sXml="<?xml version='1.0'?>"
				"<!DOCTYPE root["
                "<!ENTITY ent1 'Dallas. &ent3; #5668'>"
                "<!ENTITY ent2 '1900 Dallas Road<![CDATA[ (East) ]]>'>"
                "<!ENTITY ent3 'California. &ent4; PO'>  "
                "<!ENTITY ent4 'USA '>"
                "<!ENTITY ent5 'The Content &ent6; never reached'>"
                "<!ENTITY ent6 'ends here. <foo/>'>"
                "]>"
                "<root>&ent1; &ent2;"
                  "<elem>Home </elem>"
                  "<elem>Test: &ent5;</elem>"
                "</root>";
	MemBufInputSource is((XMLByte*)sXml, strlen(sXml), "bufId");
	parser->parse(is);
	DOMDocument* doc=parser->getDocument();
	// pointing to "Dallas. "
	DOMNode* t1=doc->getDocumentElement()->getFirstChild()->getFirstChild();
	if(t1==NULL || t1->getNodeType()!=DOMNode::TEXT_NODE)
	{
        fprintf(stderr, "checking wholeText failed at line %i\n",  __LINE__);
		return false;
	}
	const XMLCh* s1=((DOMText*)t1)->getWholeText();
    const XMLCh* refText=XMLString::transcode("Dallas. California. USA  PO #5668 1900 Dallas Road (East) ");
	if(XMLString::compareString(s1,refText)!=0)
	{
        fprintf(stderr, "checking wholeText failed at line %i\n",  __LINE__);
		return false;
	}

    // pointing to " (East) " [CDATA]
	DOMNode* t2=doc->getDocumentElement()->getFirstChild()->getNextSibling()->getNextSibling()->getFirstChild()->getNextSibling();
	if(t2==NULL || t2->getNodeType()!=DOMNode::CDATA_SECTION_NODE)
	{
        fprintf(stderr, "checking wholeText failed at line %i\n",  __LINE__);
		return false;
	}
	const XMLCh* s2=((DOMText*)t2)->getWholeText();
	if(XMLString::compareString(s2,refText)!=0)
	{
        fprintf(stderr, "checking wholeText failed at line %i\n",  __LINE__);
		return false;
	}

    // pointing to "Home "
	DOMNode* t3=doc->getDocumentElement()->getFirstChild()->getNextSibling()->getNextSibling()->getNextSibling()->getFirstChild();
	if(t3==NULL || t3->getNodeType()!=DOMNode::TEXT_NODE)
	{
        fprintf(stderr, "checking wholeText failed at line %i\n",  __LINE__);
		return false;
	}
	const XMLCh* s3=((DOMText*)t3)->getWholeText();
    refText=XMLString::transcode("Home ");
	if(XMLString::compareString(s3,refText)!=0)
	{
        fprintf(stderr, "checking wholeText failed at line %i\n",  __LINE__);
		return false;
	}

    // pointing to "The Content "
	DOMNode* t4=doc->getDocumentElement()->getFirstChild()->getNextSibling()->getNextSibling()->getNextSibling()->getNextSibling()->getFirstChild()->getNextSibling()->getFirstChild();
	if(t4==NULL || t4->getNodeType()!=DOMNode::TEXT_NODE)
	{
        fprintf(stderr, "checking wholeText failed at line %i\n",  __LINE__);
		return false;
	}
	const XMLCh* s4=((DOMText*)t4)->getWholeText();
    refText=XMLString::transcode("Test: The Content ends here. ");
	if(XMLString::compareString(s4,refText)!=0)
	{
        fprintf(stderr, "checking wholeText failed at line %i\n",  __LINE__);
		return false;
	}

    DOMNode* newt2=((DOMText*)t2)->replaceWholeText(s2);
    UNUSED(newt2); //silence warning
    DOMNode* newt3=((DOMText*)t3)->replaceWholeText(s3);
    UNUSED(newt3); //silence warning
    try
    {
        DOMNode* newt4=((DOMText*)t4)->replaceWholeText(s4);
        UNUSED(newt4); //silence warning
        // ent5 contains a <foo/> node, and cannot be removed
        fprintf(stderr, "checking wholeText failed at line %i\n",  __LINE__);
        return false;
    }
    catch(DOMException&)
    {
    }

    DOMLSSerializer* writer=DOMImplementation::getImplementation()->createLSSerializer();
    const XMLCh* xml=writer->writeToString(doc->getDocumentElement());

    refText=XMLString::transcode("<root><![CDATA[Dallas. California. USA  PO #5668 1900 Dallas Road (East) ]]>"
                                   "<elem>Home </elem>"
                                   "<elem>Test: &ent5;</elem>"
                                 "</root>");
	if(XMLString::compareString(xml,refText)!=0)
	{
        fprintf(stderr, "checking wholeText failed at line %i\n",  __LINE__);
		return false;
	}
    writer->release();
    return true;
}

class ParserAborter : public DOMLSParserFilter
{
public:
    ParserAborter() {}

    virtual FilterAction acceptNode(DOMNode* ) { return DOMLSParserFilter::FILTER_INTERRUPT; }
    virtual FilterAction startElement(DOMElement* ) { return DOMLSParserFilter::FILTER_INTERRUPT; }
    virtual DOMNodeFilter::ShowType getWhatToShow() const { return DOMNodeFilter::SHOW_ALL; }
};

class ParserNester : public DOMLSParserFilter
{
public:
    ParserNester(DOMLSParser* parser, DOMLSInput* input) { m_parser=parser; m_input=input; }

    virtual FilterAction acceptNode(DOMNode* ) { m_parser->parse(m_input); return DOMLSParserFilter::FILTER_ACCEPT;}
    virtual FilterAction startElement(DOMElement* ) { return DOMLSParserFilter::FILTER_ACCEPT; }
    virtual DOMNodeFilter::ShowType getWhatToShow() const { return DOMNodeFilter::SHOW_ALL; }

    DOMLSParser* m_parser;
    DOMLSInput* m_input;
};

class ParserSkipper : public DOMLSParserFilter
{
public:
    ParserSkipper() : fCallbackCalls(0) { }

    virtual FilterAction acceptNode(DOMNode* /* node */) { fCallbackCalls++; return DOMLSParserFilter::FILTER_ACCEPT;}
    virtual FilterAction startElement(DOMElement* node) 
    {
        XMLCh elem[]={chLatin_e, chLatin_l, chLatin_e, chLatin_m, chNull };
        if(XMLString::equals(node->getNodeName(), elem))
            return DOMLSParserFilter::FILTER_REJECT; 
        else
            return DOMLSParserFilter::FILTER_ACCEPT; 
    }
    virtual DOMNodeFilter::ShowType getWhatToShow() const { return DOMNodeFilter::SHOW_ALL; }

    unsigned int fCallbackCalls;
};

bool DOMTest::testLSExceptions() {
    bool OK = true;

	const char* sXml="<?xml version='1.0'?>"
				"<!DOCTYPE root["
                "<!ENTITY ent1 'Dallas. &ent3; #5668'>"
                "<!ENTITY ent2 '1900 Dallas Road<![CDATA[ (East) ]]>'>"
                "<!ENTITY ent3 'California. &ent4; PO'>  "
                "<!ENTITY ent4 'USA '>"
                "<!ENTITY ent5 'The Content &ent6; never reached'>"
                "<!ENTITY ent6 'ends here. <foo/>'>"
                "]>"
                "<root>&ent1; &ent2;"
                  "<elem>Home </elem>"
                  "<elem>Test: &ent5;</elem>"
                "</root>";

    static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
    DOMImplementationLS *impl = (DOMImplementationLS*)DOMImplementationRegistry::getDOMImplementation(gLS);
    DOMLSParser       *domBuilder = impl->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
    DOMLSInput        *input = impl->createLSInput();
    XMLString::transcode(sXml, tempStr, 3999);
    input->setStringData(tempStr);
    try
    {
        ParserAborter aborter;
        domBuilder->setFilter(&aborter);
        domBuilder->parse(input);

        fprintf(stderr, "checking testLSExceptions failed at line %i\n",  __LINE__);
        OK=false;
    }
    catch(DOMLSException& e)
    {
        if(e.code!=DOMLSException::PARSE_ERR)
        {
            fprintf(stderr, "checking testLSExceptions failed at line %i\n",  __LINE__);
            OK=false;
        }
    }

    try
    {
        ParserNester nester(domBuilder, input);
        domBuilder->setFilter(&nester);
        domBuilder->parse(input);

        fprintf(stderr, "checking testLSExceptions failed at line %i\n",  __LINE__);
        OK=false;
    }
    catch(DOMException& e)
    {
        if(e.code!=DOMException::INVALID_STATE_ERR)
        {
            fprintf(stderr, "checking testLSExceptions failed at line %i\n",  __LINE__);
            OK=false;
        }
    }

    try
    {
        ParserSkipper skipper;
        domBuilder->setFilter(&skipper);
        domBuilder->getDomConfig()->setParameter(XMLUni::fgDOMEntities, false);
        DOMDocument* doc=domBuilder->parse(input);

        // verify that we get only 3 calls: for the text node, the CDATA section and the root element
        if(doc==NULL || doc->getDocumentElement()==NULL || doc->getDocumentElement()->getChildElementCount()!=0 || skipper.fCallbackCalls!=3)
        {
            fprintf(stderr, "checking testLSExceptions failed at line %i\n",  __LINE__);
            OK=false;
        }
    }
    catch(DOMException&)
    {
        fprintf(stderr, "checking testLSExceptions failed at line %i\n",  __LINE__);
        OK=false;
    }

    // this XML should trigger reuse of DOMElement
	const char* sXml2="<?xml version='1.0'?>"
                "<root xmlns:x='urn:yyy'>"
                  "<elem xmlns:x='urn:xxx'>Home</elem>"
                  "<elem2>Test</elem2>"
                  "<elem>Home</elem>"
                  "<elem2>Test</elem2>"
                "</root>";
    XMLString::transcode(sXml2, tempStr, 3999);
    input->setStringData(tempStr);
    try
    {
        ParserSkipper skipper;
        domBuilder->setFilter(&skipper);
        DOMDocument* doc=domBuilder->parse(input);

        // verify that we get only 5 calls: for the root element, the two elem2 and the two text nodes under them
        if(doc==NULL || doc->getDocumentElement()==NULL || doc->getDocumentElement()->getChildElementCount()!=2 || skipper.fCallbackCalls!=5)
        {
            fprintf(stderr, "checking testLSExceptions failed at line %i\n",  __LINE__);
            OK=false;
        }
    }
    catch(DOMException&)
    {
        fprintf(stderr, "checking testLSExceptions failed at line %i\n",  __LINE__);
        OK=false;
    }

    // test for parseWithContext
    try
    {
        XMLString::transcode("root", tempStr2, 3999);
        domBuilder->setFilter(NULL);
        DOMDocument* doc=domBuilder->parse(input);
        domBuilder->parseWithContext(input, doc->getDocumentElement()->getFirstElementChild(), DOMLSParser::ACTION_APPEND_AS_CHILDREN);
        // the first 'elem' child of 'root' must have a 'root' child
        if(!XMLString::equals(doc->getDocumentElement()->getFirstElementChild()->getFirstElementChild()->getNodeName(), tempStr2))
        {
            fprintf(stderr, "checking testLSExceptions failed at line %i\n",  __LINE__);
            OK=false;
        }

        doc=domBuilder->parse(input);
        domBuilder->parseWithContext(input, doc->getDocumentElement()->getFirstElementChild(), DOMLSParser::ACTION_REPLACE_CHILDREN);
        // the first 'elem' child of 'root' must have a 'root' child
        if(!XMLString::equals(doc->getDocumentElement()->getFirstElementChild()->getFirstElementChild()->getNodeName(), tempStr2))
        {
            fprintf(stderr, "checking testLSExceptions failed at line %i\n",  __LINE__);
            OK=false;
        }

        doc=domBuilder->parse(input);
        domBuilder->parseWithContext(input, doc->getDocumentElement()->getFirstElementChild(), DOMLSParser::ACTION_INSERT_BEFORE);
        // the first child of 'root' must be another 'root' child
        if(!XMLString::equals(doc->getDocumentElement()->getFirstElementChild()->getNodeName(), tempStr2))
        {
            fprintf(stderr, "checking testLSExceptions failed at line %i\n",  __LINE__);
            OK=false;
        }

        doc=domBuilder->parse(input);
        domBuilder->parseWithContext(input, doc->getDocumentElement()->getFirstElementChild(), DOMLSParser::ACTION_INSERT_AFTER);
        // the node after the first child of 'root' must be another 'root' child
        if(!XMLString::equals(doc->getDocumentElement()->getFirstElementChild()->getNextElementSibling()->getNodeName(), tempStr2))
        {
            fprintf(stderr, "checking testLSExceptions failed at line %i\n",  __LINE__);
            OK=false;
        }

        doc=domBuilder->parse(input);
        domBuilder->parseWithContext(input, doc->getDocumentElement()->getFirstElementChild(), DOMLSParser::ACTION_REPLACE);
        // the first child of 'root' must be another 'root' child
        if(!XMLString::equals(doc->getDocumentElement()->getFirstElementChild()->getNodeName(), tempStr2))
        {
            fprintf(stderr, "checking testLSExceptions failed at line %i\n",  __LINE__);
            OK=false;
        }

        // verify that namespaces are in scope
        doc=domBuilder->parse(input);
	    const char* sXml3="<x:root/>";
        XMLString::transcode(sXml3, tempStr2, 3999);
        input->setStringData(tempStr2);
        domBuilder->parseWithContext(input, doc->getDocumentElement()->getFirstElementChild(), DOMLSParser::ACTION_APPEND_AS_CHILDREN);
        // the first 'elem' child of 'root' must have a 'x:root' child
        XMLString::transcode("urn:xxx", tempStr2, 3999);
        if(!XMLString::equals(doc->getDocumentElement()->getFirstElementChild()->getFirstElementChild()->getNamespaceURI(), tempStr2))
        {
            fprintf(stderr, "checking testLSExceptions failed at line %i\n",  __LINE__);
            OK=false;
        }
    }
    catch(DOMException&)
    {
        fprintf(stderr, "checking testLSExceptions failed at line %i\n",  __LINE__);
        OK=false;
    }

    input->release();
    domBuilder->release();

    return OK;
}

bool DOMTest::testElementTraversal() {
	const char* sXml="<?xml version='1.0'?>"
				"<!DOCTYPE g ["
                "<!ENTITY ent1 '<nestedEl>&ent2;</nestedEl>'>"
                "<!ENTITY ent2 'text'>"
                "]>"
				"<g id='shapeGroup'>\n"
                "\n"
                "\t<rect id='rect1' x='5' y='5' width='310' height='220' rx='15' ry='15' fill='skyblue'/>\n"
                "\t<rect id='rect2' x='15' y='15' width='210' height='180' rx='15' ry='15' fill='cornflowerblue'/>\n"
                "\n"
                "\t<ellipse id='ellipse1' cx='90' cy='70' rx='50' ry='30' fill='yellow' stroke='orange'/>\n"
                "\n"
                "\t<path id='path1' stroke-width='15' stroke='orange' fill='none' stroke-linecap='round'\n"
                "\t\td='M25,150 C180,180 290,0 400,140 S420,100 460,90'/>\n"
                "\t<text id='text1' x='0' y='0' font-size='35' fill='yellow' stroke='orange'\n"
                "\t\tstroke-width='2' stroke-linejoin='round' font-weight='bold'>\n"
                "\t\t<textPath id='textPath1' href='#path1'>&ent1;&ent2;&ent1;</textPath></text>\n"
                "</g>";
	MemBufInputSource is((XMLByte*)sXml, strlen(sXml), "bufId");

    static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
    DOMImplementationLS *impl = (DOMImplementationLS*)DOMImplementationRegistry::getDOMImplementation(gLS);
    DOMLSParser       *domBuilder = impl->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
    DOMLSInput        *input = impl->createLSInput();
    XMLString::transcode(sXml, tempStr, 3999);
    input->setStringData(tempStr);
    try
    {
        DOMDocument* doc=domBuilder->parse(input);

        XMLSize_t c = doc->getDocumentElement()->getChildNodes()->getLength();
	    if(c!=11)
	    {
            fprintf(stderr, "checking ElementTraversal failed at line %i\n",  __LINE__);
		    return false;
	    }
        DOMNode* firstNode = doc->getDocumentElement()->getFirstChild();
        if(firstNode==NULL || firstNode->getNodeType()!=DOMNode::TEXT_NODE || *firstNode->getNodeValue()=='\r')
	    {
            fprintf(stderr, "checking ElementTraversal failed at line %i\n",  __LINE__);
		    return false;
	    }
        DOMElement* childNode = doc->getDocumentElement()->getFirstElementChild();
        XMLString::transcode("id", tempStr, 3999);
        XMLString::transcode("rect1", tempStr2, 3999);
        if(childNode==NULL || childNode->getNodeType()!=DOMNode::ELEMENT_NODE || !XMLString::equals(childNode->getAttribute(tempStr),tempStr2))
	    {
            fprintf(stderr, "checking ElementTraversal failed at line %i\n",  __LINE__);
		    return false;
	    }
        XMLSize_t count=0;
        while(childNode!=NULL)
        {
            count++;
            childNode=childNode->getNextElementSibling();
        }
        if(count!=5)
	    {
            fprintf(stderr, "checking ElementTraversal failed at line %i\n",  __LINE__);
		    return false;
	    }
        count = doc->getDocumentElement()->getChildElementCount();
        if(count!=5)
	    {
            fprintf(stderr, "checking ElementTraversal failed at line %i\n",  __LINE__);
		    return false;
	    }
        DOMElement* text=doc->getDocumentElement()->getLastElementChild();
        XMLString::transcode("id", tempStr, 3999);
        XMLString::transcode("text1", tempStr2, 3999);
        if(text==NULL || text->getNodeType()!=DOMNode::ELEMENT_NODE || !XMLString::equals(text->getAttribute(tempStr),tempStr2))
	    {
            fprintf(stderr, "checking ElementTraversal failed at line %i\n",  __LINE__);
		    return false;
	    }
        DOMElement* textPath=text->getFirstElementChild();
        XMLString::transcode("id", tempStr, 3999);
        XMLString::transcode("textPath1", tempStr2, 3999);
        if(textPath==NULL || textPath->getNodeType()!=DOMNode::ELEMENT_NODE || !XMLString::equals(textPath->getAttribute(tempStr),tempStr2))
	    {
            fprintf(stderr, "checking ElementTraversal failed at line %i\n",  __LINE__);
		    return false;
	    }
        count = textPath->getChildElementCount();
        if(count!=2)
	    {
            fprintf(stderr, "checking ElementTraversal failed at line %i\n",  __LINE__);
		    return false;
	    }
        DOMElement* insideEntity=textPath->getFirstElementChild();
        if(insideEntity==NULL || insideEntity->getNodeType()!=DOMNode::ELEMENT_NODE)
	    {
            fprintf(stderr, "checking ElementTraversal failed at line %i\n",  __LINE__);
		    return false;
	    }
        DOMElement* insideEntity2=textPath->getLastElementChild();
        if(insideEntity2==NULL || insideEntity2->getNodeType()!=DOMNode::ELEMENT_NODE)
	    {
            fprintf(stderr, "checking ElementTraversal failed at line %i\n",  __LINE__);
		    return false;
	    }
        if(insideEntity->getNextElementSibling()!=insideEntity2 || insideEntity!=insideEntity2->getPreviousElementSibling())
	    {
            fprintf(stderr, "checking ElementTraversal failed at line %i\n",  __LINE__);
		    return false;
	    }
        return true;
    }
    catch(DOMLSException&)
    {
        fprintf(stderr, "checking testElementTraversal failed at line %i\n",  __LINE__);
        return false;
    }

    input->release();
    domBuilder->release();

    return true;
}

/**
 *
 * @param node org.w3c.dom.DOMNode
 * @param node2 org.w3c.dom.DOMNode
 *
 */
bool DOMTest::treeCompare(DOMNode* node, DOMNode* node2)
{
    bool answer = true;

    DOMNode*  kid, *kid2;         // Check the subtree for equality
    kid = node->getFirstChild();
    kid2 = node2->getFirstChild();
    if (kid && kid2)
    {
        answer = treeCompare(kid, kid2);
        if (!answer)
            return answer;
        else
            if (kid->getNextSibling() && kid2->getNextSibling())
            {
                while (kid->getNextSibling() && kid2->getNextSibling())
                {
                    answer = treeCompare(kid->getNextSibling(), kid2->getNextSibling());
                    if (!answer)
                        return answer;
                    else
                    {
                        kid = kid->getNextSibling();
                        kid2 = kid2->getNextSibling();
                    }
                }
            } else
                if (!(!kid->getNextSibling() && !kid2->getNextSibling()))
                {
                    return false;
                }
    } else
        if (kid != kid2)
        {
            // One or the other of (kid1, kid2) is 0, but not both.
            return false;
        }

    if (XMLString::compareString(node->getNodeName(), node2->getNodeName()))
        return false;
    if (node->getNodeValue()==0 && node2->getNodeValue()!=0)
        return false;
    if (node->getNodeValue()!=0 && node2->getNodeValue()==0)
        return false;
    if (XMLString::compareString(node->getNodeValue(), node2->getNodeValue()))
        return false;

    return answer;
}

#define TEST_VALID_REGEX(str, regex, line) \
    try \
    { \
        RegularExpression p(regex); \
        if(!p.matches(str)) \
        { \
            fprintf(stderr, "Regular expression test failed at line %i\n", line); \
            OK = false; \
        } \
    }   \
    catch(XMLException& ) \
    {   \
        fprintf(stderr, "Regular expression test failed at line %i\n", line); \
        OK = false; \
    }

#define TEST_INVALID_REGEX(str, regex, line) \
    try \
    { \
        RegularExpression p(regex); \
        if(p.matches(str)) \
        { \
            fprintf(stderr, "Regular expression test failed at line %i\n", line); \
            OK = false; \
        } \
    }   \
    catch(XMLException& ) \
    {   \
        fprintf(stderr, "Regular expression test failed at line %i\n", line); \
        OK = false; \
    }

#define TEST_VALID_SCHEMA_REGEX(str, regex, line) \
    try \
    { \
        RegularExpression p(regex, "X"); \
        if(!p.matches(str)) \
        { \
            fprintf(stderr, "Regular expression test failed at line %i\n", line); \
            OK = false; \
        } \
    }   \
    catch(XMLException& ) \
    {   \
        fprintf(stderr, "Regular expression test failed at line %i\n", line); \
        OK = false; \
    }

#define TEST_INVALID_SCHEMA_REGEX(str, regex, line) \
    try \
    { \
        RegularExpression p(regex, "X"); \
        if(p.matches(str)) \
        { \
            fprintf(stderr, "Regular expression test failed at line %i\n", line); \
            OK = false; \
        } \
    }   \
    catch(XMLException& ) \
    {   \
        fprintf(stderr, "Regular expression test failed at line %i\n", line); \
        OK = false; \
    }

#define HUGE_STRING (100*1024)  // 100Kb

bool DOMTest::testRegex() {
    bool OK = true;

    char* hugeString=new char[HUGE_STRING+1];
    for(int i=0;i<HUGE_STRING;i++)
        hugeString[i]='A';
    hugeString[HUGE_STRING]=0;
    TEST_VALID_REGEX(hugeString, "([A-F0-9]{2})*", __LINE__);

    TEST_VALID_REGEX("12_END", "[12]{2}_END", __LINE__);
    TEST_INVALID_REGEX("12", "[12]{2}_END", __LINE__);

    TEST_VALID_REGEX("AE", "(A|B*|C)E", __LINE__);
    TEST_VALID_REGEX("E", "(A|B*|C)E", __LINE__);
    TEST_VALID_REGEX("BBBBBBBBBBBBBE", "(A|B*|C)E", __LINE__);
    TEST_VALID_REGEX("CE", "(A|B*|C)E", __LINE__);
    TEST_INVALID_REGEX("A", "(A|B*|C)E", __LINE__);
    TEST_INVALID_REGEX("", "(A|B*|C)E", __LINE__);
    TEST_INVALID_REGEX("BBBBBBBBBBBBB", "(A|B*|C)E", __LINE__);
    TEST_INVALID_REGEX("C", "(A|B*|C)E", __LINE__);

    // this tests the closures that need fOffset in the context
    for(int j=0;j<HUGE_STRING-1;j++)
        hugeString[j]=j%2?'B':'C';
    hugeString[HUGE_STRING-1]='E';
    // we run out of stack space when testing this one
    //TEST_VALID_REGEX(hugeString, "(B?C?)*E", __LINE__);
    TEST_VALID_REGEX("BBBBE", "(B?C?)*E", __LINE__);
    TEST_VALID_REGEX("E", "(B?C?)*E", __LINE__);
    TEST_VALID_REGEX("CCCCCCCE", "(B?C?)*E", __LINE__);
    TEST_VALID_REGEX("BCBCBBBBCCCCCE", "(B?C?)*E", __LINE__);

    TEST_VALID_REGEX(" C", "[\\s]*[\\S]+[\\s\\S]*", __LINE__);
    TEST_VALID_REGEX("C asasaskja ksakj", "[\\s]*[\\S]+[\\s\\S]*", __LINE__);
    TEST_VALID_REGEX(" C", "[\\s]*[\\S]+[\\s\\S]*", __LINE__);
    TEST_INVALID_REGEX("\t         ", "[\\s]*[\\S]+[\\S]*", __LINE__);

    TEST_VALID_REGEX("U2VsZWN0IFRoaXMgaXMgZmlmdGg7DQogDQpOT1RFOiBUaGUgaW5mb3JtYXRpb24gY29udGFpbmVk\n"
                     "IGluIHRoaXMgZW1haWwgbWVzc2FnZSBpcyBjb25zaWRlcmVkIGNvbmZpZGVudGlhbCBhbmQgcHJv\n"
                     "cHJpZXRhcnkgdG8gdGhlIHNlbmRlciBhbmQgaXMgaW50ZW5kZWQgc29sZWx5IGZvciByZXZpZXcg\n"
                     "YW5kIHVzZSBieSB0aGUgbmFtZWQgcmVjaXBpZW50LiAgQW55IHVuYXV0aG9yaXplZCByZXZpZXcs\n"
                     "IHVzZSBvciBkaXN0cmlidXRpb24gaXMgc3RyaWN0bHkgcHJvaGliaXRlZC4gSWYgeW91IGhhdmUg\n"
                     "cmVjZWl2ZWQgdGhpcyBtZXNzYWdlIGluIGVycm9yLCBwbGVhc2UgYWR2aXNlIHRoZSBzZW5kZXIg\n"
                     "YnkgcmVwbHkgZW1haWwgYW5kIGRlbGV0ZSB0aGUgbWVzc2FnZS4NCg==\n", "(^(([A-Za-z0-9+/=]){4}){1,19}$)*", __LINE__);

    // examples from XMLSchema specs
    TEST_VALID_SCHEMA_REGEX("123 456", "123 (\\d+\\s)*456", __LINE__);
    TEST_VALID_SCHEMA_REGEX("123 987 456", "123 (\\d+\\s)*456", __LINE__);
    TEST_VALID_SCHEMA_REGEX("123 987 567 456", "123 (\\d+\\s)*456", __LINE__);

    TEST_VALID_SCHEMA_REGEX("P0100Y02M", "P\\p{Nd}{4}Y\\p{Nd}{2}M", __LINE__);
    TEST_VALID_SCHEMA_REGEX("en-US", "[a-zA-Z]{1,8}(-[a-zA-Z0-9]{1,8})*", __LINE__);

    TEST_VALID_SCHEMA_REGEX("123-XX", "\\d{3}-[A-Z]{2}", __LINE__);

    TEST_VALID_SCHEMA_REGEX("01803", "[0-9]{5}(-[0-9]{4})?", __LINE__);

    TEST_VALID_SCHEMA_REGEX("_id1", "\\i\\c*", __LINE__);
    TEST_INVALID_SCHEMA_REGEX("0id", "\\i\\c*", __LINE__);

    TEST_VALID_SCHEMA_REGEX("hello", "[\\i-[:]][\\c-[:]]*", __LINE__);
    TEST_INVALID_SCHEMA_REGEX("0qq", "[\\i-[:]][\\c-[:]]*", __LINE__);
    TEST_INVALID_SCHEMA_REGEX("ns:localname", "[\\i-[:]][\\c-[:]]*", __LINE__);
    TEST_INVALID_SCHEMA_REGEX("ns:", "[\\i-[:]][\\c-[:]]*", __LINE__);
    TEST_INVALID_SCHEMA_REGEX(":qq", "[\\i-[:]][\\c-[:]]*", __LINE__);

    TEST_VALID_SCHEMA_REGEX("900", "[\\-+]?[0-9]+", __LINE__);
    TEST_VALID_SCHEMA_REGEX("-900", "[\\-+]?[0-9]+", __LINE__);
    TEST_VALID_SCHEMA_REGEX("+900", "[\\-+]?[0-9]+", __LINE__);
    TEST_INVALID_SCHEMA_REGEX("+", "[\\-+]?[0-9]+", __LINE__);
    TEST_INVALID_SCHEMA_REGEX("-", "[\\-+]?[0-9]+", __LINE__);
    TEST_INVALID_SCHEMA_REGEX("0.4", "[\\-+]?[0-9]+", __LINE__);

    TEST_VALID_SCHEMA_REGEX("ABCDEFGHIJKLMNOPQRSTUVWXYZ", "A.*Z", __LINE__);
    TEST_INVALID_SCHEMA_REGEX("Z", "A.*Z", __LINE__);
    TEST_INVALID_SCHEMA_REGEX("A", "A.*Z", __LINE__);
    TEST_INVALID_SCHEMA_REGEX("ABC", "A.*Z", __LINE__);

    // tests from the XMLSchema Test Suite
    TEST_VALID_SCHEMA_REGEX("Chapter b", "Chapter\\s{0,2}\\w", __LINE__);
    TEST_VALID_SCHEMA_REGEX("Chapter\tb", "Chapter\\s{0,2}\\w", __LINE__);
    TEST_VALID_SCHEMA_REGEX("Chapter\nb", "Chapter\\s{0,2}\\w", __LINE__);

    TEST_VALID_SCHEMA_REGEX("abx", "(a|b)+x", __LINE__);
    TEST_VALID_SCHEMA_REGEX("bax", "(a|b)+x", __LINE__);
    TEST_VALID_SCHEMA_REGEX("bbx", "(a|b)+x", __LINE__);
    TEST_VALID_SCHEMA_REGEX("aaax", "(a|b)+x", __LINE__);
    TEST_VALID_SCHEMA_REGEX("abax", "(a|b)+x", __LINE__);
    TEST_VALID_SCHEMA_REGEX("abbx", "(a|b)+x", __LINE__);
    TEST_VALID_SCHEMA_REGEX("bbax", "(a|b)+x", __LINE__);

    TEST_VALID_SCHEMA_REGEX("1x2abc", ".*abc.*", __LINE__);
    TEST_VALID_SCHEMA_REGEX("abc1x2", ".*abc.*", __LINE__);
    TEST_VALID_SCHEMA_REGEX("z3455abch00ray", ".*abc.*", __LINE__);
    TEST_VALID_SCHEMA_REGEX("bcabcabcab", ".*abc.*", __LINE__);

    TEST_VALID_SCHEMA_REGEX("abbbx", "ab{2,4}x", __LINE__);
    TEST_VALID_SCHEMA_REGEX("abbbbx", "ab{2,4}x", __LINE__);
    TEST_INVALID_SCHEMA_REGEX("abx", "ab{2,4}x", __LINE__);
    TEST_INVALID_SCHEMA_REGEX("abbbbbx", "ab{2,4}x", __LINE__);
    TEST_VALID_SCHEMA_REGEX("PAG_1", "PAG_[0-9]{1,}", __LINE__);

    TEST_VALID_SCHEMA_REGEX("5 Bedford Street Boston , MA 15604-1536", "\\d{1,5}\\s([A-Z][a-z]{1,20}\\s){1}Street\\s([A-Z][a-z]{1,20}\\s){1},\\s[A-Z]{2}\\s15604-1536", __LINE__);

    // from X3D schema
    TEST_VALID_SCHEMA_REGEX("0.5 0.2 1.0", "((((\\.[0-9]+|0(\\.[0-9]*)?)((E|e)(\\+|\\-)?[0-9]+)?)|(1(\\.[0]*)?((E|e)\\-[0-9]+)?)|([1-9](\\.[0-9]*)((E|e)\\-[0-9]+))) (((\\.[0-9]+|0(\\.[0-9]*)?)((E|e)(\\+|\\-)?[0-9]+)?)|(1(\\.[0]*)?((E|e)\\-[0-9]+)?)|([1-9](\\.[0-9]*)((E|e)\\-[0-9]+))) (((\\.[0-9]+|0(\\.[0-9]*)?)((E|e)(\\+|\\-)?[0-9]+)?)|(1(\\.[0]*)?((E|e)\\-[0-9]+)?)|([1-9](\\.[0-9]*)((E|e)\\-[0-9]+))))?", __LINE__);
    TEST_VALID_SCHEMA_REGEX("5.0e-2 .2 1", "((((\\.[0-9]+|0(\\.[0-9]*)?)((E|e)(\\+|\\-)?[0-9]+)?)|(1(\\.[0]*)?((E|e)\\-[0-9]+)?)|([1-9](\\.[0-9]*)((E|e)\\-[0-9]+))) (((\\.[0-9]+|0(\\.[0-9]*)?)((E|e)(\\+|\\-)?[0-9]+)?)|(1(\\.[0]*)?((E|e)\\-[0-9]+)?)|([1-9](\\.[0-9]*)((E|e)\\-[0-9]+))) (((\\.[0-9]+|0(\\.[0-9]*)?)((E|e)(\\+|\\-)?[0-9]+)?)|(1(\\.[0]*)?((E|e)\\-[0-9]+)?)|([1-9](\\.[0-9]*)((E|e)\\-[0-9]+))))?", __LINE__);

    TEST_VALID_SCHEMA_REGEX("-0 +3989 -90.76754,+9E77, -0.3e+9", "(((\\+|\\-)?(0|[1-9][0-9]*)?(\\.[0-9]*)?((E|e)(\\+|\\-)?[0-9]+)?)?( )?(,)?( )?)*", __LINE__);

    try
    {
        Match match;
        RegularExpression p("([\\-\\(]?\\d{1,3}([, ]\\d{3})+\\.\\d+[\\)]?|[\\-\\(]?\\d+\\.\\d+[\\)]?).*");
        if(!p.matches("13.13", &match) || match.getStartPos(0)!=0 || match.getEndPos(0)!=5)
        {
            fprintf(stderr, "Regular expression test failed at line %i\n", __LINE__);
            OK = false;
        }
    }
    catch(XMLException& )
    {
        fprintf(stderr, "Regular expression test failed at line %i\n", __LINE__);
        OK = false;
    }

    delete [] hugeString;

    return OK;
}


// support classes to feed data with variable chunks

class Slicer : public BinInputStream
{
public:
	Slicer(const XMLByte* src, XMLSize_t size, const XMLSize_t* slices = 0, XMLSize_t count = 0)
	: mSrc(src), mSize(size), mSlices(slices), mCount(count), mPos(0), mSlice(0), mBoundary(0)
	{
	}

	XMLFilePos curPos() const
	{
		return mPos;
	}

	XMLSize_t readBytes(XMLByte* const toFill, const XMLSize_t maxToRead)
	{
		if (mPos == mBoundary)
		{
			if (mSlice < mCount)
			{
                XMLSize_t next = mBoundary + mSlices[mSlice++];
                mBoundary = (mSize<next)?mSize:next;
			}
			else
			{
				mBoundary = mSize;
			}
		}
        XMLSize_t remain = mBoundary - mPos;
        XMLSize_t toRead = (maxToRead<remain)?maxToRead:remain;
		memcpy(toFill, mSrc + mPos, toRead);
		mPos += toRead;
		return toRead;
	}

    virtual const XMLCh* getContentType() const
    {
        return 0;
    }

private:

	const XMLByte* const    mSrc;
	const XMLSize_t         mSize;
	const XMLSize_t* const  mSlices;
	const XMLSize_t         mCount;

	XMLSize_t               mPos, mSlice;
	XMLSize_t               mBoundary;
};


class SlicerSource : public InputSource
{
public:
	SlicerSource(const XMLByte* src, XMLSize_t size, const XMLSize_t* slices = 0, XMLSize_t count = 0)
	: mSrc(src), mSize(size), mSlices(slices), mCount(count)
	{
	}

	XERCES_CPP_NAMESPACE::BinInputStream* makeStream() const
	{
		return new Slicer(mSrc, mSize, mSlices, mCount);
	}

private:
	const XMLByte* const    mSrc;
	const XMLSize_t         mSize;
	const XMLSize_t* const  mSlices;
	const XMLSize_t         mCount;
};

bool DOMTest::testScanner(XercesDOMParser* parser) {
    bool OK = true;

    const char sampleDoc[] =
	    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
	    "<!-- Document element is document -->\n"
	    "<document attr=\"value\">\n"
	    "   <!-- a tag -->\n"
	    "   <tag>foo</tag>\n"
	    "\n"
	    "   <!-- another tag -->\n"
	    "   <tag/>\n"
	    "</document>\n";
    const size_t sampleDocSize = sizeof(sampleDoc) - 1;
    const XMLByte* sampleDocXML = reinterpret_cast<const XMLByte*>(sampleDoc);

    try
	{
		// First, try parsing the document in one hit
		SlicerSource src(sampleDocXML, sampleDocSize);
		parser->parse(src);
	}
	catch (...)
	{
        OK = false;
        fprintf(stderr, "Variable chunks parsing failed at line %i\n", __LINE__);
	}

	try
	{
		// Now, parse it in blocks that end between the '<',  '!' and '--' of a comment
		const XMLSize_t slices[] = { 142, 1 };
		const XMLSize_t count = sizeof(slices) / sizeof(slices[0]);
		SlicerSource src(sampleDocXML, sampleDocSize, slices, count);
		parser->parse(src);
	}
	catch (...)
	{
        OK = false;
        fprintf(stderr, "Variable chunks parsing failed at line %i\n", __LINE__);
	}

	try
	{
		// Now, parse it in blocks that end between the '<',  '!-' and '-' of a comment
		const XMLSize_t slices[] = { 142, 2 };
		const XMLSize_t count = sizeof(slices) / sizeof(slices[0]);
		SlicerSource src(sampleDocXML, sampleDocSize, slices, count);
		parser->parse(src);
	}
	catch (...)
	{
        OK = false;
        fprintf(stderr, "Variable chunks parsing failed at line %i\n", __LINE__);
	}

	try
	{
		// Now, parse it in blocks that end between the '<',  '!-' and '-' of a comment
		static const XMLSize_t slices[] =
        {
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1};
		static const XMLSize_t count = sizeof(slices) / sizeof(slices[0]);
		SlicerSource src(sampleDocXML, sampleDocSize, slices, count);
		parser->parse(src);
	}
	catch (...)
	{
        OK = false;
        fprintf(stderr, "Variable chunks parsing failed at line %i\n", __LINE__);
	}

    return OK;
}

#define TEST_BOOLEAN(x) \
    if(!x)  \
    {       \
        fprintf(stderr, "Boolean expression test failed at line %i\n", __LINE__); \
        OK = false; \
    }

#define TEST_STRING(x,y) \
    if(!XMLString::equals(x,y))  \
    {       \
        fprintf(stderr, "String expression test failed at line %i\n", __LINE__); \
        OK = false; \
    }

bool DOMTest::testUtilFunctions()
{
    bool OK = true;
    // test isWSReplaced
    XMLString::transcode(" xyz ", tempStr, 3999);
    TEST_BOOLEAN(XMLString::isWSReplaced(tempStr));
    XMLString::transcode(" x\tyz ", tempStr, 3999);
    TEST_BOOLEAN(!XMLString::isWSReplaced(tempStr));
    XMLString::transcode(" xyz\n", tempStr, 3999);
    TEST_BOOLEAN(!XMLString::isWSReplaced(tempStr));
    XMLString::transcode("\rxyz", tempStr, 3999);
    TEST_BOOLEAN(!XMLString::isWSReplaced(tempStr));

    // test replaceWS
    XMLString::transcode(" x yz ", tempStr2, 3999);
    XMLString::transcode(" x yz ", tempStr, 3999);
    XMLString::replaceWS(tempStr);
    TEST_STRING(tempStr, tempStr2);
    XMLString::transcode(" x\tyz ", tempStr, 3999);
    XMLString::replaceWS(tempStr);
    TEST_STRING(tempStr, tempStr2);
    XMLString::transcode(" x yz\n", tempStr, 3999);
    XMLString::replaceWS(tempStr);
    TEST_STRING(tempStr, tempStr2);
    XMLString::transcode("\rx yz ", tempStr, 3999);
    XMLString::replaceWS(tempStr);
    TEST_STRING(tempStr, tempStr2);

    // test isWSCollapsed
    XMLString::transcode(" xyz ", tempStr, 3999);
    TEST_BOOLEAN(!XMLString::isWSCollapsed(tempStr));
    XMLString::transcode(" x\tyz ", tempStr, 3999);
    TEST_BOOLEAN(!XMLString::isWSCollapsed(tempStr));
    XMLString::transcode(" xyz\n", tempStr, 3999);
    TEST_BOOLEAN(!XMLString::isWSCollapsed(tempStr));
    XMLString::transcode("\rxyz", tempStr, 3999);
    TEST_BOOLEAN(!XMLString::isWSCollapsed(tempStr));
    XMLString::transcode("xyz", tempStr, 3999);
    TEST_BOOLEAN(XMLString::isWSCollapsed(tempStr));
    XMLString::transcode("x yz", tempStr, 3999);
    TEST_BOOLEAN(XMLString::isWSCollapsed(tempStr));
    XMLString::transcode("x  yz", tempStr, 3999);
    TEST_BOOLEAN(!XMLString::isWSCollapsed(tempStr));

    // test collapseWS
    XMLString::transcode("x yz", tempStr2, 3999);
    XMLString::transcode(" x\tyz ", tempStr, 3999);
    XMLString::collapseWS(tempStr);
    TEST_STRING(tempStr, tempStr2);
    XMLString::transcode("x yz", tempStr, 3999);
    XMLString::collapseWS(tempStr);
    TEST_STRING(tempStr, tempStr2);
    XMLString::transcode("x  yz", tempStr, 3999);
    XMLString::collapseWS(tempStr);
    TEST_STRING(tempStr, tempStr2);

    XMLString::transcode("xyz", tempStr2, 3999);
    XMLString::transcode(" xyz ", tempStr, 3999);
    XMLString::collapseWS(tempStr);
    TEST_STRING(tempStr, tempStr2);
    XMLString::transcode(" xyz\n", tempStr, 3999);
    XMLString::collapseWS(tempStr);
    TEST_STRING(tempStr, tempStr2);
    XMLString::transcode("\rxyz", tempStr, 3999);
    XMLString::collapseWS(tempStr);
    TEST_STRING(tempStr, tempStr2);
    XMLString::transcode("xyz", tempStr, 3999);
    XMLString::collapseWS(tempStr);
    TEST_STRING(tempStr, tempStr2);

    // test removeWS
    XMLString::transcode("xyz", tempStr2, 3999);
    XMLString::transcode(" x\tyz ", tempStr, 3999);
    XMLString::removeWS(tempStr);
    TEST_STRING(tempStr, tempStr2);
    XMLString::transcode("x yz", tempStr, 3999);
    XMLString::removeWS(tempStr);
    TEST_STRING(tempStr, tempStr2);
    XMLString::transcode("x  yz", tempStr, 3999);
    XMLString::removeWS(tempStr);
    TEST_STRING(tempStr, tempStr2);
    XMLString::transcode(" xyz ", tempStr, 3999);
    XMLString::removeWS(tempStr);
    TEST_STRING(tempStr, tempStr2);
    XMLString::transcode(" xyz\n", tempStr, 3999);
    XMLString::removeWS(tempStr);
    TEST_STRING(tempStr, tempStr2);
    XMLString::transcode("\rxyz", tempStr, 3999);
    XMLString::removeWS(tempStr);
    TEST_STRING(tempStr, tempStr2);
    XMLString::transcode("xyz", tempStr, 3999);
    XMLString::removeWS(tempStr);
    TEST_STRING(tempStr, tempStr2);

    if(XMLString::stringLen((XMLCh*)0)!=0)
    {
        fprintf(stderr, "strLen test failed at line %i\n", __LINE__);
        OK = false;
    }
    if(XMLString::stringLen(XMLUni::fgZeroLenString)!=0)
    {
        fprintf(stderr, "strLen test failed at line %i\n", __LINE__);
        OK = false;
    }
    XMLCh one[2]={ chLatin_A, chNull };
    if(XMLString::stringLen(one)!=1)
    {
        fprintf(stderr, "strLen test failed at line %i\n", __LINE__);
        OK = false;
    }
    XMLCh two[3]={ chLatin_A, chLatin_B, chNull };
    if(XMLString::stringLen(two)!=2)
    {
        fprintf(stderr, "strLen test failed at line %i\n", __LINE__);
        OK = false;
    }

    // test copyNString
    XMLCh buffer[100];
    XMLString::transcode("xyz", tempStr, 3999);
    if(!XMLString::copyNString(buffer, tempStr, 100))
    {
        fprintf(stderr, "copyNString test failed at line %i\n", __LINE__);
        OK = false;
    }
    if(!XMLString::copyNString(buffer, tempStr, 3))
    {
        fprintf(stderr, "copyNString test failed at line %i\n", __LINE__);
        OK = false;
    }
    if(XMLString::copyNString(buffer, tempStr, 2))
    {
        fprintf(stderr, "copyNString test failed at line %i\n", __LINE__);
        OK = false;
    }
    if(!XMLString::copyNString(buffer, tempStr, 4))
    {
        fprintf(stderr, "copyNString test failed at line %i\n", __LINE__);
        OK = false;
    }

    // test indexOf
    XMLString::transcode("1234567890", tempStr, 3999);
    if(XMLString::indexOf(tempStr, '1')!=0)
    {
        fprintf(stderr, "indexOf test failed at line %i\n", __LINE__);
        OK = false;
    }
    if(XMLString::indexOf(tempStr, '5')!=4)
    {
        fprintf(stderr, "indexOf test failed at line %i\n", __LINE__);
        OK = false;
    }
    if(XMLString::indexOf(tempStr, '0')!=9)
    {
        fprintf(stderr, "indexOf test failed at line %i\n", __LINE__);
        OK = false;
    }
    if(XMLString::indexOf(tempStr, 'A')!=-1)
    {
        fprintf(stderr, "indexOf test failed at line %i\n", __LINE__);
        OK = false;
    }

    // test lastIndexOf
    XMLString::transcode("1234567890", tempStr, 3999);
    if(XMLString::lastIndexOf(tempStr, '1')!=0)
    {
        fprintf(stderr, "lastIndexOf test failed at line %i\n", __LINE__);
        OK = false;
    }
    if(XMLString::lastIndexOf(tempStr, '5')!=4)
    {
        fprintf(stderr, "lastIndexOf test failed at line %i\n", __LINE__);
        OK = false;
    }
    if(XMLString::lastIndexOf(tempStr, '0')!=9)
    {
        fprintf(stderr, "lastIndexOf test failed at line %i\n", __LINE__);
        OK = false;
    }
    if(XMLString::lastIndexOf(tempStr, 'A')!=-1)
    {
        fprintf(stderr, "lastIndexOf test failed at line %i\n", __LINE__);
        OK = false;
    }

    // this tests the cached bit storage
    CMStateSet setT(60);
    setT.setBit(8);
    setT.setBit(52);
    setT.setBit(34);

    if(!setT.getBit(8) || !setT.getBit(52) || !setT.getBit(34))
    {
        fprintf(stderr, "bitset test failed at line %i\n", __LINE__);
        OK = false;
    }

    CMStateSetEnumerator enumT(&setT);
    if(!enumT.hasMoreElements() || enumT.nextElement()!=8)
    {
        fprintf(stderr, "bitset test failed at line %i\n", __LINE__);
        OK = false;
    }
    if(!enumT.hasMoreElements() || enumT.nextElement()!=34)
    {
        fprintf(stderr, "bitset test failed at line %i\n", __LINE__);
        OK = false;
    }
    if(!enumT.hasMoreElements() || enumT.nextElement()!=52)
    {
        fprintf(stderr, "bitset test failed at line %i\n", __LINE__);
        OK = false;
    }
    if(enumT.hasMoreElements())
    {
        fprintf(stderr, "bitset test failed at line %i\n", __LINE__);
        OK = false;
    }

    // this tests the dynamic bit storage
    CMStateSet setT2(3 * CMSTATE_BITFIELD_CHUNK);
    setT2.setBit(0); // first block, begin
    setT2.setBit(CMSTATE_BITFIELD_CHUNK/2 -1); // first block, middle
    setT2.setBit(CMSTATE_BITFIELD_CHUNK/2); // first block, middle
    setT2.setBit(CMSTATE_BITFIELD_CHUNK/2 +1); // first block, middle
    setT2.setBit(CMSTATE_BITFIELD_CHUNK-1); // first block, end
    setT2.setBit(2*CMSTATE_BITFIELD_CHUNK); // last block, begin
    setT2.setBit(2*CMSTATE_BITFIELD_CHUNK + CMSTATE_BITFIELD_CHUNK/2 -1); // last block, middle
    setT2.setBit(2*CMSTATE_BITFIELD_CHUNK + CMSTATE_BITFIELD_CHUNK/2); // last block, middle
    setT2.setBit(2*CMSTATE_BITFIELD_CHUNK + CMSTATE_BITFIELD_CHUNK/2 +1); // last block, middle
    setT2.setBit(3*CMSTATE_BITFIELD_CHUNK-1); // last block, end

    // test just a few ones
    if(!setT2.getBit(0) || !setT2.getBit(CMSTATE_BITFIELD_CHUNK-1) || !setT2.getBit(2*CMSTATE_BITFIELD_CHUNK + CMSTATE_BITFIELD_CHUNK/2 +1))
    {
        fprintf(stderr, "bitset test failed at line %i\n", __LINE__);
        OK = false;
    }

    if(setT2.getBitCountInRange(0, 3*CMSTATE_BITFIELD_CHUNK)!=10)
    {
        fprintf(stderr, "bitset test failed at line %i\n", __LINE__);
        OK = false;
    }
    CMStateSetEnumerator enumT2(&setT2);
    XMLSize_t count=0;
    while(enumT2.hasMoreElements())
    {
        count++;
        enumT2.nextElement();
    }
    if(count!=10)
    {
        fprintf(stderr, "bitset test failed at line %i\n", __LINE__);
        OK = false;
    }

    // test the enumerator with a non-default start
    CMStateSetEnumerator enumT2a(&setT2, CMSTATE_BITFIELD_CHUNK/2);
    if(!enumT2a.hasMoreElements() || enumT2a.nextElement()!= (CMSTATE_BITFIELD_CHUNK/2))
    {
        fprintf(stderr, "bitset test failed at line %i\n", __LINE__);
        OK = false;
    }
    CMStateSetEnumerator enumT2b(&setT2, CMSTATE_BITFIELD_CHUNK/2+2);
    if(!enumT2b.hasMoreElements() || enumT2b.nextElement()!= (CMSTATE_BITFIELD_CHUNK-1))
    {
        fprintf(stderr, "bitset test failed at line %i\n", __LINE__);
        OK = false;
    }
    CMStateSetEnumerator enumT2c(&setT2, 2*CMSTATE_BITFIELD_CHUNK);
    if(!enumT2c.hasMoreElements() || enumT2c.nextElement()!= (2*CMSTATE_BITFIELD_CHUNK))
    {
        fprintf(stderr, "bitset test failed at line %i\n", __LINE__);
        OK = false;
    }

    // this tests the hash generator
    CMStateSet setT3(3 * CMSTATE_BITFIELD_CHUNK), setT4(3 * CMSTATE_BITFIELD_CHUNK);
    // these two sets will have a single bit set at the beginning of a chunk
    setT3.setBit(0);
    setT4.setBit(CMSTATE_BITFIELD_CHUNK);
    if(setT3.hashCode()==setT4.hashCode())
    {
        fprintf(stderr, "bitset test failed at line %i\n", __LINE__);
        OK = false;
    }

	// TranscodeFrom/ToStr

	const char* utf8 = "UTF-8";
	char* empty = (char*)TranscodeToStr(XMLUni::fgZeroLenString,utf8).adopt(); 
	if(XMLString::stringLen(empty)!=0)
	{
        fprintf(stderr, "TranscodeToStr failed at line %i\n", __LINE__);
        OK = false;
	}
	XMLCh* empty2 = TranscodeFromStr((XMLByte*)empty,strlen(empty),utf8).adopt(); 
	if(XMLString::stringLen(empty2)!=0)
	{
        fprintf(stderr, "TranscodeFromStr failed at line %i\n", __LINE__);
        OK = false;
	}
	XMLString::release(&empty);
	XMLString::release(&empty2);

	const XMLCh aval [] = { 0x0041, 0x0000}; //LATIN CAPITAL LETTER A
	char* ac = (char*)TranscodeToStr(aval,utf8).adopt(); 
	if(!XMLString::equals(ac, "A"))
	{
        fprintf(stderr, "TranscodeToStr failed at line %i\n", __LINE__);
        OK = false;
	}
	XMLCh* ac2=TranscodeFromStr((XMLByte*)ac, strlen(ac), utf8).adopt();
	if(!XMLString::equals(ac2, aval))
	{
        fprintf(stderr, "TranscodeFromStr failed at line %i\n", __LINE__);
        OK = false;
	}
	XMLString::release(&ac);
	XMLString::release(&ac2);
	const XMLCh uval [] = { 0x254B, 0x0000}; //BOX DRAWINGS HEAVY VERTICAL AND HORIZONTAL (needs 3 bytes for utf-8)
	char* uc = (char*)TranscodeToStr(uval,utf8).adopt(); 
	if(!XMLString::equals(uc, "\xE2\x95\x8B"))
	{
        fprintf(stderr, "TranscodeToStr failed at line %i\n", __LINE__);
        OK = false;
	}
	XMLCh* uc2=TranscodeFromStr((XMLByte*)uc, strlen(uc), utf8).adopt();
	if(!XMLString::equals(uc2, uval))
	{
        fprintf(stderr, "TranscodeFromStr failed at line %i\n", __LINE__);
        OK = false;
	}
	XMLString::release(&uc);
	XMLString::release(&uc2);

	XMLCh uc3[] = { 0x6B65, 0 }; // Unicode Han Character 'step, pace; walk, stroll' (U+6B65); UTF-8 = 0xE6 0xAD 0xA5 (e6ada5)
	char* uc4 = (char*)TranscodeToStr(uc3, utf8).adopt();
	if(!XMLString::equals(uc4, "\xE6\xAD\xA5"))
	{
        fprintf(stderr, "TranscodeToStr failed at line %i\n", __LINE__);
        OK = false;
	}
	XMLString::release(&uc4);

	// Input:                    U+7D5E U+308A U+8FBC U+307F U+691C U+7D22
	// Expected byte sequence:   E7 B5 9E E3 82 8A E8 BE BC E3 81 BF E6 A4 9C E7 B4 A2
	const XMLCh xmlStr [] = { 0x7D5E, 0x308A, 0x8FBC, 0x307F, 0x691C, 0x7D22, 0x0000};
	char* bytes = (char*)TranscodeToStr(xmlStr, "UTF-8").adopt();
	if(!XMLString::equals(bytes, "\xE7\xB5\x9E\xE3\x82\x8A\xE8\xBE\xBC\xE3\x81\xBF\xE6\xA4\x9C\xE7\xB4\xA2"))
	{
        fprintf(stderr, "TranscodeToStr failed at line %i\n", __LINE__);
        OK = false;
	}
	XMLString::release(&bytes);

    // XERCESC-2052
    // Input:                    U+4E2D U+56FD U+5236 U+9020 U+4E2D U+570B U+88FD U+9020
    // Expected byte sequence:   E4 B8 AD E5 9B BD E5 88 B6 E9 80 A0 20 2F 20 E4 B8 AD E5 9C 8B E8 A3 BD E9 80 A0
    const XMLCh xmlStr2[] = { 0x4E2D, 0x56FD, 0x5236, 0x9020, 0x20, 0x2F, 0x20, 0x4E2D, 0x570B, 0x88FD, 0x9020, 0x0000 };
    char* bytes2 = (char*)TranscodeToStr(xmlStr2, "UTF-8").adopt();
    if (!XMLString::equals(bytes2, "\xE4\xB8\xAD\xE5\x9B\xBD\xE5\x88\xB6\xE9\x80\xA0\x20\x2F\x20\xE4\xB8\xAD\xE5\x9C\x8B\xE8\xA3\xBD\xE9\x80\xA0"))
    {
        fprintf(stderr, "TranscodeToStr failed at line %i\n", __LINE__);
        OK = false;
    }
    XMLString::release(&bytes2);

    return OK;
}
