/*****************************************************************************
 * MeshLab                                                           o o     *
 * Visual and Computer Graphics Library                            o     o   *
 *                                                                _   O  _   *
 * Copyright(C) 2004-2022                                           \/)\/    *
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

#include "shot_widget.h"

#include <QFileDialog>

ShotWidget::ShotWidget(
	QWidget*         p,
	const RichShot&  param,
	const ShotValue& defaultValue,
	QWidget*         gla) :
		RichParameterWidget(p, param, defaultValue)
{
	paramName = param.name();

	hlay = new QHBoxLayout();

	this->setShotValue(paramName, param.value().getShot());
	// if we have a connection to the current glarea we can setup the additional
	// button for getting the current view direction.
	if (gla) {
		getShotButton = new QPushButton("Get shot", this);
		getShotButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
		hlay->addWidget(getShotButton);
		widgets.push_back(getShotButton);

		QStringList names;
		names << "Current Trackball";
		names << "Current Mesh";
		names << "Current Raster";
		names << "From File";

		getShotCombo = new QComboBox(this);
		getShotCombo->addItems(names);
		hlay->addWidget(getShotCombo);
		widgets.push_back(getShotCombo);
		connect(getShotCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(getShot()));
		connect(getShotButton, SIGNAL(clicked()), this, SLOT(getShot()));
		connect(
			gla, SIGNAL(transmitShot(QString, Shotm)), this, SLOT(setShotValue(QString, Shotm)));
		connect(this, SIGNAL(askViewerShot(QString)), gla, SLOT(sendViewerShot(QString)));
		connect(this, SIGNAL(askMeshShot(QString)), gla, SLOT(sendMeshShot(QString)));
		connect(this, SIGNAL(askRasterShot(QString)), gla, SLOT(sendRasterShot(QString)));
	}
}

ShotWidget::~ShotWidget()
{
}

void ShotWidget::addWidgetToGridLayout(QGridLayout* lay, const int r)
{
	if (lay != nullptr) {
		lay->addLayout(hlay, r, 1);
	}
	RichParameterWidget::addWidgetToGridLayout(lay, r);
}

std::shared_ptr<Value> ShotWidget::getWidgetValue() const
{
	return std::make_shared<ShotValue>(curShot);
}

void ShotWidget::setWidgetValue(const Value& nv)
{
	curShot = nv.getShot();
}

Shotm ShotWidget::getValue()
{
	return curShot;
}

void ShotWidget::getShot()
{
	int index = getShotCombo->currentIndex();
	switch (index) {
	case 0: emit askViewerShot(paramName); break;
	case 1: emit askMeshShot(paramName); break;
	case 2: emit askRasterShot(paramName); break;
	case 3: {
		QString filename = QFileDialog::getOpenFileName(
			this, tr("Load xml camera"), "./", tr("Xml Files (*.xml)"));
		QFile     qf(filename);
		QFileInfo qfInfo(filename);

		if (!qf.open(QIODevice::ReadOnly))
			return;

		QDomDocument doc("XmlDocument"); // It represents the XML document
		if (!doc.setContent(&qf))
			return;
		qf.close();

		QString type = doc.doctype().name();

	} break;
	default: assert(0);
	}
}

void ShotWidget::setShotValue(QString name, Shotm newVal)
{
	if (name == paramName) {
		curShot = newVal;
	}
}
