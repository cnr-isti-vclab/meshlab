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

#include "richparameterlistdialog.h"

#include <QDialogButtonBox>

RichParameterListDialog::RichParameterListDialog(QWidget *p, RichParameterList& curParList, const QString& title) :
	QDialog(p),
	curParList(curParList),
	stdParFrame(nullptr)
{
	createFrame();
	if(!title.isEmpty())
		setWindowTitle(title);
}

RichParameterListDialog::~RichParameterListDialog()
{
	delete stdParFrame;
}

// update the values of the widgets with the values in the paramlist;
void RichParameterListDialog::resetValues()
{
	stdParFrame->resetValues();
}

void RichParameterListDialog::toggleHelp()
{
	stdParFrame->toggleHelp();
	this->updateGeometry();
	this->adjustSize();
}

void RichParameterListDialog::createFrame()
{
	QVBoxLayout *vboxLayout = new QVBoxLayout(this);
	setLayout(vboxLayout);

	stdParFrame = new RichParameterListFrame(curParList, this);
	layout()->addWidget(stdParFrame);

	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Help | QDialogButtonBox::Ok  | QDialogButtonBox::Cancel);
	//add the reset button so we can get its signals
	QPushButton *resetButton = buttonBox->addButton(QDialogButtonBox::Reset);
	layout()->addWidget(buttonBox);

	connect(buttonBox, SIGNAL(accepted()), this, SLOT(getAccept()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	connect(buttonBox, SIGNAL(helpRequested()), this, SLOT(toggleHelp()));
	connect(resetButton, SIGNAL(clicked()), this, SLOT(resetValues()));

	setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
}

void RichParameterListDialog::addVerticalSpacer()
{
	QGridLayout* lay = (QGridLayout*)stdParFrame->layout();
	QSpacerItem *spacer = new QSpacerItem(0, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	lay->addItem(spacer, lay->rowCount(), 0);
	lay->setRowMinimumHeight(lay->rowCount(), 10);
}

void RichParameterListDialog::addCheckBox(const QString& name, bool checked)
{
	QCheckBox* cb = new QCheckBox(stdParFrame);
	cb->setText(name);
	cb->setChecked(checked);
	QGridLayout* lay = (QGridLayout*)stdParFrame->layout();
	lay->addWidget(cb,lay->rowCount(), 0);
	additionalCheckBoxes[name] = cb;
}

bool RichParameterListDialog::isCheckBoxChecked(const QString& name)
{
	return additionalCheckBoxes.at(name)->isChecked();
}


void RichParameterListDialog::getAccept()
{
	stdParFrame->writeValuesOnParameterList(curParList);
	accept();
}


