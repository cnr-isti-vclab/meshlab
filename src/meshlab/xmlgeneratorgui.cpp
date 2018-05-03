#include "../common/meshmodel.h"
#include "../common/interfaces.h"
#include "../common/mlexception.h"

#include "../common/pluginmanager.h"

#include "ui_filtercreatortab.h"
#include "ui_filtergui.h"
//#include "ui_paramgui.h"
//#include "ui_filtereditorframe.h"
#include "xmlgeneratorgui.h"
#include <QSplitter>
#include <QFileDialog>

ParamGeneratorGUI::ParamGeneratorGUI(QWidget* parent /*= NULL*/ )
:QFrame(parent),parentitem(NULL),currentname()
{
    initUI();
    initConnections();
}

ParamGeneratorGUI::ParamGeneratorGUI( const QString& name,QTreeWidgetItem* pitem,QWidget* parent /*= NULL*/ )
:QFrame(parent),parentitem(pitem),currentname(name)
{
    initUI();
    initConnections();
    setParamName(name);
}

ParamGeneratorGUI::~ParamGeneratorGUI()
{

}

void ParamGeneratorGUI::collectInfo( MLXMLParamSubTree& param )
{
    param.paraminfo[MLXMLElNames::paramType] = ptype->currentText();
    if (ptype->currentText() == MLXMLElNames::enumType)
        param.paraminfo[MLXMLElNames::paramType] += " " + enumnames->text();
    param.paraminfo[MLXMLElNames::paramName] = pname->text();
    param.paraminfo[MLXMLElNames::paramDefExpr] = pdefault->text();
    
	QString isimp("false");
    if (pisimp->isChecked())
        isimp = "true";
	param.paraminfo[MLXMLElNames::paramIsImportant] = isimp;

	QString ispers("false");
	if (pispers->isChecked())
		isimp = "true";
	param.paraminfo[MLXMLElNames::paramIsPersistent] = ispers;
    
	param.paraminfo[MLXMLElNames::paramHelpTag] = UsefulGUIFunctions::avoidProblemsWithHTMLTagInsideXML(phel->toPlainText());
    param.gui.guiinfo[MLXMLElNames::guiType] = pguitype->currentText();
    param.gui.guiinfo[MLXMLElNames::guiLabel] = pguilab->text();
    param.gui.guiinfo[MLXMLElNames::guiMinExpr] = pguimin->text();
    param.gui.guiinfo[MLXMLElNames::guiMaxExpr] = pguimax->text();
}

void ParamGeneratorGUI::initConnections()
{
    connect(ptype,SIGNAL(currentIndexChanged(const QString&)),this,SLOT(updateGUIType(const QString&)));
    connect(ptype,SIGNAL(currentIndexChanged(const QString&)),this,SLOT(updateOptionalWidgetVisibility(const QString&)));
    connect(pguitype,SIGNAL(currentIndexChanged(const QString&)),this,SLOT(updateGUIWidgetInterface(const QString&)));
    connect(pname,SIGNAL(textChanged(const QString&)),this,SLOT(updateItemLabel(const QString&)));
    connect(pname,SIGNAL(textChanged(const QString&)),this,SLOT(updateGUILabel(const QString&)));
    connect(pname,SIGNAL(editingFinished()),this,SLOT(validateName()));
}

void ParamGeneratorGUI::initUI()
{
    QLabel* tlab = new QLabel("Type",this);
    ptype = new QComboBox(this);
    ptype->view()->setAlternatingRowColors(true);
    QStringList ptypelist;
    MLXMLElNames::initMLXMLTypeList(ptypelist);
    ptype->addItems(ptypelist);

    enumlab = new QLabel("Enum Values",this);
    enumnames = new QLineEdit("{ enumval_0 : 0 | enumval_1 : 1}",this);
    enumnames->setToolTip("Enum Values Format: { enumval_0 : n0 | enumval_1 : n1 | ... | enumval_n : nn }");
    updateOptionalWidgetVisibility(ptype->currentText());

    QLabel* nlab = new QLabel("Name",this);
    pname = new QLineEdit(this);
    QLabel* dlab = new QLabel("Default Value",this);
    pdefault = new QLineEdit(this);
    pisimp = new QCheckBox("isImportant",this);
    pisimp->setCheckState(Qt::Checked);
	pispers = new QCheckBox("isPersistent", this);
	pispers->setCheckState(Qt::Unchecked);
    //QLabel* hlab = new QLabel("Help",this);
    QGroupBox* gpbox = new QGroupBox("Parameter Help",this);
    QGridLayout* laybox = new QGridLayout();
    phel = new QPlainTextEdit(this);
    laybox->addWidget(phel);
    gpbox->setLayout(laybox);

    QGroupBox* gpguibox = new QGroupBox("GUI",this);
    QGridLayout* layguibox = new QGridLayout();
    QLabel* wlab = new QLabel("Widget Type",this);
    pguitype = new QComboBox(this);
    pguitype->view()->setAlternatingRowColors(true);
    QLabel* llab = new QLabel("Label",this);
    pguilab = new QLineEdit(this);

    pguiminlab = new QLabel("Min",this);
    pguimin = new QLineEdit(this);
    pguimaxlab = new QLabel("Max",this);
    pguimax = new QLineEdit(this);
    layguibox->addWidget(wlab,0,0);
    layguibox->addWidget(pguitype,0,1);
    layguibox->addWidget(llab,1,0);
    layguibox->addWidget(pguilab,1,1);
    layguibox->addWidget(pguiminlab,2,0);
    layguibox->addWidget(pguimin,2,1);
    layguibox->addWidget(pguimaxlab,3,0);
    layguibox->addWidget(pguimax,3,1);
    gpguibox->setLayout(layguibox);

    /*QLabel* glab = new QLabel("GUI",this);*/
    QGridLayout* layout = new QGridLayout(this);
    layout->addWidget(tlab,0,0);
    layout->addWidget(ptype,0,1);
    layout->addWidget(enumlab,1,0);
    layout->addWidget(enumnames,1,1);
    layout->addWidget(nlab,2,0);
    layout->addWidget(pname,2,1);
    layout->addWidget(dlab,3,0);
    layout->addWidget(pdefault,3,1);
    layout->addWidget(pisimp,4,0);
    //layout->addWidget(hlab,4,0);
    layout->addWidget(gpbox,5,0,1,2);
    layout->addWidget(gpguibox,6,0,1,2);
    updateGUIType(ptype->currentText());
}

