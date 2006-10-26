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
/***************************************************************************/

// local headers
#include "lightingDialog.h"

// Qt headers
#include <QColorDialog>

LightingDialog::LightingDialog(const GLLightSetting &glls, QWidget *parent/*=NULL*/):
	QDialog(parent)
{
	setupUi(this);

	// custom settings
	/////////////////////////////////////////////////////////

	setupLightSettings(glls);
	setupColorsInUsed();

	// signal-slot connections
	connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	connect(pushAmbient, SIGNAL(clicked()), this, SLOT(changeAmbient()));
	connect(pushDiffuse, SIGNAL(clicked()), this, SLOT(changeDiffuse()));
	connect(pushSpecular, SIGNAL(clicked()), this, SLOT(changeSpecular()));

	connect(pushDiffuseBack, SIGNAL(clicked()), this, SLOT(changeFancyBackDiffuse()));
	connect(pushSpecularBack, SIGNAL(clicked()), this, SLOT(changeFancyBackSpecular()));
	connect(pushDiffuseFront, SIGNAL(clicked()), this, SLOT(changeFancyFrontDiffuse()));
	connect(pushSpecularFront, SIGNAL(clicked()), this, SLOT(changeFancyFrontSpecular()));
}

void LightingDialog::setupLightSettings(const GLLightSetting &ls)
{
	// standard lighting
	ambient.setRed(static_cast<int>(ls.ambient[0] * 255.0f));
	ambient.setGreen(static_cast<int>(ls.ambient[1] * 255.0f));
	ambient.setBlue(static_cast<int>(ls.ambient[2] * 255.0f));
	ambient.setAlpha(static_cast<int>(ls.ambient[3] * 255.0f));

	diffuse.setRed(static_cast<int>(ls.diffuse[0] * 255.0f));
	diffuse.setGreen(static_cast<int>(ls.diffuse[1] * 255.0f));
	diffuse.setBlue(static_cast<int>(ls.diffuse[2] * 255.0f));
	diffuse.setAlpha(static_cast<int>(ls.diffuse[3] * 255.0f));	
	
	specular.setRed(static_cast<int>(ls.specular[0] * 255.0f));
	specular.setGreen(static_cast<int>(ls.specular[1] * 255.0f));
	specular.setBlue(static_cast<int>(ls.specular[2] * 255.0f));
	specular.setAlpha(static_cast<int>(ls.specular[3] * 255.0f));

	// fancy lighting
	ambientFancyBack.setRed(static_cast<int>(ls.ambientFancyBack[0] * 255.0f));
	ambientFancyBack.setGreen(static_cast<int>(ls.ambientFancyBack[1] * 255.0f));
	ambientFancyBack.setBlue(static_cast<int>(ls.ambientFancyBack[2] * 255.0f));
	ambientFancyBack.setAlpha(static_cast<int>(ls.ambientFancyBack[3] * 255.0f));

	diffuseFancyBack.setRed(static_cast<int>(ls.diffuseFancyBack[0] * 255.0f));
	diffuseFancyBack.setGreen(static_cast<int>(ls.diffuseFancyBack[1] * 255.0f));
	diffuseFancyBack.setBlue(static_cast<int>(ls.diffuseFancyBack[2] * 255.0f));
	diffuseFancyBack.setAlpha(static_cast<int>(ls.diffuseFancyBack[3]	* 255.0f));	
	
	specularFancyBack.setRed(static_cast<int>(ls.specularFancyBack[0] * 255.0f));
	specularFancyBack.setGreen(static_cast<int>(ls.specularFancyBack[1] * 255.0f));
	specularFancyBack.setBlue(static_cast<int>(ls.specularFancyBack[2] * 255.0f));
	specularFancyBack.setAlpha(static_cast<int>(ls.specularFancyBack[3] * 255.0f));	
	
	ambientFancyFront.setRed(static_cast<int>(ls.ambientFancyFront[0] * 255.0f));
	ambientFancyFront.setGreen(static_cast<int>(ls.ambientFancyFront[1] * 255.0f));
	ambientFancyFront.setBlue(static_cast<int>(ls.ambientFancyFront[2] * 255.0f));
	ambientFancyFront.setAlpha(static_cast<int>(ls.ambientFancyFront[3] * 255.0f));

	diffuseFancyFront.setRed(static_cast<int>(ls.diffuseFancyFront[0] * 255.0f));
	diffuseFancyFront.setGreen(static_cast<int>(ls.diffuseFancyFront[1] * 255.0f));
	diffuseFancyFront.setBlue(static_cast<int>(ls.diffuseFancyFront[2] * 255.0f));
	diffuseFancyFront.setAlpha(static_cast<int>(ls.diffuseFancyFront[3] * 255.0f));	
	
	specularFancyFront.setRed(static_cast<int>(ls.specularFancyFront[0] * 255.0f));
	specularFancyFront.setGreen(static_cast<int>(ls.specularFancyFront[1] * 255.0f));
	specularFancyFront.setBlue(static_cast<int>(ls.specularFancyFront[2] * 255.0f));
	specularFancyFront.setAlpha(static_cast<int>(ls.specularFancyFront[3] * 255.0f));
}

void LightingDialog::setupColorsInUsed()
{
	QSize sz = labelDiffuse->size();

	QPixmap pxmap1(sz);
	pxmap1.fill(ambient);
	labelAmbient->setPixmap(pxmap1);

	QPixmap pxmap2(sz);
	pxmap2.fill(diffuse);
	labelDiffuse->setPixmap(pxmap2);

	QPixmap pxmap3(sz);
	pxmap3.fill(specular);
	labelSpecular->setPixmap(pxmap3);

	QPixmap pxmap4(sz);
	pxmap4.fill(diffuseFancyBack);
	labelDiffuseBack->setPixmap(pxmap4);

	QPixmap pxmap5(sz);
	pxmap5.fill(specularFancyBack);
	labelSpecularBack->setPixmap(pxmap5);

	QPixmap pxmap6(sz);
	pxmap6.fill(diffuseFancyFront);
	labelDiffuseFront->setPixmap(pxmap6);

	QPixmap pxmap7(sz);
	pxmap7.fill(specularFancyFront);
	labelSpecularFront->setPixmap(pxmap7);
}

