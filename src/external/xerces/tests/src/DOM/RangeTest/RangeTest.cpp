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



/**
 * $Id$
 */


/** This IRangeTest tests all of the cases delineated as examples
 *  in the DOM Level 2 Range specification, and a few others.
 *  <p>These do not by any means completely test the API and
 *  corner cases.
 */

#include <stdio.h>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/dom/DOMRange.hpp>


XERCES_CPP_NAMESPACE_USE

bool errorOccurred = false;

#define UNUSED(x) { if(x!=0){} }

#define TASSERT(c) tassert((c), __FILE__, __LINE__)

void tassert(bool c, const char *file, int line)
{
    if (!c) {
        errorOccurred = true;
        printf("Failure.  Line %d,   file %s\n", line, file);
    }
}


#define EXCEPTION_TEST(operation, expected_exception)               \
{                                                                   \
    try {                                                           \
    operation;                                                      \
    printf(" Error: no exception thrown at line %d\n", __LINE__);   \
    errorOccurred = true;                                           \
    }                                                               \
    catch (DOMRangeException &e) {                                  \
    if (e.code != (DOMRangeException::RangeExceptionCode)expected_exception) {                             \
        printf(" Wrong RangeException code: %d at line %d\n", e.code, __LINE__); \
        errorOccurred = true;                                       \
    }                                                               \
    }                                                               \
    catch (DOMException &e) {                                       \
    if (e.code != (DOMException::ExceptionCode)expected_exception) {                             \
        printf(" Wrong exception code: %d at line %d\n", e.code, __LINE__); \
        errorOccurred = true;                                       \
    }                                                               \
    }                                                               \
    catch (...)   {                                                 \
        printf(" Wrong exception thrown at line %d\n", __LINE__);   \
        errorOccurred = true;                                       \
    }                                                               \
}
//Define a bunch of XMLCh* string for comparison
XMLCh xa[] = {chLatin_a, chNull};
XMLCh xab[] = {chLatin_a, chLatin_b, chNull};
XMLCh xabHello[] = {chLatin_a, chLatin_b, chLatin_H, chLatin_e, chLatin_l, chLatin_l, chLatin_o, chNull};
XMLCh xabHellocd[] = {chLatin_a, chLatin_b, chLatin_H, chLatin_e, chLatin_l, chLatin_l, chLatin_o, chSpace, chLatin_c, chLatin_d, chNull};
XMLCh xAnotherText[] = {chLatin_A, chLatin_n, chLatin_o, chLatin_t, chLatin_h, chLatin_e, chLatin_r,
     chLatin_T, chLatin_e, chLatin_x, chLatin_t, chNull};
XMLCh xb[] = {chLatin_b, chNull};
XMLCh xBlahxyz[] = {chLatin_B, chLatin_l, chLatin_a, chLatin_h, chSpace,
     chLatin_x, chLatin_y, chLatin_z, chNull};
XMLCh xBody[] = {chLatin_B, chLatin_o, chLatin_d, chLatin_y, chNull};
XMLCh xbody2[] = {chLatin_b, chLatin_o, chLatin_d, chLatin_y, chDigit_2, chNull};
XMLCh xc[] = {chLatin_c, chNull};
XMLCh xcool[] = {chLatin_c, chLatin_o, chLatin_o, chLatin_l, chNull};
XMLCh xef[] = {chLatin_e, chLatin_f, chNull};
XMLCh xElement1[] = {chLatin_E, chLatin_l, chLatin_e, chLatin_m, chLatin_e, chLatin_n, chLatin_t, chDigit_1, chNull};
XMLCh xElement2[] = {chLatin_E, chLatin_l, chLatin_e, chLatin_m, chLatin_e, chLatin_n, chLatin_t, chDigit_2, chNull};
XMLCh xElement3[] = {chLatin_E, chLatin_l, chLatin_e, chLatin_m, chLatin_e, chLatin_n, chLatin_t, chDigit_3, chNull};
XMLCh xell[] = {chLatin_e, chLatin_l, chLatin_l, chNull};
XMLCh xellocd[] = {chLatin_e, chLatin_l, chLatin_l, chLatin_o, chSpace, chLatin_c, chLatin_d, chNull};
XMLCh xellocdef[] = {chLatin_e, chLatin_l, chLatin_l, chLatin_o, chSpace, chLatin_c, chLatin_d, chLatin_e, chLatin_f, chNull};
XMLCh xeplacedTextInsertedTexttle[] = {chLatin_e, chLatin_p, chLatin_l, chLatin_a, chLatin_c, chLatin_e, chLatin_d,
     chLatin_T, chLatin_e, chLatin_x, chLatin_t, chLatin_I, chLatin_n, chLatin_s, chLatin_e, chLatin_r, chLatin_t, chLatin_e, chLatin_d,
     chLatin_T, chLatin_e, chLatin_x, chLatin_t, chLatin_t, chLatin_l, chLatin_e, chNull};
XMLCh xex[] = {chLatin_e, chLatin_x, chNull};
XMLCh xext1[] = {chLatin_e, chLatin_x, chLatin_t, chDigit_1, chNull};
XMLCh xext3[] = {chLatin_e, chLatin_x, chLatin_t, chDigit_3, chNull};
XMLCh xfoo[] = {chLatin_f, chLatin_o, chLatin_o, chNull};
XMLCh xH1[] = {chLatin_H, chDigit_1, chNull};
XMLCh xh2[] = {chLatin_h, chDigit_2, chNull};
XMLCh xh3[] = {chLatin_h, chDigit_3, chNull};
XMLCh xh4[] = {chLatin_h, chDigit_4, chNull};
XMLCh xHellocd[] = {chLatin_H, chLatin_e, chLatin_l, chLatin_l, chLatin_o, chSpace, chLatin_c, chLatin_d, chNull};
XMLCh xhead[] = {chLatin_h, chLatin_e, chLatin_a, chLatin_d, chNull};
XMLCh xhead2[] = {chLatin_h, chLatin_e, chLatin_a, chLatin_d, chDigit_2, chNull};
XMLCh xInsertedText[] = {chLatin_I, chLatin_n, chLatin_s, chLatin_e, chLatin_r, chLatin_t, chLatin_e, chLatin_d,
     chLatin_T, chLatin_e, chLatin_x, chLatin_t, chNull};
