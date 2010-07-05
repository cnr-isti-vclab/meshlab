/****************************************************************************
* Render Radiance Scaling                                                   *
* Meshlab's plugin                                                          *
*                                                                           *
* Copyright(C) 2010                                                         *
* Vergne Romain, Dumas Olivier                                              *
* INRIA - Institut Nationnal de Recherche en Informatique et Automatique    *
*                                                                           *
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
#ifndef SHADERDIALOG_H
#define SHADERDIALOG_H

#include <GL/glew.h>
#include <QtGui>
#include <QGLWidget>
#include <QtGui/QDockWidget>
#include "ui_shaderDialog.h"

class RadianceScalingRendererPlugin;

class ShaderDialog : public QDockWidget {
  Q_OBJECT
    
  public:
  ShaderDialog(RadianceScalingRendererPlugin* wrp,QGLWidget* gla,QWidget *parent = 0);
  ~ShaderDialog();
  
  void changeIcon(QString path,int icon);
  
  static const int convex_icon=0;
  static const int concav_icon=1;

 private:
  RadianceScalingRendererPlugin *_wrp;
  Ui::ShaderDialogClass          _ui;
  QGLWidget                     *_gla;

  private slots:
  void enableChanged(int);
  void displayChanged(int);
  void invertChanged(int);
  void enhancementChanged(int);
  void transitionChanged(int);
  void litChanged(int);
  void load1Clicked();
  void load2Clicked();

 public:
  inline bool  getEnable()      const;
  inline int   getDisplay()     const;
  inline float getEnhancement() const;
  inline float getTransition()  const;
  inline bool  getInvert()      const;
  inline bool  getTwoLS()       const;
};

inline bool ShaderDialog::getEnable() const {
  return (_ui.enableCheckBox->checkState()==Qt::Checked);
}

inline int ShaderDialog::getDisplay() const {
  return _ui.displayBox->currentIndex();
}

inline float ShaderDialog::getEnhancement() const {
  const float scale = 100.0f;
  return (float)(_ui.enSlider->value())/scale;
}

inline float ShaderDialog::getTransition() const {
  const float scale = 100.0f;
  return (float)(_ui.transitionSlider->value())/scale;
}

inline bool ShaderDialog::getInvert() const {
  return (_ui.invertCheckBox->checkState()==Qt::Checked);
}

inline bool ShaderDialog::getTwoLS() const {
  return (_ui.litCheckBox->checkState()==Qt::Checked);
}

#endif // SHADERDIALOG_H
