/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2009                                          \/)\/    *
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

#include "shaderDialog.h"
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#define DECFACTOR 100000.0f

using namespace vcg;

ShaderDialog::ShaderDialog(ShaderInfo *sInfo, QGLWidget* gla, RenderMode &rm, QWidget *parent)
: QDockWidget(parent)
{
	ui.setupUi(this);
  this->setWidget(ui.frame);
	this->setFeatures(QDockWidget::AllDockWidgetFeatures);
	this->setAllowedAreas(Qt::LeftDockWidgetArea);
	this->setFloating(true);

	shaderInfo = sInfo;
	glarea = gla;
	rendMode = &rm;
	colorSignalMapper = new QSignalMapper(this);
	valueSignalMapper = new QSignalMapper(this);

	QGridLayout * qgrid = new QGridLayout(ui.uvTab);
	qgrid->setColumnMinimumWidth(0, 45);
	qgrid->setColumnMinimumWidth(1, 40);
	qgrid->setColumnMinimumWidth(2, 40);
	qgrid->setColumnMinimumWidth(3, 40);

	QLabel *perVertexColorLabel = new QLabel(this);
	perVertexColorLabel->setText("Use PerVertex Color");
	QCheckBox *perVertexColorCBox = new QCheckBox(this);
	rendMode->colorMode =  GLW::CMNone;
	connect(perVertexColorCBox, SIGNAL(stateChanged(int)), this, SLOT(setColorMode(int)));

	qgrid->addWidget(perVertexColorLabel, 0, 0);
	qgrid->addWidget(perVertexColorCBox, 0, 1);

	int row=1;
	std::map<QString, UniformVariable>::iterator i;
	for (i = shaderInfo->uniformVars.begin(); i != shaderInfo->uniformVars.end(); ++i) {

		QLabel *varNameLabel = new QLabel(this);
		varNameLabel->setObjectName(i->first+"_name");
		varNameLabel->setText(i->first);

		qgrid->addWidget(varNameLabel, row, 0);

		int varNum = getVarsNumber(i->second.type);

		switch (i->second.widget) {

			case WIDGET_NONE: {
				for (int j=0;j<varNum;++j) {
					QLineEdit *qline = new QLineEdit(this);
					qline->setAlignment(Qt::AlignRight);
					qline->setObjectName(tr("%1%2").arg(i->first).arg(j));
					if (i->second.type == SINGLE_INT) {
						qline->setText(tr("%1").arg(i->second.ival[j]));

					} else {
						qline->setText(tr("%1").arg(i->second.fval[j]));

					}

					connect(qline, SIGNAL(textChanged(QString)), valueSignalMapper, SLOT(map()));
					valueSignalMapper->setMapping(qline,tr("%1%2").arg(i->first).arg(j));
					lineEdits[tr("%1%2").arg(i->first).arg(j)]=qline;

					qgrid->addWidget(qline, row, j+1);
				}
												} break;
			case WIDGET_COLOR : {

				QPushButton * colorButton = new QPushButton(this);
				colorButton->setText("Change");
				connect(colorButton, SIGNAL(clicked()), colorSignalMapper, SLOT(map()));
				colorSignalMapper->setMapping(colorButton,i->first);					

				qgrid->addWidget(colorButton, row, 1);


													} break;
			case WIDGET_SLIDER : {
				for (int j=0;j<varNum;++j) {
					QSlider *qslider = new QSlider(this);
					qslider->setTickPosition(QSlider::NoTicks);
					qslider->setOrientation(Qt::Horizontal);
					qslider->setObjectName(tr("%1%2").arg(i->first).arg(j));
					if (i->second.type == SINGLE_INT) {
						qslider->setTickInterval(i->second.step);
						qslider->setRange(i->second.min, i->second.max);
						qslider->setValue(i->second.ival[j]);
					} else {
						qslider->setTickInterval(i->second.step*DECFACTOR);
						qslider->setRange(i->second.min*DECFACTOR, i->second.max*DECFACTOR);
						qslider->setValue(i->second.fval[j]*DECFACTOR);						
					}
					connect(qslider, SIGNAL(valueChanged(int)), valueSignalMapper, SLOT(map()));
					valueSignalMapper->setMapping(qslider,tr("%1%2").arg(i->first).arg(j));

					sliders[tr("%1%2").arg(i->first).arg(j)]=qslider;

					qgrid->addWidget(qslider, row, j+1);

				}			 
													 } break;
		}	
		++row;
	}


	connect(colorSignalMapper, SIGNAL(mapped(const QString &)), this, SLOT(setColorValue(const QString &)));
	connect(valueSignalMapper, SIGNAL(mapped(const QString &)), this, SLOT(valuesChanged(const QString &)));

	//Texture Tab Section

	if (shaderInfo->textureInfo.size() > 0) {
		textLineSignalMapper = new QSignalMapper(this);
		textButtonSignalMapper = new QSignalMapper(this);


		QGridLayout * qgridTexTab = new QGridLayout(ui.textureTab);
		qgridTexTab->setColumnMinimumWidth(0, 45);
		qgridTexTab->setColumnMinimumWidth(1, 40);

		row = 0;
		std::vector<TextureInfo>::iterator textIter;
		for (textIter = shaderInfo->textureInfo.begin(); textIter != shaderInfo->textureInfo.end(); ++textIter) {
			QLabel *textNameLabel = new QLabel(this);
			QLineEdit *textValueEdit = new QLineEdit(this);
			QPushButton * textButton = new QPushButton(this);
			textButton->setText("Browse");
			textNameLabel->setText(tr("Texture Unit %1:").arg(row));
			textValueEdit->setText(textIter->path);
			qgridTexTab->addWidget(textNameLabel, row, 0);
			qgridTexTab->addWidget(textValueEdit, row, 1);
			qgridTexTab->addWidget(textButton, row, 2);


			connect(textValueEdit, SIGNAL(editingFinished()), textLineSignalMapper, SLOT(map()));
			textLineSignalMapper->setMapping(textValueEdit,row);

			connect(textButton, SIGNAL(clicked()), textButtonSignalMapper, SLOT(map()));
			textButtonSignalMapper->setMapping(textButton,row);

			textLineEdits.push_back(textValueEdit);
			++row;
		}

		connect(textLineSignalMapper, SIGNAL(mapped(int)), this, SLOT(changeTexturePath(int)));
		connect(textButtonSignalMapper, SIGNAL(mapped(int)), this, SLOT(browseTexturePath(int)));
	}

	//OpenGL Status Tab Section

	QGridLayout * qgridGlStatus = new QGridLayout(ui.glTab);
	qgridGlStatus->setColumnMinimumWidth(0, 45);
	qgridGlStatus->setColumnMinimumWidth(1, 40);

	row = 0;
	std::map<int, QString>::iterator glIterator;
	for (glIterator = shaderInfo->glStatus.begin(); glIterator != shaderInfo->glStatus.end(); ++glIterator) {
		QLabel *glVarLabel = new QLabel(this);
		QLabel *glValueLabel = new QLabel(this);


		switch (glIterator->first) {
			case SHADE:  glVarLabel->setText("glShadeModel"); glValueLabel->setText(glIterator->second); ++row; break;
			case ALPHA_TEST:  glVarLabel->setText("GL_ALPHA_TEST"); glValueLabel->setText(glIterator->second); ++row; break;
			case ALPHA_FUNC:  glVarLabel->setText("glAlphaFunc"); glValueLabel->setText(glIterator->second + ", " + shaderInfo->glStatus[ALPHA_CLAMP]); ++row; break;
				//case ALPHA_CLAMP: used in ALPHA_FUNC
			case BLENDING:  glVarLabel->setText("GL_BLEND"); glValueLabel->setText(glIterator->second); ++row; break;
			case BLEND_FUNC_SRC:  glVarLabel->setText("glBlendFunc"); glValueLabel->setText(glIterator->second + ", " + shaderInfo->glStatus[BLEND_FUNC_SRC]); ++row; break;
				//case BLEND_FUNC_DST: used in BLEND_FUNC_SRC
			case BLEND_EQUATION: glVarLabel->setText("glBlendEquation"); glValueLabel->setText(glIterator->second); ++row; break;
			case DEPTH_TEST: glVarLabel->setText("GL_DEPTH_TEST"); glValueLabel->setText(glIterator->second); ++row; break;
			case DEPTH_FUNC: glVarLabel->setText("glDepthFunc"); glValueLabel->setText(glIterator->second); ++row; break;
				//case CLAMP_NEAR:
				//case CLAMP_FAR:
			case CLEAR_COLOR_R: glVarLabel->setText("glClearColor"); glValueLabel->setText(glIterator->second + ", " +
														shaderInfo->glStatus[CLEAR_COLOR_G] + ", " +
														shaderInfo->glStatus[CLEAR_COLOR_B] + ", " +
														shaderInfo->glStatus[CLEAR_COLOR_A]); ++row; break;
				//case CLEAR_COLOR_G: used in CLEAR_COLOR_R
				//case CLEAR_COLOR_B: used in CLEAR_COLOR_R
				//case CLEAR_COLOR_A: used in CLEAR_COLOR_R
		}

		qgridGlStatus->addWidget(glVarLabel, row, 0);
		qgridGlStatus->addWidget(glValueLabel, row, 1);

	}

	//Vertex and Fragment Program Tabs Section
	
	QFile qf;
	QTextStream ts(&qf);

	qf.setFileName(shaderInfo->vpFile);
	if (!qf.open(QIODevice::ReadOnly | QIODevice::Text) )
		QMessageBox::critical(this,"Opengl Shader" ,"unable to open file");
	ui.vpTextBrowser->insertPlainText(ts.readAll());
	qf.close();

	qf.setFileName(shaderInfo->fpFile);
	if (!qf.open(QIODevice::ReadOnly | QIODevice::Text) )
		QMessageBox::critical(this,"Opengl Shader" ,"unable to open file");
	ui.fpTextBrowser->insertPlainText(ts.readAll());
	qf.close();

	//End of Vertex and Fragment Program Tabs Section


//	this->setWindowFlags(Qt::WindowStaysOnTopHint);
	connect(ui.okButton, SIGNAL(clicked()), this, SLOT(accept()));




}

