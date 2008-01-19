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
Log: qualityMapper.h
Revision 1.0 2008/01/03 MAL
Beginning

****************************************************************************/

#ifndef _QUALITY_MAPPER_PLUGIN_H_
#define _QUALITY_MAPPER_PLUGIN_H_

#include <QObject>
#include <QAction>
//#include <QActionGroup>
//#include <QStringList>
#include <QList>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
#include <meshlab/glarea.h>

//#include <vcg/math/base.h>
//#include <vcg/space/triangle3.h>
#include <vcg/complex/trimesh/update/color.h> //<-- contains VertexQuality method

#include "qualitymapperdialog.h"

#include <vcg/math/histogram.h>
#include "transferfunction.h"


class QualityMapperPlugin : public QObject, public MeshEditInterface
{
    Q_OBJECT
    Q_INTERFACES(MeshEditInterface)

private:
	QualityMapperDialog *_qualityMapperDialog;
	QList <QAction *> actionList;
	Histogramf _histogram;
	TransferFunction _transfer_function;

	QualityMapperSettings _qmSettings;

public:
    QualityMapperPlugin();
    ~QualityMapperPlugin(){};

	const QString Info(QAction *);
	// Generic Info about the plug in version and author.
    const PluginInfo &Info();
	QList<QAction *> actions() const ;

	// Called when the user press the first time the button 
    void StartEdit			(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/);
	// Called when the user press the second time the button 
    void EndEdit			(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/);
    void Decorate			(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/);
    void mousePressEvent    (QAction *, QMouseEvent *, MeshModel &, GLArea * ) {};
    void mouseMoveEvent     (QAction *, QMouseEvent *, MeshModel &, GLArea * ) {};
    void mouseReleaseEvent  (QAction *, QMouseEvent *event, MeshModel &/*m*/, GLArea * );

	QPoint cur;
	bool haveToPick;

	void ComputePerVertexQualityHistogram( CMeshO & m, Histogramf &h, int bins);	
};

#endif
