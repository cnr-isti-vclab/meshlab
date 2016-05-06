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

#ifndef __ML_RENDER_GUI_H
#define __ML_RENDER_GUI_H

#include <QPushButton>
#include <QToolBar>
#include <QFrame>
#include <wrap/gl/gl_mesh_attributes_info.h>
#include "additionalgui.h"

class MLRenderToolbar : public QToolBar
{
    Q_OBJECT
public:
    MLRenderToolbar(vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY_MASK defaultrend,QWidget* parent = NULL);
    MLRenderToolbar(unsigned int meshid,vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY_MASK defaultrend,QWidget* parent = NULL);

    ~MLRenderToolbar();
public slots:
     void setPrimitiveModality(vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY_MASK mask);
private slots:
    void toggleBBox();
    void togglePoints();
    void toggleEdges();
    void toggleSolid();
signals:
    void primitiveModalityUpdateRequested(unsigned int,vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY_MASK);
    //void primitiveModalityUpdateRequested(vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY_MASK);
private:
    QAction* _bboxact;
    QAction* _pointsact;
    QAction* _edgeact;
    QAction* _solidact;
    

    void initGui();
    void toggle(QAction* but,const vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY_MASK relatedprimit);
    void checkPrimitiveButtonsAccordingToMask();
    //if meshid is -1 it means that the actions are intended to be deployed to all the document and not to a specific mesh model
    unsigned int _meshid;
    vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY_MASK _mask;
};

class MLRenderParametersFrame : public QFrame
{
    Q_OBJECT
public:
    MLRenderParametersFrame(QWidget* parent = NULL);
};

#endif