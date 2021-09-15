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
#ifndef VERTICAL_SCROLL_AREA_H
#define VERTICAL_SCROLL_AREA_H

#include <QScrollArea>

/**
 * @brief The VerticalScrollArea class
 *
 * This class is necessary because Qt does not provide a pure Vertical Scroll Area,
 * that behaves as a normal qwidget horizontally.
 *
 * @see https://forum.qt.io/topic/13374/solved-qscrollarea-vertical-scroll-only
 */
class VerticalScrollArea : public QScrollArea
{
	Q_OBJECT
public:
	explicit VerticalScrollArea(QWidget* parent = 0);

	virtual bool eventFilter(QObject* o, QEvent* e);

private:
	QWidget* m_scrollAreaWidgetContents;
};

#endif // VERTICAL_SCROLL_AREA_H
