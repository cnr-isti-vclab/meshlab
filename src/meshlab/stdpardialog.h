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

    virtual void addWidgetToGridLayout(QGridLayout* lay,const int r) = 0;
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
    //void setEqualSpaceForEachColumn();

    //int row;
    //QGridLayout* gridLay;
    void InitRichParameter(RichParameter* rpar);
};


class BoolWidget : public MeshLabWidget
{
    public:
    //QLabel* lab;
    QCheckBox* cb;
    //QRadioButton* cb;

    BoolWidget(QWidget* p,RichBool* rb);
    ~BoolWidget();

    void addWidgetToGridLayout(QGridLayout* lay,const int r);
    void collectWidgetValue();
    void resetWidgetValue();
    void setWidgetValue(const Value& nv);
};


class LineEditWidget : public MeshLabWidget
{
  Q_OBJECT
protected:
    QLabel* lab;
    QLineEdit* lned;
    QString lastVal;

    private slots:
        void changeChecker();
    signals:
        void lineEditChanged();

public:
    LineEditWidget(QWidget* p,RichParameter* rpar);
    ~LineEditWidget();
    void addWidgetToGridLayout(QGridLayout* lay,const int r);
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

    void addWidgetToGridLayout(QGridLayout* lay,const int r);
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
protected:
    QHBoxLayout* vlay;
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

    void addWidgetToGridLayout(QGridLayout* lay,const int r);
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
  QGridLayout* vlay;
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

    void addWidgetToGridLayout(QGridLayout* lay,const int r);
    void collectWidgetValue();
    void resetWidgetValue();
    void setWidgetValue(const Value& nv);

    public slots:
    void  getPoint();
    void  setValue(QString name, Point3m val);
  void  setShotValue(QString name, Shotm val);
    signals:
    void askViewDir(QString);
    void askViewPos(QString);
    void askSurfacePos(QString);
    void askCameraPos(QString);
	void askTrackballPos(QString);

protected:
    QLineEdit * coordSB[3];
    QComboBox *getPoint3Combo;
    QPushButton *getPoint3Button;
    QLabel* descLab;
    QHBoxLayout* vlay;
};

class Matrix44fWidget : public MeshLabWidget
{
    Q_OBJECT

public:
    Matrix44fWidget(QWidget *p,  RichMatrix44f* rpf, QWidget *gla_curr);
    ~Matrix44fWidget();
    QString paramName;
    vcg::Matrix44f getValue();

    void addWidgetToGridLayout(QGridLayout* lay,const int r);
    void collectWidgetValue();
    void resetWidgetValue();
    void setWidgetValue(const Value& nv);

public slots:
    void setValue(QString name, Matrix44m val);
    void getMatrix();
    void pasteMatrix();
    void invalidateMatrix(const QString& s);
signals:
    void askMeshMatrix(QString);


protected:
    QLineEdit * coordSB[16];
    QPushButton *getPoint3Button;
    QLabel* descLab;
    QGridLayout* lay44;
    QVBoxLayout* vlay;
    vcg::Matrix44f m;
    bool valid;
};

/// Widget to enter a Shot,
/// if at the creation you provide a pointer to a GLArea (the mesh viewing window)
/// the widget has a combo and a button to get shot from different places.

class ShotfWidget : public MeshLabWidget
{
  Q_OBJECT

public:
  ShotfWidget(QWidget *p, RichShotf* rpf, QWidget *gla);
  ~ShotfWidget();
  QString paramName;
  vcg::Shotf getValue();

  void addWidgetToGridLayout(QGridLayout* lay,const int r);
  void collectWidgetValue();
  void resetWidgetValue();
  void setWidgetValue(const Value& nv);

  public slots:
  void  getShot();
  void  setShotValue(QString name, Shotm val);
  signals:
  void askRasterShot(QString);
  void askMeshShot(QString);
  void askViewerShot(QString);

protected:
  vcg::Shotf curShot;
  QLineEdit * shotLE;
  QPushButton *getShotButton;
  QComboBox *getShotCombo;
  QLabel* descLab;
  QHBoxLayout* hlay;
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

