
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
	  qf->setEnabled(false);

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
	  
	  
	  this->repaint();
	  curmwi->executeFilter(curaction,&par);
	  resetMe();
  }
