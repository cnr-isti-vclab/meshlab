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
Revision 1.15  2008/01/28 13:02:00  cignoni
added support for filters on collection of meshes (layer filters)

Revision 1.14  2007/11/30 07:19:09  cignoni
moved generic dialog to the meshlab base

Revision 1.13  2007/11/20 18:26:16  ponchio
Added enum support for auto dialogs, working.

Revision 1.12  2007/11/19 17:09:20  ponchio
added enum value. [untested].

Revision 1.11  2007/11/19 15:51:50  cignoni
Added frame abstraction for reusing the std dialog mechanism

Revision 1.10  2007/11/05 13:34:41  cignoni
added color and Help

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
#include <QObject>
//#include <QDoubleSpinBox>

#include "meshmodel.h"
#include "filterparameter.h"
#include "interfaces.h"

/// Widget to enter a color. 
class QColorButton : public QHBoxLayout
{
	  Q_OBJECT
		
		QPushButton *colorButton;
		QLabel *colorLabel;
		QColor currentColor;
	public:
		QColorButton(QWidget *p, QColor newColor);
		QColor getColor();
		void  setColor(QColor newColor);
							 
	private slots:
		void pickColor(); 
};

/// Widget to enter a value as a percentage or as an absolute value. 
/// You have to specify the default value and the range of the possible values.
/// The default value is expressed in ABSolute units (e.g. it should be in the min..max range.

class AbsPercWidget : public QGridLayout
{
	  Q_OBJECT

public:
  AbsPercWidget(QWidget *p, double defaultv, double minVal, double maxVal);
  ~AbsPercWidget();

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


class StdParFrame : public QFrame
{
	Q_OBJECT
public:
	StdParFrame(QWidget *p);

	void loadFrameContent(FilterParameterSet &curParSet);
	void readValues(FilterParameterSet &curParSet);
	void resetValues(FilterParameterSet &curParSet);

	void toggleHelp();	
	
	QVector<void *> stdfieldwidgets;
	QVector<QLabel *> helpList;

};

/// Widget to enter a color. 
class EnumWidget : public QHBoxLayout
{
	  Q_OBJECT
		
		QComboBox *enumCombo;
		QLabel *enumLabel;
	public:
		EnumWidget(QWidget *p, int newEnum, QStringList values);
		int getEnum();
		void  setEnum(int newEnum);	
};

// This class provide a modal dialog box for asking a generic parameter set
// It can be used by anyone needing for some values in a structred form and having some integrated help
class GenericParamDialog: public QDialog
{
	Q_OBJECT 
public:
  GenericParamDialog(QWidget *p, FilterParameterSet *_curParSet); 
	
	FilterParameterSet *curParSet;
	FilterParameterSet *defaultParSet;
	StdParFrame *stdParFrame;
	
	void createFrame();
  void resetValues();
	
	public slots:	
	void getAccept();
	void toggleHelp();
};


// standard plugin window
//class MeshlabStdDialog : public QDialog
class MeshlabStdDialog : public QDockWidget
{
	  Q_OBJECT

public:
  MeshlabStdDialog(QWidget *p);

	void clearValues();
	void createFrame();
	void loadFrameContent();

	void showAutoDialog(MeshFilterInterface *mfi, MeshModel *mm, MeshDocument * md, QAction *q, MainWindowInterface *mwi);

private slots:
	void applyClick();
	void closeClick();
  void resetValues();
  void toggleHelp();

protected:
	QFrame *qf;
	StdParFrame *stdParFrame;
	QAction *curAction;
public:
	MeshModel *curModel;
	MeshDocument * curMeshDoc;
	MeshFilterInterface *curmfi;
	MainWindowInterface *curmwi;
	FilterParameterSet curParSet;
	
};

#endif

