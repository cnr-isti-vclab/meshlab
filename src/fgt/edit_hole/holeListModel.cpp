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

using namespace vcg;


HoleListModel::HoleListModel(MeshModel *m, QObject *parent)
	: QAbstractItemModel(parent)
{
	state = HoleListModel::Selection;
	mesh = m;
	userBitHole = -1;
	pickedAbutment.SetNull();
	updateModel();
}

void HoleListModel::clearModel()
{
	HoleVector::iterator it;
	for(it=holes.begin(); it!=holes.end(); it++)
		it->ResetFlag();

	holes.clear();
	pickedAbutment.SetNull();
}

void HoleListModel::updateModel()
{
	clearModel();
	mesh->clearDataMask(MeshModel::MM_BORDERFLAG);
	mesh->updateDataMask(MeshModel::MM_FACETOPO);
	mesh->updateDataMask(MeshModel::MM_BORDERFLAG);

	userBitHole = FgtHole<CMeshO>::GetMeshHoles(mesh->cm, holes);
	nSelected=0;
	nAccepted=0;
	emit dataChanged( index(0, 0), index(holes.size(), 2) );
	emit SGN_needUpdateGLA();
}

void HoleListModel::drawHoles() const
{
	// Disegno i contorni del buco
	glLineWidth(2.0f);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_GREATER);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	// scorro tutti i buchi
	HoleVector::const_iterator it = holes.begin();
	for( ; it != holes.end(); ++it)
	{
		if( it->IsSelected())
		{
			if(it->IsAccepted())
				glColor(Color4b::DarkGreen);
			else
				glColor(Color4b::DarkRed);
		}
		else
			glColor(Color4b::DarkBlue);
		it->Draw();
	}


	if(!pickedAbutment.IsNull())
	{
		glDepthFunc(GL_ALWAYS);
		glLineWidth(2.0f);
		glColor(Color4b::Yellow);
		glBegin(GL_LINES);
			glVertex( pickedAbutment.f->V0(pickedAbutment.z)->P() );
			glVertex( pickedAbutment.f->V1(pickedAbutment.z)->P() );
		glEnd();
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glLineWidth(2.0f);
	for(it = holes.begin(); it != holes.end(); ++it)
	{
		if(it->IsSelected())
		{
			if(it->IsAccepted())
				glColor(Color4b::Green);
			else
				glColor(Color4b::Red);
		}
		else
			glColor(Color4b::Blue);

		it->Draw();
	}
}

void HoleListModel::drawCompenetratingFaces() const
{
	glDisable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);
	glDisable(GL_LIGHTING);
	glColor3f(0.8f, 0.8f, 0.f);
	HoleVector::const_iterator it;
	for(it = holes.begin(); it != holes.end(); ++it)
		if(it->IsCompenetrating())
			it->DrawCompenetratingFace(GL_LINE_LOOP);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	for(it = holes.begin(); it != holes.end(); ++it)
		if(it->IsCompenetrating())
			it->DrawCompenetratingFace(GL_TRIANGLES);

	glLineWidth(4.0f);
	glColor3f(1.0f, 1.0f, 0.f);
	for(it = holes.begin(); it != holes.end(); ++it)
		if(it->IsCompenetrating())
			it->DrawCompenetratingFace(GL_LINE_LOOP);

}

void HoleListModel::toggleSelectionHoleFromBorderFace(CFaceO *bface)
{
	assert(FgtHole<CMeshO>::IsHoleBorderFace(*bface));
	HoleVector::iterator h;
	int ind = FgtHole<CMeshO>::FindHoleFromBorderFace(bface, holes, h);
	if( ind == -1)
		return;
	h->SetSelect( !h->IsSelected() );
  if(h->IsSelected())
    nSelected++;
  else
    nSelected--;
	emit dataChanged( index(ind, 4), index(ind, 4) );
	emit SGN_needUpdateGLA();
}

void HoleListModel::toggleAcceptanceHole(CFaceO *bface)
{
	assert(state == HoleListModel::Filled);
	HoleVector::iterator h;
	int ind = -1;
	if(FgtHole<CMeshO>::IsHoleBorderFace(*bface))
		ind = FgtHole<CMeshO>::FindHoleFromBorderFace(bface, holes, h);
	else if(FgtHole<CMeshO>::IsPatchFace(*bface) && !FgtHole<CMeshO>::IsBridgeFace(*bface))
		ind = FgtHole<CMeshO>::FindHoleFromPatchFace(bface, holes, h);

	if(ind == -1)
		return;
	h->SetAccepted( !h->IsAccepted() );
  if(h->IsAccepted())
    nAccepted++;
  else
    nAccepted--;
	emit dataChanged( index(ind, 6), index(ind, 6) );
	emit SGN_needUpdateGLA();
}

