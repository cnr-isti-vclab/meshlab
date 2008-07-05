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
	state = FillerState::Selection;
	mesh = m;
	updateModel();
}

void HoleListModel::updateModel()
{
	holes.clear();

	//mesh->clearDataMask( MeshModel::MM_BORDERFLAG | MeshModel::MM_FACETOPO );

	mesh->updateDataMask(MeshModel::MM_FACETOPO);
	mesh->updateDataMask(MeshModel::MM_BORDERFLAG);

	FgtHole<CMeshO>::GetMeshHoles(mesh->cm, holes);
	emit dataChanged( index(0, 0), index(holes.size(), 2) );
}

void HoleListModel::drawHoles() const
{

	glLineWidth(2.0f);
	glDepthFunc(GL_ALWAYS);
	glDisable(GL_DEPTH_TEST); 
	glDepthMask(GL_FALSE);
	glDisable(GL_LIGHTING);

	HoleVector::const_iterator it = holes.begin();
	// scorro tutti i buchi
	for( ; it != holes.end(); ++it)
	{
		if( it->isSelected )
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
		if( it->isSelected )
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
		else if(index.column() == 3 && state == FillerState::Filled)
			checked = holes[index.row()].isAccepted;
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
			if(state == FillerState::Selection)
				return tr("Select");
			else 
				return tr("Fill");
		case 3:
			if(state == FillerState::Filled)
				return tr("Comp.");
		}
	}
    
	if (orientation == Qt::Horizontal && role == Qt::ToolTip && state==FillerState::Filled)
		return tr("Compenetration");	

	return QVariant();
}


QModelIndex HoleListModel::index(int row, int column, const QModelIndex &parent) const
{
	if(row>= holes.size())
		return QModelIndex();

	/*void * ptr = 0;
	switch(column)
	{
	case 0:
		break;
	case 1:
		ptr = (void*)&holes->at(row).size;
		break;
	case 2:
		if(state == FillerState::Selection)
			ptr = (void*)&holes->at(row).isSelected;
		else
			ptr = (void*)&holes->at(row).isFilled;
		break;
	}
	
	if(state == FillerState::Filled && column == 3)
		ptr = (void*)&holes->at(row).isAccepted;*/
	return createIndex(row,column, 0);
}


Qt::ItemFlags HoleListModel::flags(const QModelIndex &index) const
{
	Qt::ItemFlags ret = QAbstractItemModel::flags(index);
    
	if (!index.isValid())
        return Qt::ItemIsEnabled;

	if(index.column() == 1 && state == FillerState::Filled)
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
		if(index.column() == 2 && state == FillerState::Selection)
		{
			holes[index.row()].isSelected = !holes[index.row()].isSelected;
			return true;
		}
		else if(index.column() == 2 && state == FillerState::Selection)
		{
			holes[index.row()].isSelected = !holes[index.row()].isSelected;
			return true;
		}
			
	}
	return false;
}
