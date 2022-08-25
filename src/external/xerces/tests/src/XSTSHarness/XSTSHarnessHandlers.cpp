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
#include "XSTSHarnessHandlers.hpp"
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/validators/common/Grammar.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/parsers/SAX2XMLReaderImpl.hpp>

// ---------------------------------------------------------------------------
//  XSTSHarnessHandlers: Constructors and Destructor
// ---------------------------------------------------------------------------
XSTSHarnessHandlers::XSTSHarnessHandlers(const XMLCh* baseURL, const XMLCh* scanner) : BaseHarnessHandlers(baseURL)
{
    fParser = XMLReaderFactory::createXMLReader();
    fParser->setProperty(XMLUni::fgXercesScannerName, (void*)scanner);
    fParser->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);
    fParser->setFeature(XMLUni::fgSAX2CoreNameSpacePrefixes, true);
    fParser->setFeature(XMLUni::fgSAX2CoreValidation, true);
    fParser->setFeature(XMLUni::fgXercesSchema, true);
    fParser->setFeature(XMLUni::fgXercesHandleMultipleImports, true);
    fParser->setFeature(XMLUni::fgXercesSchemaFullChecking, true);
    fParser->setFeature(XMLUni::fgXercesDynamic, false);
    fParser->setFeature(XMLUni::fgXercesUseCachedGrammarInParse, true);
    fParser->setFeature(XMLUni::fgXercesIdentityConstraintChecking, true);
    fParser->setErrorHandler(&fErrorHandler);
    ((SAX2XMLReaderImpl*)fParser)->setXMLEntityResolver(&fEntityResolver);
}

XSTSHarnessHandlers::~XSTSHarnessHandlers()
{
    delete fParser;
}

static XMLCh urlW3C[]={ chLatin_h, chLatin_t, chLatin_t, chLatin_p, chColon, chForwardSlash, chForwardSlash,
                        chLatin_w, chLatin_w, chLatin_w, chPeriod, chLatin_w, chDigit_3, chPeriod, chLatin_o, chLatin_r, chLatin_g, chForwardSlash,
                        chLatin_X, chLatin_M, chLatin_L, chForwardSlash,
                        chLatin_S, chLatin_c, chLatin_h, chLatin_e, chLatin_m, chLatin_a, chNull };

static XMLCh szTestSuite[]={ chLatin_T, chLatin_e, chLatin_s, chLatin_t, chLatin_S, chLatin_u, chLatin_i, chLatin_t, chLatin_e, chNull };
static XMLCh szTestGroup[]={ chLatin_t, chLatin_e, chLatin_s, chLatin_t, chLatin_G, chLatin_r, chLatin_o, chLatin_u, chLatin_p, chNull };
static XMLCh szSchemaTest[]={ chLatin_s, chLatin_c, chLatin_h, chLatin_e, chLatin_m, chLatin_a, chLatin_T, chLatin_e, chLatin_s, chLatin_t, chNull };
static XMLCh szInstanceTest[]={ chLatin_i, chLatin_n, chLatin_s, chLatin_t, chLatin_a, chLatin_n, chLatin_c, chLatin_e, chLatin_T, chLatin_e, chLatin_s, chLatin_t, chNull };
static XMLCh szDocumentationReference[]={ chLatin_d, chLatin_o, chLatin_c, chLatin_u, chLatin_m, chLatin_e, chLatin_n, chLatin_t, chLatin_a, chLatin_t, chLatin_i, chLatin_o, chLatin_n,
                                          chLatin_R, chLatin_e, chLatin_f, chLatin_e, chLatin_r, chLatin_e, chLatin_n, chLatin_c, chLatin_e, chNull };
static XMLCh szSchemaDocument[]={ chLatin_s, chLatin_c, chLatin_h, chLatin_e, chLatin_m, chLatin_a, chLatin_D, chLatin_o, chLatin_c, chLatin_u, chLatin_m, chLatin_e, chLatin_n, chLatin_t, chNull };
static XMLCh szInstanceDocument[]={ chLatin_i, chLatin_n, chLatin_s, chLatin_t, chLatin_a, chLatin_n, chLatin_c, chLatin_e, chLatin_D, chLatin_o, chLatin_c, chLatin_u, chLatin_m, chLatin_e, chLatin_n, chLatin_t, chNull };
static XMLCh szExpected[]={ chLatin_e, chLatin_x, chLatin_p, chLatin_e, chLatin_c, chLatin_t, chLatin_e, chLatin_d, chNull };
static XMLCh szValidity[]={ chLatin_v, chLatin_a, chLatin_l, chLatin_i, chLatin_d, chLatin_i, chLatin_t, chLatin_y, chNull };