ShaderDialog::~ShaderDialog()
{

}

void ShaderDialog::setColorValue(const QString &varName)
{
	QColor old;
	if ( shaderInfo->uniformVars[varName].type == ARRAY_3_FLOAT) {
		old.setRgbF(shaderInfo->uniformVars[varName].fval[0], shaderInfo->uniformVars[varName].fval[1], shaderInfo->uniformVars[varName].fval[2]);
	} else if (shaderInfo->uniformVars[varName].type == ARRAY_4_FLOAT) {
		old.setRgbF(shaderInfo->uniformVars[varName].fval[0], shaderInfo->uniformVars[varName].fval[1], shaderInfo->uniformVars[varName].fval[2], shaderInfo->uniformVars[varName].fval[3]);
	}

	QColor newColor = QColorDialog::getColor(old, this);
	if (newColor.isValid()) {

		shaderInfo->uniformVars[varName].fval[0] = newColor.redF(); 
		shaderInfo->uniformVars[varName].fval[1] = newColor.greenF();
		shaderInfo->uniformVars[varName].fval[2] = newColor.blueF();

		if (shaderInfo->uniformVars[varName].type == ARRAY_4_FLOAT) {

			shaderInfo->uniformVars[varName].fval[3] = newColor.alphaF();

		}
	}
	glarea->updateGL();
}


