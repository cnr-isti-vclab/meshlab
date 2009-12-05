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
#include "fillerDialog.h"
#include "holeListModel.h"
#include <common/interfaces.h>

class EditHoleAutoBridgingCB : public AutoBridgingCallback
{
public:
	EditHoleAutoBridgingCB(QLabel* lab, int off)
	{
		offset = off;
		label = lab;
	};

	void Invoke(int val)
	{
		label->setText( QString("Auto-bridging: %1%").arg(val) );
		label->repaint();			
	};

private:
	QLabel* label;
};


class EditHolePlugin : public QObject, public MeshEditInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshEditInterface)
	
public:
	
	EditHolePlugin();
	virtual ~EditHolePlugin();
	static const QString Info();
	virtual bool  StartEdit(MeshModel &/*m*/, GLArea * /*parent*/);
	virtual void  EndEdit(MeshModel &/*m*/, GLArea * /*parent*/);
	virtual void  Decorate(MeshModel &/*m*/, GLArea * /*parent*/);
	virtual void  mousePressEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea * );
	virtual void  mouseMoveEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea * );
	virtual void  mouseReleaseEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea * );	

private:
	void setInfoLabel();

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
	void manualBridge();
	void autoBridge();
	void closeNMHoles();
	void cancelFill();
	void acceptFill();
	void acceptBridges();
	void clearBridge();
	void skipTab(int index);
	void updateDWeight(int val);
	void updateBridgeSldValue(int val);
	void chekSingleBridgeOpt();
};
#endif
