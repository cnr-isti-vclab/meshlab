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

#include "dynamic_float_widget.h"

#include <QApplication>
#include <QClipboard>
#include <QColorDialog>
#include <QFileDialog>
#include <common/ml_document/mesh_document.h>

DynamicFloatWidget::DynamicFloatWidget(QWidget *p, const RichDynamicFloat &rdf, const FloatValue &defaultValue) :
		RichParameterWidget(p, rdf, defaultValue)
{
	int numbdecimaldigit = 4;
	minVal               = rdf.min;
	maxVal               = rdf.max;
	valueLE              = new QLineEdit(this);
	valueLE->setAlignment(Qt::AlignRight);

	valueSlider = new QSlider(Qt::Horizontal, this);
	valueSlider->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	valueSlider->setMinimum(0);
	valueSlider->setMaximum(100);
	valueSlider->setValue(floatToInt(parameter->value().getFloat()));
	RichDynamicFloat* dfd = reinterpret_cast<RichDynamicFloat*>(parameter);
	QFontMetrics      fm(valueLE->font());
	QSize             sz = fm.size(Qt::TextSingleLine, QString::number(0));
	valueLE->setValidator(new QDoubleValidator(dfd->min, dfd->max, numbdecimaldigit, valueLE));
	valueLE->setText(QString::number(parameter->value().getFloat()));
	valueLE->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

	hlay = new QHBoxLayout();
	hlay->addWidget(valueLE);
	hlay->addWidget(valueSlider);
	widgets.push_back(valueLE);
	widgets.push_back(valueSlider);
	int maxlenghtplusdot = 8; // numbmaxvaluedigit + numbdecimaldigit + 1;
	valueLE->setMaxLength(maxlenghtplusdot);
	valueLE->setMaximumWidth(sz.width() * maxlenghtplusdot);

	connect(valueLE, SIGNAL(textChanged(const QString&)), this, SLOT(setValue()));
	connect(valueSlider, SIGNAL(valueChanged(int)), this, SLOT(setValue(int)));
	connect(this, SIGNAL(dialogParamChanged()), this, SLOT(setParameterChanged()));
}

DynamicFloatWidget::~DynamicFloatWidget()
{
}

void DynamicFloatWidget::addWidgetToGridLayout(QGridLayout* lay, const int r)
{
	if (lay != nullptr) {
		lay->addLayout(hlay, r, 1);
	}
	RichParameterWidget::addWidgetToGridLayout(lay, r);
}

std::shared_ptr<Value> DynamicFloatWidget::getWidgetValue() const
{
	return std::make_shared<FloatValue>(valueLE->text().toFloat());
}

void DynamicFloatWidget::resetWidgetValue()
{
	valueLE->setText(QString::number(parameter->value().getFloat()));
}

void DynamicFloatWidget::setWidgetValue(const Value& nv)
{
	valueLE->setText(QString::number(nv.getFloat()));
}

float DynamicFloatWidget::getValue()
{
	return float(valueLE->text().toDouble());
}

void DynamicFloatWidget::setValue(int newVal)
{
	if (floatToInt(float(valueLE->text().toDouble())) != newVal) {
		valueLE->setText(QString::number(intToFloat(newVal)));
	}
}

void DynamicFloatWidget::setValue()
{
	float newValLE = float(valueLE->text().toDouble());
	valueSlider->setValue(floatToInt(newValLE));
	emit dialogParamChanged();
}

void DynamicFloatWidget::setValue(float newVal)
{
	if (QString::number(intToFloat(newVal)) != valueLE->text())
		valueLE->setText(QString::number(intToFloat(newVal)));
}

float DynamicFloatWidget::intToFloat(int val)
{
	return minVal + float(val) / 100.0f * (maxVal - minVal);
}
int DynamicFloatWidget::floatToInt(float val)
{
	return int(100.0f * (val - minVal) / (maxVal - minVal));
}
