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

#ifndef CADTEXTURINGCONTROL_H_
#define CADTEXTURINGCONTROL_H_

#include <vector>

#include <vcg/math/base.h>
#include <GL/glew.h>
#include <QHash>
#include <QUndoStack>
#include <QUndoGroup>
#include <QFileDialog>
#include <ui_CADtexturingControl.h>



/**
 * This class manages the user interface and is concerned
 * with emitting appropriate signals. It should not be 
 * concerned with the application logic in any way.
 */
class CADtexturingControl : public QDockWidget, private Ui::CADtexturingControl
{
	Q_OBJECT

private:
	
public:	
	CADtexturingControl(QWidget * parent = 0, Qt::WindowFlags flags = 0);


signals: 
	
	void saverenderingClicked();
	void renderedgesChanged(int);

public slots :
	void on_saverendering();
	void on_renderedgesChanged(int);
};


#endif  
