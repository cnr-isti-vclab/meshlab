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

#include "TypeInfo.hpp"

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/validators/schema/SchemaSymbols.hpp>

#if defined(XERCES_NEW_IOSTREAMS)
#include <iostream>
#else
#include <iostream.h>
#endif

#define UNUSED(x) { if(x!=0){} }

#define DOMTYPEINFOTEST(info, type, uri, line) \
    tmp = XMLString::equals(info->getTypeName(), type) && XMLString::equals(info->getTypeNamespace(), uri);\
    if(!tmp) { \
      XERCES_STD_QUALIFIER cerr << "DOMTypeInfo test failed at line, " << line << "\nExpected values : typename '" << XMLString::transcode((XMLCh*)type) << "', uri '" << XMLString::transcode((XMLCh*)uri); \
      XERCES_STD_QUALIFIER cerr << "'\nActual values   : typename '"; \
      if(info->getTypeName())   \
        XERCES_STD_QUALIFIER cerr << XMLString::transcode(info->getTypeName());  \
      else \
        XERCES_STD_QUALIFIER cerr << "(null)"; \
      XERCES_STD_QUALIFIER cerr << "', uri '"; \
      if(info->getTypeNamespace())   \
        XERCES_STD_QUALIFIER cerr << XMLString::transcode(info->getTypeNamespace()); \
      else \
        XERCES_STD_QUALIFIER cerr << "(null)"; \
      XERCES_STD_QUALIFIER cerr << "'\n" << XERCES_STD_QUALIFIER endl; \
      passed = false; \
    }

bool tmp;

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

