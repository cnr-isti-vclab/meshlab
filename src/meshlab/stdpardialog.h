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
Revision 1.9  2007/10/02 07:59:44  cignoni
New filter interface. Hopefully more clean and easy to use.

Revision 1.8  2007/03/26 08:25:10  zifnab1974
added eol at the end of the files

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


/// Widget to enter a value as a percentage or as an absolute value. 
/// You have to specify the default value and the range of the possible values.
/// The default value is expressed in ABSolute units (e.g. it should be in the min..max range.

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

	  //absSB->setMinimum(m_min);
	  absSB->setMaximum(m_max*2);
	  absSB->setDecimals(3);
	  absSB->setSingleStep((m_max-m_min)/20.0);
	  absSB->setValue(defaultv);

	  percSB->setMinimum(0);
	  percSB->setMaximum(200);
		percSB->setSingleStep(0.5);
	  percSB->setValue((100*(defaultv - m_min))/(m_max - m_min));
		QLabel *absLab=new QLabel("<i> <small> world unit</small></i>");
		QLabel *percLab=new QLabel("<i> <small> perc on"+QString("(%1 .. %2)").arg(m_min).arg(m_max)+"</small></i>");
		
		this->addWidget(absLab,0,0,Qt::AlignHCenter);
		this->addWidget(percLab,0,1,Qt::AlignHCenter);
		
	  this->addWidget(absSB,1,0);
	  this->addWidget(percSB,1,1,Qt::AlignTop);


		connect(absSB,SIGNAL(valueChanged(double)),this,SLOT(on_absSB_valueChanged(double)));
		connect(percSB,SIGNAL(valueChanged(double)),this,SLOT(on_percSB_valueChanged(double)));
  }

  ~AbsPercWidget()
  {
	  delete absSB;
	  delete percSB;
  }

  float getValue();
	void  setValue(float val, float minV, float maxV);

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
  MeshlabStdDialog(QWidget *p);

	void clearValues();
	void createFrame();
	void loadFrameContent();

	void resizeEvent(QResizeEvent *e);
	void showAutoDialog(MeshFilterInterface *mfi, MeshModel *mm, QAction *q, MainWindowInterface *mwi);

  private slots:
	void applyClick();
	void closeClick();
  void resetValues();

protected:
	QFrame *qf;
	QAction *curAction;
	MeshModel *curModel;
	MeshFilterInterface *curmfi;
	MainWindowInterface *curmwi;
	FilterParameterSet curParSet;
	
	QVector<void *> stdfieldwidgets;
	
	bool restorelastsize;
	QSize lastsize;
	
};




#endif

