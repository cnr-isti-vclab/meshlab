/****************************************************************************
* VCGLib                                                            o o     *
* Visual and Computer Graphics Library                            o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004                                                \/)\/    *
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

#include <QtGui>
#include <QString>
#include <math.h>
#include <stdlib.h>
#include <meshlab/glarea.h>
#include "edit_hole.h"
#include "fgtHole.h"
#include "holeListModel.h"
#include <wrap/gl/pick.h>

using namespace vcg;


EditHolePlugin::EditHolePlugin() {
	dialogFiller = 0;
	holesModel = 0;
	
	pickedFace = 0;
	hasPick = false;
}

EditHolePlugin::~EditHolePlugin()
{
	if ( dialogFiller !=0 )
	{
		delete dialogFiller;
		delete holesModel;
		delete holeSorter;
		dialogFiller = 0;
		holesModel = 0;
		holeSorter = 0;
		gla = 0;
		mesh = 0;
	}
}

const QString EditHolePlugin::Info() 
{
	return tr("Allow fill one or more hole into place");
}

void EditHolePlugin::mouseReleaseEvent(QMouseEvent * /*e*/, MeshModel &/*m*/, GLArea * /*gla*/)
{
}

void EditHolePlugin::mousePressEvent(QMouseEvent *e, MeshModel &/*m*/, GLArea * /*gla*/)
{
	if ( (e->button()==Qt::LeftButton) )
	{
		cur.setX(e->x());
		cur.setY(e->y());
		hasPick = true;
	}
}

void EditHolePlugin::mouseMoveEvent(QMouseEvent * /*e*/, MeshModel &/*m*/, GLArea * /*gla*/)
{
}

bool EditHolePlugin::StartEdit(MeshModel &m, GLArea *gla )
{
	m.updateDataMask(MeshModel::MM_FACEFACETOPO);
  if ( tri::Clean<CMeshO>::CountNonManifoldEdgeFF(m.cm) >0)
	{
		QMessageBox::critical(0, tr("Manifoldness Failure"), QString("Hole's managing requires manifoldness."));
		return false; // can't continue, mesh can't be processed
	}

	// necessario per evitare di avere 2 istanze del filtro se si cambia mesh
	// senza chidere il filtro
	if(dialogFiller != 0)
		//EndEdit(m, gla);
		return false;

	// if plugin restart with another mesh, recomputing of hole is forced
	if(mesh != &m)
	{
		this->mesh = &m;
		this->gla = gla;

		mesh->clearDataMask(MeshModel::MM_FACEMARK);
		mesh->updateDataMask(MeshModel::MM_FACEMARK);
	}

	bridgeOptSldVal = 50;
	dialogFiller=new FillerDialog(gla->window());
	dialogFiller->show();
	dialogFiller->setAllowedAreas(Qt::NoDockWidgetArea);

	connect(dialogFiller->ui.operationTab, SIGNAL(currentChanged(int)), this, SLOT(skipTab(int)) );
	connect(dialogFiller->ui.fillButton, SIGNAL(clicked()), this,SLOT(fill()));
	connect(dialogFiller->ui.acceptFillBtn, SIGNAL(clicked()), this, SLOT(acceptFill()) );
	connect(dialogFiller->ui.cancelFillBtn, SIGNAL(clicked()), this, SLOT(cancelFill()) );

	connect(dialogFiller->ui.manualBridgeBtn, SIGNAL(clicked()), this, SLOT(manualBridge()) );
	connect(dialogFiller->ui.autoBridgeBtn, SIGNAL(clicked()), this, SLOT(autoBridge()) );
	connect(dialogFiller->ui.nmHoleClosureBtn, SIGNAL(clicked()), this, SLOT(closeNMHoles()) );
	connect(dialogFiller->ui.acceptBridgeBtn, SIGNAL(clicked()), this, SLOT(acceptBridges()) );
	connect(dialogFiller->ui.clearBridgeBtn, SIGNAL(clicked()), this, SLOT(clearBridge()) );
	connect(dialogFiller->ui.selfHoleChkB, SIGNAL(stateChanged(int)), this, SLOT(chekSingleBridgeOpt()) );
	connect(dialogFiller->ui.diedralWeightSld, SIGNAL(valueChanged(int)), this, SLOT(updateDWeight(int)));
	connect(dialogFiller->ui.bridgeParamSld, SIGNAL(valueChanged(int)), this, SLOT(updateBridgeSldValue(int)));
	connect(dialogFiller, SIGNAL(SGN_Closing()),gla,SLOT(endEdit()) );
	connect(dialogFiller->ui.holeTree->header(), SIGNAL(sectionCountChanged(int, int)), this, SLOT(resizeViewColumn()) );
	if(holesModel != 0)
	{
		delete holeSorter;
		delete holesModel;
	}
	holesModel = new HoleListModel(&m);

	
	holesModel->holesManager.autoBridgeCB = new EditHoleAutoBridgingCB(dialogFiller->ui.infoLbl, 800);
	connect(holesModel, SIGNAL(SGN_Closing()),gla,SLOT(endEdit()) );
	connect(holesModel, SIGNAL(SGN_needUpdateGLA()), this, SLOT(upGlA()) );
	connect(holesModel, SIGNAL(SGN_ExistBridge(bool)), dialogFiller, SLOT(SLOT_ExistBridge(bool)) );
	
	holeSorter = new HoleSorterFilter();
	holeSorter->setSourceModel(holesModel);
	dialogFiller->ui.holeTree->setModel( holeSorter );

	if(holesModel->holesManager.holes.size()==0)
	{
		QMessageBox::information(0, tr("No holes"), QString("Mesh have no hole to edit."));
		//EndEdit(m, gla);
		return false;
	}
	else
	{
		Decorate(m, gla);
    //Decorate(m, gla);
		upGlA();
	}
  return true;
}