void ParamGeneratorGUI::updateGUIType( const QString& paramtype )
{
    pguitype->clear();
    QStringList ls;
    MLXMLElNames::initMLXMLGUIListForType(paramtype,ls);
    pguitype->addItems(ls);
}

void ParamGeneratorGUI::updateGUIWidgetInterface( const QString& guitype )
{
    bool show = ((guitype == MLXMLElNames::absPercTag) || (guitype == MLXMLElNames::sliderWidgetTag));
    pguiminlab->setVisible(show);
    pguimin->setVisible(show);
    pguimaxlab->setVisible(show);
    pguimax->setVisible(show);
}

void ParamGeneratorGUI::setParamName( const QString& name )
{
    currentname = name;
    pname->setText(currentname);
    //emit paramNameChanged(tmp,pname->text());
}

void ParamGeneratorGUI::validateName()
{
    emit paramNameValidationRequest(parentitem,pname->text());
}

void ParamGeneratorGUI::updateItemLabel( const QString& text)
{
    emit itemLabelUpdateRequest(parentitem,text);
}

void ParamGeneratorGUI::importInfo( const MLXMLParamSubTree& tree )
{
    QString typ = tree.paraminfo[MLXMLElNames::paramType];
    if (typ.contains(MLXMLElNames::enumType))
    {
        QString enumexp = typ;
        enumexp.remove(MLXMLElNames::enumType);
        enumnames->setText(enumexp.trimmed());
        typ = MLXMLElNames::enumType;
    }
    ptype->setCurrentIndex(ptype->findText(typ));
    pname->setText(tree.paraminfo[MLXMLElNames::paramName]);
    pdefault->setText(tree.paraminfo[MLXMLElNames::paramDefExpr]);
    phel->setPlainText(tree.paraminfo[MLXMLElNames::paramHelpTag]);
	bool isimpcheck = true;
    if (tree.paraminfo[MLXMLElNames::paramIsImportant].trimmed() == QString("false"))
		isimpcheck = false;
	pisimp->setChecked(isimpcheck);

	bool isperscheck = true;
	if (tree.paraminfo[MLXMLElNames::paramIsPersistent].trimmed() == QString("false"))
		isperscheck = false;
	pispers->setChecked(isperscheck);

    QString guity = tree.gui.guiinfo[MLXMLElNames::guiType];
    pguitype->setCurrentIndex(pguitype->findText(guity));
    pguilab->setText(tree.gui.guiinfo[MLXMLElNames::guiLabel]);
    if ((guity == MLXMLElNames::absPercTag) || (guity == MLXMLElNames::sliderWidgetTag))
    {
        pguimin->setText(tree.gui.guiinfo[MLXMLElNames::guiMinExpr]);
        pguimax->setText(tree.gui.guiinfo[MLXMLElNames::guiMaxExpr]);
    }
}

void ParamGeneratorGUI::updateOptionalWidgetVisibility( const QString& tname)
{
    bool b = (tname == MLXMLElNames::enumType);
    enumlab->setVisible(b);
    enumnames->setVisible(b);
}

void ParamGeneratorGUI::updateGUILabel( const QString& lab )
{
    pguilab->setText(lab);
}

FilterGeneratorGUI::FilterGeneratorGUI( QWidget* parent /*= NULL*/ )
:QFrame(parent)
{
    ui = new Ui::FilterCreatorGUI();
    ui->setupUi(this);
    //QPlainText { background-color: white;}
    QColor col = UsefulGUIFunctions::editorMagicColor();
    this->setStyleSheet("QFrame { background-color:rgb(" + QString::number(col.red()) + "," + QString::number(col.green()) + "," + QString::number(col.blue()) + "); border-radius: 4px; } QAbstractScrollArea { background-color: white;}  QTreeWidget  { background-color: white;} QComboBox QAbstractItemView{qproperty-alternatingRowColors:true;alternate-background-color: white;}");
    fillComboBoxes();
    createContextMenu();
    ui->paramviewer->setVerticalScrollMode(QTreeWidget::ScrollPerPixel);
    ui->arity->view()->setAlternatingRowColors(true);
    ui->paramviewer->header()->hide();
    connect(ui->nameLine,SIGNAL(textChanged(const QString&)),this,SLOT(updateFunctionName(const QString&)));
    connect(ui->nameLine,SIGNAL(textEdited(const QString&)),this,SLOT(filterNameUpdated(const QString&)));
    connect(ui->nameLine,SIGNAL(editingFinished()),this,SLOT(filterNameValidationRequest()));
}

