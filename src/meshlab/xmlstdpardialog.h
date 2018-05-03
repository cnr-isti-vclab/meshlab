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

class XMLWidgetException : public MLException
{
public:
    XMLWidgetException(const QString& text)
        :MLException(QString("XML Widget Error: ") + text){}

    ~XMLWidgetException() throw() {}
};

class XMLPersistenToolbox : public QFrame
{
	Q_OBJECT
public:
	XMLPersistenToolbox(QWidget* parent);
	~XMLPersistenToolbox() {}

	private slots:
	void saveClicked();
	void loadClicked();

signals:
	void saveRequested();
	void loadRequested();
};

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

    virtual void addWidgetToGridLayout(QGridLayout* lay,const int r);

    bool isImportant;
    // called when the user press the 'default' button to reset the parameter values to its default.
    // It just set the parameter value and then it calls the specialized resetWidgetValue() to update also the widget.
    //void resetValue();
    // update the parameter with the current widget values and return it.
    //Value& getWidgetValue();

signals:
    void parameterChanged();
	void savePersistentParameterValueRequested(QString name);
	void loadPersistentParameterValueRequested(QString name);
    //void widgetEvaluateExpression(const Expression& exp,Value** res);
    //void insertParamInEnv(const QString& paramName,Expression* paramExp);

    //int row;
    //QGridLayout* gridLay;
private slots:
	void savePersistentParameterValue();
	void loadPersistentParameterValue();
protected:
    QLabel* helpLab;
    EnvWrap env;
	XMLPersistenToolbox* perstb;
	QString parname;
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

    void addWidgetToGridLayout(QGridLayout* lay,const int r);

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

    void addWidgetToGridLayout(QGridLayout* lay,const int r);
private slots:
//	void tooltipEvaluation();
private:
    //WHY WE NEED THIS FUNCTION?
    //IN C++ IS NOT HEALTHY AT ALL TO CALL A VIRTUAL FUNCTION FROM OBJECT CONSTRUCTOR.
    //SO I CANNOT CALL DIRECTLY THE updateVisibility FUNCTION.
    //THIS THING HAS AS CONSEQUENCE THAT I HAVE TO PASTE AND COPY THE updateVisibility CODE INSIDE THE CONSTRUCTOR.
    //TO AVOID THIS FOR EACH WIDGET WE ADD A setVisibility FUNCTION (OBVIOUSLY NOT VIRTUAL) WITH THE CODE THAT WE SHOULD PUT INSIDE THE
    //updateVisibility.
    //THE CODE OF VIRTUAL FUNCTION updateVisibility WILL BE ONLY A CALL TO THE NON VIRTUAL FUNCTION setVisibility.
    void setVisibility(const bool vis);

	QHBoxLayout* hlay;
    QLabel* fieldDesc;
    QLineEdit* lineEdit;
};

class XMLStringWidget : public XMLMeshLabWidget
{
    Q_OBJECT
public:
    XMLStringWidget(const MLXMLPluginInfo::XMLMap& xmlWidgetTag,EnvWrap& envir,QWidget* parent);
    ~XMLStringWidget();

    void set(const QString& nwExpStr);
    //void updateWidget(const XMLFilterInfo::XMLMap& xmlWidgetTag);
    void updateVisibility(const bool vis);

	/*WARNING!!!! In order to be coherent with the scripting evaluation environment at the value of the XMLStringWidget a pair of double quotes is added at the beginning and at the end of the string*/
	/*The best, and safest way to remove them (if they are not needed), is to let the scripting environment to evaluate the resulting string: Env e; QString st = e.evaluate(string_widg->getWidgetExpr).toString(); */

    QString getWidgetExpression();

    void addWidgetToGridLayout(QGridLayout* lay,const int r);
private slots:
//	void tooltipEvaluation();
private:
    //WHY WE NEED THIS FUNCTION?
    //IN C++ IS NOT HEALTHY AT ALL TO CALL A VIRTUAL FUNCTION FROM OBJECT CONSTRUCTOR.
    //SO I CANNOT CALL DIRECTLY THE updateVisibility FUNCTION.
    //THIS THING HAS AS CONSEQUENCE THAT I HAVE TO PASTE AND COPY THE updateVisibility CODE INSIDE THE CONSTRUCTOR.
    //TO AVOID THIS FOR EACH WIDGET WE ADD A setVisibility FUNCTION (OBVIOUSLY NOT VIRTUAL) WITH THE CODE THAT WE SHOULD PUT INSIDE THE
    //updateVisibility.
    //THE CODE OF VIRTUAL FUNCTION updateVisibility WILL BE ONLY A CALL TO THE NON VIRTUAL FUNCTION setVisibility.
    void setVisibility(const bool vis);
	QHBoxLayout* hlay;
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

    void addWidgetToGridLayout(QGridLayout* lay,const int r);

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
    QGridLayout* glay;
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

    void addWidgetToGridLayout(QGridLayout* lay,const int r);

public slots:
        void  getPoint();
        void  setPoint( const QString& name,const vcg::Point3f& p );
        void  setShot(const QString& name,const vcg::Shotf& val );
signals:
        void askViewDir(QString);
        void askViewPos(QString);
        void askSurfacePos(QString);
        void askCameraPos(QString);
		void askTrackballPos(QString);
        void setExp(const QString& name,const QString& exp );
protected:
    QLineEdit * coordSB[3];
    QComboBox *getPoint3Combo;
    QPushButton *getPoint3Button;
    QLabel* descLab;
    QWidget* curr_gla;
    QHBoxLayout* hlay;

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
    void addWidgetToGridLayout(QGridLayout* lay,const int r);

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
    QHBoxLayout* hlay;

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
    void addWidgetToGridLayout(QGridLayout* lay,const int r);

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
    QHBoxLayout* hlay;
    float minVal;
    float maxVal;
};

