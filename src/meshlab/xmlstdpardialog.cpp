#include "xmlstdpardialog.h"
#include <climits>
#include <QColorDialog>
#include <QFileDialog>
#include <QDialogButtonBox>
#include "mainwindow.h"

MeshLabXMLStdDialog::MeshLabXMLStdDialog(QWidget *p)
    :QDockWidget(QString("Plugin"), p),isfilterexecuting(false),env(),showHelp(false),previewCB(NULL)
{
    curmask = 0;
    qf = NULL;
    stdParFrame=NULL;
    clearValues();
}

MeshLabXMLStdDialog::~MeshLabXMLStdDialog()
{

}

void MeshLabXMLStdDialog::clearValues()
{
    //curAction = NULL;
    curModel = NULL;
    curmfc = NULL;
    curmwi = NULL;
}

void MeshLabXMLStdDialog::createFrame()
{
    if(qf) delete qf;

    QFrame *newqf= new QFrame(this);
    setWidget(newqf);
    qf = newqf;
}

void MeshLabXMLStdDialog::loadFrameContent()
{
    assert(qf);
//    qf->hide();
    QLabel *ql;


    QGridLayout *gridLayout = new QGridLayout(qf);
	QString fname(curmfc->filterName());
    setWindowTitle(fname);
    ql = new QLabel("<i>"+curmfc->xmlInfo->filterHelp(fname)+"</i>",qf);
    ql->setTextFormat(Qt::RichText);
    ql->setWordWrap(true);
    gridLayout->addWidget(ql,0,0,1,2,Qt::AlignTop); // this widgets spans over two columns.

    stdParFrame = new XMLStdParFrame(this,curgla);
	connect(stdParFrame, SIGNAL(savePersistentParameterValueRequested(QString, QString)), this, SLOT(savePersistent(QString, QString)));
	connect(stdParFrame, SIGNAL(loadPersistentParameterValueRequested(QString)), this, SLOT(loadPersistent(QString)));

    EnvWrap wrap(env);
	stdParFrame->loadFrameContent(*curmfc, wrap, curMeshDoc);
    gridLayout->addWidget(stdParFrame,1,0,1,2);

    //int buttonRow = 2;  // the row where the line of buttons start

    helpButton = new QPushButton("Help", qf);
    closeButton = new QPushButton("Close", qf);
    applyButton = new QPushButton("Apply", qf);
    defaultButton = new QPushButton("Default", qf);
    applyButton->setFocus();
#ifdef Q_OS_MAC
    // Hack needed on mac for correct sizes of button in the bottom of the dialog.
    helpButton->setStyleSheet("QPushButton {min-height: 30px; } ");
    closeButton->setStyleSheet("QPushButton {min-height: 30px; }");
    applyButton->setStyleSheet("QPushButton {min-height: 30px; }");
    defaultButton->setStyleSheet("QPushButton {min-height: 30px; }");
#endif
    bool onlyimportant = true;
    connect(this->parentWidget(),SIGNAL(filterExecuted()),this,SLOT(postFilterExecution()));
    QString postCond = curmfc->xmlInfo->filterAttribute(fname,MLXMLElNames::filterPostCond);
    QStringList postCondList = postCond.split(QRegExp("\\W+"), QString::SkipEmptyParts);
    curmask = MeshLabFilterInterface::convertStringListToMeshElementEnum(postCondList);
    if(isPreviewable())
    {
        previewCB = new QCheckBox("Preview", qf);
        previewCB->setCheckState(Qt::Unchecked);
        gridLayout->addWidget(previewCB,    gridLayout->rowCount(),0,Qt::AlignBottom);
        connect(previewCB,SIGNAL(toggled(bool)),this,SLOT( togglePreview() ));
        //buttonRow++;
    }

    connect(helpButton,SIGNAL(clicked()),this,SLOT(toggleHelp()));
    connect(closeButton,SIGNAL(clicked()),this,SLOT(closeClick()));
    connect(defaultButton,SIGNAL(clicked()),this,SLOT(resetExpressions()));
    connect(applyButton,SIGNAL(clicked()),this,SLOT(applyClick()));

	MLXMLPluginInfo::XMLMapList mplist = curmfc->xmlInfo->filterParametersExtendedInfo(curmfc->filterName());
    foreach(MLXMLPluginInfo::XMLMap mp,mplist)
    {
		bool important = (wrap.evalBool(mp[MLXMLElNames::paramIsImportant]));
        onlyimportant &= important;
    }
    if (!onlyimportant)
    {
        ExpandButtonWidget* exp = new ExpandButtonWidget(qf);
        connect(exp,SIGNAL(expandView(bool)),this,SLOT(extendedView(bool)));
        gridLayout->addWidget(exp,gridLayout->rowCount(),0,1,2,Qt::AlignJustify);
    }
    int firstButLine =  gridLayout->rowCount();
    gridLayout->addWidget(helpButton,   firstButLine,1,Qt::AlignBottom);
    gridLayout->addWidget(defaultButton,firstButLine,0,Qt::AlignBottom);
    int secButLine = gridLayout->rowCount();
    gridLayout->addWidget(closeButton,  secButLine,0,Qt::AlignBottom);
    gridLayout->addWidget(applyButton,  secButLine,1,Qt::AlignBottom);

    qf->setLayout(gridLayout);
    qf->showNormal();
    this->showNormal();
}

bool MeshLabXMLStdDialog::showAutoDialog(MeshLabXMLFilterContainer& mfc,PluginManager& pm,MeshDocument * md, MainWindowInterface *mwi, QWidget *gla/*=0*/ )
{
    /*if (mfc.filterInterface == NULL)
    return false;*/
    curMeshDoc = md;
    if (curMeshDoc == NULL)
        return false;
    if (mfc.xmlInfo == NULL)
        return false;
    if (mfc.act == NULL)
        return false;

    validcache = false;
    //curAction=mfc.act;
    curmfc=&mfc;
    curmwi=mwi;
    curParMap.clear();
    //prevParMap.clear();
    curModel = md->mm();
    curgla=gla;

    QString fname = mfc.act->text();
    
	curParMap = mfc.xmlInfo->filterParametersExtendedInfo(fname);
    //curmask = mfc->xmlInfo->filterAttribute(mfc->act->text(),QString("postCond"));
    if(curParMap.isEmpty() && !isPreviewable())
        return false;

    GLArea* tmpgl = qobject_cast<GLArea*>(curgla);

	if ((tmpgl != NULL) && (tmpgl->mw() != NULL))
	{
		QMap<QString, QString> persistentparam;
		foreach(RichParameter* rp, tmpgl->mw()->currentGlobalPars().paramList)
		{
			if ((rp != NULL) && (rp->isDerivedFromXMLParam()))
				persistentparam[rp->name] = RichParameterAdapter::convertToStringValue(*rp);
		}

		env.loadMLScriptEnv(*md, pm, persistentparam);
	}
	else
		env.loadMLScriptEnv(*md, pm);

    QTime tt;
    tt.start();
    createFrame();
    loadFrameContent();

    curMeshDoc->Log.Logf(GLLogStream::SYSTEM,"GUI created in %i msec",tt.elapsed());
    //QString postCond = mfc.xmlInfo->filterAttribute(fname,MLXMLElNames::filterPostCond);
    //QStringList postCondList = postCond.split(QRegExp("\\W+"), QString::SkipEmptyParts);
    //curmask = MeshLabFilterInterface::convertStringListToMeshElementEnum(postCondList);
    if(isPreviewable())
    {
        meshState.create(curmask, curModel);
        connect(stdParFrame,SIGNAL(parameterChanged()), this, SLOT(applyDynamic()));
    }
    connect(curMeshDoc, SIGNAL(currentMeshChanged(int)),this, SLOT(changeCurrentMesh(int)));
    raise();
    activateWindow();
    return true;
}

void MeshLabXMLStdDialog::applyClick()
{
	if (stdParFrame == NULL)
		throw MLException("MeshLabXMLStdDialog: stdParFrame is NULL. It should not happen!");

	if ((curmfc == NULL) || (!curmfc->isValid()))
		throw MLException("MeshLabXMLStdDialog: curmfc is NULL and/or not valid. It should not happen!");

    QString fname = curmfc->act->text();
    if ((isfilterexecuting) && (curmfc->xmlInfo->filterAttribute(fname,MLXMLElNames::filterIsInterruptible) == "true"))
    {
        emit filterInterrupt(true);
        return;
    }

    QMap<QString,QString> parvalue;

    for(int ii = 0;ii < curParMap.size();++ii)
    {
		QString exp;

		bool valid = stdParFrame->getValue(curParMap[ii][MLXMLElNames::paramName], exp);
		if (!valid)
		{
			QString err = QString("MeshLabXMLStdDialog: the widget corresponding to the parameter ") + curParMap[ii][MLXMLElNames::paramName] + " has not been found!";
			throw MLException(err.toLocal8Bit());
		}

		parvalue[curParMap[ii][MLXMLElNames::paramName]] = exp;
    }
    emit filterParametersEvaluated(fname,parvalue);
    ////int mask = 0;//curParSet.getDynamicFloatMask();
    if(curmask)
        meshState.apply(curModel);
    //applyContext = env.currentContext()->toString();
    ////PreView Caching: if the apply parameters are the same to those used in the preview mode
    ////we don't need to reapply the filter to the mesh
    //bool isEqual = (applyContext == previewContext);
    //if ((isEqual) && (validcache))
    //	meshCacheState.apply(curModel);
    //else
    //{
    startFilterExecution();
    curmwi->executeFilter(curmfc,parvalue,false);
    /*}*/
    //env.popContext();

    if(curmask)
        meshState.create(curmask, curModel);
	if (this->curgla)
	{
		this->curgla->update();
	}
}

