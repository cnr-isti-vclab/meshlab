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
#include <QToolButton>
#include <QTabWidget>
#include <wrap/gl/gl_mesh_attributes_info.h>
#include "ml_rendering_actions.h"
#include "additionalgui.h"

class MLRenderingColorPicker : public QToolButton
{
    Q_OBJECT

public:
    MLRenderingColorPicker(int meshid,MLRenderingData::PRIMITIVE_MODALITY pr,QWidget *p);
    MLRenderingColorPicker(MLRenderingData::PRIMITIVE_MODALITY pr,QWidget *p);
    ~MLRenderingColorPicker();

    void setColor(QColor& def);
//protected:
//    void paintEvent( QPaintEvent * );
protected:
    void initGui();
    void  updateColorInfo();
    QPushButton* _cbutton;
public:
    MLRenderingUserDefinedColorAction* _act;

signals:
    void userDefinedColorAction(int,MLRenderingAction*);
protected slots:
    void pickColor();
};

class MLRenderingBBoxColorPicker : public QToolButton
{
    Q_OBJECT
public:
    MLRenderingBBoxColorPicker(QWidget* parent);
    MLRenderingBBoxColorPicker(int meshid,QWidget* parent);
    ~MLRenderingBBoxColorPicker();

    void setColor(QColor& def);
protected:
    void initGui();
    void  updateColorInfo();
    QPushButton* _cbutton;
    MLRenderingBBoxUserDefinedColorAction* _act;

signals:
    void userDefinedColorAction(int,MLRenderingAction*);
protected slots:
    void pickColor();
};

class MLRenderingFloatSlider : public MLFloatSlider
{
    Q_OBJECT

public:
    MLRenderingFloatSlider(int meshid,QWidget *p);
    MLRenderingFloatSlider(QWidget *p);
    ~MLRenderingFloatSlider();

    void setRenderingFloatAction(MLRenderingFloatAction* act);
    void setAccordingToRenderingData(const MLRenderingData& dt);
    void getRenderingDataAccordingToGUI(MLRenderingData& dt) const;
    void setAssociatedMeshId(int meshid);

private:
    MLRenderingFloatAction* _act;
    int _meshid;

private slots:
    void valueChanged(float);
signals:
    void updateRenderingDataAccordingToAction(int,MLRenderingAction*);
};

class MLRenderingToolbar : public QToolBar
{
    Q_OBJECT
public:
    MLRenderingToolbar(QWidget* parent = NULL);
    MLRenderingToolbar(int meshid,QWidget* parent = NULL);

    ~MLRenderingToolbar();

    virtual void addRenderingAction( MLRenderingAction* act );
    void addColorPicker(MLRenderingColorPicker* pick);
    void addColorPicker(MLRenderingBBoxColorPicker* pick);
    void setAccordingToRenderingData(const MLRenderingData& dt);
    void getCurrentRenderingDataAccordingToGUI(MLRenderingData& dt) const;
    QList<MLRenderingAction*>& getRenderingActions();
    void setAssociatedMeshId(int meshid);
    void updateVisibility(MeshModel* mm);

protected:
    QList<MLRenderingAction*> _acts;
    int _meshid;
    MLRenderingAction* _previoussel;
protected slots:
    virtual void toggle(QAction* act);
    void extraUpdateRequired(int,MLRenderingAction*);


signals:
    void updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>& acts);
    void activatedAction(MLRenderingAction*);
    

protected:
    //if meshid is -1 it means that the actions are intended to be deployed to all the document and not to a specific mesh model
    QActionGroup* _actgroup;
};

class MLRenderingSideToolbar: public MLRenderingToolbar
{

    Q_OBJECT
public:
    MLRenderingSideToolbar(QWidget* parent = NULL);
    MLRenderingSideToolbar(int meshid,QWidget* parent = NULL);

    ~MLRenderingSideToolbar() {}
private slots:
    void toggle(QAction* act);

private:
    void initGui();
};