static XMLCh szXLINK[]={ chLatin_h, chLatin_t, chLatin_t, chLatin_p, chColon, chForwardSlash, chForwardSlash,
                         chLatin_w, chLatin_w, chLatin_w, chPeriod, chLatin_w, chDigit_3, chPeriod, chLatin_o, chLatin_r, chLatin_g, chForwardSlash,
                         chDigit_1, chDigit_9, chDigit_9, chDigit_9, chForwardSlash,
                         chLatin_x, chLatin_l, chLatin_i, chLatin_n, chLatin_k, chNull };
static XMLCh szHREF[]={ chLatin_h, chLatin_r, chLatin_e, chLatin_f, chNull };
static XMLCh szNAME[]={ chLatin_n, chLatin_a, chLatin_m, chLatin_e, chNull };
static XMLCh szVALID[]={ chLatin_v, chLatin_a, chLatin_l, chLatin_i, chLatin_d, chNull };
static XMLCh szINVALID[]={ chLatin_i, chLatin_n, chLatin_v, chLatin_a, chLatin_l, chLatin_i, chLatin_d, chNull };


static XMLCh szTestSuite2[]={ chLatin_h, chLatin_t, chLatin_t, chLatin_p, chColon, chForwardSlash, chForwardSlash,
                        chLatin_w, chLatin_w, chLatin_w, chPeriod, chLatin_w, chDigit_3, chPeriod, chLatin_o, chLatin_r, chLatin_g, chForwardSlash,
                        chLatin_X, chLatin_M, chLatin_L, chForwardSlash,
                        chDigit_2, chDigit_0, chDigit_0, chDigit_4, chForwardSlash,
                        chLatin_x, chLatin_m, chLatin_l, chDash, chLatin_s, chLatin_c, chLatin_h, chLatin_e, chLatin_m, chLatin_a, chDash,
                        chLatin_t, chLatin_e, chLatin_s, chLatin_t, chDash, chLatin_s, chLatin_u, chLatin_i, chLatin_t, chLatin_e, chForwardSlash, chNull };

static XMLCh szTestSetRef[]={ chLatin_t, chLatin_e, chLatin_s, chLatin_t, chLatin_S, chLatin_e, chLatin_t, chLatin_R, chLatin_e, chLatin_f, chNull };

