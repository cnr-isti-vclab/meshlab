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

#include "../common/interfaces.h"
#include<QCheckBox>
#include<QRadioButton>
#include<QSpinBox>
#include<QTableWidget>
#include<QComboBox>
#include<QGridLayout>
#include<QDockWidget>

class MeshLabWidget : public QWidget
{
	Q_OBJECT
public:
	MeshLabWidget(QWidget* p,RichParameter* rpar);
	
    // this one is called by resetValue to reset the values inside the widgets.
    virtual void resetWidgetValue() = 0;
    // bring the values from the Qt widgets to the parameter (e.g. from the checkBox to the parameter).
	virtual void collectWidgetValue() = 0;
	virtual void setWidgetValue(const Value& nv) = 0;
	virtual ~MeshLabWidget();

    // called when the user press the 'default' button to reset the parameter values to its default.
    // It just set the parameter value and then it calls the specialized resetWidgetValue() to update also the widget.
    void resetValue();
    // update the parameter with the current widget values and return it.
	Value& getWidgetValue();

	RichParameter* rp;
	QLabel* helpLab;
signals:
	void parameterChanged();
protected:
	int row;
	QGridLayout* gridLay;
	void InitRichParameter(RichParameter* rpar);
};

class BoolWidget : public MeshLabWidget 
{
	//QLabel* lab;
	QCheckBox* cb;
	//QRadioButton* cb;
public:
	BoolWidget(QWidget* p,RichBool* rb);
	~BoolWidget();
	void collectWidgetValue();
	void resetWidgetValue();
	void setWidgetValue(const Value& nv);
};


class LineEditWidget : public MeshLabWidget
{
protected:
	QLabel* lab;
	QLineEdit* lned;
public:

	LineEditWidget(QWidget* p,RichParameter* rpar);
	~LineEditWidget();
	virtual void collectWidgetValue() = 0;
	virtual void resetWidgetValue() = 0;
	virtual void setWidgetValue(const Value& nv) = 0;
};



class IntWidget : public LineEditWidget
{
public:
	IntWidget(QWidget* p,RichInt* rpar);
	~IntWidget(){}
	void collectWidgetValue();
	void resetWidgetValue();
	void setWidgetValue(const Value& nv);
};



class FloatWidget : public  LineEditWidget
{
public:
	FloatWidget(QWidget* p,RichFloat* rpar);
	~FloatWidget(){}
	void collectWidgetValue();
	void resetWidgetValue();
	void setWidgetValue(const Value& nv);
};



class StringWidget  : public  LineEditWidget
{
public:
	StringWidget(QWidget* p,RichString* rpar);
	~StringWidget(){}
	void collectWidgetValue();
	void resetWidgetValue();
	void setWidgetValue(const Value& nv);
};

//class Matrix44fWidget : public  MeshLabWidget
//{
//public:
//	Matrix44fWidget(QWidget* p,RichMatrix44f* rpar);
//
//	void collectWidgetValue();
//	void resetWidgetValue();
//};

/*
class FloatListWidget : public MeshLabWidget
{
public:
	FloatListWidget(QWidget* p,RichFloatList* rpar);

	void collectWidgetValue();
	void resetWidgetValue();
};
*/

/*
class OpenFileWidget : public MeshLabWidget
{
public:
	OpenFileWidget(QWidget* p,RichOpenFile* rpar);

	void collectWidgetValue();
	void resetWidgetValue();
};
*/


/// Widget to enter a color.
// public QHBoxLayout,
class ColorWidget : public MeshLabWidget
{
	  Q_OBJECT
		
		QPushButton* colorButton;
		QLabel* colorLabel;
		QLabel* descLabel;
		QColor pickcol;

	public:
	ColorWidget(QWidget *p, RichColor* newColor);
	~ColorWidget();

		void collectWidgetValue();
		void resetWidgetValue();
		void setWidgetValue(const Value& nv);
		void initWidgetValue();

private:
	void  updateColorInfo(const ColorValue& newColor);

	signals:
		void dialogParamChanged();
	private slots:
		void pickColor(); 
};



/// Widget to enter a value as a percentage or as an absolute value. 
/// You have to specify the default value and the range of the possible values.
/// The default value is expressed in ABSolute units (e.g. it should be in the min..max range.


//public QGridLayout
class AbsPercWidget : public MeshLabWidget
{
	  Q_OBJECT

public:
  AbsPercWidget(QWidget *p,RichAbsPerc* rabs);
  ~AbsPercWidget();
	
	void collectWidgetValue();
	void resetWidgetValue();
	void setWidgetValue(const Value& nv);

private:
	void  setValue(float val, float minV, float maxV);

public slots:

	void on_absSB_valueChanged(double newv); 
	void on_percSB_valueChanged(double newv);
signals:
	void dialogParamChanged();

protected:
  QDoubleSpinBox *absSB;
  QDoubleSpinBox *percSB;
	QLabel* fieldDesc;
  float m_min;
  float m_max;
};



/// Widget to enter a Point3f, 
/// if at the creation you provide a pointer to a GLArea (the mesh viewing window) 
/// the widget exposes a button for getting the current view directiont 

