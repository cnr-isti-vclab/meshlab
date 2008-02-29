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
/****************************************************************************
History
Revision 1.0 2008/02/20 Alessandro Maione, Federico Bellucci
FIRST RELEASE

****************************************************************************/

#ifndef _MESHMETHODS_H_
#define _MESHMETHODS_H_

#include "const_types.h"
#include "transferfunction.h"
#include <vcg/math/histogram.h>
#include <meshlab/meshmodel.h>
#include <cassert>
#include <utility> // for pair<T,T>

#include <QString>

using namespace std;
using namespace vcg;

// Applies colors to mesh vertexes by quality
void applyColorByVertexQuality(MeshModel& mesh, TransferFunction *transferFunction, float minQuality, float maxQuality, float midHandlePercentilePosition, float brightness);

// Opens a CSV file and gets its equalizer parameters
void loadEqualizerInfo(QString fileName, EQUALIZER_INFO *data);


#endif
