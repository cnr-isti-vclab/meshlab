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

#include "line_edit_widget.h"

LineEditWidget::LineEditWidget(QWidget* p, const RichParameter& param, const Value& defaultValue) :
		RichParameterWidget(p, param, defaultValue)
{
	lned = new QLineEdit(this);

	connect(lned, SIGNAL(editingFinished()), this, SLOT(changeChecker()));
	connect(this, SIGNAL(lineEditChanged()), this, SLOT(setParameterChanged()));
	lned->setAlignment(Qt::AlignLeft);
	widgets.push_back(lned);
}

LineEditWidget::~LineEditWidget()
{
}

void LineEditWidget::addWidgetToGridLayout(QGridLayout* lay, const int r)
{
	if (lay != nullptr) {
		lay->addWidget(lned, r, 1);
	}
	RichParameterWidget::addWidgetToGridLayout(lay, r);
}

void LineEditWidget::changeChecker()
{
	if (lned->text() != this->lastVal) {
		this->lastVal = lned->text();
		if (!this->lastVal.isEmpty())
			emit lineEditChanged();
	}
}
