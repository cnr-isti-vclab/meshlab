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
 *
 * DOMException errors are tested by calls to DOMExceptionsTest from: Main, docBuilder...
 *
 */

#include <xercesc/dom/DOM.hpp>

//  define null for compatibility with original Java source code.
#define null 0

XERCES_CPP_NAMESPACE_USE

XERCES_CPP_NAMESPACE_BEGIN
class XercesDOMParser;
XERCES_CPP_NAMESPACE_END


class DOMTest {
public:
	static DOMElement           *testElementNode;
	static DOMAttr              *testAttributeNode;
	static DOMText              *testTextNode;
	static DOMCDATASection      *testCDATASectionNode;
	static DOMEntityReference   *testEntityReferenceNode;
	static DOMEntity            *testEntityNode;
	static DOMProcessingInstruction *testProcessingInstructionNode;
	static DOMComment           *testCommentNode;
	static DOMDocument          *testDocumentNode;
	static DOMDocumentType      *testDocumentTypeNode;
	static DOMDocumentFragment  *testDocumentFragmentNode;
	static DOMNotation          *testNotationNode;


DOMTest();

DOMDocument* createDocument();
DOMDocumentType* createDocumentType(DOMDocument* doc, XMLCh* name);
DOMEntity* createEntity(DOMDocument* doc, XMLCh* name);
DOMNotation* createNotation(DOMDocument* doc, XMLCh* name);
bool docBuilder(DOMDocument* document, XMLCh* name);

void findTestNodes(DOMDocument* document);
void findTestNodes(DOMNode* node);


bool testAttr(DOMDocument* document);
bool testCDATASection(DOMDocument* document);
bool testCharacterData(DOMDocument* document);
bool testChildNodeList(DOMDocument* document);
bool testComment(DOMDocument* document);
bool testDeepNodeList(DOMDocument* document);

/**
 **** ALL DOMDocument create methods are run in docBuilder except createAttribute which is in testAttribute**
 */
bool testDocument(DOMDocument* document);


/**
 ********This really isn't needed, only exists to throw NO_MODIFICATION_ALLOWED_ERR ********
 */
bool testDocumentFragment(DOMDocument* document);

bool testDocumentType(DOMDocument* document);
bool testDOMerrors(DOMDocument* document);
bool testXPath(DOMDocument* document);
bool testDOMImplementation(DOMDocument* document);
bool testElement(DOMDocument* document);
bool testEntity(DOMDocument* document);
bool testEntityReference(DOMDocument* document);


/**
 ********* This is only for a test of cloneNode "deep"*******
 ********* And for error tests*********
 */
bool testNode(DOMDocument* document);

bool testNotation(DOMDocument* document);
bool testPI(DOMDocument* document);
bool testText(DOMDocument* document);
bool treeCompare(DOMNode* node, DOMNode* node2);

bool testBaseURI(XercesDOMParser* parser);
bool testWholeText(XercesDOMParser* parser);
bool testLSExceptions();
bool testElementTraversal();

bool testRegex();
bool testScanner(XercesDOMParser* parser);
bool testUtilFunctions();

};

class myUserDataHandler : public DOMUserDataHandler {
private:
    DOMOperationType currentType;
    XMLCh* currentKey;
    void* currentData;
    DOMNode* currentSrc;
    DOMNode* currentDst;

public:
    myUserDataHandler() :
      currentKey(0),
      currentData(0),
      currentSrc(0),
      currentDst(0) {};

    virtual void handle(DOMOperationType operation,
                const XMLCh* const key,
                void* data,
                const DOMNode* src,
                DOMNode* dst)
    {
        currentType = operation;
        currentKey = (XMLCh*) key;
        currentData = data;
        currentSrc = (DOMNode*) src;
        currentDst = dst;
    };

    DOMOperationType getCurrentType() {
        return currentType;
    };
    XMLCh* getCurrentKey() const {
        return currentKey;
    };
    void* getCurrentData() const {
        return currentData;
    };
    DOMNode* getCurrentSrc() const {
        return currentSrc;
    };
    DOMNode* getCurrentDst() const {
        return currentDst;
    };

};