void ShaderDialog::valuesChanged(const QString &varNameAndIndex) {


	int varIndex = varNameAndIndex[varNameAndIndex.length()-1].digitValue();
	QString varName = varNameAndIndex;
	varName.chop(1);

	int varWidget = shaderInfo->uniformVars[varName].widget;
	int varType = shaderInfo->uniformVars[varName].type;
	switch (varWidget) {
		case WIDGET_NONE: {
			QLineEdit * qline = lineEdits[varNameAndIndex];
			if (varType == SINGLE_INT) {
				shaderInfo->uniformVars[varName].ival[varIndex] = qline->text().toInt();
			} else {
				shaderInfo->uniformVars[varName].fval[varIndex] = qline->text().toFloat();
			}
											} break;
		case WIDGET_SLIDER: {
			QSlider * qslider = sliders[varNameAndIndex];
			if (varType == SINGLE_INT) {
				shaderInfo->uniformVars[varName].ival[varIndex] = qslider->value();
			} else {
				shaderInfo->uniformVars[varName].fval[varIndex] = qslider->value()/DECFACTOR; 
			}
												} break;
	}
	glarea->updateGL();

}


void ShaderDialog::setColorMode(int state) {
	if (state == Qt::Checked) {
		rendMode->colorMode = GLW::CMPerVert;
	} else {
		rendMode->colorMode = GLW::CMNone;
	}
	glarea->updateGL();
}

