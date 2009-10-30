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
/****************************************************************************
  History
$Log$
Revision 1.8  2007/03/26 08:25:09  zifnab1974
added eol at the end of the files

Revision 1.7  2006/01/16 05:34:16  cignoni
Added backward qt4.0 compatibility for setAutoFillBackground

Revision 1.6  2006/01/15 15:27:59  glvertex
Added few lines to set background even in qt 4.1

Revision 1.5  2006/01/02 18:54:52  glvertex
added multilevel logging support

Revision 1.4  2006/01/02 17:39:18  glvertex
Added info types in a combobox

Revision 1.3  2005/12/04 16:50:15  glvertex
Removed [using namespace] directive form .h
Renaming in QT style
Adapted method behavior to the new ui interface

Revision 1.2  2005/12/03 22:49:46  cignoni
Added copyright info

****************************************************************************/

#include "ui_customDialog.h"
#include "customDialog.h"
#include <QPalette>
#include "stdpardialog.h"

using namespace vcg;

CustomDialog::CustomDialog(RichParameterSet& parset,QWidget * parent)
		:QDialog(parent),richparset(parset)
{
	setModal(false);
	closebut = new QPushButton("Close",this);
	//QVBoxLayout* layout = new QVBoxLayout(parent);
	QGridLayout* layout = new QGridLayout(parent);
	setLayout(layout);
	listwid = new QListWidget(this); 
	for(int ii = 0;ii < richparset.paramList.size();++ii)
	{
		//butt = new QPushButton(richparset.paramList.at(ii)->name,this);
		QListWidgetItem* item = new QListWidgetItem(parset.paramList.at(ii)->name,listwid);
		mp.insert(item,parset.paramList.at(ii));
		//connect(butt,SIGNAL(clicked()),this,SLOT(openSubDialog()));
	}
	layout->addWidget(listwid,0,0,1,4);
	layout->addWidget(closebut,1,3);
	connect(listwid,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(openSubDialog(QListWidgetItem*)));
	connect(closebut,SIGNAL(clicked()),this,SLOT(close()));
}

void CustomDialog::openSubDialog( QListWidgetItem* itm )
{
	QMap<QListWidgetItem*,RichParameter*>::iterator it = mp.find(itm);
	if (it != mp.end())
	{
		SettingDialog* setdial = new SettingDialog(it.value(),this);
		connect(setdial,SIGNAL(applySettingSignal()),this,SIGNAL(applyCustomSetting()));
		setdial->exec();
		delete setdial;
	}
}

CustomDialog::~CustomDialog()
{
	delete listwid;
	delete closebut;
	//RichParameter will be destroyed elsewhere
	//for(QMap<QListWidgetItem*,RichParameter*>::iterator it = mp.begin();it != mp.end();it++)
	//	delete it.key();
}
//Maybe a MeshDocument parameter is needed. See loadFrameContent definition
SettingDialog::SettingDialog( RichParameter* rpar,QWidget* parent /*= 0*/ )
:QDialog(parent),frame(this),tmppar(NULL),richpar(rpar)
{
	setModal(true);
	savebut = new QPushButton("Save",this);
	resetbut = new QPushButton("Reset",this);
	applybut = new QPushButton("Apply",this);
	closebut = new QPushButton("Close",this);

	QGridLayout* dialoglayout = new QGridLayout(parent);
	
	dialoglayout->addWidget(savebut,1,0);
	dialoglayout->addWidget(resetbut,1,1);
	dialoglayout->addWidget(applybut,1,2);
	dialoglayout->addWidget(closebut,1,3);

	RichParameterCopyConstructor cp;
	richpar->accept(cp);
	tmppar = cp.lastCreated;
	frame.loadFrameContent(tmppar);
	dialoglayout->addWidget(&frame,0,0,1,4);
	dialoglayout->setSizeConstraint(QLayout::SetFixedSize);
	setLayout(dialoglayout);
	connect(applybut,SIGNAL(clicked()),this,SLOT(apply()));
	connect(resetbut,SIGNAL(clicked()),this,SLOT(reset()));
	connect(savebut,SIGNAL(clicked()),this,SLOT(save()));
	connect(closebut,SIGNAL(clicked()),this,SLOT(close()));
}

void SettingDialog::save()
{
	QDomDocument doc("MeshLabSettings");
	RichParameterXMLVisitor v(doc);
	tmppar->accept(v);
	doc.appendChild(v.parElem);
	QString docstring =  doc.toString();
	QSettings setting;
	setting.setValue(tmppar->name,QVariant(docstring));
	apply();
}

void SettingDialog::apply()
{
	assert(frame.stdfieldwidgets.size() == 1);
	frame.stdfieldwidgets.at(0)->collectWidgetValue();
	richpar->val->set(*tmppar->val);
	emit applySettingSignal();
}

void SettingDialog::reset()
{
	assert(frame.stdfieldwidgets.size() == 1);
	frame.stdfieldwidgets.at(0)->resetValue();
}

SettingDialog::~SettingDialog()
{
	delete tmppar;
	delete savebut;
	delete resetbut;
	delete applybut;
	delete closebut;
	//RichParameter Value will be destroyed elsewhere
}