// ---------------------------------------------------------------------------
//  XSTSHarnessHandlers: Implementation of the SAX DocumentHandler interface
// ---------------------------------------------------------------------------
void XSTSHarnessHandlers::startElement(const XMLCh* const uri
                                   , const XMLCh* const localname
                                   , const XMLCh* const /* qname */
                                   , const Attributes& attrs)
{
    if(XMLString::equals(uri, szTestSuite) || XMLString::equals(uri, szTestSuite2))
    {
        if(XMLString::equals(localname, szTestSetRef))
        {
            XMLURL testSet, backupBase(fBaseURL);
            testSet.setURL(fBaseURL, attrs.getValue(szXLINK, szHREF));

            fBaseURL=testSet;
            SAX2XMLReader* parser = XMLReaderFactory::createXMLReader();
            try
            {
                parser->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);
                parser->setContentHandler(this);
                parser->setErrorHandler(this);
                parser->parse(testSet.getURLText());
            }
            catch (...)
            {
            }
            delete parser;
            fBaseURL=backupBase;

        }
        else if(XMLString::equals(localname, szTestGroup))
        {
            fCurrentTest.fExpectedResult=unknown;
            fCurrentTest.fSpecReference.setURL(urlW3C);
            fCurrentTest.fTestName[0]=0;
            fCurrentTest.fXMLName.setURL(dummy);
            fCurrentTest.fXSDNames.removeAllElements();
            StrX x(attrs.getValue(szNAME));
            const char* groupName=x.localForm();
            if(XMLString::equals(groupName,"addB194") ||
               XMLString::equals(groupName,"particlesZ033_c") ||
               XMLString::equals(groupName,"particlesZ033_d") ||
               XMLString::equals(groupName,"particlesZ033_e") ||
               XMLString::equals(groupName,"particlesZ033_f") ||
               XMLString::equals(groupName,"particlesZ033_g") ||
               XMLString::equals(groupName,"particlesZ035_a") ||
               XMLString::equals(groupName,"particlesZ036_b1") ||
               XMLString::equals(groupName,"particlesZ036_b2") ||
               XMLString::equals(groupName,"particlesZ036_c") ||
               XMLString::equals(groupName,"wildG032")
               )
                fCurrentTest.fSkipped=true;
            else
                fCurrentTest.fSkipped=false;
            fParser->resetCachedGrammarPool();
        }
        else if(XMLString::equals(localname, szDocumentationReference))
        {
            const XMLCh* ref=attrs.getValue(szXLINK, szHREF);
            if(ref && *ref)
                fCurrentTest.fSpecReference.setURL(ref);
            else
                fCurrentTest.fSpecReference.setURL(dummy);
        }
        else if(XMLString::equals(localname, szSchemaTest) ||
                XMLString::equals(localname, szInstanceTest))
        {
            XMLString::copyString(fCurrentTest.fTestName, attrs.getValue(szNAME));
        }
        else if(XMLString::equals(localname, szSchemaDocument))
        {
            fCurrentTest.fXSDNames.addElement(new XMLURL(fBaseURL, attrs.getValue(szXLINK, szHREF)));
        }
        else if(XMLString::equals(localname, szInstanceDocument))
        {
            fCurrentTest.fXMLName.setURL(fBaseURL, attrs.getValue(szXLINK, szHREF));
        }
        else if(XMLString::equals(localname, szExpected))
        {
            const XMLCh* validity=attrs.getValue(szValidity);
            if(XMLString::equals(validity, szVALID))
                fCurrentTest.fExpectedResult=valid;
            else if(XMLString::equals(validity, szINVALID))
                fCurrentTest.fExpectedResult=invalid;
            else
                fCurrentTest.fExpectedResult=unknown;
        }
    }
}

