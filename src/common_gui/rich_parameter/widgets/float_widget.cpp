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

#include "float_widget.h"

#include <QApplication>
#include <QClipboard>
#include <QColorDialog>
#include <QFileDialog>
#include <common/ml_document/mesh_document.h>

FloatWidget::FloatWidget(QWidget* p, const RichFloat& rpar, const RichFloat& rdef) :
		LineEditWidget(p, rpar, rdef)
{
	lned->setText(QString::number(parameter->value().getFloat(), 'g', 3));
}

void FloatWidget::collectWidgetValue()
{
	parameter->setValue(FloatValue(lned->text().toFloat()));
}

void FloatWidget::resetWidgetValue()
{
	lned->setText(QString::number(parameter->value().getFloat(), 'g', 3));
}

void FloatWidget::setWidgetValue(const Value& nv)
{
	lned->setText(QString::number(nv.getFloat(), 'g', 3));
}
