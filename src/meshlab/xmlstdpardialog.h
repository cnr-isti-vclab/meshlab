#ifndef MESHLAB_XMLSTDPARDIALOG
#define MESHLAB_XMLSTDPARDIALOG

#include "../common/xmlfilterinfo.h"
#include<QCheckBox>
#include<QPushButton>
#include<QRadioButton>
#include<QSpinBox>
#include<QTableWidget>
#include<QComboBox>
#include<QHBoxLayout>
#include<QGridLayout>
#include<QDockWidget>
#include<QTextEdit>

//
class XMLMeshLabWidget : public QWidget
{
	Q_OBJECT
public:
	XMLMeshLabWidget(const XMLFilterInfo::XMLMap& xmlWidgetTag,QWidget* parent);
	// bring the values from the Qt widgets to the parameter (e.g. from the checkBox to the parameter).
	virtual void collectWidgetValue() = 0;
	virtual void setWidgetExpression(const QString& nwExpStr) = 0;
	virtual void updateWidget(const XMLFilterInfo::XMLMap& xmlWidgetTag) = 0;
	void setVisibility(const bool vis);
	virtual void updateVisibility(const bool vis) = 0;

	virtual ~XMLMeshLabWidget() {};
	inline QLabel* helpLabel() {return helpLab;} 

	bool isImportant;
	// called when the user press the 'default' button to reset the parameter values to its default.
	// It just set the parameter value and then it calls the specialized resetWidgetValue() to update also the widget.
	//void resetValue();
	// update the parameter with the current widget values and return it.
	//Value& getWidgetValue();

signals:
	void parameterChanged();
	void widgetEvaluateExpression(const Expression& exp,Value** res);

protected:
	int row;
	QGridLayout* gridLay;
	QLabel* helpLab;
	QLineEdit* lExprEditor;
	QTextEdit* tExprEditor;
	
};

class XMLMeshLabWidgetFactory 
{
public:	
	static XMLMeshLabWidget* create(const XMLFilterInfo::XMLMap& widgetTable,QWidget* parent);
};

//
class XMLCheckBoxWidget : public XMLMeshLabWidget
{
	Q_OBJECT
public:
	XMLCheckBoxWidget(const XMLFilterInfo::XMLMap& xmlWidgetTag,QWidget* parent);
	~XMLCheckBoxWidget();
	void resetWidgetValue();
	// bring the values from the Qt widgets to the parameter (e.g. from the checkBox to the parameter).
	void collectWidgetValue();
	void setWidgetExpression(const QString& nv);
	void updateWidget(const XMLFilterInfo::XMLMap& xmlWidgetTag);
	void updateVisibility(const bool vis);

private:

	QCheckBox* cb; 
};
//
//

class XMLEditWidget : public XMLMeshLabWidget
{
	Q_OBJECT
public:
	XMLEditWidget(const XMLFilterInfo::XMLMap& xmlWidgetTag,QWidget* parent);
	~XMLEditWidget();
	void resetWidgetValue();
	// bring the values from the Qt widgets to the parameter (e.g. from the checkBox to the parameter).
	void collectWidgetValue();
	void setWidgetExpression(const QString& nv);
	void updateWidget(const XMLFilterInfo::XMLMap& xmlWidgetTag);
	void updateVisibility(const bool vis);

private:
	QLabel* fieldDesc; 
	QLineEdit* lineEdit;
};

class XMLAbsWidget : public XMLMeshLabWidget
{	
public:
	XMLAbsWidget(const XMLFilterInfo::XMLMap& xmlWidgetTag,QWidget* parent);
	~XMLAbsWidget();
	
	void resetWidgetValue();
	// bring the values from the Qt widgets to the parameter (e.g. from the checkBox to the parameter).
	void collectWidgetValue();
	void setWidgetExpression(const QString& nv);
	void updateWidget(const XMLFilterInfo::XMLMap& xmlWidgetTag);
	void updateVisibility(const bool vis);

private:
	QLabel* fieldDesc; 
	Value* minVal;
	Value* maxVal;
	QLabel *absLab;
	QLabel *percLab;

	QDoubleSpinBox *absSB;
	QDoubleSpinBox *percSB;
};

//class XMLSliderWidget : public XMLFloatWidget
//{
//private:
//	QSlider *valueSlider;
//	int mask;
//};

class XMLStdParFrame : public QFrame
{
	Q_OBJECT
public:
	XMLStdParFrame(QWidget *p, QWidget *gla=0);
	void loadFrameContent(const XMLFilterInfo::XMLMapList& parMap);
	void extendedView(bool ext,bool help);
	//void loadFrameContent(RichParameter* par,MeshDocument *mdPt = 0);

	//// The curParSet that is passed must be 'compatible' with the RichParameterSet that have been used to create the frame.
	//// This function updates the RichParameterSet used to create the frame AND fill also the passed <curParSet>
	//void readValues(RichParameterSet &curParSet);
	//void resetValues(RichParameterSet &curParSet);

	void toggleHelp(bool help);	

	QVector<XMLMeshLabWidget*> xmlfieldwidgets;
	QVector<QLabel *> helpList;

	QWidget *curr_gla; // used for having a link to the glarea that spawned the parameter asking.
	~XMLStdParFrame();
signals:
	void frameEvaluateExpression(const Expression& exp,Value** res);
	
private:
	QGridLayout * vLayout;
	bool extended;

	/*void dynamicFloatChanged(int mask);
	void parameterChanged();*/
};

class ExpandButtonWidget : public QWidget
{
	Q_OBJECT
public:
	ExpandButtonWidget(QWidget* parent);	
	~ExpandButtonWidget();

private slots:
	void changeIcon();
signals:
	void expandView(bool exp);
private:
	QString arrow;
	const QChar up;
	const QChar down;
	QPushButton* exp;
	QHBoxLayout* hlay;
	bool isExpanded;
};

class MeshLabXMLStdDialog : public QDockWidget
{
	Q_OBJECT

public:
	MeshLabXMLStdDialog(QWidget *p);
	~MeshLabXMLStdDialog();

	void clearValues();
	void createFrame();
	void loadFrameContent();

	bool showAutoDialog(MeshLabXMLFilterContainer* mfc, MeshDocument * md, MainWindowInterface *mwi, QWidget *gla=0);
	bool isDynamic() const;
signals:
	void dialogEvaluateExpression(const Expression& exp,Value** res);
	//void expandView(bool exp);

private slots:
	void applyClick();
	void closeClick();
	void resetValues();
	void toggleHelp();
	void togglePreview();
	void applyDynamic();
	void changeCurrentMesh(int meshInd);
	void extendedView(bool ext);

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
	XMLFilterInfo::XMLMapList curParMap;
	XMLFilterInfo::XMLMapList prevParMap;
	QWidget * curgla;
	bool validcache;
	bool showHelp;
};

#endif