void XSTSHarnessHandlers::endElement(const XMLCh* const uri,
	                                 const XMLCh* const localname,
	                                 const XMLCh* const /*qname*/)
{
    if(XMLString::equals(uri, szTestSuite) || XMLString::equals(uri, szTestSuite2))
    {
        if(XMLString::equals(localname, szSchemaTest))
        {
            if(fCurrentTest.fSkipped)
            {
                fTests++;
                fFailures++;
                XERCES_STD_QUALIFIER cout << "Test " << StrX(fCurrentTest.fTestName) << " skipped" << XERCES_STD_QUALIFIER endl;
                return;
            }
            bool success=true, fatalFailure=false;
            try
            {
                fErrorHandler.resetErrors();
                for(XMLSize_t i=0;i<fCurrentTest.fXSDNames.size();i++)
                {
                    Grammar* grammar=fParser->loadGrammar(fCurrentTest.fXSDNames.elementAt(i)->getURLText(), Grammar::SchemaGrammarType, true);
                    success=(success && (grammar!=NULL));
                }
            }
            catch (const OutOfMemoryException&)
            {
                fatalFailure=true;
                XERCES_STD_QUALIFIER cout << "Test " << StrX(fCurrentTest.fTestName) << " ran out of memory" << XERCES_STD_QUALIFIER endl;
                success=false;
            }
            catch(const XMLException& exc)
            {
                XERCES_STD_QUALIFIER cout << "Test " << StrX(fCurrentTest.fTestName) << " threw " << StrX(exc.getMessage()) << XERCES_STD_QUALIFIER endl;
                success=false;
            }
            catch (...)
            {
                fatalFailure=true;
                XERCES_STD_QUALIFIER cout << "Test " << StrX(fCurrentTest.fTestName) << " crashed" << XERCES_STD_QUALIFIER endl;
                success=false;
                exit(1);
            }
            fTests++;
            if(fatalFailure)
            {
                // skip the rest of the group, as we had problems with the schema itself
                fCurrentTest.fSkipped=true;
                fFailures++;
                for(XMLSize_t i=0;i<fCurrentTest.fXSDNames.size();i++)
                    printFile(*fCurrentTest.fXSDNames.elementAt(i));
            }
            else
            {
                if(success && !fErrorHandler.getSawErrors())
                {
                    if(fCurrentTest.fExpectedResult!=valid)
                    {
                        // skip the rest of the group, as we had problems with the schema itself
                        fCurrentTest.fSkipped=true;
                        fFailures++;
                        XERCES_STD_QUALIFIER cout << "Test " << StrX(fCurrentTest.fTestName) << " succeeded but was expected to fail" << XERCES_STD_QUALIFIER endl;
                        for(XMLSize_t i=0;i<fCurrentTest.fXSDNames.size();i++)
                            printFile(*fCurrentTest.fXSDNames.elementAt(i));
                    }
                }
                else
                {
                    if(fCurrentTest.fExpectedResult!=invalid)
                    {
                        // skip the rest of the group, as we had problems with the schema itself
                        fCurrentTest.fSkipped=true;
                        fFailures++;
                        XERCES_STD_QUALIFIER cout << "Test " << StrX(fCurrentTest.fTestName) << " failed but was expected to pass" << XERCES_STD_QUALIFIER endl;
                        XERCES_STD_QUALIFIER cout << "Reported error: " << StrX(fErrorHandler.getErrorText()) << XERCES_STD_QUALIFIER endl;
                        for(XMLSize_t i=0;i<fCurrentTest.fXSDNames.size();i++)
                            printFile(*fCurrentTest.fXSDNames.elementAt(i));
                    }
                }
            }
        }
        else if(XMLString::equals(localname, szInstanceTest))
        {
            if(fCurrentTest.fSkipped)
            {
                fTests++;
                fFailures++;
                XERCES_STD_QUALIFIER cout << "Test " << StrX(fCurrentTest.fTestName) << " skipped" << XERCES_STD_QUALIFIER endl;
                return;
            }
            bool success=true, fatalFailure=false;
            try
            {
                fErrorHandler.resetErrors();
                fParser->parse(fCurrentTest.fXMLName.getURLText());
            }
            catch (const OutOfMemoryException&)
            {
                fatalFailure=true;
                XERCES_STD_QUALIFIER cout << "Test " << StrX(fCurrentTest.fTestName) << " ran out of memory" << XERCES_STD_QUALIFIER endl;
                success=false;
            }
            catch(const XMLException& exc)
            {
                XERCES_STD_QUALIFIER cout << "Test " << StrX(fCurrentTest.fTestName) << " threw " << StrX(exc.getMessage()) << XERCES_STD_QUALIFIER endl;
                success=false;
            }
            catch (...)
            {
                fatalFailure=true;
                XERCES_STD_QUALIFIER cout << "Test " << StrX(fCurrentTest.fTestName) << " crashed" << XERCES_STD_QUALIFIER endl;
                success=false;
                exit(1);
            }
            fTests++;
            if(fatalFailure)
            {
                fFailures++;
                for(XMLSize_t i=0;i<fCurrentTest.fXSDNames.size();i++)
                    printFile(*fCurrentTest.fXSDNames.elementAt(i));
                printFile(fCurrentTest.fXMLName);
            }
            else
            {
                if(success && !fErrorHandler.getSawErrors())
                {
                    if(fCurrentTest.fExpectedResult!=valid)
                    {
                        fFailures++;
                        XERCES_STD_QUALIFIER cout << "Test " << StrX(fCurrentTest.fTestName) << " succeeded but was expected to fail" << XERCES_STD_QUALIFIER endl;
                        for(XMLSize_t i=0;i<fCurrentTest.fXSDNames.size();i++)
                            printFile(*fCurrentTest.fXSDNames.elementAt(i));
                        printFile(fCurrentTest.fXMLName);
                    }
                }
                else
                {
                    if(fCurrentTest.fExpectedResult!=invalid)
                    {
                        fFailures++;
                        XERCES_STD_QUALIFIER cout << "Test " << StrX(fCurrentTest.fTestName) << " failed but was expected to pass" << XERCES_STD_QUALIFIER endl;
                        XERCES_STD_QUALIFIER cout << "Reported error: " << StrX(fErrorHandler.getErrorText()) << XERCES_STD_QUALIFIER endl;
                        for(XMLSize_t i=0;i<fCurrentTest.fXSDNames.size();i++)
                            printFile(*fCurrentTest.fXSDNames.elementAt(i));
                        printFile(fCurrentTest.fXMLName);
                    }
                }
            }
        }
    }
}
