#ifndef MESHLAB_XMLSTDPARDIALOG
#define MESHLAB_XMLSTDPARDIALOG

#include "../common/xmlfilterinfo.h"
#include<QCheckBox>
#include<QRadioButton>
#include<QSpinBox>
#include<QTableWidget>
#include<QComboBox>
#include<QGridLayout>
#include<QDockWidget>

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

class XMLStdParFrame : public QFrame
{
	Q_OBJECT
public:
	XMLStdParFrame(QWidget *p, QWidget *gla=0);
	//void loadFrameContent(RichParameterSet &curParSet,MeshDocument *mdPt = 0);
	//void loadFrameContent(RichParameter* par,MeshDocument *mdPt = 0);

	//// The curParSet that is passed must be 'compatible' with the RichParameterSet that have been used to create the frame.
	//// This function updates the RichParameterSet used to create the frame AND fill also the passed <curParSet>
	//void readValues(RichParameterSet &curParSet);
	//void resetValues(RichParameterSet &curParSet);

	//void toggleHelp();	

	//QVector<MeshLabWidget *> stdfieldwidgets;
	//QVector<QLabel *> helpList;

	QWidget *curr_gla; // used for having a link to the glarea that spawned the parameter asking.
	~XMLStdParFrame();
//signals:

	/*void dynamicFloatChanged(int mask);
	void parameterChanged();*/
};

class MeshLabXMLStdDialog : public QDockWidget
{
	Q_OBJECT

public:
	MeshLabXMLStdDialog(QWidget *p);
	~MeshLabXMLStdDialog();

	void clearValues();
	void createFrame();
	void loadFrameContent(MeshDocument *mdPt=0);

	bool showAutoDialog(MeshLabXMLFilterContainer* mfc,const XMLFilterInfo::MapList& mplist, MeshDocument * md, MainWindowInterface *mwi, QWidget *gla=0);
	bool isDynamic() const;
private slots:
	void applyClick();
	void closeClick();
	void resetValues();
	void toggleHelp();
	void togglePreview();
	void applyDynamic();
	void changeCurrentMesh(int meshInd);

private:
	QFrame *qf;
	XMLStdParFrame *stdParFrame;
	QAction *curAction;
	MeshModelState meshState;
	MeshModelState meshCacheState;
	QCheckBox *previewCB;


	int curmask;
	MeshModel *curModel;
	MeshDocument * curMeshDoc;
	MeshLabXMLFilterContainer* curmfc;
	MainWindowInterface *curmwi;
	XMLFilterInfo::MapList curParMap;
	XMLFilterInfo::MapList prevParMap;
	QWidget * curgla;
	bool validcache;
};

#endif
