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
#include "XMLHarnessHandlers.hpp"
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/validators/common/Grammar.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/util/BinInputStream.hpp>

// ---------------------------------------------------------------------------
//  XMLHarnessHandlers: Constructors and Destructor
// ---------------------------------------------------------------------------
XMLHarnessHandlers::XMLHarnessHandlers(const XMLCh* baseURL, const XMLCh* scanner) : BaseHarnessHandlers(baseURL)
, fTestBaseURL(5)
{
    fParser = XMLReaderFactory::createXMLReader();
    fParser->setProperty(XMLUni::fgXercesScannerName, (void*)scanner);
    fParser->setFeature(XMLUni::fgSAX2CoreValidation, true);
    fParser->setFeature(XMLUni::fgXercesDynamic, false);
    fParser->setErrorHandler(&fErrorHandler);
    
    fTestBaseURL.push(new XMLURL(fBaseURL));
}

XMLHarnessHandlers::~XMLHarnessHandlers()
{
    delete fParser;
}

static XMLCh szTest[]={ chLatin_T, chLatin_E, chLatin_S, chLatin_T, chNull };
static XMLCh szTestCases[]={ chLatin_T, chLatin_E, chLatin_S, chLatin_T, chLatin_C, chLatin_A, chLatin_S, chLatin_E, chLatin_S, chNull };
static XMLCh szID[]={ chLatin_I, chLatin_D, chNull };
static XMLCh szURI[]={ chLatin_U, chLatin_R, chLatin_I, chNull };
static XMLCh szType[]={ chLatin_T, chLatin_Y, chLatin_P, chLatin_E, chNull };
static XMLCh szValid[]={ chLatin_v, chLatin_a, chLatin_l, chLatin_i, chLatin_d, chNull };
static XMLCh szInvalid[]={ chLatin_i, chLatin_n, chLatin_v, chLatin_a, chLatin_l, chLatin_i, chLatin_d, chNull };
static XMLCh szNotWellFormed[]={ chLatin_n, chLatin_o, chLatin_t, chDash, chLatin_w, chLatin_f, chNull };
static XMLCh szError[]={ chLatin_e, chLatin_r, chLatin_r, chLatin_o, chLatin_r, chNull };
static XMLCh szBase[]={ chLatin_x, chLatin_m, chLatin_l, chColon, chLatin_b, chLatin_a, chLatin_s, chLatin_e, chNull };
static XMLCh szNamespace[]={ chLatin_N, chLatin_A, chLatin_M, chLatin_E, chLatin_S, chLatin_P, chLatin_A, chLatin_C, chLatin_E, chNull };
static XMLCh szNO[]={ chLatin_n, chLatin_o, chNull };
static XMLCh szVersion[] = { chLatin_V, chLatin_E, chLatin_R, chLatin_S, chLatin_I, chLatin_O, chLatin_N, chNull };
static XMLCh szEdition[] = { chLatin_E, chLatin_D, chLatin_I, chLatin_T, chLatin_I, chLatin_O, chLatin_N, chNull };
static XMLCh szFive[]={ chDigit_5, chNull };

