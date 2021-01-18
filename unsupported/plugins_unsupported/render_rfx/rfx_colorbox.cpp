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

#include "rfx_colorbox.h"

const QString RfxColorBox::_BASE_RGB_BOX_STYLE = QString("margin-left: 25px;border-top-left-radius: 5px;border-bottom-left-radius: 5px;border-left: 1px solid black;border-top: 1px solid black;border-bottom: 1px solid black;padding: 2px;");
const QString RfxColorBox::_BASE_RGBA_BOX_STYLE = QString("margin-left: 25px;margin-right: 25px;border: 1px solid black;border-radius: 5px;padding: 2px;");

/* Constructor 
   @param w the width of the widget.
   @param h the height of the widget.
   @param first the start color of the box.
   @param parent the parent widget. Default to 0.
*/
RfxColorBox::RfxColorBox(const int& w, const int& h, const QColor& first, QWidget* parent)
: QWidget(parent)
{
     int r,g,b,a;
     r = first.red();
     g = first.green();
     b = first.blue();
     a = first.alpha();

     this->_rgbaBox = new QPushButton(this);
     this->_rgbaBox->setObjectName("rgbaBox");
     this->_rgbaBox->setFixedSize (w, h);
     this->_rgbaBox->setStyleSheet(RfxColorBox::_BASE_RGBA_BOX_STYLE + QString("background-color: rgba(%1,%2,%3,%4);").arg(r).arg(g).arg(b).arg(a));
	 
     this->_rgbBox = new QFrame(this);
     this->_rgbBox->setObjectName("rgbBox");
     this->_rgbBox->setFixedSize ((w/2), h);
     this->_rgbBox->setStyleSheet(RfxColorBox::_BASE_RGB_BOX_STYLE + QString("background-color: rgb(%1,%2,%3);").arg(r).arg(g).arg(b));

     this->_scacchiera = new QFrame(this);
     this->_scacchiera->setObjectName("scacchiera");
     this->_scacchiera->setStyleSheet(RfxColorBox::_BASE_RGBA_BOX_STYLE + QString("background-image: url(:/images/scacchiera.png);"));
     this->_scacchiera->setFixedSize (w, h);


     this->_rgbBox->raise();
     this->_rgbaBox->raise();

	 QVBoxLayout *verticalLayout = new QVBoxLayout();


	/* the sliders and text boxes */
	this->_redS = new QSlider();
	initSlider(_redS, r);
	this->_redT = new QLineEdit(QString().setNum(r));
	initTextBox(this->_redT);
	connect(this->_redT, SIGNAL(editingFinished()), this, SLOT(setR()));

	this->_greenS = new QSlider();
	initSlider(_greenS, g);
	this->_greenT = new QLineEdit(QString().setNum(g));
	initTextBox(this->_greenT);
	connect(this->_greenT, SIGNAL(editingFinished()), this, SLOT(setG()));

	this->_blueS = new QSlider();
	initSlider(_blueS, b);
	this->_blueT = new QLineEdit(QString().setNum(b));
	initTextBox(this->_blueT);
	connect(this->_blueT, SIGNAL(editingFinished()), this, SLOT(setB()));

	this->_alphaS = new QSlider();
	initSlider(_alphaS, a);
	this->_alphaT = new QLineEdit(QString().setNum(a));
	initTextBox(this->_alphaT);
	connect(this->_alphaT, SIGNAL(editingFinished()), this, SLOT(setA()));

	connectSliders();

	QHBoxLayout *redLayout = new QHBoxLayout();
	QWidget* redBox = new QWidget();
	redBox->setFixedSize (10, 10);
	redBox->setStyleSheet("background-color: rgb(255,0,0);");
	redLayout->addWidget(redBox);
	redLayout->addWidget(this->_redS);
	redLayout->addWidget(this->_redT);
	redLayout->setSpacing(3);

	QHBoxLayout *greenLayout = new QHBoxLayout();
	QWidget* greenBox = new QWidget();
	greenBox->setFixedSize (10, 10);
	greenBox->setStyleSheet("background-color: rgb(0,255,0);");
	greenLayout->addWidget(greenBox);
	greenLayout->addWidget(this->_greenS);
	greenLayout->addWidget(this->_greenT);
	greenLayout->setSpacing(3);

	QHBoxLayout *blueLayout = new QHBoxLayout();
	QWidget* blueBox = new QWidget();
	blueBox->setFixedSize (10, 10);
	blueBox->setStyleSheet("background-color: rgb(0,0,255);");
	blueLayout->addWidget(blueBox);
	blueLayout->addWidget(this->_blueS);
	blueLayout->addWidget(this->_blueT);
	blueLayout->setSpacing(3);

	QHBoxLayout *alphaLayout = new QHBoxLayout();
	QWidget* alphaBox = new QWidget();
	alphaBox->setFixedSize (10, 10);
	alphaBox->setStyleSheet("background-color: rgb(100,100,100);");
	alphaLayout->addWidget(alphaBox);
	alphaLayout->addWidget(this->_alphaS);
	alphaLayout->addWidget(this->_alphaT);
	alphaLayout->setSpacing(3);

	QGridLayout *colorDialogLayout = new QGridLayout();
	colorDialogLayout->setVerticalSpacing(0);
	colorDialogLayout->setHorizontalSpacing(0);
	colorDialogLayout->setMargin(0);
	colorDialogLayout->setSpacing(0);

	verticalLayout->addLayout(redLayout);
	verticalLayout->addLayout(greenLayout);
	verticalLayout->addLayout(blueLayout);
	verticalLayout->addLayout(alphaLayout);

	connect(this->_rgbaBox, SIGNAL(clicked()), this, SLOT(setBoxColorFromDialog()));

     QGridLayout *layout = new QGridLayout();
     layout->addWidget(_scacchiera, 0, 0);
     layout->addWidget(_rgbBox, 0, 0);
     layout->addWidget(_rgbaBox, 0, 0);
     this->_rgbBox->raise();
     this->_rgbaBox->raise();
     
	 colorDialogLayout->addLayout(verticalLayout, 0,0);
	 colorDialogLayout->addLayout(layout, 0,1);

	setLayout(colorDialogLayout);
}

