/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2008                                          \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/


#include "stdpardialog.h"
#include <QtGui>
using namespace vcg;
MeshlabStdDialog::MeshlabStdDialog(QWidget *p)
:QDockWidget(QString("Plugin"), p),curmask(MeshModel::MM_UNKNOWN)
{
		qf = NULL;
		stdParFrame=NULL;
		clearValues();
}

StdParFrame::StdParFrame(QWidget *p, QWidget *curr_gla)
//:QDialog(p)
:QFrame(p)
{
	gla=curr_gla;
}


/* manages the setup of the standard parameter window, when the execution of a plugin filter is requested */
bool MeshlabStdDialog::showAutoDialog(MeshFilterInterface *mfi, MeshModel *mm, MeshDocument * mdp, QAction *action, MainWindowInterface *mwi, QWidget *gla)
{
		validcache = false;
		curAction=action;
		curmfi=mfi;
		curmwi=mwi;
		curParSet.clear();
		prevParSet.clear();
		curModel = mm;
		curMeshDoc = mdp;
		curgla=gla;

		mfi->initParameterSet(action, *mdp, curParSet);
        curmask = mfi->postCondition(action);
        if(curParSet.isEmpty() && !isDynamic()) return false;

        createFrame();
		loadFrameContent(mdp);
		if(isDynamic())
		{
			meshState.create(curmask, curModel);
			connect(stdParFrame,SIGNAL(dynamicFloatChanged(int)), this, SLOT(applyDynamic()));
			connect(stdParFrame,SIGNAL(parameterChanged()), this, SLOT(applyDynamic()));
		}
        connect(curMeshDoc, SIGNAL(currentMeshChanged(int)),this, SLOT(changeCurrentMesh(int)));
        return true;
  }

void MeshlabStdDialog::changeCurrentMesh(int meshInd)
{
    if(isDynamic())
    {
        meshState.apply(curModel);
        curModel=curMeshDoc->meshList.at(meshInd);
        meshState.create(curmask, curModel);
        applyDynamic();
    }
}

bool MeshlabStdDialog::isDynamic()
{
    return ((curmask != MeshModel::MM_UNKNOWN) && (curmask != MeshModel::MM_NONE) && !(curmask & MeshModel::MM_VERTNUMBER) && !(curmask & MeshModel::MM_FACENUMBER));
}


void MeshlabStdDialog::clearValues()
{
    curAction = NULL;
    curModel = NULL;
    curmfi = NULL;
    curmwi = NULL;
}

void MeshlabStdDialog::createFrame()
{
	if(qf) delete qf;

	QFrame *newqf= new QFrame(this);
	setWidget(newqf);
	setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
  qf = newqf;
}

// update the values of the widgets with the values in the paramlist;
void MeshlabStdDialog::resetValues()
{
	curParSet.clear();
	curmfi->initParameterSet(curAction, *curMeshDoc, curParSet);

	assert(qf);
	assert(qf->isVisible());
 // assert(curParSet.paramList.count() == stdfieldwidgets.count());
 stdParFrame->resetValues(curParSet);
}

void StdParFrame::resetValues(RichParameterSet &curParSet)
{
	QList<RichParameter*> &parList =curParSet.paramList;
	assert(stdfieldwidgets.size() == parList.size());
	for(int i = 0; i < parList.count(); i++)
	{
		RichParameter* fpi= parList.at(i);
		if (fpi != NULL)	
			stdfieldwidgets[i]->resetValue();
	}
}

	/* creates widgets for the standard parameters */

