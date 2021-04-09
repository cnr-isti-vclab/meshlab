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

#include <mutex>

#include <xercesc/util/MutexManagers/StdMutexMgr.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/RuntimeException.hpp>
#include <xercesc/util/PanicHandler.hpp>

XERCES_CPP_NAMESPACE_BEGIN


//	Wrap up the mutex with XMemory
class StdMutexWrap : public XMemory {
public:
    std::recursive_mutex m;
};


StdMutexMgr::StdMutexMgr()
{
}


StdMutexMgr::~StdMutexMgr()
{
}


XMLMutexHandle
StdMutexMgr::create(MemoryManager* const manager)
{
    StdMutexWrap* mutex = new (manager) StdMutexWrap;
    return (void*)(mutex);
}


void
StdMutexMgr::destroy(XMLMutexHandle mtx, MemoryManager* const manager)
{
    StdMutexWrap* mutex = (StdMutexWrap*)(mtx);
    if (mutex != NULL)
    {
        delete mutex;
    }
}


void
StdMutexMgr::lock(XMLMutexHandle mtx)
{
    StdMutexWrap* mutex = (StdMutexWrap*)(mtx);
    if (mutex != NULL)
    {
        mutex->m.lock();
    }
}


void
StdMutexMgr::unlock(XMLMutexHandle mtx)
{
    StdMutexWrap* mutex = (StdMutexWrap*)(mtx);
    if (mutex != NULL)
    {
        mutex->m.unlock();
    }
}


XERCES_CPP_NAMESPACE_END

