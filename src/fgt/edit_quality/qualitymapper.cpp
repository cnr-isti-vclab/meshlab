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
#include <vcg/complex/trimesh/stat.h>
#include <vcg/complex/trimesh/update/flag.h>
// #include "color_manifold.h"
#include "../../meshlabplugins/meshcolorize/curvature.h"		//<--contains Frange
#include "../../meshlabplugins/meshcolorize/equalizerDialog.h"	//for EqualizerSettings
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

void QualityMapperPlugin::StartEdit(QAction * /*mode*/, MeshModel &/*m*/, GLArea *gla )
{
//	gla->setCursor(QCursor(QPixmap(":/images/cur_info.png"),1,1));	



	if(_qualityMapperDialog==0)
	{
		//_qualityMapperDialog=new _qualityMapperDialog(gla->parentWidget()->parentWidget());
		_qualityMapperDialog = new QualityMapperDialog(gla->window());
// 		connect(_qualityMapperDialog->ui.icpParamButton,SIGNAL(clicked()),this,SLOT(alignParam()));
// 		connect(_qualityMapperDialog->ui.icpButton,SIGNAL(clicked()),this,SLOT(process()));
// 		connect(_qualityMapperDialog->ui.manualAlignButton,SIGNAL(clicked()),this,SLOT(glueManual()));
// 		connect(_qualityMapperDialog->ui.pointBasedAlignButton,SIGNAL(clicked()),this,SLOT(glueByPicking()));
// 		connect(_qualityMapperDialog->ui.glueHereButton,SIGNAL(clicked()),this,SLOT(glueHere()));
// 		connect(_qualityMapperDialog->ui.glueHereAllButton,SIGNAL(clicked()),this,SLOT(glueHereAll()));
// 		connect(_qualityMapperDialog->ui.falseColorCB, SIGNAL(clicked(bool)) , _gla->window(),  SLOT(updateGL() ) );

	}
// 	_qualityMapperDialog->edit=this;
// 	_qualityMapperDialog->setTree(& meshTree, meshTree.nodeList.front());
	_qualityMapperDialog->show();
}



/**** VECCHI METODI ****/

// this function is called to fill the parameter list 
// It is called only for filters that have a not empty list of parameters and 
// that do not use the auto generated dialog, but want a personalized dialog.
bool QualityMapperPlugin::getParameters(QAction *action, QWidget * parent, MeshModel &m, FilterParameterSet & par, MainWindowInterface *mw) 
{
//************************************************************************/
//* CONSERVARE QUESTO!!! CREA L'ISTOGRAMMA DELLA QUALITA' DA SOLO!!      */
//* m@l                                                                  */
//************************************************************************/
	Histogramf H;
	tri::Stat<CMeshO>::ComputePerVertexQualityHistogram(m.cm,H);

	Frange mmmq(tri::Stat<CMeshO>::ComputePerVertexQualityMinMax(m.cm));

	H.FileWrite("risultato.txt");

	//probabilmente questa roba EqualizerSettings non serve...
	EqualizerSettings eqSettings;
	eqSettings.meshMinQ = mmmq.minV;
	eqSettings.meshMaxQ = mmmq.maxV;

	eqSettings.histoMinQ = H.Percentile(eqSettings.percentile/100);
	eqSettings.histoMaxQ = H.Percentile(1.0f-eqSettings.percentile/100);

//	EqualizerDialog eqdialog(parent);
//	eqdialog.setValues(eqSettings);


 	Frange FinalRange;
// 	eqSettings=eqdialog.getValues();
// 	if (eqSettings.useManual) 
// 	FinalRange = Frange(eqSettings.manualMinQ,eqSettings.manualMaxQ);
// 	else
// 	{
// 		FinalRange.minV=H.Percentile(eqSettings.percentile/100.0);
// 		FinalRange.maxV=H.Percentile(1.0f-(eqSettings.percentile/100.0));
// 	}

	par.addFloat("RangeMin",FinalRange.minV);
	par.addFloat("RangeMax",FinalRange.maxV);      
	mw->executeFilter(action,par);

	return true;
}


void QualityMapperPlugin::initParameterSet(QAction *a,MeshModel &m, FilterParameterSet & par)
{
// 	switch(ID(a))
//   {
// 	case CP_TRIANGLE_QUALITY: {
// 			QStringList metrics;
// 			metrics.push_back("area/max side");
// 			metrics.push_back("inradius/circumradius");
// 			metrics.push_back("mean ratio");
// 			par.addEnum("Metric", 0, metrics, tr("Metric:"), tr("Choose a metric to compute triangle quality."));
// /// transformation matrix into a regular simplex"));
// 
// //			par.addEnum("Metric", 0, metrics, tr("Metric:"), tr("Choose a metric to compute triangle quality."));
// 			break;
// 		}
// 	default: assert(0);
// 	}
}




Q_EXPORT_PLUGIN(QualityMapperPlugin)
  