void MeshlabStdDialog::loadFrameContent(MeshDocument *mdPt)
{
	assert(qf);
	qf->hide();
  QLabel *ql;

	QGridLayout *gridLayout = new QGridLayout(qf);
	qf->setLayout(gridLayout);

	setWindowTitle(curmfi->filterName(curAction));
	ql = new QLabel("<i>"+curmfi->filterInfo(curAction)+"</i>",qf);
	ql->setTextFormat(Qt::RichText);
	ql->setWordWrap(true);
	gridLayout->addWidget(ql,0,0,1,2,Qt::AlignTop); // this widgets spans over two columns.

	stdParFrame = new StdParFrame(this,curgla);
	stdParFrame->loadFrameContent(curParSet,mdPt);
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

//void StdParFrame::loadFrameContent(ParameterDeclarationSet &curParSet,MeshDocument *mdPt)
void StdParFrame::loadFrameContent(RichParameterSet &curParSet,MeshDocument */*mdPt*/)
{
 if(layout()) delete layout();
	QGridLayout * vLayout = new QGridLayout(this);
    vLayout->setAlignment(Qt::AlignTop);
	//QLabel* lb = new QLabel("",this);
	//vLayout->addWidget(lb);
	
	setLayout(vLayout);
	
	//QLabel *ql;

	QString descr;
	RichWidgetInterfaceConstructor rwc(this);
	for(int i = 0; i < curParSet.paramList.count(); i++)
	{
		RichParameter* fpi=curParSet.paramList.at(i);
		fpi->accept(rwc);
		//vLayout->addWidget(rwc.lastCreated,i,0,1,1,Qt::AlignTop);
		stdfieldwidgets.push_back(rwc.lastCreated);
		helpList.push_back(rwc.lastCreated->helpLab);
	} // end for each parameter
	showNormal();
	adjustSize();
}

void StdParFrame::loadFrameContent( RichParameter* par,MeshDocument */*mdPt*/ /*= 0*/ )
{
	if(layout()) delete layout();
	QGridLayout * vLayout = new QGridLayout(this);
    vLayout->setAlignment(Qt::AlignTop);
	setLayout(vLayout);

	QString descr;
	RichWidgetInterfaceConstructor rwc(this);

		par->accept(rwc);
		//vLayout->addWidget(rwc.lastCreated,i,0,1,1,Qt::AlignTop);
		stdfieldwidgets.push_back(rwc.lastCreated);
		helpList.push_back(rwc.lastCreated->helpLab);
	showNormal();
	adjustSize();
}
void StdParFrame::toggleHelp()
{
	for(int i = 0; i < helpList.count(); i++)
		helpList.at(i)->setVisible(!helpList.at(i)->isVisible()) ;
	updateGeometry();
  adjustSize();
}

void MeshlabStdDialog::toggleHelp()
{
	stdParFrame->toggleHelp();
	qf->updateGeometry();	
	qf->adjustSize();
	this->updateGeometry();
	this->adjustSize();
}

//void StdParFrame::readValues(ParameterDeclarationSet &curParSet)
void StdParFrame::readValues(RichParameterSet &curParSet)
{
	QList<RichParameter*> &parList =curParSet.paramList;
	assert(parList.size() == stdfieldwidgets.size());
	QVector<MeshLabWidget*>::iterator it = stdfieldwidgets.begin(); 
	for(int i = 0; i < parList.count(); i++)
	 {
			QString sname = parList.at(i)->name;
			curParSet.setValue(sname,(*it)->getWidgetValue());
			++it;
	 }
}

StdParFrame::~StdParFrame()
{

}

/* click event for the apply button of the standard plugin window */
// If the filter has some dynamic parameters
// - before applying the filter restore the original state of the mesh.
// - after applying the filter reget the state of the mesh.

void MeshlabStdDialog::applyClick()
{
	QAction *q = curAction;
	stdParFrame->readValues(curParSet);
	
	////int mask = 0;//curParSet.getDynamicFloatMask();
	if(curmask)	meshState.apply(curModel);

	//PreView Caching: if the apply parameters are the same to those used in the preview mode
	//we don't need to reapply the filter to the mesh
	bool isEqual = (curParSet == prevParSet);
        if ((isEqual) && (validcache))
		meshCacheState.apply(curModel);
	else
		curmwi->executeFilter(q, curParSet, false);

	if(curmask)	meshState.create(curmask, curModel);
	if(this->curgla) this->curgla->update();

}

void MeshlabStdDialog::applyDynamic()
{
	if(!previewCB->isChecked()) return;
	QAction *q = curAction;
	stdParFrame->readValues(curParSet);
	//for cache mechanism
	//needed to allocate the required memory space in prevParSet
	//it called the operator=(RichParameterSet) function defined in RichParameterSet
	prevParSet = curParSet;
	stdParFrame->readValues(prevParSet);
	// Restore the
	meshState.apply(curModel);
	curmwi->executeFilter(q, curParSet, true);
	meshCacheState.create(curmask,curModel);
	validcache = true;
	

	if(this->curgla) 
		this->curgla->update();
}

void MeshlabStdDialog::togglePreview()
{
	if(previewCB->isChecked()) 
	{
		applyDynamic();
	}
	else
		meshState.apply(curModel);
	
	curgla->update();
}

/* click event for the close button of the standard plugin window */

void MeshlabStdDialog::closeClick()
{
	//int mask = 0;//curParSet.getDynamicFloatMask();
	if(curmask)	meshState.apply(curModel);
	curmask = MeshModel::MM_UNKNOWN;
	// Perform the update only if there is Valid GLarea. 
	if(this->curgla) this->curgla->update();
	close();
}


// click event for the standard red crossed close button in the upper right widget's corner
void MeshlabStdDialog::closeEvent(QCloseEvent * /*event*/)
{
	closeClick();
}

MeshlabStdDialog::~MeshlabStdDialog()
{
	delete stdParFrame;
	if(isDynamic()) 
		delete previewCB;
}


/******************************************/
// AbsPercWidget Implementation
/******************************************/


//QGridLayout(NULL)
  AbsPercWidget::AbsPercWidget(QWidget *p, RichAbsPerc* rabs):MeshLabWidget(p,rabs)

  {
		AbsPercDecoration* absd = reinterpret_cast<AbsPercDecoration*>(rp->pd);
	  m_min = absd->min;
	  m_max = absd->max;

 		fieldDesc = new QLabel(rp->pd->fieldDesc + " (abs and %)",p);
		fieldDesc->setToolTip(rp->pd->tooltip);
	  absSB = new QDoubleSpinBox(p);
	  percSB = new QDoubleSpinBox(p);

	  absSB->setMinimum(m_min-(m_max-m_min));
	  absSB->setMaximum(m_max*2);
	  absSB->setAlignment(Qt::AlignRight);
		
		int decimals= 7-ceil(log10(fabs(m_max-m_min)) ) ;
	//qDebug("range is (%f %f) %f ",m_max,m_min,fabs(m_max-m_min));
	//qDebug("log range is %f ",log10(fabs(m_max-m_min)));
	  absSB->setDecimals(decimals);
	  absSB->setSingleStep((m_max-m_min)/100.0);
	  float initVal = rp->val->getAbsPerc();
		absSB->setValue(initVal);

	  percSB->setMinimum(-200);
	  percSB->setMaximum(200);
	  percSB->setAlignment(Qt::AlignRight);
		percSB->setSingleStep(0.5);
	  percSB->setValue((100*(initVal - m_min))/(m_max - m_min));
		percSB->setDecimals(3);
		QLabel *absLab=new QLabel("<i> <small> world unit</small></i>");
		QLabel *percLab=new QLabel("<i> <small> perc on"+QString("(%1 .. %2)").arg(m_min).arg(m_max)+"</small></i>");

		//int row = gridLay->rowCount() - 1;
		gridLay->addWidget(fieldDesc,row,0,Qt::AlignHCenter);

		QGridLayout* lay = new QGridLayout(p);
		lay->addWidget(absLab,0,0,Qt::AlignHCenter);
		lay->addWidget(percLab,0,1,Qt::AlignHCenter);

	  lay->addWidget(absSB,1,0,Qt::AlignTop);
	  lay->addWidget(percSB,1,1,Qt::AlignTop);

		gridLay->addLayout(lay,row,1,Qt::AlignTop);

		connect(absSB,SIGNAL(valueChanged(double)),this,SLOT(on_absSB_valueChanged(double)));
		connect(percSB,SIGNAL(valueChanged(double)),this,SLOT(on_percSB_valueChanged(double)));
		connect(this,SIGNAL(dialogParamChanged()),p,SIGNAL(parameterChanged()));
  }

  AbsPercWidget::~AbsPercWidget()
  {
	  delete absSB;
	  delete percSB;
		delete fieldDesc;
  }


void AbsPercWidget::on_absSB_valueChanged(double newv)
{
	percSB->setValue((100*(newv - m_min))/(m_max - m_min));
	emit dialogParamChanged();
}

void AbsPercWidget::on_percSB_valueChanged(double newv)
{
	absSB->setValue((m_max - m_min)*0.01*newv + m_min);
	emit dialogParamChanged();
}

//float AbsPercWidget::getValue()
//{
//	return float(absSB->value());
//}

void AbsPercWidget::setValue(float val, float minV, float maxV)
{
	assert(absSB);
	absSB->setValue (val);
	m_min=minV;
	m_max=maxV;
}

void AbsPercWidget::collectWidgetValue()
{
	rp->val->set(AbsPercValue(float(absSB->value())));
}

void AbsPercWidget::resetWidgetValue()
{
	const AbsPercDecoration* absd = reinterpret_cast<const AbsPercDecoration*>(&(rp->pd));
	setValue(rp->pd->defVal->getAbsPerc(),absd->min,absd->max);	
}

void AbsPercWidget::setWidgetValue( const Value& nv )
{
	const AbsPercDecoration* absd = reinterpret_cast<const AbsPercDecoration*>(&(rp->pd));
	setValue(nv.getAbsPerc(),absd->min,absd->max);	
}

/******************************************/
// Point3fWidget Implementation
/******************************************/


//QHBoxLayout(NULL)
Point3fWidget::Point3fWidget(QWidget *p, RichPoint3f* rpf, QWidget *gla_curr): MeshLabWidget(p,rpf)
{

	paramName = rpf->name;
	//int row = gridLay->rowCount() - 1;

	descLab = new QLabel(rpf->pd->fieldDesc,p);
	descLab->setToolTip(rpf->pd->fieldDesc);
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
	this->setValue(paramName,rp->val->getPoint3f());
	if(gla_curr) // if we have a connection to the current glarea we can setup the additional button for getting the current view direction.
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
			connect(gla_curr,SIGNAL(transmitViewDir(QString,vcg::Point3f)),this,SLOT(setValue(QString,vcg::Point3f)));
			connect(gla_curr,SIGNAL(transmitViewPos(QString,vcg::Point3f)),this,SLOT(setValue(QString,vcg::Point3f)));
			connect(gla_curr,SIGNAL(transmitSurfacePos(QString,vcg::Point3f)),this,SLOT(setValue(QString,vcg::Point3f)));
			connect(this,SIGNAL(askViewDir(QString)),gla_curr,SLOT(sendViewDir(QString)));
			connect(this,SIGNAL(askViewPos(QString)),gla_curr,SLOT(sendViewPos(QString)));
			connect(this,SIGNAL(askSurfacePos(QString)),gla_curr,SLOT(sendSurfacePos(QString)));
			connect(this,SIGNAL(askCameraPos(QString)),gla_curr,SLOT(sendCameraPos(QString)));
		}
	gridLay->addLayout(lay,row,1,Qt::AlignTop);
}

