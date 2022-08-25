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

#include "qualitychecker.h"
#include <QFileDialog>

QualityChecker::QualityChecker(QWidget * parent, Qt::WindowFlags flags) : QDockWidget(parent, flags)
{
	setupUi(this);
	
	//QObject::connect(clone_source_view, SIGNAL(positionChanged(double, double)), this, SLOT(movePixmapDelta(double, double)));
	//QObject::connect(clone_source_view, SIGNAL(positionReset()), this, SLOT(resetPixmapDelta()));

	QObject::connect(this->createImageSpacePushButton, SIGNAL(clicked()), this, SLOT(on_createImageSpace()));
	QObject::connect(this->createImageSpacePushButton, SIGNAL(clicked()), this, SLOT(on_compare()));
	QObject::connect(this->useCurrentLayerPushButton, SIGNAL(clicked()), this, SLOT(on_layerChosen()));
	comparePushButton->setVisible(false);
}

void QualityChecker::on_createImageSpace(){
	emit(createImageSpaceClicked());
}

void QualityChecker::on_compare(){
	emit(compareClicked());
}

void QualityChecker::on_layerChosen(){
	emit(layerChosenChanged());
}
