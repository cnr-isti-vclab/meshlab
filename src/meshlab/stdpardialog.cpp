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

#include<QObject>
#include "meshmodel.h"
#include "interfaces.h"
#include "mainwindow.h"
//#include "plugindialog.h"
#include "stdpardialog.h"
using namespace vcg;
MeshlabStdDialog::MeshlabStdDialog(QWidget *p)
:QDockWidget(QString("Plugin"), p)
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
void MeshlabStdDialog::showAutoDialog(MeshFilterInterface *mfi, MeshModel *mm, MeshDocument * mdp, QAction *action, MainWindowInterface *mwi, QWidget *gla)
  {
		curAction=action;
		curmfi=mfi;
		curmwi=mwi;
		curParSet.clear();
		curModel = mm;
		curMeshDoc = mdp;
//		MainWindow * mwp = dynamic_cast<MainWindow *>(mwi);
//		MainWindow * mwp = (MainWindow *)(mwi);
		curgla=gla;

		mfi->initParameterSet(action, *mdp, curParSet);
		createFrame();
		loadFrameContent(mdp);
		if(isDynamic())
		{
			int mask = curParSet.getDynamicFloatMask();
			meshState.create(mask, curModel);
			connect(stdParFrame,SIGNAL(dynamicFloatChanged(int)), this, SLOT(applyDynamic()));
		}
  }

	bool MeshlabStdDialog::isDynamic()
	{
		return (curParSet.getDynamicFloatMask()!= 0);
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
					else ((QCheckBox *)stdfieldwidgets.at(i))->setCheckState(Qt::Unchecked);
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
				case FilterParameter::PARMESH:
					if(NULL != fpi.pointerVal)
						((MeshEnumWidget *)stdfieldwidgets.at(i))->setMesh((MeshModel *)(fpi.pointerVal));
					else
					{
						int index = fpi.fieldVal.toInt();
						//if a mesh exists at this index leave it otherwise pick 0
						if(index >= ((MeshEnumWidget *)stdfieldwidgets.at(i))->getSize() ) index = 0;

						((MeshEnumWidget *)stdfieldwidgets.at(i))->setEnum(index);

					}
					break;
				case FilterParameter::PARFLOATLIST:
					{
						QList<QVariant> list = fpi.fieldVal.toList();
						((QVariantListWidget *)stdfieldwidgets.at(i))->setList(list);
					}
					break;
				case FilterParameter::PARDYNFLOAT:
				{
					float initVal = (float)fpi.fieldVal.toDouble();
					((DynamicFloatWidget *)stdfieldwidgets.at(i))->setValue(initVal);
				}
					break;
				case FilterParameter::PAROPENFILENAME:
				case FilterParameter::PARSAVEFILENAME:
					((GetFileNameWidget *)stdfieldwidgets.at(i))->setFileName(fpi.fieldVal.toString());
					break;
				default: assert(0);

			}
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