RfxColorBox::~RfxColorBox()
{
	if(this->_rgbaBox)
		delete _rgbaBox;
	if(this->_rgbBox)
		delete _rgbBox;
	if(this->_scacchiera)
		delete _scacchiera;

	if(this->_redS)
		delete _redS;
	if(this->_greenS)
		delete _greenS;
	if(this->_blueS)
		delete _blueS;
	if(this->_alphaS)
		delete _alphaS;

	if(this->_redT)
		delete _redT;
	if(this->_greenT)
		delete _greenT;
	if(this->_blueT)
		delete _blueT;
	if(this->_alphaT)
		delete _alphaT;
}

/* Receives button click and shows a default system dialog for color selection.
Once the color has been selected sets it as the color of the widget and emits the color changes signals.
*/
void RfxColorBox::setBoxColorFromDialog(){
     
	QColor c = QColorDialog::getColor(QColor(_redS->value(), _greenS->value(), _blueS->value()));//(*(this->_color));
     if(c.isValid()){
		disconnectSliders();
		this->_redS->setValue(c.red());
		this->_redS->setToolTip(QString().setNum(c.red()));
		this->_redT->setText(QString().setNum(c.red()));

		this->_greenS->setValue(c.green());
		this->_greenS->setToolTip(QString().setNum(c.green()));
		this->_greenT->setText(QString().setNum(c.green()));

		this->_blueS->setValue(c.blue());
		this->_blueS->setToolTip(QString().setNum(c.blue()));
		this->_blueT->setText(QString().setNum(c.blue()));

		emit colorChanged();
		int r, g, b, a;
		r = c.red();
		g = c.green();
		b = c.blue();
		a = this->_alphaS->value();

		int* vals[4] = {&r, &g, &b, &a};
		this->_rgbaBox->setStyleSheet(getNewRGBAStylesheet(this->_rgbaBox->styleSheet(), CHANNEL_ALL, vals));
		this->_rgbBox->setStyleSheet(getNewRGBStylesheet(this->_rgbBox->styleSheet(), CHANNEL_ALL, vals));
		connectSliders();
     }
}

/* Sets just the red channel.
   @param r The new red value.
*/
void RfxColorBox::setR(int r){
	this->_redS->setToolTip(QString().setNum(r));
	this->_redT->setText(QString().setNum(r));
	emit colorChanged();
     int* vals[1] = {&r};
     this->_rgbaBox->setStyleSheet(getNewRGBAStylesheet(this->_rgbaBox->styleSheet(), CHANNEL_R, vals));
     this->_rgbBox->setStyleSheet(getNewRGBStylesheet(this->_rgbBox->styleSheet(), CHANNEL_R, vals));
}

