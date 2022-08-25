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
#include "XSTSHarness.hpp"
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/util/XMLURL.hpp>
#include <xercesc/util/RefStackOf.hpp>

XERCES_CPP_NAMESPACE_USE

class XMLHarnessHandlers : public BaseHarnessHandlers 
{
public:
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    XMLHarnessHandlers(const XMLCh* baseURL, const XMLCh* scanner);
    ~XMLHarnessHandlers();

    // -----------------------------------------------------------------------
    //  Handlers for the SAX ContentHandler interface
    // -----------------------------------------------------------------------
    void startElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const Attributes& attrs);
    void endElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname);

private:
    SAX2XMLReader*      fParser;
    BaseErrorHandler    fErrorHandler;
    RefStackOf<XMLURL>  fTestBaseURL;
};

