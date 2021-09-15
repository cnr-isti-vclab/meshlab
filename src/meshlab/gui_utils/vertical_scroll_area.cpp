/****************************************************************************
 * VCGLib                                                            o o     *
 * Visual and Computer Graphics Library                            o     o   *
 *                                                                _   O  _   *
 * Copyright(C) 2004-2021                                           \/)\/    *
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
#include "vertical_scroll_area.h"

#include <QEvent>
#include <QScrollBar>
#include <QVBoxLayout>

#include "vertical_scroll_area.h"

VerticalScrollArea::VerticalScrollArea(QWidget* parent) : QScrollArea(parent)
{
	setWidgetResizable(true);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}

bool VerticalScrollArea::eventFilter(QObject* o, QEvent* e)
{
	// This works because QScrollArea::setWidget installs an eventFilter on the widget
	if (o && o == widget() && e->type() == QEvent::Resize)
		setMinimumWidth(widget()->minimumSizeHint().width() + verticalScrollBar()->width());

	return QScrollArea::eventFilter(o, e);
}