    void addWidgetToGridLayout(QGridLayout* lay,const int r);
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
    QHBoxLayout* hlay;
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
    void addWidgetToGridLayout(QGridLayout* lay,const int r);
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
    //int defaultMeshIndex;
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
    void addWidgetToGridLayout(QGridLayout* lay,const int r);
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
    QHBoxLayout* hlay;
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

//class XMLMeshLabWidgetFactory
//{
//protected:
//	static QMap<QString,XMLMeshLabWidgetFactory*> mpfact;
//public:
//	XMLMeshLabWidgetFactory()
//	virtual XMLMeshLabWidget* create(const QMap<QString,QString>& widgetTable) = 0;
//};
//
//class XMLMeshLabWidget : public QObject
//{
//	Q_OBJECT
//public:
//	XMLMeshLabWidget(Expression* expr,QWidget* parent);
//	// bring the values from the Qt widgets to the parameter (e.g. from the checkBox to the parameter).
//	virtual void collectWidgetValue() = 0;
//	virtual void setWidgetExpression(const QString& nwExpStr) = 0;
//	virtual void updateWidget(const QStringList& xmlWidgetTag) = 0;
//	virtual ~XMLMeshLabWidget();
//
//	// called when the user press the 'default' button to reset the parameter values to its default.
//	// It just set the parameter value and then it calls the specialized resetWidgetValue() to update also the widget.
//	//void resetValue();
//	// update the parameter with the current widget values and return it.
//	//Value& getWidgetValue();
//
//signals:
//	void parameterChanged();
//
//protected:
//	int row;
//	QGridLayout* gridLay;
//	QLabel* helpLab;
//	QLineEdit* lExprEditor;
//	QTextEdit* tExprEditor;
//};
//
//class XMLBoolWidget : public XMLMeshLabWidget
//{
//	Q_OBJECT
//public:
//	XMLBoolWidget(QWidget* parent,const QStringList& xmlWidgetTag);
//	~XMLBoolWidget();
//	void resetWidgetValue();
//	// bring the values from the Qt widgets to the parameter (e.g. from the checkBox to the parameter).
//	void collectWidgetValue();
//	void setWidgetValue(const Value& nv);
//	void updateWidget(const QStringList& xmlWidgetTag);
//
//private:
//	QCheckBox* box;
//};
//
//
//struct AbsView
//{
//	QDoubleSpinBox *absSB;
//	QDoubleSpinBox *percSB;
//};
//
//struct DynamicView
//{
//	QSlider *valueSlider;
//	int mask;
//};
//
//class XMLFloatWidget : public XMLMeshLabWidget
//{
//	Q_OBJECT
//public:
//	XMLFloatWidget(QWidget* parent,const QStringList& xmlWidgetTag);
//	~XMLFloatWidget();
//	void resetWidgetValue();
//	// bring the values from the Qt widgets to the parameter (e.g. from the checkBox to the parameter).
//	void collectWidgetValue();
//	void setWidgetValue(const Value& nv);
//	void updateWidget(const QStringList& xmlWidgetTag);
//
//private:
//	AbsView* abs;
//	DynamicView* dyn;
//
//	QLabel* fieldDesc;
//	float minVal;
//	float maxVal;
//
//	float intToFloat(int val);
//	int floatToInt(float val);
//}

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
    void visit(RichMatrix44f&  pd ){  lastCreated = new Matrix44fWidget(par,&pd,reinterpret_cast<StdParFrame*>(par)->gla); }
    void visit(RichPoint3f& pd){lastCreated = new Point3fWidget(par,&pd,reinterpret_cast<StdParFrame*>(par)->gla);};
    void visit(RichShotf& pd){lastCreated = new ShotfWidget(par,&pd,reinterpret_cast<StdParFrame*>(par)->gla);};
    void visit(RichColor& pd){lastCreated = new ColorWidget(par,&pd);};
    void visit(RichAbsPerc& pd){lastCreated = new AbsPercWidget(par,&pd);};
    void visit(RichEnum& pd){lastCreated = new EnumWidget(par,&pd);};
    void visit(RichFloatList& /*pd*/){assert(0);/*TO BE IMPLEMENTED*/ /*lastCreated = new FloatListWidget(par,&pd);*/};
    void visit(RichDynamicFloat& pd){lastCreated = new DynamicFloatWidget(par,&pd);};
    void visit(RichOpenFile& pd){lastCreated = new OpenFileWidget(par,&pd);};
    void visit(RichSaveFile& pd){lastCreated = new SaveFileWidget(par,&pd);};
    void visit(RichMesh& pd){lastCreated = new MeshWidget(par,&pd);};

    ~RichWidgetInterfaceConstructor() {}

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
  void visit(RichShotf& pd);
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

