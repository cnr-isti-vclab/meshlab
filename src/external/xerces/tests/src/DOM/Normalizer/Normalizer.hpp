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

//  define null for compatibility with original Java source code.
#define null 0

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMErrorHandler.hpp>

XERCES_CPP_NAMESPACE_USE

XERCES_CPP_NAMESPACE_BEGIN
class XercesDOMParser;
XERCES_CPP_NAMESPACE_END




class Normalizer : public DOMErrorHandler {

public:
	Normalizer();
	~Normalizer();

    DOMDocument* createDocument();
    void serializeNode(const DOMNode *const node);

    void printEntityRefNodes(DOMElement *ele);
    bool handleError(const DOMError& domError);
    void resetErrors(){};

private:
    Normalizer(const Normalizer&);
    void operator=(const Normalizer&);


	XercesDOMParser *parser;
    DOMDocument *doc;


};
