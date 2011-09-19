#include "xmlstdpardialog.h"
#include <QtGui>

MeshLabXMLStdDialog::MeshLabXMLStdDialog( Env& envir,QWidget *p )
:QDockWidget(QString("Plugin"), p),env(envir),showHelp(false)
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

void MeshLabXMLStdDialog::loadFrameContent( )
{
	assert(qf);
	qf->hide();
	QLabel *ql;


	QGridLayout *gridLayout = new QGridLayout(qf);
	qf->setLayout(gridLayout);
	QString fname(curmfc->act->text());
	setWindowTitle(fname);
	ql = new QLabel("<i>"+curmfc->xmlInfo->filterHelp(fname)+"</i>",qf);
	ql->setTextFormat(Qt::RichText);
	ql->setWordWrap(true);
	gridLayout->addWidget(ql,0,0,1,2,Qt::AlignTop); // this widgets spans over two columns.

	stdParFrame = new XMLStdParFrame(this,curgla);
	//connect(stdParFrame,SIGNAL(frameEvaluateExpression(const Expression&,Value**)),this,SIGNAL(dialogEvaluateExpression(const Expression&,Value**)),Qt::DirectConnection);

	XMLFilterInfo::XMLMapList mplist = curmfc->xmlInfo->filterParametersExtendedInfo(fname);
	EnvWrap wrap(env);
	stdParFrame->loadFrameContent(mplist,wrap);
	gridLayout->addWidget(stdParFrame,1,0,1,2);

	//int buttonRow = 2;  // the row where the line of buttons start

	QPushButton *helpButton = new QPushButton("Help", qf);
	//helpButton->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Minimum);
	QPushButton *closeButton = new QPushButton("Close", qf);
	//closeButton->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Minimum);
	QPushButton *applyButton = new QPushButton("Apply", qf);
	//applyButton->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Minimum);
	QPushButton *defaultButton = new QPushButton("Default", qf);
	//defaultButton->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Minimum);
	ExpandButtonWidget* exp = new ExpandButtonWidget(qf);
	connect(exp,SIGNAL(expandView(bool)),this,SLOT(extendedView(bool)));

#ifdef Q_WS_MAC
	// Hack needed on mac for correct sizes of button in the bottom of the dialog.
	helpButton->setMinimumSize(100, 25);
	closeButton->setMinimumSize(100,25);
	applyButton->setMinimumSize(100,25);
	defaultButton->setMinimumSize(100, 25);
#endif 	
	QString postCond = curmfc->xmlInfo->filterAttribute(fname,MLXMLElNames::filterPostCond);
	QStringList postCondList = postCond.split(QRegExp("\\W+"), QString::SkipEmptyParts);
	curmask = MeshLabFilterInterface::convertStringListToMeshElementEnum(postCondList);
	if(isDynamic())
	{
		previewCB = new QCheckBox("Preview", qf);
		previewCB->setCheckState(Qt::Unchecked);
		gridLayout->addWidget(previewCB,    gridLayout->rowCount(),0,Qt::AlignBottom);
		connect(previewCB,SIGNAL(toggled(bool)),this,SLOT( togglePreview() ));
		//buttonRow++;
	}

	gridLayout->addWidget(exp,gridLayout->rowCount(),0,1,2,Qt::AlignJustify);
	int firstButLine =  gridLayout->rowCount();
	gridLayout->addWidget(helpButton,   firstButLine,1,Qt::AlignBottom);
	gridLayout->addWidget(defaultButton,firstButLine,0,Qt::AlignBottom);
	int secButLine = gridLayout->rowCount();
	gridLayout->addWidget(closeButton,  secButLine,0,Qt::AlignBottom);
	gridLayout->addWidget(applyButton,  secButLine,1,Qt::AlignBottom);


	connect(helpButton,SIGNAL(clicked()),this,SLOT(toggleHelp()));
	connect(applyButton,SIGNAL(clicked()),this,SLOT(applyClick()));
	connect(closeButton,SIGNAL(clicked()),this,SLOT(closeClick()));
	connect(defaultButton,SIGNAL(clicked()),this,SLOT(resetExpressions()));

	qf->showNormal();
	qf->adjustSize();

	//set the minimum size so it will shrink down to the right size	after the help is toggled
	//this->setMinimumSize(qf->sizeHint());
	this->showNormal();
	this->adjustSize();
	//setSizePolicy(QSizePolicy::Minimum,QSizePolicy::MinimumExpanding);
}

