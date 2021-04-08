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
#include <xercesc/internal/BinFileOutputStream.hpp>
#include <xercesc/util/Janitor.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLExceptMsgs.hpp>
#include <xercesc/util/XMLString.hpp>

XERCES_CPP_NAMESPACE_BEGIN

// ---------------------------------------------------------------------------
//  BinFileOutputStream: Constructors and Destructor
// ---------------------------------------------------------------------------
BinFileOutputStream::BinFileOutputStream(const XMLCh*   const fileName
                                         , MemoryManager* const manager)

:fSource(XMLPlatformUtils::openFileToWrite(fileName, manager))
,fMemoryManager(manager)
{
}

BinFileOutputStream::BinFileOutputStream(const char*    const fileName
                                       , MemoryManager* const manager)
:fSource(XMLPlatformUtils::openFileToWrite(fileName, manager))
,fMemoryManager(manager)
{
}

BinFileOutputStream::~BinFileOutputStream()
{
    if (getIsOpen())
    {
        try
        {
            XMLPlatformUtils::closeFile(fSource, fMemoryManager);
        }
        catch (...)
        {
            // There is nothing we can do about it here.
        }
    }
}

// ---------------------------------------------------------------------------
//  BinFileOutputStream: Getter methods
// ---------------------------------------------------------------------------
XMLFilePos BinFileOutputStream::getSize() const
{
    return XMLPlatformUtils::fileSize(fSource, fMemoryManager);
}


// ---------------------------------------------------------------------------
//  BinFileOutputStream: Stream management methods
// ---------------------------------------------------------------------------
void BinFileOutputStream::reset()
{
    XMLPlatformUtils::resetFile(fSource, fMemoryManager);
}


// ---------------------------------------------------------------------------
//  BinFileOutputStream: Implementation of the input stream interface
// ---------------------------------------------------------------------------
XMLFilePos BinFileOutputStream::curPos() const
{
    return XMLPlatformUtils::curFilePos(fSource, fMemoryManager);
}


void BinFileOutputStream::writeBytes( const XMLByte* const  toGo
                                    , const XMLSize_t       maxToWrite)
{
    //
    //  Write up to the maximum bytes requested. 
    //  

    XMLPlatformUtils::writeBufferToFile(fSource, maxToWrite, toGo, fMemoryManager);
}

XERCES_CPP_NAMESPACE_END