void MeshLabXMLStdDialog::closeClick()
{
    if(curmask != MeshModel::MM_UNKNOWN)
        meshState.apply(curModel);
    curmask = MeshModel::MM_UNKNOWN;

    // Perform the update only if there is Valid GLarea.
	if (this->curgla)
	{
		GLArea* glarea = qobject_cast<GLArea*>(curgla);
		if ((previewCB != NULL) && (previewCB->isChecked()) && (glarea != NULL))
			glarea->updateAllDecorators();
		this->curgla->update();
	}
    close();
}

void MeshLabXMLStdDialog::toggleHelp()
{
    showHelp = !showHelp;
    stdParFrame->toggleHelp(showHelp);
    qf->updateGeometry();
    qf->adjustSize();
    this->updateGeometry();
    this->adjustSize();
}

void MeshLabXMLStdDialog::extendedView(bool ext)
{
    stdParFrame->extendedView(ext,showHelp);
    qf->updateGeometry();
    qf->adjustSize();
    this->updateGeometry();
    this->adjustSize();
}

void MeshLabXMLStdDialog::togglePreview()
{
	if (previewCB == NULL)
		return;

	GLArea* glarea = qobject_cast<GLArea*>(curgla);
    if(previewCB->isChecked())
    {
        applyDynamic();
		if (glarea != NULL)
			glarea->updateAllDecorators();
    }
	else
	{
		meshState.apply(curModel);
		if (glarea != NULL)
			glarea->updateAllDecorators();
	}

    curgla->update();
}

void MeshLabXMLStdDialog::applyDynamic()
{
    if(!previewCB->isChecked())
        return;

	QMap<QString, QString> parvalue;

	for (int ii = 0; ii < curParMap.size(); ++ii)
	{
		if (curParMap[ii][MLXMLElNames::paramIsPersistent] != QString("true"))
		{
			QString exp;

			bool valid = stdParFrame->getValue(curParMap[ii][MLXMLElNames::paramName], exp);
			if (!valid)
			{
				QString err = QString("MeshLabXMLStdDialog: the widget corresponding to the parameter ") + curParMap[ii][MLXMLElNames::paramName] + " has not been found!";
				throw MLException(err.toLocal8Bit());
			}

			parvalue[curParMap[ii][MLXMLElNames::paramName]] = exp;
		}
	}
    //two different executions give the identical result if the two contexts (with the filter's parameters inside) are identical
    //previewContext = env.currentContext()->toString();

    meshState.apply(curModel);
    startFilterExecution();
    curmwi->executeFilter(this->curmfc, parvalue, true);
    //env.pushContext();
    meshCacheState.create(curmask,curModel);
    validcache = true;

    if(this->curgla)
        this->curgla->update();
}

void MeshLabXMLStdDialog::changeCurrentMesh( int meshInd )
{
    if(isPreviewable() && (curModel) && (curModel->id() != meshInd))
    {
        meshState.apply(curModel);
        curModel=curMeshDoc->getMesh(meshInd);
        meshState.create(curmask, curModel);
        applyDynamic();
    }
}

//void MeshLabXMLStdDialog::closeEvent( QCloseEvent * event )
//{
//
//}

bool MeshLabXMLStdDialog::isPreviewable() const
{
    if( (curmask == MeshModel::MM_UNKNOWN) || (curmask == MeshModel::MM_NONE) )
        return false;

    if( (curmask & MeshModel::MM_VERTNUMBER ) ||
        (curmask & MeshModel::MM_FACENUMBER ) )
        return false;

    return true;
}

void MeshLabXMLStdDialog::resetExpressions()
{
    QString fname(curmfc->act->text());
	for (int ii = 0; ii < curParMap.size(); ++ii)
	{
		if (!stdParFrame->setValue(curParMap[ii][MLXMLElNames::paramName], curParMap[ii][MLXMLElNames::paramDefExpr]))
		{
			QString err = QString("MeshLabXMLStdDialog: the widget corresponding to the parameter ") + curParMap[ii][MLXMLElNames::paramName] + " has not been found!";
			throw MLException(err.toLocal8Bit());
		}
	}
}

void MeshLabXMLStdDialog::closeEvent( QCloseEvent * /*event*/ )
{
    closeClick();
}

void MeshLabXMLStdDialog::resetPointers()
{
    curMeshDoc = NULL;
    curgla = NULL;
    curModel = NULL;
}

void MeshLabXMLStdDialog::postFilterExecution()
{
    setDialogStateRelativeToFilterExecution(false);
}

void MeshLabXMLStdDialog::loadPersistent(QString name)
{
	if (curmfc == NULL)
		throw MLException(QString("MeshLabXMLStdDialog: curmfc is NULL!"));

	if (stdParFrame == NULL)
		throw MLException(QString("MeshLabXMLStdDialog: stdParFrame is NULL!"));

	QString valexpr = curmfc->readPersistentValueFromSettings(name);
	stdParFrame->setValue(name, valexpr);
}

void MeshLabXMLStdDialog::savePersistent(QString name, QString expr)
{
	if (curmfc == nullptr)
		throw MLException(QString("MeshLabXMLStdDialog: curmfc is NULL!"));

	if (curmfc->xmlInfo == nullptr)
		throw MLException(QString("MeshLabXMLStdDialog: xmlinfo is NULL!"));

	curmfc->writePersistentValueIntoSettings(name, expr);
}

void MeshLabXMLStdDialog::startFilterExecution()
{
    setDialogStateRelativeToFilterExecution(true);
}

void MeshLabXMLStdDialog::setDialogStateRelativeToFilterExecution( const bool isfilterinexecution )
{
    isfilterexecuting = isfilterinexecution;
    if (curmfc == NULL)
        return;
    QString inter = curmfc->xmlInfo->filterAttribute(curmfc->act->text(),MLXMLElNames::filterIsInterruptible);

    if (inter == "true")
    {
        applyButton->setEnabled(true);
        //during filter execution Stop label should appear, otherwise the apply button is in the usual apply state
        applyButton->setText(MeshLabXMLStdDialog::applyButtonLabel(!isfilterinexecution));
    }
    else
    {
        applyButton->setText(MeshLabXMLStdDialog::applyButtonLabel(true));
        applyButton->setEnabled(!isfilterexecuting);
    }
    stdParFrame->setEnabled(!isfilterexecuting);
}

QString MeshLabXMLStdDialog::applyButtonLabel( const bool applystate )
{
    if (applystate)
        return QString("Apply");
    else
        return QString("Stop");
    return QString();
}

XMLStdParFrame::XMLStdParFrame( QWidget *p,QWidget *gla/*=0*/ )
    :QFrame(p),extended(false)
{
    curr_gla=gla;
    //vLayout = new QGridLayout();
    //vLayout->setAlignment(Qt::AlignTop);
    //setLayout(vLayout);
    //connect(p,SIGNAL(expandView(bool)),this,SLOT(expandView(bool)));
    //updateFrameContent(parMap,false);
    //this->setMinimumWidth(vLayout->sizeHint().width());


    //this->showNormal();
    //this->adjustSize();
}

XMLStdParFrame::~XMLStdParFrame()
{

}

void XMLStdParFrame::loadFrameContent(const MeshLabXMLFilterContainer& filtcont, EnvWrap& envir, MeshDocument* md)
{
	if ((filtcont.act == nullptr) || (filtcont.xmlInfo == nullptr))
		return;

    QGridLayout* glay = new QGridLayout();
    int ii = 0;

	MeshLabXMLStdDialog* parent = qobject_cast<MeshLabXMLStdDialog*>(parentWidget());
	if (parent == NULL)
		return;
	MLXMLPluginInfo::XMLMapList mplist = filtcont.xmlInfo->filterParametersExtendedInfo(filtcont.filterName());
	for (MLXMLPluginInfo::XMLMapList::const_iterator it = mplist.constBegin(); it != mplist.constEnd(); ++it)
    {
		XMLMeshLabWidget* widg = XMLMeshLabWidgetFactory::create(*it, envir, md, this);
		if (widg == NULL)
			return;

		if (envir.evalBool((*it)[MLXMLElNames::paramIsPersistent]))
		{
			QString pers = filtcont.readPersistentValueFromSettings((*it)[MLXMLElNames::paramName]);
			widg->set(pers);
		}

		connect(widg, SIGNAL(loadPersistentParameterValueRequested(QString)), this, SIGNAL(loadPersistentParameterValueRequested(QString)));
		connect(widg, SIGNAL(savePersistentParameterValueRequested(QString)), this, SLOT(savePersistentParameterValue(QString)));
		xmlfieldwidgets[(*it)[MLXMLElNames::paramName]] = widg;
		helpList[(*it)[MLXMLElNames::paramName]] = widg->helpLabel();
		widg->addWidgetToGridLayout(glay, ii);
        ++ii;
    }
    setLayout(glay);
    //showNormal();
    updateGeometry();
    adjustSize();
}