void HoleListModel::addBridgeFace(CFaceO *pickedFace, int pickedX, int pickedY)
{
	BridgeAbutment<CMeshO> picked;
	if (!FgtBridge<CMeshO>::FindBridgeAbutmentFromPick(pickedFace, pickedX, pickedY, holes, picked))
		return;

	if(pickedAbutment.f == picked.f && pickedAbutment.z == picked.z )
	{
		pickedAbutment.SetNull();
		return;
	}

	if(pickedAbutment.IsNull() || pickedAbutment.f == picked.f)
		pickedAbutment = picked;
	else
	{
		std::vector<CMeshO::FacePointer *> local_facePointer;
		local_facePointer.push_back(&pickedAbutment.f);
		local_facePointer.push_back(&picked.f);

		if(FgtBridge<CMeshO>::CreateBridge(pickedAbutment, picked, mesh->cm, holes, &local_facePointer))
		{
			emit SGN_ExistBridge(true);
			emit layoutChanged();
		}
		else
			QMessageBox::warning(0, tr("Bridge autocompenetration"), QString("Bridge is compenetrating with mesh."));

		pickedAbutment.SetNull();
	}
}

void HoleListModel::fill(FgtHole<CMeshO>::FillerMode mode)
{
	std::vector<CMeshO::FacePointer *> local_facePointer;

	HoleVector::iterator it;
	FgtHole<CMeshO>::AddFaceReference(holes, local_facePointer);

	for(it = holes.begin(); it != holes.end(); it++ )
		if( it->IsSelected() )
		{
			it->Fill(mode, mesh->cm, local_facePointer);
			state = HoleListModel::Filled;
		}
  nAccepted=nSelected;
  emit layoutChanged();
}

void HoleListModel::acceptFilling(bool accept)
{
  nSelected=0;
	HoleVector::iterator it = holes.begin();
	while( it != holes.end() )
	{
		if( it->IsFilled() )
		{
			if( (it->IsSelected() && !it->IsAccepted()) || !accept)
			{
				if( it->IsFilled() )
				{
				  	it->RestoreHole(mesh->cm);
				  	nSelected++;
				}
			}
			else if( it->IsSelected() && it->IsAccepted() )
			{
				it->ResetFlag();
				it = holes.erase(it);
				continue;
			}
		}
		it++;
	}

	state = HoleListModel::Selection;
	emit dataChanged( index(0, 0), index(holes.size(), 2) );
	emit SGN_needUpdateGLA();
	emit layoutChanged();
}

void HoleListModel::autoBridge(bool singleHole, double distCoeff, QLabel *infoLabel)
{
	FgtBridge<CMeshO>::RemoveBridges(mesh->cm, holes);
	
	int nb=0;
	if(singleHole)
		nb = FgtBridge<CMeshO>::AutoSelfBridging(mesh->cm, holes, infoLabel, distCoeff, 0);
	else
		nb = FgtBridge<CMeshO>::AutoMultiBridging(mesh->cm, holes, infoLabel, distCoeff, 0);

	countSelected();

	emit SGN_ExistBridge( nb>0 );
	emit layoutChanged();
}

void HoleListModel::removeBridges()
{
	FgtBridge<CMeshO>::RemoveBridges(mesh->cm, holes);
	countSelected();

	emit SGN_ExistBridge(false);
	emit layoutChanged();
}

void HoleListModel::acceptBridges()
{
	FgtBridge<CMeshO>::AcceptBridges(holes);
	state = HoleListModel::Selection;
	emit SGN_ExistBridge(false);
}

void HoleListModel::closeNonManifolds()
{
	if (FgtBridge<CMeshO>::CloseNonManifoldVertex(mesh->cm, holes) > 0 )
	{
		countSelected();
    emit SGN_ExistBridge(true);
	}
	emit layoutChanged();
}

void HoleListModel::countSelected()
{
	nSelected = 0;
  HoleVector::iterator hit = holes.begin();
  for( ; hit!=holes.end(); hit++)
    if(hit->IsSelected())
      nSelected++;
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
			return holes[index.row()].name;
		case 1:
			return holes.at(index.row()).Size();
		case 2:
			return QString("%1").arg(holes.at(index.row()).Perimeter(), 0, 'f', 5);
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
			checked = holes[index.row()].IsNonManifold();
		else if(index.column() == 4)
			checked = holes[index.row()].IsSelected();
		else if(state == HoleListModel::Filled && holes[index.row()].IsSelected())
		{
			if(index.column() == 5)
				checked = holes[index.row()].IsCompenetrating();
			else if(index.column() == 6)
				checked = holes[index.row()].IsAccepted();
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


QModelIndex HoleListModel::index(int row, int column, const QModelIndex &parent) const
{
	if(row >= (int)holes.size())
		return QModelIndex();
	return createIndex(row,column, 0);
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
			holes[index.row()].name = newName;
			ret = true;
		}
	}
	else if(role == Qt::CheckStateRole)
	{
		if(state == HoleListModel::Selection)
		{
			if(index.column() == 4 && state == HoleListModel::Selection)
			{
				holes[index.row()].SetSelect( !holes[index.row()].IsSelected() );
				if(holes[index.row()].IsSelected() ) nSelected++;
        else nSelected--;
				ret = true;
			}
		}
		else if(index.column() == 6)
		{	// check accept
			holes[index.row()].SetAccepted( !holes[index.row()].IsAccepted() );
			if(holes[index.row()].IsAccepted() ) nAccepted++;
      else nAccepted--;
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