void EditHolePlugin::Decorate(MeshModel &m, GLArea * gla)
{
	if(holesModel==0)
		return;

	glPushMatrix();
	glMultMatrix(mesh->cm.Tr);
	glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LINE_BIT | GL_DEPTH_BUFFER_BIT);

	holesModel->drawCompenetratingFaces();
	holesModel->drawHoles();

	if(hasPick)
	{
		hasPick = false;
		pickedFace =0;
		int inverseY = gla->curSiz.height() - cur.y();
		GLPickTri<CMeshO>::PickNearestFace(cur.x(), inverseY, m.cm, pickedFace, 4, 4);
		
		if( pickedFace != 0 )
		{
			bool oldAbutmentPresence;
			switch(holesModel->getState())
			{
			case HoleListModel::Selection:
				if(holesModel->holesManager.IsHoleBorderFace(pickedFace))
					holesModel->toggleSelectionHoleFromFace(pickedFace);
				break;
			case HoleListModel::Filled:
				holesModel->toggleAcceptanceHole(pickedFace);
				break;
			case HoleListModel::ManualBridging:
				oldAbutmentPresence = holesModel->PickedAbutment();
				gla->meshDoc.busy=true;
				holesModel->addBridgeFace(pickedFace, cur.x(), inverseY);
				gla->meshDoc.busy=false;
				if(holesModel->PickedAbutment() != oldAbutmentPresence)
				{
					if(oldAbutmentPresence == true)
						gla->setCursor(QCursor(QPixmap(":/images/bridgeCursor.png", "PNG"), 1, 1));
					else
						gla->setCursor(QCursor(QPixmap(":/images/bridgeCursor1.png", "PNG"), 1, 1));
				}
				break;
			}
		}
	}

	glPopAttrib();
	glPopMatrix();
 }

 void EditHolePlugin::EndEdit(MeshModel &/*m*/, GLArea *gla ){
	 if(holesModel == 0)	// means editing is not started
		 return;

	 if(holesModel->getState() == HoleListModel::Filled)
	 	 holesModel->acceptFilling(false);
	 if(holesModel->holesManager.bridges.size()>0)
		holesModel->removeBridges();

	 if ( dialogFiller!=0) {
		delete dialogFiller;
		delete holesModel;
		delete holeSorter;
		dialogFiller = 0;
		holesModel = 0;
		holeSorter = 0;
		gla = 0;
		mesh = 0;
	 }
 }


