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

RichParameterListDialog::RichParameterListDialog(QWidget *p, RichParameterList& curParSet, QString title, MeshDocument *_meshDocument)
	: QDialog(p), curParSet(curParSet){
		stdParFrame=NULL;
		meshDocument = _meshDocument;
		createFrame();
		if(!title.isEmpty())
			setWindowTitle(title);
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

	stdParFrame = new RichParameterListFrame(this);
	stdParFrame->loadFrameContent(curParSet, meshDocument);
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

	//set the minimum size so it will shrink down to the right size	after the help is toggled
	this->setMinimumSize(stdParFrame->sizeHint());
	this->showNormal();
	this->adjustSize();
}


void RichParameterListDialog::getAccept()
{
	stdParFrame->readValues(curParSet);
	accept();
}

RichParameterListDialog::~RichParameterListDialog()
{
	delete stdParFrame;
}


