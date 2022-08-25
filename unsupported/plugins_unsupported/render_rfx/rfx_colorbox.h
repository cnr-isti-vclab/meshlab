/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005-2008                                           \/)\/    *
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

#ifndef RFX_COLORBOX_H_
#define RFX_COLORBOX_H_

#include <cassert>
#include <climits>
#include <cfloat>
#include <QSignalMapper>
#include <QWidget>
#include <QFrame>
#include <QPushButton>
#include <QColor>
#include <QColorDialog>
#include <QGridLayout>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QSlider>
#include <vcg/space/color4.h>
#include "rfx_dialog.h"

class RfxColorBox : public QWidget
{
	Q_OBJECT

public:
     explicit RfxColorBox(const int&, const int&, const QColor&, QWidget *parent = 0);
	virtual ~RfxColorBox();

	/*
		Returns the current color as an array of float value between 0 and 1.
		@return the current color as float array.
	*/
	inline vcg::Color4f getColorf(){
		vcg::Color4f c(_redS->value() / 255.0f,_greenS->value() / 255.0f,_blueS->value() / 255.0f,_alphaS->value() / 255.0f);
		return c;
	}

public slots:
	void setR(int);
	void setR();
	void setG(int);
	void setG();
	void setB(int);
	void setB();
	void setA(int);
	void setA();

private slots:
     void setBoxColorFromDialog();
     
signals:
     void colorChanged();
     /*
	 Unused at the moment...maybe for further usages.

	 void redChanged();
     void greenChanged();
     void blueChanged();
     void alphaChanged();*/

private:
	QFrame* _scacchiera;
	/*Colored box without alpha channel*/
	QFrame* _rgbBox;
	/*Colored box with alpha channel*/
	QPushButton* _rgbaBox;

	/* The sliders */
	QSlider* _redS;
	QSlider* _greenS;
	QSlider* _blueS;
	QSlider* _alphaS;

	/* The text boxes */
	QLineEdit* _redT;
	QLineEdit* _greenT;
	QLineEdit* _blueT;
	QLineEdit* _alphaT;

	 enum Channels{
		 CHANNEL_R, CHANNEL_G, CHANNEL_B, CHANNEL_A, CHANNEL_ALL
	 };

     /*Returns a string rapresenting a new stylesheet(without alpha channel)*/
	 QString getNewRGBStylesheet(const QString&, RfxColorBox::Channels, int*[]);
     /*Returns a string rapresenting a new stylesheet(with alpha channel)*/
     QString getNewRGBAStylesheet(const QString&, RfxColorBox::Channels, int*[]);

     /* default basic style for the rgb box */
     static const QString _BASE_RGB_BOX_STYLE;

     /* default basic style for the rgba box */
     static const QString _BASE_RGBA_BOX_STYLE;

	 /* Initialize a slider.
	 @param slider the slider to initialize
	 @param value the initial value
	 */
	 void initSlider(QSlider*, int);

	 /* Initialize a text box.
	 @param box the text box to initialize
	 */
	 void initTextBox(QLineEdit*);

	 /* Connects all the slider to the color box. */
	 void connectSliders();

	 /* Disconnects all the slider to the color box. */
	 void disconnectSliders();
};
#endif /* RFX_COLORBOX_H_*/
