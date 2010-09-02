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
#include "shaderDialog.h"
#include "radianceScalingRenderer.h"

using namespace std;

ShaderDialog::ShaderDialog(RadianceScalingRendererPlugin* wrp,QGLWidget* gla,QWidget *parent)
  : QDockWidget(parent),
    _wrp(wrp),
    _gla(gla) {

  _ui.setupUi(this);
  this->setWidget(_ui.frame);
  this->setFeatures(QDockWidget::AllDockWidgetFeatures);
  this->setAllowedAreas(Qt::LeftDockWidgetArea);
  this->setFloating(true);

  connect(_ui.enableCheckBox,SIGNAL(stateChanged(int)),this,SLOT(enableChanged(int)));
  connect(_ui.invertCheckBox,SIGNAL(stateChanged(int)),this,SLOT(invertChanged(int)));
  connect(_ui.displayBox,SIGNAL(currentIndexChanged(int)),this,SLOT(displayChanged(int)));
  connect(_ui.enSlider,SIGNAL(valueChanged(int)),this,SLOT(enhancementChanged(int)));
  connect(_ui.transitionSlider,SIGNAL(valueChanged(int)),this,SLOT(transitionChanged(int)));
  connect(_ui.litCheckBox,SIGNAL(stateChanged(int)),this,SLOT(litChanged(int)));
  connect(_ui.loadButton1,SIGNAL(clicked()),this,SLOT(load1Clicked()));
  connect(_ui.loadButton2,SIGNAL(clicked()),this,SLOT(load2Clicked()));

  _ui.litCheckBox->hide();
  _ui.litIcon1->hide();
  _ui.litIcon2->hide();
  _ui.litLabel1->hide();
  _ui.litLabel2->hide();
  _ui.loadButton1->hide();
  _ui.loadButton2->hide();
  _ui.transitionTitle->hide();
  _ui.transitionSlider->hide();
  _ui.transitionLabel->hide();
}

ShaderDialog::~ShaderDialog() {
  
}

void ShaderDialog::enableChanged(int) {
  bool enableChecked = (_ui.enableCheckBox->checkState()==Qt::Checked) ? true : false;

  _wrp->setEnable(enableChecked);
  _gla->update();
}

void ShaderDialog::invertChanged(int) {
  (_ui.invertCheckBox->checkState()==Qt::Checked) ? _wrp->setInvert(true) : _wrp->setInvert(false);
  _gla->update();
}

void ShaderDialog::displayChanged(int index) {
  if(index==1) { 
    // special case of lit sphere rendering
    _ui.litCheckBox->show();
    _ui.litIcon1->show();
    _ui.litLabel1->show();
    _ui.loadButton1->show();
    litChanged(0);
  } else {
    // other cases 
    _ui.litCheckBox->hide();
    _ui.litIcon1->hide();
    _ui.litIcon2->hide();
    _ui.litLabel1->hide();
    _ui.litLabel2->hide();
    _ui.loadButton1->hide();
    _ui.loadButton2->hide();
    _ui.transitionTitle->hide();
    _ui.transitionSlider->hide();
    _ui.transitionLabel->hide();
  }
  
  _wrp->setDisplay(index);
  _gla->update();
}

void ShaderDialog::enhancementChanged(int value) {
  const float scale = 100.0f;
  float val = (float)value/scale;
  QString s;
  s.setNum(val,'f',2); 
  _ui.enLabel->setText(s);
  _wrp->setEnhancement(val);
  _gla->update();
}

void ShaderDialog::transitionChanged(int value) {
  const float scale = 100.0f;
  float val = (float)value/scale;
  QString s;
  s.setNum(val,'f',2); 
  _ui.transitionLabel->setText(s);
  _wrp->setTransition(val);
  _gla->update();
}

void ShaderDialog::litChanged(int) {
  bool doubleLit = (_ui.litCheckBox->checkState()==Qt::Checked) ? true : false;
  if(doubleLit) {
    _ui.litIcon2->show();
    _ui.litLabel2->show();
    _ui.loadButton2->show();
    _ui.transitionTitle->show();
    _ui.transitionSlider->show();
    _ui.transitionLabel->show();
    _ui.litLabel1->setText("Convexities");
  } else {
    _ui.litIcon2->hide();
    _ui.litLabel2->hide();
    _ui.loadButton2->hide();
    _ui.transitionTitle->hide();
    _ui.transitionSlider->hide();
    _ui.transitionLabel->hide();
    _ui.litLabel1->setText("Convexities and Concavities");
  }
  
  _wrp->setLit(doubleLit);
  _gla->update();
}

void ShaderDialog::changeIcon(QString path,int icon) {
  if(icon!=convex_icon && icon!=concav_icon) 
    return;

  const int w = 128;

  QPixmap pix(path);

  pix = pix.scaledToWidth(w,Qt::SmoothTransformation);

  if(icon==convex_icon) {
    _ui.litIcon1->setPixmap(pix);
  } else if(icon==concav_icon) {
    _ui.litIcon2->setPixmap(pix);
  }
}

void ShaderDialog::load1Clicked() {
  QString filename = QFileDialog::getOpenFileName(0, QString(),QString(),
						  tr("Images (*.png *.xpm *.jpg *.bmp *.tif)"));
  
  if(filename.isNull())
    return;

  changeIcon(filename,convex_icon);

  _wrp->setConvexLit(filename);
  _gla->update();
}

void ShaderDialog::load2Clicked() {
  QString filename = QFileDialog::getOpenFileName(0, QString(),QString(),
						  tr("Images (*.png *.xpm *.jpg *.bmp *.tif)"));
  
  if(filename.isNull())
    return;

  changeIcon(filename,concav_icon);

  _wrp->setConcavLit(filename);
  _gla->update();
}
