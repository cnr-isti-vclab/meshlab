#ifndef MESHLAB_XMLSTDPARDIALOG
#define MESHLAB_XMLSTDPARDIALOG

#include "../common/xmlfilterinfo.h"
#include "../common/scriptinterface.h"
#include "../common/interfaces.h"
#include "../common/mlexception.h"
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
#include<QLabel>
#include "qstyleoption.h"
#include "additionalgui.h"

class XMLWidgetException : public MeshLabException
{
public:
	XMLWidgetException(const QString& text)
		:MeshLabException(QString("XML Widget Error: ") + text){}

	~XMLWidgetException() throw() {}
};

//
class XMLMeshLabWidget : public QWidget
{
	Q_OBJECT
public:
	//WARNING! This constructor could rise up a set of MeshLabExceptions! this does it mean that the object COULD BE NOT correctly constructed!
	XMLMeshLabWidget(const MLXMLPluginInfo::XMLMap& xmlWidgetTag,EnvWrap& envir,QWidget* parent);
	// bring the values from the Qt widgets to the parameter (e.g. from the checkBox to the parameter).
	//virtual void collectWidgetValue() = 0;
	//void reset();
	virtual void set(const QString& nwExpStr) = 0;
	virtual QString getWidgetExpression() = 0;
	//virtual void updateWidget(const XMLFilterInfo::XMLMap& xmlWidgetTag) = 0;
	void setVisibility(const bool vis);
	virtual void updateVisibility(const bool vis) = 0;

        virtual ~XMLMeshLabWidget() {}
	inline QLabel* helpLabel() {return helpLab;} 

	bool isImportant;
	// called when the user press the 'default' button to reset the parameter values to its default.
	// It just set the parameter value and then it calls the specialized resetWidgetValue() to update also the widget.
	//void resetValue();
	// update the parameter with the current widget values and return it.
	//Value& getWidgetValue();

signals:
	void parameterChanged();
	//void widgetEvaluateExpression(const Expression& exp,Value** res);
	//void insertParamInEnv(const QString& paramName,Expression* paramExp);

protected:
	int row;
	QGridLayout* gridLay;
	QLabel* helpLab;
	EnvWrap env;
	/*const XMLFilterInfo::XMLMap& map;*/
	//QLineEdit* lExprEditor;
	//QTextEdit* tExprEditor;
};

class XMLMeshLabWidgetFactory 
{
public:	
	//WARNING! this function call constructors that could rise up a set of MeshLabExceptions but it is not able to manage it, so let the exceptions floating up!
	//IN ANY CASE the callee MUST check if the returned value is not NULL.
	static XMLMeshLabWidget* create(const MLXMLPluginInfo::XMLMap& widgetTable,EnvWrap& env,MeshDocument* md,QWidget* parent);
};

//
class XMLCheckBoxWidget : public XMLMeshLabWidget
{
	Q_OBJECT
public:
	XMLCheckBoxWidget(const MLXMLPluginInfo::XMLMap& xmlWidgetTag,EnvWrap& envir,QWidget* parent);
	~XMLCheckBoxWidget();
	
	// bring the values from the Qt widgets to the parameter (e.g. from the checkBox to the parameter).
	void set(const QString& nwExpStr);
	//void updateWidget(const XMLFilterInfo::XMLMap& xmlWidgetTag);
	void updateVisibility(const bool vis);
	QString getWidgetExpression();

private:

	//WHY WE NEED THIS FUNCTION?
	//IN C++ IS NOT HEALTHY AT ALL TO CALL A VIRTUAL FUNCTION FROM OBJECT CONSTRUCTOR.
	//SO I CANNOT CALL DIRECTLY THE updateVisibility FUNCTION. 
	//THIS THING HAS AS CONSEQUENCE THAT I HAVE TO PASTE AND COPY THE updateVisibility CODE INSIDE THE CONSTRUCTOR.
	//TO AVOID THIS FOR EACH WIDGET WE ADD A setVisibility FUNCTION (OBVIOUSLY NOT VIRTUAL) WITH THE CODE THAT WE SHOULD PUT INSIDE THE
	//updateVisibility. 
	//THE CODE OF VIRTUAL FUNCTION updateVisibility WILL BE ONLY A CALL TO THE NON VIRTUAL FUNCTION setVisibility.
	void setVisibility(const bool vis);
	QCheckBox* cb; 
};
//
//

