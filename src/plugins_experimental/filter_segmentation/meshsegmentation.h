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

//-------------------------------------------------------//
// File:  meshsegmentation.h                             //
//                                                       //
// Description: Mesh Segmentation Filter Header File     //
//                                                       //
// Authors: Anthousis Andreadis - http://anthousis.com   //
// Date: 7-Oct-2015                                      //
//                                                       //
// Computer Graphics Group                               //
// http://graphics.cs.aueb.gr/graphics/                  //
// AUEB - Athens University of Economics and Business    //
//                                                       //
//                                                       //
// This work was funded by the EU-FP7 - PRESIOUS project //
//-------------------------------------------------------//

#ifndef EXTRAFILTERSPLUGIN_H
#define EXTRAFILTERSPLUGIN_H

#include <common/interfaces.h>

class SegmentationPlugin : public QObject, public MeshFilterInterface
{
    Q_OBJECT
        MESHLAB_PLUGIN_IID_EXPORTER(MESH_FILTER_INTERFACE_IID)
        Q_INTERFACES(MeshFilterInterface)

    enum RefPlane { REF_CENTER, REF_MIN, REF_ORIG };

public:
    /* naming convention :
    - FP -> Filter Plugin
    - name of the filter separated by _
    */
    enum { FP_SEGMENTATION  };

    SegmentationPlugin();
    ~SegmentationPlugin(){}
    QString filterName(FilterIDType filter) const;
    QString filterInfo(FilterIDType filter) const;

    FilterClass getClass(QAction *);
    int getRequirements(QAction *);
    void initParameterSet(QAction *, MeshModel &/*m*/, RichParameterSet & /*parent*/);
    bool applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & /*parent*/, vcg::CallBackPos * cb);
    int postCondition(QAction * filter) const;

protected:
    float seg_eThreshold;   // we will hold the 1.f-cosf(seg_eThresholdDegrees*PI/180.f) as the error weight;
    float seg_eThresholdDegrees;
    bool  seg_localNeighbAware;
    float seg_localNeighbDist;
    float seg_areaThreshold;
    bool seg_hierarchical;
    bool seg_fifoRegionGrowing;
};

#endif
