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
	updateModel();	
}

void HoleListModel::clearModel()
{
	holes.clear();

	if(userBitHole > 0)
	{
		//mesh->clearDataMask(userBitHole);
		CMeshO::FaceIterator fi;
		for(fi = mesh->cm.face.begin(); fi!=mesh->cm.face.end(); ++fi)
		{
			//if(!(*fi).IsD())
				(*fi).ClearUserBit(userBitHole);
		}
	}
}

void HoleListModel::updateModel()
{
	clearModel();
	mesh->clearDataMask(MeshModel::MM_BORDERFLAG);
	mesh->updateDataMask(MeshModel::MM_FACETOPO);
	mesh->updateDataMask(MeshModel::MM_BORDERFLAG);

	userBitHole = FgtHole<CMeshO>::GetMeshHoles(mesh->cm, holes);
	emit dataChanged( index(0, 0), index(holes.size(), 2) );
}

void HoleListModel::drawHoles() const
{
	// Disegno i contorni del buco
	glLineWidth(2.0f);
	glDepthFunc(GL_ALWAYS);
	glDisable(GL_DEPTH_TEST); 
	glDepthMask(GL_FALSE);
	glDisable(GL_LIGHTING);

	// scorro tutti i buchi
	HoleVector::const_iterator it = holes.begin();
	for( ; it != holes.end(); ++it)
	{
		if( it->isSelected && it->isAccepted)
			glColor(Color4b::DarkGreen);
		else
			glColor(Color4b::DarkRed);
		it->Draw();
	}

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST); 
	glDepthFunc(GL_LESS);

	for(it = holes.begin(); it != holes.end(); ++it)
	{
		if( it->isSelected && it->isAccepted)
			glColor(Color4b::Green);
		else
			glColor(Color4b::Red);
				
		it->Draw();
	}	
}

void HoleListModel::toggleSelectionHoleFromBorderFace(CFaceO *bface)
{
	int ind = FgtHole<CMeshO>::FindHoleFromBorderFace(bface, holes);
	if(ind == -1)
		return;
	holes[ind].isSelected = !holes[ind].isSelected;

	emit dataChanged( index(ind, 2), index(ind, 2) );
}

void HoleListModel::fill()
{
	std::vector<CMeshO::FacePointer *> local_facePointer;
	
	HoleVector::iterator it = holes.begin();
	for( ; it != holes.end(); it++ )
		local_facePointer.push_back(&it->holeInfo.p.f);
	
	for(it = holes.begin(); it != holes.end(); it++ )
	{
		if( it->isSelected )
		{
			it->facesPatch.clear();
			vcgHole::FillHoleEar<vcg::tri::TrivialEar<CMeshO> >(mesh->cm, it->holeInfo, userBitHole, local_facePointer, &(it->facesPatch));
			state = HoleListModel::Filled;
		}
	}

	emit layoutChanged();
}

void HoleListModel::acceptFilling(bool forcedCancel)
{
	HoleVector::iterator it = holes.begin();
	for( ; it != holes.end(); it++ )
	{
		if( (it->isSelected && !it->isAccepted) || forcedCancel)
			it->RestoreHole(mesh->cm);
	}
	
	mesh->clearDataMask(MeshModel::MM_FACETOPO);
	updateModel();
	state = HoleListModel::Selection;
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
			return holes[index.row()].name;
		case 1:
			return holes.at(index.row()).size;		
		}
	}
	else if (role == Qt::CheckStateRole)
	{
		bool checked;
		if(index.column() == 2)
			checked = checked = holes[index.row()].isSelected;
		else if(state == HoleListModel::Filled && holes[index.row()].isSelected)
		{
			if(index.column() == 3)
				checked = holes[index.row()].isCompenetrating;
			else if(index.column() == 4)
				checked = holes[index.row()].isAccepted;
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
			return tr("Size");
		case 2:
			if(state == HoleListModel::Selection)
				return tr("Select");
			else 
				return tr("Fill");
		case 3:
			if(state == HoleListModel::Filled)
				return tr("Comp.");
		case 4:
			if(state == HoleListModel::Filled)
				return tr("Accept");
		}
	}
/*	else if (orientation == Qt::Horizontal && role == Qt::SizeHintRole)
	{
		switch(section)
		{
		case 0:
			return 70;
		case 1:
			return 70;
		case 2:
			return 30;
		case 3:
			return 30;
		}
	}
*/	else if (orientation == Qt::Horizontal && role == Qt::ToolTip && state==HoleListModel::Filled && section == 3)
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

	if(index.column() == 1 && state == HoleListModel::Filled)
		return ret;
	else if(index.column() > 1)
		ret = ret | Qt::ItemIsUserCheckable ;
	else if(index.column() == 0)
		ret = ret | Qt::ItemIsEditable;
		
	return ret;
}

bool HoleListModel::setData( const QModelIndex & index, const QVariant & value, int role )
{	
	if(!index.isValid())
		return false;

	if(role == Qt::EditRole && index.column() == 0)
	{
		QString newName = value.toString().trimmed();
		if(newName != "")
		{
			holes[index.row()].name = newName;
			emit dataChanged(index, index);
			return true;
		}
	}
	else if(role == Qt::CheckStateRole)
	{
		if(state == HoleListModel::Selection)
		{
			if(index.column() == 2 && state == HoleListModel::Selection)
			{
				holes[index.row()].isSelected = !holes[index.row()].isSelected;
				return true;
			}			
		}
		else if(index.column() == 4)
		{	// check accept
			holes[index.row()].isAccepted = !holes[index.row()].isAccepted;
			return true;
		}
	}
	return false;
}