void XMLStdParFrame::toggleHelp(bool help)
{
	for (QMap<QString, QLabel*>::iterator it = helpList.begin(); it != helpList.end(); ++it)
		it.value()->setVisible(help && xmlfieldwidgets[it.key()]->isVisible());
    updateGeometry();
    adjustSize();
}

void XMLStdParFrame::savePersistentParameterValue(QString name)
{
	auto it = xmlfieldwidgets.find(name);

	if (it != xmlfieldwidgets.end())
	{
		XMLMeshLabWidget* widg = it.value();
		if (widg != nullptr)
		{
			/*WARNING!!!! In order to be coherent with the scripting evaluation environment at the value of the XMLStringWidget a pair of double quotes is added at the beginning and at the end of the string*/
			/*The best, and safest way to remove them (if they are not needed), is to let the scripting environment to evaluate the resulting string: Env e; QString st = e.evaluate(string_widg->getWidgetExpr).toString(); */
			QString expr = widg->getWidgetExpression();
			XMLStringWidget* stringwid = qobject_cast<XMLStringWidget*>(widg);
			if (stringwid != nullptr)
			{
				/*WARNING!!!! WHY NOT USING THE ENV PATTERN FOR ALL THE CASES? Because QT when convert a QScriptValue to a QString of a javascript array (eg. [i0,i1,...,in]) return a i0,i1,...,in without the square brackets, making impossible to correctly re evalute again the resulting value*/
				Env e;
				expr = e.evaluate(expr).toString();
			}
			emit savePersistentParameterValueRequested(name,expr);
		}
	}
}

void XMLStdParFrame::extendedView(bool ext,bool help)
{
	for (QMap<QString, XMLMeshLabWidget*>::iterator it = xmlfieldwidgets.begin(); it != xmlfieldwidgets.end(); ++it)
		(*it)->setVisibility(ext || (*it)->isImportant);
    if (help)
        toggleHelp(help);
    updateGeometry();
    adjustSize();
}

bool XMLStdParFrame::setValue(const QString& name, const QString& val)
{
	bool isvalid = false;

	QMap<QString, XMLMeshLabWidget*>::iterator it = xmlfieldwidgets.find(name);
	if ((it != xmlfieldwidgets.end()) && (it.value() != NULL))
	{
		(*it)->set(val);
		isvalid = true;
	}

	return isvalid;
}

bool XMLStdParFrame::getValue(const QString& name, QString& val)
{
	bool isvalid = false;
	QMap<QString, XMLMeshLabWidget*>::iterator it = xmlfieldwidgets.find(name);
	if ((it != xmlfieldwidgets.end()) && (it.value() != NULL))
	{
		val = (*it)->getWidgetExpression();
		isvalid = true;
	}
	return isvalid;
}


XMLMeshLabWidget::XMLMeshLabWidget(const MLXMLPluginInfo::XMLMap& mp,EnvWrap& envir,QWidget* parent )
    :QWidget(parent),env(envir)
{
    //WARNING!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //It's not nice at all doing the connection for an external object! The connect should be called in XMLStdParFrame::loadFrameContent but in this way
    //we must break the construction of the widget in two steps because otherwise in the constructor (called by XMLMeshLabWidgetFactory::create) the emit is invoked
    //before the connection!
    //connect(this,SIGNAL(widgetEvaluateExpression(const Expression&,Value**)),parent,SIGNAL(frameEvaluateExpression(const Expression&,Value**)),Qt::DirectConnection);
	parname = mp[MLXMLElNames::paramName];
	isImportant = env.evalBool(mp[MLXMLElNames::paramIsImportant]);
    setVisible(isImportant);

    helpLab = new QLabel("<small>"+ mp[MLXMLElNames::paramHelpTag] +"</small>",this);
    helpLab->setTextFormat(Qt::RichText);
    helpLab->setWordWrap(true);
    helpLab->setVisible(false);
    helpLab->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    helpLab->setMinimumWidth(250);
    helpLab->setMaximumWidth(QWIDGETSIZE_MAX);
	perstb = new XMLPersistenToolbox(this);
	perstb->setVisible(env.evalBool(mp[MLXMLElNames::paramIsPersistent]));
	perstb->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
	connect(perstb, SIGNAL(saveRequested()), this, SLOT(savePersistentParameterValue()));
	connect(perstb, SIGNAL(loadRequested()), this, SLOT(loadPersistentParameterValue()));
}

void XMLMeshLabWidget::setVisibility( const bool vis )
{
    helpLabel()->setVisible(helpLabel()->isVisible() && vis);
	perstb->setVisible(perstb->isVisible() && vis);
    updateVisibility(vis);
    setVisible(vis);
}

void XMLMeshLabWidget::addWidgetToGridLayout( QGridLayout* lay,const int r )
{
	if (lay != NULL)
		lay->addWidget(helpLab, r, 3, 1, 1);
}

void XMLMeshLabWidget::savePersistentParameterValue()
{
	emit savePersistentParameterValueRequested(parname);
}

void XMLMeshLabWidget::loadPersistentParameterValue()
{
	emit loadPersistentParameterValueRequested(parname);
}

XMLCheckBoxWidget::XMLCheckBoxWidget( const MLXMLPluginInfo::XMLMap& xmlWidgetTag,EnvWrap& envir,QWidget* parent )
    :XMLMeshLabWidget(xmlWidgetTag,envir,parent)
{
    cb = new QCheckBox(xmlWidgetTag[MLXMLElNames::guiLabel],this);
    cb->setToolTip(xmlWidgetTag[MLXMLElNames::paramHelpTag]);
    bool defVal = env.evalBool(xmlWidgetTag[MLXMLElNames::paramDefExpr]);
    cb->setChecked(defVal);
    //cb->setVisible(isImportant);

    ////gridlay->addWidget(this,i,0,1,1,Qt::AlignTop);

    ////int row = gridLay->rowCount() -1 ;
    ////WARNING!!!!!!!!!!!!!!!!!! HORRIBLE PATCH FOR THE BOOL WIDGET PROBLEM
    //if (row == 1)
    //	gridLay->addWidget(cb,row + 1,0,1,2,Qt::AlignTop);
    ///////////////////////////////////////////////////////////////////////////
    //else
    //	gridLay->addWidget(cb,row,0,1,2,Qt::AlignTop);

    setVisibility(isImportant);
    //cb->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    connect(cb,SIGNAL(stateChanged(int)),parent,SIGNAL(parameterChanged()));
}

XMLCheckBoxWidget::~XMLCheckBoxWidget()
{

}

void XMLCheckBoxWidget::set( const QString& nwExpStr )
{
    cb->setChecked(env.evalBool(nwExpStr));
}

void XMLCheckBoxWidget::updateVisibility( const bool vis )
{
    setVisibility(vis);
}

QString XMLCheckBoxWidget::getWidgetExpression()
{
    QString state;
    if (cb->isChecked())
        state = QString("true");
    else
        state = QString("false");
    return state;
}

void XMLCheckBoxWidget::setVisibility( const bool vis )
{
    cb->setVisible(vis);
}

void XMLCheckBoxWidget::addWidgetToGridLayout(QGridLayout* lay, const int r)
{
	if (lay != NULL)
	{
		QHBoxLayout* hlay = new QHBoxLayout();
		hlay->addWidget(cb);
		hlay->addWidget(perstb);
		hlay->addStretch();
		lay->addLayout(hlay, r, 0, 1, 2);
	}
	XMLMeshLabWidget::addWidgetToGridLayout(lay, r);
}

XMLMeshLabWidget* XMLMeshLabWidgetFactory::create(const MLXMLPluginInfo::XMLMap& widgetTable,EnvWrap& env,MeshDocument* md,QWidget* parent)
{
	QString guiType = widgetTable[MLXMLElNames::guiType];
	if (guiType == MLXMLElNames::editTag)
		return new XMLEditWidget(widgetTable, env, parent);

	if (guiType == MLXMLElNames::checkBoxTag)
		return new XMLCheckBoxWidget(widgetTable, env, parent);

	if (guiType == MLXMLElNames::absPercTag)
		return new XMLAbsWidget(widgetTable, env, parent);

	if (guiType == MLXMLElNames::vec3WidgetTag)
		return new XMLVec3Widget(widgetTable, env, parent);

	if (guiType == MLXMLElNames::colorWidgetTag)
		return new XMLColorWidget(widgetTable, env, parent);

	if (guiType == MLXMLElNames::sliderWidgetTag)
		return new XMLSliderWidget(widgetTable, env, parent);

	if (guiType == MLXMLElNames::enumWidgetTag)
		return new XMLEnumWidget(widgetTable, env, parent);

	if (guiType == MLXMLElNames::meshWidgetTag)
		return new XMLMeshWidget(md, widgetTable, env, parent);

	if (guiType == MLXMLElNames::shotWidgetTag)
		return new XMLShotWidget(widgetTable, env, parent);

	if (guiType == MLXMLElNames::stringWidgetTag)
		return new XMLStringWidget(widgetTable, env, parent);
    return NULL;
}

