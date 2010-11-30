#include "xmlstdpardialog.h"
#include <QtGui>

MeshLabXMLStdDialog::MeshLabXMLStdDialog( QWidget *p )
:QDockWidget(QString("Plugin"), p)
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
	curAction = NULL;
	curModel = NULL;
	curmfc = NULL;
	curmwi = NULL;
}

void MeshLabXMLStdDialog::createFrame()
{
	if(qf) delete qf;

	QFrame *newqf= new QFrame(this);
	setWidget(newqf);
	setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
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
	connect(stdParFrame,SIGNAL(frameEvaluateExpression(const Expression&,Value**)),this,SIGNAL(dialogEvaluateExpression(const Expression&,Value**)),Qt::DirectConnection);
	XMLFilterInfo::XMLMapList mplist = curmfc->xmlInfo->filterParametersExtendedInfo(fname);
	stdParFrame->loadFrameContent(mplist);
	gridLayout->addWidget(stdParFrame,1,0,1,2);

	int buttonRow = 2;  // the row where the line of buttons start

	QPushButton *helpButton = new QPushButton("Help", qf);
	QPushButton *closeButton = new QPushButton("Close", qf);
	QPushButton *applyButton = new QPushButton("Apply", qf);
	QPushButton *defaultButton = new QPushButton("Default", qf);

#ifdef Q_WS_MAC
	// Hack needed on mac for correct sizes of button in the bottom of the dialog.
	helpButton->setMinimumSize(100, 25);
	closeButton->setMinimumSize(100,25);
	applyButton->setMinimumSize(100,25);
	defaultButton->setMinimumSize(100, 25);
#endif 	

	if(isDynamic())
	{
		previewCB = new QCheckBox("Preview", qf);
		previewCB->setCheckState(Qt::Unchecked);
		gridLayout->addWidget(previewCB,    buttonRow+0,0,Qt::AlignBottom);
		connect(previewCB,SIGNAL(toggled(bool)),this,SLOT( togglePreview() ));
		buttonRow++;
	}

	gridLayout->addWidget(helpButton,   buttonRow+0,1,Qt::AlignBottom);
	gridLayout->addWidget(defaultButton,buttonRow+0,0,Qt::AlignBottom);
	gridLayout->addWidget(closeButton,  buttonRow+1,0,Qt::AlignBottom);
	gridLayout->addWidget(applyButton,  buttonRow+1,1,Qt::AlignBottom);


	connect(helpButton,SIGNAL(clicked()),this,SLOT(toggleHelp()));
	connect(applyButton,SIGNAL(clicked()),this,SLOT(applyClick()));
	connect(closeButton,SIGNAL(clicked()),this,SLOT(closeClick()));
	connect(defaultButton,SIGNAL(clicked()),this,SLOT(resetValues()));

	qf->showNormal();
	qf->adjustSize();

	//set the minimum size so it will shrink down to the right size	after the help is toggled
	this->setMinimumSize(qf->sizeHint());
	this->showNormal();
	this->adjustSize();
}

bool MeshLabXMLStdDialog::showAutoDialog(MeshLabXMLFilterContainer *mfc,MeshDocument * md, MainWindowInterface *mwi, QWidget *gla/*=0*/ )
{
	if (mfc == NULL) 
		return false;
	if (mfc->filterInterface == NULL)
		return false;
	if (mfc->xmlInfo == NULL)
		return false;
	if (mfc->act == NULL)
		return false;

	validcache = false;
	curAction=mfc->act;
	curmfc=mfc;
	curmwi=mwi;
	curParMap.clear();
	prevParMap.clear();
	curModel = md->mm();
	curMeshDoc = md;
	curgla=gla;

	QString fname = mfc->act->text();
	//mfi->initParameterSet(action, *mdp, curParSet);
  XMLFilterInfo::XMLMapList mplist = mfc->xmlInfo->filterParametersExtendedInfo(fname);
	curParMap = mplist;
	//curmask = mfc->xmlInfo->filterAttribute(mfc->act->text(),QString("postCond"));
	if(curParMap.isEmpty() && !isDynamic()) 
		return false;

	createFrame();
	loadFrameContent();
	QString postCond = mfc->xmlInfo->filterAttribute(fname,MLXMLElNames::filterPostCond);
	QStringList postCondList = postCond.split(QRegExp("\\W+"), QString::SkipEmptyParts);
	curmask = MeshLabFilterInterface::convertStringListToMeshElementEnum(postCondList);
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

}

void MeshLabXMLStdDialog::closeClick()
{

}