void EditHolePlugin::upGlA()
{
	gla->update();
	setInfoLabel();
}

void  EditHolePlugin::setInfoLabel()
{
  int ns =  holesModel->holesManager.SelectionCount();
  int nh = holesModel->holesManager.HolesCount();
  QString infoStr;
  if(holesModel->getState() == HoleListModel::Filled)
  {
    int na = holesModel->holesManager.AcceptedCount();
    infoStr = QString("Filled: %1/%2; Accepted: %3").arg(ns).arg(nh).arg(na);
  }
  else
    infoStr = QString("Selected: %1/%2").arg(ns).arg(nh);
  dialogFiller->ui.infoLbl->setText(infoStr);
};


void EditHolePlugin::resizeViewColumn()
{
	dialogFiller->ui.holeTree->header()->resizeSections(QHeaderView::ResizeToContents);
}

void EditHolePlugin::updateDWeight(int val)
{
	vcg::tri::MinimumWeightEar<CMeshO>::DiedralWeight() = (float)val / 50;
}

void EditHolePlugin::fill()
{
  gla->meshDoc.busy=true;
	if(holesModel->getState() == HoleListModel::Filled)
		holesModel->acceptFilling(false);

	if( dialogFiller->ui.trivialRBtn->isChecked())
		holesModel->fill( FgtHole<CMeshO>::Trivial);
	else if( dialogFiller->ui.minWRBtn->isChecked())
		holesModel->fill( FgtHole<CMeshO>::MinimumWeight);
	else
		holesModel->fill( FgtHole<CMeshO>::SelfIntersection);
  gla->meshDoc.busy=false;
	upGlA();
}

void EditHolePlugin::acceptFill()
{
	if(holesModel->getState() == HoleListModel::Filled)
	{
	  gla->meshDoc.busy=true;
		holesModel->acceptFilling(true);
		gla->meshDoc.busy=false;
		gla->setWindowModified(true);
	}
}

void EditHolePlugin::cancelFill()
{
	gla->meshDoc.busy=true;
	if(holesModel->getState() == HoleListModel::Filled)
		holesModel->acceptFilling(false);
	gla->meshDoc.busy=false;
}

void EditHolePlugin::updateBridgeSldValue(int val)
{
	bridgeOptSldVal = val;
}

void EditHolePlugin::manualBridge()
{
	if(holesModel->getState() != HoleListModel::ManualBridging)
	{
		holesModel->setStartBridging();
		dialogFiller->clickStartBridging();
		gla->setCursor(QCursor(QPixmap(":/images/bridgeCursor.png", "PNG"), 1, 1));
	}
	else
	{
		holesModel->setEndBridging();
		dialogFiller->clickEndBridging();
		gla->setCursor(QCursor());
	}
	gla->update();
}

void EditHolePlugin::autoBridge()
{
	gla->meshDoc.busy=true;
	bool singleHole = dialogFiller->ui.selfHoleChkB->isChecked();
	holesModel->autoBridge(singleHole, bridgeOptSldVal*0.0017);
	gla->meshDoc.busy=false;
	upGlA();
}

void EditHolePlugin::closeNMHoles()
{
	gla->meshDoc.busy=true;
	holesModel->closeNonManifolds();
	gla->meshDoc.busy=false;
	upGlA();
}


void EditHolePlugin::chekSingleBridgeOpt()
{
	dialogFiller->clickSingleHoleBridgeOpt();
}

void EditHolePlugin::acceptBridges()
{
	holesModel->acceptBridges();
	upGlA();
}

void EditHolePlugin::clearBridge()
{
	gla->meshDoc.busy=true;
	holesModel->removeBridges();
	gla->meshDoc.busy=false;
	upGlA();
}


void EditHolePlugin::skipTab(int index)
{
	if(holesModel->getState() == HoleListModel::Selection)
		return;

	if(index == 0)
	{
		holesModel->setEndBridging();
		dialogFiller->clickEndBridging();
		gla->setCursor(QCursor());
	}
	else
		cancelFill();
}
