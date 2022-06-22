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

#include "../rich_parameter_list_frame.h"

RichParameterWidget::RichParameterWidget(
	QWidget*             p,
	const RichParameter& param,
	const Value&         defaultValue) :
		QWidget(p), defaultValue(defaultValue.clone()), visible(true), helpVisible(false)
{
	descriptionLabel = new ClickableLabel(param.fieldDescription(), this);
	descriptionLabel->setToolTip(param.toolTip());
	descriptionLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

	helpLabel = new QLabel("<small>" + param.toolTip() + "</small>", this);
	helpLabel->setTextFormat(Qt::RichText);
	helpLabel->setWordWrap(true);
	helpLabel->setVisible(false);
	helpLabel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
	helpLabel->setMinimumWidth(250);
}

RichParameterWidget::~RichParameterWidget()
{
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

void RichParameterWidget::resetWidgetToDefaultValue()
{
	setWidgetValue(*defaultValue);
	parameterValueChanged = false;
}

void RichParameterWidget::setHelpVisible(bool b)
{
	helpVisible = b;
	helpLabel->setVisible(visible && helpVisible);
}

bool RichParameterWidget::hasBeenChanged() const
{
	return parameterValueChanged;
}

void RichParameterWidget::setParameterChanged()
{
	parameterValueChanged = true;
	emit parameterChanged();
}