XMLCh xInsertedTexttle[] = {chLatin_I, chLatin_n, chLatin_s, chLatin_e, chLatin_r, chLatin_t, chLatin_e, chLatin_d,
     chLatin_T, chLatin_e, chLatin_x, chLatin_t, chLatin_t, chLatin_l, chLatin_e, chNull};
XMLCh xmoo[] = {chLatin_m, chLatin_o, chLatin_o, chNull};
XMLCh xroot[] = {chLatin_r, chLatin_o, chLatin_o, chLatin_t, chNull};
XMLCh xroot2[] = {chLatin_r, chLatin_o, chLatin_o, chLatin_t, chDigit_2, chNull};
XMLCh xP[] = {chLatin_P, chNull};
XMLCh xp1[] = {chLatin_p, chDigit_1, chNull};
XMLCh xp2[] = {chLatin_p, chDigit_2, chNull};
XMLCh xptext2[] = {chLatin_p, chLatin_t, chLatin_e, chLatin_x, chLatin_t, chDigit_2, chNull};
XMLCh xReplacedText[] = {chLatin_R, chLatin_e, chLatin_p, chLatin_l, chLatin_a, chLatin_c, chLatin_e, chLatin_d,
     chLatin_T, chLatin_e, chLatin_x, chLatin_t, chNull};
XMLCh xSurroundNode1[] = {chLatin_S, chLatin_u, chLatin_r, chLatin_r, chLatin_o, chLatin_u, chLatin_n, chLatin_d,
      chLatin_N, chLatin_o, chLatin_d, chLatin_e, chDigit_1, chNull};
XMLCh xt[] = {chLatin_t, chNull};
XMLCh xtl[] = {chLatin_t, chLatin_l, chNull};
XMLCh xtext1[] = {chLatin_t, chLatin_e, chLatin_x, chLatin_t, chDigit_1, chNull};
XMLCh xtext2[] = {chLatin_t, chLatin_e, chLatin_x, chLatin_t, chDigit_2, chNull};
XMLCh xtext2ex[] = {chLatin_t, chLatin_e, chLatin_x, chLatin_t, chDigit_2, chLatin_e, chLatin_x, chNull};
XMLCh xtext3[] = {chLatin_t, chLatin_e, chLatin_x, chLatin_t, chDigit_3, chNull};
XMLCh xtext4[] = {chLatin_t, chLatin_e, chLatin_x, chLatin_t, chDigit_4, chNull};
XMLCh xtext4ext3[] = {chLatin_t, chLatin_e, chLatin_x, chLatin_t, chDigit_4, chLatin_e, chLatin_x, chLatin_t, chDigit_3, chNull};
XMLCh xttext4ext3[] = {chLatin_t, chLatin_t, chLatin_e, chLatin_x, chLatin_t, chDigit_4, chLatin_e, chLatin_x, chLatin_t, chDigit_3, chNull};
XMLCh xtext5[] = {chLatin_t, chLatin_e, chLatin_x, chLatin_t, chDigit_5, chNull};
XMLCh xtext6[] = {chLatin_t, chLatin_e, chLatin_x, chLatin_t, chDigit_6, chNull};
XMLCh xTi[] = {chLatin_T, chLatin_i, chNull};
XMLCh xTitle[] = {chLatin_T, chLatin_i, chLatin_t, chLatin_l, chLatin_e, chNull};
XMLCh xtle[] = {chLatin_t, chLatin_l, chLatin_e, chNull};
XMLCh xu[] = {chLatin_u, chNull};
XMLCh xupbody[] = {chLatin_u, chLatin_p, chLatin_b, chLatin_o, chLatin_d, chLatin_y, chNull};
XMLCh xupbody2[] = {chLatin_u, chLatin_p, chLatin_b, chLatin_o, chLatin_d, chLatin_y, chDigit_2, chNull};
XMLCh xupp1[] = {chLatin_u, chLatin_p, chLatin_p, chDigit_1, chNull};
XMLCh xuptext1[] = {chLatin_u, chLatin_p, chLatin_t, chLatin_e, chLatin_x, chLatin_t, chDigit_1, chNull};
XMLCh xuptext2[] = {chLatin_u, chLatin_p, chLatin_t, chLatin_e, chLatin_x, chLatin_t, chDigit_2, chNull};


