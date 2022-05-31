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

#include "enum_widget.h"

#include <QApplication>
#include <QClipboard>
#include <QColorDialog>
#include <QFileDialog>
#include <common/ml_document/mesh_document.h>

EnumWidget::EnumWidget(QWidget* p, const RichEnum& rpar, const RichEnum& rdef) :
		ComboWidget(p, rpar, rdef)
{
	init(p, rpar.value().getInt(), rpar.enumvalues);
}

EnumWidget::~EnumWidget()
{
}

std::shared_ptr<Value> EnumWidget::getWidgetValue() const
{
	return std::make_shared<IntValue>(enumCombo->currentIndex());
}

void EnumWidget::resetWidgetValue()
{
	enumCombo->setCurrentIndex(parameter->value().getInt());
}

void EnumWidget::setWidgetValue(const Value& nv)
{
	enumCombo->setCurrentIndex(nv.getInt());
}