XMLEditWidget::XMLEditWidget(const MLXMLPluginInfo::XMLMap& xmlWidgetTag,EnvWrap& envir,QWidget* parent)
    :XMLMeshLabWidget(xmlWidgetTag,envir,parent)
{
    fieldDesc = new QLabel(xmlWidgetTag[MLXMLElNames::guiLabel],this);
	fieldDesc->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
	lineEdit = new QLineEdit(this);
	lineEdit->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    //int row = gridLay->rowCount() -1;
	QHBoxLayout* hlay = new QHBoxLayout();
	hlay->addWidget(lineEdit);
	
	hlay->addWidget(perstb);

    fieldDesc->setToolTip(xmlWidgetTag[MLXMLElNames::paramHelpTag]);
    lineEdit->setText(xmlWidgetTag[MLXMLElNames::paramDefExpr]);

    //gridLay->addWidget(fieldDesc,row,0,Qt::AlignTop);
    //gridLay->addWidget(lineEdit,row,1,Qt::AlignTop);
    connect(lineEdit,SIGNAL(editingFinished()),parent,SIGNAL(parameterChanged()));
//    connect(lineEdit,SIGNAL(selectionChanged()),this,SLOT(tooltipEvaluation()));

    setVisibility(isImportant);
}



XMLEditWidget::~XMLEditWidget()
{

}



void XMLEditWidget::set( const QString& nwExpStr )
{
    lineEdit->setText(nwExpStr);
}

void XMLEditWidget::updateVisibility( const bool vis )
{
    setVisibility(vis);
}

//void XMLEditWidget::tooltipEvaluation()
//{
//    try
//    {
//        QString exp = lineEdit->selectedText();
//        QString res = env.evalString(exp);
//        lineEdit->setToolTip(res);
//    }
//    catch (MeshLabException& /*e*/)
//    {
//        //WARNING!!! it's needed otherwise there is a stack overflow due to the Qt selection mechanism!
//        return;
//    }
//}

QString XMLEditWidget::getWidgetExpression()
{
    return this->lineEdit->text();
}

void XMLEditWidget::setVisibility( const bool vis )
{
    fieldDesc->setVisible(vis);
    this->lineEdit->setVisible(vis);
}

void XMLEditWidget::addWidgetToGridLayout( QGridLayout* lay,const int r )
{
	if (lay != nullptr)
	{
		lay->addWidget(fieldDesc, r, 0);
		lay->addLayout(hlay, r, 1);
	}
    XMLMeshLabWidget::addWidgetToGridLayout(lay,r);
}

XMLStringWidget::XMLStringWidget(const MLXMLPluginInfo::XMLMap& xmlWidgetTag,EnvWrap& envir,QWidget* parent)
    :XMLMeshLabWidget(xmlWidgetTag,envir,parent)
{
    fieldDesc = new QLabel(xmlWidgetTag[MLXMLElNames::guiLabel],this);

	fieldDesc->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    lineEdit = new QLineEdit(this);
	lineEdit->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    //int row = gridLay->rowCount() -1;
	hlay = new QHBoxLayout();
	hlay->addWidget(lineEdit);
	hlay->addWidget(perstb);


	fieldDesc->setToolTip(xmlWidgetTag[MLXMLElNames::paramHelpTag]);
    lineEdit->setText(xmlWidgetTag[MLXMLElNames::paramDefExpr]);

    //gridLay->addWidget(fieldDesc,row,0,Qt::AlignTop);
    //gridLay->addWidget(lineEdit,row,1,Qt::AlignTop);
    connect(lineEdit,SIGNAL(editingFinished()),parent,SIGNAL(parameterChanged()));
//    connect(lineEdit,SIGNAL(selectionChanged()),this,SLOT(tooltipEvaluation()));

    setVisibility(isImportant);
}



XMLStringWidget::~XMLStringWidget()
{

}



void XMLStringWidget::set( const QString& nwExpStr )
{
    lineEdit->setText(nwExpStr);
}

void XMLStringWidget::updateVisibility( const bool vis )
{
    setVisibility(vis);
}


/*WARNING!!!! In order to be coherent with the scripting evaluation environment at the value of the XMLStringWidget a pair of double quotes is added at the beginning and at the end of the string*/
/*The best, and safest way to remove them (if they are not needed), is to let the scripting environment to evaluate the resulting string: Env e; QString st = e.evaluate(string_widg->getWidgetExpr).toString(); */

QString XMLStringWidget::getWidgetExpression()
{
    return QString("\"")+this->lineEdit->text()+QString("\"");
}

void XMLStringWidget::setVisibility( const bool vis )
{
    fieldDesc->setVisible(vis);
    this->lineEdit->setVisible(vis);
}

void XMLStringWidget::addWidgetToGridLayout( QGridLayout* lay,const int r )
{
    if (lay != nullptr)
    {
        lay->addWidget(fieldDesc,r,0);
		lay->addLayout(hlay, r, 1);
    }
    XMLMeshLabWidget::addWidgetToGridLayout(lay,r);
}



XMLAbsWidget::XMLAbsWidget(const MLXMLPluginInfo::XMLMap& xmlWidgetTag, EnvWrap& envir,QWidget* parent )
    :XMLMeshLabWidget(xmlWidgetTag,envir,parent)
{
    m_min = env.evalFloat(xmlWidgetTag[MLXMLElNames::guiMinExpr]);
    m_max = env.evalFloat(xmlWidgetTag[MLXMLElNames::guiMaxExpr]);

    fieldDesc = new QLabel(xmlWidgetTag[MLXMLElNames::guiLabel] + " (abs and %)",this);
    fieldDesc->setToolTip(xmlWidgetTag[MLXMLElNames::paramHelpTag]);
    absSB = new QDoubleSpinBox(this);
	absSB->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    percSB = new QDoubleSpinBox(this);
	percSB->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    
	absSB->setMinimum(m_min-(m_max-m_min));
    absSB->setMaximum(m_max*2);
    absSB->setAlignment(Qt::AlignRight);

    int decimals= 7-ceil(log10(fabs(m_max-m_min)) ) ;
    //qDebug("range is (%f %f) %f ",m_max,m_min,fabs(m_max-m_min));
    //qDebug("log range is %f ",log10(fabs(m_max-m_min)));
    absSB->setDecimals(decimals);
    absSB->setSingleStep((m_max-m_min)/100.0);
    float initVal = env.evalFloat(xmlWidgetTag[MLXMLElNames::paramDefExpr]);
    absSB->setValue(initVal);

    percSB->setMinimum(-200);
    percSB->setMaximum(200);
    percSB->setAlignment(Qt::AlignRight);
    percSB->setSingleStep(0.5);
    percSB->setValue((100*(initVal - m_min))/(m_max - m_min));
    percSB->setDecimals(3);
    absLab=new QLabel("<i> <small> world unit</small></i>",this);
    percLab=new QLabel("<i> <small> perc on"+QString("(%1 .. %2)").arg(m_min).arg(m_max)+"</small></i>",this);

    //gridLay->addWidget(fieldDesc,row,0,Qt::AlignLeft);
    glay = new QGridLayout();
    glay->addWidget(absLab,0,0,Qt::AlignHCenter);
    glay->addWidget(percLab,0,1,Qt::AlignHCenter);
    glay->addWidget(absSB,1,0,Qt::AlignTop);
    glay->addWidget(percSB,1,1,Qt::AlignTop);
	glay->addWidget(perstb, 0, 2, 2, 1, Qt::AlignVCenter);


    connect(absSB,SIGNAL(valueChanged(double)),this,SLOT(on_absSB_valueChanged(double)));
    connect(percSB,SIGNAL(valueChanged(double)),this,SLOT(on_percSB_valueChanged(double)));
    connect(this,SIGNAL(dialogParamChanged()),parent,SIGNAL(parameterChanged()));
    setVisibility(isImportant);
}

XMLAbsWidget::~XMLAbsWidget()
{
}

void XMLAbsWidget::set( const QString& nwExpStr )
{
    absSB->setValue(env.evalFloat(nwExpStr));
}

void XMLAbsWidget::updateVisibility( const bool vis )
{
    setVisibility(vis);
}

QString XMLAbsWidget::getWidgetExpression()
{
    return QString::number(absSB->value());
}

void XMLAbsWidget::on_absSB_valueChanged(double newv)
{
    percSB->setValue((100*(newv - m_min))/(m_max - m_min));
    emit dialogParamChanged();
}

void XMLAbsWidget::on_percSB_valueChanged(double newv)
{
    absSB->setValue((m_max - m_min)*0.01*newv + m_min);
    emit dialogParamChanged();
}