/* Sets just the green channel.
   @param g The new green value.
*/
void RfxColorBox::setG(int g){
	this->_greenS->setToolTip(QString().setNum(g));
	this->_greenT->setText(QString().setNum(g));
	emit colorChanged();
     int* vals[1] = {&g};
     this->_rgbaBox->setStyleSheet(getNewRGBAStylesheet(this->_rgbaBox->styleSheet(), CHANNEL_G, vals));
     this->_rgbBox->setStyleSheet(getNewRGBStylesheet(this->_rgbBox->styleSheet(), CHANNEL_G, vals));
}

/* Sets just the blue channel.
   @param b The new blue value.
*/
void RfxColorBox::setB(int b){
this->_blueS->setToolTip(QString().setNum(b));
this->_blueT->setText(QString().setNum(b));
emit colorChanged();
     int* vals[1] = {&b};
	 this->_rgbaBox->setStyleSheet(getNewRGBAStylesheet(this->_rgbaBox->styleSheet(), CHANNEL_B, vals));
     this->_rgbBox->setStyleSheet(getNewRGBStylesheet(this->_rgbBox->styleSheet(), CHANNEL_B, vals));
}

/* Sets just the alpha channel.
   @param a The new alpha value.
*/
void RfxColorBox::setA(int a){
	this->_alphaS->setToolTip(QString().setNum(a));
	this->_alphaT->setText(QString().setNum(a));
	emit colorChanged();
     int* vals[1] = {&a};
     this->_rgbaBox->setStyleSheet(getNewRGBAStylesheet(this->_rgbaBox->styleSheet(), CHANNEL_A, vals));
}

/* Sets just the red channel.*/
void RfxColorBox::setR(){
	bool res = false;
	int val = this->_redT->text().toInt(&res);
	if (!res){
		this->_redT->setText(QString().setNum(this->_redS->value()));
		return;
	}

	if (val > 255)
		this->_redS->setValue(255);
	else if (val < 0)
		this->_redS->setValue(0);
	else
		this->_redS->setValue(val);
}

/* Sets just the green channel.*/
void RfxColorBox::setG(){
	bool res = false;
	int val = this->_greenT->text().toInt(&res);
	if (!res){
		this->_redT->setText(QString().setNum(this->_greenS->value()));
		return;
	}

	if (val > 255)
		this->_greenS->setValue(255);
	else if (val < 0)
		this->_greenS->setValue(0);
	else
		this->_greenS->setValue(val);
}

/* Sets just the blue channel.*/
void RfxColorBox::setB(){
	bool res = false;
	int val = this->_blueT->text().toInt(&res);
	if (!res){
		this->_blueT->setText(QString().setNum(this->_blueS->value()));
		return;
	}

	if (val > 255)
		this->_blueS->setValue(255);
	else if (val < 0)
		this->_blueS->setValue(0);
	else
		this->_blueS->setValue(val);
}

/* Sets just the alpha channel.*/
void RfxColorBox::setA(){
	bool res = false;
	int val = this->_alphaT->text().toInt(&res);
	if (!res){
		this->_alphaT->setText(QString().setNum(this->_alphaS->value()));
		return;
	}

	if (val > 255)
		this->_alphaS->setValue(255);
	else if (val < 0)
		this->_alphaS->setValue(0);
	else
		this->_alphaS->setValue(val);
}