void Point3fWidget::getPoint()
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

Point3fWidget::~Point3fWidget() {}

void Point3fWidget::setValue(QString name,Point3f newVal)
{
	if(name==paramName)
	{
		for(int i =0;i<3;++i)
			coordSB[i]->setText(QString::number(newVal[i],'g',4));
	}
}

vcg::Point3f Point3fWidget::getValue()
{
	return Point3f(coordSB[0]->text().toFloat(),coordSB[1]->text().toFloat(),coordSB[2]->text().toFloat());
}

void Point3fWidget::collectWidgetValue()
{
	rp->val->set(Point3fValue(vcg::Point3f(coordSB[0]->text().toFloat(),coordSB[1]->text().toFloat(),coordSB[2]->text().toFloat())));
}

void Point3fWidget::resetWidgetValue()
{
	for(unsigned int ii = 0; ii < 3;++ii)
		coordSB[ii]->setText(QString::number(rp->pd->defVal->getPoint3f()[ii],'g',3));
}

void Point3fWidget::setWidgetValue( const Value& nv )
{
	for(unsigned int ii = 0; ii < 3;++ii)
		coordSB[ii]->setText(QString::number(nv.getPoint3f()[ii],'g',3));
}

ComboWidget::ComboWidget(QWidget *p, RichParameter* rpar) :MeshLabWidget(p,rpar) {
}

