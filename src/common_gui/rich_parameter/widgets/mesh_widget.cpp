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

#include "mesh_widget.h"

#include <QApplication>
#include <QClipboard>
#include <QColorDialog>
#include <QFileDialog>
#include <common/ml_document/mesh_document.h>

MeshWidget::MeshWidget(QWidget* p, const RichMesh& rpar, const RichMesh& rdef) :
		ComboWidget(p, rpar, rdef)
{
	md = ((RichMesh*) parameter)->meshdoc;

	QStringList meshNames;

	// make the default mesh Index be 0
	// defaultMeshIndex = -1;
	int          currentmeshindex = -1;
	unsigned int i                = 0;
	for (const MeshModel& mm : md->meshIterator()) {
		QString shortName = mm.label();
		meshNames.push_back(shortName);
		if (mm.id() == (unsigned int) parameter->value().getInt()) {
			currentmeshindex = i;
		}
		++i;
	}

	init(p, currentmeshindex, meshNames);
}

MeshWidget::~MeshWidget()
{
}

void MeshWidget::collectWidgetValue()
{
	auto it = md->meshBegin();
	std::advance(it, enumCombo->currentIndex());
	parameter->setValue(IntValue((*it).id()));
}

void MeshWidget::resetWidgetValue()
{
	int          meshindex = -1;
	unsigned int i         = 0;
	for (const MeshModel& mm : md->meshIterator()) {
		if (mm.id() == (unsigned int) parameter->value().getInt()) {
			meshindex = i;
		}
		++i;
	}
	enumCombo->setCurrentIndex(meshindex);
}

void MeshWidget::setWidgetValue(const Value& nv)
{
	int          meshindex = -1;
	unsigned int i         = 0;
	for (const MeshModel& mm : md->meshIterator()) {
		if (mm.id() == (unsigned int) nv.getInt()) {
			meshindex = i;
		}
		++i;
	}
	enumCombo->setCurrentIndex(meshindex);
}