/*
     Returns a string rapresenting a new stylesheet(with alpha channel)
	 @param stykeSheet the old stylesheet.
	 @param channels The channel that have to be modified.
	 @param vals the new values
	 @return the new stylesheet

*/
QString RfxColorBox::getNewRGBAStylesheet(const QString& styleSheet, RfxColorBox::Channels channels, int* vals[]){
     int openBracket = styleSheet.indexOf("(");
     int closedBracket = styleSheet.indexOf(")");
     QStringList list = styleSheet.mid(openBracket + 1, closedBracket - openBracket).split(",");
     QString newStyleSheet = RfxColorBox::_BASE_RGBA_BOX_STYLE + QString("background-color: rgba(");

	 switch(channels){
		 case CHANNEL_R :
          newStyleSheet = newStyleSheet+QString("%1,").arg(*vals[0])+list[1]+','+list[2]+','+list[3]+';';
		  break;

		 case CHANNEL_G:
          newStyleSheet = newStyleSheet+list[0]+','+QString("%1,").arg(*vals[0])+list[2]+','+list[3]+';';
		  break;

		 case CHANNEL_B:
		  newStyleSheet = newStyleSheet+list[0]+','+list[1]+','+QString("%1,").arg(*vals[0])+list[3]+';';   
		  break;

		 case CHANNEL_A:
		  newStyleSheet = newStyleSheet+list[0]+','+list[1]+','+list[2]+','+QString("%1);").arg(*vals[0]);
		  break;

		 case CHANNEL_ALL:
			newStyleSheet = newStyleSheet+QString("%1,").arg(*vals[0])+QString("%1,").arg(*vals[1])+QString("%1,").arg(*vals[2])+QString("%1);").arg(*vals[3]);
			break;
     }
     
     return newStyleSheet;
}

/*
	 Returns a string rapresenting a new stylesheet(without alpha channel)
	 @param stykeSheet the old stylesheet.
	 @param channels The channel that have to be modified.
	 @param vals the new values
	 @return the new stylesheet

*/
QString RfxColorBox::getNewRGBStylesheet(const QString& styleSheet, RfxColorBox::Channels channels, int* vals[]){
     int openBracket = styleSheet.indexOf("(");
     int closedBracket = styleSheet.indexOf(")");
     QStringList list = styleSheet.mid(openBracket + 1, closedBracket - openBracket).split(",");
     QString newStyleSheet = RfxColorBox::_BASE_RGB_BOX_STYLE + QString("background-color: rgb(");

	 switch(channels){
		 case CHANNEL_R :
          newStyleSheet = newStyleSheet+QString("%1,").arg(*vals[0])+list[1]+','+list[2]+';';
		  break;

		 case CHANNEL_G:
          newStyleSheet = newStyleSheet+list[0]+','+QString("%1,").arg(*vals[0])+list[2]+';';
		  break;

		 case CHANNEL_B:
		  newStyleSheet = newStyleSheet+list[0]+','+list[1]+','+QString("%1);").arg(*vals[0]);
		  break;

		 case CHANNEL_A:
		  break;

		 case CHANNEL_ALL:
			newStyleSheet = newStyleSheet+QString("%1,").arg(*vals[0])+QString("%1,").arg(*vals[1])+QString("%1);").arg(*vals[2]);
			break;
     }     
     return newStyleSheet;
}


/* Initialize a slider
@param slider the slider to initialize
@param value the initial value
*/
void RfxColorBox::initSlider(QSlider* slider, int value){
	slider->setTickPosition(QSlider::NoTicks);
	slider->setOrientation(Qt::Horizontal);
	slider->setMaximumSize(100, 15);

	slider->setTickInterval(1);
	slider->setRange(0,255);
	slider->setValue(value);
	slider->setToolTip(QString().setNum(value));
}

/* Initialize a text box.
@param box the text box to initialize
*/
void RfxColorBox::initTextBox(QLineEdit* box){
	box->setAlignment(Qt::AlignRight);
	box->setMaximumWidth(75);
	box->setMaximumHeight(15);
	box->setFont(QFont("verdana", 7));
}

/* Connects all the slider to the color box. */
void RfxColorBox::connectSliders(){
	connect(this->_redS, SIGNAL(valueChanged(int)), this, SLOT(setR(int)));
	 connect(this->_greenS, SIGNAL(valueChanged(int)), this, SLOT(setG(int)));
	 connect(this->_blueS, SIGNAL(valueChanged(int)), this, SLOT(setB(int)));
	 connect(this->_alphaS, SIGNAL(valueChanged(int)), this, SLOT(setA(int)));
}

/* Disconnects all the slider to the color box. */
void RfxColorBox::disconnectSliders(){
	disconnect(this->_redS, SIGNAL(valueChanged(int)), this, SLOT(setR(int)));
	 disconnect(this->_greenS, SIGNAL(valueChanged(int)), this, SLOT(setG(int)));
	 disconnect(this->_blueS, SIGNAL(valueChanged(int)), this, SLOT(setB(int)));
	 disconnect(this->_alphaS, SIGNAL(valueChanged(int)), this, SLOT(setA(int)));
}