//public QHBoxLayout,
class Point3fWidget : public MeshLabWidget
{
	Q_OBJECT
	
public:
  Point3fWidget(QWidget *p, RichPoint3f* rpf, QWidget *gla);
  ~Point3fWidget();
	QString paramName;
	vcg::Point3f getValue();

	void collectWidgetValue();
	void resetWidgetValue();
	void setWidgetValue(const Value& nv);

	public slots:
	void  getPoint();
	void  setValue(QString name, vcg::Point3f val);	
	signals:
	void askViewDir(QString);
	void askViewPos(QString);
	void askSurfacePos(QString);
	void askCameraPos(QString);

protected:
	QLineEdit * coordSB[3];
	QComboBox *getPoint3Combo;
	QPushButton *getPoint3Button;
	QLabel* descLab;
};



//public QGridLayout,
class DynamicFloatWidget : public MeshLabWidget
{
	Q_OBJECT
	
public:
  DynamicFloatWidget(QWidget *p, RichDynamicFloat* rdf);
  ~DynamicFloatWidget();
	
  float getValue();
	void  setValue(float val, float minV, float maxV);

	void collectWidgetValue();
	void resetWidgetValue();
	void setWidgetValue(const Value& nv);
	
	public slots:
		void setValue(int newv);
		void setValue();
		void setValue(float newValue);

	signals:
		//void valueChanged(int mask);
		void dialogParamChanged();
	
protected:
	QLineEdit *valueLE;
	QSlider   *valueSlider;
	QLabel* fieldDesc; 
  float minVal;
  float maxVal;
	int mask;
private :
	float intToFloat(int val);  
	int floatToInt(float val);
};



class ComboWidget : public MeshLabWidget
{
	Q_OBJECT
protected:	
	QComboBox *enumCombo;
	QLabel *enumLabel;
public:
	ComboWidget(QWidget *p, RichParameter* rpar);
	~ComboWidget();
	void Init(QWidget *p,int newEnum, QStringList values);
	virtual void collectWidgetValue() = 0;
	virtual void resetWidgetValue() = 0;
	virtual void setWidgetValue(const Value& nv) = 0;

	int getIndex();
	void  setIndex(int newEnum);	

signals:
	void dialogParamChanged();
};

/// Widget to select an entry from a list

//public QHBoxLayout
class EnumWidget : public ComboWidget
{
	Q_OBJECT

public:	
	EnumWidget(QWidget *p, RichEnum* rpar);
	~EnumWidget(){};

	void collectWidgetValue();
	void resetWidgetValue();
	void setWidgetValue(const Value& nv);

	//returns the number of items in the list 
	int getSize();
};


/// Widget to select a Layer the current one
class MeshWidget : public ComboWidget
{
private:
	MeshDocument *md;
	int defaultMeshIndex;
public:
	MeshWidget(QWidget *p, RichMesh* defaultMesh);
	~MeshWidget(){};
	void collectWidgetValue();
	void resetWidgetValue();
	void setWidgetValue(const Value& nv);

	MeshModel * getMesh();
	void setMesh(MeshModel * newMesh);
};

class IOFileWidget : public MeshLabWidget
{
	Q_OBJECT

protected:
	IOFileWidget(QWidget* p,RichParameter* rpar);
	~IOFileWidget();

	void  updateFileName(const FileValue& file);

public:
	void collectWidgetValue();
	void resetWidgetValue();
	void setWidgetValue(const Value& nv);

protected slots:
	virtual void selectFile() = 0;

signals:
	void dialogParamChanged();


protected:
	QLineEdit* filename;
	QPushButton* browse;
	QString fl;
	QLabel* descLab;
};

class SaveFileWidget : public IOFileWidget
{
	Q_OBJECT
public:
	SaveFileWidget(QWidget* p,RichSaveFile* rpar);
	~SaveFileWidget(); 

protected slots:
	void selectFile();

};

class OpenFileWidget : public IOFileWidget
{
	Q_OBJECT
public:
	OpenFileWidget(QWidget *p, RichOpenFile* rdf);
	~OpenFileWidget();

	/*void collectWidgetValue();
	void resetWidgetValue();
	void setWidgetValue(const Value& nv);*/

protected slots:
	void selectFile();
};
/*
class QVariantListWidget : public MeshLabWidget
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

		void collectWidgetValue();
		void resetWidgetValue();

private:
	QTableWidget *tableWidget;

};
*/


/*
//public QVBoxLayout
class GetFileNameWidget : public MeshLabWidget
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

		void collectWidgetValue();
		void resetWidgetValue();
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
*/

/*---------------------------------*/

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

	void loadFrameContent(RichParameterSet &curParSet,MeshDocument *mdPt = 0);
	void loadFrameContent(RichParameter* par,MeshDocument *mdPt = 0);

    // The curParSet that is passed must be 'compatible' with the RichParameterSet that have been used to create the frame.
    // This function updates the RichParameterSet used to create the frame AND fill also the passed <curParSet>
	void readValues(RichParameterSet &curParSet);
	void resetValues(RichParameterSet &curParSet);

	void toggleHelp();	
	
	QVector<MeshLabWidget *> stdfieldwidgets;
	QVector<QLabel *> helpList;

	QWidget *gla; // used for having a link to the glarea that spawned the parameter asking.
	~StdParFrame();
