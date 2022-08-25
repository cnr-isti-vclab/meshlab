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

#ifndef __ML_THREAD_SAFE_MEMORY_INFO_H
#define __ML_THREAD_SAFE_MEMORY_INFO_H

#include <QReadWriteLock>

#include <wrap/system/memory_info.h>


class MLThreadSafeMemoryInfo : public vcg::NotThreadSafeMemoryInfo
{
public:
	MLThreadSafeMemoryInfo(std::ptrdiff_t originalmem);

	~MLThreadSafeMemoryInfo();

	void acquiredMemory(std::ptrdiff_t mem);

	std::ptrdiff_t usedMemory() const;

	std::ptrdiff_t currentFreeMemory() const;

	void releasedMemory(std::ptrdiff_t mem = 0);
	
	bool isAdditionalMemoryAvailable(std::ptrdiff_t mem);
private:
	//mutable objects can be modified from the declared const functions
	//in this way we have not to modified the basic vcg::MemoryInfo interface for the logically const functions
	//whose need to lock the mutex for a simple reading operation
	mutable QReadWriteLock lock;
};

#endif