void XMLAbsWidget::setVisibility( const bool vis )
{
    this->absLab->setVisible(vis);
    this->percLab->setVisible(vis);
    this->fieldDesc->setVisible(vis);
    this->absSB->setVisible(vis);
    this->percSB->setVisible(vis);
}

void XMLAbsWidget::addWidgetToGridLayout( QGridLayout* lay,const int r )
{
    if (lay != NULL)
    {
        lay->addWidget(fieldDesc,r,0,Qt::AlignLeft);
        lay->addLayout(glay,r,1,Qt::AlignTop);
    }
    XMLMeshLabWidget::addWidgetToGridLayout(lay,r);
}

XMLVec3Widget::XMLVec3Widget(const MLXMLPluginInfo::XMLMap& xmlWidgetTag,EnvWrap& envir,QWidget* p)
    :XMLMeshLabWidget(xmlWidgetTag,envir,p)
{
    XMLStdParFrame* par = qobject_cast<XMLStdParFrame*>(p);
    if (par != NULL)
    {
        curr_gla = par->curr_gla;
        paramName = xmlWidgetTag[MLXMLElNames::paramName];
        //int row = gridLay->rowCount() - 1;

        descLab = new QLabel( xmlWidgetTag[MLXMLElNames::guiLabel],this);
        descLab->setToolTip(xmlWidgetTag[MLXMLElNames::paramHelpTag]);
        //gridLay->addWidget(descLab,row,0,Qt::AlignTop);

        hlay = new QHBoxLayout();

        for(int i =0;i<3;++i)
        {
            coordSB[i]= new QLineEdit(this);
            QFont baseFont=coordSB[i]->font();
            if(baseFont.pixelSize() != -1) baseFont.setPixelSize(baseFont.pixelSize()*3/4);
            else baseFont.setPointSize(baseFont.pointSize()*3/4);
            coordSB[i]->setFont(baseFont);
            //coordSB[i]->setMinimumWidth(coordSB[i]->sizeHint().width()/4);
            coordSB[i]->setMinimumWidth(0);
            coordSB[i]->setMaximumWidth(coordSB[i]->sizeHint().width()/2);
            //coordSB[i]->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);
            coordSB[i]->setValidator(new QDoubleValidator(this));
            coordSB[i]->setAlignment(Qt::AlignRight);
            //this->addWidget(coordSB[i],1,Qt::AlignHCenter);
            coordSB[i]->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);
            hlay->addWidget(coordSB[i]);
        }
        vcg::Point3f def = envir.evalVec3(xmlWidgetTag[MLXMLElNames::paramDefExpr]);
        this->setPoint(paramName,def);
        if(curr_gla) // if we have a connection to the current glarea we can setup the additional button for getting the current view direction.
        {
            getPoint3Button = new QPushButton("Get",this);
            getPoint3Button->setMaximumWidth(getPoint3Button->sizeHint().width()/2);

            getPoint3Button->setFlat(true);
            getPoint3Button->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

            hlay->addWidget(getPoint3Button);
            QStringList names;
			names << "View Dir.";
			names << "View Pos.";
			names << "Surf. Pos.";
			names << "Raster Camera Pos.";
			names << "Trackball Center";

            getPoint3Combo = new QComboBox(this);
            getPoint3Combo->addItems(names);
            //getPoint3Combo->setMinimumWidth(getPoint3Combo->sizeHint().width());
            //this->addWidget(getPoint3Combo,0,Qt::AlignHCenter);
            hlay->addWidget(getPoint3Combo);
			hlay->addWidget(perstb, 0, Qt::AlignVCenter);

            connect(getPoint3Button,SIGNAL(clicked()),this,SLOT(getPoint()));
            connect(getPoint3Combo,SIGNAL(currentIndexChanged(int)),this,SLOT(getPoint()));
            connect(curr_gla,SIGNAL(transmitViewDir(QString,vcg::Point3f)),this,SLOT(setPoint(QString,vcg::Point3f)));
            connect(curr_gla,SIGNAL(transmitShot(QString,vcg::Shotf)),this,SLOT(setShot(QString,vcg::Shotf)));
            connect(curr_gla,SIGNAL(transmitSurfacePos(QString,vcg::Point3f)),this,SLOT(setPoint(QString,vcg::Point3f)));
			connect(curr_gla,SIGNAL(transmitCameraPos(QString, Point3m)),this,SLOT(setValue(QString, Point3m)));
			connect(curr_gla,SIGNAL(transmitTrackballPos(QString, Point3m)),this,SLOT(setValue(QString, Point3m)));
            connect(this,SIGNAL(askViewDir(QString)),curr_gla,SLOT(sendViewDir(QString)));
            connect(this,SIGNAL(askViewPos(QString)),curr_gla,SLOT(sendViewerShot(QString)));
            connect(this,SIGNAL(askSurfacePos(QString)),curr_gla,SLOT(sendSurfacePos(QString)));
            connect(this,SIGNAL(askCameraPos(QString)),curr_gla,SLOT(sendRasterShot(QString)));
			connect(this,SIGNAL(askTrackballPos(QString)),curr_gla,SLOT(sendTrackballPos(QString)));
        }
        //gridLay->addLayout(hlay,row,1,Qt::AlignTop);
    }
    setVisibility(isImportant);
}

void XMLVec3Widget::set( const QString& exp )
{
    vcg::Point3f newVal = env.evalVec3(exp);
    for(int ii = 0;ii < 3;++ii)
        coordSB[ii]->setText(QString::number(newVal[ii],'g',4));
}

void XMLVec3Widget::updateVisibility( const bool vis )
{
    setVisibility(vis);
}

QString XMLVec3Widget::getWidgetExpression()
{
    return QString("[" + coordSB[0]->text() + "," + coordSB[1]->text() + "," + coordSB[2]->text() + "]");
}

//void XMLVec3Widget::setExp(const QString& name,const QString& exp )
//{
//	QRegExp pointRegExp("\[\d+(\.\d)*,\d+(\.\d)*,\d+(\.\d)*\]");
//	if ((name==paramName) && (pointRegExp.exactMatch(exp)))
//	{
//		for(int i =0;i<3;++i)
//			coordSB[i]->setText(QString::number(val[i],'g',4));
//	}
//}

void XMLVec3Widget::getPoint()
{
    int index = getPoint3Combo->currentIndex();
    qDebug("Got signal %i",index);
    switch(index)
    {
		case 0 : emit askViewDir(paramName);       break;
		case 1 : emit askViewPos(paramName);       break;
		case 2 : emit askSurfacePos(paramName);    break;
		case 3 : emit askCameraPos(paramName);     break;
		case 4 : emit askTrackballPos(paramName);  break;
		default : assert(0);
    }
}

void XMLVec3Widget::setShot(const QString& name,const vcg::Shotf& shot )
{
    vcg::Point3f p = shot.GetViewPoint();
    setPoint(name,p);
}

void XMLVec3Widget::setPoint( const QString& name,const vcg::Point3f& p )
{
    if (name == paramName)
    {
        QString exp("[" + QString::number(p[0]) + "," + QString::number(p[1]) + "," + QString::number(p[2]) + "]");
        set(exp);
    }
}

XMLVec3Widget::~XMLVec3Widget()
{

}

void XMLVec3Widget::setVisibility( const bool vis )
{
    for(int ii = 0;ii < 3;++ii)
        coordSB[ii]->setVisible(vis);
    getPoint3Button->setVisible(vis);
    getPoint3Combo->setVisible(vis);
    descLab->setVisible(vis);
}

void XMLVec3Widget::addWidgetToGridLayout( QGridLayout* lay,const int r )
{
    if (lay != NULL)
    {
        lay->addWidget(descLab,r,0);
        lay->addLayout(hlay,r,1);
    }
    XMLMeshLabWidget::addWidgetToGridLayout(lay,r);
}

XMLColorWidget::XMLColorWidget( const MLXMLPluginInfo::XMLMap& xmlWidgetTag,EnvWrap& envir,QWidget* p )
    :XMLMeshLabWidget(xmlWidgetTag,envir,p)
{
    colorLabel = new QLabel(this);
    QString paramName = xmlWidgetTag[MLXMLElNames::paramName];
    descLabel = new QLabel(xmlWidgetTag[MLXMLElNames::guiLabel],this);
    colorButton = new QPushButton(this);
    colorButton->setAutoFillBackground(true);
    colorButton->setFlat(true);
	colorButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    //const QColor cl = rp->pd->defVal->getColor();
    //resetWidgetValue();
    QColor cl = envir.evalColor(xmlWidgetTag[MLXMLElNames::paramDefExpr]);
    pickcol = cl;
    updateColorInfo(cl);
    //int row = gridLay->rowCount() - 1;
    //gridLay->addWidget(descLabel,row,0,Qt::AlignTop);

    hlay = new QHBoxLayout();
    QFontMetrics met(colorLabel->font());
    QColor black(Qt::black);
    QString blackname = "(" + black.name() + ")";
    QSize sz = met.size(Qt::TextSingleLine,blackname);
    colorLabel->setMaximumWidth(sz.width());
    colorLabel->setMinimumWidth(sz.width());
    hlay->addWidget(colorLabel,0,Qt::AlignRight);
    hlay->addWidget(colorButton);
	hlay->addWidget(perstb,0,Qt::AlignVCenter);

    //gridLay->addLayout(lay,row,1,Qt::AlignTop);
    connect(colorButton,SIGNAL(clicked()),this,SLOT(pickColor()));
    connect(this,SIGNAL(dialogParamChanged()),p,SIGNAL(parameterChanged()));
    setVisibility(isImportant);
}

