/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2009                                          \/)\/    *
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

/*
 * MeshlabDockWidget.h
 *
 *  Created on: Aug 29, 2008
 *      Author: racoon
 */

#ifndef MESHLABEDITDOCKWIDGET_H_
#define MESHLABEDITDOCKWIDGET_H_

#include <QDockWidget>
#include "glarea.h"

class MeshlabEditDockWidget: public QDockWidget{
	Q_OBJECT
public:
	MeshlabEditDockWidget(GLArea *gla){
		if (gla !=0){
			setParent(gla->window());
			connect(this, SIGNAL(MEDW_Closing()),gla,SLOT(endEdit()) );
		}
	}
	
	void closeEvent ( QCloseEvent * event )
	{
		emit MEDW_Closing();
	}
	
	Q_SIGNALS:
		void MEDW_Closing();

};


#endif /* MESHLABEDITDOCKWIDGET_H_ */