class XMLComboWidget : public XMLMeshLabWidget
{
    Q_OBJECT
protected:
    QComboBox *enumCombo;
    QLabel *enumLabel;
	QHBoxLayout* hlay;

public:
    XMLComboWidget(const MLXMLPluginInfo::XMLMap& xmlWidgetTag,EnvWrap& envir,QWidget* p);
    ~XMLComboWidget();
    void updateVisibility(const bool vis);
    virtual QString getWidgetExpression();
    virtual void set(const QString &) {}
    void addWidgetToGridLayout(QGridLayout* lay,const int r);
	
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
	void set(const QString&);
private:
	QMap<int, QString> mp;
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
    void set(const QString & expr);

    QString paramName;

    QString getWidgetExpression();
    void addWidgetToGridLayout(QGridLayout* lay,const int r);

public slots:
    void  getShot();
    void  setShotValue(QString name, Shotm val);
signals:
    void askRasterShot(QString);
    void askMeshShot(QString);
    void askViewerShot(QString);
    void dialogParamChanged();

protected:
    QWidget *gla_curr;
    Shotm curShot;
    //QLineEdit * shotLE;
    QComboBox *getShotCombo;
    QLabel* descLab;
    QPushButton* getShotButton;
    QHBoxLayout* hlay;

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

class XMLStdParFrame : public QFrame
{
    Q_OBJECT
public:
    XMLStdParFrame(QWidget *p,QWidget *gla=0);
	~XMLStdParFrame();
	void loadFrameContent(const MeshLabXMLFilterContainer& filtcont, EnvWrap& envir, MeshDocument* md);
    void extendedView(bool ext,bool help);
	bool setValue(const QString& name, const QString& val);
	bool getValue(const QString& name, QString& val);

    void toggleHelp(const bool help);

    QWidget *curr_gla; // used for having a link to the glarea that spawned the parameter asking.
signals:
    void parameterChanged();
	void savePersistentParameterValueRequested(QString name, QString value);
	void loadPersistentParameterValueRequested(QString name);

private slots:
	void savePersistentParameterValue(QString name);
private:
	QMap<QString, XMLMeshLabWidget*> xmlfieldwidgets;
	QMap<QString, QLabel *> helpList;
    bool extended;

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

    bool showAutoDialog(MeshLabXMLFilterContainer& mfc, PluginManager& pm, MeshDocument * md, MainWindowInterface *mwi, QWidget *gla=0);
    bool isPreviewable() const;
    void closeEvent ( QCloseEvent * event );
    void resetPointers();
    MeshModel *curModel;
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
    void postFilterExecution();
	void loadPersistent(QString name);
	void savePersistent(QString name, QString expr);

private:
    void startFilterExecution();
    void setDialogStateRelativeToFilterExecution(const bool isfilterinexecution);
    static QString applyButtonLabel(const bool applystate);

    bool isfilterexecuting;

    Env env;
    QFrame *qf;
    XMLStdParFrame *stdParFrame;
    //QAction *curAction;
    MeshModelState meshState;
    MeshModelState meshCacheState;
    QCheckBox *previewCB;
    QWidget * curgla;
    uint curmask;
    MeshLabXMLFilterContainer* curmfc;
    MainWindowInterface *curmwi;
    MLXMLPluginInfo::XMLMapList curParMap;
    //XMLFilterInfo::XMLMapList prevParMap;
    QString applyContext;
    QString previewContext;
    bool validcache;
    bool showHelp;
    //we have to change the button caption when the filter has been launched for execution
    QPushButton *applyButton;
    QPushButton *helpButton;
    QPushButton *closeButton;
    QPushButton *defaultButton;

signals:
    void filterInterrupt(const bool isinterruptrequested);
    void filterParametersEvaluated(const QString& fnmame,const QMap<QString,QString>& parvalue);
};

// This class provide a modal dialog box for asking a generic parameter set
// It can be used by anyone needing for some values in a structred form and having some integrated help
class OldScriptingSystemXMLParamDialog: public QDialog
{
    Q_OBJECT
public:
    OldScriptingSystemXMLParamDialog(QMap<QString,QString>& currparamvalues,MeshLabXMLFilterContainer& mfc, PluginManager& pm, MeshDocument * md, MainWindowInterface *mwi, QWidget* p, QWidget *gla=0);
    ~OldScriptingSystemXMLParamDialog();


    void createFrame();

public slots:
    void getAccept();
    void toggleHelp();
        //reset the values on the gui back to the ones originally given to the dialog
    void resetValues();
//signals:
//    void newParamValues(const QString fname, const QMap<QString,QString> pvals);
private:
    Env _env;
    QMap<QString,QString>& _paramvalues;
    XMLStdParFrame* _stdparframe;
    MeshLabXMLFilterContainer& _mfc;
    PluginManager& _pm;
    MeshDocument *_meshdocument;
    MainWindowInterface* _mwi;
    QWidget* _gla;
    bool _showhelp;
};

#endif