// ---------------------------------------------------------------------------
//  XMLHarnessHandlers: Implementation of the SAX DocumentHandler interface
// ---------------------------------------------------------------------------
void XMLHarnessHandlers::startElement(const XMLCh* const /* uri */
                                   , const XMLCh* const localname
                                   , const XMLCh* const /* qname */
                                   , const Attributes& attrs)
{
    if(XMLString::equals(localname, szTestCases))
    {
        const XMLCh* baseUrl=attrs.getValue(szBase);
        XMLURL newBase;
        XMLURL prevBase=*fTestBaseURL.peek();
        if(baseUrl!=NULL)
            newBase.setURL(prevBase, baseUrl);
        else
            newBase=prevBase;
        fTestBaseURL.push(new XMLURL(newBase));
    }
    else if(XMLString::equals(localname, szTest))
    {
        const XMLCh* useNS=attrs.getValue(szNamespace);
        const XMLCh* testName=attrs.getValue(szID);
        const XMLCh* version=attrs.getValue(szVersion);
        if(version == NULL || XMLString::equals(version, XMLUni::fgVersion1_0))
        {
            const XMLCh* editions=attrs.getValue(szEdition);
            // skip tests that don't apply to v.1.0 5th Edition
            if(editions)
            {
                BaseRefVectorOf<XMLCh>* tokens = XMLString::tokenizeString(editions);
                bool appliesTo5 = false;
                for (XMLSize_t i = 0; i < tokens->size(); i++) {
                    if (XMLString::equals(tokens->elementAt(i), szFive)) {
                        appliesTo5 = true;
                        break;
                    }
                }
                delete tokens;
                if(!appliesTo5)
                    return;
            }
        }

        XMLURL testSet;
        testSet.setURL(*fTestBaseURL.peek(), attrs.getValue(szURI));
        bool success=true, fatalFailure=false;
        try
        {
            if(useNS!=NULL && XMLString::equals(useNS, szNO))
            {
                fParser->setFeature(XMLUni::fgSAX2CoreNameSpaces, false);
                fParser->setFeature(XMLUni::fgSAX2CoreNameSpacePrefixes, false);
            }
            else
            {
                fParser->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);
                fParser->setFeature(XMLUni::fgSAX2CoreNameSpacePrefixes, true);
            }
            fErrorHandler.resetErrors();
            fParser->parse(testSet.getURLText());
        }
        catch (const OutOfMemoryException&)
        {
            fatalFailure=true;
            XERCES_STD_QUALIFIER cout << "Test " << StrX(testName) << " ran out of memory" << XERCES_STD_QUALIFIER endl;
            success=false;
        }
        catch(const XMLException& exc)
        {
            XERCES_STD_QUALIFIER cout << "Test " << StrX(testName) << " threw " << StrX(exc.getMessage()) << XERCES_STD_QUALIFIER endl;
            success=false;
        }
        catch (...)
        {
            fatalFailure=true;
            XERCES_STD_QUALIFIER cout << "Test " << StrX(testName) << " crashed" << XERCES_STD_QUALIFIER endl;
            success=false;
            exit(1);
        }
        fTests++;
        if(fatalFailure)
        {
            fFailures++;
            printFile(testSet);
        }
        else
        {
            ValidityOutcome expResult=unknown;
            const XMLCh* validity=attrs.getValue(szType);
            if(XMLString::equals(validity, szValid))
                expResult=valid;
            else if(XMLString::equals(validity, szInvalid) || XMLString::equals(validity, szNotWellFormed) || XMLString::equals(validity, szError) )
                expResult=invalid;
            else
                XERCES_STD_QUALIFIER cerr << "Unknown result type " << StrX(validity) << XERCES_STD_QUALIFIER endl;
            if(success && !fErrorHandler.getSawErrors())
            {
                if(expResult!=valid)
                {
                    fFailures++;
                    XERCES_STD_QUALIFIER cout << "Test " << StrX(testName) << " succeeded but was expected to fail" << XERCES_STD_QUALIFIER endl;
                    printFile(testSet);
                }
            }
            else
            {
                if(expResult!=invalid)
                {
                    fFailures++;
                    XERCES_STD_QUALIFIER cout << "Test " << StrX(testName) << " failed but was expected to pass" << XERCES_STD_QUALIFIER endl;
                    XERCES_STD_QUALIFIER cout << "Reported error: " << StrX(fErrorHandler.getErrorText()) << XERCES_STD_QUALIFIER endl;
                    printFile(testSet);
                }
            }
        }
    }
}

void XMLHarnessHandlers::endElement(const XMLCh* const /* uri */, const XMLCh* const localname, const XMLCh* const /* qname */)
{
    if(XMLString::equals(localname, szTestCases))
    {
        fTestBaseURL.pop();
    }
}