class XMLEditWidget : public XMLMeshLabWidget
{
	Q_OBJECT
public:
	XMLEditWidget(const MLXMLPluginInfo::XMLMap& xmlWidgetTag,EnvWrap& envir,QWidget* parent);
	~XMLEditWidget();

	void set(const QString& nwExpStr);
	//void updateWidget(const XMLFilterInfo::XMLMap& xmlWidgetTag);
	void updateVisibility(const bool vis);
	QString getWidgetExpression();
private slots:
	void tooltipEvaluation();
private:
	//WHY WE NEED THIS FUNCTION?
	//IN C++ IS NOT HEALTHY AT ALL TO CALL A VIRTUAL FUNCTION FROM OBJECT CONSTRUCTOR.
	//SO I CANNOT CALL DIRECTLY THE updateVisibility FUNCTION. 
	//THIS THING HAS AS CONSEQUENCE THAT I HAVE TO PASTE AND COPY THE updateVisibility CODE INSIDE THE CONSTRUCTOR.
	//TO AVOID THIS FOR EACH WIDGET WE ADD A setVisibility FUNCTION (OBVIOUSLY NOT VIRTUAL) WITH THE CODE THAT WE SHOULD PUT INSIDE THE
	//updateVisibility. 
	//THE CODE OF VIRTUAL FUNCTION updateVisibility WILL BE ONLY A CALL TO THE NON VIRTUAL FUNCTION setVisibility.
	void setVisibility(const bool vis);
	QLabel* fieldDesc; 
	QLineEdit* lineEdit;
};

class XMLAbsWidget : public XMLMeshLabWidget
{	
	Q_OBJECT
public:
	XMLAbsWidget(const MLXMLPluginInfo::XMLMap& xmlWidgetTag,EnvWrap& envir,QWidget* parent);
	~XMLAbsWidget();

	void set(const QString& nwExpStr);
	//void updateWidget(const XMLFilterInfo::XMLMap& xmlWidgetTag);
	void updateVisibility(const bool vis);
	QString getWidgetExpression();

public slots:
	void on_absSB_valueChanged(double newv);
	void on_percSB_valueChanged(double newv);

signals:
	void dialogParamChanged();

private:
	//WHY WE NEED THIS FUNCTION?
	//IN C++ IS NOT HEALTHY AT ALL TO CALL A VIRTUAL FUNCTION FROM OBJECT CONSTRUCTOR.
	//SO I CANNOT CALL DIRECTLY THE updateVisibility FUNCTION. 
	//THIS THING HAS AS CONSEQUENCE THAT I HAVE TO PASTE AND COPY THE updateVisibility CODE INSIDE THE CONSTRUCTOR.
	//TO AVOID THIS FOR EACH WIDGET WE ADD A setVisibility FUNCTION (OBVIOUSLY NOT VIRTUAL) WITH THE CODE THAT WE SHOULD PUT INSIDE THE
	//updateVisibility. 
	//THE CODE OF VIRTUAL FUNCTION updateVisibility WILL BE ONLY A CALL TO THE NON VIRTUAL FUNCTION setVisibility.
	void setVisibility(const bool vis);
	QLabel* fieldDesc; 
	float m_min;
	float m_max;
	QLabel *absLab;
	QLabel *percLab;

	QDoubleSpinBox *absSB;
	QDoubleSpinBox *percSB;
};

class XMLVec3Widget : public XMLMeshLabWidget
{
	Q_OBJECT
public:
	XMLVec3Widget(const MLXMLPluginInfo::XMLMap& xmlWidgetTag,EnvWrap& envir,QWidget* p);
	~XMLVec3Widget();
	QString paramName;

	void set(const QString& nwExpStr);
	//void updateWidget(const XMLFilterInfo::XMLMap& xmlWidgetTag);
	void updateVisibility(const bool vis);
	QString getWidgetExpression();

