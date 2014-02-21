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

AlignPairDialog::AlignPairDialog (QWidget * parent) : QDialog(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    aa=new AlignPairWidget(this);
    aa->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok  | QDialogButtonBox::Cancel);
    allowScalingCB = new QCheckBox("Allow Scaling");
    allowScalingCB->setChecked(false);

    QLabel *helpLabel = new QLabel("Choose at least 4 matching pair of points on the two meshes. <br>Double Click over each mesh to add new points. Choose points in consistent order");
    helpLabel->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    layout->addWidget(helpLabel);
    layout->addWidget(aa);
    layout->addWidget(allowScalingCB);
    layout->addWidget(buttonBox);

    setLayout(layout);
    adjustSize();
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    QRect rr= QApplication::desktop()->screenGeometry ( this );
    setMinimumSize(rr.width()*0.8,rr.width()*0.5);
}