void ComboWidget::Init(QWidget *p,int defaultEnum, QStringList values)
{
  enumLabel = new QLabel(p);
	enumLabel->setText(rp->pd->fieldDesc);
	enumCombo = new QComboBox(p);
  enumCombo->addItems(values);
	setIndex(defaultEnum);
	//int row = gridLay->rowCount() - 1;
	gridLay->addWidget(enumLabel,row,0,Qt::AlignTop);
	gridLay->addWidget(enumCombo,row,1,Qt::AlignTop);
	connect(enumCombo,SIGNAL(activated(int)),this,SIGNAL(dialogParamChanged()));
	connect(this,SIGNAL(dialogParamChanged()),p,SIGNAL(parameterChanged()));
}

void ComboWidget::setIndex(int newEnum)
{
	enumCombo->setCurrentIndex(newEnum);
}

int ComboWidget::getIndex()
{
	return enumCombo->currentIndex();
}

ComboWidget::~ComboWidget()
{
	delete enumCombo;
	delete enumLabel;
}

/******************************************/
//EnumWidget Implementation
/******************************************/

EnumWidget::EnumWidget(QWidget *p, RichEnum* rpar)
:ComboWidget(p,rpar)
{
	//you MUST call it!!!!
	Init(p,rpar->val->getEnum(),reinterpret_cast<EnumDecoration*>(rpar->pd)->enumvalues);
	//assert(enumCombo != NULL);
}

int EnumWidget::getSize()
{
	return enumCombo->count();
}


void EnumWidget::collectWidgetValue()
{
	rp->val->set(EnumValue(enumCombo->currentIndex()));
}

void EnumWidget::resetWidgetValue()
{
	//lned->setText(QString::number(rp->val->getFloat(),'g',3));
	enumCombo->setCurrentIndex(rp->pd->defVal->getEnum());
}

void EnumWidget::setWidgetValue( const Value& nv )
{
	enumCombo->setCurrentIndex(nv.getEnum());
}

/******************************************/
//MeshEnumWidget Implementation
/******************************************/


MeshWidget::MeshWidget(QWidget *p, RichMesh* rpar)
:ComboWidget(p,rpar) 
{
	md=reinterpret_cast<MeshDecoration*>(rp->pd)->meshdoc;

	QStringList meshNames;

	//make the default mesh Index be 0
	defaultMeshIndex = -1;

	for(int i=0;i<md->meshList.size();++i)
	 {
        QString shortName = md->meshList.at(i)->shortName();
		meshNames.push_back(shortName);
		if(md->meshList.at(i) == rp->pd->defVal->getMesh()) defaultMeshIndex = i;
	 }

	//add a blank choice because there is no default available
	if(defaultMeshIndex == -1)
	{
		meshNames.push_back("");
		defaultMeshIndex = meshNames.size()-1;  //have the blank choice be shown
	}
	Init(p,defaultMeshIndex,meshNames);
}

MeshModel * MeshWidget::getMesh()
{
	//test to make sure index is in bounds
	int index = enumCombo->currentIndex();
	if(index < md->meshList.size() && index > -1)
		return md->meshList.at(enumCombo->currentIndex());
	else return NULL;
}

void MeshWidget::setMesh(MeshModel * newMesh)
{
	for(int i=0; i < md->meshList.size(); ++i)
	{
		if(md->meshList.at(i) == newMesh) 
			setIndex(i);
	}
}

void MeshWidget::collectWidgetValue()
{
	MeshDecoration* dec = reinterpret_cast<MeshDecoration*>(rp->pd);
	dec->meshindex = enumCombo->currentIndex();
	rp->val->set(MeshValue(md->meshList.at(dec->meshindex)));
}

void MeshWidget::resetWidgetValue()
{
	enumCombo->setCurrentIndex(defaultMeshIndex);
}

void MeshWidget::setWidgetValue( const Value& /*nv*/ )
{
	//WARNING!!!!! I HAVE TO THINK CAREFULLY ABOUT THIS FUNCTION!!!
	assert(0);
	//enumCombo->setCurrentIndex(md->meshList(nv.getMesh());
}

/******************************************
 QVariantListWidget Implementation
******************************************/

/*
QVariantListWidget::QVariantListWidget(QWidget *parent, QList<QVariant> &values)
{
	tableWidget = new QTableWidget(parent);
	tableWidget->setColumnCount(1);
	tableWidget->setRowCount(values.size() );
	tableWidget->setMaximumSize(135, 180);
	tableWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	tableWidget->horizontalHeader()->setDisabled(true);

	setList(values);

	this->addWidget(tableWidget);
}

QList<QVariant> QVariantListWidget::getList()
{
	QList<QVariant> values;

	for(int i = 0; i < tableWidget->rowCount(); i++)
	{
		QString rowValue = tableWidget->item(i,0)->text();

		//if the row was not blank
		if("" != rowValue) values.push_back(QVariant(rowValue));
	}

	return values;
}


void QVariantListWidget::setList(QList<QVariant> &values)
{
	tableWidget->clearContents();
	tableWidget->setRowCount(values.size() );
	for(int i = 0; i < values.size(); i++)
	{
		tableWidget->setItem(i, 0, new QTableWidgetItem(values.at(i).toString()));
	}
}

void QVariantListWidget::addRow()
{
	int count = tableWidget->rowCount();

	//insert a new row at the end
	tableWidget->insertRow(count);

	//fill row with something so that it can be removed if left blank... qt forces us to do this
	tableWidget->setItem(count, 0, new QTableWidgetItem(""));
}


void QVariantListWidget::removeRow()
{
	QList<QTableWidgetItem *> items = tableWidget->selectedItems();

	if(items.size() == 0){
		//remove the last row if none are selected
		tableWidget->removeRow(tableWidget->rowCount()-1);
	} else
	{
		//remove each selected row
		for(int i = 0; i < items.size(); i++){
			tableWidget->removeRow(items.at(i)->row());
		}
	}
}
*/