class MLRenderingOnOffToolbar : public QToolBar
{
    Q_OBJECT
public:
    MLRenderingOnOffToolbar(int meshid,QWidget* parent = NULL);
    MLRenderingOnOffToolbar(MLRenderingAction* act,QWidget* parent = NULL);
    MLRenderingOnOffToolbar(MLRenderingAction* act,int meshid,QWidget* parent = NULL);
    ~MLRenderingOnOffToolbar();

    void setRenderingAction(MLRenderingAction* act);
    void setAssociatedMeshId(int meshid);
    void setAccordingToRenderingData(const MLRenderingData& dt);
    void getRenderingDataAccordingToGUI(MLRenderingData& dt) const;
    void updateVisibility(MeshModel* mm);
private:
    void initGui();
    int _meshid;
    MLRenderingAction* _act;
    QAction* _onact;
    QAction* _offact;

signals:
    void updateRenderingDataAccordingToAction(int,MLRenderingAction*);

private slots:
    void toggle(QAction* act);
};

class MLRenderingParametersFrame : public QFrame
{
    Q_OBJECT
public:
    MLRenderingParametersFrame(int meshid,QWidget* parent);
    virtual ~MLRenderingParametersFrame(); 
    virtual void setPrimitiveButtonStatesAccordingToRenderingData(const MLRenderingData& dt) = 0;
    virtual void setAssociatedMeshId(int meshid) = 0;
    virtual void getCurrentRenderingDataAccordingToGUI(MLRenderingData& dt) const = 0;
    virtual void updateVisibility(MeshModel*) {}
    static MLRenderingParametersFrame* factory(MLRenderingAction* act,int meshid,QWidget* parent);
    static void destroy(MLRenderingParametersFrame* pf);

protected:
    int _meshid;

signals:
    void updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&);
    void updateRenderingDataAccordingToAction(int,MLRenderingAction*);
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
    void getCurrentRenderingDataAccordingToGUI(MLRenderingData& dt) const;
    void updateVisibility(MeshModel* mm);

private:
    void initGui();
    MLRenderingToolbar* _shadingtool;
    MLRenderingToolbar* _colortool;
    MLRenderingOnOffToolbar* _texttool;
};

class MLRenderingWireParametersFrame : public MLRenderingParametersFrame
{
    Q_OBJECT
public:
    MLRenderingWireParametersFrame(QWidget* parent);
    MLRenderingWireParametersFrame(int meshid,QWidget* parent);    
    ~MLRenderingWireParametersFrame();
    void setPrimitiveButtonStatesAccordingToRenderingData(const MLRenderingData& dt);
    void setAssociatedMeshId(int meshid);
    void getCurrentRenderingDataAccordingToGUI(MLRenderingData& dt) const;
    void updateVisibility(MeshModel* mm);

private:
    void initGui();
    
    MLRenderingToolbar* _shadingtool;
    MLRenderingToolbar* _colortool;
    MLRenderingOnOffToolbar* _edgetool;
    MLRenderingFloatSlider* _dimension;
};

class MLRenderingPointsParametersFrame : public MLRenderingParametersFrame
{
    Q_OBJECT
public:
    MLRenderingPointsParametersFrame(QWidget* parent);
    MLRenderingPointsParametersFrame(int meshid,QWidget* parent);    
    ~MLRenderingPointsParametersFrame();
    void setPrimitiveButtonStatesAccordingToRenderingData(const MLRenderingData& dt);
    void setAssociatedMeshId(int meshid);
    void getCurrentRenderingDataAccordingToGUI(MLRenderingData& dt) const;
    void updateVisibility(MeshModel* mm);

private:
    void initGui();
    MLRenderingToolbar* _shadingtool;
    MLRenderingToolbar* _colortool;
    MLRenderingOnOffToolbar* _texttool;
    MLRenderingFloatSlider* _dimension;
};

