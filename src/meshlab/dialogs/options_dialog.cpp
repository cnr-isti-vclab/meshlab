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
	closebut = new QPushButton("Close", this);
	QGridLayout* layout = new QGridLayout(parent);
	setLayout(layout);
	
		   // Add to the window a filter/search widget with a lens icon
	searchWidget = new QLineEdit(this);
	QHBoxLayout *searchLayout = new QHBoxLayout;
	QLabel *lensIcon = new QLabel(this);
	lensIcon->setText(QString::fromUtf8("\U0001F50D")); // Unicode character for the lens icon
	searchLayout->addWidget(lensIcon);
	searchLayout->addWidget(searchWidget);
	
	tw = new QTableWidget(currentParameterList.size(), 3, this);
	tw->setColumnHidden(2,true);	
	updateSettings();
	int totlen = tw->columnWidth(0) + tw->columnWidth(1) + this->frameSize().width();
	setMinimumWidth(totlen);
	
	layout->addLayout(searchLayout, 0, 0, 1, 5);
	layout->addWidget(tw, 1, 0, 1, 5);
	layout->addWidget(closebut, 2, 4, 1, 1);
	
	connect(tw, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(openSubDialog(QTableWidgetItem*)));
	connect(closebut, SIGNAL(clicked()), this, SLOT(close()));
	connect(searchWidget, &QLineEdit::textChanged, this, &MeshLabOptionsDialog::onSearchTextChanged);
	
		   // Set focus to the search widget when the dialog opens
	searchWidget->setFocus();
	
	this->setWindowTitle(tr("Global Parameters Window"));
}
MeshLabOptionsDialog::~MeshLabOptionsDialog()
{
}


void MeshLabOptionsDialog::onSearchTextChanged(const QString &text) {
	if (text.length() > 2) {
		filterTable(text);
	} else {
		// Show all rows if the text is 2 characters or less
		for (int row = 0; row < tw->rowCount(); ++row) {
			tw->setRowHidden(row, false);
		}
	}
}
void MeshLabOptionsDialog::filterTable(const QString &text) {
	for (int row = 0; row < tw->rowCount(); ++row) {
		bool match = false;
		for (int col = 0; col < tw->columnCount(); ++col) {
			QTableWidgetItem *item = tw->item(row, col);
			if (item && item->text().contains(text, Qt::CaseInsensitive)) {
				match = true;
				break;
			}
		}
		tw->setRowHidden(row, !match);
	}
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
	slst.push_back("Description");
	
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
		
		QTableWidgetItem* itemDesc = new QTableWidgetItem(p.toolTip());
		itemDesc->setFlags(Qt::ItemIsSelectable | Qt::ItemIsDragEnabled |Qt::ItemIsDropEnabled |Qt::ItemIsUserCheckable |Qt::ItemIsEnabled);
		tw->setItem(ii,2,itemDesc);
		

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
	if (pd.isOfType<RichBool>()){
		if (pd.value().getBool())
			return new QTableWidgetItem("true"/*,lst*/);
		else
			return new QTableWidgetItem("false"/*,lst*/);
	}
	else if (pd.isOfType<RichInt>()){
		return new QTableWidgetItem(QString::number(pd.value().getInt()));
	}
	else if (pd.isOfType<RichFloat>()){
		return new QTableWidgetItem(QString::number(pd.value().getFloat()));
	}
	else if (pd.isOfType<RichString>()){
		return new QTableWidgetItem(pd.value().getString());
	}
	else if (pd.isOfType<RichPercentage>()){
		return new QTableWidgetItem(QString::number(pd.value().getFloat()));
	}
	else if (pd.isOfType<RichDynamicFloat>()){
		return new QTableWidgetItem(QString::number(pd.value().getFloat()));
	}
	else if (pd.isOfType<RichEnum>()){
		return new QTableWidgetItem(QString::number(pd.value().getInt()));
	}
	else if (pd.isOfType<RichMatrix44>()){
		assert(0);
		return nullptr;
	}
	else if (pd.isOfType<RichPosition>() || pd.isOfType<RichDirection>()){
		vcg::Point3f pp = pd.value().getPoint3();
		QString pst = "P3(" + QString::number(pp.X()) + "," + QString::number(pp.Y()) + "," + QString::number(pp.Z()) + ")";
		return new QTableWidgetItem(pst);
	}
	else if (pd.isOfType<RichShot>()){
		assert(0); ///
		return new QTableWidgetItem(QString("TODO"));
	}
	else if (pd.isOfType<RichColor>()){
		QPixmap pix(10,10);
		pix.fill(pd.value().getColor());
		QIcon ic(pix);
		return new QTableWidgetItem(ic,"");
	}
	else if (pd.isOfType<RichFileOpen>()){
		new QTableWidgetItem(pd.value().getString());
	}
	else if (pd.isOfType<RichFileSave>()){
		assert(0);
		return nullptr;
	}
	else if (pd.isOfType<RichMesh>()){
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