void MeshLabXMLStdDialog::resetValues()
{
	//curParSet.clear();
	//curmfi->initParameterSet(curAction, *curMeshDoc, curParSet);

	//assert(qf);
	//assert(qf->isVisible());
	//// assert(curParSet.paramList.count() == stdfieldwidgets.count());
	//stdParFrame->resetValues(curParSet);
}

void MeshLabXMLStdDialog::toggleHelp()
{
	stdParFrame->toggleHelp();
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


XMLStdParFrame::XMLStdParFrame( QWidget *p, QWidget *gla/*=0*/ )
:QFrame(p)
{
	curr_gla=gla;
}

XMLStdParFrame::~XMLStdParFrame()
{

}

void XMLStdParFrame::loadFrameContent(const XMLFilterInfo::XMLMapList& parMap)
{
	delete layout();
	QGridLayout * vLayout = new QGridLayout(this);
	vLayout->setAlignment(Qt::AlignTop);
	setLayout(vLayout);

	QString descr;
	for(XMLFilterInfo::XMLMapList::const_iterator it = parMap.constBegin();it != parMap.constEnd();++it)
	{
		XMLMeshLabWidget* widg = XMLMeshLabWidgetFactory::create(*it,this);
		if (widg == NULL)
			return;
		xmlfieldwidgets.push_back(widg); 
		helpList.push_back(widg->helpLabel());
	}
	showNormal();
	adjustSize();
}

void XMLStdParFrame::toggleHelp()
{
	for(int i = 0; i < helpList.count(); i++)
		helpList.at(i)->setVisible(!helpList.at(i)->isVisible()) ;
	updateGeometry();
	adjustSize();
}

XMLMeshLabWidget::XMLMeshLabWidget(const XMLFilterInfo::XMLMap& mp,QWidget* parent )
:QWidget(parent)
{
	//WARNING!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//It's not nice at all doing the connection for an external object! The connect should be called in XMLStdParFrame::loadFrameContent but in this way 
	//we must break the construction of the widget in two steps because otherwise in the constructor (called by XMLMeshLabWidgetFactory::create) the emit is invoked
	//before the connection!
	connect(this,SIGNAL(widgetEvaluateExpression(const Expression&,Value**)),parent,SIGNAL(frameEvaluateExpression(const Expression&,Value**)),Qt::DirectConnection);

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
	if ((row == 1) && (mp[MLXMLElNames::paramType] == MLXMLElNames::boolType))	
	{

		QLabel* lb = new QLabel("",this);
		gridLay->addWidget(lb);
		gridLay->addWidget(helpLab,row+1,3,1,1,Qt::AlignTop);
	}
	/////////////////////////////////////////////////////////////////////////
	else
		gridLay->addWidget(helpLab,row,3,1,1,Qt::AlignTop);
	updateGeometry();
	adjustSize();
}

XMLCheckBoxWidget::XMLCheckBoxWidget( const XMLFilterInfo::XMLMap& xmlWidgetTag,QWidget* parent )
:XMLMeshLabWidget(xmlWidgetTag,parent)
{
	cb = new QCheckBox(xmlWidgetTag[MLXMLElNames::paramName],this);
	cb->setToolTip(xmlWidgetTag[MLXMLElNames::paramHelpTag]);
	BoolExpression exp(xmlWidgetTag[MLXMLElNames::paramDefExpr]);
	Value* boolVal = NULL;
	emit widgetEvaluateExpression(exp,&boolVal);
	cb->setChecked(boolVal->getBool());
	delete boolVal;


	//gridlay->addWidget(this,i,0,1,1,Qt::AlignTop);

	//int row = gridLay->rowCount() -1 ;
	//WARNING!!!!!!!!!!!!!!!!!! HORRIBLE PATCH FOR THE BOOL WIDGET PROBLEM
	if (row == 1)
		gridLay->addWidget(cb,row + 1,0,1,2,Qt::AlignTop);
	/////////////////////////////////////////////////////////////////////////
	else
		gridLay->addWidget(cb,row,0,1,2,Qt::AlignTop);

	connect(cb,SIGNAL(stateChanged(int)),parent,SIGNAL(parameterChanged()));
}

XMLCheckBoxWidget::~XMLCheckBoxWidget()
{

}

void XMLCheckBoxWidget::resetWidgetValue()
{

}

void XMLCheckBoxWidget::collectWidgetValue()
{

}

void XMLCheckBoxWidget::setWidgetExpression( const QString& nv )
{

}

void XMLCheckBoxWidget::updateWidget( const XMLFilterInfo::XMLMap& xmlWidgetTag )
{

}

XMLMeshLabWidget* XMLMeshLabWidgetFactory::create(const XMLFilterInfo::XMLMap& widgetTable,QWidget* parent)
{
	QString guiType = widgetTable[MLXMLElNames::guiType];
	if (guiType == MLXMLElNames::editTag)
		return new XMLEditWidget(widgetTable,parent);

	if (guiType == MLXMLElNames::checkBoxTag)
		return new XMLCheckBoxWidget(widgetTable,parent);

	if (guiType == MLXMLElNames::absPercTag)
		return new XMLAbsWidget(widgetTable,parent);
	return NULL;
}

XMLEditWidget::XMLEditWidget(const XMLFilterInfo::XMLMap& xmlWidgetTag,QWidget* parent)
:XMLMeshLabWidget(xmlWidgetTag,parent)
{

}

XMLEditWidget::~XMLEditWidget()
{

}

void XMLEditWidget::resetWidgetValue()
{

}

void XMLEditWidget::collectWidgetValue()
{

}


void XMLEditWidget::setWidgetExpression( const QString& nv )
{

}

void XMLEditWidget::updateWidget( const XMLFilterInfo::XMLMap& xmlWidgetTag )
{

}

XMLAbsWidget::XMLAbsWidget(const XMLFilterInfo::XMLMap& xmlWidgetTag, QWidget* parent )
:XMLMeshLabWidget(xmlWidgetTag,parent),minVal(NULL),maxVal(NULL)
{
	FloatExpression minExp(xmlWidgetTag[MLXMLElNames::guiMinExpr]);
	FloatExpression maxExp(xmlWidgetTag[MLXMLElNames::guiMaxExpr]);
	emit widgetEvaluateExpression(minExp,&minVal);
	emit widgetEvaluateExpression(maxExp,&maxVal);


	fieldDesc = new QLabel(xmlWidgetTag[MLXMLElNames::paramName] + " (abs and %)",this);
	fieldDesc->setToolTip(xmlWidgetTag[MLXMLElNames::paramHelpTag]);
	absSB = new QDoubleSpinBox(this);
	percSB = new QDoubleSpinBox(this);

	//called with m_* only to maintain backward compatibility
	float m_min = minVal->getFloat();
	float m_max = maxVal->getFloat();

	absSB->setMinimum(m_min-(m_max-m_min));
	absSB->setMaximum(m_max*2);
	absSB->setAlignment(Qt::AlignRight);

	int decimals= 7-ceil(log10(fabs(m_max-m_min)) ) ;
	//qDebug("range is (%f %f) %f ",m_max,m_min,fabs(m_max-m_min));
	//qDebug("log range is %f ",log10(fabs(m_max-m_min)));
	absSB->setDecimals(decimals);
	absSB->setSingleStep((m_max-m_min)/100.0);
	//float initVal = rp->val->getAbsPerc();
	float initVal = 0.0f;
	absSB->setValue(initVal);

	percSB->setMinimum(-200);
	percSB->setMaximum(200);
	percSB->setAlignment(Qt::AlignRight);
	percSB->setSingleStep(0.5);
	percSB->setValue((100*(initVal - m_min))/(m_max - m_min));
	percSB->setDecimals(3);
	QLabel *absLab=new QLabel("<i> <small> world unit</small></i>");
	QLabel *percLab=new QLabel("<i> <small> perc on"+QString("(%1 .. %2)").arg(m_min).arg(m_max)+"</small></i>");

	gridLay->addWidget(fieldDesc,row,0,Qt::AlignHCenter);

	QGridLayout* lay = new QGridLayout();
	lay->addWidget(absLab,0,0,Qt::AlignHCenter);
	lay->addWidget(percLab,0,1,Qt::AlignHCenter);
	lay->addWidget(absSB,1,0,Qt::AlignTop);
	lay->addWidget(percSB,1,1,Qt::AlignTop);
	gridLay->addLayout(lay,row,1,Qt::AlignTop);
	
        //connect(absSB,SIGNAL(valueChanged(double)),this,SLOT(on_absSB_valueChanged(double)));
        //connect(percSB,SIGNAL(valueChanged(double)),this,SLOT(on_percSB_valueChanged(double)));
        //connect(this,SIGNAL(dialogParamChanged()),parent,SIGNAL(parameterChanged()));
}

XMLAbsWidget::~XMLAbsWidget()
{
	delete minVal;
	delete maxVal;
}

void XMLAbsWidget::resetWidgetValue()
{

}

void XMLAbsWidget::collectWidgetValue()
{

}

void XMLAbsWidget::setWidgetExpression( const QString& nv )
{

}

void XMLAbsWidget::updateWidget( const XMLFilterInfo::XMLMap& xmlWidgetTag )
{

}
