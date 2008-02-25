/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2006                                          \/)\/    *
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
/****************************************************************************
History

$Log$
Revision 1.25  2008/02/25 14:51:01  ponchio
added parent window reference durinc creation

Revision 1.24  2008/01/28 13:02:00  cignoni
added support for filters on collection of meshes (layer filters)

Revision 1.23  2008/01/10 17:16:44  cignoni
unsaved dialog has a better behaviour

Revision 1.22  2007/12/11 23:56:40  cignoni
better resizing of dialogs

Revision 1.21  2007/11/30 07:19:09  cignoni
moved generic dialog to the meshlab base

Revision 1.20  2007/11/20 18:55:32  ponchio
removed qDebug

Revision 1.18  2007/11/19 17:09:20  ponchio
added enum value. [untested].

Revision 1.17  2007/11/19 15:51:50  cignoni
Added frame abstraction for reusing the std dialog mechanism

Revision 1.16  2007/11/09 11:27:27  cignoni
corrected resizing strategy and hints (still not perfect)

Revision 1.15  2007/11/05 13:34:41  cignoni
added color and Help

Revision 1.14  2007/10/17 21:23:30  cignoni
added wordwrapping to the first line of the automatic dialog

Revision 1.13  2007/10/16 11:04:06  cignoni
better init of the frame

Revision 1.12  2007/10/02 10:03:03  cignoni
wrong init in a case statement

Revision 1.11  2007/10/02 07:59:35  cignoni
New filter interface. Hopefully more clean and easy to use.

Revision 1.10  2007/04/16 09:22:43  cignoni
corrected resizing bug in the dialog

Revision 1.9  2007/02/27 23:58:36  cignoni
Changed apply/ok into apply/close

Revision 1.8  2007/02/10 16:41:18  pirosu
replaced NULL with empty FilterParameter for filters that don't use the standard plugin window

Revision 1.7  2007/02/09 09:09:40  pirosu
Added ToolTip support for standard parameters

Revision 1.6  2007/01/19 09:12:59  cignoni
Better formatting of floating points numbers in the dialog

Revision 1.5  2007/01/13 02:02:28  cignoni
Changed loadFrameContent to pass the QString not as a reference (to avoid a temp var referencing)

Revision 1.4  2007/01/11 19:51:46  pirosu
fixed bug for QT 4.1.0/dotnet2003
removed the request of the window title to the plugin. The action description is used instead.

Revision 1.3  2006/12/27 21:41:41  pirosu
Added improvements for the standard plugin window:
split of the apply button in two buttons:ok and apply
added support for parameters with absolute and percentage values

Revision 1.2  2006/12/13 21:54:35  pirosu
2 updates for the standard plugin window: 1) it recovers its last size when it is undocked and 2) it closes itself when a filter is applied (only if it is floating)

Revision 1.0  2006/12/13 17:37:02  pirosu
Added standard plugin window support

****************************************************************************/
#include<QObject>
#include "meshmodel.h"
#include "interfaces.h"
#include "mainwindow.h"
#include "plugindialog.h"
#include "stdpardialog.h"

MeshlabStdDialog::MeshlabStdDialog(QWidget *p)
:QDockWidget(QString("Plugin"), p)
{
		qf = NULL;
		stdParFrame=NULL;
		clearValues();
}

StdParFrame::StdParFrame(QWidget *p)
//:QDialog(p)
:QFrame(p)
{
}


