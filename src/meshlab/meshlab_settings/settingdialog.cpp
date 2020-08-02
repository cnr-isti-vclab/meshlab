/****************************************************************************
* VCGLib                                                            o o     *
* Visual and Computer Graphics Library                            o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004-2020                                           \/)\/    *
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

#include "settingdialog.h"

SettingDialog::SettingDialog(
		const RichParameter& currentPar,
		const RichParameter& defaultPar,
		QWidget* parent) :
	QDialog(parent),
	frame(this),
	curPar(currentPar.clone()),
	defPar(defaultPar.clone())
{
	tmpParSet = RichParameterList();
	tmpParSet.addParam(*curPar);
	frame.loadFrameContent(tmpParSet);

	setModal(true);

	//no need to delete these objects, thanks to qt's parent resource management
	savebut = new QPushButton("Save",this);
	resetbut = new QPushButton("Reset",this);
	applybut = new QPushButton("Apply",this);
	loadbut = new QPushButton("Load",this);
	closebut = new QPushButton("Close",this);

	QGridLayout* dialoglayout = new QGridLayout(parent);

	dialoglayout->addWidget(savebut,1,0);
	dialoglayout->addWidget(resetbut,1,1);
	dialoglayout->addWidget(loadbut,1,2);
	dialoglayout->addWidget(applybut,1,3);
	dialoglayout->addWidget(closebut,1,4);


	dialoglayout->addWidget(&frame,0,0,1,5);
	dialoglayout->setSizeConstraint(QLayout::SetFixedSize);
	setLayout(dialoglayout);

	connect(applybut,SIGNAL(clicked()),this,SLOT(apply()));
	connect(resetbut,SIGNAL(clicked()),this,SLOT(reset()));
	connect(savebut,SIGNAL(clicked()),this,SLOT(save()));
	connect(loadbut,SIGNAL(clicked()),this,SLOT(load()));
	connect(closebut,SIGNAL(clicked()),this,SLOT(close()));
}

SettingDialog::~SettingDialog()
{
}

void SettingDialog::save()
{
	apply();
	const RichParameter& tmppar = frame.stdfieldwidgets.at(0)->richParameter();
	QDomDocument doc("MeshLabSettings");
	doc.appendChild(tmppar.fillToXMLDocument(doc));
	QString docstring =  doc.toString();
	qDebug("Writing into Settings param with name %s and content ****%s****", qUtf8Printable(tmppar.name()), qUtf8Printable(docstring));
	QSettings setting;
	setting.setValue(tmppar.name(),QVariant(docstring));
	curPar->setValue(tmppar.value());
}

void SettingDialog::apply()
{
	assert(frame.stdfieldwidgets.size() == 1);
	curPar->setValue(frame.stdfieldwidgets.at(0)->widgetValue());
	emit applySettingSignal(*curPar);
}

void SettingDialog::reset()
{
	qDebug("resetting the value of param %s to the hardwired default", qUtf8Printable(curPar->name()));

	assert(frame.stdfieldwidgets.size() == 1);
	frame.stdfieldwidgets.at(0)->setValue(defPar->value());
	apply();
}

void SettingDialog::load()
{
	assert(frame.stdfieldwidgets.size() == 1);
	frame.stdfieldwidgets.at(0)->setWidgetValue(curPar->value());
}


