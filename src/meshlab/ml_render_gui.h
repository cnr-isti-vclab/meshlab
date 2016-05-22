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
#include "ml_rendering_actions.h"
#include "additionalgui.h"


class MLRenderingToolbar : public QToolBar
{
    Q_OBJECT
public:
    MLRenderingToolbar(bool exclusive,QWidget* parent = NULL);
    MLRenderingToolbar(int meshid,bool exclusive,QWidget* parent = NULL);

    ~MLRenderingToolbar();

    void addRenderingAction( MLRenderingAction* act );
    void setAccordingToRenderingData(const MLRenderingData& dt);
    QList<MLRenderingAction*>& getRenderingActions();
    void getRenderingActionsCopy(QList<MLRenderingAction*>& acts,QObject* newparent = 0) const;
    void setAssociatedMeshId(int meshid);

protected:
    QList<MLRenderingAction*> _acts;
    int _meshid;
    MLRenderingAction* _previoussel;

private slots:
    void toggle(QAction* act);


signals:
    void updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>& acts);

private:
    //if meshid is -1 it means that the actions are intended to be deployed to all the document and not to a specific mesh model
    bool _exclusive;
};

class MLRenderingSideToolbar: public MLRenderingToolbar
{

    Q_OBJECT
public:
    MLRenderingSideToolbar(QWidget* parent = NULL);
    MLRenderingSideToolbar(int meshid,QWidget* parent = NULL);

    ~MLRenderingSideToolbar() {}
private:
    void initGui();
};

class MLRenderingParametersFrame : public QFrame
{
    Q_OBJECT
public:
    MLRenderingParametersFrame(int meshid,QWidget* parent);
    virtual ~MLRenderingParametersFrame();
    virtual void setPrimitiveButtonStatesAccordingToRenderingData(const MLRenderingData& dt) = 0;
    virtual void setAssociatedMeshId(int meshid) = 0;
    virtual void getAllRenderingActions(QList<MLRenderingAction*>& acts) = 0;
    static MLRenderingParametersFrame* factory(MLRenderingAction* act,int meshid,QWidget* parent);
    static void destroy(MLRenderingParametersFrame* pf);

protected:
    int _meshid;

signals:
    void updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&);
};

class MLRenderingSolidParametersFrame : public MLRenderingParametersFrame
{
    Q_OBJECT
public:
    MLRenderingSolidParametersFrame(QWidget* parent);
    MLRenderingSolidParametersFrame(int meshid,QWidget* parent);    
    ~MLRenderingSolidParametersFrame();
    void setPrimitiveButtonStatesAccordingToRenderingData(const MLRenderingData& dt);
    void setAssociatedMeshId(int meshid);
    void getAllRenderingActions(QList<MLRenderingAction*>& acts);

private:
    void initGui();
    MLRenderingToolbar* _shadingtool;
    MLRenderingToolbar* _colortool;
    MLRenderingToolbar* _texttool;
};

//class MLRenderingSolidParametersFrame : public MLRenderingParametersFrame
//{
//    Q_OBJECT
//public:
//    MLRenderingSolidParametersFrame(QWidget* parent);
//    MLRenderingSolidParametersFrame(int meshid,QWidget* parent);    
//    ~MLRenderingSolidParametersFrame();
//    void setPrimitiveButtonStatesAccordingToRenderingData(const MLRenderingData& dt);
//    void setAssociatedMeshId(int meshid);
//
//private:
//    void initGui();
//    MLRenderingToolbar* _shadingtool;
//    MLRenderingToolbar* _colortool;
//    MLRenderingToolbar* _texttool;
//};
//
//
//class MLRenderingSolidParametersFrame : public MLRenderingParametersFrame
//{
//    Q_OBJECT
//public:
//    MLRenderingSolidParametersFrame(QWidget* parent);
//    MLRenderingSolidParametersFrame(int meshid,QWidget* parent);    
//    ~MLRenderingSolidParametersFrame();
//    void setPrimitiveButtonStatesAccordingToRenderingData(const MLRenderingData& dt);
//    void setAssociatedMeshId(int meshid);
//
//private:
//    void initGui();
//    MLRenderingToolbar* _shadingtool;
//    MLRenderingToolbar* _colortool;
//    MLRenderingToolbar* _texttool;
//};
//
//
//class MLRenderingSolidParametersFrame : public MLRenderingParametersFrame
//{
//    Q_OBJECT
//public:
//    MLRenderingSolidParametersFrame(QWidget* parent);
//    MLRenderingSolidParametersFrame(int meshid,QWidget* parent);    
//    ~MLRenderingSolidParametersFrame();
//    void setPrimitiveButtonStatesAccordingToRenderingData(const MLRenderingData& dt);
//    void setAssociatedMeshId(int meshid);
//
//private:
//    void initGui();
//    MLRenderingToolbar* _shadingtool;
//    MLRenderingToolbar* _colortool;
//    MLRenderingToolbar* _texttool;
//};


class MLRenderingParametersTab : public QTabWidget
{
    Q_OBJECT
public:
    MLRenderingParametersTab(int meshid,const QList<MLRenderingAction*>& tab, QWidget* parent);
    ~MLRenderingParametersTab();

    void setPrimitiveButtonStatesAccordingToRenderingData(const MLRenderingData& dt);
    void setAssociatedMeshIdAndRenderingData(int meshid,const MLRenderingData& dt);
private:
    void initGui(const QList<MLRenderingAction*>& tab);

    int _meshid;
public slots:
    void switchTab(const QString& name);
    void setAssociatedMeshId(int meshid);

signals:
    void updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&);
private:
    QMap<QString,MLRenderingParametersFrame*> _parframe;
};


#endif