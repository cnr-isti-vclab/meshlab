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

#ifndef EditEpochPLUGIN_H
#define EditEpochPLUGIN_H

#include <QObject>
#include <common/interfaces.h>
#include "v3dImportDialog.h"

#include "epoch_reconstruction.h"
#include <vcg/complex/append.h>
#include <vcg/math/histogram.h>
#include <vcg/complex/algorithms/create/platonic.h>
#include <vcg/complex/algorithms/clustering.h>
#include <vcg/complex/algorithms/hole.h>
#include <wrap/io_trimesh/export_ply.h>
#include <meshlab/alnParser.h>


class EditEpochPlugin : public QObject, public MeshEditInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshEditInterface)
	
	
public:
    EditEpochPlugin();
        virtual ~EditEpochPlugin() {}

    static const QString Info();

    bool StartEdit(MeshDocument &/*m*/, GLArea * /*parent*/);
    void EndEdit(MeshModel &/*m*/, GLArea * /*parent*/);
    void mousePressEvent(QMouseEvent *, MeshModel &, GLArea * ) ;
    void mouseMoveEvent(QMouseEvent *, MeshModel &, GLArea * ) ;
    void mouseReleaseEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea * );
    bool isSingleMeshEdit() const {return false;}

    QFont qFont;
  	int mode;			

    v3dImportDialog *epochDialog; //the widget with the list of the meshes. 
		
	
	void toggleButtons();

		
		
		MeshDocument *md; 
		
    	GLArea * gla;

public:

	void depthFilter(FloatImage &depthImgf, FloatImage &countImgf, float depthJumpThr, 
														 bool dilation, int dilationNumPasses, int dilationWinsize,
														 bool erosion, int erosionNumPasses, int erosionWinsize);
	float ComputeDepthJumpThr(FloatImage &depthImgf, float percentile);
	bool CombineHandMadeMaskAndCount(CharImage &CountImg, QString maskName );
    void SmartSubSample(int factor, FloatImage &fli, CharImage &chi, FloatImage &subD, FloatImage &subQ, int minCount);
	void Laplacian2(FloatImage &depthImg, FloatImage &countImg, int minCount, CharImage &featureMask, float depthThr);
	void GenerateGradientSmoothingMask(int subsampleFactor, QImage &OriginalTexture, CharImage &mask);
	
	// this callback MUST be redefined because we are able to manage internally the layer change.
  virtual void LayerChanged(MeshDocument &/*md*/, MeshModel &/*oldMeshModel*/, GLArea */*parent*/)
	{ 
	// add code here to manage the external layer switching
	}

public slots:
	void ExportPly();	
		
		
		
signals:
	void suspendEditToggle();
	void resetTrackBall();

};

#endif
