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
Revision 1.16  2008/04/16 12:05:51  cignoni
option for title of the dialog

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



/// Widget to enter a Point3f, 
/// if at the creation you provide a pointer to a GLArea (the mesh viewing window) 
/// the widget exposes a button for getting the current view directiont 

class Point3fWidget : public QGridLayout
{
	Q_OBJECT
	
public:
  Point3fWidget(QWidget *p, vcg::Point3f defaultv, QWidget *gla);
  ~Point3fWidget();
	
	vcg::Point3f getValue();
	public slots:
	void  setValue(vcg::Point3f val);	
		
protected:
	QLineEdit * coordSB[3];
	QPushButton *getViewButton;
};

class DynamicFloatWidget : public QGridLayout
{
	Q_OBJECT
	
public:
  DynamicFloatWidget(QWidget *p, double defaultv, double minVal, double maxVal, int mask);
  ~DynamicFloatWidget();
	
  float getValue();
	void  setValue(float val, float minV, float maxV);
	
	public slots:
		void setValue(int newv);
		void setValue();
		void setValue(float newValue);

	signals:
		void valueChanged(int mask);
	
protected:
	QLineEdit *valueLE;
	QSlider   *valueSlider;
  float minVal;
  float maxVal;
	int mask;
private :
	float intToFloat(int val);  
	int floatToInt(float val);
};

/* 
This class is used to automatically create a frame from a set of parameters. 
it is used mostly for creating the main dialog of the filters, but it is used also 
in the creation of the additional saving options, post and pre opening processing 
and for general parameter setting in edit plugins (e.g. look at the aligment parameters)
*/
class StdParFrame : public QFrame
{
	Q_OBJECT
public:
	StdParFrame(QWidget *p, QWidget *gla=0);

	void loadFrameContent(FilterParameterSet &curParSet,MeshDocument *mdPt = 0);
	void readValues(FilterParameterSet &curParSet);
	void resetValues(FilterParameterSet &curParSet);

	void toggleHelp();	
	
	QVector<void *> stdfieldwidgets;
	QVector<QLabel *> helpList;
private: 
	QWidget *gla; // used for having a link to the glarea that spawned the parameter asking.
	
signals:
		void dynamicFloatChanged(int mask);
};

/// Widget to select an entry from a list
class EnumWidget : public QHBoxLayout
{
	  Q_OBJECT
protected:	
		QComboBox *enumCombo;
		QLabel *enumLabel;
	public:
		EnumWidget(){};		
		EnumWidget(QWidget *p, int newEnum, QStringList values);
		void Init(QWidget *p, int newEnum, QStringList values);
		int getEnum();
		void  setEnum(int newEnum);	
		
		//returns the number of items in the list 
		int getSize();
};



/// Widget to select a Layer the current one
class MeshEnumWidget : public EnumWidget
{
private:
	MeshDocument *md;
public:
	MeshEnumWidget(QWidget *p, MeshModel *defaultMesh, MeshDocument &md);
		
	MeshModel * getMesh();
	void setMesh(MeshModel * newMesh);
};


class QVariantListWidget : public QHBoxLayout
{
	Q_OBJECT
public:
	QVariantListWidget(QWidget *parent, QList<QVariant> &values);
	
	//get the values listed in this widget
	QList<QVariant> getList();
	
	//set the values this widget lists
	void setList(QList<QVariant> &values);
	
public slots:
	//add a new row for input at the end
	void addRow();
	
	//remove the last row of the table widget
	void removeRow();
	
private:
	QTableWidget *tableWidget;
	
};

class GetFileNameWidget : public QVBoxLayout
{
	Q_OBJECT
public:
	GetFileNameWidget(QWidget *parent, QString &defaultString, bool getOpenFileName, QString fileExtension = QString("*.*"));
	
	~GetFileNameWidget();
	
	//set the values this widget lists
	QString getFileName();
	
	//set the name to be something else
	void setFileName(QString newName);
	
public slots:
	//add a new row for input at the end
	void launchGetFileNameDialog();
	
private:
	
	//open or save filename
	bool _getOpenFileName;
	
	//label to display the current value of _filename
	QLabel *fileNameLabel;
	
	//button to launch the get filename dialog
	QPushButton *launchFileNameDialogButton;
	
	//the filename colected by the fileName dialog 
	QString _fileName;
	
	//the extension of the files to look for
	QString _fileExtension;
	
};

// This class provide a modal dialog box for asking a generic parameter set
// It can be used by anyone needing for some values in a structred form and having some integrated help
class GenericParamDialog: public QDialog
{
	Q_OBJECT 
public:
  GenericParamDialog(QWidget *p, FilterParameterSet *_curParSet, QString title=QString(), MeshDocument *_meshDocument = 0);
	
	FilterParameterSet *curParSet;
	StdParFrame *stdParFrame;
	
	void createFrame();
	
	public slots:	
	void getAccept();
	void toggleHelp();
	
	//reset the values on the gui back to the ones originally given to the dialog
	void resetValues();
	
private:
	MeshDocument *meshDocument;
	
};


// This is the dialog used to ask parameters for the MeshLab filters.
// This dialog is automatically configurated starting from the parameter asked by a given filter. 
// It can handle dynamic parameters that modify only partially a given mesh. 

class MeshlabStdDialog : public QDockWidget
{
	  Q_OBJECT

public:
  MeshlabStdDialog(QWidget *p);

	void clearValues();
	void createFrame();
	void loadFrameContent(MeshDocument *mdPt=0);

	void showAutoDialog(MeshFilterInterface *mfi, MeshModel *mm, MeshDocument * md, QAction *q, MainWindowInterface *mwi, QWidget *gla=0);
	bool isDynamic();
private slots:
	void applyClick();
	void closeClick();
  void resetValues();
  void toggleHelp();
	void togglePreview();
	void applyDynamic();

protected:
	QFrame *qf;
	StdParFrame *stdParFrame;
	QAction *curAction;
	MeshModelState meshState;
	QCheckBox *previewCB;
public:
	MeshModel *curModel;
	MeshDocument * curMeshDoc;
	MeshFilterInterface *curmfi;
	MainWindowInterface *curmwi;
	QWidget * curgla;
	FilterParameterSet curParSet;
	
};

#endif