/******************************************
 GetFileNameButton Implementation
******************************************/

/*
GetFileNameWidget::GetFileNameWidget(QWidget *parent,
		QString &defaultString, bool getOpenFileName, QString fileExtension) : QVBoxLayout(parent)
{
	_fileName = defaultString;
	_fileExtension = fileExtension;

	_getOpenFileName = getOpenFileName;

	launchFileNameDialogButton = new QPushButton(QString("Get FileName"), parent);
	fileNameLabel = new QLabel(_fileName, parent);

	addWidget(launchFileNameDialogButton);
	addWidget(fileNameLabel);

	connect(launchFileNameDialogButton, SIGNAL(clicked()), this, SLOT(launchGetFileNameDialog()));
	
}

GetFileNameWidget::~GetFileNameWidget()
{
	delete launchFileNameDialogButton;
}

QString GetFileNameWidget::getFileName()
{
	return _fileName;
}

void GetFileNameWidget::launchGetFileNameDialog()
{
	QString temp;
	if(_getOpenFileName){
		temp = QFileDialog::getOpenFileName(parentWidget(), tr("Select File"),".", "*"+_fileExtension);
	} else
		temp = QFileDialog::getSaveFileName(parentWidget(), tr("Select File"),".", "*"+_fileExtension);

	if(temp != ""){
		_fileName = temp;
		fileNameLabel->setText(QFileInfo(_fileName).fileName());
	}
}

void GetFileNameWidget::setFileName(QString newName){
	_fileName = newName;
}
*/ 

GenericParamDialog::GenericParamDialog(QWidget *p, RichParameterSet *_curParSet, QString title, MeshDocument *_meshDocument)
 : QDialog(p) {
	stdParFrame=NULL;
	curParSet=_curParSet;
	meshDocument = _meshDocument;
	createFrame();
	if(!title.isEmpty())
		setWindowTitle(title);
}


// update the values of the widgets with the values in the paramlist;
void GenericParamDialog::resetValues()
{
	stdParFrame->resetValues(*curParSet);
}

void GenericParamDialog::toggleHelp()
{
	stdParFrame->toggleHelp();
	this->updateGeometry();
	this->adjustSize();
}


void GenericParamDialog::createFrame()
{
	QVBoxLayout *vboxLayout = new QVBoxLayout(this);
	setLayout(vboxLayout);

	stdParFrame = new StdParFrame(this);
	stdParFrame->loadFrameContent(*curParSet, meshDocument);
  layout()->addWidget(stdParFrame);

	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Help | QDialogButtonBox::Ok  | QDialogButtonBox::Cancel);
	//add the reset button so we can get its signals
	QPushButton *resetButton = buttonBox->addButton(QDialogButtonBox::Reset);
	layout()->addWidget(buttonBox);

	connect(buttonBox, SIGNAL(accepted()), this, SLOT(getAccept()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	connect(buttonBox, SIGNAL(helpRequested()), this, SLOT(toggleHelp()));
	connect(resetButton, SIGNAL(clicked()), this, SLOT(resetValues()));

	setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);

	//set the minimum size so it will shrink down to the right size	after the help is toggled
	this->setMinimumSize(stdParFrame->sizeHint());
	this->showNormal();
	this->adjustSize();
}


void GenericParamDialog::getAccept()
{
	stdParFrame->readValues(*curParSet);
	accept();
}

GenericParamDialog::~GenericParamDialog()
{
	delete stdParFrame;
}

/******************************************/
// DynamicFloatWidget Implementation
/******************************************/


//QGridLayout(NULL)
DynamicFloatWidget::DynamicFloatWidget(QWidget *p, RichDynamicFloat* rdf):MeshLabWidget(p,rdf)
{
	minVal = reinterpret_cast<DynamicFloatDecoration*>(rdf->pd)->min;
	maxVal = reinterpret_cast<DynamicFloatDecoration*>(rdf->pd)->max;
	valueLE = new QLineEdit(p);
	valueSlider = new QSlider(Qt::Horizontal,p);
    valueSlider->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
	fieldDesc = new QLabel(rp->pd->fieldDesc);
	valueSlider->setMinimum(0);
	valueSlider->setMaximum(100);
	valueSlider->setValue(floatToInt(rp->val->getFloat()));
	const DynamicFloatDecoration* dfd = reinterpret_cast<const DynamicFloatDecoration*>(&(rp->pd));
	valueLE->setValidator(new QDoubleValidator (dfd->min,dfd->max, 5, valueLE));
	valueLE->setText(QString::number(rp->val->getFloat()));

	
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
	
}

DynamicFloatWidget::~DynamicFloatWidget()
{
}