class MLRenderingSelectionParametersFrame : public MLRenderingParametersFrame
{
    Q_OBJECT
public:
    MLRenderingSelectionParametersFrame(QWidget* parent);
    MLRenderingSelectionParametersFrame(int meshid,QWidget* parent);    
    ~MLRenderingSelectionParametersFrame();
    void setPrimitiveButtonStatesAccordingToRenderingData(const MLRenderingData& dt);
    void setAssociatedMeshId(int meshid);
    void getCurrentRenderingDataAccordingToGUI(MLRenderingData& dt) const;

private:
    void initGui();
    MLRenderingOnOffToolbar* _vertexseltool;
    MLRenderingOnOffToolbar* _faceseltool;
};

class MLRenderingEdgeDecoratorParametersFrame : public MLRenderingParametersFrame
{
    Q_OBJECT
public:
    MLRenderingEdgeDecoratorParametersFrame(QWidget* parent);
    MLRenderingEdgeDecoratorParametersFrame(int meshid,QWidget* parent);    
    ~MLRenderingEdgeDecoratorParametersFrame();
    void setPrimitiveButtonStatesAccordingToRenderingData(const MLRenderingData& dt);
    void setAssociatedMeshId(int meshid);
    void getCurrentRenderingDataAccordingToGUI(MLRenderingData& dt) const;

private:
    void initGui();
    MLRenderingOnOffToolbar* _boundarytool;
    MLRenderingOnOffToolbar* _vertmanifoldtool;
    MLRenderingOnOffToolbar* _edgemanifoldtool;
    MLRenderingOnOffToolbar* _texturebordertool;
};

//class MLRenderingGlobalParametersFrame : public MLRenderingParametersFrame
//{
//    Q_OBJECT
//public:
//    MLRenderingGlobalParametersFrame(QWidget* parent);
//    MLRenderingGlobalParametersFrame(int meshid,QWidget* parent);    
//    ~MLRenderingGlobalParametersFrame();
//    void setPrimitiveButtonStatesAccordingToRenderingData(const MLRenderingData& dt);
//    void setAssociatedMeshId(int meshid);
//    void getCurrentRenderingDataAccordingToGUI(MLRenderingData& dt) const;
//
//private:
//    void initGui();
//    MLRenderingToolbar* _lighttool;
//};

class MLRenderingBBoxParametersFrame : public MLRenderingParametersFrame
{
    Q_OBJECT
public:
    MLRenderingBBoxParametersFrame(QWidget* parent);
    MLRenderingBBoxParametersFrame(int meshid,QWidget* parent);    
    ~MLRenderingBBoxParametersFrame();
    void setPrimitiveButtonStatesAccordingToRenderingData(const MLRenderingData& dt);
    void setAssociatedMeshId(int meshid);
    void getCurrentRenderingDataAccordingToGUI(MLRenderingData& dt) const;
    void updateVisibility(MeshModel*) {}

private:
    void initGui();
    MLRenderingToolbar* _colortool;
    MLRenderingColorPicker* _userdef;
};

class MLRenderingParametersTab : public QTabWidget
{
    Q_OBJECT
public:
    MLRenderingParametersTab(int meshid,const QList<MLRenderingAction*>& tab, QWidget* parent = NULL);
    ~MLRenderingParametersTab();

    void updateGUIAccordingToRenderingData(const MLRenderingData& dt);
    void setAssociatedMeshIdAndRenderingData(int meshid,const MLRenderingData& dt);
    void getCurrentRenderingDataAccordingToGUI(MLRenderingData& dt) const;
    int associatedMeshId() const {return _meshid;}
    void updateVisibility(MeshModel* mm);
private:
    void initGui(const QList<MLRenderingAction*>& tab);

    int _meshid;
public slots:
    void switchTab(int meshid,const QString& tabname);
    void setAssociatedMeshId(int meshid);

signals:
    void updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&);
    void updateRenderingDataAccordingToAction(int,MLRenderingAction*);

private:
    QMap<QString,MLRenderingParametersFrame*> _parframe;
};

#endif
