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

#if !defined(XERCESC_INCLUDE_GUARD_DOMCASTS_HPP)
#define XERCESC_INCLUDE_GUARD_DOMCASTS_HPP

//
//  This file is part of the internal implementation of the C++ XML DOM.
//  It should NOT be included or used directly by application programs.
//
//  Applications should include the file <xercesc/dom/DOM.hpp> for the entire
//  DOM API, or xercesc/dom/DOM*.hpp for individual DOM classes, where the class
//  name is substituded for the *.
//

//
//  Define inline casting functions to convert from
//    (DOMNode*) to the embedded instances of DOMNodeImpl,
//    DOMParentNode, and DOMChildNode.
//
//  Each type of embedded object corresponds to a HasXXX virtual
//  interface class that a given DOM implementation class will
//  support to expose its embedded object(s) to other implementation
//  classes.
//
//  This replaces the previous implementation that relied upon unsafe
//  casts and member offsets that rely on unspecified behavior in C++,
//  with a hopefully small cost in memory and performance.
//

#include <xercesc/dom/DOMException.hpp>
#include "DOMNodeBase.hpp"
#include "DOMElementImpl.hpp"
#include "DOMTextImpl.hpp"

XERCES_CPP_NAMESPACE_BEGIN


static inline const DOMNodeImpl *castToNodeImpl(const DOMNode *p)
{
    const HasDOMNodeImpl* pE = dynamic_cast<const HasDOMNodeImpl*>(p);
    if (!pE || !pE->getNodeImpl()) {
        throw DOMException(DOMException::INVALID_STATE_ERR, 0, XMLPlatformUtils::fgMemoryManager);
    }
    return pE->getNodeImpl();
}

static inline DOMNodeImpl *castToNodeImpl(DOMNode *p)
{
    HasDOMNodeImpl *pE = dynamic_cast<HasDOMNodeImpl*>(p);
    if (!pE || !pE->getNodeImpl()) {
        throw DOMException(DOMException::INVALID_STATE_ERR, 0, XMLPlatformUtils::fgMemoryManager);
    }
    return pE->getNodeImpl();
}

static inline const DOMParentNode *castToParentImpl(const DOMNode *p) {
    const HasDOMParentImpl *pE = dynamic_cast<const HasDOMParentImpl*>(p);
    if (!pE || !pE->getParentNodeImpl()) {
        throw DOMException(DOMException::INVALID_STATE_ERR, 0, XMLPlatformUtils::fgMemoryManager);
    }
    return pE->getParentNodeImpl();
}

static inline DOMParentNode *castToParentImpl(DOMNode *p) {
    HasDOMParentImpl *pE = dynamic_cast<HasDOMParentImpl*>(p);
    if (!pE || !pE->getParentNodeImpl()) {
        throw DOMException(DOMException::INVALID_STATE_ERR, 0, XMLPlatformUtils::fgMemoryManager);
    }
    return pE->getParentNodeImpl();
}

static inline const DOMChildNode *castToChildImpl(const DOMNode *p) {
    const HasDOMChildImpl *pE = dynamic_cast<const HasDOMChildImpl*>(p);
    if (!pE || !pE->getChildNodeImpl()) {
        throw DOMException(DOMException::INVALID_STATE_ERR, 0, XMLPlatformUtils::fgMemoryManager);
    }
    return pE->getChildNodeImpl();
}

static inline DOMChildNode *castToChildImpl(DOMNode *p) {
    HasDOMChildImpl *pE = dynamic_cast<HasDOMChildImpl*>(p);
    if (!pE || !pE->getChildNodeImpl()) {
        throw DOMException(DOMException::INVALID_STATE_ERR, 0, XMLPlatformUtils::fgMemoryManager);
    }
    return pE->getChildNodeImpl();
}

XERCES_CPP_NAMESPACE_END

#endif