void ShaderDialog::changeTexturePath(int i) {
	shaderInfo->textureInfo[i].path = textLineEdits[i]->text();
	reloadTexture(i);
}

void ShaderDialog::browseTexturePath(int i) {
	QFileDialog fd(0,"Choose new texture");

	QDir shadersDir = QDir(qApp->applicationDirPath());
#if defined(Q_OS_WIN)
	if (shadersDir.dirName() == "debug" || shadersDir.dirName() == "release")
		shadersDir.cdUp();
#elif defined(Q_OS_MAC)
	if (shadersDir.dirName() == "MacOS") {
		shadersDir.cdUp();
		shadersDir.cdUp();
		shadersDir.cdUp();
	}
#endif
	shadersDir.cd("textures");
	
	fd.setDirectory(shadersDir);
	fd.move(500, 100);

	QStringList newPath;
	if (fd.exec())
	{
		newPath = fd.selectedFiles();
		textLineEdits[i]->setText(newPath.at(0));
		shaderInfo->textureInfo[i].path = newPath.at(0);
		reloadTexture(i);
	} 
	
}

void ShaderDialog::reloadTexture(int i) {
	glDeleteTextures( 1, &shaderInfo->textureInfo[i].tId);

	glEnable(shaderInfo->textureInfo[i].Target);
	QImage img, imgScaled, imgGL;
	img.load(shaderInfo->textureInfo[i].path);
	// image has to be scaled to a 2^n size. We choose the first 2^N <= picture size.
	int bestW=pow(2.0,floor(::log(double(img.width() ))/::log(2.0)));
	int bestH=pow(2.0,floor(::log(double(img.height()))/::log(2.0)));
	imgScaled=img.scaled(bestW,bestH,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
	imgGL=QGLWidget::convertToGLFormat(imgScaled);

	glGenTextures( 1, &(shaderInfo->textureInfo[i].tId) );
	glBindTexture( shaderInfo->textureInfo[i].Target, shaderInfo->textureInfo[i].tId );
	glTexImage2D( shaderInfo->textureInfo[i].Target, 0, 3, imgGL.width(), imgGL.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, imgGL.bits() );
	glTexParameteri( shaderInfo->textureInfo[i].Target, GL_TEXTURE_MIN_FILTER, shaderInfo->textureInfo[i].MinFilter );
	glTexParameteri( shaderInfo->textureInfo[i].Target, GL_TEXTURE_MAG_FILTER, shaderInfo->textureInfo[i].MagFilter ); 
	glTexParameteri( shaderInfo->textureInfo[i].Target, GL_TEXTURE_WRAP_S, shaderInfo->textureInfo[i].WrapS ); 
	glTexParameteri( shaderInfo->textureInfo[i].Target, GL_TEXTURE_WRAP_T, shaderInfo->textureInfo[i].WrapT ); 
	glTexParameteri( shaderInfo->textureInfo[i].Target, GL_TEXTURE_WRAP_R, shaderInfo->textureInfo[i].WrapR ); 

	glarea->updateGL();
}