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

#ifndef EDITHOLEPLUGIN_H
#define EDITHOLEPLUGIN_H

#include <QObject>
#include <QList>
#include "fillerDialog.h"
#include "holeListModel.h"
#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>

class EditHolePlugin : public QObject, public MeshEditInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshEditInterface)
	QList <QAction *> actionList;
	
public:
	typedef vcg::tri::Hole<CMeshO> vcgHole;
	typedef vcgHole::Info HoleInfo;
	typedef std::vector< FgtHole<CMeshO> > HoleVector;
	typedef vcg::face::Pos<CMeshO::FaceType> PosType;
	typedef CMeshO::FaceIterator FaceIterator;
	
	EditHolePlugin();
    virtual ~EditHolePlugin();
	virtual const QString Info(QAction *);
    virtual const PluginInfo &Info();
	virtual void  StartEdit(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/);
    virtual void  EndEdit(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/);
    virtual void  Decorate(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/);
    virtual void  mousePressEvent    (QAction *, QMouseEvent *event, MeshModel &/*m*/, GLArea * );
    virtual void  mouseMoveEvent     (QAction *,QMouseEvent *event, MeshModel &/*m*/, GLArea * );
    virtual void  mouseReleaseEvent  (QAction *,QMouseEvent *event, MeshModel &/*m*/, GLArea * );
	virtual QList<QAction *> actions() const ;
	
	
private:
	HoleListModel	*holesModel;
	HoleSorterFilter *holeSorter;
	GLArea *gla;
	MeshModel *mesh;
  FillerDialog *dialogFiller;
	
	bool toDraw;
	bool hasPick;
	QPoint cur;
	CFaceO* pickedFace;
	int bridgeOptSldVal;
	
private Q_SLOTS:
	void upGlA();
	void resizeViewColumn();
	void fill();
	void bridge();
	void cancelFill();
	void acceptFill();
	void clearBridge();
	void skipTab(int index);
	void updateDWeight(int val);
	void updateBridgeSldValue(int val);
	void chekSingleBridgeOpt();
};
#endif
