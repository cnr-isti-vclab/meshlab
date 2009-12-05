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
#ifndef HOLELISTMODEL_H
#define HOLELISTMODEL_H

#include <QWidget>
#include <QHeaderView>
#include <QtGui>
#include <common/meshmodel.h>
#include "fgtHole.h"
#include "fgtBridge.h"
#include "holeSetManager.h"

/*  This class is the "model" of model-view architecture, so it implements methods to exposes data
 *  informations about holes as QAbstractItemModel says.
 *  It contains an istance of HoleSetManager to know and manipulate holes and bridges.
 */
class HoleListModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	enum FillerState
	{
		Selection, ManualBridging, Filled
	};

	typedef FgtHole<CMeshO>					HoleType;
	typedef std::vector< HoleType >	HoleVector;
	
	HoleListModel(MeshModel *m, QObject *parent = 0);
	virtual ~HoleListModel() { holesManager.Clear(); };

	inline int rowCount(const QModelIndex &/*parent = QModelIndex()*/) const { return holesManager.HolesCount(); };
	inline int columnCount(const QModelIndex &/*parent = QModelIndex()*/) const
	{
		if(state == HoleListModel::Selection) return 5;
		else return 7;
	};

	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &/*child*/) const { return QModelIndex(); };

  QVariant data(const QModelIndex &index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	Qt::ItemFlags flags(const QModelIndex &index) const;
	bool setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );


	void drawHoles() const;
	void drawCompenetratingFaces() const;

	inline void setState(HoleListModel::FillerState s) { state = s; emit layoutChanged(); };
	inline FillerState getState() const { return state; };
	void toggleSelectionHoleFromFace(CFaceO *bface);
	void toggleAcceptanceHole(CFaceO *bface);
	void fill(FgtHole<CMeshO>::FillerMode mode);
	void acceptFilling(bool accept=true);
	void acceptBridges();
	void removeBridges();
	void closeNonManifolds();
	inline MeshModel* getMesh() const { return mesh; };

	void autoBridge(bool singleHole=false, double distCoeff=0);

	inline bool PickedAbutment() const {return !pickedAbutment.IsNull(); };
	inline void setStartBridging()
	{
		assert(state != HoleListModel::Filled);
		state = HoleListModel::ManualBridging ;
	};
	inline void setEndBridging() { state = HoleListModel::Selection; pickedAbutment.SetNull(); };
	void addBridgeFace(CFaceO *pickedFace, int pickX, int pickY);

	
private:
	MeshModel *mesh;
	FillerState state;
	BridgeAbutment<CMeshO> pickedAbutment;
	
public:
	HoleSetManager<CMeshO> holesManager;
	
Q_SIGNALS:
	void SGN_Closing();
	void SGN_needUpdateGLA();
	void SGN_ExistBridge(bool exist);
};


class HoleSorterFilter: public QSortFilterProxyModel
{
	Q_OBJECT

public:
	HoleSorterFilter(QObject *parent = 0): QSortFilterProxyModel(parent){};
	virtual ~HoleSorterFilter() { };

	bool lessThan(const QModelIndex &left, const QModelIndex &right) const
	{
		if(left.column() == 0)
			return left.data().toString() < right.data().toString();
		else if(left.column() == 1 || left.column() == 2)
			return left.data().toDouble() < right.data().toDouble();

		// check box
		if(!left.data(Qt::CheckStateRole).isValid() && right.data(Qt::CheckStateRole).isValid())
			return false;
		else if(!right.data(Qt::CheckStateRole).isValid() && left.data(Qt::CheckStateRole).isValid())
			return true;

		if(left.data(Qt::CheckStateRole) == Qt::Unchecked && right.data(Qt::CheckStateRole) == Qt::Checked)
			return false;
		else
			return true;
	};

};


#endif