FilterGeneratorGUI::~FilterGeneratorGUI()
{
    delete ui;
}

void FilterGeneratorGUI::collectInfo( MLXMLFilterSubTree& filter )
{
    filter.filterinfo[MLXMLElNames::filterName] = ui->nameLine->text();
    filter.filterinfo[MLXMLElNames::filterScriptFunctName] = ui->functionLine->text();
    filter.filterinfo[MLXMLElNames::filterArity] = ui->arity->currentText();
    filter.filterinfo[MLXMLElNames::filterPreCond] = ui->precond->selectedItemsString();
    filter.filterinfo[MLXMLElNames::filterPostCond] = ui->postcond->selectedItemsString();
    filter.filterinfo[MLXMLElNames::filterClass] = ui->category->selectedItemsString();
    filter.filterinfo[MLXMLElNames::filterHelpTag] = UsefulGUIFunctions::avoidProblemsWithHTMLTagInsideXML(ui->helpedit->toPlainText());
    QString isint("false");
    if (ui->isinterr->isChecked())
        isint = "true";
    filter.filterinfo[MLXMLElNames::filterIsInterruptible] = isint;
    QList<QTreeWidgetItem*> items = ui->paramviewer->findItems(QString(".*"),Qt::MatchRegExp);
    for(int ii = 0;ii < items.size();++ii)
    {
        if (items.at(ii)->child(0) != NULL)
        {
            QWidget* wid = ui->paramviewer->itemWidget(items.at(ii)->child(0),0);
            if (wid)
            {
                ParamGeneratorGUI* pgui = qobject_cast<ParamGeneratorGUI*>(wid);
                if (pgui)
                {
                    MLXMLParamSubTree subtree;
                    pgui->collectInfo(subtree);
                    filter.params.push_back(subtree);
                }
            }
        }
    }
}

void FilterGeneratorGUI::fillComboBoxes()
{
    QString gen("Generic");
    QMap<QString,MeshFilterInterface::FilterClass> category;
    MeshLabFilterInterface::initConvertingCategoryMap(category);
    for(QMap<QString,MeshFilterInterface::FilterClass>::iterator it = category.begin();it != category.end();++it)
    {

        if (it.key() != gen)
            ui->category->addItem(it.key());
        else
            ui->category->setDefaultValue(gen);
    }

    QString none("MM_NONE");
    QMap<QString,MeshModel::MeshElement> element;
    MeshLabFilterInterface::initConvertingMap(element);
    for(QMap<QString,MeshModel::MeshElement>::iterator it = element.begin();it != element.end();++it)
    {
        if (it.key() != none)
        {
            ui->precond->addItem(it.key());
            ui->postcond->addItem(it.key());
        }
        else
        {
            ui->precond->setDefaultValue(none);
            ui->postcond->setDefaultValue(none);
        }
    }

    ui->arity->addItem(MLXMLElNames::singleMeshArity);
    ui->arity->addItem(MLXMLElNames::fixedArity);
    ui->arity->addItem(MLXMLElNames::variableArity);
}

void FilterGeneratorGUI::createContextMenu()
{
    ui->paramviewer->insertInMenu("Add New Parameter",QVariant(MN_ADDPARAM));
    ui->paramviewer->insertInMenu("Remove Current Parameter",QVariant(MN_REMOVECURRENTPARAM));
    connect(ui->paramviewer,SIGNAL(selectedAction(QAction*)),this,SLOT(menuSelection(QAction*)));
}

void FilterGeneratorGUI::menuSelection( QAction* act)
{
    if (act != NULL)
    {
        int indact = act->data().toInt();
        switch(indact)
        {
        case MN_ADDPARAM:
            {
                addParam();
                break;
            }
        case MN_REMOVECURRENTPARAM:
            {
                removeCurrentParam();
                break;
            }
        }
    }
}

void FilterGeneratorGUI::removeCurrentParam()
{
    QList<QTreeWidgetItem*> sel = ui->paramviewer->selectedItems();
    if (sel.size() == 1)
        ui->paramviewer->takeTopLevelItem(ui->paramviewer->indexOfTopLevelItem(sel[0]));

}

void FilterGeneratorGUI::addParam()
{
    QStringList namelist;
    QList<QTreeWidgetItem*> totitems = ui->paramviewer->findItems(QString(".*"),Qt::MatchRegExp);
    for(int ii = 0;ii < totitems.size();++ii)
        namelist.push_back(totitems[ii]->text(0));
    QString tmpname = UsefulGUIFunctions::generateUniqueDefaultName("Param",namelist);
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0,tmpname);
    QTreeWidgetItem* childItem = new QTreeWidgetItem();
    item->addChild(childItem);
    ParamGeneratorGUI* pgui = new ParamGeneratorGUI(tmpname,item,this);
    connect(pgui,SIGNAL(paramNameValidationRequest(QTreeWidgetItem*,const QString&)),this,SLOT(validateAndSetItemName(QTreeWidgetItem*,const QString&)));
    connect(pgui,SIGNAL(itemLabelUpdateRequest(QTreeWidgetItem*,const QString&)),this,SLOT(updateItemText(QTreeWidgetItem*,const QString&)));
    ui->paramviewer->setItemWidget(childItem,0,pgui);
    ui->paramviewer->addTopLevelItem(item);
}

