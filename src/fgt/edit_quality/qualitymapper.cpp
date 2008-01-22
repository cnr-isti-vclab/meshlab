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
Log: qualityMapper.cpp
Revision 1.0 2008/01/03 MAL
Beginning

****************************************************************************/
#include "qualitymapper.h"
#include <QtGui>
#include <limits>
#include <vcg/complex/trimesh/clean.h>

#include <vcg/complex/trimesh/update/flag.h>
// #include "color_manifold.h"

//#include "../../meshlabplugins/meshcolorize/equalizerDialog.h"	//for EqualizerSettings
// #include "smoothcolor.h"
#include <vcg/space/triangle3.h> //for quality

//using namespace std;
using namespace vcg;

QualityMapperPlugin::QualityMapperPlugin()
{
	/*
	isDragging=false;
	first=true;
	paintbox=0;
	pixels=0;
	pressed=0;*/

	actionList << new QAction(QIcon(":/images/qualitymapper.png"),"Quality Mapper. More info coming soon...", this);
	QAction *editAction;
	foreach(editAction, actionList)
		editAction->setCheckable(true);

	_qualityMapperDialog = 0;
}

QList<QAction *> QualityMapperPlugin::actions() const
{	return actionList;	}


const QString QualityMapperPlugin::Info(QAction *action) 
{
	if( action->text() != tr("Get Info") ) assert (0);

	return tr("Colorize mesh vertexes by Quality following some rules");
}

const PluginInfo &QualityMapperPlugin::Info() 
{
	static PluginInfo ai; 
	ai.Date=tr("Jan 2008");
	ai.Version = tr("1.0");
	ai.Author = ("Alessandro Maione, Federico Bellucci");
	return ai;
} 

void QualityMapperPlugin::mouseReleaseEvent  (QAction *,QMouseEvent * event, MeshModel &/*m*/, GLArea * gla)
{
	gla->update();
	cur=event->pos();
	haveToPick=true;
}

void QualityMapperPlugin::Decorate(QAction * /*ac*/, MeshModel &m, GLArea * gla)
{
}

void QualityMapperPlugin::StartEdit(QAction *mode, MeshModel &m, GLArea *gla )
{
	//	gla->setCursor(QCursor(QPixmap(":/images/cur_info.png"),1,1));	

	if(_qualityMapperDialog==0)
	{
		//_qualityMapperDialog=new _qualityMapperDialog(gla->parentWidget()->parentWidget());

		_qualityMapperDialog = new QualityMapperDialog(gla->window());
		
	}

	//building up histogram...
	Frange mmmq(tri::Stat<CMeshO>::ComputePerVertexQualityMinMax(m.cm));
	QualityMapperPlugin::ComputePerVertexQualityHistogram(m.cm, mmmq, _histogram, 100);
	//...histogram built

	//setting and applying settings to dialog (??) MAL
	_qmSettings.meshMinQ = mmmq.minV;
	_qmSettings.meshMaxQ = mmmq.maxV;
	_qmSettings.meshMidQ = (mmmq.minV+mmmq.maxV)/2;
	_qualityMapperDialog->setValues(_qmSettings);


	//drawing histogram in dialog(??) MAL
	_qualityMapperDialog->drawEqualizerHistogram(_histogram);

	//drawing transferFunction
	_qualityMapperDialog->drawTransferFunction( _transfer_function );

	// 	_qualityMapperDialog->edit=this;
	// 	_qualityMapperDialog->setTree(& meshTree, meshTree.nodeList.front());

	//dialog ready to be displayed. Show it now!
	_qualityMapperDialog->show();
}

void QualityMapperPlugin::EndEdit(QAction * , MeshModel &, GLArea * )
{
	if ( _qualityMapperDialog )
	{
		delete _qualityMapperDialog;
		_qualityMapperDialog = 0;
	}
}

void QualityMapperPlugin::ComputePerVertexQualityHistogram( CMeshO & m, Frange range, Histogramf &h, int bins=10000)    // V1.0
      {
        h.Clear();
		h.SetRange( range.minV, range.maxV, bins);
        CMeshO::VertexIterator vi;
        for(vi = m.vert.begin(); vi != m.vert.end(); ++vi)
          if(!(*vi).IsD()) h.Add((*vi).Q());
      }

Q_EXPORT_PLUGIN(QualityMapperPlugin)