	public slots:
		void  getPoint();
		void  setPoint( const QString& name,const vcg::Point3f& p );	
		void  setShot(const QString& name,const vcg::Shotf& val );
signals:
		void askViewDir(QString);
		void askViewPos(QString);
		void askSurfacePos(QString);
		void askCameraPos(QString);
		void setExp(const QString& name,const QString& exp );
protected:
	QLineEdit * coordSB[3];
	QComboBox *getPoint3Combo;
	QPushButton *getPoint3Button;
	QLabel* descLab;
	QWidget* curr_gla;
private:
	//WHY WE NEED THIS FUNCTION?
	//IN C++ IS NOT HEALTHY AT ALL TO CALL A VIRTUAL FUNCTION FROM OBJECT CONSTRUCTOR.
	//SO I CANNOT CALL DIRECTLY THE updateVisibility FUNCTION. 
	//THIS THING HAS AS CONSEQUENCE THAT I HAVE TO PASTE AND COPY THE updateVisibility CODE INSIDE THE CONSTRUCTOR.
	//TO AVOID THIS FOR EACH WIDGET WE ADD A setVisibility FUNCTION (OBVIOUSLY NOT VIRTUAL) WITH THE CODE THAT WE SHOULD PUT INSIDE THE
	//updateVisibility. 
	//THE CODE OF VIRTUAL FUNCTION updateVisibility WILL BE ONLY A CALL TO THE NON VIRTUAL FUNCTION setVisibility.
	void setVisibility(const bool vis);
};

class XMLColorWidget : public XMLMeshLabWidget
{
	Q_OBJECT

	QPushButton* colorButton;
	QLabel* colorLabel;
	QLabel* descLabel;
	QColor pickcol;

public:
	XMLColorWidget(const MLXMLPluginInfo::XMLMap& xmlWidgetTag,EnvWrap& envir,QWidget* p);
	~XMLColorWidget();

	void set(const QString& nwExpStr);
	//void updateWidget(const XMLFilterInfo::XMLMap& xmlWidgetTag);
	void updateVisibility(const bool vis);
	QString getWidgetExpression();
private:
	//WHY WE NEED THIS FUNCTION?
	//IN C++ IS NOT HEALTHY AT ALL TO CALL A VIRTUAL FUNCTION FROM OBJECT CONSTRUCTOR.
	//SO I CANNOT CALL DIRECTLY THE updateVisibility FUNCTION. 
	//THIS THING HAS AS CONSEQUENCE THAT I HAVE TO PASTE AND COPY THE updateVisibility CODE INSIDE THE CONSTRUCTOR.
	//TO AVOID THIS FOR EACH WIDGET WE ADD A setVisibility FUNCTION (OBVIOUSLY NOT VIRTUAL) WITH THE CODE THAT WE SHOULD PUT INSIDE THE
	//updateVisibility. 
	//THE CODE OF VIRTUAL FUNCTION updateVisibility WILL BE ONLY A CALL TO THE NON VIRTUAL FUNCTION setVisibility.
	void setVisibility(const bool vis);
	void  updateColorInfo(const QColor& col);

signals:
	void dialogParamChanged();
	private slots:
		void pickColor(); 
};

class XMLSliderWidget : public XMLMeshLabWidget
{
	Q_OBJECT
signals:
	void dialogParamChanged();
public:
	XMLSliderWidget(const MLXMLPluginInfo::XMLMap& xmlWidgetTag,EnvWrap& envir,QWidget* p);
	~XMLSliderWidget();
	void set(const QString& nwExpStr);
	void updateVisibility(const bool vis);
	QString getWidgetExpression();

public slots:
	void setValue(int newv);
	void setValue();
	void setValue(float newValue);

private:
	//WHY WE NEED THIS FUNCTION?
	//IN C++ IS NOT HEALTHY AT ALL TO CALL A VIRTUAL FUNCTION FROM OBJECT CONSTRUCTOR.
	//SO I CANNOT CALL DIRECTLY THE updateVisibility FUNCTION. 
	//THIS THING HAS AS CONSEQUENCE THAT I HAVE TO PASTE AND COPY THE updateVisibility CODE INSIDE THE CONSTRUCTOR.
	//TO AVOID THIS FOR EACH WIDGET WE ADD A setVisibility FUNCTION (OBVIOUSLY NOT VIRTUAL) WITH THE CODE THAT WE SHOULD PUT INSIDE THE
	//updateVisibility. 
	//THE CODE OF VIRTUAL FUNCTION updateVisibility WILL BE ONLY A CALL TO THE NON VIRTUAL FUNCTION setVisibility.
	void setVisibility(const bool vis);
	float intToFloat(int val);  
	int floatToInt(float val);
	QLineEdit *valueLE;
	QSlider   *valueSlider;
	QLabel* fieldDesc; 
	float minVal;
	float maxVal;
};