void FilterGeneratorGUI::addParam(const MLXMLParamSubTree& param)
{
    QStringList namelist;
    QList<QTreeWidgetItem*> totitems = ui->paramviewer->findItems(QString(".*"),Qt::MatchRegExp);
    for(int ii = 0;ii < totitems.size();++ii)
        namelist.push_back(totitems[ii]->text(0));
    QString tmpname = param.paraminfo[MLXMLElNames::paramName];
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0,tmpname);
    QTreeWidgetItem* childItem = new QTreeWidgetItem();
    item->addChild(childItem);
    ParamGeneratorGUI* pgui = new ParamGeneratorGUI(tmpname,item,this);
    pgui->importInfo(param);
    connect(pgui,SIGNAL(paramNameValidationRequest(QTreeWidgetItem*,const QString&)),this,SLOT(validateAndSetItemName(QTreeWidgetItem*,const QString&)));
    connect(pgui,SIGNAL(itemLabelUpdateRequest(QTreeWidgetItem*,const QString&)),this,SLOT(updateItemText(QTreeWidgetItem*,const QString&)));
    ui->paramviewer->setItemWidget(childItem,0,pgui);
    ui->paramviewer->addTopLevelItem(item);
}

void FilterGeneratorGUI::validateAndSetItemName( QTreeWidgetItem* parentitem,const QString& newname )
{
    QStringList allnames;
    QList<QTreeWidgetItem*> allwid = ui->paramviewer->findItems(QString(".*"),Qt::MatchRegExp);
    for(int ii = 0;ii < allwid.size();++ii)
        allnames.push_back(allwid[ii]->text(0));
    QString tmpname = UsefulGUIFunctions::changeNameIfAlreadyInList(newname,allnames);
    parentitem->setText(0,tmpname);
    ParamGeneratorGUI* gui = qobject_cast<ParamGeneratorGUI*>(ui->paramviewer->itemWidget(parentitem->child(0),0));
    if (gui != NULL)
        gui->setParamName(tmpname);
}

void FilterGeneratorGUI::updateItemText( QTreeWidgetItem* parent,const QString& text)
{
    parent->setText(0,text);
}

void FilterGeneratorGUI::updateFunctionName( const QString& /*fun*/ )
{
    QString nm = UsefulGUIFunctions::generateFunctionName(ui->nameLine->text());
    ui->functionLine->setText(nm);
}

void FilterGeneratorGUI::setFilterName( const QString& newname )
{
    ui->nameLine->setText(newname);
}

void FilterGeneratorGUI::filterNameValidationRequest()
{
    emit validateFilterName(ui->nameLine->text(),this);
}

void FilterGeneratorGUI::filterNameUpdated( const QString& name )
{
    emit filterNameUpdated(name,this);
}

void FilterGeneratorGUI::importInfo( const MLXMLFilterSubTree& tree )
{
    ui->nameLine->setText(tree.filterinfo[MLXMLElNames::filterName]);
    ui->functionLine->setText(tree.filterinfo[MLXMLElNames::filterScriptFunctName]);
    QString arval = tree.filterinfo[MLXMLElNames::filterArity];
    ui->arity->setCurrentIndex(ui->arity->findText(arval));
    ui->category->setCurrentValue(tree.filterinfo[MLXMLElNames::filterClass].split(" | "));
    ui->precond->setCurrentValue(tree.filterinfo[MLXMLElNames::filterPreCond].split(" | "));
    ui->postcond->setCurrentValue(tree.filterinfo[MLXMLElNames::filterPostCond].split(" | "));
    ui->helpedit->setPlainText(tree.filterinfo[MLXMLElNames::filterHelpTag]);
    if (tree.filterinfo[MLXMLElNames::filterIsInterruptible] == "true")
        ui->isinterr->setCheckState(Qt::Checked);
    for(int ii = 0;ii < tree.params.size();++ii)
        addParam(tree.params[ii]);
}