XMLColorWidget::~XMLColorWidget()
{

}

void XMLColorWidget::updateVisibility( const bool vis )
{
    setVisibility(vis);
}

void XMLColorWidget::set( const QString& nwExpStr )
{
    QColor col = env.evalColor(nwExpStr);
    updateColorInfo(col);
}

QString XMLColorWidget::getWidgetExpression()
{
    return QString("[" + QString::number(pickcol.red()) + "," + QString::number(pickcol.green()) + "," + QString::number(pickcol.blue()) + "," + QString::number(pickcol.alpha()) + "]");
}

void XMLColorWidget::updateColorInfo( const QColor& col )
{
    colorLabel->setText("("+col.name()+")");
    QPalette palette(col);
    colorButton->setPalette(palette);
}

void XMLColorWidget::pickColor()
{
    pickcol =QColorDialog::getColor(pickcol);
    if(pickcol.isValid())
        updateColorInfo(pickcol);
    emit dialogParamChanged();
}

void XMLColorWidget::setVisibility( const bool vis )
{
    colorLabel->setVisible(vis);
    descLabel->setVisible(vis);
    colorButton->setVisible(vis);
}

void XMLColorWidget::addWidgetToGridLayout( QGridLayout* lay,const int r )
{
    if (lay != NULL)
    {
        lay->addWidget(descLabel,r,0,Qt::AlignTop);
        lay->addLayout(hlay,r,1,Qt::AlignTop);
    }
    XMLMeshLabWidget::addWidgetToGridLayout(lay,r);
}

XMLSliderWidget::XMLSliderWidget( const MLXMLPluginInfo::XMLMap& xmlWidgetTag,EnvWrap& envir,QWidget* p )
    :XMLMeshLabWidget(xmlWidgetTag,envir,p)
{
    minVal = env.evalFloat(xmlWidgetTag[MLXMLElNames::guiMinExpr]);
    maxVal = env.evalFloat(xmlWidgetTag[MLXMLElNames::guiMaxExpr]);
    valueLE = new QLineEdit(this);
    valueLE->setAlignment(Qt::AlignRight);
    valueSlider = new QSlider(Qt::Horizontal,this);
    valueSlider->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);
    fieldDesc = new QLabel(xmlWidgetTag[MLXMLElNames::guiLabel],this);
    valueSlider->setMinimum(0);
    valueSlider->setMaximum(100);
    float fval = env.evalFloat(xmlWidgetTag[MLXMLElNames::paramDefExpr]);
    valueSlider->setValue(floatToInt( fval));
    valueLE->setValidator(new QDoubleValidator (minVal,maxVal, 5, valueLE));
    valueLE->setText(QString::number(fval));


    //int row = gridLay->rowCount() - 1;
    //gridLay->addWidget(fieldDesc,row,0,Qt::AlignTop);

    hlay = new QHBoxLayout();
    hlay->addWidget(valueLE,0,Qt::AlignHCenter);
    //lay->addWidget(valueSlider,0,Qt::AlignJustify);
    hlay->addWidget(valueSlider,0);
	hlay->addWidget(perstb, 0, Qt::AlignVCenter);
    //gridLay->addLayout(hlay,row,1,Qt::AlignTop);

    connect(valueLE,SIGNAL(textChanged(const QString &)),this,SLOT(setValue()));
    connect(valueSlider,SIGNAL(valueChanged(int)),this,SLOT(setValue(int)));
    connect(this,SIGNAL(dialogParamChanged()),p,SIGNAL(parameterChanged()));
    setVisibility(isImportant);
}

XMLSliderWidget::~XMLSliderWidget()
{

}

void XMLSliderWidget::set( const QString& nwExpStr )
{
    float fval = env.evalFloat(nwExpStr);
    valueSlider->setValue(floatToInt(fval));
}

void XMLSliderWidget::updateVisibility( const bool /*vis*/ )
{
    setVisibility(isImportant);
}

QString XMLSliderWidget::getWidgetExpression()
{
    return valueLE->text();
}

void XMLSliderWidget::setValue( int newv )
{
    if( QString::number(intToFloat(newv)) != valueLE->text())
        valueLE->setText(QString::number(intToFloat(newv)));
}

void XMLSliderWidget::setValue( float newValue )
{
    if(floatToInt(float(valueLE->text().toDouble())) != newValue)
        valueLE->setText(QString::number(intToFloat(newValue)));
}

void XMLSliderWidget::setValue()
{
    float newValLE=float(valueLE->text().toDouble());
    valueSlider->setValue(floatToInt(newValLE));
    emit dialogParamChanged();
}

float XMLSliderWidget::intToFloat( int val )
{
    return minVal+float(val)/100.0f*(maxVal-minVal);
}

int XMLSliderWidget::floatToInt( float val )
{
    return int (100.0f*(val-minVal)/(maxVal-minVal));
}

void XMLSliderWidget::setVisibility( const bool vis )
{
    valueLE->setVisible(vis);
    valueSlider->setVisible(vis);
    fieldDesc->setVisible(vis);
}

void XMLSliderWidget::addWidgetToGridLayout( QGridLayout* lay,const int r )
{
    if (lay != NULL)
    {
        lay->addWidget(fieldDesc,r,0);
        lay->addLayout(hlay,r,1);
    }
    XMLMeshLabWidget::addWidgetToGridLayout(lay,r);
}

XMLComboWidget::XMLComboWidget( const MLXMLPluginInfo::XMLMap& xmlWidgetTag,EnvWrap& envir,QWidget* p )
    :XMLMeshLabWidget(xmlWidgetTag,envir,p)
{
    enumLabel = new QLabel(this);
    enumLabel->setText(xmlWidgetTag[MLXMLElNames::guiLabel]);
    enumCombo = new QComboBox(this);
	enumCombo->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	
	hlay = new QHBoxLayout();
	hlay->addWidget(enumCombo);
	hlay->addWidget(perstb);

    int def;
    try
    {
        def = envir.evalInt(xmlWidgetTag[MLXMLElNames::paramDefExpr]);
    }
    catch (ExpressionHasNotThisTypeException& /*ex*/)
    {
        def = 0;
    }
    enumCombo->setCurrentIndex(def);
    //int row = gridLay->rowCount() - 1;
    //gridLay->addWidget(enumLabel,row,0,Qt::AlignTop);
    //gridLay->addWidget(enumCombo,row,1,Qt::AlignTop);
    connect(enumCombo,SIGNAL(activated(int)),this,SIGNAL(dialogParamChanged()));
    connect(this,SIGNAL(dialogParamChanged()),p,SIGNAL(parameterChanged()));
    setVisibility(isImportant);
}

void XMLComboWidget::setVisibility( const bool vis )
{
    enumLabel->setVisible(vis);
    enumCombo->setVisible(vis);
}

void XMLComboWidget::updateVisibility( const bool vis )
{
    setVisibility(vis);
}

QString XMLComboWidget::getWidgetExpression()
{
    return enumCombo->currentText();
}

XMLComboWidget::~XMLComboWidget()
{

}

void XMLComboWidget::addWidgetToGridLayout( QGridLayout* lay,const int r )
{
	if (lay != nullptr)
	{
		lay->addWidget(enumLabel, r, 0);
		lay->addLayout(hlay, r, 1);
	}
    XMLMeshLabWidget::addWidgetToGridLayout(lay,r);
}

XMLEnumWidget::XMLEnumWidget( const MLXMLPluginInfo::XMLMap& xmlWidgetTag,EnvWrap& envir,QWidget* p )
    :XMLComboWidget(xmlWidgetTag,envir,p)
{
    QString typ = xmlWidgetTag[MLXMLElNames::paramType];
    bool rr = MLXMLUtilityFunctions::getEnumNamesValuesFromString(typ,mp);
    if (rr)
    {
        for(QMap<int,QString>::iterator it = mp.begin();it != mp.end();++it)
            enumCombo->addItem(it.value(),QVariant(it.key()));
        enumCombo->setCurrentIndex(env.evalInt(xmlWidgetTag[MLXMLElNames::paramDefExpr]));
    }
}

QString XMLEnumWidget::getWidgetExpression()
{
    return enumCombo->itemData(enumCombo->currentIndex()).toString();
}


void XMLEnumWidget::set(const QString& ind)
{
	int index = ind.toInt();
	auto it = mp.find(index);
	if (it != mp.end())
		enumCombo->setCurrentIndex(index);
}

