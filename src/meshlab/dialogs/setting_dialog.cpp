/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005-2020                                           \/)\/    *
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

#include "setting_dialog.h"

#include <QSettings>

SettingDialog::SettingDialog(
		const RichParameter& currentParam,
		const RichParameter& defaultParam,
		QWidget* parent) :
	QDialog(parent),
	currentParameter(currentParam.clone()),
	savedParameter(currentParam.clone()),
	defaultParameter(defaultParam),
	frame(*currentParameter, defaultParameter, this)
{

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
	delete currentParameter;
	delete savedParameter;
}

void SettingDialog::save()
{
	apply();
	delete savedParameter;
	savedParameter = currentParameter->clone();
	QDomDocument doc("MeshLabSettings");
	doc.appendChild(currentParameter->fillToXMLDocument(doc));
	QString docstring =  doc.toString();
	qDebug("Writing into Settings param with name %s and content ****%s****", qUtf8Printable(currentParameter->name()), qUtf8Printable(docstring));
	QSettings setting;
	setting.setValue(currentParameter->name(),QVariant(docstring));
}

void SettingDialog::apply()
{
	assert(frame.size() == 1);
	currentParameter->setValue(frame.begin()->second->widgetValue());
	emit applySettingSignal(*currentParameter);
}

void SettingDialog::reset()
{
	qDebug("resetting the value of param %s to the hardwired default", qUtf8Printable(currentParameter->name()));

	assert(frame.size() == 1);
	frame.begin()->second->setValue(defaultParameter.value());
	apply();
}

void SettingDialog::load()
{
	assert(frame.size() == 1);
	frame.begin()->second->setWidgetValue(savedParameter->value());
	apply();
}