FilterGeneratorTab::FilterGeneratorTab(const QString& filtername,QWidget* parent /*= NULL*/,const QList<LibraryElementInfo>& libinfolist /*= QStringList()*/)
:QFrame(parent),jsexp(true),guiexp(false)
{
    //filtereditor = new FilterEditorFrame(this);
    ui = new Ui::FilterCreatorTab();
    ui->setupUi(this);
    ui->guibut->setPrimitiveElement(QStyle::PE_IndicatorArrowRight);
    ui->guibut->setMaximumSize(16,16);
    ui->jsbut->setPrimitiveElement(QStyle::PE_IndicatorArrowDown);
    ui->jsbut->setMaximumSize(16,16);
    ui->guiframe->setVisible(false);
    ui->guiframe->setFilterName(filtername);
    ui->jsframe->setVisible(true);
    QColor col = UsefulGUIFunctions::editorMagicColor();
    ui->jsframe->setStyleSheet("QFrame {background-color:rgb("  + QString::number(col.red()) + "," + QString::number(col.green()) + "," + QString::number(col.blue()) + "); border-radius: 4px; } QAbstractScrollArea {background-color: white;}");
    //ui->jscode->setAutoFormatting(QTextEdit::AutoAll);
    layout()->setAlignment(Qt::AlignTop);
    connect(ui->guibut,SIGNAL(released()),this,SLOT(guiButtonClicked()));
    connect(ui->jsbut,SIGNAL(released()),this,SLOT(jsButtonClicked()));
    connect(ui->guiframe,SIGNAL(filterNameUpdated(const QString&,QWidget*)),this,SIGNAL(filterNameUpdated(const QString&,QWidget*)));
    connect(ui->guiframe,SIGNAL(validateFilterName(const QString&,FilterGeneratorGUI*)),this,SIGNAL(validateFilterName(const QString&,FilterGeneratorGUI*)));
    JavaScriptLanguage* js = new JavaScriptLanguage();
    js->addLibrary(libinfolist);
    js->addLibrary(js->getExternalLibrariesMembersInfo());
    ui->jscode->setScriptLanguage(js);

    //disconnect(this,SIGNAL(itemExpanded(QTreeWidgetItem*)),this,SLOT(expandItem(QTreeWidgetItem*)));
    //JavaScriptSyntaxHighLighter* js = new JavaScriptSyntaxHighLighter(pluginvar,namespacelist,filterlist,filtersign,ui->jscode);
    //ui->jscode->setSyntaxHighlighter(js);
}

FilterGeneratorTab::~FilterGeneratorTab()
{
    delete ui;
}

//void FilterGeneratorTab::addRemoveVerticalSpacer()
//{
//	if (!jsexp && !guiexp)
//		ui->mainlayout
//}

void FilterGeneratorTab::jsButtonClicked()
{
    jsexp = !jsexp;
    update();
}

void FilterGeneratorTab::guiButtonClicked()
{
    guiexp = !guiexp;
    update();
}

void FilterGeneratorTab::expandCollapse(const bool exp,PrimitiveButton* pb,QFrame* fr )
{
    fr->setVisible(exp);
    if (exp)
        pb->setPrimitiveElement(QStyle::PE_IndicatorArrowDown);
    else
        pb->setPrimitiveElement(QStyle::PE_IndicatorArrowRight);
}

void FilterGeneratorTab::paintEvent( QPaintEvent* /*p*/ )
{
    expandCollapse(jsexp,ui->jsbut,ui->jsframe);
    expandCollapse(guiexp,ui->guibut,ui->guiframe);
}

void FilterGeneratorTab::collectInfo( MLXMLFilterSubTree& filter )
{
    filter.filterinfo[MLXMLElNames::filterJSCodeTag] = UsefulGUIFunctions::avoidProblemsWithHTMLTagInsideXML(ui->jscode->toPlainText ());
    ui->guiframe->collectInfo(filter);
}

void FilterGeneratorTab::importInfo( const MLXMLFilterSubTree& tree )
{
    ui->jscode->setPlainText(tree.filterinfo[MLXMLElNames::filterJSCodeTag]);
    ui->guiframe->importInfo(tree);
}

QString FilterGeneratorTab::getCode() const
{
    return ui->jscode->toPlainText();
}

void FilterGeneratorTab::setCode( const QString& code )
{
    ui->jscode->setPlainText(code);
}

/*" + gaycolor.red() + "," + gaycolor.green() + "," + gaycolor.blue() + "*/
PluginGeneratorGUI::PluginGeneratorGUI(PluginManager& pman,QWidget* parent )
:QDockWidget(parent),plugscriptname(),author(),mail(),init(false),finfo(QApplication::applicationDirPath()),doc(NULL),PM(pman)
{
    QFrame* f = new QFrame(this);
    QGridLayout* lay = new QGridLayout();
    tabs = new QTabWidget(this);
    tabs->setUsesScrollButtons(true);
    //setWidget(tabs);
    int openedtabs = tabs->count();
    if (openedtabs == 0)
        addNewFilter();
    else
    {
        if (openedtabs > 1)
            for (int ii = 1;ii < openedtabs;++ii)
                tabs->removeTab(ii);
        addNewFilter();
    }
    lay->addWidget(tabs);
    lay->setAlignment(Qt::AlignVCenter);
    f->setLayout(lay);
    setWidget(f);
    createContextMenu();
    this->setVisible(false);
    //this->setScroll
}


void PluginGeneratorGUI::addNewFilter( const MLXMLPluginInfo::XMLMap& /*filter*/ )
{

}

PluginGeneratorGUI::~PluginGeneratorGUI()
{

}


void PluginGeneratorGUI::contextMenuEvent( QContextMenuEvent * event )
{
    menu->popup(event->globalPos());
}