/* manages the setup of the standard parameter window, when the execution of a plugin filter is requested */
void MeshlabStdDialog::showAutoDialog(MeshFilterInterface *mfi, MeshModel *mm, MeshDocument * md, QAction *action, MainWindowInterface *mwi)
  {
		curAction=action;
		curmfi=mfi;
		curmwi=mwi;
		curParSet.clear();
		curModel = mm;
		curMeshDoc = md;
		
		
		mfi->initParameterSet(action, *md, curParSet);	
		createFrame();
		loadFrameContent();
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

void StdParFrame::resetValues(FilterParameterSet &curParSet)
{
	QList<FilterParameter> &parList =curParSet.paramList;
	for(int i = 0; i < parList.count(); i++)
		{
			const FilterParameter &fpi=parList.at(i);
			switch(fpi.fieldType)
			{ 
				case FilterParameter::PARBOOL:
					if(fpi.fieldVal.toBool()) ((QCheckBox *)stdfieldwidgets.at(i))->setCheckState(Qt::Checked);
					break;
				case FilterParameter::PARFLOAT:
					((QLineEdit *)stdfieldwidgets.at(i))->setText(QString::number(fpi.fieldVal.toDouble(),'g',3));
					break;
				case FilterParameter::PARINT:
				case FilterParameter::PARSTRING:
					((QLineEdit *)stdfieldwidgets.at(i))->setText(fpi.fieldVal.toString());
					break;
				case FilterParameter::PARABSPERC:
					((AbsPercWidget *)stdfieldwidgets.at(i))->setValue(fpi.fieldVal.toDouble(),fpi.min,fpi.max);
					break;
				case FilterParameter::PARCOLOR:
					((QColorButton *)stdfieldwidgets.at(i))->setColor(QColor(fpi.fieldVal.toUInt()));
					break;
				case FilterParameter::PARENUM:
					((EnumWidget *)stdfieldwidgets.at(i))->setEnum(fpi.fieldVal.toUInt());
					break;
				default: assert(0);

			}
	}
			
}

	/* creates widgets for the standard parameters */

void MeshlabStdDialog::loadFrameContent()
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
	
	stdParFrame = new StdParFrame(this);
	stdParFrame->loadFrameContent(curParSet);
  gridLayout->addWidget(stdParFrame,1,0,1,2);

	int buttonRow = 2;  // the row where the line of buttons start 
	
	QPushButton *helpButton = new QPushButton("Help", qf);
	QPushButton *closeButton = new QPushButton("Close", qf);
	QPushButton *applyButton = new QPushButton("Apply", qf);
	QPushButton *defaultButton = new QPushButton("Default", qf);
	
	gridLayout->addWidget(helpButton,buttonRow,1,Qt::AlignBottom);
	gridLayout->addWidget(defaultButton,buttonRow,0,Qt::AlignBottom);
	gridLayout->addWidget(closeButton,buttonRow+1,0,Qt::AlignBottom);
	gridLayout->addWidget(applyButton,buttonRow+1,1,Qt::AlignBottom);
	
	connect(helpButton,SIGNAL(clicked()),this,SLOT(toggleHelp()));
	connect(applyButton,SIGNAL(clicked()),this,SLOT(applyClick()));
	connect(closeButton,SIGNAL(clicked()),this,SLOT(closeClick()));
	connect(defaultButton,SIGNAL(clicked()),this,SLOT(resetValues()));
	
	qf->showNormal();	
	qf->adjustSize();
	
	this->showNormal();
	this->adjustSize();		
}

void StdParFrame::loadFrameContent(FilterParameterSet &curParSet)
{
 if(layout()) delete layout();
	QGridLayout *gridLayout = new QGridLayout(this);
	setLayout(gridLayout);
//  gridLayout->setColumnStretch(0,1);
//	gridLayout->setColumnStretch(1,1);
//	gridLayout->setColumnStretch(2,2);

	QCheckBox *qcb;
	QLineEdit *qle;
	QLabel *ql;
	AbsPercWidget *apw;
	QColorButton *qcbt;
	EnumWidget *ew;
	QList<FilterParameter> &parList =curParSet.paramList;
	
	QString descr;
	
	for(int i = 0; i < parList.count(); i++)
	{
		const FilterParameter &fpi=parList.at(i);
		ql = new QLabel("<small>"+fpi.fieldToolTip +"</small>",this);
		ql->setTextFormat(Qt::RichText);
		ql->setWordWrap(true);
		ql->setVisible(false);
		ql->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
		ql->setMinimumWidth(250);
		ql->setMaximumWidth(QWIDGETSIZE_MAX);
		gridLayout->addWidget(ql,i,3,1,1,Qt::AlignTop); 
		helpList.push_back(ql);
		
		switch(fpi.fieldType)
	  { 
			case FilterParameter::PARBOOL:
				qcb = new QCheckBox(fpi.fieldDesc,this);
				qcb->setToolTip(fpi.fieldToolTip);		  
				if(fpi.fieldVal.toBool()) qcb->setCheckState(Qt::Checked);
				gridLayout->addWidget(qcb,i,0,1,2,Qt::AlignTop);
				stdfieldwidgets.push_back(qcb);
				break;
				
			case FilterParameter::PARFLOAT:
				ql = new QLabel(fpi.fieldDesc,this);
				ql->setToolTip(fpi.fieldToolTip);			  
				qle = new QLineEdit(QString::number(fpi.fieldVal.toDouble(),'g',3),this); // better formatting of floating point numbers		  
				gridLayout->addWidget(ql,i,0,Qt::AlignTop);
				gridLayout->addWidget(qle,i,1,Qt::AlignTop);
				stdfieldwidgets.push_back(qle);
				break;
			case FilterParameter::PARINT:
			case FilterParameter::PARSTRING:
				ql = new QLabel(fpi.fieldDesc,this);
				ql->setToolTip(fpi.fieldToolTip);		
						
				qle = new QLineEdit(fpi.fieldVal.toString(),this);
				
				gridLayout->addWidget(ql,i,0,Qt::AlignTop);
				gridLayout->addWidget(qle,i,1,Qt::AlignTop);

				stdfieldwidgets.push_back(qle);

				break;
			case FilterParameter::PARABSPERC:
				descr = fpi.fieldDesc + " (abs and %)";
				ql = new QLabel(descr ,this);
				ql->setToolTip(fpi.fieldToolTip);	
				
				apw = new AbsPercWidget(this,float(fpi.fieldVal.toDouble()),fpi.min,fpi.max);
				gridLayout->addWidget(ql,i,0,Qt::AlignTop);
				gridLayout->addLayout(apw,i,1,Qt::AlignTop);
					
				stdfieldwidgets.push_back(apw);
		
				break;
			case FilterParameter::PARCOLOR:
				ql = new QLabel(fpi.fieldDesc,this);
				ql->setToolTip(fpi.fieldToolTip);	
				
				qcbt = new QColorButton(this,QColor(fpi.fieldVal.toUInt()));
				gridLayout->addWidget(ql,i,0,Qt::AlignTop);
				gridLayout->addLayout(qcbt,i,1,Qt::AlignTop);
					
				stdfieldwidgets.push_back(qcbt);
		
				break;
			case FilterParameter::PARENUM:
				ql = new QLabel(fpi.fieldDesc,this);
				ql->setToolTip(fpi.fieldToolTip);	
				
				ew = new EnumWidget(this, fpi.fieldVal.toUInt(), fpi.enumValues);
				gridLayout->addWidget(ql,i,0,Qt::AlignTop);
				gridLayout->addLayout(ew,i,1,Qt::AlignTop);
					
				stdfieldwidgets.push_back(ew);
		
				break;


			default: assert(0);
		} //end case
	} // end for each parameter
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
	this->updateGeometry();
	this->adjustSize();
}