signals:

		void dynamicFloatChanged(int mask);
		void parameterChanged();
};



// This class provide a modal dialog box for asking a generic parameter set
// It can be used by anyone needing for some values in a structred form and having some integrated help
class GenericParamDialog: public QDialog
{
	Q_OBJECT 
public:
  GenericParamDialog(QWidget *p, RichParameterSet *_curParSet, QString title=QString(), MeshDocument *_meshDocument = 0);
	~GenericParamDialog();

	RichParameterSet *curParSet;
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

class MainWindow;
class MeshlabStdDialog : public QDockWidget
{
	  Q_OBJECT

public:
  MeshlabStdDialog(QWidget *p);
	~MeshlabStdDialog();

	void clearValues();
	void createFrame();
	void loadFrameContent(MeshDocument *mdPt=0);

    bool showAutoDialog(MeshFilterInterface *mfi, MeshModel *mm, MeshDocument * md, QAction *q, MainWindowInterface *mwi, QWidget *gla=0);
	bool isDynamic();

private slots:
	void applyClick();
	void closeClick();
  void resetValues();
  void toggleHelp();
	void togglePreview();
	void applyDynamic();
    void changeCurrentMesh(int meshInd);

public:
	QFrame *qf;
	StdParFrame *stdParFrame;
	QAction *curAction;
	MeshModelState meshState;
	MeshModelState meshCacheState;
	QCheckBox *previewCB;

	void closeEvent ( QCloseEvent * event ); 

	int curmask;
	MeshModel *curModel;
	MeshDocument * curMeshDoc;
	MeshFilterInterface *curmfi;
    MainWindowInterface *curmwi;
	QWidget * curgla;
	RichParameterSet curParSet;
	RichParameterSet prevParSet;
	bool validcache;
};



//QWidget* parent parameter says to the class who will destroy the MeshLabWidget object that it had created
//RichWidgetConstructor shouldn't destroy anything

class RichWidgetInterfaceConstructor : public Visitor
{
public:
        RichWidgetInterfaceConstructor(QWidget* parent):lastCreated(NULL),par(parent){}

	void visit(RichBool& pd) {lastCreated = new BoolWidget(par,&pd);};
	void visit(RichInt& pd) {lastCreated = new IntWidget(par,&pd);};
	void visit(RichFloat& pd){lastCreated = new FloatWidget(par,&pd);};
	void visit(RichString& pd){lastCreated = new StringWidget(par,&pd);};
	void visit(RichMatrix44f& /*pd*/){assert(0);/*TO BE IMPLEMENTED*/ /*lastCreated = new Matrix44fWidget(par,&pd);*/};
	void visit(RichPoint3f& pd){lastCreated = new Point3fWidget(par,&pd,reinterpret_cast<StdParFrame*>(par)->gla);};
	void visit(RichColor& pd){lastCreated = new ColorWidget(par,&pd);};
	void visit(RichAbsPerc& pd){lastCreated = new AbsPercWidget(par,&pd);};
	void visit(RichEnum& pd){lastCreated = new EnumWidget(par,&pd);};
	void visit(RichFloatList& /*pd*/){assert(0);/*TO BE IMPLEMENTED*/ /*lastCreated = new FloatListWidget(par,&pd);*/};
	void visit(RichDynamicFloat& pd){lastCreated = new DynamicFloatWidget(par,&pd);};
	void visit(RichOpenFile& pd){lastCreated = new OpenFileWidget(par,&pd);};
	void visit(RichSaveFile& pd){lastCreated = new SaveFileWidget(par,&pd);};
	void visit(RichMesh& pd){lastCreated = new MeshWidget(par,&pd);};

	~RichWidgetInterfaceConstructor() {}

	void setParentWidget(QWidget* parent) {par = parent;}
	MeshLabWidget* lastCreated;
private:
	QWidget* par;

};

class RichParameterToQTableWidgetItemConstructor : public Visitor
{
public:
	RichParameterToQTableWidgetItemConstructor(/*QListWidget* widlst*/):/*lst(widlst),*/lastCreated(NULL){}

	void visit(RichBool& pd);
	void visit(RichInt& pd);
	void visit(RichFloat& pd);
	void visit(RichString& pd);
	void visit(RichMatrix44f& /*pd*/){assert(0);};
	void visit(RichPoint3f& pd);
	void visit(RichColor& pd);
	void visit(RichAbsPerc& pd);
	void visit(RichEnum& pd);
	void visit(RichFloatList& /*pd*/){assert(0);};
	void visit(RichDynamicFloat& pd);
	void visit(RichOpenFile& pd);
	void visit(RichSaveFile& /*pd*/){assert(0);};
	void visit(RichMesh& /*pd*/){assert(0);};

	/*QListWidget* lst;*/
	QTableWidgetItem* lastCreated;
};



#endif