class XMLComboWidget : public XMLMeshLabWidget
{
	Q_OBJECT
protected:	
	QComboBox *enumCombo;
	QLabel *enumLabel;
public:
	XMLComboWidget(const MLXMLPluginInfo::XMLMap& xmlWidgetTag,EnvWrap& envir,QWidget* p);
	~XMLComboWidget();
	void updateVisibility(const bool vis);
	virtual QString getWidgetExpression();
	virtual void set(const QString &) {}

private:
	//WHY WE NEED THIS FUNCTION?
	//IN C++ IS NOT HEALTHY AT ALL TO CALL A VIRTUAL FUNCTION FROM OBJECT CONSTRUCTOR.
	//SO I CANNOT CALL DIRECTLY THE updateVisibility FUNCTION. 
	//THIS THING HAS AS CONSEQUENCE THAT I HAVE TO PASTE AND COPY THE updateVisibility CODE INSIDE THE CONSTRUCTOR.
	//TO AVOID THIS FOR EACH WIDGET WE ADD A setVisibility FUNCTION (OBVIOUSLY NOT VIRTUAL) WITH THE CODE THAT WE SHOULD PUT INSIDE THE
	//updateVisibility. 
	//THE CODE OF VIRTUAL FUNCTION updateVisibility WILL BE ONLY A CALL TO THE NON VIRTUAL FUNCTION setVisibility.
	void setVisibility(const bool vis);

signals:
	void dialogParamChanged();
};

class XMLEnumWidget : public XMLComboWidget
{
	Q_OBJECT

public:	
	XMLEnumWidget(const MLXMLPluginInfo::XMLMap& xmlWidgetTag,EnvWrap& envir,QWidget* p);
	~XMLEnumWidget(){};

	QString getWidgetExpression();
};

class XMLMeshWidget : public XMLEnumWidget
{
	Q_OBJECT

public:
	XMLMeshWidget(MeshDocument* mdoc,const MLXMLPluginInfo::XMLMap& xmlWidgetTag,EnvWrap& envir,QWidget* p);
	~XMLMeshWidget(){};

private:
	MeshDocument* md;
};

class XMLShotWidget : public XMLMeshLabWidget
{
	Q_OBJECT

public:
	XMLShotWidget(const MLXMLPluginInfo::XMLMap& xmlWidgetTag,EnvWrap& envir,QWidget* p);
	~XMLShotWidget(){};
	void updateVisibility(const bool vis);
	void set(const QString &) {}

	QString paramName;

	QString getWidgetExpression();

public slots:
	void  getShot();
	void  setShotValue(QString name, vcg::Shotf val);
signals:
	void askRasterShot(QString);
	void askMeshShot(QString);
	void askViewerShot(QString);

protected:
	QWidget *gla_curr;
	vcg::Shotf curShot;
	//QLineEdit * shotLE;
	QComboBox *getShotCombo;
	QLabel* descLab;
	QPushButton* getShotButton;
private:
	//WHY WE NEED THIS FUNCTION?
	//IN C++ IS NOT HEALTHY AT ALL TO CALL A VIRTUAL FUNCTION FROM OBJECT CONSTRUCTOR.
	//SO I CANNOT CALL DIRECTLY THE updateVisibility FUNCTION. 
	//THIS THING HAS AS CONSEQUENCE THAT I HAVE TO PASTE AND COPY THE updateVisibility CODE INSIDE THE CONSTRUCTOR.
	//TO AVOID THIS FOR EACH WIDGET WE ADD A setVisibility FUNCTION (OBVIOUSLY NOT VIRTUAL) WITH THE CODE THAT WE SHOULD PUT INSIDE THE
	//updateVisibility. 
	//THE CODE OF VIRTUAL FUNCTION updateVisibility WILL BE ONLY A CALL TO THE NON VIRTUAL FUNCTION setVisibility.
	void setVisibility(const bool vis);
};

