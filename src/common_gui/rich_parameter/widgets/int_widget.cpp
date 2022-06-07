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

#include "int_widget.h"

IntWidget::IntWidget(QWidget* p, const RichInt& param, const IntValue& defaultValue) :
		LineEditWidget(p, param, defaultValue)
{
	lned->setText(QString::number(param.value().getInt()));
}

IntWidget::~IntWidget()
{
}

std::shared_ptr<Value> IntWidget::getWidgetValue() const
{
	return std::make_shared<IntValue>(lned->text().toInt());
}

void IntWidget::setWidgetValue(const Value& nv)
{
	lned->setText(QString::number(nv.getInt()));
}
