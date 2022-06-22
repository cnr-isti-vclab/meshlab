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

#include "bool_widget.h"

BoolWidget::BoolWidget(QWidget* p, const RichBool& param, const BoolValue& defaultValue) :
		RichParameterWidget(p, param, defaultValue)
{
	cb = new QCheckBox("", this);
	cb->setToolTip(param.toolTip());
	cb->setChecked(param.value().getBool());
	widgets.push_back(cb);

	connect(cb, SIGNAL(stateChanged(int)), this, SLOT(setParameterChanged()));
	connect(descriptionLabel, SIGNAL(clicked()), cb, SLOT(toggle()));
}

BoolWidget::~BoolWidget()
{
}

void BoolWidget::addWidgetToGridLayout(QGridLayout* lay, const int r)
{
	if (lay != nullptr) {
		lay->addWidget(cb, r, 1);
	}
	RichParameterWidget::addWidgetToGridLayout(lay, r);
}

std::shared_ptr<Value> BoolWidget::getWidgetValue() const
{
	return std::make_shared<BoolValue>(cb->isChecked());
}

void BoolWidget::setWidgetValue(const Value& nv)
{
	cb->setChecked(nv.getBool());
}