void PluginGeneratorGUI::menuSelection( QAction* act)
{
    if (act != NULL)
    {
        int indact = act->data().toInt();
        switch(indact)
        {
            case MN_EXECUTECODE:
            {
                /*FilterGeneratorTab* tb = tab(tabs->currentIndex());
                if (tb != NULL)
                {
                    QScriptValue result = tb->executeCode(doc);
                    emit scriptCodeExecuted(result);
                }
                else
                    throw MeshLabException("System Error: A FilterGeneratorTab object has been expected.");*/
                executeCurrentCode();
                break;
            }
            case MN_LOADCODE:
            {
                loadScriptCode();
                break;
            }
            case MN_SAVECODE:
            {
                saveScriptCode();
                break;
            }
            case MN_ADDFILTER:
            {
                addNewFilter();
                break;
            }
            case MN_REMOVEFILTER:
            {
                removeFilter();
                break;
            }
            case MN_NEWXMLPLUGIN:
            {
                newXMLPlugin();
                break;
            }
            case MN_SAVEXMLPLUGIN:
            {
                saveXMLPlugin();
                break;
            }
            case MN_SAVEASXMLPLUGIN:
            {
                saveAsXMLPlugin();
                break;
            }
            case MN_LOADXMLPLUGIN:
            {
                loadXMLPlugin();
                break;
            }
            case MN_GETHISTORY:
            {
                importHistory();
                break;
            }
            case MN_INSERTPLUGINMESHLAB:
            {
                insertPluginInMeshLab();
                break;
            }
        }
    }
}

void PluginGeneratorGUI::createContextMenu()
{
    menu = new QMenu(this);
    QAction* runfilt = menu->addAction("Run Current Code");
    runfilt->setData(QVariant(MN_EXECUTECODE));
    QAction* ldscfilt = menu->addAction("Load Script Code");
    ldscfilt->setData(QVariant(MN_LOADCODE));
    QAction* svscfilt = menu->addAction("Save Script Code");
    svscfilt->setData(QVariant(MN_SAVECODE));
    menu->addSeparator();
    QAction* actnewfilt = menu->addAction("New Filter");
    actnewfilt->setData(QVariant(MN_ADDFILTER));
    QAction* actremfilt = menu->addAction("Remove Current Filter");
    actremfilt->setData(QVariant(MN_REMOVEFILTER));
    menu->addSeparator();
    QAction* actnewplug = menu->addAction("New XML Plugin");
    actnewplug->setData(QVariant(MN_NEWXMLPLUGIN));
    QAction* actloadplug = menu->addAction("Load XML Plugin...");
    actloadplug->setData(QVariant(MN_LOADXMLPLUGIN));
    QAction* actsaveplug = menu->addAction("Save XML Plugin");
    actsaveplug->setData(QVariant(MN_SAVEXMLPLUGIN));
    QAction* actsaveasplug = menu->addAction("Save XML Plugin As...");
    actsaveasplug->setData(QVariant(MN_SAVEASXMLPLUGIN));
    menu->addSeparator();
    QAction* acthistory = menu->addAction("Import XML filters history");
    acthistory->setData(QVariant(MN_GETHISTORY));
    menu->addSeparator();
    QAction* actloadmeshlabsplug = menu->addAction("Load Plugin In MeshLab");
    actloadmeshlabsplug->setData(QVariant(MN_INSERTPLUGINMESHLAB));
    connect(menu,SIGNAL(triggered(QAction*)),this,SLOT(menuSelection(QAction*)));
}

void PluginGeneratorGUI::executeCurrentCode()
{
    Env env;
    if (doc != NULL)
    {
        QScriptValue res = env.loadMLScriptEnv(*doc,PM);
        if (res.isError())
            throw JavaScriptException("A Plugin-bridge-code generated a JavaScript Error: " + res.toString() + "\n");
        FilterGeneratorTab* ftab = tab(tabs->currentIndex());
        if (ftab != NULL)
        {
            QTime t;
            t.start();
            QScriptValue result = env.evaluate(ftab->getCode());
            int time = t.elapsed();
            emit scriptCodeExecuted(result,time,env.output());
        }
        else
            throw MLException("System Error: A FilterGeneratorTab object has been expected.");
    }
    else
        throw MLException("System Error: A valid MeshLabDocument has not correctly been associated with PluginGeneratorGUI. Code cannot be executed.");
}

void PluginGeneratorGUI::addNewFilter()
{
    QStringList namelist;
    for(int ii = 0;ii < tabs->count();++ii)
        namelist.push_back(tabs->tabText(ii));
    QString tmpname = UsefulGUIFunctions::generateUniqueDefaultName("Filter",namelist);
    FilterGeneratorTab* tb = new FilterGeneratorTab(tmpname,this,PM.libinfolist);
    //tb->initLibInEnv(PM);
    tabs->addTab(tb,tmpname);
    connect(tb,SIGNAL(filterNameUpdated(const QString&,QWidget*)),this,SLOT(updateTabTitle(const QString&,QWidget*)));
    connect(tb,SIGNAL(validateFilterName(const QString&,FilterGeneratorGUI*)),this,SLOT(validateFilterName(const QString&,FilterGeneratorGUI*)));
}

void PluginGeneratorGUI::removeFilter()
{
    tabs->removeTab(tabs->currentIndex());
}

