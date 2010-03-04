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

#include "ui_customDialog.h"
#include "customDialog.h"
#include <QPalette>
 #include <QHeaderView>
#include "stdpardialog.h"
using namespace vcg;

CustomDialog::CustomDialog(RichParameterSet& curparset, RichParameterSet& defparset, QWidget * parent)
		:QDialog(parent),curParSet(curparset),defParSet(defparset)
{
	setModal(false);
	closebut = new QPushButton("Close",this);
	//QVBoxLayout* layout = new QVBoxLayout(parent);
	QGridLayout* layout = new QGridLayout(parent);
	setLayout(layout);
	tw = new QTableWidget(curParSet.paramList.size(),2,this);
	updateSettings();
	int totlen = tw->columnWidth(0) + tw->columnWidth(1) + this->frameSize().width();
	setMinimumWidth(totlen);
	layout->addWidget(tw,0,0,1,5);
	layout->addWidget(closebut,1,4,1,1);
	connect(tw,SIGNAL(itemDoubleClicked(QTableWidgetItem* )),this,SLOT(openSubDialog(QTableWidgetItem*)));
	connect(closebut,SIGNAL(clicked()),this,SLOT(close()));
	this->setWindowTitle(tr("Global Parameters Window"));
}

void CustomDialog::openSubDialog( QTableWidgetItem* itm )
{
	int rprow = tw->row(itm);
	RichParameter *defPar = defParSet.findParameter(vrp[rprow]->name);
	SettingDialog* setdial = new SettingDialog(vrp[rprow],defPar,this);
	connect(setdial,SIGNAL(applySettingSignal()),this,SIGNAL(applyCustomSetting()));
	connect(setdial,SIGNAL(applySettingSignal()),this,SLOT(updateSettings()));
	setdial->exec();
	delete setdial;
}

CustomDialog::~CustomDialog()
{
}

void CustomDialog::updateSettings()
{
	RichParameterToQTableWidgetItemConstructor v;
	QStringList slst;
	slst.push_back("Variable Name");
	slst.push_back("Variable Value");
	tw->setHorizontalHeaderLabels(slst);
	tw->horizontalHeader()->setStretchLastSection(true);
	tw->setShowGrid(true);
	//tw->setWordWrap(false);
	tw->verticalHeader()->hide();

	tw->setSelectionBehavior(QAbstractItemView::SelectRows);
	
	//tw->horizontalHeader()->setResizeMode(tw->columnCount() - 1, QHeaderView::Stretch);
	
	//int sz = tw->font().pointSize();
	for(int ii = 0;ii < curParSet.paramList.size();++ii)
	{
		QTableWidgetItem* item = new QTableWidgetItem(curParSet.paramList.at(ii)->name);
		item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsDragEnabled |Qt::ItemIsDropEnabled |Qt::ItemIsUserCheckable |Qt::ItemIsEnabled);

		//if (maxlen[0] < item->text().size() * sz)
		//	maxlen[0] = item->text().size() * sz;
		//item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

		tw->setItem(ii,0,item);
		//butt = new QPushButton(richparset.paramList.at(ii)->name,this);

		curParSet.paramList[ii]->accept(v);
		v.lastCreated->setFlags(Qt::ItemIsSelectable | Qt::ItemIsDragEnabled |Qt::ItemIsDropEnabled |Qt::ItemIsUserCheckable |Qt::ItemIsEnabled);
		tw->setItem(ii,1,v.lastCreated);
		//if (maxlen[1] < v.lastCreated->text().size() * sz)
		//	maxlen[1] = v.lastCreated->text().size() * sz;
		vrp.push_back(curParSet.paramList.at(ii));
	}
	tw->resizeColumnsToContents();
	tw->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	//tw->setColumnWidth(0,tw->horizontalHeader()->width());
	//tw->setColumnWidth(1,tw->horizontalHeader()->width());

	/*emit tw->horizontalHeader()->sectionAutoResize( 0,QHeaderView::ResizeToContents);
	emit tw->horizontalHeader()->sectionAutoResize( 1,QHeaderView::ResizeToContents);*/
}
//Maybe a MeshDocument parameter is needed. See loadFrameContent definition
SettingDialog::SettingDialog( RichParameter* currentPar, RichParameter* defaultPar, QWidget* parent /*= 0*/ )
:QDialog(parent),frame(this),curPar(currentPar),defPar(defaultPar),tmppar(NULL)
{
	setModal(true);
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

	RichParameterCopyConstructor cp;
	curPar->accept(cp);
	tmppar = cp.lastCreated;
	frame.loadFrameContent(tmppar);
	dialoglayout->addWidget(&frame,0,0,1,5);
	dialoglayout->setSizeConstraint(QLayout::SetFixedSize);
	setLayout(dialoglayout);
	connect(applybut,SIGNAL(clicked()),this,SLOT(apply()));
	connect(resetbut,SIGNAL(clicked()),this,SLOT(reset()));
	connect(savebut,SIGNAL(clicked()),this,SLOT(save()));
	connect(loadbut,SIGNAL(clicked()),this,SLOT(load()));
	connect(closebut,SIGNAL(clicked()),this,SLOT(close()));
}

void SettingDialog::save()
{
	apply();
	QDomDocument doc("MeshLabSettings");
	RichParameterXMLVisitor v(doc);
	tmppar->accept(v);
	doc.appendChild(v.parElem);
	QString docstring =  doc.toString();
	qDebug("Writing into Settings param with name %s and content ****%s****",qPrintable(tmppar->name),qPrintable(docstring));
	QSettings setting;
	setting.setValue(tmppar->name,QVariant(docstring));
	tmppar->pd->defVal->set(*tmppar->val);
}

void SettingDialog::apply()
{
	assert(frame.stdfieldwidgets.size() == 1);
	frame.stdfieldwidgets.at(0)->collectWidgetValue();
	curPar->val->set(*tmppar->val);
	emit applySettingSignal();
}

void SettingDialog::reset()
{
	qDebug("resetting the value of param %s to the hardwired default",qPrintable(curPar->name));
	tmppar->val->set(*defPar->val);
	assert(frame.stdfieldwidgets.size() == 1);
	frame.stdfieldwidgets.at(0)->setWidgetValue(*tmppar->val);
	apply();
}

void SettingDialog::load()
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
	delete loadbut;
	//RichParameter Value will be destroyed elsewhere
}