float DynamicFloatWidget::getValue()
{
	return float(valueLE->text().toDouble());

}

void DynamicFloatWidget::setValue(float  newVal)
{
	if( QString::number(intToFloat(newVal)) != valueLE->text())
		 valueLE->setText(QString::number(intToFloat(newVal)));
}

void DynamicFloatWidget::setValue(int  newVal)
{
	if(floatToInt(float(valueLE->text().toDouble())) != newVal)
	{
		valueLE->setText(QString::number(intToFloat(newVal)));
	}
}

void DynamicFloatWidget::setValue()
{
	float newValLE=float(valueLE->text().toDouble());
	valueSlider->setValue(floatToInt(newValLE));
	emit dialogParamChanged();
}

float DynamicFloatWidget::intToFloat(int val)
{
	return minVal+float(val)/100.0f*(maxVal-minVal);
}
int DynamicFloatWidget::floatToInt(float val)
{
	return int (100.0f*(val-minVal)/(maxVal-minVal));
}

void DynamicFloatWidget::collectWidgetValue()
{
	rp->val->set(DynamicFloatValue(valueLE->text().toFloat()));
}

void DynamicFloatWidget::resetWidgetValue()
{
	valueLE->setText(QString::number(rp->pd->defVal->getFloat()));
}

void DynamicFloatWidget::setWidgetValue( const Value& nv )
{
	valueLE->setText(QString::number(nv.getFloat()));
}
/****************************/
Value& MeshLabWidget::getWidgetValue()
{
	collectWidgetValue();
	return *(rp->val);
}


void MeshLabWidget::resetValue()
{
	rp->val->set(*rp->pd->defVal);
	resetWidgetValue();
}

MeshLabWidget::MeshLabWidget( QWidget* p,RichParameter* rpar )
:QWidget(p),rp(rpar)
{
	if (rp!= NULL)
	{
		helpLab = new QLabel("<small>"+rpar->pd->tooltip +"</small>",p);
		helpLab->setTextFormat(Qt::RichText);
		helpLab->setWordWrap(true);
		helpLab->setVisible(false);
		helpLab->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
		helpLab->setMinimumWidth(250);
		helpLab->setMaximumWidth(QWIDGETSIZE_MAX);
		gridLay = qobject_cast<QGridLayout*>(p->layout());
		assert(gridLay != 0);

		row = gridLay->rowCount();
		//WARNING!!!!!!!!!!!!!!!!!! HORRIBLE PATCH FOR THE BOOL WIDGET PROBLEM
		if ((row == 1) && (rpar->val->isBool()))	
		{
			
			QLabel* lb = new QLabel("",p);
			gridLay->addWidget(lb);
			gridLay->addWidget(helpLab,row+1,3,1,1,Qt::AlignTop);
		}
		///////////////////////////////////////////////////////////////////////
		else
			gridLay->addWidget(helpLab,row,3,1,1,Qt::AlignTop);
	}
}

void MeshLabWidget::InitRichParameter(RichParameter* rpar)
{
	rp = rpar;
}

MeshLabWidget::~MeshLabWidget()
{
	//delete rp;
	delete helpLab;
}

//connect(qcb,SIGNAL(stateChanged(int)),this,SIGNAL(parameterChanged()));
BoolWidget::BoolWidget( QWidget* p,RichBool* rb )
:MeshLabWidget(p,rb)
{

	cb = new QCheckBox(rp->pd->fieldDesc,p);
	cb->setToolTip(rp->pd->tooltip);
	cb->setChecked(rp->val->getBool());


	//gridlay->addWidget(this,i,0,1,1,Qt::AlignTop);

	//int row = gridLay->rowCount() -1 ;
	//WARNING!!!!!!!!!!!!!!!!!! HORRIBLE PATCH FOR THE BOOL WIDGET PROBLEM
	if (row == 1)
		gridLay->addWidget(cb,row + 1,0,1,2,Qt::AlignTop);
	///////////////////////////////////////////////////////////////////////
	else
		gridLay->addWidget(cb,row,0,1,2,Qt::AlignTop);

	connect(cb,SIGNAL(stateChanged(int)),p,SIGNAL(parameterChanged()));

}

void BoolWidget::collectWidgetValue()
{
	rp->val->set(BoolValue(cb->isChecked()));
}

void BoolWidget::resetWidgetValue()
{
	cb->setChecked(rp->pd->defVal->getBool());
}

BoolWidget::~BoolWidget()
{
	//delete lab;
	delete cb;
}

void BoolWidget::setWidgetValue( const Value& nv )
{
	cb->setChecked(nv.getBool());
}

//connect(qle,SIGNAL(editingFinished()),this,SIGNAL(parameterChanged()));
LineEditWidget::LineEditWidget( QWidget* p,RichParameter* rpar )
:MeshLabWidget(p,rpar)
{
	lab = new QLabel(rp->pd->fieldDesc,this);
	lned = new QLineEdit(this);
	//int row = gridLay->rowCount() -1;
	
	lab->setToolTip(rp->pd->tooltip);
	gridLay->addWidget(lab,row,0,Qt::AlignTop);
	gridLay->addWidget(lned,row,1,Qt::AlignTop);
	connect(lned,SIGNAL(editingFinished()),p,SIGNAL(parameterChanged()));
}

LineEditWidget::~LineEditWidget()
{
	delete lned;
	delete lab;
}
IntWidget::IntWidget( QWidget* p,RichInt* rpar )
:LineEditWidget(p,rpar)
{
	lned->setText(QString::number(rp->val->getInt()));
}

