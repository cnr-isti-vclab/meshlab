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

#ifndef MESHLAB_COMBO_WIDGET_H
#define MESHLAB_COMBO_WIDGET_H

#include "rich_parameter_widget.h"

class ComboWidget : public RichParameterWidget
{
	Q_OBJECT
protected:
	QComboBox* enumCombo;

public:
	ComboWidget(QWidget* p, const RichParameter& param, const Value& defaultValue);
	ComboWidget(
		QWidget*             p,
		const RichParameter& rpar,
		const Value&         defaultValue,
		const QStringList&   values,
		int                  defaultEnum);
	~ComboWidget();

	void                           addWidgetToGridLayout(QGridLayout* lay, const int r);
	virtual std::shared_ptr<Value> getWidgetValue() const          = 0;
	virtual void                   setWidgetValue(const Value& nv) = 0;

	int  getIndex();
	void setIndex(int newEnum);

signals:
	void dialogParamChanged();

protected:
	void init(int newEnum, QStringList values);
};

#endif // MESHLAB_COMBO_WIDGET_H
