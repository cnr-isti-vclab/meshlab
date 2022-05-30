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

#include "abs_perc_widget.h"

#include <QApplication>
#include <QClipboard>
#include <QColorDialog>
#include <QFileDialog>
#include <common/ml_document/mesh_document.h>

AbsPercWidget::AbsPercWidget(QWidget* p, const RichAbsPerc& rabs, const RichAbsPerc& rdef) :
		RichParameterWidget(p, rabs, rdef)

{
	m_min = rabs.min;
	m_max = rabs.max;

	descriptionLabel->setText(descriptionLabel->text() + " (abs and %)");

	absSB  = new QDoubleSpinBox(this);
	percSB = new QDoubleSpinBox(this);

	absSB->setMinimum(m_min - (m_max - m_min));
	absSB->setMaximum(m_max * 2);
	absSB->setAlignment(Qt::AlignRight);

	int decimals = 7 - ceil(log10(fabs(m_max - m_min)));
	absSB->setDecimals(decimals);
	absSB->setSingleStep((m_max - m_min) / 100.0);
	float initVal = parameter->value().getFloat();
	absSB->setValue(initVal);
	absSB->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

	percSB->setMinimum(-200);
	percSB->setMaximum(200);
	percSB->setAlignment(Qt::AlignRight);
	percSB->setSingleStep(0.5);
	percSB->setValue((100 * (initVal - m_min)) / (m_max - m_min));
	percSB->setDecimals(3);
	percSB->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	QLabel* absLab  = new QLabel("<i> <small> world unit</small></i>");
	QLabel* percLab = new QLabel(
		"<i> <small> perc on" + QString("(%1 .. %2)").arg(m_min).arg(m_max) + "</small></i>");

	vlay = new QGridLayout();
	vlay->addWidget(absLab, 0, 0, Qt::AlignHCenter);
	vlay->addWidget(percLab, 0, 1, Qt::AlignHCenter);

	vlay->addWidget(absSB, 1, 0, Qt::AlignTop);
	vlay->addWidget(percSB, 1, 1, Qt::AlignTop);

	widgets.push_back(absLab);
	widgets.push_back(percLab);
	widgets.push_back(absSB);
	widgets.push_back(percSB);

	connect(absSB, SIGNAL(valueChanged(double)), this, SLOT(on_absSB_valueChanged(double)));
	connect(percSB, SIGNAL(valueChanged(double)), this, SLOT(on_percSB_valueChanged(double)));
	connect(this, SIGNAL(dialogParamChanged()), p, SIGNAL(parameterChanged()));
}

AbsPercWidget::~AbsPercWidget()
{
	delete absSB;
	delete percSB;
}

void AbsPercWidget::on_absSB_valueChanged(double newv)
{
	disconnect(percSB, SIGNAL(valueChanged(double)), this, SLOT(on_percSB_valueChanged(double)));
	percSB->setValue((100 * (newv - m_min)) / (m_max - m_min));
	connect(percSB, SIGNAL(valueChanged(double)), this, SLOT(on_percSB_valueChanged(double)));
	emit dialogParamChanged();
}

void AbsPercWidget::on_percSB_valueChanged(double newv)
{
	disconnect(absSB, SIGNAL(valueChanged(double)), this, SLOT(on_absSB_valueChanged(double)));
	absSB->setValue((m_max - m_min) * 0.01 * newv + m_min);
	connect(absSB, SIGNAL(valueChanged(double)), this, SLOT(on_absSB_valueChanged(double)));
	emit dialogParamChanged();
}

void AbsPercWidget::setValue(float val, float minV, float maxV)
{
	absSB->setValue(val);
	m_min = minV;
	m_max = maxV;
}

void AbsPercWidget::collectWidgetValue()
{
	parameter->setValue(FloatValue(absSB->value()));
}

void AbsPercWidget::resetWidgetValue()
{
	RichAbsPerc* ap = reinterpret_cast<RichAbsPerc*>(parameter);
	setValue(parameter->value().getFloat(), ap->min, ap->max);
}

void AbsPercWidget::setWidgetValue(const Value& nv)
{
	RichAbsPerc* ap = reinterpret_cast<RichAbsPerc*>(parameter);
	setValue(nv.getFloat(), ap->min, ap->max);
}

void AbsPercWidget::addWidgetToGridLayout(QGridLayout* lay, const int r)
{
	if (lay != nullptr) {
		lay->addLayout(vlay, r, 1, Qt::AlignTop);
	}
	RichParameterWidget::addWidgetToGridLayout(lay, r);
}
