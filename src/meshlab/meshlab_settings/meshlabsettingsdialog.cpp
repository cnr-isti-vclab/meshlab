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

#include "meshlabsettingsdialog.h"

#include "settingdialog.h"

#include <QHeaderView>


using namespace vcg;

MeshLabSettingsDialog::MeshLabSettingsDialog(RichParameterList& curparset, RichParameterList& defparset, QWidget * parent)
		:QDialog(parent),curParSet(curparset),defParSet(defparset)
{
	setModal(false);
	closebut = new QPushButton("Close",this);
	//QVBoxLayout* layout = new QVBoxLayout(parent);
	QGridLayout* layout = new QGridLayout(parent);
	setLayout(layout);
	tw = new QTableWidget(curParSet.size(),2,this);
	updateSettings();
	int totlen = tw->columnWidth(0) + tw->columnWidth(1) + this->frameSize().width();
	setMinimumWidth(totlen);
	layout->addWidget(tw,0,0,1,5);
	layout->addWidget(closebut,1,4,1,1);
	connect(tw,SIGNAL(itemDoubleClicked(QTableWidgetItem* )),this,SLOT(openSubDialog(QTableWidgetItem*)));
	connect(closebut,SIGNAL(clicked()),this,SLOT(close()));
	this->setWindowTitle(tr("Global Parameters Window"));
}

void MeshLabSettingsDialog::openSubDialog( QTableWidgetItem* itm )
{
	int rprow = tw->row(itm);
	RichParameter *defPar = defParSet.findParameter(vrp[rprow]->name());
	SettingDialog* setdial = new SettingDialog(vrp[rprow],defPar,this);
	connect(setdial,SIGNAL(applySettingSignal()),this,SIGNAL(applyCustomSetting()));
	connect(setdial,SIGNAL(applySettingSignal()),this,SLOT(updateSettings()));
	setdial->exec();
	delete setdial;
}

MeshLabSettingsDialog::~MeshLabSettingsDialog()
{
}

void MeshLabSettingsDialog::updateSettings()
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
	int ii = 0;
	for(RichParameter* p : curParSet)
	{
		QTableWidgetItem* item = new QTableWidgetItem(p->name());
		item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsDragEnabled |Qt::ItemIsDropEnabled |Qt::ItemIsUserCheckable |Qt::ItemIsEnabled);

		//if (maxlen[0] < item->text().size() * sz)
		//	maxlen[0] = item->text().size() * sz;
		//item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

		tw->setItem(ii,0,item);
		//butt = new QPushButton(richparset.paramList.at(ii)->name,this);

		QTableWidgetItem* twi = createQTableWidgetItemFromRichParameter(*p);
		twi->setFlags(Qt::ItemIsSelectable | Qt::ItemIsDragEnabled |Qt::ItemIsDropEnabled |Qt::ItemIsUserCheckable |Qt::ItemIsEnabled);
		tw->setItem(ii++,1,twi);
		//if (maxlen[1] < twi->text().size() * sz)
		//	maxlen[1] = twi->text().size() * sz;
		vrp.push_back(p);
	}
	tw->resizeColumnsToContents();
	tw->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	//tw->setColumnWidth(0,tw->horizontalHeader()->width());
	//tw->setColumnWidth(1,tw->horizontalHeader()->width());

	/*emit tw->horizontalHeader()->sectionAutoResize( 0,QHeaderView::ResizeToContents);
	emit tw->horizontalHeader()->sectionAutoResize( 1,QHeaderView::ResizeToContents);*/
}

QTableWidgetItem* MeshLabSettingsDialog::createQTableWidgetItemFromRichParameter(const RichParameter& pd)
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