void StdParFrame::readValues(FilterParameterSet &curParSet)
{
		QList<FilterParameter> &parList =curParSet.paramList;
		
	  for(int i = 0; i < parList.count(); i++)
	  {
		  QString sname = parList.at(i).fieldName;
		  switch(parList.at(i).fieldType)
		  {
				case FilterParameter::PARBOOL:
				curParSet.setBool(sname,((QCheckBox *)stdfieldwidgets[i])->checkState() == Qt::Checked);
			  break;
		  case FilterParameter::PARINT:
			  curParSet.setInt(sname,((QLineEdit *)stdfieldwidgets[i])->text().toInt());
			  break;
		  case FilterParameter::PARFLOAT:
			  curParSet.setFloat(sname,((QLineEdit *)stdfieldwidgets[i])->text().toFloat());
			  break;
		  case FilterParameter::PARABSPERC:
			  curParSet.setAbsPerc(sname,((AbsPercWidget *)stdfieldwidgets[i])->getValue());
			  break;
		  case FilterParameter::PARSTRING:
			  curParSet.setString(sname,((QLineEdit *)stdfieldwidgets[i])->text());
			  break;
		  case FilterParameter::PARCOLOR:
			  curParSet.setColor(sname,((QColorButton *)stdfieldwidgets[i])->getColor());
			  break;
		  case FilterParameter::PARENUM:
			  curParSet.setEnum(sname,((EnumWidget *)stdfieldwidgets[i])->getEnum());
			  break;

		  }
	  }
}


/* click event for the apply button of the standard plugin window */
void MeshlabStdDialog::applyClick()
{
	QAction *q = curAction;
	stdParFrame->readValues(curParSet);
	curmwi->executeFilter(q,curParSet);		
}

/* click event for the close button of the standard plugin window */

void MeshlabStdDialog::closeClick()
{
	this->hide();
}