bool MeshLabXMLStdDialog::showAutoDialog(MeshLabXMLFilterContainer& mfc,MeshDocument * md, MainWindowInterface *mwi, QWidget *gla/*=0*/ )
{
	if (mfc.filterInterface == NULL)
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
	curMeshDoc = md;
	curgla=gla;

	QString fname = mfc.act->text();
	//mfi->initParameterSet(action, *mdp, curParSet);
  XMLFilterInfo::XMLMapList mplist = mfc.xmlInfo->filterParametersExtendedInfo(fname);
	curParMap = mplist;
	//curmask = mfc->xmlInfo->filterAttribute(mfc->act->text(),QString("postCond"));
	if(curParMap.isEmpty() && !isDynamic()) 
		return false;

	createFrame();
	loadFrameContent();
	//QString postCond = mfc.xmlInfo->filterAttribute(fname,MLXMLElNames::filterPostCond);
	//QStringList postCondList = postCond.split(QRegExp("\\W+"), QString::SkipEmptyParts);
	//curmask = MeshLabFilterInterface::convertStringListToMeshElementEnum(postCondList);
	if(isDynamic())
	{
		meshState.create(curmask, curModel);
		connect(stdParFrame,SIGNAL(dynamicFloatChanged(int)), this, SLOT(applyDynamic()));
		connect(stdParFrame,SIGNAL(parameterChanged()), this, SLOT(applyDynamic()));
	}
	connect(curMeshDoc, SIGNAL(currentMeshChanged(int)),this, SLOT(changeCurrentMesh(int)));
	return true;
}

void MeshLabXMLStdDialog::applyClick()
{
	env.pushContext();
	assert(curParMap.size() == stdParFrame->xmlfieldwidgets.size());
	for(int ii = 0;ii < curParMap.size();++ii)	
	{
		XMLMeshLabWidget* wid = stdParFrame->xmlfieldwidgets[ii];
		QString exp = wid->getWidgetExpression();
		env.insertExpressionBinding(curParMap[ii][MLXMLElNames::paramName],exp);
		//delete exp;
	}
	////int mask = 0;//curParSet.getDynamicFloatMask();
	if(curmask)	
		meshState.apply(curModel);

	applyContext = env.currentContext()->toString();
	//PreView Caching: if the apply parameters are the same to those used in the preview mode
	//we don't need to reapply the filter to the mesh
	bool isEqual = (applyContext == previewContext);
	if ((isEqual) && (validcache))
		meshCacheState.apply(curModel);
	else
	{
		QString nm = curmfc->act->text();
		EnvWrap wrap(env);
		curmwi->executeFilter(curmfc,wrap,false);
	}
	env.popContext();

	if(curmask)	
		meshState.create(curmask, curModel);
	if(this->curgla) 
		this->curgla->update();
}

void MeshLabXMLStdDialog::closeClick()
{
	if(curmask != MeshModel::MM_UNKNOWN)
		meshState.apply(curModel);
	curmask = MeshModel::MM_UNKNOWN;
	// Perform the update only if there is Valid GLarea. 
	if(this->curgla) 
		this->curgla->update();
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
	if(previewCB->isChecked()) 
	{
		applyDynamic();
	}
	else
		meshState.apply(curModel);

	curgla->update();
}

void MeshLabXMLStdDialog::applyDynamic()
{
	if(!previewCB->isChecked()) 
		return;
	//QAction *q = curAction;
	env.pushContext();
	assert(curParMap.size() == stdParFrame->xmlfieldwidgets.size());
	for(int ii = 0;ii < curParMap.size();++ii)	
	{
		XMLMeshLabWidget* wid = stdParFrame->xmlfieldwidgets[ii];
		QString exp = wid->getWidgetExpression();
		env.insertExpressionBinding(curParMap[ii][MLXMLElNames::paramName],exp);
		//delete exp;
	}
	//two different executions give the identical result if the two contexts (with the filter's parameters inside) are identical
	previewContext = env.currentContext()->toString();
	
	meshState.apply(curModel);
	EnvWrap envir(env);
	curmwi->executeFilter(this->curmfc, envir, true);
	env.pushContext();
	meshCacheState.create(curmask,curModel);
	validcache = true;

	if(this->curgla) 
		this->curgla->update();
}

