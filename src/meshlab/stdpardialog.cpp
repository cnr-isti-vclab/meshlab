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




/* manages the setup of the standard plugin window, when the execution of a plugin filter is requested */
void MeshlabStdDialog::loadPluginAction(MeshFilterInterface *mfi,MeshModel *mm,QAction *q,MainWindowInterface *mwi)
  {
		StdParList *newparlist = new StdParList();

	  if(mm == NULL)
		  return;

	  /* checks wether the plugin action wants to handle parameters input by the standard plugin window or by itself */
	  if(!mfi->getStdFields(q,*mm,*newparlist))
	  {
		  /* the plugin action wants to handle parameters input by itself: the executeFilter() function is directly called */
		  FilterParameter fp;
		  fp.clear();
		  mwi->executeFilter(q,&fp);
	  }
	  else
	  {
		  /* the plugin action wants to handle parameters input by the standard plugin window */
		  	resetMe();
			delete parlist;
			parlist = newparlist;

			curaction = q;
			curmodel = mm;
			curmfi = mfi;
			curmwi = mwi;
			loadFrameContent(q->text());
	  }


  }
	void MeshlabStdDialog::initValues()
	{
		curaction = NULL;
		stdfieldwidgets.clear();
		curmodel = NULL;
		curmfi = NULL;
		curmwi = NULL;
	}

void MeshlabStdDialog::resetMe()
{
  stdfieldwidgets.clear();

  parlist->clear();

  QFrame *newqf = new MeshlabStdDialogFrame(this);

  newqf->setFrameStyle(QFrame::Box | QFrame::Sunken);
	newqf->setMinimumSize(75, 75);
	setWidget(newqf);

  delete qf;
  qf = newqf;


  initValues();
}

void MeshlabStdDialog::loadFrameContent(QString actiondesc)
{


	qf->hide();	
	setWindowTitle(actiondesc);

	QGridLayout *gridLayout = new QGridLayout(qf);
    qf->setLayout(gridLayout);

	QCheckBox *qcb;
	QLineEdit *qle;
	QLabel *ql;
	AbsPercWidget *apw;

	/* creates widgets for the standard parameters */


	for(int i = 0; i < parlist->count(); i++)
	{
		QString *ttip = parlist->getFieldToolTip(i);
	  switch(parlist->getFieldType(i))
	  {
	  case MESHLAB_STDPAR_PARBOOL:
		  qcb = new QCheckBox(parlist->getFieldDesc(i),qf);
		  
		  if(ttip != NULL)
			qcb->setToolTip(*ttip);
		  
		  if(parlist->getFieldVal(i).toBool())
			  qcb->setCheckState(Qt::Checked);

		  gridLayout->addWidget(qcb,i,0,1,2,Qt::AlignTop);

		  stdfieldwidgets.push_back(qcb);

		  break;
	  case MESHLAB_STDPAR_PARFLOAT:
		 ql = new QLabel(parlist->getFieldDesc(i),qf);
		  
		  if(ttip != NULL)
			ql->setToolTip(*ttip);
		  
		  qle = new QLineEdit(QString::number(parlist->getFieldVal(i).toDouble(),'g',3),qf); // better formatting of floating point numbers		  
		  gridLayout->addWidget(ql,i,0,Qt::AlignTop);
		  gridLayout->addWidget(qle,i,1,Qt::AlignTop);
		  stdfieldwidgets.push_back(qle);

		  break;
	  case MESHLAB_STDPAR_PARINT:
	  case MESHLAB_STDPAR_PARSTRING:
		  ql = new QLabel(parlist->getFieldDesc(i),qf);
		  
		  if(ttip != NULL)
			ql->setToolTip(*ttip);
		  
		  qle = new QLineEdit(parlist->getFieldVal(i).toString(),qf);
		  
		  gridLayout->addWidget(ql,i,0,Qt::AlignTop);
		  gridLayout->addWidget(qle,i,1,Qt::AlignTop);

		  stdfieldwidgets.push_back(qle);

		  break;
	  case MESHLAB_STDPAR_PARABSPERC:
		  QString desc = parlist->getFieldDesc(i) + " (abs and %)";
		  ql = new QLabel(desc ,qf);
		  
		  if(ttip != NULL)
			ql->setToolTip(*ttip);
		  
		  apw = new AbsPercWidget(qf,float(parlist->getFieldVal(i).toDouble()),parlist->getMin(i),parlist->getMax(i));
		  gridLayout->addWidget(ql,i,0,Qt::AlignTop);
		  gridLayout->addLayout(apw,i,1,Qt::AlignTop);
	  		
		  stdfieldwidgets.push_back(apw);

		  break;
	  }

	}

	int nbut = parlist->count();

	QPushButton *closeButton = new QPushButton("Close", qf);
	QPushButton *applyButton = new QPushButton("Apply", qf);

	gridLayout->addWidget(closeButton,nbut,0,Qt::AlignBottom);
	gridLayout->addWidget(applyButton,nbut,1,Qt::AlignBottom);

	connect(applyButton,SIGNAL(clicked()),this,SLOT(applyClick()));
	connect(closeButton,SIGNAL(clicked()),this,SLOT(closeClick()));

	qf->showNormal();	

	if(this->isHidden())
	{
		this->showNormal();
		this->adjustSize();
	}
	
}

void MeshlabStdDialog::stdClick()
{
	  FilterParameter par;
	  QAction *q = curaction;

	  par.clear();
	  for(int i = 0; i < parlist->count(); i++)
	  {
		  QString &sname = parlist->getFieldName(i);
		  switch(parlist->getFieldType(i))
		  {
		  case MESHLAB_STDPAR_PARBOOL:
			  par.addBool(sname,((QCheckBox *)stdfieldwidgets[i])->checkState() == Qt::Checked);
			  break;
		  case MESHLAB_STDPAR_PARINT:
			  par.addInt(sname,((QLineEdit *)stdfieldwidgets[i])->text().toInt());
			  break;
		  case MESHLAB_STDPAR_PARFLOAT:
			  par.addFloat(sname,((QLineEdit *)stdfieldwidgets[i])->text().toFloat());
			  break;
		  case MESHLAB_STDPAR_PARABSPERC:
			  par.addFloat(sname,((AbsPercWidget *)stdfieldwidgets[i])->getValue());
			  break;
		  case MESHLAB_STDPAR_PARSTRING:
			  par.addString(sname,((QLineEdit *)stdfieldwidgets[i])->text());
			  break;
		  }
	  }

	  curmwi->executeFilter(q,&par);

}

/* click event for the apply button of the standard plugin window */

void MeshlabStdDialog::applyClick()
{

	stdClick();

	QAction *curactions = curaction;
	MeshModel *curmodels = curmodel;
	MeshFilterInterface *curmfis = curmfi;
	MainWindowInterface *curmwis = curmwi;
	resetMe();
	curaction = curactions;
	curmodel = curmodels;
	curmfi = curmfis;
	curmwi = curmwis;


	curmfi->getStdFields(curaction,*curmodel,*parlist);
    loadFrameContent(curaction->text());
}

/* click event for the close button of the standard plugin window */

void MeshlabStdDialog::closeClick()
{
	this->hide();
	// stdClick(); commented out when switched from ok to close
}

void MeshlabStdDialog::topLevelChanged (bool topLevel)
{
	if(topLevel)
		restorelastsize = true; /* i want to restore the old size but i can't do it here
								   because, after the topLevelChanged event, QT wants to
								   resize the window, so i must postpone the resize in order
								   to prevent being overridden by the QT resize */
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
