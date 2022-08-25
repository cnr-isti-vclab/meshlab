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

#include "point3_widget.h"

Point3Widget::Point3Widget(
	QWidget*             p,
	const RichParameter& param,
	const Point3Value&   defaultValue,
	QWidget*             gla) :
		RichParameterWidget(p, param, defaultValue)
{
	assert(param.value().isPoint3());

	paramName = param.name();

	vlay = new QHBoxLayout();
	vlay->setSpacing(0);
	const Point3Value& p3v = param.value().getPoint3();
	for (unsigned int i = 0; i < 3; ++i) {
		coordSB[i]     = new QLineEdit(this);
		QFont baseFont = coordSB[i]->font();
		if (baseFont.pixelSize() != -1)
			baseFont.setPixelSize(baseFont.pixelSize() * 3 / 4);
		else
			baseFont.setPointSize(baseFont.pointSize() * 3 / 4);
		coordSB[i]->setFont(baseFont);
		coordSB[i]->setMinimumWidth(coordSB[i]->sizeHint().width() / 4);
		coordSB[i]->setValidator(new QDoubleValidator());
		coordSB[i]->setAlignment(Qt::AlignRight);
		coordSB[i]->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
		vlay->addWidget(coordSB[i]);
		widgets.push_back(coordSB[i]);
	}
	setValue(paramName, param.value().getPoint3());
	// connects must be made AFTER setValue
	for (unsigned int i = 0; i < 3 ; i++) {
		connect(coordSB[i], SIGNAL(textChanged(QString)), this, SLOT(setParameterChanged()));
	}
	// if we have a connection to the current glarea we can setup the additional
	// button for getting the current view direction.
	if (gla) {
		getPoint3Combo = new QComboBox(this);
		getPoint3Combo->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		vlay->addWidget(getPoint3Combo);
		widgets.push_back(getPoint3Combo);

		getPoint3Button = new QPushButton("Get", this);
		getPoint3Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

		vlay->addWidget(getPoint3Button);
		widgets.push_back(getPoint3Button);
	}
}

Point3Widget::~Point3Widget()
{
	this->disconnect();
}

void Point3Widget::addWidgetToGridLayout(QGridLayout* lay, const int r)
{
	if (lay != nullptr) {
		lay->addLayout(vlay, r, 1);
	}
	RichParameterWidget::addWidgetToGridLayout(lay, r);
}

std::shared_ptr<Value> Point3Widget::getWidgetValue() const
{
	return std::make_shared<Point3Value>(Point3m(
		coordSB[0]->text().toFloat(), coordSB[1]->text().toFloat(), coordSB[2]->text().toFloat()));
}

void Point3Widget::setWidgetValue(const Value& nv)
{
	for (unsigned int ii = 0; ii < 3; ++ii)
		coordSB[ii]->setText(QString::number(nv.getPoint3()[ii], 'g', 3));
}

vcg::Point3f Point3Widget::getValue()
{
	return vcg::Point3f(
		coordSB[0]->text().toFloat(), coordSB[1]->text().toFloat(), coordSB[2]->text().toFloat());
}

void Point3Widget::setValue(QString name, Point3m newVal)
{
	if (name == paramName) {
		for (int i = 0; i < 3; ++i)
			coordSB[i]->setText(QString::number(newVal[i], 'g', 4));
	}
}

void Point3Widget::setShotValue(QString name, Shotm newValShot)
{
	vcg::Point3f p = newValShot.GetViewPoint();
	setValue(name, p);
}