void IntWidget::collectWidgetValue()
{
	rp->val->set(IntValue(lned->text().toInt()));
}

void IntWidget::resetWidgetValue()
{
	lned->setText(QString::number(rp->pd->defVal->getInt()));
}

void IntWidget::setWidgetValue( const Value& nv )
{
	lned->setText(QString::number(nv.getInt()));
}
//
FloatWidget::FloatWidget( QWidget* p,RichFloat* rpar )
:LineEditWidget(p,rpar)
{
	lned->setText(QString::number(rp->val->getFloat(),'g',3));
}

void FloatWidget::collectWidgetValue()
{
	rp->val->set(FloatValue(lned->text().toFloat()));
}

void FloatWidget::resetWidgetValue()
{
	lned->setText(QString::number(rp->pd->defVal->getFloat(),'g',3));
}

void FloatWidget::setWidgetValue( const Value& nv )
{
	lned->setText(QString::number(nv.getFloat(),'g',3));
}

StringWidget::StringWidget( QWidget* p,RichString* rpar )
:LineEditWidget(p,rpar)
{
	lned->setText(rp->val->getString());
}

void StringWidget::collectWidgetValue()
{
	rp->val->set(StringValue(lned->text()));
}

void StringWidget::resetWidgetValue()
{
	lned->setText(rp->pd->defVal->getString());
}

void StringWidget::setWidgetValue( const Value& nv )
{
	lned->setText(nv.getString());
}

//Matrix44fWidget::Matrix44fWidget( QWidget* p,RichMatrix44f* rpar )
//:MeshLabWidget(p,rb)
//{
//}
//
//void Matrix44fWidget::collectWidgetValue()
//{
//}
//
//void Matrix44fWidget::resetWidgetValue()
//{
//}


/*
FloatListWidget::FloatListWidget( QWidget* p,RichMesh* rpar )
:MeshLabWidget(p,rb)
{
}


void FloatListWidget::collectWidgetValue()
{
}

void FloatListWidget::resetWidgetValue()
{
}

OpenFileWidget::OpenFileWidget( QWidget* p,RichMesh* rpar )
:MeshLabWidget(p,rb)
{
}

void OpenFileWidget::collectWidgetValue()
{
}

void OpenFileWidget::resetWidgetValue()
{
}
*/

/*
ql = new QLabel(fpi.fieldDesc,this);
ql->setToolTip(fpi.fieldToolTip);

qcbt = new QColorButton(this,QColor(fpi.fieldVal.toUInt()));
gridLayout->addWidget(ql,i,0,Qt::AlignTop);
gridLayout->addLayout(qcbt,i,1,Qt::AlignTop);

stdfieldwidgets.push_back(qcbt);
connect(qcbt,SIGNAL(dialogParamChanged()),this,SIGNAL(parameterChanged()));
*/

/******************************************/
// ColorWidget Implementation
/******************************************/


ColorWidget::ColorWidget(QWidget *p, RichColor* newColor)
:MeshLabWidget(p,newColor),pickcol()
{
	
	colorLabel = new QLabel(p);
	descLabel = new QLabel(rp->pd->fieldDesc,p);
	colorButton = new QPushButton(p);
	colorButton->setAutoFillBackground(true);
	colorButton->setFlat(true);
	//const QColor cl = rp->pd->defVal->getColor();
	//resetWidgetValue();
	initWidgetValue();
	//int row = gridLay->rowCount() - 1;
	gridLay->addWidget(descLabel,row,0,Qt::AlignTop);

	QHBoxLayout* lay = new QHBoxLayout(p);
	lay->addWidget(colorLabel);
	lay->addWidget(colorButton);

	gridLay->addLayout(lay,row,1,Qt::AlignTop);
	pickcol = rp->val->getColor();
	connect(colorButton,SIGNAL(clicked()),this,SLOT(pickColor()));
	connect(this,SIGNAL(dialogParamChanged()),p,SIGNAL(parameterChanged()));
}

void ColorWidget::updateColorInfo(const ColorValue& newColor)
{
	QColor col = newColor.getColor();
	colorLabel->setText("("+col.name()+")");
	QPalette palette(col);
	colorButton->setPalette(palette);
}

void ColorWidget::pickColor()
{
	pickcol =QColorDialog::getColor(pickcol);
	if(pickcol.isValid()) 
	{
		collectWidgetValue();
		updateColorInfo(ColorValue(pickcol));
	}
	emit dialogParamChanged();
}

void ColorWidget::initWidgetValue()
{
	QColor cl = rp->val->getColor();
	pickcol = cl;
	updateColorInfo(cl);
}

void ColorWidget::resetWidgetValue()
{
	QColor cl = rp->pd->defVal->getColor();
	pickcol = cl;
	updateColorInfo(cl);
}

void ColorWidget::collectWidgetValue()
{
	rp->val->set(ColorValue(pickcol));
}

ColorWidget::~ColorWidget()
{
	delete colorButton;
	delete colorLabel;
	delete descLabel;
}

void ColorWidget::setWidgetValue( const Value& nv )
{
	QColor cl = nv.getColor();
	pickcol = cl;
	updateColorInfo(cl);
}
/*
void GetFileNameWidget::launchGetFileNameDialog()
{

}*/

void RichParameterToQTableWidgetItemConstructor::visit( RichBool& pd )
{
	if (pd.val->getBool())
		lastCreated = new QTableWidgetItem("true"/*,lst*/);
	else
		lastCreated = new QTableWidgetItem("false"/*,lst*/);

}

