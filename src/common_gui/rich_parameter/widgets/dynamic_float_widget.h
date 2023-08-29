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

#ifndef MESHLAB_DYNAMIC_FLOAT_WIDGET_H
#define MESHLAB_DYNAMIC_FLOAT_WIDGET_H

#include "rich_parameter_widget.h"

class DynamicFloatWidget : public RichParameterWidget
{
	Q_OBJECT

public:
	DynamicFloatWidget(QWidget* p, const RichDynamicFloat& param, const FloatValue& defaultValue);
	~DynamicFloatWidget();

	void                   addWidgetToGridLayout(QGridLayout* lay, const int r);
	std::shared_ptr<Value> getWidgetValue() const;
	void                   setWidgetValue(const Value& nv);

	float getValue();

public slots:
	void setValue(int newv);
	void setValueFromTextBox();
	void setValue(float newValue);

signals:
	// void valueChanged(int mask);
	void dialogParamChanged();

private:
	QLineEdit*   valueLE;
	QSlider*     valueSlider;
	float        minVal;
	float        maxVal;
	QHBoxLayout* hlay;

	float intToFloat(int val);
	int   floatToInt(float val);
};

#endif // MESHLAB_DYNAMIC_FLOAT_WIDGET_H
