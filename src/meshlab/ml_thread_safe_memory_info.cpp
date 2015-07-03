/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/

#include "ml_thread_safe_memory_info.h"

MLThreadSafeMemoryInfo::MLThreadSafeMemoryInfo( long long unsigned int originalmem ) 
	:vcg::NotThreadSafeMemoryInfo(originalmem),lock(QReadWriteLock::Recursive)
{

}

MLThreadSafeMemoryInfo::~MLThreadSafeMemoryInfo()
{
}

void MLThreadSafeMemoryInfo::acquiredMemory(long long unsigned int mem)
{
	QWriteLocker locker(&lock);
	vcg::NotThreadSafeMemoryInfo::acquiredMemory(mem);
}

long long unsigned int MLThreadSafeMemoryInfo::usedMemory() const
{
	QReadLocker locker(&lock);
	return vcg::NotThreadSafeMemoryInfo::usedMemory();
	
}

long long unsigned int MLThreadSafeMemoryInfo::currentFreeMemory() const
{
	QReadLocker locker(&lock);
	return vcg::NotThreadSafeMemoryInfo::currentFreeMemory();
}

void MLThreadSafeMemoryInfo::releasedMemory(long long unsigned int mem)
{
	QWriteLocker locker(&lock);
	vcg::NotThreadSafeMemoryInfo::releasedMemory(mem);
}

bool MLThreadSafeMemoryInfo::isAdditionalMemoryAvailable( long long unsigned int mem )
{
	QReadLocker locker(&lock);
	return vcg::NotThreadSafeMemoryInfo::isAdditionalMemoryAvailable(mem);
}
