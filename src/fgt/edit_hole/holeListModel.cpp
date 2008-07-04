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

/** Implementazione QAbstractItemModel class **/

QVariant HoleListModel::data(const QModelIndex &index, int role) const
{
	if(!index.isValid() || role != Qt::DisplayRole)
		return QVariant();
	
	switch(index.column())
	{
	case 0:
		return QString("Hole_%1").arg(index.row());
	case 1:
		return holes.at(index.row()).size;
	case 2:
		return holes.at(index.row()).isSelected;
	case 3:
		return holes.at(index.row()).isFilled;
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

/*
Qt::ItemFlags HoleListModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

	if(index.column()>1)
		return QAbstractItemModel::flags(index) | Qt::ItemIsUserCheckable ;
	else
		return QAbstractItemModel::flags(index);
}
*/