void MeshLabXMLStdDialog::changeCurrentMesh( int meshInd )
{
	if(isDynamic())
	{
		meshState.apply(curModel);
		curModel=curMeshDoc->meshList.at(meshInd);
		meshState.create(curmask, curModel);
		applyDynamic();
	}
}

//void MeshLabXMLStdDialog::closeEvent( QCloseEvent * event )
//{
//
//}

bool MeshLabXMLStdDialog::isDynamic() const
{
	 return ((curmask != MeshModel::MM_UNKNOWN) && (curmask != MeshModel::MM_NONE) && !(curmask & MeshModel::MM_VERTNUMBER) && !(curmask & MeshModel::MM_FACENUMBER));
}

void MeshLabXMLStdDialog::resetExpressions()
{
	QString fname(curmfc->act->text());
	XMLFilterInfo::XMLMapList mplist = curmfc->xmlInfo->filterParametersExtendedInfo(fname);
	EnvWrap envir(env);
	stdParFrame->resetExpressions(mplist);
}

void MeshLabXMLStdDialog::closeEvent( QCloseEvent * /*event*/ )
{
	closeClick();
}
XMLStdParFrame::XMLStdParFrame( QWidget *p,QWidget *gla/*=0*/ )
:QFrame(p),extended(false)
{
	curr_gla=gla;
	vLayout = new QGridLayout(this);
	vLayout->setAlignment(Qt::AlignTop);
	setLayout(vLayout);
	//connect(p,SIGNAL(expandView(bool)),this,SLOT(expandView(bool)));
	//updateFrameContent(parMap,false);
	//this->setMinimumWidth(vLayout->sizeHint().width());
	

	//this->showNormal();
	this->adjustSize();
}

XMLStdParFrame::~XMLStdParFrame()
{
	
}

void XMLStdParFrame::loadFrameContent(const XMLFilterInfo::XMLMapList& parMap,EnvWrap& envir)
{
	for(XMLFilterInfo::XMLMapList::const_iterator it = parMap.constBegin();it != parMap.constEnd();++it)
	{
		XMLMeshLabWidget* widg = XMLMeshLabWidgetFactory::create(*it,envir,this);
		if (widg == NULL)
			return;
		xmlfieldwidgets.push_back(widg); 
		helpList.push_back(widg->helpLabel());
	}
	//showNormal();
	updateGeometry();
	adjustSize();
}

void XMLStdParFrame::toggleHelp(bool help)
{
	for(int i = 0; i < helpList.count(); i++)
		helpList.at(i)->setVisible(help && xmlfieldwidgets[i]->isVisible()) ;
	updateGeometry();
	adjustSize();
}

void XMLStdParFrame::extendedView(bool ext,bool help)
{
	for(int i = 0; i < xmlfieldwidgets.count(); i++)
		xmlfieldwidgets[i]->setVisibility(ext || xmlfieldwidgets[i]->isImportant); 
	if (help)
		toggleHelp(help);
	updateGeometry();
	adjustSize();
}

void XMLStdParFrame::resetExpressions(const XMLFilterInfo::XMLMapList& mplist)
{
	for(int i = 0; i < xmlfieldwidgets.count(); i++)
		xmlfieldwidgets[i]->set(mplist[i][MLXMLElNames::paramDefExpr]);
}

