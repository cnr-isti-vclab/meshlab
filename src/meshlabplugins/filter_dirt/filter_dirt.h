/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2007                                                \/)\/    *
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

#ifndef FILTERDIRTPLUGIN_H
#define FILTERDIRTPLUGIN_H

#include <QObject>
#include <QStringList>
#include <QString>
#include <common/interfaces/filter_plugin_interface.h>
#include<vector>
#include<vcg/complex/complex.h>
//#include "muParser.h"

using namespace vcg;
//using namespace mu;


class FilterDirt : public QObject, public FilterPluginInterface
{
    Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(FILTER_PLUGIN_INTERFACE_IID)
    Q_INTERFACES(FilterPluginInterface)
protected:
    double x,y,z,nx,ny,nz,r,g,b,q,rad;
    //double x0,y0,z0,x1,y1,z1,x2,y2,z2,nx0,ny0,nz0,nx1,ny1,nz1,nx2,ny2,nz2,r0,g0,b0,r1,g1,b1,r2,g2,b2,q0,q1,q2;
    double v,f,v0i,v1i,v2i;
    std::vector<std::string> v_attrNames;
    std::vector<double> v_attrValue;
    //std::vector<std::string> f_attrNames;
    //std:: vector<double> f_attrValue;
    std::vector<CMeshO::PerVertexAttributeHandle<float> > vhandlers;
    //std::vector<CMeshO::PerFaceAttributeHandle<float> > fhandlers;

public:
    enum {FP_DIRT,FP_CLOUD_MOVEMENT} ;

    FilterDirt();
    ~FilterDirt(){};

    QString pluginName() const;
    virtual QString filterName(FilterIDType filter) const;
    virtual QString filterInfo(FilterIDType filter) const;
    virtual int getRequirements(const QAction*);
    virtual bool autoDialog(QAction *) {return true;}
    //      virtual void initParameterSet(QAction* filter,MeshModel &,RichParameterSet &){};
    virtual void initParameterList(const QAction*, MeshDocument &/*m*/, RichParameterList & /*parent*/);
    virtual bool applyFilter(const QAction* filter, MeshDocument &md, unsigned int& postConditionMask, const RichParameterList & par, vcg::CallBackPos *cb);
    virtual int postCondition(const QAction*) const;
    virtual FilterClass getClass (const QAction *) const;
    FILTER_ARITY filterArity(const QAction*) const {return SINGLE_MESH;}
};


#endif
