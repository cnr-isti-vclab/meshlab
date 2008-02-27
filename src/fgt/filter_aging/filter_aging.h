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



#ifndef GEOMETRYAGINGPLUGIN_H
#define GEOMETRYAGINGPLUGIN_H

#include <QObject>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>


class GeometryAgingPlugin : public QObject, public MeshFilterInterface
{
    Q_OBJECT
    Q_INTERFACES(MeshFilterInterface)

    public:
        enum {FP_ERODE};
        typedef struct EP{
            float lx;       // x offset along the surface of the face on the left side of the edge
            float rx;       // x offset along the surface of the face on the right side of the edge
            float yrel;     // y relative position between the two edge vertexes
            EP(float yrel, float lx, float rx=0.0) {
                this->yrel = yrel;
                this->lx = lx;
                this->rx = rx;
            }
        } EroderPoint;

        GeometryAgingPlugin();
        virtual ~GeometryAgingPlugin();
        
        virtual const QString filterInfo(FilterIDType filter);
        virtual const QString filterName(FilterIDType filter);
        virtual const PluginInfo &pluginInfo();
        virtual const int getRequirements(QAction *) {return (MeshModel::MM_FACETOPO | MeshModel::MM_BORDERFLAG);}
        virtual bool autoDialog(QAction *) {return true;}
        virtual void initParameterSet(QAction *action, MeshModel &m, FilterParameterSet &params);
        virtual bool applyFilter(QAction *filter, MeshModel &m, FilterParameterSet &params, vcg::CallBackPos *cb);
        
    protected:
        virtual bool hasSelected(MeshModel &m);
        vector<EroderPoint>* generateEdgeEroder(bool border, float len);
        
};


#endif
