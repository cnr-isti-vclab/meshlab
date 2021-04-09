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
#include "SAX2FilterHandlers.hpp"
#include <xercesc/sax2/Attributes.hpp>

struct Attr
{
    const XMLCh* qName;
    const XMLCh* uri;
    const XMLCh* localPart;
    const XMLCh* value;
    const XMLCh* attrType;
};

class AttrList : public Attributes, public RefVectorOf<Attr>
{
public:
    AttrList(XMLSize_t count) : RefVectorOf<Attr>(count) {}

    virtual XMLSize_t getLength() const
    {
        return size();
    }

    virtual const XMLCh* getURI(const XMLSize_t index) const
    {
        return elementAt(index)->uri;
    }
    virtual const XMLCh* getLocalName(const XMLSize_t index) const
    {
        return elementAt(index)->localPart;
    }
    virtual const XMLCh* getQName(const XMLSize_t index) const
    {
        return elementAt(index)->qName;
    }
    virtual const XMLCh* getType(const XMLSize_t index) const
    {
        return elementAt(index)->attrType;
    }
    virtual const XMLCh* getValue(const XMLSize_t index) const
    {
        return elementAt(index)->value;
    }


    virtual bool getIndex(const XMLCh* const uri,
                          const XMLCh* const localPart,
                          XMLSize_t& i) const
    {
        for(i=0;i<size();i++)
            if(XMLString::equals(elementAt(i)->uri,uri) && XMLString::equals(elementAt(i)->localPart,localPart))
                return true;
        return false;
    }

    virtual int getIndex(const XMLCh* const uri, const XMLCh* const localPart ) const
    {
        for(XMLSize_t i=0;i<size();i++)
            if(XMLString::equals(elementAt(i)->uri,uri) && XMLString::equals(elementAt(i)->localPart,localPart))
                return (int)i;
        return -1;
    }

    virtual bool getIndex(const XMLCh* const qName, XMLSize_t& i) const
    {
        for(i=0;i<size();i++)
            if(XMLString::equals(elementAt(i)->qName,qName))
                return true;
        return false;
    }

    virtual int getIndex(const XMLCh* const qName ) const
    {
        for(XMLSize_t i=0;i<size();i++)
            if(XMLString::equals(elementAt(i)->qName,qName))
                return (int)i;
        return -1;
    }

    virtual const XMLCh* getType(const XMLCh* const uri, const XMLCh* const localPart ) const
    {
        for(XMLSize_t i=0;i<size();i++)
            if(XMLString::equals(elementAt(i)->uri,uri) && XMLString::equals(elementAt(i)->localPart,localPart))
                return elementAt(i)->attrType;
        return NULL;
    }
    virtual const XMLCh* getType(const XMLCh* const qName) const
    {
        for(XMLSize_t i=0;i<size();i++)
            if(XMLString::equals(elementAt(i)->qName,qName))
                return elementAt(i)->attrType;
        return NULL;
    }

    virtual const XMLCh* getValue(const XMLCh* const uri, const XMLCh* const localPart ) const
    {
        for(XMLSize_t i=0;i<size();i++)
            if(XMLString::equals(elementAt(i)->uri,uri) && XMLString::equals(elementAt(i)->localPart,localPart))
                return elementAt(i)->value;
        return NULL;
    }
    virtual const XMLCh* getValue(const XMLCh* const qName) const
    {
        for(XMLSize_t i=0;i<size();i++)
            if(XMLString::equals(elementAt(i)->qName,qName))
                return elementAt(i)->value;
        return NULL;
    }

};

// ---------------------------------------------------------------------------
//  SAX2SortAttributesFilter: Constructors and Destructor
// ---------------------------------------------------------------------------
SAX2SortAttributesFilter::SAX2SortAttributesFilter(SAX2XMLReader* parent) : SAX2XMLFilterImpl(parent)
{
}

SAX2SortAttributesFilter::~SAX2SortAttributesFilter()
{
}


// ---------------------------------------------------------------------------
//  SAX2SortAttributesFilter: Overrides of the SAX2XMLFilter interface
// ---------------------------------------------------------------------------
void SAX2SortAttributesFilter::startElement(const   XMLCh* const    uri,
                                            const   XMLCh* const    localname,
                                            const   XMLCh* const    qname,
                                            const   Attributes&		attributes)
{
    AttrList sortedList(attributes.getLength());
    for(XMLSize_t i=0;i<attributes.getLength();i++)
    {
        XMLSize_t j;
        for(j=0;j<sortedList.getLength();j++)
        {
            if(XMLString::compareString(sortedList.elementAt(j)->qName,attributes.getQName(i))>=0)
                break;
        }
        Attr* pClone=new Attr;
        pClone->qName       = attributes.getQName(i);
        pClone->uri         = attributes.getURI(i);
        pClone->localPart   = attributes.getLocalName(i);
        pClone->value       = attributes.getValue(i);
        pClone->attrType    = attributes.getType(i);
        sortedList.insertElementAt(pClone, j);
    }
    SAX2XMLFilterImpl::startElement(uri, localname, qname, sortedList);
}