XMLMeshWidget::XMLMeshWidget( MeshDocument* mdoc,const MLXMLPluginInfo::XMLMap& xmlWidgetTag,EnvWrap& envir,QWidget* p )
    :XMLEnumWidget(xmlWidgetTag,envir,p)
{
    foreach(MeshModel* mm,mdoc->meshList)
        enumCombo->addItem(mm->shortName(),mm->id());
    int def = env.evalInt(xmlWidgetTag[MLXMLElNames::paramDefExpr]);
    if (mdoc->getMesh(def))
        enumCombo->setCurrentIndex(def);
}


XMLShotWidget::XMLShotWidget( const MLXMLPluginInfo::XMLMap& xmlWidgetTag,EnvWrap& envir,QWidget* p )
    :XMLMeshLabWidget(xmlWidgetTag,envir,p)
{
    XMLStdParFrame* par = qobject_cast<XMLStdParFrame*>(p);
    if (par == NULL)
        throw MLException("Critical Error: A widget must have an instance of XMLStdParFrame as parent.");
    gla_curr = par->curr_gla;
    //int row = gridLay->rowCount() - 1;
    this->setShotValue(paramName,Shotm());
    paramName = xmlWidgetTag[MLXMLElNames::paramName];
    descLab = new QLabel(xmlWidgetTag[MLXMLElNames::guiLabel],this);
    //gridLay->addWidget(descLab,row,0,Qt::AlignTop);
    hlay = new QHBoxLayout();
    getShotButton = new QPushButton("Get Shot",this);

    getShotButton->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);
    getShotCombo = new QComboBox(this);
    int def;
    try
    {
        def = envir.evalInt(xmlWidgetTag[MLXMLElNames::paramDefExpr]);
    }
    catch (ExpressionHasNotThisTypeException& /*ex*/)
    {
        def = 0;
    }
    getShotCombo->setCurrentIndex(def);
    //int row = gridLay->rowCount() - 1;
    QStringList names;
    if(gla_curr) // if we have a connection to the current glarea we can setup the additional button for getting the current view direction.
    {
        names << "Current Trackball";
        names << "Current Mesh";
        names << "Current Raster";
        connect(gla_curr,SIGNAL(transmitShot(QString,Shotm)),this,SLOT(setShotValue(QString,Shotm)));
        connect(this,SIGNAL(askViewerShot(QString)),gla_curr,SLOT(sendViewerShot(QString)));
        connect(this,SIGNAL(askMeshShot(QString)),  gla_curr,SLOT(sendMeshShot(QString)));
        connect(this,SIGNAL(askRasterShot(QString)),gla_curr,SLOT(sendRasterShot(QString)));
    }
    names << "From File";
    getShotCombo->addItems(names);
    hlay->addWidget(getShotCombo);
    hlay->addWidget(getShotButton);

    connect(getShotCombo,SIGNAL(activated(int)),this,SIGNAL(dialogParamChanged()));
    connect(this,SIGNAL(dialogParamChanged()),p,SIGNAL(parameterChanged()));
    connect(getShotCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(getShot()));
    connect(getShotButton,SIGNAL(clicked()),this,SLOT(getShot()));
    //gridLay->addLayout(hlay,row,1,Qt::AlignTop);
    setVisibility(isImportant);
}

QString XMLShotWidget::getWidgetExpression()
{
    Matrix44m m = curShot.Extrinsics.Rot();
    Point3m t = curShot.Extrinsics.Tra();
    Scalarm foc = curShot.Intrinsics.FocalMm;
    Point2m pxs = curShot.Intrinsics.PixelSizeMm;
    Point2m cp = curShot.Intrinsics.CenterPx;
    vcg::Point2i vw = curShot.Intrinsics.ViewportPx;
    Point2m dist = curShot.Intrinsics.DistorCenterPx;
    Scalarm* k = curShot.Intrinsics.k;
    QString ms = "new " + MLXMLElNames::shotType + "([";
    for(int ii = 0;ii < 4;++ii)
        for(int jj = 0;jj < 4;++jj)
            ms = ms + QString::number(m[ii][jj]) + ",";
    ms += "],[";
    for(int ii = 0;ii < 3;++ii)
        ms = ms + QString::number(t[ii]) + ",";
    ms += "]," + QString::number(foc) + ",[";
    for(int ii = 0;ii < 2;++ii)
        ms = ms + QString::number(pxs[ii]) + ",";
    ms += "],[";
    for(int ii = 0;ii < 2;++ii)
        ms = ms + QString::number(cp[ii]) + ",";
    ms += "],[";
    for(int ii = 0;ii < 2;++ii)
        ms = ms + QString::number(vw[ii]) + ",";
    ms += "],[";
    for(int ii = 0;ii < 2;++ii)
        ms = ms + QString::number(dist[ii]) + ",";
    ms += "],[";
    for(int ii = 0;ii < 4;++ii)
        ms = ms + QString::number(k[ii]) + ",";
    ms += "])";
    //no ; it will be added by the addExpressionBinding
    return ms;
}

//expr == new CameraShot(rotmat[0],....,rotmat[15],travec[0],..,travec[2],foc,pxs[0],pxs[1],centpxs[0],centpxs[1],viewpxs[0],viewpxs[1],distcent[0],distcent[1],k[0],...,k[3])
void XMLShotWidget::set( const QString & expr )
{
    QString regexpstr = "(new\\s+" + MLXMLElNames::shotType + "\\(|\\)|\\[|\\])";
    QRegExp openexp(regexpstr);
    QString tmp(expr);
    tmp.remove(openexp);
    QStringList numbs = tmp.split(",",QString::SkipEmptyParts);
    if (numbs.size() != 32)
    {
        QString err = "Something bad happened in XMLShotWidget::set function: expected expression should match the following format : new " + MLXMLElNames::shotType + "(rotmat[0],....,rotmat[15],travec[0],..,travec[2],foc,pxs[0],pxs[1],centpxs[0],centpxs[1],viewpxs[0],viewpxs[1],distcent[0],distcent[1],k[0],...,k[3])";
        throw MLException(err);
    }
    int offset = 0;
    Matrix44m rot;
    int ii = 0;
    for(ii = 0;ii < 16;++ii)
    {
        bool ok = false;
        rot[ii / 4][ii % 4] = Scalarm(numbs[ii].toDouble(&ok));
        if (!ok)
        {
            QString err = "Something bad happened in XMLShotWidget::set function: bad value conversion to float.";
            throw MLException(err);
        }
    }
    offset += ii;
    curShot.Extrinsics.SetRot(rot);
    Point3m tra;
    for(ii = 0; ii < 3;++ii)
    {
        bool ok = false;
        tra[ii] = numbs[ii + offset].toFloat(&ok);
        if (!ok)
        {
            QString err = "Something bad happened in XMLShotWidget::set function: bad value conversion to float.";
            throw MLException(err);
        }
    }
    offset += ii;
    curShot.Extrinsics.SetTra(tra);
    bool ok = false;
    curShot.Intrinsics.FocalMm = numbs[offset].toFloat(&ok);
    if (!ok)
    {
        QString err = "Something bad happened in XMLShotWidget::set function: bad value conversion to float.";
        throw MLException(err);
    }
    ++offset;
    Point2m tmp2vcf;
    for(ii = 0; ii < 2;++ii)
    {
        bool ok = false;
        tmp2vcf[ii] = numbs[ii + offset].toFloat(&ok);
        if (!ok)
        {
            QString err = "Something bad happened in XMLShotWidget::set function: bad value conversion to float.";
            throw MLException(err);
        }
    }
    offset += ii;
    curShot.Intrinsics.PixelSizeMm = tmp2vcf;
    for(ii = 0; ii < 2;++ii)
    {
        bool ok = false;
        tmp2vcf[ii] = numbs[ii + offset].toFloat(&ok);
        if (!ok)
        {
            QString err = "Something bad happened in XMLShotWidget::set function: bad value conversion to float.";
            throw MLException(err);
        }
    }
    offset += ii;
    curShot.Intrinsics.CenterPx = tmp2vcf;
    vcg::Point2i tmp2vci;
    for(ii = 0; ii < 2;++ii)
    {
        bool ok = false;
        tmp2vci[ii] = numbs[ii + offset].toInt(&ok);
        if (!ok)
        {
            QString err = "Something bad happened in XMLShotWidget::set function: bad value conversion to float.";
            throw MLException(err);
        }
    }
    offset += ii;
    curShot.Intrinsics.ViewportPx = tmp2vci;
    for(ii = 0; ii < 2;++ii)
    {
        bool ok = false;
        tmp2vcf[ii] = numbs[ii + offset].toFloat(&ok);
        if (!ok)
        {
            QString err = "Something bad happened in XMLShotWidget::set function: bad value conversion to float.";
            throw MLException(err);
        }
    }
    offset += ii;
    curShot.Intrinsics.DistorCenterPx = tmp2vcf;
    for(ii = 0; ii < 4;++ii)
    {
        bool ok = false;
        curShot.Intrinsics.k[ii] = numbs[ii + offset].toFloat(&ok);
        if (!ok)
        {
            QString err = "Something bad happened in XMLShotWidget::set function: bad value conversion to float.";
            throw MLException(err);
        }
    }
    offset += ii;
}

