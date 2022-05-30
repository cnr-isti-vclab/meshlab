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

#ifndef MESHLAB_RICH_PARAMETER_WIDGET_H
#define MESHLAB_RICH_PARAMETER_WIDGET_H

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSlider>
#include <QWidget>

#include <memory>

#include <common/ml_document/cmesh.h>
#include <common/parameters/rich_parameter_list.h>

#include "../../gui_utils/clickable_label.h"

class RichParameterWidget : public QWidget
{
	Q_OBJECT
public:
	RichParameterWidget(QWidget* p, const RichParameter& rpar, const RichParameter& defaultValue);

	// this one is called by resetValue to reset the values inside the widgets.
	virtual void resetWidgetValue() = 0;
	// bring the values from the Qt widgets to the parameter (e.g. from the checkBox to the
	// parameter).
	//virtual std::shared_ptr<Value> getWidgetValue() const;
	virtual void collectWidgetValue()            = 0;
	virtual void setWidgetValue(const Value& nv) = 0;
	virtual ~RichParameterWidget();

	virtual void addWidgetToGridLayout(QGridLayout* lay, const int r) = 0;
	// called when the user press the 'default' button to reset the parameter values to its default.
	// It just set the parameter value and then it calls the specialized resetWidgetValue() to
	// update also the widget.
	void resetValue();
	void setValue(const Value& v);
	void setHelpVisible(bool b);
	void setVisible(bool b);
	// update the parameter with the current widget values and return it.
	const Value&         widgetValue();
	const RichParameter& richParameter() const;

	QString parameterName() const;

signals:
	void parameterChanged();

protected:
	ClickableLabel* descriptionLabel;
	QLabel* helpLabel;
	std::vector<QWidget*> widgets; //will contain all the other widgets of this widget

	RichParameter* parameter;
	RichParameter* defaultParameter;

	bool visible;
	bool helpVisible;
};

#endif // MESHLAB_RICH_PARAMETER_WIDGET_H
