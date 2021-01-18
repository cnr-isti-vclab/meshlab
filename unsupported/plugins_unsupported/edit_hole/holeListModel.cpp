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

#include "holeListModel.h"
#include <QMessageBox>

using namespace vcg;


HoleListModel::HoleListModel(MeshModel *m, QObject *parent)
	: QAbstractItemModel(parent)
{
	state = HoleListModel::Selection;
	mesh = m;
	pickedAbutment.SetNull();
	mesh->updateDataMask(MeshModel::MM_FACEFACETOPO);
	//tri::UpdateTopology<CMeshO>::FaceFace(mesh->cm);
	holesManager.Init(&m->cm);
}


void HoleListModel::drawHoles() const
{
	glLineWidth(2.0f);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_GREATER);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	HoleVector::const_iterator it = holesManager.holes.begin();
	for( ; it != holesManager.holes.end(); ++it)
	{
		if( it->IsSelected())
		{
			if(it->IsAccepted())
				glColor(Color4b(Color4b::DarkGreen));
			else
				glColor(Color4b(Color4b::DarkRed));
		}
		else
			glColor(Color4b(Color4b::DarkBlue));
		it->Draw();
	}

	// draw the edge selected as a bridge abutment in manual-bridging
	if(!pickedAbutment.IsNull())
	{
		glDepthFunc(GL_ALWAYS);
		glLineWidth(2.0f);
		glColor(Color4b(Color4b::Yellow));
		glBegin(GL_LINES);
			glVertex( pickedAbutment.f->V0(pickedAbutment.z)->P() );
			glVertex( pickedAbutment.f->V1(pickedAbutment.z)->P() );
		glEnd();
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glLineWidth(2.0f);
	for(it = holesManager.holes.begin(); it != holesManager.holes.end(); ++it)
	{
		if(it->IsSelected())
		{
			if(it->IsAccepted())
				glColor(Color4b(Color4b::Green));
			else
				glColor(Color4b(Color4b::Red));
		}
		else
			glColor(Color4b(Color4b::Blue));

		it->Draw();
	}
}

void HoleListModel::drawCompenetratingFaces() const
{
	glDisable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);
	glDisable(GL_LIGHTING);
	glColor3f(0.8f, 0.8f, 0.f);

	// draw face border also behind other face
	HoleVector::const_iterator it;
	for(it = holesManager.holes.begin(); it != holesManager.holes.end(); ++it)
		if(it->IsCompenetrating())
			it->DrawCompenetratingFace(GL_LINE_LOOP);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// draw compenetrating face, only the visible part
	for(it = holesManager.holes.begin(); it != holesManager.holes.end(); ++it)
		if(it->IsCompenetrating())
			it->DrawCompenetratingFace(GL_TRIANGLES);

	// draw face border only visible part
	glLineWidth(4.0f);
	glColor3f(1.0f, 1.0f, 0.f);
	for(it = holesManager.holes.begin(); it != holesManager.holes.end(); ++it)
		if(it->IsCompenetrating())
			it->DrawCompenetratingFace(GL_LINE_LOOP);
}

void HoleListModel::toggleSelectionHoleFromFace(CFaceO *bface)
{
	assert(holesManager.IsHoleBorderFace(bface));
	HoleVector::iterator h;
	
	int ind = holesManager.FindHoleFromFace(bface, h);
	if( ind == -1)
		return;
	h->SetSelect( !h->IsSelected() );
  
	emit dataChanged( index(ind, 4), index(ind, 4) );
	emit SGN_needUpdateGLA();
}

void HoleListModel::toggleAcceptanceHole(CFaceO *bface)
{
	assert(state == HoleListModel::Filled);
	HoleVector::iterator h;
	int ind = holesManager.FindHoleFromFace(bface, h);
	
	if(ind == -1)
		return;
	h->SetAccepted( !h->IsAccepted() );
  
	emit dataChanged( index(ind, 6), index(ind, 6) );
	emit SGN_needUpdateGLA();
}

