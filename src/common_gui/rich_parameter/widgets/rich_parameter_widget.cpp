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

#include "rich_parameter_widget.h"

#include <QApplication>
#include <QClipboard>
#include <QColorDialog>
#include <QFileDialog>
#include <common/ml_document/mesh_document.h>

#include "../richparameterlistframe.h"

RichParameterWidget::RichParameterWidget(
	QWidget*             p,
	const RichParameter& rpar,
	const Value&         defaultValue) :
		QWidget(p),
		parameter(rpar.clone()),
		defaultValue(defaultValue.clone()),
		visible(true),
		helpVisible(false)
{
	if (parameter != nullptr) {
		descriptionLabel = new ClickableLabel(parameter->fieldDescription(), this);
		descriptionLabel->setToolTip(parameter->toolTip());
		descriptionLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

		helpLabel = new QLabel("<small>" + rpar.toolTip() + "</small>", this);
		helpLabel->setTextFormat(Qt::RichText);
		helpLabel->setWordWrap(true);
		helpLabel->setVisible(false);
		helpLabel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
		helpLabel->setMinimumWidth(250);
	}
}

RichParameterWidget::RichParameterWidget(
	QWidget*             p,
	const RichParameter& rpar,
	const RichParameter& defaultParam) :
		RichParameterWidget(p, rpar, defaultParam.value())
{
}

RichParameterWidget::~RichParameterWidget()
{
	delete parameter;
	delete defaultValue;
}

void RichParameterWidget::addWidgetToGridLayout(QGridLayout* lay, const int r)
{
	if (lay != NULL) {
		lay->addWidget(descriptionLabel, r, 0, 1, 1, Qt::AlignRight);
		lay->addWidget(helpLabel, r, 2);
	}
}

void RichParameterWidget::setVisible(bool b)
{
	visible = b;
	descriptionLabel->setVisible(b);
	for (QWidget* w : widgets)
		w->setVisible(b);
	if (b && helpVisible)
		helpLabel->setVisible(true);
	else if (!b)
		helpLabel->setVisible(false);
	QWidget::setVisible(b);
}

/**
 * @brief called when the user press the 'default' button to reset the parameter values to its default.
 * It just set the parameter value and then it calls the specialized resetWidgetValue() to
 * update also the widget.
 */
void RichParameterWidget::resetValue()
{
	parameter->setValue(*defaultValue);
	resetWidgetValue();
}

void RichParameterWidget::setValue(const Value& v)
{
	parameter->setValue(v);
	resetWidgetValue();
}

void RichParameterWidget::setHelpVisible(bool b)
{
	helpVisible = b;
	helpLabel->setVisible(visible && helpVisible);
}

void RichParameterWidget::setParameterChanged()
{
	parameterValueChanged = true;
	QObject* p = parent();
	RichParameterListFrame* f = dynamic_cast<RichParameterListFrame*>(p);
	if (f) {
		emit f->parameterChanged();
	}
}