XMLMeshLabWidget::XMLMeshLabWidget(const XMLFilterInfo::XMLMap& mp,EnvWrap& envir,QWidget* parent )
:QWidget(parent),env(envir)
{
	//WARNING!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//It's not nice at all doing the connection for an external object! The connect should be called in XMLStdParFrame::loadFrameContent but in this way 
	//we must break the construction of the widget in two steps because otherwise in the constructor (called by XMLMeshLabWidgetFactory::create) the emit is invoked
	//before the connection!
	//connect(this,SIGNAL(widgetEvaluateExpression(const Expression&,Value**)),parent,SIGNAL(frameEvaluateExpression(const Expression&,Value**)),Qt::DirectConnection);
	isImportant = env.evalBool(mp[MLXMLElNames::paramIsImportant]);
	setVisible(isImportant);
		
	helpLab = new QLabel("<small>"+ mp[MLXMLElNames::paramHelpTag] +"</small>",this);
	helpLab->setTextFormat(Qt::RichText);
	helpLab->setWordWrap(true);
	helpLab->setVisible(false);
	helpLab->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
	helpLab->setMinimumWidth(250);
	helpLab->setMaximumWidth(QWIDGETSIZE_MAX);
	gridLay = qobject_cast<QGridLayout*>(parent->layout());
	assert(gridLay != 0);

	row = gridLay->rowCount();
	////WARNING!!!!!!!!!!!!!!!!!! HORRIBLE PATCH FOR THE BOOL WIDGET PROBLEM
	if ((row == 1) && (mp[MLXMLElNames::guiType] == MLXMLElNames::checkBoxTag))	
	{

		QLabel* lb = new QLabel("",this);
		gridLay->addWidget(lb);
		gridLay->addWidget(helpLab,row+1,3,1,1,Qt::AlignTop);
	}
	/////////////////////////////////////////////////////////////////////////
	else
		gridLay->addWidget(helpLab,row,3,1,1,Qt::AlignTop);
}

void XMLMeshLabWidget::setVisibility( const bool vis )
{
	helpLabel()->setVisible(helpLabel()->isVisible() && vis);
	updateVisibility(vis);
	setVisible(vis);
}

//void XMLMeshLabWidget::reset()
//{	
//	this->set(map[MLXMLElNames::paramDefExpr]);
//}