void HoleListModel::addBridgeFace(CFaceO *pickedFace, int pickedX, int pickedY)
{
	BridgeAbutment<CMeshO> picked;
	if (!holesManager.FindBridgeAbutmentFromPick(pickedFace, pickedX, pickedY, picked))
		return;

	// reselect an already selected edge... deselect
	if(pickedAbutment.f == picked.f && pickedAbutment.z == picked.z )
	{
		pickedAbutment.SetNull();
		return;
	}

	if(pickedAbutment.IsNull() || pickedAbutment.f == picked.f)
		pickedAbutment = picked;
	else
	{
		// 2 edge selected... bridge building
		std::vector<CMeshO::FacePointer *> local_facePointer;
		local_facePointer.push_back(&pickedAbutment.f);
		local_facePointer.push_back(&picked.f);
		QString errLog;
		if(FgtBridge<CMeshO>::CreateBridge(pickedAbutment, picked, &holesManager, errLog))
		{
			emit SGN_ExistBridge(true);
			emit layoutChanged();
		}
		else
			QMessageBox::warning(0, tr("Bridge error"), errLog);

		pickedAbutment.SetNull();
	}
}

void HoleListModel::fill(FgtHole<CMeshO>::FillerMode mode)
{
	mesh->updateDataMask(MeshModel::MM_FACEFACETOPO);
  //tri::UpdateTopology<CMeshO>::FaceFace(mesh->cm);
    if(holesManager.Fill(mode))
	{
		state = HoleListModel::Filled;
		emit layoutChanged();
	}
}

void HoleListModel::acceptFilling(bool accept)
{
/*	if(!accept)
	{
		mesh->clearDataMask(MeshModel::MM_FACEFLAGBORDER | MeshModel::MM_FACEFACETOPO );
		mesh->updateDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER );
	}
*/
	holesManager.ConfirmFilling(accept);
	
	state = HoleListModel::Selection;
	emit dataChanged( index(0, 0), index(holesManager.HolesCount(), 2) );
	if(holesManager.holes.size()==0)
	{
		QMessageBox::information(0, tr("No holes"), QString("Mesh have no hole to edit."));
		emit SGN_Closing();	
	}
	else
	{
		//emit layoutAboutToBeChanged();
		emit SGN_ExistBridge( holesManager.bridges.size() > 0 );
		emit SGN_needUpdateGLA();
		emit layoutChanged();
	}
}

void HoleListModel::autoBridge(bool singleHole, double distCoeff)
{
	holesManager.DiscardBridges();
	tri::UpdateTopology<CMeshO>::FaceFace(mesh->cm);

	if(singleHole)
		holesManager.AutoSelfBridging(distCoeff, 0);
	else
		holesManager.AutoMultiBridging(0);	

	emit SGN_ExistBridge( holesManager.bridges.size() > 0 );
	emit layoutChanged();
}

void HoleListModel::removeBridges()
{
	holesManager.DiscardBridges();
	emit SGN_ExistBridge(false);
	emit layoutChanged();
}

void HoleListModel::acceptBridges()
{
	holesManager.ConfirmBridges();
	emit SGN_ExistBridge(false);
}

void HoleListModel::closeNonManifolds()
{
	holesManager.CloseNonManifoldHoles();
	emit SGN_ExistBridge( holesManager.bridges.size()>0 );
	
	emit layoutChanged();
}


/************* Implementazione QAbstractItemModel class *****************/


