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
/****************************************************************************/

#ifndef LIGHTINGDIALOG_H
#define LIGHTINGDIALOG_H

// local headers
#include "ui_lightingProperties.h"
#include "glarea.h"

// Qt headers
#include <QDialog>
#include <qcolor.h>

class LightingDialog : public QDialog, private Ui::lightSettings
{
	Q_OBJECT

// private data members
private:

	// Main light
	QColor ambient;
	QColor diffuse;
	QColor specular;

	// Fancy lighting
	QColor ambientFancyBack;
	QColor diffuseFancyBack;
	QColor specularFancyBack;
	QColor ambientFancyFront;
	QColor diffuseFancyFront;
	QColor specularFancyFront;

// construction
public:

	LightingDialog(const GLLightSetting &glls, QWidget *parent = NULL);

// private methods
private:

	void setupLightSettings(const GLLightSetting &ls);
	void setupColorsInUsed();

// private slots
private slots: 

	void changeAmbient();
	void changeDiffuse();
	void changeSpecular();

	void changeFancyBackDiffuse();
	void changeFancyBackSpecular();
	void changeFancyFrontDiffuse();
	void changeFancyFrontSpecular();

// public methods
public:

	void lightSettingsToGL(GLLightSetting &ls);
};

#endif /* LIGHTINGDIALOG_H */
