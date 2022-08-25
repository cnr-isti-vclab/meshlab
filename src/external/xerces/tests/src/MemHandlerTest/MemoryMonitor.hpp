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


#include <xercesc/framework/MemoryManager.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/dom/DOMLSParser.hpp>
#include <xercesc/dom/DOMErrorHandler.hpp>
#include <xercesc/dom/DOMError.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMImplementationRegistry.hpp>
#include <xercesc/parsers/AbstractDOMParser.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/parsers/SAXParser.hpp>
#include "SimpleValueHashTableOf.hpp"

XERCES_CPP_NAMESPACE_USE

/**
  * Configurable memory manager
  *
  * <p>This class is a memory manager implementation that keeps track of all
  * allocations/deallocations to ensure that all memory that it allocated is
  * deallocated.
  * </p>
  */

class MemoryMonitor : public MemoryManager
{
public:

    /** @name Constructor */
    //@{

    /**
      * Default constructor
      */
    MemoryMonitor()
    {
      fHashTable = new SimpleValueHashTableOf<unsigned int>(1013);
    }
    //@}


    /** @name Destructor */
    //@{

    /**
      * Default destructor
      */
    virtual ~MemoryMonitor()
    {
      delete fHashTable;
    }
    //@}

    virtual MemoryManager* getExceptionMemoryManager();

    /** @name The virtual methods in MemoryManager */
    //@{

    /**
      * This method allocates requested memory.
      *
      * @param size The requested memory size
      *
      * @return A pointer to the allocated memory
      */
    virtual void* allocate(XMLSize_t size);

    /**
      * This method deallocates memory
      *
      * @param p The pointer to the allocated memory to be deleted
      */
    virtual void deallocate(void* p);

    //@}

    // Print out amount of currently allocated memory
    unsigned int getTotalMemory();

private:
    // -----------------------------------------------------------------------
    //  Unimplemented constructors and operators
    // -----------------------------------------------------------------------
    MemoryMonitor(const MemoryMonitor &);
    MemoryMonitor& operator=(const MemoryMonitor &);
    SimpleValueHashTableOf<unsigned int>* fHashTable;

};
