/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2009                                          \/)\/    *
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

#include "edit_align.h"
#include <QGLWidget>
#include "AlignPairDialog.h"
#include <QLabel>
#include <QDialogButtonBox>
#include <QDesktopWidget>

AlignPairDialog::AlignPairDialog (GLArea* gla,QWidget * parent) : QDialog(parent)
{
    QVBoxLayout *layout = new QVBoxLayout();
	
    aa=new AlignPairWidget(gla,this);
	connect(this, SIGNAL(finished(int)), aa, SLOT(cleanDataOnClosing(int)));
	aa->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok  | QDialogButtonBox::Cancel);
	QHBoxLayout* checklayout = new QHBoxLayout();
	
	QCheckBox* fakecolor = new QCheckBox("use False Color",this);
	fakecolor->setChecked(true);
	checklayout->addWidget(fakecolor);
	setFakeColorFlag(fakecolor->isChecked());
	connect(fakecolor, SIGNAL(clicked(bool)), this, SLOT(setFakeColorFlag(bool)));


	QCheckBox* pointrend = new QCheckBox("use Point Rendering", this);
    pointrend->setChecked(false);
	checklayout->addWidget(pointrend);
	setPointRenderingFlag(pointrend->isChecked());
	connect(pointrend, SIGNAL(clicked(bool)), this, SLOT(setPointRenderingFlag(bool)));

	QCheckBox* allowScalingCB = new QCheckBox("Allow Scaling", this);
	allowScalingCB->setChecked(false);
	checklayout->addWidget(allowScalingCB);
	setScalingFlag(allowScalingCB->isChecked());
	connect(allowScalingCB, SIGNAL(clicked(bool)), this, SLOT(setScalingFlag(bool)));

    QLabel *helpLabel = new QLabel("Choose at least 4 matching pair of points on the two meshes. <br>Double Click over each mesh to add new points. Choose points in consistent order");
    helpLabel->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    layout->addWidget(helpLabel);
    layout->addWidget(aa);
	layout->addLayout(checklayout);
	layout->addWidget(buttonBox);


    setLayout(layout);
    adjustSize();
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    QRect rr= QApplication::desktop()->screenGeometry ( this );
    setMinimumSize(rr.width()*0.8,rr.width()*0.5);
}

void AlignPairDialog::setScalingFlag(bool checked)
{
	aa->allowscaling = checked;
	aa->update();
}

void AlignPairDialog::setPointRenderingFlag(bool checked)
{
	aa->usePointRendering = checked;
	aa->update();
}

void AlignPairDialog::setFakeColorFlag(bool checked)
{
	aa->isUsingVertexColor = !checked;
	aa->update();
}

