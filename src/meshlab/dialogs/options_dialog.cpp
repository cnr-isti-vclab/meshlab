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

#include "options_dialog.h"

#include "setting_dialog.h"

#include <QHeaderView>


using namespace vcg;

MeshLabOptionsDialog::MeshLabOptionsDialog(
		RichParameterList& curparset,
		const RichParameterList& defparset,
		QWidget * parent) :
	QDialog(parent),
	currentParameterList(curparset),
	defaultParameterList(defparset)
{
	setModal(false);
	closebut = new QPushButton("Close",this);
	QGridLayout* layout = new QGridLayout(parent);
	setLayout(layout);
	tw = new QTableWidget(currentParameterList.size(),2,this);
	updateSettings();
	int totlen = tw->columnWidth(0) + tw->columnWidth(1) + this->frameSize().width();
	setMinimumWidth(totlen);
	layout->addWidget(tw,0,0,1,5);
	layout->addWidget(closebut,1,4,1,1);
	connect(tw, SIGNAL(itemDoubleClicked(QTableWidgetItem* )), this, SLOT(openSubDialog(QTableWidgetItem*)));
	connect(closebut, SIGNAL(clicked()), this, SLOT(close()));
	this->setWindowTitle(tr("Global Parameters Window"));
}

MeshLabOptionsDialog::~MeshLabOptionsDialog()
{
}

/**
 * @brief This slot is executed when a setting is double clicked
 * @param itm
 */
void MeshLabOptionsDialog::openSubDialog(QTableWidgetItem* itm)
{
	int rprow = tw->row(itm);
	const RichParameter& curPar = currentParameterList.at(rprow);
	const RichParameter& defPar = defaultParameterList.getParameterByName(curPar.name());
	SettingDialog* setdial = new SettingDialog(curPar, defPar, this);
	connect(
				setdial, SIGNAL(applySettingSignal(const RichParameter&)),
				this,    SLOT(updateSingleSetting(const RichParameter&)));
	setdial->exec();
	delete setdial;
}

void MeshLabOptionsDialog::updateSettings()
{
	QStringList slst;
	slst.push_back("Variable Name");
	slst.push_back("Variable Value");
	tw->setHorizontalHeaderLabels(slst);
	tw->horizontalHeader()->setStretchLastSection(true);
	tw->setShowGrid(true);
	tw->verticalHeader()->hide();

	tw->setSelectionBehavior(QAbstractItemView::SelectRows);

	int ii = 0;
	for(const RichParameter& p : currentParameterList) {
		QTableWidgetItem* item = new QTableWidgetItem(p.name());
		item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsDragEnabled |Qt::ItemIsDropEnabled |Qt::ItemIsUserCheckable |Qt::ItemIsEnabled);

		tw->setItem(ii,0,item);

		QTableWidgetItem* twi = createQTableWidgetItemFromRichParameter(p);
		twi->setFlags(Qt::ItemIsSelectable | Qt::ItemIsDragEnabled |Qt::ItemIsDropEnabled |Qt::ItemIsUserCheckable |Qt::ItemIsEnabled);
		tw->setItem(ii++,1,twi);
	}
	tw->resizeColumnsToContents();
	tw->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
}

void MeshLabOptionsDialog::updateSingleSetting(const RichParameter& rp)
{
	assert(currentParameterList.getParameterByName(rp.name()).stringType() == rp.stringType());
	currentParameterList.setValue(rp.name(), rp.value());
	updateSettings();
	emit applyCustomSetting();
}

QTableWidgetItem* MeshLabOptionsDialog::createQTableWidgetItemFromRichParameter(const RichParameter& pd)
{
	if (pd.isOfType<RichAbsPerc>()){
		return new QTableWidgetItem(QString::number(pd.value().getFloat()));
	}
	else if (pd.isOfType<RichDynamicFloat>()){
		return new QTableWidgetItem(QString::number(pd.value().getFloat()));
	}
	else if (pd.isOfType<RichEnum>()){
		return new QTableWidgetItem(QString::number(pd.value().getInt()));
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
	return nullptr;
}