void PluginGeneratorGUI::newXMLPlugin()
{
    QMessageBox::StandardButton ret=QMessageBox::question(this,  tr("MeshLab"), tr("Project '%1' modified.\n\nClose without saving?").arg(finfo.fileName()),
        QMessageBox::Yes|QMessageBox::No,
        QMessageBox::No);
    if(ret==QMessageBox::No)	// don't close please!
        return;
    int count = tabs->count();
    for(int ii = 0;ii < count;++ii)
        removeFilter();
    this->author ="";
    this->mail = "";
    this->plugscriptname ="";
    finfo.setFile(QApplication::applicationDirPath());
    addNewFilter();
}

void PluginGeneratorGUI::saveXMLPlugin()
{
    if (finfo.isFile())
        save(finfo.absoluteFilePath(),false);
    else
        saveAsXMLPlugin();
}

void PluginGeneratorGUI::loadScriptCode()
{
    QFileDialog* lddiag = new QFileDialog(this,tr("Load Script File"),finfo.absolutePath(), tr("Script File (*.js)"));
    lddiag->setAcceptMode(QFileDialog::AcceptOpen);
    lddiag->exec();
    QStringList files = lddiag->selectedFiles();
    delete lddiag;
    if (files.size() != 1)
        return;
    QFile file(files[0]);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        qDebug("Warning: File %s has not been loaded.", qUtf8Printable(files[0]));
    finfo.setFile(files[0]);
    QByteArray code = file.readAll();
    file.close();
    FilterGeneratorTab* tb = tab(tabs->currentIndex());
    if (tb != NULL)
        tb->setCode(QString(code));

}

void PluginGeneratorGUI::saveScriptCode()
{
    QString filename = QFileDialog::getSaveFileName(this,tr("Save Script File"),finfo.absolutePath(),tr("Script File (*.js)"));
    QFile file(filename);
    if (!file.open(QFile::WriteOnly | QIODevice::Text))
        qDebug("Warning: File %s has not been saved.", qUtf8Printable(filename));
    finfo.setFile(filename);
    FilterGeneratorTab* tb = tab(tabs->currentIndex());
    if (tb != NULL)
    {
        QTextStream out(&file);
        out << tb->getCode();
    }
    file.close();
}

void PluginGeneratorGUI::saveAsXMLPlugin()
{
    QFileDialog* saveDiag = new QFileDialog(this,tr("Save XML Plugin File"),finfo.absolutePath(), tr("MeshLab XML Plugin (*.xml)"));
//#if defined(Q_OS_MAC)
    saveDiag->setOption(QFileDialog::DontUseNativeDialog,true);
//#endif
    QLabel* namelab = new QLabel(tr("JScript Plugin Name"),saveDiag);
    QLineEdit* jsline= new QLineEdit(plugscriptname,saveDiag);
    QLabel* authlab = new QLabel(tr("Author Name"),saveDiag);
    QLineEdit* authline= new QLineEdit(author,saveDiag);
    QLabel* maillab = new QLabel(tr("Author e-Mail"),saveDiag);
    QLineEdit* mailline= new QLineEdit(mail,saveDiag);
    QCheckBox* cpp = new QCheckBox(tr("Generate *.cpp,*.h"),saveDiag);
    cpp->setChecked(false);

    QGridLayout* layout = (QGridLayout*) saveDiag->layout();
    layout->addWidget(namelab,4,0);
    layout->addWidget(jsline,4,1);
    layout->addWidget(authlab,5,0);
    layout->addWidget(authline,5,1);
    layout->addWidget(maillab,6,0);
    layout->addWidget(mailline,6,1);
    layout->addWidget(cpp,7,1);

    saveDiag->setAcceptMode(QFileDialog::AcceptSave);
    saveDiag->exec();
    plugscriptname = jsline->text();
    author = authline->text();
    mail = mailline->text();
    QStringList files = saveDiag->selectedFiles();
    if (files.size() != 1)
        return;
    QString fileName = files[0];
    save(fileName,cpp->isChecked());
    /*for(int ii = 0;ii < tabs->count();++ii)
    {
        XMLFilterInfo::XMLMap mp;
        collectFilterInfo(mp);
        ls.push_back(mp);
    }*/
    delete saveDiag;
}

void PluginGeneratorGUI::save( const QString& filepath,const bool generatecpp)
{
    MLXMLTree tree;
    collectInfo(tree);
    if (generatecpp)
    {
        QDir dir(finfo.absolutePath());
        QString cppcode = MLXMLUtilityFunctions::generateCPP(finfo.baseName(),tree);
        QFileInfo cppinfo(finfo.absolutePath() + "/" +finfo.baseName() + ".cpp");
        QString cppoldfilename = UsefulGUIFunctions::generateBackupName(cppinfo);
        dir.rename(cppinfo.fileName(),cppoldfilename);

        QFile cppfile(cppinfo.absoluteFilePath());
        if (!cppfile.open(QIODevice::WriteOnly | QIODevice::Text))
            return;
        QTextStream outcpp(&cppfile);
        outcpp << cppcode;
        cppfile.close();

        QString hcode = MLXMLUtilityFunctions::generateH(finfo.baseName(),tree);
        QFileInfo hinfo(finfo.absolutePath() + "/" +finfo.baseName() + ".h");
        QString holdfilename = UsefulGUIFunctions::generateBackupName(hinfo);
        dir.rename(hinfo.fileName(),holdfilename);
        QFile hfile(hinfo.absoluteFilePath());
        if (!hfile.open(QIODevice::WriteOnly | QIODevice::Text))
            return;
        QTextStream outh(&hfile);
        outh << hcode;
        hfile.close();
    }
    QString xml = generateXML(tree);
    //QDomDocument has been introduced only in order to indent the xml code
    QDomDocument doc;
    doc.setContent(xml,false);
    xml = doc.toString();
    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream out(&file);
    out << xml;
    file.close();
    finfo.setFile(filepath);
}