void XMLShotWidget::getShot()
{
    int index = getShotCombo->currentIndex();
    switch(index)  {
    case 0 : emit askViewerShot(paramName); break;
    case 1 : emit askMeshShot(paramName); break;
    case 2 : emit askRasterShot(paramName); break;
    case 3:
        {
            QString filename = QFileDialog::getOpenFileName(this, tr("Load xml camera"), "./", tr("Xml Files (*.xml)"));
            QFile qf(filename);
            QFileInfo qfInfo(filename);

            if( !qf.open(QIODevice::ReadOnly ) )
                return ;

            QDomDocument doc("XmlDocument");    //It represents the XML document
            if(!doc.setContent( &qf ))     return;
            qf.close();

            QString type = doc.doctype().name();

            //TextAlign file project
            //if(type == "RegProjectML")   loadShotFromTextAlignFile(doc);
            //View State file
            //else if(type == "ViewState") loadViewFromViewStateFile(doc);

            //qDebug("End file reading");


            // return true;
        }
    default : assert(0);
    }
}

void XMLShotWidget::setShotValue(QString name,Shotm newVal)
{
    if(name==paramName)
    {
        curShot=newVal;
    }
}

void XMLShotWidget::updateVisibility( const bool vis )
{
    setVisibility(vis);
}

void XMLShotWidget::setVisibility( const bool vis )
{
    descLab->setVisible(vis);
    getShotCombo->setVisible(vis);
    getShotButton->setVisible(vis);
}

void XMLShotWidget::addWidgetToGridLayout( QGridLayout* lay,const int r )
{
    if (lay != NULL)
    {
        lay->addWidget(descLab,r,0);
        lay->addLayout(hlay,r,1);
    }
    XMLMeshLabWidget::addWidgetToGridLayout(lay,r);
}



OldScriptingSystemXMLParamDialog::OldScriptingSystemXMLParamDialog(QMap<QString,QString>& currparamvalues,MeshLabXMLFilterContainer& mfc, PluginManager& pm, MeshDocument * md, MainWindowInterface *mwi, QWidget* p, QWidget *gla)
    : QDialog(p),_env(),_paramvalues(currparamvalues),_stdparframe(NULL),_mfc(mfc),_pm(pm),_meshdocument(md),_mwi(mwi),_gla(gla),_showhelp(false)
{
    createFrame();
    if (mfc.act != NULL)
        setWindowTitle(mfc.act->text());
}

OldScriptingSystemXMLParamDialog::~OldScriptingSystemXMLParamDialog()
{
    delete _stdparframe;
}

// update the values of the widgets with the values in the paramlist;
void OldScriptingSystemXMLParamDialog::resetValues()
{
    if ((_mfc.act != NULL) && (_mfc.xmlInfo != NULL))
    {
        QString fname(_mfc.act->text());
        MLXMLPluginInfo::XMLMapList mplist = _mfc.xmlInfo->filterParametersExtendedInfo(fname);
		for (int ii = 0; ii < mplist.size(); ++ii)
		{
			if (mplist[ii][MLXMLElNames::paramIsPersistent] != QString("true"))
			{
				if (!_stdparframe->setValue(mplist[ii][MLXMLElNames::paramName], mplist[ii][MLXMLElNames::paramDefExpr]))
				{
					QString err = QString("MeshLabXMLStdDialog: the widget corresponding to the parameter ") + mplist[ii][MLXMLElNames::paramName] + " has not been found!";
					throw MLException(err.toLocal8Bit());
				}
			}
		}
    }
}


void OldScriptingSystemXMLParamDialog::createFrame()
{
    //if ((_mfc.act != NULL) && (_mfc.xmlInfo != NULL))
    //{
    //    QString fname(_mfc.act->text());
    //    MLXMLPluginInfo::XMLMapList mplist = _mfc.xmlInfo->filterParametersExtendedInfo(fname);
    //    if (mplist.size() != _paramvalues.size())
    //        MLException("OldScriptingSystemXMLParamDialog::createFrame() : Something really bad happened. The mplist and _paramvalues MUST have the same number of items.");

    //    QVBoxLayout *vboxLayout = new QVBoxLayout(this);
    //    setLayout(vboxLayout);

    //    GLArea* tmpgl = qobject_cast<GLArea*>(_gla);

    //    if (tmpgl != NULL)
    //        _env.loadMLScriptEnv(*_meshdocument,_pm,tmpgl->mw()->currentGlobalPars());
    //    else
    //        _env.loadMLScriptEnv(*_meshdocument,_pm);

    //    EnvWrap envwrap(_env);
    //    _stdparframe = new XMLStdParFrame(this);
    //    _stdparframe->loadFrameContent(mplist, envwrap,_meshdocument);
    //    for(int ii = 0;ii < mplist.size();++ii)
    //    {
    //        QMap<QString,QString>::iterator it = _paramvalues.find(mplist[ii][MLXMLElNames::paramName]);
    //        if (it == _paramvalues.end())
    //        {
    //            QString err = "OldScriptingSystemXMLParamDialog::createFrame() : Something really bad happened. Param " + mplist[ii][MLXMLElNames::paramName] + " has not been found in the _paramvalues map.";
    //            throw MLException(err);
    //        }
    //        _stdparframe->xmlfieldwidgets[ii]->set(it.value());
    //        //in this dialog we will not make distinction between important/not-important parameters
    //        _stdparframe->xmlfieldwidgets[ii]->setVisibility(true);

    //    }
    //    layout()->addWidget(_stdparframe);

    //    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Help | QDialogButtonBox::Ok  | QDialogButtonBox::Cancel );
    //    //add the reset button so we can get its signals
    //    QPushButton *resetButton = buttonBox->addButton(QDialogButtonBox::Reset);
    //    layout()->addWidget(buttonBox);

    //    connect(buttonBox, SIGNAL(accepted()), this, SLOT(getAccept()));
    //    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    //    connect(buttonBox, SIGNAL(helpRequested()), this, SLOT(toggleHelp()));
    //    connect(resetButton, SIGNAL(clicked()), this, SLOT(resetValues()));

    //    setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);

    //    //set the minimum size so it will shrink down to the right size	after the help is toggled
    //    this->setMinimumSize(_stdparframe->sizeHint());
    //    this->showNormal();
    //    this->adjustSize();
    //}
}


void OldScriptingSystemXMLParamDialog::getAccept()
{
    /*if ((_mfc.act != NULL) && (_mfc.xmlInfo != NULL))
    {
        QString fname(_mfc.act->text());
        MLXMLPluginInfo::XMLMapList mplist = _mfc.xmlInfo->filterParametersExtendedInfo(fname);
        if (_stdparframe->xmlfieldwidgets.size() != _paramvalues.size())
            MLException("OldScriptingSystemXMLParamDialog::getAccept() : Something really bad happened. The _stdparframe->xmlfieldwidgets and _paramvalues MUST have the same number of items.");
        for(int ii = 0;ii < mplist.size();++ii)
        {
            QMap<QString,QString>::iterator it = _paramvalues.find(mplist[ii][MLXMLElNames::paramName]);
            if (it == _paramvalues.end())
            {
                QString err = "OldScriptingSystemXMLParamDialog::createFrame() : Something really bad happened. Param " + mplist[ii][MLXMLElNames::paramName] + " has not been found in the _paramvalues map.";
                throw MLException(err);
            }
            it.value() = _stdparframe->xmlfieldwidgets[ii]->getWidgetExpression();
        }
        accept();
    }
    reject();*/
}

void OldScriptingSystemXMLParamDialog::toggleHelp()
{
    _showhelp = !_showhelp;
    _stdparframe->toggleHelp(_showhelp);
    updateGeometry();
    adjustSize();
}

XMLPersistenToolbox::XMLPersistenToolbox(QWidget* parent)
	:QFrame(parent)
{
	//setStyleSheet("QFrame {margin-top:0;margin-bottom:0;margin-right:0;margin-left:0;shape:box}");
	QMargins m(0, 0, 0, 0);
	setContentsMargins(m);
	QVBoxLayout* layout = new QVBoxLayout();
	layout->setSpacing(0);
	QFont f;
	int pointsize = 5;
	int widsize = pointsize * 3;
	f.setPointSize(pointsize);
	QPushButton* savebutton = new QPushButton(tr("S"), this);
	savebutton->setFont(f);
	savebutton->setToolTipDuration(0);
	savebutton->setToolTip(tr("Save"));
	savebutton->setFixedSize(widsize, widsize);
	connect(savebutton, SIGNAL(clicked()), this, SLOT(saveClicked()));
	QPushButton* loadbutton = new QPushButton(tr("L"), this);
	loadbutton->setFont(f);
	loadbutton->setToolTip(tr("Load"));
	loadbutton->setFixedSize(widsize, widsize);
	connect(loadbutton, SIGNAL(clicked()), this, SLOT(loadClicked()));

	layout->addWidget(savebutton);
	layout->addWidget(loadbutton);
	layout->setContentsMargins(m);

	setLayout(layout);
}

void XMLPersistenToolbox::saveClicked()
{
	emit saveRequested();
}

void XMLPersistenToolbox::loadClicked()
{
	emit loadRequested();
}