//class XMLShotfWidget : public MeshLabWidget
//{
//	Q_OBJECT
//
//public:
//	ShotfWidget(QWidget *p, RichShotf* rpf, QWidget *gla);
//	~ShotfWidget();
//	QString paramName;
//	vcg::Shotf getValue();
//
//	void collectWidgetValue();
//	void resetWidgetValue();
//	void setWidgetValue(const Value& nv);
//
//	public slots:
//		void  getShot();
//		void  setShotValue(QString name, vcg::Shotf val);
//signals:
//		void askRasterShot(QString);
//		void askMeshShot(QString);
//		void askViewerShot(QString);
//
//protected:
//	vcg::Shotf curShot;
//	QLineEdit * shotLE;
//	QPushButton *getShotButton;
//	QComboBox *getShotCombo;
//	QLabel* descLab;
//};

class XMLStdParFrame : public QFrame
{
	Q_OBJECT
public:
	XMLStdParFrame(QWidget *p,QWidget *gla=0);
	void loadFrameContent(const MLXMLPluginInfo::XMLMapList& parMap,EnvWrap& envir);
	void extendedView(bool ext,bool help);
	//void loadFrameContent(RichParameter* par,MeshDocument *mdPt = 0);

	//// The curParSet that is passed must be 'compatible' with the RichParameterSet that have been used to create the frame.
	//// This function updates the RichParameterSet used to create the frame AND fill also the passed <curParSet>
	//void readValues(RichParameterSet &curParSet);
	void resetExpressions(const MLXMLPluginInfo::XMLMapList& mplist);

	void toggleHelp(bool help);	

	QVector<XMLMeshLabWidget*> xmlfieldwidgets;
	QVector<QLabel *> helpList;

	QWidget *curr_gla; // used for having a link to the glarea that spawned the parameter asking.
	~XMLStdParFrame();
signals:
	//void frameEvaluateExpression(const Expression& exp,Value** res);
	void parameterChanged();

private:
	QGridLayout * vLayout;
	bool extended;

	/*void dynamicFloatChanged(int mask);
	void parameterChanged();*/
};

class MeshLabXMLStdDialog : public QDockWidget
{
	Q_OBJECT

public:
	MeshLabXMLStdDialog(Env& envir,QWidget *p);
	~MeshLabXMLStdDialog();

	void clearValues();
	void createFrame();
	void loadFrameContent();

	bool showAutoDialog(MeshLabXMLFilterContainer& mfc, MeshDocument * md, MainWindowInterface *mwi, QWidget *gla=0);
	bool isPreviewable() const;
	void closeEvent ( QCloseEvent * event ); 
	void resetPointers();
	MeshModel *curModel;
//signals:
	//void dialogEvaluateExpression(const Expression& exp,Value** res);
	//void expandView(bool exp);

	MeshDocument * curMeshDoc;
private slots:
	void applyClick();
	void closeClick();
	void resetExpressions();
	void toggleHelp();
	void togglePreview();
	void applyDynamic();
	void changeCurrentMesh(int meshInd);
	void extendedView(bool ext);

private:
	Env& env;
	QFrame *qf;
	XMLStdParFrame *stdParFrame;
	//QAction *curAction;
	MeshModelState meshState;
	MeshModelState meshCacheState;
	QCheckBox *previewCB;
	QWidget * curgla;
	int curmask;
	MeshLabXMLFilterContainer* curmfc;
	MainWindowInterface *curmwi;
	MLXMLPluginInfo::XMLMapList curParMap;
	//XMLFilterInfo::XMLMapList prevParMap;
	QString applyContext;
	QString previewContext;
	bool validcache;
	bool showHelp;
};

#endif
