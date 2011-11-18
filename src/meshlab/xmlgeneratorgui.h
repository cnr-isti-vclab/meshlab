#ifndef XMLGENERATORGUI_H
#define XMLGENERATORGUI_H

#include "../common/meshmodel.h"
#include "../common/xmlfilterinfo.h"
#include "../common/pluginmanager.h"
#include "additionalgui.h"
#include <QTabWidget>
#include <QFrame>
#include <QTreeWidget>
#include <QDockWidget>
#include <QCheckBox>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QPlainTextEdit>

namespace Ui 
{
	class FilterCreatorGUI;
	class ParamCreatorGUI;
	class FilterCreatorTab;
} 

class ParamGeneratorGUI : public QFrame
{
	Q_OBJECT
public:
	ParamGeneratorGUI(QWidget* parent = NULL);
	ParamGeneratorGUI(const QString& name,QTreeWidgetItem* pitem,QWidget* parent = NULL);
	~ParamGeneratorGUI();

	/*this is the external function for setting the name. It's invoked by the caller of ParamGeneratorGUI constructor.*/
	void setParamName(const QString& name);

private slots:
	void updateGUIType(const QString& paramtype);
	void updateGUILabel(const QString& lab);
	void updateOptionalWidgetVisibility(const QString&);
	void updateGUIWidgetInterface(const QString& guitype);
	void validateName();
	void updateItemLabel(const QString&);

public slots:
	void collectInfo(MLXMLParamSubTree& param);
	void importInfo(const MLXMLParamSubTree& tree);

private:
	void initUI();
	void initConnections();
	QTreeWidgetItem* parentitem;
	QComboBox* ptype;
	QLineEdit* pname;
	QLineEdit* pdefault;
	QCheckBox* pisimp;
	QPlainTextEdit* phel;
	//QTreeWidget* pguitree;
	QLineEdit* pguilab;
	//QTreeWidgetItem* pguiminitem;
	//QTreeWidgetItem* pguimaxitem;
	QLabel* pguiminlab;
	QLabel* pguimaxlab;
	QLineEdit* pguimin;
	QLineEdit* pguimax;
	QComboBox* pguitype;
	QLabel* enumlab;
	QLineEdit* enumnames;

	
	QString currentname;
signals:
	void paramNameValidationRequest(QTreeWidgetItem* parent,const QString& newname);
	void itemLabelUpdateRequest(QTreeWidgetItem* parent,const QString& text);
};

class FilterGeneratorTab;

class FilterGeneratorGUI : public QFrame
{
	Q_OBJECT
	friend class FilterGeneratorTab;
public:
	FilterGeneratorGUI(QWidget* parent = NULL);
	~FilterGeneratorGUI();

public slots:
	void collectInfo(MLXMLFilterSubTree& filter);
	void importInfo(const MLXMLFilterSubTree& tree);
	
	void setFilterName(const QString& newname);
private slots:
	void menuSelection( QAction* act);
	void validateAndSetItemName(QTreeWidgetItem* parent,const QString& newname);
	void updateItemText(QTreeWidgetItem* parent,const QString& text);
	void filterNameValidationRequest();
	void updateFunctionName(const QString& fun);
	void filterNameUpdated(const QString& name);
	/*void validateAndSetFilterName(const int tabindex,)*/
private:
	enum MenuOption {MN_ADDPARAM,MN_REMOVECURRENTPARAM};

	void fillComboBoxes();
	void createContextMenu();
	void addParam();
	void addParam(const MLXMLParamSubTree& param);
	void removeCurrentParam();

	Ui::FilterCreatorGUI* ui;
signals:
	void validateFunctionName(const QString& name);
	void filterNameUpdated(const QString& name,QWidget* wid);
	void validateFilterName(const QString& name,FilterGeneratorGUI* thiswid);
	
//	void invalidExistingNameChangedWith(QTreeWidgetItem* item,const QString& corrected);
};

class FilterGeneratorTab : public QFrame
{
	Q_OBJECT
public:
	FilterGeneratorTab(const QString& filtername,QWidget* parent = NULL);
	~FilterGeneratorTab();

	void collectInfo(MLXMLFilterSubTree& filter);
	void importInfo(const MLXMLFilterSubTree& tree);
	void initLibInEnv(PluginManager& pm);
	QString getCode() const;
	void setCode(const QString& code);
	QScriptValue executeCode(MeshDocument* md);

protected:
	void paintEvent(QPaintEvent* p);
private slots:
	void jsButtonClicked();
	void guiButtonClicked();
	//void filterNameValidationRequest(const QString& name);
	//void parentDocked( Qt::DockWidgetArea area);
signals:
	void filterNameUpdated(const QString& fname,QWidget* wid);
	void validateFilterName(const QString& fname,FilterGeneratorGUI* wid);

private:	
	//void expandCollapse(bool& exp,PrimitiveButton* pb,QFrame* fr );
	void expandCollapse(const bool exp,PrimitiveButton* pb,QFrame* fr );

	Env env;
	bool jsexp;
	bool guiexp;
	Ui::FilterCreatorTab* ui;
};

class PluginGeneratorGUI : public QDockWidget
{
	Q_OBJECT
public:
	PluginGeneratorGUI(PluginManager& pman,QWidget* parent = NULL);
	~PluginGeneratorGUI();
	void setDocument(MeshDocument* mdoc);

protected:
	void paintEvent(QPaintEvent *event);
	void contextMenuEvent ( QContextMenuEvent * event );
private slots:
	void menuSelection(QAction*);
	void addNewFilter(const MLXMLPluginInfo::XMLMap& filter);
	void updateTabTitle(const QString& name,QWidget* wid);
	void validateFilterName(const QString& name,FilterGeneratorGUI* wid);
private:
	enum MenuOption {MN_ADDFILTER,MN_REMOVEFILTER,MN_EXPORTFILTERINPLUGIN,MN_EXECUTECODE,MN_LOADCODE,MN_SAVECODE,MN_NEWXMLPLUGIN,MN_SAVEXMLPLUGIN,MN_SAVEASXMLPLUGIN,MN_LOADXMLPLUGIN,MN_INSERTPLUGINMESHLAB};
	void createContextMenu();
	void executeCurrentCode();
	void loadScriptCode();
	void saveScriptCode();
	void addNewFilter();
	void removeFilter();
	void exportFilterInPlugin();
	void newXMLPlugin();
	void saveXMLPlugin();
	void saveAsXMLPlugin();
	void loadXMLPlugin();
	void insertPluginInMeshLab();
	int getPageIndexOfWidget(QWidget* wid);
	QString generateXML(const MLXMLTree& tree);
	void collectInfo(MLXMLTree& tree);
	void importInfo(const MLXMLTree& tree);
	FilterGeneratorTab* tab(int ii);

	QString plugname;
	QString author;
	QString mail;

	QMenu* menu;
	QTabWidget* tabs;
	bool init;
	QDir directory;
	MeshDocument* doc;
	PluginManager& PM;
signals:
	void scriptCodeExecuted(const QScriptValue& val,const int time,const QString&);
};
#endif