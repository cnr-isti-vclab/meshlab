/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2009                                          \/)\/    *
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

#include <colorpicker.h>

void ColorPicker::setColor(QColor c) {
	color = c;
	update();
}

void ColorPicker::paintEvent(QPaintEvent * event ) {
	QPainter painter(this);
	painter.fillRect(QRect(0,0,width(),height()),color);
	int col=color.red()+color.green()+color.blue();
	if (col<150) painter.setPen(Qt::white);
	else painter.setPen(Qt::black);
	painter.drawRect(QRect(1,1,width()-3,height()-3));
}

void ColorPicker::mousePressEvent ( QMouseEvent * event ) {
	QColor temp = QColorDialog::getColor(color);
	if (temp.isValid()) color = temp;
	update();
}
