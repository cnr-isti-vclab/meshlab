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

#include "color_widget.h"

#include <QApplication>
#include <QClipboard>
#include <QColorDialog>
#include <QFileDialog>
#include <common/ml_document/mesh_document.h>

ColorWidget::ColorWidget(QWidget *p, const RichColor &newColor, const ColorValue &defaultValue) :
		RichParameterWidget(p, newColor, defaultValue), pickcol(defaultValue.getColor())
{
	colorLabel  = new QLabel(this);
	colorButton = new QPushButton(this);
	colorButton->setAutoFillBackground(true);
	colorButton->setFlat(true);
	colorButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	initWidgetValue();

	vlay = new QHBoxLayout();
	QFontMetrics met(colorLabel->font());
	QColor       black(Qt::black);
	QString      blackname = "(" + black.name() + ")";
	QSize        sz        = met.size(Qt::TextSingleLine, blackname);
	colorLabel->setMaximumWidth(sz.width());
	colorLabel->setMinimumWidth(sz.width());
	vlay->addWidget(colorLabel, 0, Qt::AlignRight);
	vlay->addWidget(colorButton);
	widgets.push_back(colorLabel);
	widgets.push_back(colorButton);

	connect(colorButton, SIGNAL(clicked()), this, SLOT(pickColor()));
	connect(this, SIGNAL(dialogParamChanged()), this, SLOT(setParameterChanged()));
}

ColorWidget::~ColorWidget()
{
	delete colorButton;
	delete colorLabel;
}

void ColorWidget::addWidgetToGridLayout(QGridLayout* lay, const int r)
{
	if (lay != nullptr) {
		lay->addLayout(vlay, r, 1);
	}
	RichParameterWidget::addWidgetToGridLayout(lay, r);
}

std::shared_ptr<Value> ColorWidget::getWidgetValue() const
{
	return std::make_shared<ColorValue>(pickcol);
}

void ColorWidget::resetWidgetValue()
{
	QColor cl = parameter->value().getColor();
	pickcol   = cl;
	updateColorInfo(cl);
}

void ColorWidget::setWidgetValue(const Value& nv)
{
	QColor cl = nv.getColor();
	pickcol   = cl;
	updateColorInfo(cl);
}

void ColorWidget::initWidgetValue()
{
	QColor cl = parameter->value().getColor();
	pickcol   = cl;
	updateColorInfo(cl);
}

void ColorWidget::updateColorInfo(const ColorValue& newColor)
{
	QColor col = newColor.getColor();
	colorLabel->setText("(" + col.name() + ")");
	QPalette palette(col);
	colorButton->setPalette(palette);
}

void ColorWidget::pickColor()
{
	pickcol = QColorDialog::getColor(
		pickcol,
		this->parentWidget(),
		"Pick a Color",
		QColorDialog::DontUseNativeDialog | QColorDialog::ShowAlphaChannel);
	if (pickcol.isValid()) {
		parameter->setValue(ColorValue(pickcol));
		updateColorInfo(ColorValue(pickcol));
	}
	emit dialogParamChanged();
}