void StdParFrame::loadFrameContent(FilterParameterSet &curParSet,MeshDocument *mdPt)
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
	QLayout *layout;
	MeshEnumWidget *mew;
	DynamicFloatWidget *dfw;
	Point3fWidget *p3w;

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

				layout = new EnumWidget(this, fpi.fieldVal.toUInt(), fpi.enumValues);
				gridLayout->addWidget(ql,i,0,Qt::AlignTop);
				gridLayout->addLayout(layout,i,1,Qt::AlignTop);

				stdfieldwidgets.push_back(layout);

				break;

			case FilterParameter::PARMESH:
				{
					assert(mdPt);
					ql = new QLabel(fpi.fieldDesc,this);
					ql->setToolTip(fpi.fieldToolTip);

					MeshModel *defaultModel = 0;
					int position = fpi.fieldVal.toInt();

					//if there was no pointer try to use the position value to find the mesh in the mesh document
					if(NULL == (fpi.pointerVal) &&
							position >= 0 &&
							position < mdPt->meshList.size() )
					{
						//get the model from the position in the meshDocument
						defaultModel = mdPt->getMesh(position);
					} else //use the pointer provided
						defaultModel = (MeshModel *)(fpi.pointerVal);

					mew = new MeshEnumWidget(this, defaultModel, *mdPt);

					gridLayout->addWidget(ql,i,0,Qt::AlignTop);
					gridLayout->addLayout(mew,i,1,Qt::AlignTop);
					stdfieldwidgets.push_back(mew);
				}
				break;

			case FilterParameter::PARFLOATLIST:
				{
					ql = new QLabel(fpi.fieldDesc,this);
					ql->setToolTip(fpi.fieldToolTip);

					QToolButton *addButton = new QToolButton(this);
					addButton->setText("Add Row");
					QToolButton *removeButton = new QToolButton(this);
					removeButton->setText("Remove Row");

					QVBoxLayout *leftLayout = new QVBoxLayout(this);
					leftLayout->addWidget(ql);
					leftLayout->addWidget(addButton);
					leftLayout->addWidget(removeButton);

					QList<QVariant> list = fpi.fieldVal.toList();

					layout = new QVariantListWidget(this, list);
					gridLayout->addLayout(leftLayout,i,0,Qt::AlignTop);
					gridLayout->addLayout(layout,i,1,Qt::AlignTop);

					connect(addButton, SIGNAL(clicked()), layout, SLOT(addRow()));
					connect(removeButton, SIGNAL(clicked()), layout, SLOT(removeRow()));

					stdfieldwidgets.push_back(layout);
				}
				break;

			case FilterParameter::PARDYNFLOAT :
				ql = new QLabel(fpi.fieldDesc ,this);
				ql->setToolTip(fpi.fieldToolTip);

				dfw = new DynamicFloatWidget(this,float(fpi.fieldVal.toDouble()),fpi.min,fpi.max,fpi.mask);
				gridLayout->addWidget(ql,i,0,Qt::AlignTop);
				gridLayout->addLayout(dfw,i,1,Qt::AlignTop);

				stdfieldwidgets.push_back(dfw);
				connect(dfw,SIGNAL(valueChanged(int)),this,SIGNAL( dynamicFloatChanged(int) ));
				break;


			case FilterParameter::PARPOINT3F :
			{
				ql = new QLabel(fpi.fieldDesc ,this);
				ql->setToolTip(fpi.fieldToolTip);

				Point3f point;
				QList<QVariant> pointVals = fpi.fieldVal.toList();
				assert(pointVals.size()==3);
				for(int ii=0;ii<3;++ii)
				point[ii]=pointVals[ii].toDouble();

				p3w = new Point3fWidget(this,point,gla);
				gridLayout->addWidget(ql,i,0,Qt::AlignTop);
				gridLayout->addLayout(p3w,i,1,Qt::AlignTop);
				stdfieldwidgets.push_back(p3w);
			}
				break;


			case FilterParameter::PAROPENFILENAME:
				{
					ql = new QLabel(fpi.fieldDesc,this);
					ql->setToolTip(fpi.fieldToolTip);

					QString defaultFileName = fpi.fieldVal.toString();

					GetFileNameWidget *fileNameWidget = new GetFileNameWidget(
							this, defaultFileName, true, fpi.enumValues.back());

					gridLayout->addWidget(ql, i, 0, Qt::AlignTop);
					gridLayout->addLayout(fileNameWidget, i, 1, Qt::AlignTop);

					stdfieldwidgets.push_back(fileNameWidget);
				}
				break;

			case FilterParameter::PARSAVEFILENAME:
				{
					ql = new QLabel(fpi.fieldDesc,this);
					ql->setToolTip(fpi.fieldToolTip);

					QString defaultFileName = fpi.fieldVal.toString();

					GetFileNameWidget *fileNameWidget = new GetFileNameWidget(
							this, defaultFileName, false, fpi.enumValues.back());

					gridLayout->addWidget(ql, i, 0, Qt::AlignTop);
					gridLayout->addLayout(fileNameWidget, i, 1, Qt::AlignTop);

					stdfieldwidgets.push_back(fileNameWidget);
				}
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
	qf->updateGeometry();	
	qf->adjustSize();
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
		  case FilterParameter::PARMESH:
			  curParSet.setMesh(sname,((MeshEnumWidget *)stdfieldwidgets[i])->getMesh(), ((MeshEnumWidget *)stdfieldwidgets[i])->getEnum());
			  break;
		  case FilterParameter::PARFLOATLIST:
			  curParSet.findParameter(sname)->fieldVal = ((QVariantListWidget *)stdfieldwidgets[i])->getList();
			  break;
		 case FilterParameter::PARDYNFLOAT:
			  curParSet.findParameter(sname)->fieldVal = ((DynamicFloatWidget *)stdfieldwidgets[i])->getValue();
			  break;
		 case FilterParameter::PAROPENFILENAME:
			 curParSet.setOpenFileName(sname, ((GetFileNameWidget *)stdfieldwidgets[i])->getFileName());
			 break;
		 case FilterParameter::PARSAVEFILENAME:
		 	curParSet.setSaveFileName(sname, ((GetFileNameWidget *)stdfieldwidgets[i])->getFileName());
		 	break;
		 case FilterParameter::PARPOINT3F:
			 curParSet.setPoint3f(sname,((Point3fWidget *)stdfieldwidgets[i])->getValue());
			 break;
		 default:
				assert(0);
		  }
	  }
}


