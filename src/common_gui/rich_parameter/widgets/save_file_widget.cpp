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

#include "save_file_widget.h"

#include <QApplication>
#include <QClipboard>
#include <QColorDialog>
#include <QFileDialog>
#include <common/ml_document/mesh_document.h>

SaveFileWidget::SaveFileWidget(
	QWidget*            p,
	const RichSaveFile& rpar,
	const StringValue&  defaultValue) :
		IOFileWidget(p, rpar, defaultValue)
{
	filename->setText(parameter->value().getString());
}

SaveFileWidget::~SaveFileWidget()
{
}

void SaveFileWidget::selectFile()
{
	RichSaveFile* dec = reinterpret_cast<RichSaveFile*>(parameter);
	QString       fl =
		QFileDialog::getSaveFileName(this, tr("Save"), parameter->value().getString(), dec->ext);
	updateFileName(fl);
	StringValue fileName(fl);
	parameter->setValue(fileName);
	emit dialogParamChanged();
}
