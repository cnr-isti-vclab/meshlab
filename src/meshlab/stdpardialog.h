/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2006                                          \/)\/    *
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

$Log$
Revision 1.2  2006/12/13 21:54:35  pirosu
2 updates for the standard plugin window: 1) it recovers its last size when it is undocked and 2) it closes itself when a filter is applied (only if it is floating)


Revision 1.0  2006/12/13 17:37:02  pirosu
Added standard plugin window support

****************************************************************************/

#ifndef MESHLAB_STDPARDIALOG_H
#define MESHLAB_STDPARDIALOG_H

#include <QtCore>
#include <QAction>
#include <QtGui>

#include "meshmodel.h"
#include "filterparameter.h"
#include "interfaces.h"

// frame for the standard plugin window
class MeshlabStdDialogFrame : public QFrame
{
public:
    MeshlabStdDialogFrame(QWidget *parent)
        : QFrame(parent)
    {
    }

};


// standard plugin window
class MeshlabStdDialog : public QDockWidget
{
	  Q_OBJECT

public:
  MeshlabStdDialog(QWidget *p):QDockWidget(QString("Plugin"),p)
  {
	qf = NULL;
	initValues();
	QSize siz = this->size();
	lastsize.setWidth(siz.width());
	lastsize.setHeight(siz.height());
	restorelastsize = false;
	connect(this,SIGNAL(topLevelChanged(bool)),this,SLOT(topLevelChanged(bool)));

  }

	void initValues();
	void resetMe();
    void resizeEvent(QResizeEvent *e);
	void loadPluginAction(MeshFilterInterface *mfi,MeshModel *mm,QAction *q,MainWindowInterface *mwi);




  private slots:
	 void applyClick();
	 void topLevelChanged(bool);
 
protected:
	QFrame *qf;
	QAction *curaction;
	MainWindowInterface *curmwi;
	QVector<QWidget *> stdfieldwidgets;
	QWidget *curextra;
	StdParList parlist;

	bool restorelastsize;
	QSize lastsize;
};




#endif