TypeInfo::TypeInfo() {
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

TypeInfo::~TypeInfo() {
    XMLPlatformUtils::Terminate();
}

bool TypeInfo::testInBuiltTypesOnAttributes(bool DTDPresent) {

    bool passed = true;
    DOMElement *testEle = findElement(X("attrTest"));

    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), X("attrTestType"), X(""), __LINE__);

    DOMAttr *testAttr;

    testAttr = testEle->getAttributeNodeNS(0, X("anySimpleType"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_ANYSIMPLETYPE, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(0, X("string"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_STRING, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(0, X("boolean"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_BOOLEAN, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(0, X("decimal"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_DECIMAL, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(0, X("float"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_FLOAT, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(0, X("double"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_DOUBLE, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(0, X("duration"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_DURATION, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(0, X("dateTime"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_DATETIME, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(0, X("time"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_TIME, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(0, X("date"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_DATE, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(0, X("gYearMonth"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_YEARMONTH, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(0, X("gYear"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_YEAR, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(0, X("gMonthDay"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_MONTHDAY, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(0, X("gDay"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_DAY, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(0, X("gMonth"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_MONTH, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(0, X("hexBinary"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_HEXBINARY, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(0, X("base64Binary"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_BASE64BINARY, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(0, X("anyURI"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_ANYURI, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(0, X("QName"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_QNAME, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(0, X("normalizedString"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_NORMALIZEDSTRING, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(0, X("token"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_TOKEN, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(0, X("language"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_LANGUAGE, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(0, X("NMTOKEN"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), XMLUni::fgNmTokenString, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(0, X("NMTOKENS"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), XMLUni::fgNmTokensString, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(0, X("Name"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_NAME, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(0, X("NCName"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_NCNAME, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(0, X("ID"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), XMLUni::fgIDString, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(0, X("IDREF"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), XMLUni::fgIDRefString, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(0, X("IDREFS"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), XMLUni::fgIDRefsString, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);


    if(DTDPresent) {
        testAttr = testEle->getAttributeNodeNS(0, X("ENTITY"));
        DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), XMLUni::fgEntityString, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

        testAttr = testEle->getAttributeNodeNS(0, X("ENTITIES"));
        DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), XMLUni::fgEntitiesString, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);
    }

    testAttr = testEle->getAttributeNodeNS(0, X("integer"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_INTEGER, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(0, X("nonPositiveInteger"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_NONPOSITIVEINTEGER, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(0, X("negativeInteger"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_NEGATIVEINTEGER, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(0, X("long"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_LONG, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(0, X("int"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_INT, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(0, X("short"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_SHORT, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(0, X("byte"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_BYTE, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(0, X("nonNegativeInteger"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_NONNEGATIVEINTEGER, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(0, X("unsignedLong"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_ULONG, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(0, X("unsignedInt"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_UINT, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(0, X("unsignedShort"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_USHORT, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(0, X("unsignedByte"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_UBYTE, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(0, X("positiveInteger"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_POSITIVEINTEGER, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    //couple of defaulted ones
    testAttr = testEle->getAttributeNodeNS(0, X("defaultString"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_STRING, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(0, X("defaultInt"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_INTEGER, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    //ns attr
    testAttr = testEle->getAttributeNodeNS(X("http://www.w3.org/2000/xmlns/"), X("prefix"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_ANYURI, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(X("http://www.w3.org/2001/XMLSchema-instance"), X("noNamespaceSchemaLocation"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_ANYURI, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    return passed;
}


bool TypeInfo::testInBuiltTypesOnElements() {

    bool passed = true;
    DOMNode *docEle = doc->getDocumentElement();
    UNUSED(docEle); // silence warning

    //the eleTest element.
    DOMElement *testEle = findElement(X("eleTest"));

    testEle = (DOMElement *)testEle->getFirstChild()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), SchemaSymbols::fgDT_STRING, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);
    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), SchemaSymbols::fgDT_BOOLEAN, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), SchemaSymbols::fgDT_DECIMAL, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), SchemaSymbols::fgDT_FLOAT, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), SchemaSymbols::fgDT_DOUBLE, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), SchemaSymbols::fgDT_DURATION, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), SchemaSymbols::fgDT_DATETIME, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), SchemaSymbols::fgDT_TIME, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), SchemaSymbols::fgDT_DATE, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), SchemaSymbols::fgDT_YEARMONTH, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), SchemaSymbols::fgDT_YEAR, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), SchemaSymbols::fgDT_MONTHDAY, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), SchemaSymbols::fgDT_DAY, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), SchemaSymbols::fgDT_MONTH, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), SchemaSymbols::fgDT_HEXBINARY, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), SchemaSymbols::fgDT_BASE64BINARY, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), SchemaSymbols::fgDT_ANYURI, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), SchemaSymbols::fgDT_QNAME, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), SchemaSymbols::fgDT_NORMALIZEDSTRING, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), SchemaSymbols::fgDT_TOKEN, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), SchemaSymbols::fgDT_LANGUAGE, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), XMLUni::fgNmTokenString, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), XMLUni::fgNmTokensString, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), SchemaSymbols::fgDT_NAME, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), SchemaSymbols::fgDT_NCNAME, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), SchemaSymbols::fgDT_INTEGER, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), SchemaSymbols::fgDT_NONPOSITIVEINTEGER, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), SchemaSymbols::fgDT_NEGATIVEINTEGER, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), SchemaSymbols::fgDT_LONG, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), SchemaSymbols::fgDT_INT, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), SchemaSymbols::fgDT_SHORT, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), SchemaSymbols::fgDT_BYTE, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), SchemaSymbols::fgDT_NONNEGATIVEINTEGER, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), SchemaSymbols::fgDT_ULONG, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), SchemaSymbols::fgDT_UINT, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), SchemaSymbols::fgDT_USHORT, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), SchemaSymbols::fgDT_UBYTE, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), SchemaSymbols::fgDT_POSITIVEINTEGER, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), SchemaSymbols::fgATTVAL_ANYTYPE, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    return passed;
}

bool TypeInfo::testSimpleDerived() {

    bool passed = true;
    DOMNode *docEle = doc->getDocumentElement();
    UNUSED(docEle); // silence warning

    //element simpleDerTest
    DOMElement *testEle = (DOMElement *)docEle->getFirstChild()->getNextSibling()->getNextSibling()
        ->getNextSibling()->getNextSibling()->getNextSibling();

    DOMAttr *testAtt = testEle->getAttributeNodeNS(0, X("decimalDerived"));
    DOMTYPEINFOTEST(testAtt->getSchemaTypeInfo(), X("decimalDerivedType"), X(""),  __LINE__);

    testAtt = testEle->getAttributeNodeNS(0, X("stringDerived"));
    DOMTYPEINFOTEST(testAtt->getSchemaTypeInfo(), X("stringDerivedType"), X(""), __LINE__);

    testEle = (DOMElement *)testEle->getFirstChild()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), X("decimalDerivedType"), X(""), __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), X("stringDerivedType"), X(""), __LINE__);

    return passed;
}

bool TypeInfo::testComplexTypes() {

    bool passed = true;
    DOMNode *docEle = doc->getDocumentElement();
    UNUSED(docEle); // silence warning

    //element complexTest
    DOMElement *testEle = findElement(X("complexTest"));
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), X("complexTestType"), X(""), __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), X("complexDerTestType"), X(""), __LINE__);
    return passed;
}


bool TypeInfo::testUnions() {

    bool passed = true;
    DOMNode *docEle = doc->getDocumentElement();
    UNUSED(docEle); // silence warning
    DOMAttr *testAttr;

    //element unionTest
    DOMElement *testEle = findElement(X("unionTest"));

    testEle = (DOMElement *)testEle->getFirstChild()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), X("decimalDerivedType"), X(""), __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), X("stringDerivedType"), X(""), __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), X("decimal"), SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), X("string"), SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    testAttr = testEle->getAttributeNodeNS(0, X("testAttr"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), X("decimalDerivedType"), X(""), __LINE__);
    testAttr = testEle->getAttributeNodeNS(0, X("testAttr2"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), X("stringDerivedType"), X(""), __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    testAttr = testEle->getAttributeNodeNS(0, X("testAttr"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), X("stringDerivedType"), X(""), __LINE__);
    testAttr = testEle->getAttributeNodeNS(0, X("testAttr2"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), X("decimalDerivedType"), X(""), __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    testAttr = testEle->getAttributeNodeNS(0, X("testAttr"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), X("decimal"), SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);
    testAttr = testEle->getAttributeNodeNS(0, X("testAttr2"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), X("string"), SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    testAttr = testEle->getAttributeNodeNS(0, X("testAttr"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), X("string"), SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);
    testAttr = testEle->getAttributeNodeNS(0, X("testAttr2"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), X("decimal"), SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    return passed;

}

bool TypeInfo::testAnonymous() {

    bool passed = true;
    DOMNode *docEle = doc->getDocumentElement();
    UNUSED(docEle); // silence warning
    DOMAttr *testAttr;


    //element anonymousTest
    DOMElement *testEle = findElement(X("anonymousTest"));
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), X("__AnonC1"), X(""), __LINE__);

    testAttr = testEle->getAttributeNodeNS(X(""), X("partNum"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), X("__AnonS7"), X(""), __LINE__);

    testEle = (DOMElement *)testEle->getFirstChild()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), X("__AnonS2"), X(""), __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), X("__AnonS4"), X(""), __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), X("__AnonS6"), X(""), __LINE__);

    return passed;
}


bool TypeInfo::testXsiTypes() {

    bool passed = true;
    DOMNode *docEle = doc->getDocumentElement();
    UNUSED(docEle); // silence warning

    //element xsiTypeTest
    DOMElement *testEle = findElement(X("xsiTypeTest"));

    testEle = (DOMElement *)testEle->getFirstChild()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), X("base"), X(""), __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), X("level1"), X(""), __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), X("level2"), X(""), __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), X("baseComplex"), X(""), __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), X("level1Complex"), X(""), __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), X("level2Complex"), X(""), __LINE__);

    return passed;
}



bool TypeInfo::testAnys() {

    bool passed = true;
    DOMNode *docEle = doc->getDocumentElement();
    UNUSED(docEle); // silence warning
    DOMAttr *testAttr;

    //element anyTestPartial
    DOMElement *testEle = findElement(X("anyTestPartial"));
    DOMElement *back = testEle;

    testAttr = testEle->getAttributeNodeNS(X("http://www.w3.org/1999/xhtml"), X("attr2"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), 0, 0, __LINE__);


    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), X("anyTestPartialType"), X(""), __LINE__);

    testEle = (DOMElement *)testEle->getFirstChild()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), 0, 0, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), 0, 0, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), SchemaSymbols::fgDT_STRING, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);


    //element anyTest
    testEle = (DOMElement *)back->getNextSibling()->getNextSibling();
    back = testEle;

    testAttr = testEle->getAttributeNodeNS(X("http://www.secondSchema"), X("attr1"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_STRING, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(X("http://www.secondSchema"), X("attr2"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_INTEGER, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(X("http://www.secondSchema"), X("attr3"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_STRING, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(X("http://www.secondSchema"), X("attr4"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_INTEGER, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), X("anyTestType"), X(""), __LINE__);

    testEle = (DOMElement *)testEle->getFirstChild()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), SchemaSymbols::fgDT_STRING, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), SchemaSymbols::fgDT_STRING, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), 0, 0, __LINE__);

    //anyTestAttr1
    testEle = (DOMElement *)back->getNextSibling()->getNextSibling();
    back = testEle;

    testAttr = testEle->getAttributeNodeNS(X("http://www.secondSchema"), X("attr5"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), 0, 0, __LINE__);

    //anyTestAttr2
    testEle = (DOMElement *)back->getNextSibling()->getNextSibling();
    back = testEle;

    testAttr = testEle->getAttributeNodeNS(X("http://www.secondSchema"), X("attr5"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), 0, 0, __LINE__);

    //anyTestMissing
    testEle = (DOMElement *)back->getNextSibling()->getNextSibling();
    back = testEle;
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), X("anyTestType"), X(""), __LINE__);
    return passed;
}



bool TypeInfo::testInvaild() {

    bool passed = true;
    DOMNode *docEle = doc->getDocumentElement();
    UNUSED(docEle); // silence warning
    DOMAttr *testAttr;


    DOMTYPEINFOTEST(((DOMElement *)docEle)->getSchemaTypeInfo(), X("rootType"), X(""), __LINE__);

    //element invalidTest
    DOMElement *testEle = findElement(X("invalidTest"));

    testAttr = testEle->getAttributeNodeNS(X(""), X("simple"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), X("stringDerivedType"), X(""), __LINE__);

    testAttr = testEle->getAttributeNodeNS(X(""), X("invalid"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), 0, 0, __LINE__);

    testEle = (DOMElement *)testEle->getFirstChild()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), X("stringDerivedType"), X(""), __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), X("baseComplex"), X(""), __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    // this is a "number" of type "base" but it has a xsi:type="stringDerivedType"
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), X("stringDerivedType"), X(""), __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), X("baseComplex"), X(""), __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), X("uType"), X(""), __LINE__);
    //an undeclared element does not have anon value. Test this here
    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), 0, 0, __LINE__);

    DOMElement *testEle2 = (DOMElement *)testEle->getFirstChild()->getNextSibling();
    DOMTYPEINFOTEST(testEle2->getSchemaTypeInfo(), 0, 0, __LINE__);
    
    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), X("attrOnlyType"), X(""), __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), X("attrOnlyType"), X(""), __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), X("prohibitedAttrType"), X(""), __LINE__);

    return passed;
}


bool TypeInfo::compareDOMTypeInfo(const DOMTypeInfo *info, const XMLCh* type, const XMLCh* uri) {
    return XMLString::equals(info->getTypeName(), type) && XMLString::equals(info->getTypeNamespace(), uri);
}


bool TypeInfo::testDTD() {

    bool passed = true;
    DOMElement *testEle = doc->getDocumentElement();
    DOMAttr *testAttr;

    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), X(""), X(""), __LINE__);

    testAttr = testEle->getAttributeNodeNS(X(""), X("cdata"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), X("CDATA"), X("http://www.w3.org/TR/REC-xml"), __LINE__);

    testAttr = testEle->getAttributeNodeNS(X(""), X("enum"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), X("ENUMERATION"), X("http://www.w3.org/TR/REC-xml"), __LINE__);

    testAttr = testEle->getAttributeNodeNS(X(""), X("id"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), X("ID"), X("http://www.w3.org/TR/REC-xml"), __LINE__);

    testAttr = testEle->getAttributeNodeNS(X(""), X("idRef"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), X("IDREF"), X("http://www.w3.org/TR/REC-xml"), __LINE__);

    testAttr = testEle->getAttributeNodeNS(X(""), X("idRefs"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), X("IDREFS"), X("http://www.w3.org/TR/REC-xml"), __LINE__);

    testAttr = testEle->getAttributeNodeNS(X(""), X("nmToken"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), X("NMTOKEN"), X("http://www.w3.org/TR/REC-xml"), __LINE__);

    testAttr = testEle->getAttributeNodeNS(X(""), X("nmTokenDefault"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), X("NMTOKEN"), X("http://www.w3.org/TR/REC-xml"), __LINE__);

    testAttr = testEle->getAttributeNodeNS(X(""), X("nmTokenDefault2"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), X("NMTOKEN"), X("http://www.w3.org/TR/REC-xml"), __LINE__);

    testAttr = testEle->getAttributeNodeNS(X(""), X("nmTokens"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), X("NMTOKENS"), X("http://www.w3.org/TR/REC-xml"), __LINE__);

    testAttr = testEle->getAttributeNodeNS(X(""), X("entity"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), X("ENTITY"), X("http://www.w3.org/TR/REC-xml"), __LINE__);

    testAttr = testEle->getAttributeNodeNS(X(""), X("entities"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), X("ENTITIES"), X("http://www.w3.org/TR/REC-xml"), __LINE__);

    testAttr = testEle->getAttributeNodeNS(X(""), X("notation"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), X("NOTATION"), X("http://www.w3.org/TR/REC-xml"), __LINE__);

    testAttr = testEle->getAttributeNodeNS(X(""), X("noDecl"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), X("CDATA"), X("http://www.w3.org/TR/REC-xml"), __LINE__);

    testAttr = testEle->getAttributeNode(X("xmlns:foo"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), X("CDATA"), X("http://www.w3.org/TR/REC-xml"), __LINE__);

    return passed;

}

bool TypeInfo::combinedTest() {
    bool passed = true;
    DOMNode *docEle = doc->getDocumentElement();
    UNUSED(docEle); // silence warning
    DOMAttr *testAttr;

    DOMElement *testEle = doc->getDocumentElement();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), X("rootType"), X(""), __LINE__);

    testAttr = testEle->getAttributeNodeNS(X(""), X("attBoth"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_STRING, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(X(""), X("attSchema"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), SchemaSymbols::fgDT_STRING, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testAttr = testEle->getAttributeNodeNS(X(""), X("attDTD"));
    DOMTYPEINFOTEST(testAttr->getSchemaTypeInfo(), 0, 0, __LINE__);

    testEle = (DOMElement *)testEle->getFirstChild()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), SchemaSymbols::fgDT_STRING, SchemaSymbols::fgURI_SCHEMAFORSCHEMA, __LINE__);

    testEle = (DOMElement *)testEle->getNextSibling()->getNextSibling();
    DOMTYPEINFOTEST(testEle->getSchemaTypeInfo(), 0, 0, __LINE__);

    return passed;
}


DOMElement* TypeInfo::findElement(const XMLCh *name) const {

    DOMNode *toTest = doc->getDocumentElement()->getFirstChild();

    while(!XMLString::equals(toTest->getNodeName(), name)) {
        toTest = toTest->getNextSibling();
    }

    return (DOMElement *)toTest;
}

int main(int /*argc*/, char ** /*argv*/)
{
    bool passed = true;

    TypeInfo ti;

    try {
        //first the combined DTD/Schema scanner
        ti.parser = new XercesDOMParser;
        ti.parser->setValidationScheme(XercesDOMParser::Val_Auto);
        ti.parser->setCreateSchemaInfo(true);
        ti.parser->setDoNamespaces(true);
        ti.parser->setDoSchema(true);
        ti.parser->parse("data/TypeInfo.xml");
        ti.doc = ti.parser->getDocument();
    }
    catch (...) {
        XERCES_STD_QUALIFIER cerr << "parsing data/TypeInfo.xml failed at line" <<  __LINE__ << XERCES_STD_QUALIFIER endl;
        delete ti.parser;
        return false;
    }

    // test only if we got a doc
    if (ti.doc) {
        passed &= ti.testInBuiltTypesOnAttributes(true);
        passed &= ti.testInBuiltTypesOnElements();
        passed &= ti.testSimpleDerived();
        passed &= ti.testComplexTypes();
        passed &= ti.testUnions();
        passed &= ti.testAnonymous();
        passed &= ti.testXsiTypes();
        passed &= ti.testAnys();
        passed &= ti.testInvaild();
    }
    else
        XERCES_STD_QUALIFIER cout << "DOMTypeInfo test at line " << __LINE__ << "was not carried out" << XERCES_STD_QUALIFIER endl;

    delete ti.parser;

    //lets do the same for the just schema scanner
    try {
        ti.parser = new XercesDOMParser;
        ti.parser->setValidationScheme(XercesDOMParser::Val_Auto);
        ti.parser->setCreateSchemaInfo(true);
        ti.parser->setDoNamespaces(true);
        ti.parser->setDoSchema(true);
        ti.parser->useScanner(X("SGXMLScanner"));
        ti.parser->parse("data/TypeInfoNoDTD.xml");
        ti.doc = ti.parser->getDocument();
    }
    catch (...) {
        XERCES_STD_QUALIFIER cerr << "parsing data/TypeInfoNoDTD.xml failed at line" <<  __LINE__ << XERCES_STD_QUALIFIER endl;
        delete ti.parser;
        return false;
    }

    // test only if we got a doc
    if (ti.doc) {
        passed &= ti.testInBuiltTypesOnAttributes(false);
        passed &= ti.testInBuiltTypesOnElements();
        passed &= ti.testSimpleDerived();
        passed &= ti.testComplexTypes();
        passed &= ti.testUnions();
        passed &= ti.testAnonymous();
        passed &= ti.testXsiTypes();
        passed &= ti.testAnys();
        passed &= ti.testInvaild();
    }
    else
        XERCES_STD_QUALIFIER cout << "DOMTypeInfo test at line " << __LINE__ << "was not carried out" << XERCES_STD_QUALIFIER endl;

    delete ti.parser;


    //now default for DTD
    try {
        ti.parser = new XercesDOMParser;
        ti.parser->setValidationScheme(XercesDOMParser::Val_Auto);
        ti.parser->setCreateSchemaInfo(true);
        ti.parser->parse("data/TypeInfoJustDTD.xml");
        ti.doc = ti.parser->getDocument();
    }
    catch (...) {
        XERCES_STD_QUALIFIER cerr << "parsing data/TypeInfoJustDTD.xml failed at line" <<  __LINE__ << XERCES_STD_QUALIFIER endl;
        delete ti.parser;
        return false;
    }

    // test only if we got a doc
    if (ti.doc) {
        passed &= ti.testDTD();
    }
    else
        XERCES_STD_QUALIFIER cout << "DOMTypeInfo test at line " << __LINE__ << "was not carried out" << XERCES_STD_QUALIFIER endl;

    delete ti.parser;


    //and specific scanner
    try {
        ti.parser = new XercesDOMParser;
        ti.parser->setValidationScheme(XercesDOMParser::Val_Auto);
        ti.parser->setCreateSchemaInfo(true);
        ti.parser->useScanner(X("DGXMLScanner"));
        ti.parser->parse("data/TypeInfoJustDTD.xml");
        ti.doc = ti.parser->getDocument();
    }
    catch (...) {
        XERCES_STD_QUALIFIER cerr << "parsing data/TypeInfoJustDTD.xml failed at line" <<  __LINE__ << XERCES_STD_QUALIFIER endl;
        delete ti.parser;
        return false;
    }

    // test only if we got a doc
    if (ti.doc) {
        passed &=  ti.testDTD();
    }
    else
        XERCES_STD_QUALIFIER cout << "DOMTypeInfo test at line " << __LINE__ << "was not carried out" << XERCES_STD_QUALIFIER endl;

    delete ti.parser;

    try {
        ti.parser = new XercesDOMParser;
        ti.parser->setValidationScheme(XercesDOMParser::Val_Auto);
        ti.parser->setCreateSchemaInfo(true);
        ti.parser->setDoNamespaces(true);
        ti.parser->setDoSchema(true);
        ti.parser->parse("data/combined.xml");
        ti.doc = ti.parser->getDocument();
    }
    catch (...) {
        XERCES_STD_QUALIFIER cerr << "parsing data/combined.xml failed at line" <<  __LINE__ << XERCES_STD_QUALIFIER endl;
        delete ti.parser;
        return false;
    }

    // test only if we got a doc
    if (ti.doc) {
        passed &= ti.combinedTest();
    }
    else
        XERCES_STD_QUALIFIER cout << "DOMTypeInfo test at line " << __LINE__ << "was not carried out" << XERCES_STD_QUALIFIER endl;

    delete ti.parser;

    if (!passed) {
        XERCES_STD_QUALIFIER cerr << "test failed" << XERCES_STD_QUALIFIER endl;
        return 4;
    }

    XERCES_STD_QUALIFIER cerr << "Test Run Successfully" << XERCES_STD_QUALIFIER endl;
    return 0;
}
