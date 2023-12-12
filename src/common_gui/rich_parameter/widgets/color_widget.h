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

#ifndef MESHLAB_COLOR_WIDGET_H
#define MESHLAB_COLOR_WIDGET_H

#include "rich_parameter_widget.h"

class ColorWidget : public RichParameterWidget
{
	Q_OBJECT
public:
	ColorWidget(QWidget* p, const RichColor& param, const ColorValue& defaultValue);
	~ColorWidget();

	void                   addWidgetToGridLayout(QGridLayout* lay, const int r);
	std::shared_ptr<Value> getWidgetValue() const;
	void                   setWidgetValue(const Value& nv);

private:
	void updateColorInfo(const ColorValue& newColor);
private slots:
	void pickColor();
signals:
	void dialogParamChanged();

protected:
	QHBoxLayout* vlay;

private:
	QPushButton* colorButton;
	QLabel*      colorLabel;
	QColor       pickcol;
};

#endif // MESHLAB_COLOR_WIDGET_H