void RichParameterToQTableWidgetItemConstructor::visit( RichInt& pd )
{
	lastCreated = new QTableWidgetItem(QString::number(pd.val->getInt())/*,lst*/);
}

void RichParameterToQTableWidgetItemConstructor::visit( RichFloat& pd )
{
	lastCreated = new QTableWidgetItem(QString::number(pd.val->getFloat())/*,lst*/);
}

void RichParameterToQTableWidgetItemConstructor::visit( RichString& pd )
{
	lastCreated = new QTableWidgetItem(pd.val->getString()/*,lst*/);
}

void RichParameterToQTableWidgetItemConstructor::visit( RichPoint3f& pd )
{
	vcg::Point3f pp = pd.val->getPoint3f(); 
	QString pst = "P3(" + QString::number(pp.X()) + "," + QString::number(pp.Y()) + "," + QString::number(pp.Z()) + ")"; 
	lastCreated = new QTableWidgetItem(pst/*,lst*/);
}

void RichParameterToQTableWidgetItemConstructor::visit(RichOpenFile& pd)
{
	lastCreated = new QTableWidgetItem(pd.val->getFileName()/*,lst*/);
}

void RichParameterToQTableWidgetItemConstructor::visit( RichColor& pd )
{
	QPixmap pix(10,10);
	pix.fill(pd.val->getColor());
	QIcon ic(pix);
	lastCreated = new QTableWidgetItem(ic,""/*,lst*/);
}

void RichParameterToQTableWidgetItemConstructor::visit( RichAbsPerc& pd )
{
	lastCreated = new QTableWidgetItem(QString::number(pd.val->getAbsPerc())/*,lst*/);
}

void RichParameterToQTableWidgetItemConstructor::visit( RichEnum& pd )
{
	lastCreated = new QTableWidgetItem(QString::number(pd.val->getEnum())/*,lst*/);
}

void RichParameterToQTableWidgetItemConstructor::visit( RichDynamicFloat& pd )
{
	lastCreated = new QTableWidgetItem(QString::number(pd.val->getDynamicFloat())/*,lst*/);
}

IOFileWidget::IOFileWidget( QWidget* p,RichParameter* rpar )
:MeshLabWidget(p,rpar),fl()
{
	filename = new QLineEdit(p);
	filename->setText(tr(""));
	browse = new QPushButton(p);
	descLab = new QLabel(rp->pd->fieldDesc,p);
	browse->setText("...");
	//const QColor cl = rp->pd->defVal->getColor();
	//resetWidgetValue();
	//int row = gridLay->rowCount() - 1;
	gridLay->addWidget(descLab,row,0,Qt::AlignTop);
	QHBoxLayout* lay = new QHBoxLayout(p);
	lay->addWidget(filename,2);
	lay->addWidget(browse);

	gridLay->addLayout(lay,row,1,Qt::AlignTop);

	connect(browse,SIGNAL(clicked()),this,SLOT(selectFile()));
	connect(this,SIGNAL(dialogParamChanged()),p,SIGNAL(parameterChanged()));
}

IOFileWidget::~IOFileWidget()
{
	delete filename;
	delete browse;
	delete descLab;
}

void IOFileWidget::collectWidgetValue()
{
	rp->val->set(FileValue(fl));
}

void IOFileWidget::resetWidgetValue()
{
	QString fle = rp->pd->defVal->getFileName();
	fl = fle;
	updateFileName(fle);
}


void IOFileWidget::setWidgetValue(const Value& nv)
{
	QString fle = nv.getFileName();
	fl = fle;
	updateFileName(QString());
}

void IOFileWidget::updateFileName( const FileValue& file )
{
	filename->setText(file.getFileName());
}

OpenFileWidget::OpenFileWidget( QWidget *p, RichOpenFile* rdf )
:IOFileWidget(p,rdf)
{
}

void OpenFileWidget::selectFile()
{
	OpenFileDecoration* dec = reinterpret_cast<OpenFileDecoration*>(rp->pd);
	QString ext;
	fl = QFileDialog::getOpenFileName(this,tr("Open"),dec->defVal->getFileName(),dec->exts.join(" "));
	collectWidgetValue();
	updateFileName(fl);
	emit dialogParamChanged();
}

//void OpenFileWidget::collectWidgetValue()
//{
//	rp->val->set(FileValue(fl));
//}
//
//void OpenFileWidget::resetWidgetValue()
//{
//	QString fle = rp->pd->defVal->getFileName();
//	fl = fle;
//	updateFileName(fle);
//}
//
//
//void OpenFileWidget::setWidgetValue(const Value& nv)
//{
//	QString fle = nv.getFileName();
//	fl = fle;
//	updateFileName(QString());
//}

OpenFileWidget::~OpenFileWidget()
{
}


SaveFileWidget::SaveFileWidget( QWidget* p,RichSaveFile* rpar )
:IOFileWidget(p,rpar)
{
	filename->setText(rpar->val->getFileName());
}

SaveFileWidget::~SaveFileWidget()
{
}

void SaveFileWidget::selectFile()
{
	SaveFileDecoration* dec = reinterpret_cast<SaveFileDecoration*>(rp->pd);
	QString ext;
	fl = QFileDialog::getSaveFileName(this,tr("Save"),dec->defVal->getFileName(),dec->ext);
	collectWidgetValue();
	updateFileName(fl);
	emit dialogParamChanged();
}