XMLCheckBoxWidget::XMLCheckBoxWidget( const XMLFilterInfo::XMLMap& xmlWidgetTag,EnvWrap& envir,QWidget* parent )
:XMLMeshLabWidget(xmlWidgetTag,envir,parent)
{
	cb = new QCheckBox(xmlWidgetTag[MLXMLElNames::guiLabel],this);
	cb->setToolTip(xmlWidgetTag[MLXMLElNames::paramHelpTag]);
	bool defVal = env.evalBool(xmlWidgetTag[MLXMLElNames::paramDefExpr]);
	cb->setChecked(defVal);
	cb->setVisible(isImportant);

	//gridlay->addWidget(this,i,0,1,1,Qt::AlignTop);

	//int row = gridLay->rowCount() -1 ;
	//WARNING!!!!!!!!!!!!!!!!!! HORRIBLE PATCH FOR THE BOOL WIDGET PROBLEM
	if (row == 1)
		gridLay->addWidget(cb,row + 1,0,1,2,Qt::AlignTop);
	/////////////////////////////////////////////////////////////////////////
	else
		gridLay->addWidget(cb,row,0,1,2,Qt::AlignTop);

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

XMLMeshLabWidget* XMLMeshLabWidgetFactory::create(const XMLFilterInfo::XMLMap& widgetTable,EnvWrap& env,QWidget* parent)
{
	QString guiType = widgetTable[MLXMLElNames::guiType];
	if (guiType == MLXMLElNames::editTag)
		return new XMLEditWidget(widgetTable,env,parent);

	if (guiType == MLXMLElNames::checkBoxTag)
		return new XMLCheckBoxWidget(widgetTable,env,parent);

	if (guiType == MLXMLElNames::absPercTag)
		return new XMLAbsWidget(widgetTable,env,parent);
	
	if (guiType == MLXMLElNames::vec3WidgetTag)
		return new XMLVec3Widget(widgetTable,env,parent);

	if (guiType == MLXMLElNames::colorWidgetTag)
		return new XMLColorWidget(widgetTable,env,parent);
	
	if (guiType == MLXMLElNames::sliderWidgetTag)
		return new XMLSliderWidget(widgetTable,env,parent);

	if (guiType == MLXMLElNames::enumWidgetTag)
		return new XMLEnumWidget(widgetTable,env,parent);

	return NULL;
}

XMLEditWidget::XMLEditWidget(const XMLFilterInfo::XMLMap& xmlWidgetTag,EnvWrap& envir,QWidget* parent)
:XMLMeshLabWidget(xmlWidgetTag,envir,parent)
{
	fieldDesc = new QLabel(xmlWidgetTag[MLXMLElNames::guiLabel],this);
	lineEdit = new QLineEdit(this);
	//int row = gridLay->rowCount() -1;

	fieldDesc->setToolTip(xmlWidgetTag[MLXMLElNames::paramHelpTag]);
	lineEdit->setText(xmlWidgetTag[MLXMLElNames::paramDefExpr]);
	
	gridLay->addWidget(fieldDesc,row,0,Qt::AlignTop);
	gridLay->addWidget(lineEdit,row,1,Qt::AlignTop);
	connect(lineEdit,SIGNAL(editingFinished()),parent,SIGNAL(parameterChanged()));
	connect(lineEdit,SIGNAL(selectionChanged()),this,SLOT(tooltipEvaluation()));
	
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

void XMLEditWidget::tooltipEvaluation()
{
	try
	{
		QString exp = lineEdit->selectedText();
		QString res = env.evalString(exp);
		lineEdit->setToolTip(res);
	}
	catch (MeshLabException& /*e*/)
	{
		//WARNING!!! it's needed otherwise there is a stack overflow due to the Qt selection mechanism!
		return;
	}
}

QString XMLEditWidget::getWidgetExpression()
{
	return this->lineEdit->text();
}

void XMLEditWidget::setVisibility( const bool vis )
{
	fieldDesc->setVisible(vis);
	this->lineEdit->setVisible(vis);
}

XMLAbsWidget::XMLAbsWidget(const XMLFilterInfo::XMLMap& xmlWidgetTag, EnvWrap& envir,QWidget* parent )
:XMLMeshLabWidget(xmlWidgetTag,envir,parent)
{
	m_min = env.evalFloat(xmlWidgetTag[MLXMLElNames::guiMinExpr]);
	m_max = env.evalFloat(xmlWidgetTag[MLXMLElNames::guiMaxExpr]);

	fieldDesc = new QLabel(xmlWidgetTag[MLXMLElNames::guiLabel] + " (abs and %)",this);
	fieldDesc->setToolTip(xmlWidgetTag[MLXMLElNames::paramHelpTag]);
	absSB = new QDoubleSpinBox(this);
	percSB = new QDoubleSpinBox(this);

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

	gridLay->addWidget(fieldDesc,row,0,Qt::AlignLeft);
	QGridLayout* lay = new QGridLayout();
	lay->addWidget(absLab,0,0,Qt::AlignHCenter);
	lay->addWidget(percLab,0,1,Qt::AlignHCenter);
	lay->addWidget(absSB,1,0,Qt::AlignTop);
	lay->addWidget(percSB,1,1,Qt::AlignTop);
	gridLay->addLayout(lay,row,1,1,2,Qt::AlignTop);
	

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

ExpandButtonWidget::ExpandButtonWidget( QWidget* parent )
:QWidget(parent),up(0x0035),down(0x0036),isExpanded(false)
{
	arrow = down;
	hlay = new QHBoxLayout(this);
	//QChar ch(0x0036);
	exp = new QPushButton(arrow,this);
	exp->setFlat(true);
	exp->setFont(QFont("Webdings",12));
	//connect(exp,SIGNAL(clicked(bool)),this,SLOT(expandFrame(bool)));
	QFontMetrics mt(exp->font(),exp);
	QSize sz = mt.size(Qt::TextSingleLine,arrow);
	sz.setWidth(sz.width() + 10);
	exp->setMaximumSize(sz);
	hlay->addWidget(exp,0,Qt::AlignHCenter);
	connect(exp,SIGNAL(clicked(bool)),this,SLOT(changeIcon()));
}

ExpandButtonWidget::~ExpandButtonWidget()
{

}

void ExpandButtonWidget::changeIcon()
{
	isExpanded = !isExpanded;
	if (isExpanded)
		arrow = up;
	else
		arrow = down;
	exp->setText(arrow);
	emit expandView(isExpanded);
}

XMLVec3Widget::XMLVec3Widget(const XMLFilterInfo::XMLMap& xmlWidgetTag,EnvWrap& envir,QWidget* p)
:XMLMeshLabWidget(xmlWidgetTag,envir,p)
{
	XMLStdParFrame* par = qobject_cast<XMLStdParFrame*>(p);
	if (par != NULL)
	{
		curr_gla = par->curr_gla;
		paramName = xmlWidgetTag[MLXMLElNames::paramName];
		//int row = gridLay->rowCount() - 1;

		descLab = new QLabel( xmlWidgetTag[MLXMLElNames::guiLabel],p);
		descLab->setToolTip(xmlWidgetTag[MLXMLElNames::paramHelpTag]);
		gridLay->addWidget(descLab,row,0,Qt::AlignTop);

		QHBoxLayout* lay = new QHBoxLayout(p);

		for(int i =0;i<3;++i)
		{
			coordSB[i]= new QLineEdit(p);
			QFont baseFont=coordSB[i]->font();
			if(baseFont.pixelSize() != -1) baseFont.setPixelSize(baseFont.pixelSize()*3/4);
			else baseFont.setPointSize(baseFont.pointSize()*3/4);
			coordSB[i]->setFont(baseFont);
			//coordSB[i]->setMinimumWidth(coordSB[i]->sizeHint().width()/4);
			coordSB[i]->setMinimumWidth(0);
			coordSB[i]->setMaximumWidth(coordSB[i]->sizeHint().width()/2);
			//coordSB[i]->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);
			coordSB[i]->setValidator(new QDoubleValidator(p));
			coordSB[i]->setAlignment(Qt::AlignRight);
			//this->addWidget(coordSB[i],1,Qt::AlignHCenter);
			lay->addWidget(coordSB[i]);
		}
		vcg::Point3f def = envir.evalVec3(xmlWidgetTag[MLXMLElNames::paramDefExpr]);
		this->setPoint(paramName,def);
		if(curr_gla) // if we have a connection to the current glarea we can setup the additional button for getting the current view direction.
		{
			getPoint3Button = new QPushButton("Get",p);
			getPoint3Button->setMaximumWidth(getPoint3Button->sizeHint().width()/2);

			getPoint3Button->setFlat(true);
			//getPoint3Button->setMinimumWidth(getPoint3Button->sizeHint().width());
			//this->addWidget(getPoint3Button,0,Qt::AlignHCenter);
			lay->addWidget(getPoint3Button);
			QStringList names;
			names << "View Dir";
			names << "View Pos";
			names << "Surf. Pos";
			names << "Camera Pos";

			getPoint3Combo = new QComboBox(p);
			getPoint3Combo->addItems(names);
			//getPoint3Combo->setMinimumWidth(getPoint3Combo->sizeHint().width());
			//this->addWidget(getPoint3Combo,0,Qt::AlignHCenter);
			lay->addWidget(getPoint3Combo);

			connect(getPoint3Button,SIGNAL(clicked()),this,SLOT(getPoint()));
			connect(getPoint3Combo,SIGNAL(currentIndexChanged(int)),this,SLOT(getPoint()));
			connect(curr_gla,SIGNAL(transmitViewDir(QString,vcg::Point3f)),this,SLOT(setPoint(QString,vcg::Point3f)));
			connect(curr_gla,SIGNAL(transmitShot(QString,vcg::Shotf)),this,SLOT(setShot(QString,vcg::Shotf)));
			connect(curr_gla,SIGNAL(transmitSurfacePos(QString,vcg::Point3f)),this,SLOT(setPoint(QString,vcg::Point3f)));
			connect(this,SIGNAL(askViewDir(QString)),curr_gla,SLOT(sendViewDir(QString)));
			connect(this,SIGNAL(askViewPos(QString)),curr_gla,SLOT(sendMeshShot(QString)));
			connect(this,SIGNAL(askSurfacePos(QString)),curr_gla,SLOT(sendSurfacePos(QString)));
			connect(this,SIGNAL(askCameraPos(QString)),curr_gla,SLOT(sendCameraPos(QString)));
		}
		gridLay->addLayout(lay,row,1,Qt::AlignTop);
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
		case 0 : emit askViewDir(paramName);		 break;
		case 1 : emit askViewPos(paramName);		 break;
		case 2 : emit askSurfacePos(paramName); break;
		case 3 : emit askCameraPos(paramName); break;
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

XMLColorWidget::XMLColorWidget( const XMLFilterInfo::XMLMap& xmlWidgetTag,EnvWrap& envir,QWidget* p )
:XMLMeshLabWidget(xmlWidgetTag,envir,p)
{
	colorLabel = new QLabel(p);
	QString paramName = xmlWidgetTag[MLXMLElNames::paramName];
	descLabel = new QLabel(xmlWidgetTag[MLXMLElNames::guiLabel],p);
	colorButton = new QPushButton(p);
	colorButton->setAutoFillBackground(true);
	colorButton->setFlat(true);
	//const QColor cl = rp->pd->defVal->getColor();
	//resetWidgetValue();
	QColor cl = envir.evalColor(xmlWidgetTag[MLXMLElNames::paramDefExpr]);
	pickcol = cl;
	updateColorInfo(cl);
	//int row = gridLay->rowCount() - 1;
	gridLay->addWidget(descLabel,row,0,Qt::AlignTop);

	QHBoxLayout* lay = new QHBoxLayout(p);
	lay->addWidget(colorLabel);
	lay->addWidget(colorButton);

	gridLay->addLayout(lay,row,1,Qt::AlignTop);
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

XMLSliderWidget::XMLSliderWidget( const XMLFilterInfo::XMLMap& xmlWidgetTag,EnvWrap& envir,QWidget* p )
:XMLMeshLabWidget(xmlWidgetTag,envir,p)
{
	minVal = env.evalFloat(xmlWidgetTag[MLXMLElNames::guiMinExpr]);
	maxVal = env.evalFloat(xmlWidgetTag[MLXMLElNames::guiMaxExpr]);
	valueLE = new QLineEdit(p);
	valueLE->setAlignment(Qt::AlignRight);
	valueSlider = new QSlider(Qt::Horizontal,p);
	valueSlider->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
	fieldDesc = new QLabel(xmlWidgetTag[MLXMLElNames::guiLabel]);
	valueSlider->setMinimum(0);
	valueSlider->setMaximum(100);
	float fval = env.evalFloat(xmlWidgetTag[MLXMLElNames::paramDefExpr]);
	valueSlider->setValue(floatToInt( fval));
	valueLE->setValidator(new QDoubleValidator (minVal,maxVal, 5, valueLE));
	valueLE->setText(QString::number(fval));


	//int row = gridLay->rowCount() - 1;
	gridLay->addWidget(fieldDesc,row,0,Qt::AlignTop);

	QHBoxLayout* lay = new QHBoxLayout(p);
	lay->addWidget(valueLE,0,Qt::AlignHCenter);
	//lay->addWidget(valueSlider,0,Qt::AlignJustify);
	lay->addWidget(valueSlider,0,0);
	gridLay->addLayout(lay,row,1,Qt::AlignTop);

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

XMLComboWidget::XMLComboWidget( const XMLFilterInfo::XMLMap& xmlWidgetTag,EnvWrap& envir,QWidget* p )
:XMLMeshLabWidget(xmlWidgetTag,envir,p)
{
	enumLabel = new QLabel(p);
	enumLabel->setText(xmlWidgetTag[MLXMLElNames::guiLabel]);
	enumCombo = new QComboBox(p);
	//enumCombo->setCurrentIndex(newEnum);
	//int row = gridLay->rowCount() - 1;
	gridLay->addWidget(enumLabel,row,0,Qt::AlignTop);
	gridLay->addWidget(enumCombo,row,1,Qt::AlignTop);
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

XMLEnumWidget::XMLEnumWidget( const XMLFilterInfo::XMLMap& xmlWidgetTag,EnvWrap& envir,QWidget* p )
:XMLComboWidget(xmlWidgetTag,envir,p)
{
	QString typ = xmlWidgetTag[MLXMLElNames::paramType];
	QMap<int,QString> mp;
	bool rr = MLXMLUtilityFunctions::getEnumNamesValuesFromString(typ,mp);
	if (rr)
	{
		for(QMap<int,QString>::iterator it = mp.begin();it != mp.end();++it)
			enumCombo->addItem(it.value(),QVariant(it.key()));
		enumCombo->setCurrentIndex(env.evalInt(xmlWidgetTag[MLXMLElNames::paramDefExpr]));
	}
}

void XMLEnumWidget::set( const QString& nwExpStr )
{
}

QString XMLEnumWidget::getWidgetExpression()
{
	return enumCombo->itemData(enumCombo->currentIndex()).toString();
}