/******************************************/ 
// AbsPercWidget Implementation
/******************************************/ 
  AbsPercWidget::AbsPercWidget(QWidget *p, double defaultv, double minVal, double maxVal):QGridLayout(NULL)
  {
	  m_min = minVal;
	  m_max = maxVal;
	  absSB = new QDoubleSpinBox(p);
	  percSB = new QDoubleSpinBox(p);

	  //absSB->setMinimum(m_min);
	  absSB->setMaximum(m_max*2);
	  absSB->setDecimals(3);
	  absSB->setSingleStep((m_max-m_min)/20.0);
	  absSB->setValue(defaultv);

	  percSB->setMinimum(0);
	  percSB->setMaximum(200);
		percSB->setSingleStep(0.5);
	  percSB->setValue((100*(defaultv - m_min))/(m_max - m_min));
		QLabel *absLab=new QLabel("<i> <small> world unit</small></i>");
		QLabel *percLab=new QLabel("<i> <small> perc on"+QString("(%1 .. %2)").arg(m_min).arg(m_max)+"</small></i>");
		
		this->addWidget(absLab,0,0,Qt::AlignHCenter);
		this->addWidget(percLab,0,1,Qt::AlignHCenter);
		
	  this->addWidget(absSB,1,0);
	  this->addWidget(percSB,1,1,Qt::AlignTop);


		connect(absSB,SIGNAL(valueChanged(double)),this,SLOT(on_absSB_valueChanged(double)));
		connect(percSB,SIGNAL(valueChanged(double)),this,SLOT(on_percSB_valueChanged(double)));
  }

  AbsPercWidget::~AbsPercWidget()
  {
	  delete absSB;
	  delete percSB;
  }


void AbsPercWidget::on_absSB_valueChanged(double newv) 
{
	percSB->setValue((100*(newv - m_min))/(m_max - m_min));
}

void AbsPercWidget::on_percSB_valueChanged(double newv)
{
	absSB->setValue((m_max - m_min)*0.01*newv + m_min);
}

float AbsPercWidget::getValue()
{
	return float(absSB->value());
}

void AbsPercWidget::setValue(float val, float minV, float maxV)
{
	assert(absSB);
	absSB->setValue (val);
	m_min=minV;
	m_max=maxV;
}

/******************************************/ 
// QColorButton Implementation
/******************************************/ 
QColorButton::QColorButton(QWidget *p, QColor newColor):QHBoxLayout()
{
		colorLabel = new QLabel(p);
		colorButton = new QPushButton(p);
		colorButton->setAutoFillBackground(true);
		colorButton->setFlat(true);
		setColor(newColor);
		this->addWidget(colorLabel);
		this->addWidget(colorButton);		
		connect(colorButton,SIGNAL(clicked()),this,SLOT(pickColor()));		
}

QColor QColorButton::getColor()
{
	return currentColor;
}

void  QColorButton::setColor(QColor newColor)
{
	currentColor=newColor;
	colorLabel->setText("("+currentColor.name()+")");
	QPalette palette(currentColor);
	colorButton->setPalette(palette);		 
}

void QColorButton::pickColor()
{
	 QColor newColor=QColorDialog::getColor(QColor(255,255,255,255));
	 setColor(newColor);
}

/******************************************/ 
//EnumWidget Implementation
/******************************************/ 
EnumWidget::EnumWidget(QWidget *p, int defaultEnum, QStringList values) {
  enumLabel = new QLabel(p);
	enumCombo = new QComboBox(p);
  enumCombo->addItems(values);
	setEnum(defaultEnum);
	this->addWidget(enumLabel);
	this->addWidget(enumCombo);
}

int EnumWidget::getEnum()
{
	return enumCombo->currentIndex();
}

void EnumWidget::setEnum(int newEnum) 
{
	enumCombo->setCurrentIndex(newEnum);
}




GenericParamDialog::GenericParamDialog(QWidget *p, FilterParameterSet *_curParSet) :QDialog(p)
{
		stdParFrame=NULL;
		curParSet=_curParSet;
		createFrame();
}

// update the values of the widgets with the values in the paramlist;
void GenericParamDialog::resetValues()
{
		stdParFrame->resetValues(*defaultParSet);
}

void GenericParamDialog::toggleHelp()
{	
	stdParFrame->toggleHelp();
	this->updateGeometry();
	this->adjustSize();
}


void GenericParamDialog::createFrame()
{
	setWindowTitle("Alignment Parameters");

	QVBoxLayout *vboxLayout = new QVBoxLayout(this);
	setLayout(vboxLayout);
	
	stdParFrame = new StdParFrame(this);
	stdParFrame->loadFrameContent(*curParSet);
  layout()->addWidget(stdParFrame);

	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Help  |QDialogButtonBox::Reset  | QDialogButtonBox::Ok  | QDialogButtonBox::Cancel);
	layout()->addWidget(buttonBox);
	
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(getAccept()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	connect(buttonBox, SIGNAL(helpRequested()), this, SLOT(toggleHelp()));
	
	setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
		
	this->showNormal();
	this->adjustSize();		
}


void GenericParamDialog::getAccept()
{
	stdParFrame->readValues(*curParSet);
	accept();
}