int  main()
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

    /*
    Range tests include testing of

    createRange

    setStart, setStartBefore. setStartAfter,
    setEnd, setEndBefore. setEndAfter
    getStartContainer, getStartOffset
    getEndContainer, getEndOffset
    getCommonAncestorContainer
    selectNode
    selectNodeContents
    insertNode
    deleteContents
    collapse
    getCollapsed
    surroundContents
    compareBoundaryPoints
    cloneRange
    cloneContents
    extractContents
    toString
    detach
    removeChild
    */
    {

        XMLCh tempStr[100];
        XMLString::transcode("Range",tempStr,99);
        {
            DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(tempStr);
            DOMDocument* doc = impl->createDocument();

            //Creating a root element
            DOMElement*     root = doc->createElement(xBody);
            doc->appendChild(root);

            //Creating the siblings of root
            DOMElement*     E11 = doc->createElement(xH1);
            root->appendChild(E11);

            DOMElement*     E12 = doc->createElement(xP);
            root->appendChild(E12);

            //Attaching texts to siblings
            DOMText*        textNode1 = doc->createTextNode(xTitle);
            E11->appendChild(textNode1);

            DOMText*        textNode11 = doc->createTextNode(xAnotherText);
            E11->appendChild(textNode11);

            DOMText*        textNode2 = doc->createTextNode(xBlahxyz);
            E12->appendChild(textNode2);

            DOMText*     E210 = doc->createTextNode(xInsertedText);
            UNUSED(E210); // silence warning
            doc->release();


        }


        {
            //DOM Tree and some usable node creation
            DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation(tempStr);
            DOMDocument* doc = impl->createDocument();

            //Creating a root element
            DOMElement*     root = doc->createElement(xBody);
            doc->appendChild(root);

            //Creating the siblings of root
            DOMElement*     E11 = doc->createElement(xH1);
            root->appendChild(E11);

            DOMElement*     E12 = doc->createElement(xP);
            root->appendChild(E12);

            //Attaching texts to siblings
            DOMText*        textNode1 = doc->createTextNode(xTitle);
            E11->appendChild(textNode1);

            DOMText*        textNode11 = doc->createTextNode(xAnotherText);
            E11->appendChild(textNode11);

            DOMText*        textNode2 = doc->createTextNode(xBlahxyz);
            E12->appendChild(textNode2);

            //experimental nodes
            DOMElement*     E120 = doc->createElement(xElement1);
            DOMElement*     E121 = doc->createElement(xElement2);
            DOMElement*     E122 = doc->createElement(xElement3);
            DOMElement*     E311 = doc->createElement(xSurroundNode1);

            DOMText*     E210 = doc->createTextNode(xInsertedText);

            DOMNode* rt = doc->getDocumentElement();
            DOMRange* range = ((DOMDocumentRange*)doc)->createRange();



            //Tests start here
            // Initial dom tree looks like :
            // <Body><H1>TitleAnother Text</H1><P>Blah xyz</P></Body>
            //i.e.,            Body(rt)
            //     _____________|________________
            //     |                           |
            //  ___H1(E11)___                    P(E12)
            //  |           |                    |
            //  "Title"  "Another Text"        "Blah xyz"


            //test for start and end settings of a range
            range->setStart(rt->getFirstChild(), 0);
            TASSERT(range->getStartContainer() == rt->getFirstChild() );
            TASSERT(range->getStartOffset() == 0);

            range->setEnd(rt->getFirstChild(), 1);
            TASSERT(range->getEndContainer() == rt->getFirstChild() );
            TASSERT(range->getEndOffset() == 1);


            //DOMNode* node = range->getCommonAncestorContainer();
            TASSERT(range->getCommonAncestorContainer() == rt->getFirstChild());

            //selection related test
            range->selectNode(rt->getLastChild());
            TASSERT(range->getStartContainer() == rt);
            TASSERT(range->getStartOffset() == 1);
            TASSERT(range->getEndContainer() == rt);
            TASSERT(range->getEndOffset() == 2);

            //insertion related tests
            range->insertNode(E120);

            //only end offset moves and new node gets into range as being inserted at boundary point
            TASSERT(range->getStartContainer() == rt);
            TASSERT(range->getStartOffset() == 1);
            TASSERT(range->getEndContainer() == rt);
            TASSERT(range->getEndOffset() == 3);

            range->insertNode(E121);
            //only end offset moves and new node gets into range as being inserted at boundary point
            TASSERT(range->getStartContainer() == rt);
            TASSERT(range->getStartOffset() == 1);
            TASSERT(range->getEndContainer() == rt);
            TASSERT(range->getEndOffset() == 4);

            rt->insertBefore(E122, rt->getFirstChild());
            //both offsets move as new node is not part of the range
            TASSERT(range->getStartContainer() == rt);
            TASSERT(range->getStartOffset() == 2);
            TASSERT(range->getEndContainer() == rt);
            TASSERT(range->getEndOffset() == 5);

            //After above operations, now the tree looks like:
            // <Body><Element3/><H1>TitleAnother Text</H1><Element2/><Element1/><P>Blah xyz</P></Body>
            //i.e.,            Body(rt)
            //     _____________|_______________________________________________________________
            //     |                |                  |                |                      |
            //  Element3(E122)  ___H1(E11)___        Element2(E121)    Element1(E120)          P(E12)
            //                  |           |                                                  |
            //               "Title"  "Another Text"                                        "Blah xyz"
            //
            // range has rt as start and end container, and 2 as start offset, 5 as end offset

            //changing selection
            range->selectNode(rt->getLastChild()->getPreviousSibling());
            TASSERT(range->getStartContainer() == rt);
            TASSERT(range->getStartOffset() == 3);
            TASSERT(range->getEndContainer() == rt);
            TASSERT(range->getEndOffset() == 4);

            //deleting related tests
            range->deleteContents();
            TASSERT(rt->getLastChild()->getPreviousSibling() == E121);

            range->setStart(rt->getFirstChild()->getNextSibling()->getFirstChild(), 2);
            TASSERT(range->getStartContainer() == rt->getFirstChild()->getNextSibling()->getFirstChild());
            TASSERT(!XMLString::compareString(range->getStartContainer()->getNodeValue(),xTitle));
            TASSERT(range->getStartOffset() == 2);

            range->setEnd(rt->getFirstChild()->getNextSibling()->getFirstChild(), 4);
            TASSERT(range->getEndContainer() == rt->getFirstChild()->getNextSibling()->getFirstChild());
            TASSERT(!XMLString::compareString(range->getEndContainer()->getNodeValue(),xTitle));
            TASSERT(range->getEndOffset() == 4);
            TASSERT(!XMLString::compareString(range->toString(),xtl));

            //inserting text between a text node
            range->insertNode(E210);

            //only end offset moves and new node gets into range as being inserted at boundary point
            TASSERT(range->getStartContainer() == rt->getFirstChild()->getNextSibling()->getFirstChild());
            TASSERT(range->getStartOffset() == 2);
            TASSERT(range->getEndContainer() == rt->getFirstChild()->getNextSibling()->getLastChild()->getPreviousSibling());
            TASSERT(range->getEndOffset() == 2);

            //inserting element node before the selected text node
            range->insertNode(E120);
            //only end offset moves and new node gets into range as being inserted at boundary point
            TASSERT(range->getStartContainer() == rt->getFirstChild()->getNextSibling()->getFirstChild());
            TASSERT(!XMLString::compareString(range->getStartContainer()->getNodeValue(),xTi));
            TASSERT(range->getStartOffset() == 2);
            TASSERT(range->getEndContainer() == rt->getFirstChild()->getNextSibling()->getLastChild()->getPreviousSibling());
            TASSERT(!XMLString::compareString(range->getEndContainer()->getNodeValue(),xtle));
            TASSERT(range->getEndOffset() == 2);
            TASSERT(E11->getChildNodes()->getLength()==6);

           //checking the text replacment
            range->getStartContainer()->setNodeValue(xReplacedText);
            //only the start offset is impact
            TASSERT(range->getStartContainer() == rt->getFirstChild()->getNextSibling()->getFirstChild());
            TASSERT(!XMLString::compareString(range->getStartContainer()->getNodeValue(),xReplacedText));
            TASSERT(range->getStartOffset() == 0);
            TASSERT(range->getEndContainer() == rt->getFirstChild()->getNextSibling()->getLastChild()->getPreviousSibling());
            TASSERT(!XMLString::compareString(range->getEndContainer()->getNodeValue(),xtle));
            TASSERT(range->getEndOffset() == 2);

            //After above operations, now the tree looks like:
            // <Body><Element3/><H1>ReplacedText<Element1/>InsertedTexttleAnother Text</H1><Element2/><P>Blah xyz</P></Body>
            //i.e.,            Body(rt)
            //     _____________|_______________________________________________________________________________________________
            //     |                |                                                                          |                |
            //  Element3(E122)  ___H1(E11)___________________________________________________________        Element2(E121)    P(E12)
            //                  |              |     |                |                      |      |                             |
            //               "ReplacedText"   ""   Element1(E120)   "InsertedText"(E210)   "tle"  "Another Text"              "Blah xyz"
            //
            // range has "ReplacedText" as start container and "tle" as end container
            //   and 0 as start offset, 2 as end offset

            //changing the selection. Preparing for 'surround'
            range->setStart(range->getStartContainer()->getParentNode(), 2);
            range->setEnd(range->getStartContainer(), 5);
            TASSERT(!XMLString::compareString(range->getStartContainer()->getNodeName(),xH1));
            TASSERT(!XMLString::compareString(range->getEndContainer()->getNodeName(),xH1));
            TASSERT(!XMLString::compareString(range->toString(),xInsertedTexttle));

            range->surroundContents(E311);
            TASSERT(!XMLString::compareString(range->getStartContainer()->getNodeName(),xH1));
            TASSERT(range->getStartOffset() == 2);
            TASSERT(!XMLString::compareString(range->getEndContainer()->getNodeName(),xH1));
            TASSERT(range->getEndOffset() == 3);
            TASSERT(E11->getChildNodes()->getLength()==4);
            TASSERT(E311->getChildNodes()->getLength()==3);
            TASSERT(!XMLString::compareString(range->toString(),xInsertedTexttle));

            //After above operations, now the tree looks like:
            // <Body><Element3/><H1>ReplacedText<SurroundNode1><Element1/>InsertedTexttle</SurroundNode1>Another Text</H1><Element2/><P>Blah xyz</P></Body>
            //i.e.,            Body(rt)
            //     _____________|_________________________________________________________________________
            //     |                |                                                    |                |
            //  Element3(E122)  ___H1(E11)___________________________________        Element2(E121)    P(E12)
            //                  |              |     |                      |                            |
            //               "ReplacedText"   ""   SurroundNode1(E311)  "Another Text"              "Blah xyz"
            //                          ____________ |_____________________________
            //                          |                    |                    |
            //                          Element1(E120)   "InsertedText"(E210)   "tle"
            //
            // range has H1 as start and end container and 2 as start offset, 3 as end offset

            //testing cloning
            DOMRange* aRange = range->cloneRange();

            TASSERT(aRange->getStartContainer() == range->getStartContainer());
            TASSERT(aRange->getEndContainer() == range->getEndContainer());
            TASSERT(aRange->getStartOffset() == 2);
            TASSERT(aRange->getEndOffset() == 3);
            //changing the new ranges start
            aRange->setStart(aRange->getStartContainer()->getFirstChild(), 1);

            //comparing the ranges
            short compVal = range->compareBoundaryPoints(DOMRange::END_TO_END, aRange);
            TASSERT(compVal == 0);
            compVal = range->compareBoundaryPoints(DOMRange::START_TO_START, aRange);
            TASSERT(compVal == 1);
            compVal = range->compareBoundaryPoints(DOMRange::START_TO_END, aRange);
            TASSERT(compVal == 1);
            compVal = range->compareBoundaryPoints(DOMRange::END_TO_START, aRange);
            TASSERT(compVal == -1);

            //testing collapse
            //not collapsed
            TASSERT(range->getCollapsed() == false);
            TASSERT(range->getStartOffset() == 2);
            TASSERT(range->getEndOffset() == 3);

            //selectNodeContents
            range->selectNodeContents(rt->getLastChild()->getFirstChild());
            TASSERT(range->getStartContainer() == rt->getLastChild()->getFirstChild());
            TASSERT(range->getEndContainer() == rt->getLastChild()->getFirstChild());
            TASSERT(range->getStartOffset() == 0);
            TASSERT(range->getEndOffset() == 8);
            TASSERT(!XMLString::compareString(range->toString(),xBlahxyz));

            //testing collapse
            range->collapse(true); //collapse to start
            TASSERT(range->getCollapsed() == true);
            TASSERT(range->getStartOffset() == 0);
            TASSERT(range->getEndOffset() == 0);
            TASSERT(!XMLString::compareString(range->toString(),XMLUni::fgZeroLenString));
            TASSERT(aRange->getEndOffset() == 3); //other range is unaffected
            TASSERT(!XMLString::compareString(aRange->toString(),xeplacedTextInsertedTexttle));

            //After above operations, now the tree looks like:
            // <Body><Element3/><H1>ReplacedText<SurroundNode1><Element1/>InsertedTexttle</SurroundNode1>Another Text</H1><Element2/><P>Blah xyz</P></Body>
            //i.e.,            Body(rt)
            //     _____________|_________________________________________________________________________
            //     |                |                                                    |                |
            //  Element3(E122)  ___H1(E11)___________________________________        Element2(E121)    P(E12)
            //                  |              |     |                      |                            |
            //               "ReplacedText"   ""   SurroundNode1(E311)  "Another Text"              "Blah xyz"
            //                          ____________ |_____________________________
            //                          |                    |                    |
            //                          Element1(E120)   "InsertedText"(E210)   "tle"
            //
            // range has "Blah xyz" as start and end container and 0 as start and end offset (collapsed)
            // aRange has "ReplacedText" as start container and H1 as end container
            //    and 1 as start offset and 3 as end offset

            DOMDocumentFragment* docFrag = aRange->cloneContents();
            TASSERT( docFrag != 0);
            range->selectNode(rt->getFirstChild());
            TASSERT(range->getStartContainer() == rt);
            TASSERT(range->getEndContainer() == rt);
            TASSERT(range->getStartOffset() == 0);
            TASSERT(range->getEndOffset() == 1);

            //Testing toString()
            const XMLCh* str = aRange->toString();
            TASSERT(!XMLString::compareString(str, xeplacedTextInsertedTexttle));

            //start and end before and after tests
            range->setStartBefore(rt->getFirstChild());
            TASSERT(range->getStartOffset() == 0);
            range->setEndBefore(rt->getFirstChild());
            TASSERT(range->getEndOffset() == 0);
            range->setStartAfter(rt->getLastChild());
            TASSERT(range->getStartOffset() == 4);

            range->setStartAfter(rt->getFirstChild());
            TASSERT(range->getStartOffset() == 1);

            range->setEndBefore(rt->getLastChild());
            TASSERT(range->getEndOffset() == 3);

            range->setEndAfter(rt->getLastChild());
            TASSERT(range->getEndOffset() == 4);

            //testing extract()
            DOMDocumentFragment* frag2 = range->extractContents();
            TASSERT( frag2 != 0);

            //After above operations, now the tree looks like:
            // <Body><Element3/></Body>
            //i.e.,            Body(rt)
            //                  |
            //               Element3(E122)
            //
            // aRange has rt as start and end container, and 1 as start and end offset (collapsed)
            // range has rt as start and end container, and 1 as start and end offset (collapsed)
            //
            //and frag2 looks:
            // <Body>ReplacedText<SurroundNode1><Element1/>InsertedTexttle</SurroundNode1>Another Text</H1><Element2/><P>Blah xyz</P></Body>
            //i.e.,             Body(rt)
            //      ______________|________________________________________________________
            //      |                                                    |                |
            //   ___H1(E11)___________________________________        Element2(E121)    P(E12)
            //   |              |     |                      |                            |
            //"ReplacedText"   ""   SurroundNode1(E311)  "Another Text"              "Blah xyz"
            //           ____________ |_____________________________
            //           |                    |                    |
            //        Element1(E120)   "InsertedText"(E210)   "tle"
            //

            //the tree do not have those node anymore after extract
            //only Element3 left
            TASSERT(rt->getChildNodes()->getLength()==1);

            //aRange is collapsed
            TASSERT(aRange->getCollapsed() == true);
            TASSERT(aRange->getStartContainer() == rt);
            TASSERT(aRange->getStartOffset() == 1);
            TASSERT(aRange->getEndContainer() == rt);
            TASSERT(aRange->getEndOffset() == 1);

            //range is collapsed as well
            TASSERT(range->getCollapsed() == true);
            TASSERT(range->getStartContainer() == rt);
            TASSERT(range->getStartOffset() == 1);
            TASSERT(range->getEndContainer() == rt);
            TASSERT(range->getEndOffset() == 1);

            //test the document fragment frag2
            TASSERT(frag2->getChildNodes()->getLength()==3);

            //detaching the other range
            aRange->detach();
            range->detach();

            //***************************************************************
            //another set of test
            //TEST createRange, setStart and setEnd, insertnode
            //***************************************************************
            DOMImplementation* impl2 = DOMImplementationRegistry::getDOMImplementation(tempStr);
            DOMDocument* doc2 = impl2->createDocument();

            DOMElement* root2 = doc2->createElement(xroot2);
            doc2->appendChild(root2);
            //case 1: simple text node, start==end
            // <body>text1</body>
            DOMElement* body = doc2->createElement(xBody);
            DOMText* text1 = doc2->createTextNode(xtext1);
            body->appendChild(text1);
            root2->appendChild(body);

            //set range
            DOMRange* range1 = doc2->createRange();
            range1->setStart(text1,1);
            range1->setEnd(text1,3);

            TASSERT(!XMLString::compareString(range1->toString(),xex));
            TASSERT(range1->getStartOffset()==1);
            TASSERT(!XMLString::compareString(range1->getStartContainer()->getNodeValue(),xtext1));
            TASSERT(range1->getEndOffset()==3);
            TASSERT(!XMLString::compareString(range1->getEndContainer()->getNodeValue(),xtext1));

            //now insert a text node
            //<body>ttext2ext1</body>
            DOMText* text2 = doc2->createTextNode(xtext2);
            range1->insertNode(text2);

            TASSERT(!XMLString::compareString(range1->toString(),xtext2ex));
            TASSERT(range1->getStartOffset()==1);
            TASSERT(!XMLString::compareString(range1->getStartContainer()->getNodeValue(),xt));
            TASSERT(range1->getEndOffset()==2);
            TASSERT(!XMLString::compareString(range1->getEndContainer()->getNodeValue(),xext1));

            //now insert a non-text node
            //<body>t<p1/>text2ext1</body>
            DOMElement* p1 = doc2->createElement(xp1);
            range1->insertNode(p1);

            TASSERT(!XMLString::compareString(range1->toString(),xtext2ex));
            TASSERT(range1->getStartOffset()==1);
            TASSERT(!XMLString::compareString(range1->getStartContainer()->getNodeValue(),xt));
            TASSERT(range1->getEndOffset()==2);
            TASSERT(!XMLString::compareString(range1->getEndContainer()->getNodeValue(),xext1));

            //case 2: non-text node, start==end
            // <head><h1/></head>
            DOMElement* head = doc2->createElement(xhead);
            DOMElement* h1 = doc2->createElement(xH1);
            head->appendChild(h1);
            root2->appendChild(head);

            //set range
            DOMRange* range2 = doc2->createRange();
            range2->setStart(head,0);
            range2->setEnd(head,1);

            TASSERT(!XMLString::compareString(range2->toString(),XMLUni::fgZeroLenString));
            TASSERT(range2->getStartOffset()==0);
            TASSERT(!XMLString::compareString(range2->getStartContainer()->getNodeName(),xhead));
            TASSERT(range2->getEndOffset()==1);
            TASSERT(!XMLString::compareString(range2->getEndContainer()->getNodeName(),xhead));

            //now insert a non-text node
            //<head><h2/><h1/></head>
            DOMElement* h2 = doc2->createElement(xh2);
            range2->insertNode(h2);

            TASSERT(!XMLString::compareString(range2->toString(),XMLUni::fgZeroLenString));
            TASSERT(range2->getStartOffset()==0);
            TASSERT(!XMLString::compareString(range2->getStartContainer()->getNodeName(),xhead));
            TASSERT(range2->getEndOffset()==2);
            TASSERT(!XMLString::compareString(range2->getEndContainer()->getNodeName(),xhead));

            //now insert a text node
            //<head>text5<h2/><h1/></head>
            DOMText* text5 = doc2->createTextNode(xtext5);
            range2->insertNode(text5);

            TASSERT(!XMLString::compareString(range2->toString(),xtext5));
            TASSERT(range2->getStartOffset()==0);
            TASSERT(!XMLString::compareString(range2->getStartContainer()->getNodeName(),xhead));
            TASSERT(range2->getEndOffset()==3);
            TASSERT(!XMLString::compareString(range2->getEndContainer()->getNodeName(),xhead));

            //case 3: simple text node, start!=end
            // <body2>text3</body2>
            DOMElement* body2 = doc2->createElement(xbody2);
            DOMText* text3 = doc2->createTextNode(xtext3);
            body2->appendChild(text3);
            root2->appendChild(body2);

            //set range
            DOMRange* range3 = ((DOMDocumentRange*)doc2)->createRange();
            range3->setStart(text3,1);
            range3->setEnd(body2,1);

            TASSERT(!XMLString::compareString(range3->toString(),xext3));
            TASSERT(range3->getStartOffset()==1);
            TASSERT(!XMLString::compareString(range3->getStartContainer()->getNodeValue(),xtext3));
            TASSERT(range3->getEndOffset()==1);
            TASSERT(!XMLString::compareString(range3->getEndContainer()->getNodeName(),xbody2));

            //now insert a textnode
            //<body2>ttext4ext3</body2>
            DOMText* text4 = doc2->createTextNode(xtext4);
            range3->insertNode(text4);

            TASSERT(!XMLString::compareString(range3->toString(),XMLUni::fgZeroLenString));
            TASSERT(range3->getStartOffset()==1);
            TASSERT(!XMLString::compareString(range3->getStartContainer()->getNodeValue(),xt));
            TASSERT(range3->getEndOffset()==1);
            TASSERT(!XMLString::compareString(range3->getEndContainer()->getNodeName(),xbody2));

            //now insert a non-text node
            //<body2>t<p2/>text4ext3</body2>
            DOMElement* p2 = doc2->createElement(xp2);
            range3->insertNode(p2);

            //extra empty node caused by splitting 't'
            TASSERT(!XMLString::compareString(range3->toString(),XMLUni::fgZeroLenString));
            TASSERT(range3->getStartOffset()==1);
            TASSERT(!XMLString::compareString(range3->getStartContainer()->getNodeValue(),xt));
            TASSERT(range3->getEndOffset()==1);
            TASSERT(!XMLString::compareString(range3->getEndContainer()->getNodeName(),xbody2));

            //test toString a bit
            range3->setStart(body2,1);
            range3->setEnd(body2,5);

            TASSERT(!XMLString::compareString(range3->toString(),xtext4ext3));

            range3->setStart(body2,0);
            range3->setEnd(body2,5);

            TASSERT(!XMLString::compareString(range3->toString(),xttext4ext3));

            //case 4: non-text node, start!=end
            // <head2><h3/></head2>
            DOMElement* head2 = doc2->createElement(xhead2);
            DOMElement* h3 = doc2->createElement(xh3);
            head2->appendChild(h3);
            root2->appendChild(head2);

            //set range
            DOMRange* range4 = doc2->createRange();
            range4->setStart(head2,0);
            range4->setEnd(h3,0);

            TASSERT(!XMLString::compareString(range4->toString(),XMLUni::fgZeroLenString));
            TASSERT(range4->getStartOffset()==0);
            TASSERT(!XMLString::compareString(range4->getStartContainer()->getNodeName(),xhead2));
            TASSERT(range4->getEndOffset()==0);
            TASSERT(!XMLString::compareString(range4->getEndContainer()->getNodeName(),xh3));

            //now insert a non-text node
            //<head2><h4/><h3/></head2>
            DOMElement* h4 = doc2->createElement(xh4);
            range4->insertNode(h4);

            TASSERT(!XMLString::compareString(range4->toString(),XMLUni::fgZeroLenString));
            TASSERT(range4->getStartOffset()==0);
            TASSERT(!XMLString::compareString(range4->getStartContainer()->getNodeName(),xhead2));
            TASSERT(range4->getEndOffset()==0);
            TASSERT(!XMLString::compareString(range4->getEndContainer()->getNodeName(),xh3));

            //now insert a text node
            //<head2>text6<h4/><h3/></head2>
            DOMText* text6 = doc2->createTextNode(xtext6);
            range4->insertNode(text6);

            TASSERT(!XMLString::compareString(range4->toString(),xtext6));
            TASSERT(range4->getStartOffset()==0);
            TASSERT(!XMLString::compareString(range4->getStartContainer()->getNodeName(),xhead2));
            TASSERT(range4->getEndOffset()==0);
            TASSERT(!XMLString::compareString(range4->getEndContainer()->getNodeName(),xh3));

            //***************************************************************
            // quick test of updating
            //***************************************************************
            // <upbody>text1</upbody>
            DOMElement* upbody = doc2->createElement(xupbody);
            DOMText* uptext1 = doc2->createTextNode(xuptext1);
            upbody->appendChild(uptext1);
            root2->appendChild(upbody);

            DOMRange* uprange = ((DOMDocumentRange*)doc2)->createRange();
            uprange->setStart(upbody,0);
            uprange->setEnd(upbody,1);

            TASSERT(!XMLString::compareString(uprange->toString(),xuptext1));
            TASSERT(uprange->getStartOffset()==0);
            TASSERT(!XMLString::compareString(uprange->getStartContainer()->getNodeName(),xupbody));
            TASSERT(uprange->getEndOffset()==1);
            TASSERT(!XMLString::compareString(uprange->getEndContainer()->getNodeName(),xupbody));

            // split text
            uptext1->splitText(1);

            TASSERT(!XMLString::compareString(uprange->toString(),xu));
            TASSERT(uprange->getStartOffset()==0);
            TASSERT(!XMLString::compareString(uprange->getStartContainer()->getNodeName(),xupbody));
            TASSERT(uprange->getEndOffset()==1);
            TASSERT(!XMLString::compareString(uprange->getEndContainer()->getNodeName(),xupbody));

            //insert node
            DOMElement* upbody2 = doc2->createElement(xupbody2);
            DOMText* uptext2 = doc2->createTextNode(xuptext2);
            upbody2->appendChild(uptext2);
            root2->appendChild(upbody2);

            DOMRange* uprange2 = ((DOMDocumentRange*)doc2)->createRange();
            uprange2->setStart(uptext2,1);
            uprange2->setEnd(upbody2,1);

            DOMRange* uprange3 = doc2->createRange();
            uprange3->setStart(uptext2,1);
            uprange3->setEnd(upbody2,1);

            TASSERT(!XMLString::compareString(uprange2->toString(),xptext2));
            TASSERT(uprange2->getStartOffset()==1);
            TASSERT(!XMLString::compareString(uprange2->getStartContainer()->getNodeValue(),xuptext2));
            TASSERT(uprange2->getEndOffset()==1);
            TASSERT(!XMLString::compareString(uprange2->getEndContainer()->getNodeName(),xupbody2));

            TASSERT(!XMLString::compareString(uprange3->toString(),xptext2));
            TASSERT(uprange3->getStartOffset()==1);
            TASSERT(!XMLString::compareString(uprange3->getStartContainer()->getNodeValue(),xuptext2));
            TASSERT(uprange3->getEndOffset()==1);
            TASSERT(!XMLString::compareString(uprange3->getEndContainer()->getNodeName(),xupbody2));

            DOMElement* upp1 = doc2->createElement(xupp1);
            uprange2->insertNode(upp1);

            TASSERT(!XMLString::compareString(uprange2->toString(),XMLUni::fgZeroLenString));
            TASSERT(uprange2->getStartOffset()==1);
            TASSERT(!XMLString::compareString(uprange2->getStartContainer()->getNodeValue(),xu));
            TASSERT(uprange2->getEndOffset()==1);
            TASSERT(!XMLString::compareString(uprange2->getEndContainer()->getNodeName(),xupbody2));

            TASSERT(!XMLString::compareString(uprange3->toString(),XMLUni::fgZeroLenString));
            TASSERT(uprange3->getStartOffset()==1);
            TASSERT(!XMLString::compareString(uprange3->getStartContainer()->getNodeValue(),xu));
            TASSERT(uprange3->getEndOffset()==1);
            TASSERT(!XMLString::compareString(uprange3->getEndContainer()->getNodeName(),xupbody2));

            //***************************************************************
            //another set of test
            //<foo><c/><moo><b/></moo>ab<a>Hello cd</a><cool>ef</cool></foo>
            //
            //  ______________________foo_____________________
            //  |          |           |          |           |
            //  c         moo        "ab"         a          cool
            //             |                      |           |
            //             b                    "Hello cd"   "ef"
            //
            DOMImplementation* impl3 = DOMImplementationRegistry::getDOMImplementation(tempStr);
            DOMDocument* doc3 = impl3->createDocument();

            DOMElement* root3 = doc3->createElement(xroot);
            doc3->appendChild(root3);

            DOMElement* foo = doc3->createElement(xfoo);
            DOMElement* moo = doc3->createElement(xmoo);
            DOMElement* cool = doc3->createElement(xcool);
            DOMText* ab = doc3->createTextNode(xab);
            DOMText* cd = doc3->createTextNode(xHellocd);
            DOMText* ef = doc3->createTextNode(xef);

            DOMElement* a = doc3->createElement(xa);
            DOMElement* b = doc3->createElement(xb);
            DOMElement* c = doc3->createElement(xc);

            root3->appendChild(foo);
            foo->appendChild(c);
            foo->appendChild(moo);
            foo->appendChild(ab);
            foo->appendChild(a);
            foo->appendChild(cool);
            moo->appendChild(b);
            a->appendChild(cd);
            cool->appendChild(ef);

            //***************************************************************
            //TEST toString
            //***************************************************************
            DOMRange* newtestrange = ((DOMDocumentRange*)doc3)->createRange();
            //case 1:
            //start container is text node
            //   i) end container is also text node
            //    a) start==end
            //    b) start!=end
            //  ii) end container is not text node
            //    a) start==end => impossible
            //    b) start!=end
            //
            //case 2:
            //start container is not text node
            //   i) end container is text node
            //    a) start==end => impossible
            //    b) start!=end
            //  ii) end container is not text node
            //    a) start==end
            //    b) start!=end

            //case 1, i, a
            newtestrange->setStart( cd, 1 );
            newtestrange->setEnd( cd, 4 );

            TASSERT(!XMLString::compareString(newtestrange->toString(),xell));

            //case 1, i, b
            newtestrange->setStart( cd, 1 );
            newtestrange->setEnd( ef, 2 );

            TASSERT(!XMLString::compareString(newtestrange->toString(),xellocdef));

            //case 1, ii, b
            newtestrange->setStart( cd, 1 );
            newtestrange->setEnd( foo, 4 );

            TASSERT(!XMLString::compareString(newtestrange->toString(),xellocd));

            //case 2, i, b
            newtestrange->setStart( foo, 1 );
            newtestrange->setEnd( cd, 5 );

            TASSERT(!XMLString::compareString(newtestrange->toString(),xabHello));

            //case 2, ii, a
            newtestrange->setStart( foo, 1 );
            newtestrange->setEnd( foo, 4 );

            TASSERT(!XMLString::compareString(newtestrange->toString(),xabHellocd));

            //case 2, ii, b
            newtestrange->setStart( moo, 1 );
            newtestrange->setEnd( foo, 4 );

            TASSERT(!XMLString::compareString(newtestrange->toString(),xabHellocd));

            //***************************************************************
            //test removeChild
            //***************************************************************
            DOMRange* newrange = doc3->createRange();
            newrange->setStart( moo, 0 );
            newrange->setEnd( foo, 4 );

            TASSERT(newrange->getStartOffset()==0);
            TASSERT(!XMLString::compareString(newrange->getStartContainer()->getNodeName(),xmoo));
            TASSERT(newrange->getEndOffset()==4);
            TASSERT(!XMLString::compareString(newrange->getEndContainer()->getNodeName(),xfoo));
            TASSERT(!XMLString::compareString(newrange->toString(),xabHellocd));

            DOMNode* n = newrange->cloneContents();
            UNUSED(n); // silence warning
            DOMNodeList* nol = foo->getChildNodes();

            //removing moo
            DOMNode* rem = foo->removeChild(nol->item(1));
            rem->release();
            TASSERT(newrange->getStartOffset()==1);
            TASSERT(!XMLString::compareString(newrange->getStartContainer()->getNodeName(),xfoo));
            TASSERT(newrange->getEndOffset()==3);
            TASSERT(!XMLString::compareString(newrange->getEndContainer()->getNodeName(),xfoo));
            TASSERT(!XMLString::compareString(newrange->toString(),xabHellocd));

            TASSERT(newtestrange->getStartOffset()==1);
            TASSERT(!XMLString::compareString(newtestrange->getStartContainer()->getNodeName(),xfoo));
            TASSERT(newtestrange->getEndOffset()==3);
            TASSERT(!XMLString::compareString(newtestrange->getEndContainer()->getNodeName(),xfoo));
            TASSERT(!XMLString::compareString(newtestrange->toString(),xabHellocd));

            // Now do some exception test
            newrange->detach();
            EXCEPTION_TEST(newrange->setStart( moo, 0 ), DOMException::INVALID_STATE_ERR);
            EXCEPTION_TEST(newtestrange->setStartBefore(moo), DOMRangeException::INVALID_NODE_TYPE_ERR);


            doc->release();
            doc2->release();
            doc3->release();
        }
    } //creating the dom tree and tests

    // And call the termination method
    XMLPlatformUtils::Terminate();

    if (errorOccurred) {
        printf("Test Failed\n");
        return 4;
    }

    printf("Test Run Successfully\n");

    return 0;
}