void PluginGeneratorGUI::loadXMLPlugin()
{
    QString file = QFileDialog::getOpenFileName(this,"Load XML Plugin",finfo.absolutePath(),QString("*.xml"));
    if (!file.isEmpty())
    {
        MLXMLTree tree;
        XMLMessageHandler msg;
        MLXMLPluginInfo* plug = MLXMLPluginInfo::createXMLPluginInfo(file,MLXMLUtilityFunctions::xmlSchemaFile(),msg);
        if (plug != NULL)
        {
            finfo.setFile(file);
            MLXMLUtilityFunctions::loadMeshLabXML(tree,*plug);
            importInfo(tree);
        }
        else
            QMessageBox::warning(0,"MeshLab XML Plugin Import","File " + file + " has not a valid a MeshLab XML Plugin File Format's Syntax.\nError in line " + QString::number(msg.line()) + " column " + QString::number(msg.column()) + " : " + msg.statusMessage());
        MLXMLPluginInfo::destroyXMLPluginInfo(plug);
    }
}

void PluginGeneratorGUI::insertPluginInMeshLab()
{
    saveXMLPlugin();
    emit insertXMLPluginRequested(finfo.absoluteFilePath(),plugscriptname);
}

void PluginGeneratorGUI::collectInfo(MLXMLTree& tree )
{
    tree.interfaceinfo[MLXMLElNames::mfiVersion] = MLXMLElNames::mfiCurrentVersion;
    tree.plugin.pluginfo[MLXMLElNames::pluginScriptName] = plugscriptname;
    tree.plugin.pluginfo[MLXMLElNames::pluginAuthor] = author;
    tree.plugin.pluginfo[MLXMLElNames::pluginEmail] = mail;
    for(int ii = 0;ii < tabs->count();++ii)
    {
        MLXMLFilterSubTree filtertree;
        FilterGeneratorTab* ftab = tab(ii);
        if (ftab != NULL)
        {
            ftab->collectInfo(filtertree);
            tree.plugin.filters.push_back(filtertree);
        }
    }
}

QString PluginGeneratorGUI::generateXML(const MLXMLTree& tree)
{
    return MLXMLUtilityFunctions::generateMeshLabXML(tree);
}

void PluginGeneratorGUI::updateTabTitle( const QString& name,QWidget* wid)
{
    int widind = getPageIndexOfWidget(wid);
    if (widind != -1)
        tabs->setTabText(widind,name);
}

void PluginGeneratorGUI::validateFilterName( const QString& name,FilterGeneratorGUI* wid )
{
    QStringList ls;
//	int widind = getPageIndexOfWidget(wid);
    for(int ii = 0;ii < tabs->count();++ii)
        ls.push_back(tabs->tabText(ii));
    QString res = UsefulGUIFunctions::changeNameIfAlreadyInList(name,ls);
    if (res != name)
    {
        updateTabTitle(res,wid);
        wid->setFilterName(res);
    }
}

int PluginGeneratorGUI::getPageIndexOfWidget( QWidget* wid )
{
    int widind = -1;
    for(int ii = 0;ii < tabs->count();++ii)
    {
        if (wid && tabs->widget(ii)->isAncestorOf(wid))
            widind = ii;
    }
    return widind;
}

void PluginGeneratorGUI::importInfo( const MLXMLTree& tree )
{
    plugscriptname = tree.plugin.pluginfo[MLXMLElNames::pluginScriptName];
    author = tree.plugin.pluginfo[MLXMLElNames::pluginAuthor];
    mail = tree.plugin.pluginfo[MLXMLElNames::pluginEmail];
    int nwtab = tree.plugin.filters.size() - tabs->count();
    for(int ii = 0;ii < nwtab;++ii)
        addNewFilter();
    for(int ii = 0;ii < tree.plugin.filters.size();++ii)
    {
        tabs->setTabText(ii,tree.plugin.filters[ii].filterinfo[MLXMLElNames::filterName]);
        FilterGeneratorTab* gentab = tab(ii);
        gentab->importInfo(tree.plugin.filters[ii]);
    }
}

void PluginGeneratorGUI::setDocument( MeshDocument* mdoc )
{
    doc = mdoc;
}

FilterGeneratorTab* PluginGeneratorGUI::tab(int ii)
{
    return qobject_cast<FilterGeneratorTab*>(tabs->widget(ii));
}

void PluginGeneratorGUI::importHistory()
{
    emit historyRequest();
}

void PluginGeneratorGUI::getHistory( const QStringList& hist )
{
    tab(tabs->currentIndex())->setCode(hist.join("\n"));
}

void PluginGeneratorGUI::resizeEvent( QResizeEvent *)
{
    if (!init)
    {
        setMinimumSize(this->parentWidget()->size().width() * 0.5,0);
        init = true;
    }
}


