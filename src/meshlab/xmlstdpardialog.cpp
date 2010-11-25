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
	XMLFilterInfo::XMLMapList mplist = curmfc->xmlInfo->filterParameters(fname);
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
	XMLFilterInfo::XMLMapList& mplist = mfc->xmlInfo->filterParametersExtendedInfo(fname);
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

}

void MeshLabXMLStdDialog::toggleHelp()
{

}

void MeshLabXMLStdDialog::togglePreview()
{

}

void MeshLabXMLStdDialog::applyDynamic()
{

}

void MeshLabXMLStdDialog::changeCurrentMesh( int meshInd )
{

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
	if(layout()) delete layout();
	QGridLayout * vLayout = new QGridLayout(this);
	vLayout->setAlignment(Qt::AlignTop);

	setLayout(vLayout);

	QString descr;

	//RichWidgetInterfaceConstructor rwc(this);
	//for(int i = 0; i < curParSet.paramList.count(); i++)
	//{
	//	RichParameter* fpi=curParSet.paramList.at(i);
	//	fpi->accept(rwc);
	//	//vLayout->addWidget(rwc.lastCreated,i,0,1,1,Qt::AlignTop);
	//	stdfieldwidgets.push_back(rwc.lastCreated);
	//	helpList.push_back(rwc.lastCreated->helpLab);
	//} // end for each parameter
	//showNormal();
	//adjustSize();
}

XMLMeshLabWidget::XMLMeshLabWidget( QWidget* parent )
:QWidget(parent)
{
	
}

XMLBoolWidget::XMLBoolWidget( const XMLFilterInfo::XMLMap& xmlWidgetTag,QWidget* parent )
:XMLMeshLabWidget(parent)
{
}

XMLBoolWidget::~XMLBoolWidget()
{

}

void XMLBoolWidget::resetWidgetValue()
{

}

void XMLBoolWidget::collectWidgetValue()
{

}

void XMLBoolWidget::setWidgetExpression( const QString& nv )
{

}

void XMLBoolWidget::updateWidget( const XMLFilterInfo::XMLMap& xmlWidgetTag )
{

}

XMLMeshLabWidget* XMLMeshLabWidgetFactory::create(const XMLFilterInfo::XMLMap& widgetTable,QWidget* parent)
{
	QString guiType = widgetTable[MLXMLElNames::guiType];
	if (guiType == MLXMLElNames::editTag)
		return new XMLEditWidget(widgetTable,parent);

	if (guiType == MLXMLElNames::checkBoxTag)
		return new XMLBoolWidget(widgetTable,parent);

	if (guiType == MLXMLElNames::absPercTag)
		return new XMLAbsWidget(widgetTable,parent);
}

XMLEditWidget::XMLEditWidget(const XMLFilterInfo::XMLMap& xmlWidgetTag,QWidget* parent)
:XMLMeshLabWidget(parent)
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
:XMLMeshLabWidget(parent)
{

}

XMLAbsWidget::~XMLAbsWidget()
{

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