void LightingDialog::lightSettingsToGL(GLLightSetting &ls)
{
	ls.ambient[0] = static_cast<float>(ambient.red()) / 255.0f;
	ls.ambient[1] = static_cast<float>(ambient.green()) / 255.0f;
	ls.ambient[2] = static_cast<float>(ambient.blue()) / 255.0f;
	ls.ambient[3] = 1.0f;

	ls.diffuse[0] = static_cast<float>(diffuse.red()) / 255.0f;
	ls.diffuse[1] = static_cast<float>(diffuse.green()) / 255.0f;
	ls.diffuse[2] = static_cast<float>(diffuse.blue()) / 255.0f;
	ls.diffuse[3] = 1.0f;

	ls.specular[0] = static_cast<float>(specular.red()) / 255.0f;
	ls.specular[1] = static_cast<float>(specular.green()) / 255.0f;
	ls.specular[2] = static_cast<float>(specular.blue()) / 255.0f;
	ls.specular[3] = 1.0f;

	ls.ambientFancyBack[0] = static_cast<float>(ambientFancyBack.red()) / 255.0f;
	ls.ambientFancyBack[1] = static_cast<float>(ambientFancyBack.green()) / 255.0f;
	ls.ambientFancyBack[2] = static_cast<float>(ambientFancyBack.blue()) / 255.0f;
	ls.ambientFancyBack[3] = 1.0f;

	ls.diffuseFancyBack[0] = static_cast<float>(diffuseFancyBack.red()) / 255.0f;
	ls.diffuseFancyBack[1] = static_cast<float>(diffuseFancyBack.green()) / 255.0f;
	ls.diffuseFancyBack[2] = static_cast<float>(diffuseFancyBack.blue()) / 255.0f;
	ls.diffuseFancyBack[3] = 1.0f;

	ls.specularFancyBack[0] = static_cast<float>(specularFancyBack.red()) / 255.0f;
	ls.specularFancyBack[1] = static_cast<float>(specularFancyBack.green()) / 255.0f;
	ls.specularFancyBack[2] = static_cast<float>(specularFancyBack.blue()) / 255.0f;
	ls.specularFancyBack[3] = 1.0f;

	ls.ambientFancyFront[0] = static_cast<float>(ambientFancyFront.red()) / 255.0f;
	ls.ambientFancyFront[1] = static_cast<float>(ambientFancyFront.green()) / 255.0f;
	ls.ambientFancyFront[2] = static_cast<float>(ambientFancyFront.blue()) / 255.0f;
	ls.ambientFancyFront[3] = 1.0f;
	
	ls.diffuseFancyFront[0] = static_cast<float>(diffuseFancyFront.red()) / 255.0f;
	ls.diffuseFancyFront[1] = static_cast<float>(diffuseFancyFront.green()) / 255.0f;
	ls.diffuseFancyFront[2] = static_cast<float>(diffuseFancyFront.blue()) / 255.0f;
	ls.diffuseFancyFront[3] = 1.0f;

	ls.specularFancyFront[0] = static_cast<float>(specularFancyFront.red()) / 255.0f;
	ls.specularFancyFront[1] = static_cast<float>(specularFancyFront.green()) / 255.0f;
	ls.specularFancyFront[2] = static_cast<float>(specularFancyFront.blue()) / 255.0f;
	ls.specularFancyFront[3] = 1.0f;
}

// Qt-slot
void LightingDialog::changeAmbient()
{
	// Select draw color
	QColor newcolor = QColorDialog::getColor(ambient, this);

	if (newcolor.isValid())
	{
		ambient = newcolor;
		setupColorsInUsed();
	}
}

// Qt-slot
void LightingDialog::changeDiffuse()
{
	QColor newcolor = QColorDialog::getColor(diffuse, this);

	if (newcolor.isValid())
	{
		diffuse = newcolor;
		setupColorsInUsed();
	}
}

// Qt-slot
void LightingDialog::changeSpecular()
{
	QColor newcolor = QColorDialog::getColor(specular, this);

	if (newcolor.isValid())
	{
		specular = newcolor;
		setupColorsInUsed();
	}
}

// Qt-slot
void LightingDialog::changeFancyFrontDiffuse()
{
	// Select ambient color
	QColor newcolor = QColorDialog::getColor(diffuseFancyFront, this);

	if (newcolor.isValid())
	{
		diffuseFancyFront = newcolor;
		setupColorsInUsed();
	}
}

// Qt-slot
void LightingDialog::changeFancyFrontSpecular()
{
	QColor newcolor = QColorDialog::getColor(specularFancyFront, this);

	if (newcolor.isValid())
	{
		specularFancyFront = newcolor;
		setupColorsInUsed();
	}
}

// Qt-slot
void LightingDialog::changeFancyBackDiffuse()
{
	QColor newcolor = QColorDialog::getColor(diffuseFancyBack, this);

	if (newcolor.isValid())
	{
		diffuseFancyBack = newcolor;
		setupColorsInUsed();
	}
}

// Qt-slot
void LightingDialog::changeFancyBackSpecular()
{
	// Select ambient color
	QColor newcolor = QColorDialog::getColor(specularFancyBack, this);

	if (newcolor.isValid())
	{
		specularFancyBack = newcolor;
		setupColorsInUsed();
	}
}

