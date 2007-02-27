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
Revision 1.7  2007/02/27 23:58:36  cignoni
Changed apply/ok into apply/close

Revision 1.6  2007/01/13 02:02:28  cignoni
Changed loadFrameContent to pass the QString not as a reference (to avoid a temp var referencing)

Revision 1.5  2007/01/11 19:51:46  pirosu
fixed bug for QT 4.1.0/dotnet2003
removed the request of the window title to the plugin. The action description is used instead.

Revision 1.4  2007/01/11 10:40:09  cignoni
Renamed ambiguous min/max vars

Revision 1.3  2006/12/27 21:41:41  pirosu
Added improvements for the standard plugin window:
split of the apply button in two buttons:ok and apply
added support for parameters with absolute and percentage values

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
#include <QDoubleSpinBox>

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

class AbsPercWidget : public QGridLayout
{
	  Q_OBJECT

public:
  AbsPercWidget(QWidget *p, double defaultv, double minVal, double maxVal):QGridLayout(NULL)
  {
	  m_min = minVal;
	  m_max = maxVal;
	  absSB = new QDoubleSpinBox(p);
	  percSB = new QDoubleSpinBox(p);

	  absSB->setMinimum(m_min);
	  absSB->setMaximum(m_max);
	  absSB->setDecimals(3);
	  absSB->setSingleStep(0.001);
	  absSB->setValue(defaultv);

	  percSB->setMinimum(0);
	  percSB->setMaximum(100);
      percSB->setSingleStep(0.2);
	  percSB->setValue((100*(defaultv - m_min))/(m_max - m_min));

	  this->addWidget(absSB,0,0,Qt::AlignTop);
	  this->addWidget(percSB,0,1,Qt::AlignTop);


  	  connect(absSB,SIGNAL(valueChanged(double)),this,SLOT(on_absSB_valueChanged(double)));
  	  connect(percSB,SIGNAL(valueChanged(double)),this,SLOT(on_percSB_valueChanged(double)));


  }

  ~AbsPercWidget()
  {
	  delete absSB;
	  delete percSB;
  }

  float getValue();

public slots:

	void on_absSB_valueChanged(double newv); 
	void on_percSB_valueChanged(double newv);

protected:
  QDoubleSpinBox *absSB;
  QDoubleSpinBox *percSB;
  float m_min;
  float m_max;

};


// standard plugin window
class MeshlabStdDialog : public QDockWidget
{
	  Q_OBJECT

public:
  MeshlabStdDialog(QWidget *p):QDockWidget(QString("Plugin"),p)
  {
	qf = NULL;
	parlist = new StdParList();
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
	 void closeClick();
	 void topLevelChanged(bool);
 
protected:
	QFrame *qf;
	QAction *curaction;
	MeshModel *curmodel;
	MeshFilterInterface *curmfi;
	MainWindowInterface *curmwi;
	QVector<void *> stdfieldwidgets;
	StdParList *parlist;

	bool restorelastsize;
	QSize lastsize;

	void loadFrameContent(QString actiondesc);
	void stdClick();

};




#endif