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
/****************************************************************************
History
Revision 1.0 2008/02/20 Alessandro Maione, Federico Bellucci
FIRST RELEASE

****************************************************************************/

#ifndef _QUALITY_MAPPER_PLUGIN_H_
#define _QUALITY_MAPPER_PLUGIN_H_

#include <QObject>
#include <QAction>
#include <QList>

#include <common/interfaces.h>
#include "qualitymapperdialog.h"

//This class defines the plugin interface
class QualityMapperPlugin : public QObject, public MeshEditInterface
{
    Q_OBJECT
    Q_INTERFACES(MeshEditInterface)

private:
	QualityMapperDialog *_qualityMapperDialog;

public:
    QualityMapperPlugin(void);
    ~QualityMapperPlugin(void){};

	static const QString Info();
    virtual bool StartEdit(MeshModel&, GLArea*);
    virtual void EndEdit(MeshModel&, GLArea*);
    virtual void Decorate(MeshModel&, GLArea*);
    virtual void mousePressEvent(QMouseEvent*, MeshModel&, GLArea*) {};
    virtual void mouseMoveEvent(QMouseEvent*, MeshModel&, GLArea*) {};
    virtual void mouseReleaseEvent(QMouseEvent *event, MeshModel&, GLArea*){};

	QPoint cur;
	bool haveToPick;
};

#endif
