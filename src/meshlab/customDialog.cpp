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
#include <common/filter_parameter/rich_parameter_set.h>
#include "customDialog.h"
#include <QPalette>
 #include <QHeaderView>
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
	RichParameter *defPar = defParSet.findParameter(vrp[rprow]->name());
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
		QTableWidgetItem* item = new QTableWidgetItem(curParSet.paramList.at(ii)->name());
		item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsDragEnabled |Qt::ItemIsDropEnabled |Qt::ItemIsUserCheckable |Qt::ItemIsEnabled);

		//if (maxlen[0] < item->text().size() * sz)
		//	maxlen[0] = item->text().size() * sz;
		//item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

		tw->setItem(ii,0,item);
		//butt = new QPushButton(richparset.paramList.at(ii)->name,this);

		QTableWidgetItem* twi = createQTableWidgetItemFromRichParameter(*curParSet.paramList[ii]);
		twi->setFlags(Qt::ItemIsSelectable | Qt::ItemIsDragEnabled |Qt::ItemIsDropEnabled |Qt::ItemIsUserCheckable |Qt::ItemIsEnabled);
		tw->setItem(ii,1,twi);
		//if (maxlen[1] < twi->text().size() * sz)
		//	maxlen[1] = twi->text().size() * sz;
		vrp.push_back(curParSet.paramList.at(ii));
	}
	tw->resizeColumnsToContents();
	tw->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	//tw->setColumnWidth(0,tw->horizontalHeader()->width());
	//tw->setColumnWidth(1,tw->horizontalHeader()->width());

	/*emit tw->horizontalHeader()->sectionAutoResize( 0,QHeaderView::ResizeToContents);
	emit tw->horizontalHeader()->sectionAutoResize( 1,QHeaderView::ResizeToContents);*/
}

QTableWidgetItem* CustomDialog::createQTableWidgetItemFromRichParameter(const RichParameter& pd)
{
	if (pd.value().isAbsPerc()){
		return new QTableWidgetItem(QString::number(pd.value().getAbsPerc()));
	}
	else if (pd.value().isDynamicFloat()){
		return new QTableWidgetItem(QString::number(pd.value().getDynamicFloat()));
	}
	else if (pd.value().isEnum()){
		return new QTableWidgetItem(QString::number(pd.value().getEnum()));
	}
	else if (pd.value().isBool()){
		if (pd.value().getBool())
			return new QTableWidgetItem("true"/*,lst*/);
		else
			return new QTableWidgetItem("false"/*,lst*/);
	}
	else if (pd.value().isInt()){
		return new QTableWidgetItem(QString::number(pd.value().getInt()));
	}
	else if (pd.value().isFloat()){
		return new QTableWidgetItem(QString::number(pd.value().getFloat()));
	}
	else if (pd.value().isString()){
		return new QTableWidgetItem(pd.value().getString());
	}
	else if (pd.value().isMatrix44f()){
		assert(0);
		return nullptr;
	}
	else if (pd.value().isPoint3f()){
		vcg::Point3f pp = pd.value().getPoint3f();
		QString pst = "P3(" + QString::number(pp.X()) + "," + QString::number(pp.Y()) + "," + QString::number(pp.Z()) + ")";
		return new QTableWidgetItem(pst);
	}
	else if (pd.value().isShotf()){
		assert(0); ///
		return new QTableWidgetItem(QString("TODO"));
	}
	else if (pd.value().isColor()){
		QPixmap pix(10,10);
		pix.fill(pd.value().getColor());
		QIcon ic(pix);
		return new QTableWidgetItem(ic,"");
	}
	else if (pd.value().isFileName() && pd.stringType() == "RichOpenFile"){
		new QTableWidgetItem(pd.value().getFileName());
	}
	else if (pd.value().isFileName() && pd.stringType() == "RichSaveFile"){
		assert(0);
		return nullptr;
	}
	else if (pd.value().isMesh()){
		assert(0);
		return nullptr;
	}
	else {
		std::cerr << "RichParameter type not supported for QTableWidget creation.\n";
		assert(0);
		return nullptr;
	}
}

//Maybe a MeshDocument parameter is needed. See loadFrameContent definition

/*WARNING!*******************************************************/
//In defPar->defVal the hardwired value is memorized
//in curPar->defVal the one in the sys reg
/****************************************************************/

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

	RichParameterSet tmpParSet;
	tmppar = curPar->clone();
	tmpParSet.addParam(tmppar);
	frame.loadFrameContent(tmpParSet);
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
	doc.appendChild(tmppar->fillToXMLDocument(doc));
	QString docstring =  doc.toString();
	qDebug("Writing into Settings param with name %s and content ****%s****", qUtf8Printable(tmppar->name()), qUtf8Printable(docstring));
	QSettings setting;
	setting.setValue(tmppar->name(),QVariant(docstring));
	curPar->value().set(tmppar->value());
}

void SettingDialog::apply()
{
	assert(frame.stdfieldwidgets.size() == 1);
	frame.stdfieldwidgets.at(0)->collectWidgetValue();
	curPar->value().set(tmppar->value());
	emit applySettingSignal();
}

void SettingDialog::reset()
{
	qDebug("resetting the value of param %s to the hardwired default", qUtf8Printable(curPar->name()));
	tmppar->value().set(defPar->value());
	assert(frame.stdfieldwidgets.size() == 1);
	frame.stdfieldwidgets.at(0)->setWidgetValue(tmppar->value());
	apply();
}

void SettingDialog::load()
{
	assert(frame.stdfieldwidgets.size() == 1);
	frame.stdfieldwidgets.at(0)->setWidgetValue(curPar->value());
}

SettingDialog::~SettingDialog()
{
}
