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
 * $Id:$
 */

#if !defined(XERCESC_INCLUDE_GUARD_DOMNODEBASE_HPP)
#define XERCESC_INCLUDE_GUARD_DOMNODEBASE_HPP

#include <xercesc/util/XercesDefs.hpp>

XERCES_CPP_NAMESPACE_BEGIN


class DOMNodeImpl;
class DOMParentNode;
class DOMChildNode;

//
//  This file is part of the internal implementation of the C++ XML DOM.
//  It should NOT be included or used directly by application programs.
//
//  Applications should include the file <xercesc/dom/DOM.hpp> for the entire
//  DOM API, or xercesc/dom/DOM*.hpp for individual DOM classes, where the class
//  name is substituded for the *.
//

/**
 * Virtual base class with accessors for shared characteristics of DOM implementation
 * types, this is a workaround for the current class design that allows the various
 * implementation classes to punch into the internals of each others members without
 * using unsafe casts that depend on object layout.
 */
class  CDOM_EXPORT HasDOMNodeImpl {
protected:
    // -----------------------------------------------------------------------
    //  Hidden constructor
    // -----------------------------------------------------------------------
    /** @name Hidden constructors */
    //@{
    HasDOMNodeImpl() {}
    //@}

private:
    // -----------------------------------------------------------------------
    // Unimplemented constructors and operators
    // -----------------------------------------------------------------------
    /** @name Unimplemented operators */
    //@{
    HasDOMNodeImpl & operator= (const HasDOMNodeImpl &);
    //@}

public:
    // -----------------------------------------------------------------------
    //  All constructors are hidden, just the destructor is available
    // -----------------------------------------------------------------------
    /** @name Destructor */
    //@{
    /**
     * Destructor
     *
     */
    virtual ~HasDOMNodeImpl() {};
    //@}

    // -----------------------------------------------------------------------
    //  Virtual HasDOMNodeImpl interface
    // -----------------------------------------------------------------------
    //@{
    // -----------------------------------------------------------------------
    //  Getter methods
    // -----------------------------------------------------------------------

    /**
     * Gets the embedded <code>DOMNodeImpl</code> member.
     */
    virtual DOMNodeImpl        *getNodeImpl() = 0;

    /**
     * Gets the embedded <code>DOMNodeImpl</code> member.
     */
    virtual const DOMNodeImpl  *getNodeImpl() const = 0;
    //@}
};

/**
 * Virtual base class with accessors for shared characteristics of DOM implementation
 * types, this is a workaround for the current class design that allows the various
 * implementation classes to punch into the internals of each others members without
 * using unsafe casts that depend on object layout.
 */
class  CDOM_EXPORT HasDOMParentImpl {
protected:
    // -----------------------------------------------------------------------
    //  Hidden constructor
    // -----------------------------------------------------------------------
    /** @name Hidden constructors */
    //@{
    HasDOMParentImpl() {}
    //@}

private:
    // -----------------------------------------------------------------------
    // Unimplemented constructors and operators
    // -----------------------------------------------------------------------
    /** @name Unimplemented operators */
    //@{
    HasDOMParentImpl & operator= (const HasDOMParentImpl &);
    //@}

public:
    // -----------------------------------------------------------------------
    //  All constructors are hidden, just the destructor is available
    // -----------------------------------------------------------------------
    /** @name Destructor */
    //@{
    /**
     * Destructor
     *
     */
    virtual ~HasDOMParentImpl() {};
    //@}

    // -----------------------------------------------------------------------
    //  Virtual HasDOMParentImpl interface
    // -----------------------------------------------------------------------
    //@{
    // -----------------------------------------------------------------------
    //  Getter methods
    // -----------------------------------------------------------------------

    /**
     * Gets the embedded <code>DOMParentNode</code> member.
     */
    virtual DOMParentNode      *getParentNodeImpl() = 0;

    /**
     * Gets the embedded <code>DOMParentNode</code> member.
     */
    virtual const DOMParentNode *getParentNodeImpl() const = 0;
    //@}
};

/**
 * Virtual base class with accessors for shared characteristics of DOM implementation
 * types, this is a workaround for the current class design that allows the various
 * implementation classes to punch into the internals of each others members without
 * using unsafe casts that depend on object layout.
 */
class  CDOM_EXPORT HasDOMChildImpl {
protected:
    // -----------------------------------------------------------------------
    //  Hidden constructor
    // -----------------------------------------------------------------------
    /** @name Hidden constructors */
    //@{
    HasDOMChildImpl() {}
    //@}

private:
    // -----------------------------------------------------------------------
    // Unimplemented constructors and operators
    // -----------------------------------------------------------------------
    /** @name Unimplemented operators */
    //@{
    HasDOMChildImpl & operator= (const HasDOMChildImpl &);
    //@}

public:
    // -----------------------------------------------------------------------
    //  All constructors are hidden, just the destructor is available
    // -----------------------------------------------------------------------
    /** @name Destructor */
    //@{
    /**
     * Destructor
     *
     */
    virtual ~HasDOMChildImpl() {};
    //@}

    // -----------------------------------------------------------------------
    //  Virtual HasDOMChildImpl interface
    // -----------------------------------------------------------------------
    //@{
    // -----------------------------------------------------------------------
    //  Getter methods
    // -----------------------------------------------------------------------

    /**
     * Gets the embedded <code>DOMChildNode</code> member.
     */
    virtual DOMChildNode      *getChildNodeImpl() = 0;

    /**
     * Gets the embedded <code>DOMChildNode</code> member.
     */
    virtual const DOMChildNode *getChildNodeImpl() const = 0;
    //@}
};

#define DOMNODEIMPL_DECL \
    virtual DOMNodeImpl* getNodeImpl(); \
    virtual const DOMNodeImpl* getNodeImpl() const;

#define DOMNODEIMPL_IMPL(classname) \
    DOMNodeImpl*         classname::getNodeImpl() {return &fNode;} \
    const DOMNodeImpl*   classname::getNodeImpl() const {return &fNode;}

#define DOMPARENTIMPL_DECL \
    virtual DOMParentNode* getParentNodeImpl(); \
    virtual const DOMParentNode* getParentNodeImpl() const;

#define DOMPARENTIMPL_IMPL(classname) \
    DOMParentNode*       classname::getParentNodeImpl() {return &fParent;} \
    const DOMParentNode* classname::getParentNodeImpl() const {return &fParent;}

#define DOMCHILDIMPL_DECL \
    virtual DOMChildNode* getChildNodeImpl(); \
    virtual const DOMChildNode* getChildNodeImpl() const;
    
#define DOMCHILDIMPL_IMPL(classname) \
    DOMChildNode*       classname::getChildNodeImpl() {return &fChild;} \
    const DOMChildNode* classname::getChildNodeImpl() const {return &fChild;}


XERCES_CPP_NAMESPACE_END

#endif
