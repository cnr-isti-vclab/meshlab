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

#include "io_file_widget.h"

#include <QFileDialog>

IOFileWidget::IOFileWidget(
	QWidget*             p,
	const RichParameter& param,
	const StringValue&   defaultValue) :
		RichParameterWidget(p, param, defaultValue)
{
	filename = new QLineEdit(this);
	filename->setText(param.value().getString());
	browse = new QPushButton(this);
	browse->setText("...");
	hlay = new QHBoxLayout();
	hlay->addWidget(filename, 2);
	hlay->addWidget(browse);
	widgets.push_back(filename);
	widgets.push_back(browse);

	connect(browse, SIGNAL(clicked()), this, SLOT(selectFile()));
	connect(this, SIGNAL(dialogParamChanged()), this, SLOT(setParameterChanged()));
}

IOFileWidget::~IOFileWidget()
{
	delete filename;
	delete browse;
}

void IOFileWidget::addWidgetToGridLayout(QGridLayout* lay, const int r)
{
	if (lay != nullptr) {
		lay->addLayout(hlay, r, 1, Qt::AlignTop);
	}
	RichParameterWidget::addWidgetToGridLayout(lay, r);
}

std::shared_ptr<Value> IOFileWidget::getWidgetValue() const
{
	return std::make_shared<StringValue>(filename->text());
}

void IOFileWidget::setWidgetValue(const Value& nv)
{
	QString fle = nv.getString();
	updateFileName(fle);
}

void IOFileWidget::updateFileName(const StringValue& file)
{
	filename->setText(file.getString());
}
