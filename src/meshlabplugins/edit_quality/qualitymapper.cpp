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

#include <QtGui>
#include <limits>
#include <meshlab/glarea.h>
#include "qualitymapper.h"
#include <vcg/space/triangle3.h> //for quality

using namespace vcg;

QualityMapperPlugin::QualityMapperPlugin()
{
	//initializing dialog pointer
	_qualityMapperDialog = 0;
}

const QString QualityMapperPlugin::Info() 
{
	return tr("Colorize mesh vertexes by Quality");
}

bool QualityMapperPlugin::StartEdit(MeshModel& m, GLArea *gla )
{
	if(!m.hasDataMask(MeshModel::MM_VERTQUALITY))
	{
			QMessageBox::warning(gla, tr("Quality Mapper"), tr("The model has no vertex quality"), QMessageBox::Ok); 
			return false;
	}
	if(_qualityMapperDialog==0)
		_qualityMapperDialog = new QualityMapperDialog(gla->window(), m, gla);

	//drawing histogram
	//bool ret = _qualityMapperDialog->initEqualizerHistogram();
	if ( !_qualityMapperDialog->initEqualizerHistogram() )
	{
		//EndEdit(m, gla);
		return false;
	}

	//drawing transferFunction
	_qualityMapperDialog->drawTransferFunction();

	//dialog ready to be displayed. Show it now!
	_qualityMapperDialog->show();

	connect(_qualityMapperDialog, SIGNAL(closingDialog()),gla,SLOT(endEdit()) );
	return true;
}

void QualityMapperPlugin::EndEdit(MeshModel &, GLArea * )
{
	//if a dialog exists, it's time to destroy it
	if ( _qualityMapperDialog )
	{
		//disconnecting...
		_qualityMapperDialog->disconnect();
		//...and deleting dialog
		delete _qualityMapperDialog;
		_qualityMapperDialog = 0;
	}
}

void QualityMapperPlugin::Decorate(MeshModel&, GLArea*)
{
// Draw a vertical bar 
// Enter in 2D screen Mode again
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-1,1,-1,1,-1,1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	
	const float barTop    = - 0.60f;
	const float barBottom =   0.60f;
	const float barLeft   = - 0.90f;
	const float barRight  = - 0.85f;
	const int steps = 100;
	const float delta = (barTop-barBottom)/steps;
	glColor4f(.3f,.3f,.3f,.3f);
	glBegin(GL_QUAD_STRIP);

	if ( _qualityMapperDialog == 0 )
		return;

	float maxQuality = _qualityMapperDialog->maxQuality();
	float minQuality = _qualityMapperDialog->minQuality();
	float brightness = _qualityMapperDialog->brightness();
	float relativeMidQuality = _qualityMapperDialog->relativeMidQuality();
	float step = (float)(maxQuality - minQuality)/(float)steps;
	float curQ;
	for(int i=0;i<steps;++i)
	{
		glVertex2f(barLeft,barBottom+delta*i); glVertex2f(barRight,barBottom+delta*i);
		curQ = float(i)*step + minQuality;
		// Color4b cc = _qualityMapperDialog->_transferFunction->getColorByQuality(float(i)/float(steps));
		Color4b cc = _qualityMapperDialog->_transferFunction->getColorByQuality(curQ, minQuality, maxQuality, relativeMidQuality, brightness);
		cc[3]=64;
		glColor(cc);
	}
	glVertex2f(barLeft,barTop); glVertex2f(barRight,barTop);
	glEnd();
		
	
	// Closing 2D
	glPopAttrib();
	glPopMatrix(); // restore modelview
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}