/* click event for the apply button of the standard plugin window */
// If the filter has some dynamic parameters
// - before applying the filter restore the original state of the mesh.
// - after applying the filter reget the state of the mesh.

void MeshlabStdDialog::applyClick()
{
	QAction *q = curAction;
	stdParFrame->readValues(curParSet);

	int mask = curParSet.getDynamicFloatMask();
	if(mask)	meshState.apply(curModel);

	curmwi->executeFilter(q, curParSet, false);

	if(mask)	meshState.create(mask, curModel);

}

void MeshlabStdDialog::applyDynamic()
{
	if(!previewCB->isChecked()) return;
	QAction *q = curAction;
	stdParFrame->readValues(curParSet);
	// Restore the
	meshState.apply(curModel);
	curmwi->executeFilter(q, curParSet, true);
}

void MeshlabStdDialog::togglePreview()
{
	if(previewCB->isChecked()) applyDynamic();
	else meshState.apply(curModel);
}

/* click event for the close button of the standard plugin window */

void MeshlabStdDialog::closeClick()
{
	int mask = curParSet.getDynamicFloatMask();
	if(mask)	meshState.apply(curModel);
	this->close();
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

	  absSB->setMinimum(m_min-(m_max-m_min));
	  absSB->setMaximum(m_max*2);
	  absSB->setAlignment(Qt::AlignRight);
		int decimals= 6-ceil(log10(fabs(m_max-m_min)) ) ;
	//qDebug("range is (%f %f) %f ",m_max,m_min,fabs(m_max-m_min));
	//qDebug("log range is %f ",log10(fabs(m_max-m_min)));
	  absSB->setDecimals(decimals);
	  absSB->setSingleStep((m_max-m_min)/100.0);
	  absSB->setValue(defaultv);

	  percSB->setMinimum(-200);
	  percSB->setMaximum(200);
	  percSB->setAlignment(Qt::AlignRight);
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
// Point3fWidget Implementation
/******************************************/
Point3fWidget::Point3fWidget(QWidget *p, Point3f defaultv, QWidget *gla_curr):QGridLayout(NULL)
{
//	gla=gla_curr;
	for(int i =0;i<3;++i)
		{
			coordSB[i]= new QLineEdit(p);
			QFont baseFont=coordSB[i]->font();
			if(baseFont.pixelSize() != -1) baseFont.setPixelSize(baseFont.pixelSize()*3/4);
															  else baseFont.setPointSize(baseFont.pointSize()*3/4);
			coordSB[i]->setFont(baseFont);
			coordSB[i]->setMinimumWidth(coordSB[i]->sizeHint().width()/4);
			coordSB[i]->setMaximumWidth(coordSB[i]->sizeHint().width()/3);
			coordSB[i]->setValidator(new QDoubleValidator(p));
			coordSB[i]->setAlignment(Qt::AlignRight);
			this->addWidget(coordSB[i],0,i,Qt::AlignHCenter);
		}
	this->setValue(defaultv);
	if(gla_curr) // if we have a connection to the current glarea we can setup the additional button for getting the current view direction.
		{
			getViewButton = new QPushButton(tr("Get View Dir"),p);
			this->addWidget(getViewButton,0,3,Qt::AlignHCenter);
			connect(getViewButton,SIGNAL(clicked()),gla_curr,SLOT(sendViewDir()));
			connect(gla_curr,SIGNAL(transmitViewDir(vcg::Point3f)),this,SLOT(setValue(vcg::Point3f)));
		}
}

Point3fWidget::~Point3fWidget()
{
	for(int i =0;i<3;++i)
		{
			delete coordSB[i];
		}
}

void Point3fWidget::setValue(Point3f newVal)
{
	for(int i =0;i<3;++i)
		coordSB[i]->setText(QString::number(newVal[i],'g',4));
}

vcg::Point3f Point3fWidget::getValue()
{
	return Point3f(coordSB[0]->text().toFloat(),coordSB[1]->text().toFloat(),coordSB[2]->text().toFloat());
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
	 if(newColor.isValid()) setColor(newColor);
}

/******************************************/
//EnumWidget Implementation
/******************************************/
EnumWidget::EnumWidget(QWidget *p, int defaultEnum, QStringList values) {
	Init(p,defaultEnum,values);
}

void EnumWidget::Init(QWidget *p, int defaultEnum, QStringList values)
{
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

int EnumWidget::getSize()
{
	return enumCombo->count();
}

/******************************************/
//MeshEnumWidget Implementation
/******************************************/
MeshEnumWidget::MeshEnumWidget(QWidget *p, MeshModel *defaultMesh, MeshDocument &_md)
{
	md=&_md;
	QStringList meshNames;

	//make the default mesh Index be 0
	int defaultMeshIndex = -1;

	for(int i=0;i<md->meshList.size();++i)
	 {
		QString shortName(QFileInfo(md->meshList.at(i)->fileName.c_str()).fileName());
		meshNames.push_back(shortName);
		if(md->meshList.at(i) == defaultMesh) defaultMeshIndex = i;
	 }

	//add a blank choice because there is no default available
	if(defaultMeshIndex == -1)
	{
		meshNames.push_back("");
		defaultMeshIndex = meshNames.size()-1;  //have the blank choice be shown
	}
	Init(p,defaultMeshIndex,meshNames);
}

MeshModel * MeshEnumWidget::getMesh()
{
	//test to make sure index is in bounds
	int index = enumCombo->currentIndex();
	if(index < md->meshList.size() && index > -1)
		return md->meshList.at(enumCombo->currentIndex());
	else return NULL;
}

void MeshEnumWidget::setMesh(MeshModel * newMesh)
{
	for(int i=0; i < md->meshList.size(); ++i)
	{
		if(md->meshList.at(i) == newMesh) setEnum(i);
	}
}



/******************************************
 QVariantListWidget Implementation
******************************************/
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

/******************************************
 GetFileNameButton Implementation
******************************************/
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

GenericParamDialog::GenericParamDialog(QWidget *p, FilterParameterSet *_curParSet, QString title, MeshDocument *_meshDocument)
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


/******************************************/
// DynamicFloatWidget Implementation
/******************************************/
DynamicFloatWidget::DynamicFloatWidget(QWidget *p, double defaultv, double _minVal, double _maxVal, int _mask):QGridLayout(NULL)
{
	mask = _mask;
	minVal = _minVal;
	maxVal = _maxVal;
	valueLE = new QLineEdit(p);
	valueSlider = new QSlider(Qt::Horizontal,p);

	valueSlider->setMinimum(0);
	valueSlider->setMaximum(100);
	valueSlider->setValue(floatToInt(defaultv));
	valueLE->setValidator(new QDoubleValidator (minVal, maxVal, 5, valueLE));
	valueLE->setText(QString::number(defaultv));

	this->addWidget(valueLE,0,0,Qt::AlignHCenter);
	this->addWidget(valueSlider,0,1,Qt::AlignHCenter);

	connect(valueLE,SIGNAL(textChanged(const QString &)),this,SLOT(setValue()));
	connect(valueSlider,SIGNAL(valueChanged(int)),this,SLOT(setValue(int)));
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
	emit valueChanged(mask);
}

float DynamicFloatWidget::intToFloat(int val)
{
	return minVal+float(val)/100.0f*(maxVal-minVal);
}
int DynamicFloatWidget::floatToInt(float val)
{
	return int (100.0f*(val-minVal)/(maxVal-minVal));
}