QVariant HoleListModel::data(const QModelIndex &index, int role) const
{
	if(!index.isValid() )
		return QVariant();

	if(role == Qt::DisplayRole)
	{
		switch(index.column())
		{
		case 0:
			return holesManager.holes[index.row()].name;
		case 1:
			return holesManager.holes.at(index.row()).Size();
		case 2:
			return QString("%1").arg(holesManager.holes.at(index.row()).Perimeter(), 0, 'f', 5);
		}
	}
	else if (role == Qt::TextAlignmentRole)
	{
		if(index.column() == 0)   return Qt::AlignLeft;
		if(index.column() == 1 ||
			 index.column() == 2)   return Qt::AlignRight;
		return Qt::AlignCenter;
	}
	else if (role == Qt::CheckStateRole)
	{
		bool checked;
		if(index.column() == 3)
			checked = holesManager.holes[index.row()].IsNonManifold();
		else if(index.column() == 4)
			checked = holesManager.holes[index.row()].IsSelected();
		else if(state == HoleListModel::Filled && holesManager.holes[index.row()].IsSelected())
		{
			if(index.column() == 5)
				checked = holesManager.holes[index.row()].IsCompenetrating();
			else if(index.column() == 6)
				checked = holesManager.holes[index.row()].IsAccepted();
			else
				return QVariant();
		}
		else
			return QVariant();

		if(checked)
			return Qt::Checked;
		else
			return Qt::Unchecked;
	}

	return QVariant();
}


QVariant HoleListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		switch(section)
		{
		case 0:
			return tr("Hole");
		case 1:
			return tr("Edges");
		case 2:
			return tr("Perimeter");
		case 3:
			return tr("Non Manif.");
		case 4:
			if(state == HoleListModel::Filled)
				return tr("Fill");
			else
				return tr("Select");
		case 5:
			if(state == HoleListModel::Filled)
				return tr("Comp.");
		case 6:
			if(state == HoleListModel::Filled)
				return tr("Accept");
		}
	}
	else if (orientation == Qt::Horizontal && role == Qt::SizeHintRole)
	{
		switch(section)
		{
		case 0:
			return QSize(63, 20);
		case 1:
			return QSize(38, 20);
		case 2:
			return QSize(55, 20);
		case 3:
			return QSize(60, 20);
		case 4:
			if(state == HoleListModel::Filled)
				return QSize(20, 20);
			else
				return QSize(50, 20);
		case 5:
			return QSize(38, 20);
		case 6:
			return QSize(42, 20);
		}
	}
	else if (orientation == Qt::Horizontal && role == Qt::ToolTip && state==HoleListModel::Filled && section == 4)
		return tr("Compenetration");

	return QVariant();
}


QModelIndex HoleListModel::index(int row, int column, const QModelIndex &/*parent*/) const
{
	if(row >= (int)holesManager.holes.size())
		return QModelIndex();
	return createIndex(row,column/*,0*/);
}


Qt::ItemFlags HoleListModel::flags(const QModelIndex &index) const
{
	Qt::ItemFlags ret = QAbstractItemModel::flags(index);

	if (!index.isValid())
		return Qt::ItemIsEnabled;

	if(index.column() == 0)
		ret = ret | Qt::ItemIsEditable;
	else if( (index.column() == 4 && state == HoleListModel::Selection) ||
					 (index.column() == 6 && state == HoleListModel::Filled) )
		return Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
		//return ret = ret | Qt::ItemIsUserCheckable ;

	return ret;
}

bool HoleListModel::setData( const QModelIndex & index, const QVariant & value, int role )
{
	if(!index.isValid())
		return false;

	bool ret = false;
	if(role == Qt::EditRole && index.column() == 0)
	{
		QString newName = value.toString().trimmed();
		if(newName != "")
		{
			holesManager.holes[index.row()].name = newName;
			ret = true;
		}
	}
	else if(role == Qt::CheckStateRole)
	{
		if(state == HoleListModel::Selection)
		{
			if(index.column() == 4 && state == HoleListModel::Selection)
			{
				holesManager.holes[index.row()].SetSelect( !holesManager.holes[index.row()].IsSelected() );
				ret = true;
			}
		}
		else if(index.column() == 6)
		{	// check accept
			holesManager.holes[index.row()].SetAccepted( !holesManager.holes[index.row()].IsAccepted() );
			ret = true;
		}
	}
	if(ret)
	{
		emit dataChanged(index, index);
		emit SGN_needUpdateGLA();
	}

	return ret;
}

void HoleListModel::emitPostConstructionSignals()
{
	emit dataChanged( index(0, 0), index(holesManager.HolesCount(), 2) );
	emit SGN_needUpdateGLA();
}
