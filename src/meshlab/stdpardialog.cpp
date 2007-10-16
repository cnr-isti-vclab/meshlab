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

#include "stdpardialog.h"

MeshlabStdDialog::MeshlabStdDialog(QWidget *p)
	:QDockWidget(QString("Plugin"),p)
  {
		qf = NULL;
		clearValues();
}

/* manages the setup of the standard parameter window, when the execution of a plugin filter is requested */
void MeshlabStdDialog::showAutoDialog(MeshFilterInterface *mfi, MeshModel *mm, QAction *action, MainWindowInterface *mwi)
  {
		curAction=action;
		curmfi=mfi;
		curmwi=mwi;
		curParSet.clear();
		curModel = mm;
		mfi->initParameterSet(action, *mm, curParSet);	
		createFrame();
		loadFrameContent();
  }


	void MeshlabStdDialog::clearValues()
	{
		curAction = NULL;
		stdfieldwidgets.clear();
		curModel = NULL;
		curmfi = NULL;
		curmwi = NULL;
	}

void MeshlabStdDialog::createFrame()
{
	if(qf)   {
				delete qf;
				stdfieldwidgets.clear();
				}
  QFrame *newqf = new MeshlabStdDialogFrame(this);
  newqf->setFrameStyle(QFrame::Box | QFrame::Sunken);
	newqf->setMinimumSize(75, 75);
	setWidget(newqf);
  qf = newqf;
}

// update the values of the widgets with the values in the paramlist;
void MeshlabStdDialog::resetValues()
{
	curParSet.clear();
	curmfi->initParameterSet(curAction, *curModel, curParSet);	
		
	assert(qf);
	assert(qf->isVisible());
  assert(curParSet.paramList.count() == stdfieldwidgets.count());
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
			}
	}
			
}

void MeshlabStdDialog::loadFrameContent()
{
	assert(qf);
	qf->hide();	
	setWindowTitle(curmfi->filterName(curAction));
	//stdfieldwidgets.clear();
//	qf->clear();
	QGridLayout *gridLayout = new QGridLayout(qf);
	qf->setLayout(gridLayout);

	QCheckBox *qcb;
	QLineEdit *qle;
	QLabel *ql;
	AbsPercWidget *apw;
	QList<FilterParameter> &parList =curParSet.paramList;
	/* creates widgets for the standard parameters */
	
	ql = new QLabel("<i>"+curmfi->filterInfo(curAction)+"</i>",qf);
	ql->setTextFormat(Qt::RichText);
	gridLayout->addWidget(ql,0,0,1,2,Qt::AlignTop);
	
	for(int i = 0; i < parList.count(); i++)
	{
		const FilterParameter &fpi=parList.at(i);
		switch(parList.at(i).fieldType)
	  { 
	  case FilterParameter::PARBOOL:
		  qcb = new QCheckBox(fpi.fieldDesc,qf);
		  qcb->setToolTip(fpi.fieldToolTip);		  
		  if(fpi.fieldVal.toBool()) qcb->setCheckState(Qt::Checked);
		  gridLayout->addWidget(qcb,i+1,0,1,2,Qt::AlignTop);
		  stdfieldwidgets.push_back(qcb);
		  break;
			
	  case FilterParameter::PARFLOAT:
		  ql = new QLabel(fpi.fieldDesc,qf);
		  ql->setToolTip(fpi.fieldToolTip);			  
		  qle = new QLineEdit(QString::number(fpi.fieldVal.toDouble(),'g',3),qf); // better formatting of floating point numbers		  
		  gridLayout->addWidget(ql,i+1,0,Qt::AlignTop);
		  gridLayout->addWidget(qle,i+1,1,Qt::AlignTop);
		  stdfieldwidgets.push_back(qle);
		  break;
	  case FilterParameter::PARINT:
	  case FilterParameter::PARSTRING:
		  ql = new QLabel(parList.at(i).fieldDesc,qf);
			ql->setToolTip(fpi.fieldToolTip);		
				  
		  qle = new QLineEdit(fpi.fieldVal.toString(),qf);
		  
		  gridLayout->addWidget(ql,i+1,0,Qt::AlignTop);
		  gridLayout->addWidget(qle,i+1,1,Qt::AlignTop);

		  stdfieldwidgets.push_back(qle);

		  break;
	  case FilterParameter::PARABSPERC:
		  QString desc = parList.at(i).fieldDesc + " (abs and %)";
		  ql = new QLabel(desc ,qf);
		  ql->setToolTip(fpi.fieldToolTip);	
		  
		  apw = new AbsPercWidget(qf,float(fpi.fieldVal.toDouble()),fpi.min,fpi.max);
		  gridLayout->addWidget(ql,i+1,0,Qt::AlignTop);
		  gridLayout->addLayout(apw,i+1,1,Qt::AlignTop);
	  		
		  stdfieldwidgets.push_back(apw);

		  break;
	  }

	}

	int nbut = parList.count()+1;

	QPushButton *closeButton = new QPushButton("Close", qf);
	QPushButton *applyButton = new QPushButton("Apply", qf);
	QPushButton *defaultButton = new QPushButton("Default", qf);

	gridLayout->addWidget(defaultButton,nbut,0,Qt::AlignBottom);
	gridLayout->addWidget(closeButton,nbut+1,0,Qt::AlignBottom);
	gridLayout->addWidget(applyButton,nbut+1,1,Qt::AlignBottom);

	connect(applyButton,SIGNAL(clicked()),this,SLOT(applyClick()));
	connect(closeButton,SIGNAL(clicked()),this,SLOT(closeClick()));
	connect(defaultButton,SIGNAL(clicked()),this,SLOT(resetValues()));
 
	qf->showNormal();	
	qf->adjustSize();

	//if(this->isHidden())
	//{
		this->showNormal();
		this->adjustSize();
	//}
	
}


/* click event for the apply button of the standard plugin window */
void MeshlabStdDialog::applyClick()
{
	  QAction *q = curAction;
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
		  }
	  }

	  curmwi->executeFilter(q,curParSet);
		
}

/* click event for the close button of the standard plugin window */

void MeshlabStdDialog::closeClick()
{
	this->hide();
}

void MeshlabStdDialog::resizeEvent ( QResizeEvent *  )
{
	if(!this->isFloating())
		return;

	if(restorelastsize)
	{
		this->resize(lastsize);
		restorelastsize = false;

		// this is the only way to convince QT to refresh the window clipcontrols...
		this->hide();
		this->showNormal();
	}
	else
	{
		QSize siz = this->size();
		lastsize.setWidth(siz.width());
		lastsize.setHeight(siz.height());
	}
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
