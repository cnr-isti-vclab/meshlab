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
Revision 1.2  2006/12/13 21:54:35  pirosu
2 updates for the standard plugin window: 1) it recovers its last size when it is undocked and 2) it closes itself when a filter is applied (only if it is floating)


Revision 1.0  2006/12/13 17:37:02  pirosu
Added standard plugin window support

****************************************************************************/


#include "stdpardialog.h"




/* manages the setup of the standard plugin window, when the execution of a plugin filter is requested */
void MeshlabStdDialog::loadPluginAction(MeshFilterInterface *mfi,MeshModel *mm,QAction *q,MainWindowInterface *mwi)
  {
	  if(mm == NULL)
		  return;


	  QWidget *extraw = NULL;
	  char *actiondesc = NULL;

	  resetMe();

	  /* checks wether the plugin action wants to handle parameters input by the standard plugin window or by itself */
	  if(!mfi->getStdFields(q,*mm,parlist,&actiondesc,&extraw))
	  {

		  if(this->isFloating())
			  this->hide();

		  /* the plugin action wants to handle parameters input by itself: the executeFilter() function is directly called */
		  mwi->executeFilter(q,NULL);
	  }
	  else
	  {
		  /* the plugin action wants to handle parameters input by the standard plugin window */
		if(this->isHidden())
			this->showNormal();

		
		setWindowTitle(QString(actiondesc));

		curextra = extraw;
		curaction = q;

		QGridLayout *gridLayout = new QGridLayout(qf);
	    qf->setLayout(gridLayout);

		QCheckBox *qcb;
		QLineEdit *qle;
		QLabel *ql;

		/* creates widgets for the standard parameters */


		for(int i = 0; i < parlist.count(); i++)
		{
		  switch(parlist.getFieldType(i))
		  {
		  case MESHLAB_STDPAR_PARBOOL:
			  qcb = new QCheckBox(parlist.getFieldDesc(i),qf);
			  
			  if(parlist.getFieldVal(i).toBool())
				  qcb->setCheckState(Qt::Checked);

			  gridLayout->addWidget(qcb,i,0,1,2,Qt::AlignTop);

			  stdfieldwidgets.push_back(qcb);

			  break;
		  case MESHLAB_STDPAR_PARINT:
		  case MESHLAB_STDPAR_PARFLOAT:
		  case MESHLAB_STDPAR_PARSTRING:
			  ql = new QLabel(parlist.getFieldDesc(i),qf);
			  qle = new QLineEdit(parlist.getFieldVal(i).toString(),qf);
			  
			  gridLayout->addWidget(ql,i,0,Qt::AlignTop);
			  gridLayout->addWidget(qle,i,1,Qt::AlignTop);

			  stdfieldwidgets.push_back(qle);

			  break;
		  }

		}


		/* creates the extra custom widget, if requested */

		if(curextra != NULL)
   		  gridLayout->addWidget(curextra,parlist.count(),0,1,2,Qt::AlignTop);

		/* appends the apply button */

		int nbut = (curextra == NULL) ? parlist.count() : parlist.count()+1;

		QPushButton *applyButton = new QPushButton("Apply", qf);

		gridLayout->addWidget(applyButton,nbut,0,1,2,Qt::AlignBottom);
		gridLayout->setRowStretch(nbut,2);

		curmwi = mwi;
		this->adjustSize();
		connect(applyButton,SIGNAL(clicked()),this,SLOT(applyClick()));
	  }


  }
	void MeshlabStdDialog::initValues()
	{
		curaction = NULL;
		stdfieldwidgets.clear();
		curextra = NULL;
	}

	void MeshlabStdDialog::resetMe()
	{
	  stdfieldwidgets.clear();

	  parlist.clear();

	  QFrame *newqf = new MeshlabStdDialogFrame(this);

   	  newqf->setFrameStyle(QFrame::Box | QFrame::Sunken);
    	newqf->setMinimumSize(75, 75);
		setWidget(newqf);
		setWindowTitle(QString("Plugin"));

	  delete qf;
	  qf = newqf;


	  initValues();
	}

/* click event for the apply button of the standard plugin window */
void MeshlabStdDialog::applyClick()
{
	  FilterParameter par;
	  QAction *q = curaction;

	  par.clear();
	  for(int i = 0; i < parlist.count(); i++)
	  {
		  QString &sname = parlist.getFieldName(i);
		  switch(parlist.getFieldType(i))
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
		  case MESHLAB_STDPAR_PARSTRING:
			  par.addString(sname,((QLineEdit *)stdfieldwidgets[i])->text());
			  break;
		  }
	  }

	  resetMe();

	  if(this->isFloating())
		  this->hide();
	  else
		  this->repaint();

	  curmwi->executeFilter(q,&par);
  }

void MeshlabStdDialog::topLevelChanged (bool topLevel)
{
	if(topLevel)
		restorelastsize = true; /* i want to restore the old size but i can't do it here
								   because, after the topLevelChanged event, QT wants to
								   resize the window, so i must postpone the resize in order
								   to prevent being overridden by the QT resize */
}

void MeshlabStdDialog::resizeEvent ( QResizeEvent * event )
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