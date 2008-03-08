/****************************************************************************
 * MeshLab                                                           o o     *
 * A versatile mesh processing toolbox                             o     o   *
 *                                                                _   O  _   *
 * Copyright(C) 2005                                                \/)\/    *
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

#include "paintbox.h"

Paintbox::Paintbox(QWidget * parent, Qt::WindowFlags flags) : QWidget(parent, flags)
{
	setupUi(this);
	
	setUndoStack(new QUndoStack(this));	
	
	active[0] = COLOR_PAINT; active[1] = MESH_SELECT;
	gradient_frame->setHidden(true);
	pick_frame->setHidden(true);
	smooth_frame->setHidden(true);

	QPoint p=parent->mapToGlobal(QPoint(0,0));
	setGeometry(p.x()+parent->width()-width(),p.y(),width(), 100);
	
}

void Paintbox::setUndoStack(QUndoStack * qus)
{
	stack = qus;
	
	QIcon undo = undo_button->icon();
	undo_button->setDefaultAction(stack->createUndoAction(undo_button));
	undo_button->defaultAction()->setIcon(undo);
	mesh_undo_button->setDefaultAction(stack->createUndoAction(mesh_undo_button));
	mesh_undo_button->defaultAction()->setIcon(undo);
	
	QIcon redo = redo_button->icon();
	redo_button->setDefaultAction(stack->createRedoAction(redo_button));
	redo_button->defaultAction()->setIcon(redo);
	mesh_redo_button->setDefaultAction(stack->createRedoAction(mesh_redo_button));
	mesh_redo_button->defaultAction()->setIcon(redo);
}

void Paintbox::on_default_colors_clicked()
{
	foreground_frame->setColor(Qt::black);
	background_frame->setColor(Qt::white);
}

void Paintbox::on_switch_colors_clicked()
{
	QColor temp = foreground_frame->getColor();
	foreground_frame->setColor(background_frame->getColor()); 
	background_frame->setColor(temp);
}

void Paintbox::setForegroundColor(QColor & c)
{
	foreground_frame->setColor(c);
}

void Paintbox::setBackgroundColor(QColor & c)
{
	background_frame->setColor(c);
}
