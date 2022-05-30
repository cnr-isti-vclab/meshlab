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

#include "combo_widget.h"

#include <QApplication>
#include <QClipboard>
#include <QColorDialog>
#include <QFileDialog>
#include <common/ml_document/mesh_document.h>

ComboWidget::ComboWidget(QWidget* p, const RichParameter& rpar, const RichParameter& rdef) :
		RichParameterWidget(p, rpar, rdef)
{
}

void ComboWidget::init(QWidget* p, int defaultEnum, QStringList values)
{
	enumCombo = new QComboBox(this);
	enumCombo->addItems(values);
	widgets.push_back(enumCombo);
	setIndex(defaultEnum);
	connect(enumCombo, SIGNAL(activated(int)), this, SIGNAL(dialogParamChanged()));
	connect(this, SIGNAL(dialogParamChanged()), this, SLOT(setParameterChanged()));
}

void ComboWidget::setIndex(int newEnum)
{
	enumCombo->setCurrentIndex(newEnum);
}

int ComboWidget::getIndex()
{
	return enumCombo->currentIndex();
}

ComboWidget::~ComboWidget()
{
	delete enumCombo;
}

void ComboWidget::addWidgetToGridLayout(QGridLayout* lay, const int r)
{
	if (lay != nullptr) {
		lay->addWidget(enumCombo, r, 1);
	}
	RichParameterWidget::addWidgetToGridLayout(lay, r);
}
