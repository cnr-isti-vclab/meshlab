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

#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QFileDialog>
#include <QDialogButtonBox>
#include <QClipboard>
#include <QColorDialog>
#include <QToolBar>
#include "mainwindow.h"


using namespace vcg;

StdParFrame::StdParFrame(QWidget *p, QWidget *curr_gla )
    :QFrame(p)
{
    gla=curr_gla;
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
void StdParFrame::loadFrameContent(RichParameterSet &curParSet,MeshDocument * /*_mdPt*/ )
{
    if(layout()) delete layout();
    QGridLayout* glay = new QGridLayout();
    // QGridLayout *vlayout = new QGridLayout(this);
    //    vLayout->setAlignment(Qt::AlignTop);
    RichWidgetInterfaceConstructor rwc(this);
    for(int i = 0; i < curParSet.paramList.count(); i++)
    {
        RichParameter* fpi=curParSet.paramList.at(i);
        fpi->accept(rwc);
        //vLayout->addWidget(rwc.lastCreated,i,0,1,1,Qt::AlignTop);
        stdfieldwidgets.push_back(rwc.lastCreated);
        helpList.push_back(rwc.lastCreated->helpLab);
        //glay->addItem(rwc.lastCreated->leftItem(),i,0);
        //glay->addItem(rwc.lastCreated->centralItem(),i,1);
        //glay->addItem(rwc.lastCreated->rightItem(),i,2);
        rwc.lastCreated->addWidgetToGridLayout(glay,i);

    } // end for each parameter
    setLayout(glay);
    this->setMinimumSize(glay->sizeHint());
	glay->setSizeConstraint(QLayout::SetMinimumSize);
    this->showNormal();
    this->adjustSize();
}

void StdParFrame::toggleHelp()
{
    for(int i = 0; i < helpList.count(); i++)
        helpList.at(i)->setVisible(!helpList.at(i)->isVisible()) ;
    updateGeometry();
    adjustSize();
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


/******************************************/
// AbsPercWidget Implementation
/******************************************/


//QGridLayout(NULL)
AbsPercWidget::AbsPercWidget(QWidget *p, RichAbsPerc* rabs):MeshLabWidget(p,rabs)

{
    AbsPercDecoration* absd = reinterpret_cast<AbsPercDecoration*>(rp->pd);
    m_min = absd->min;
    m_max = absd->max;

    fieldDesc = new QLabel(rp->pd->fieldDesc + " (abs and %)",this);
    fieldDesc->setToolTip(rp->pd->tooltip);
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

    //  gridLay->addWidget(fieldDesc,row,0,Qt::AlignHCenter);

    vlay = new QGridLayout();
    vlay->addWidget(absLab,0,0,Qt::AlignHCenter);
    vlay->addWidget(percLab,0,1,Qt::AlignHCenter);

    vlay->addWidget(absSB,1,0,Qt::AlignTop);
    vlay->addWidget(percSB,1,1,Qt::AlignTop);

    //gridLay->addLayout(lay,row,1,Qt::AlignTop);

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
    disconnect(percSB,SIGNAL(valueChanged(double)),this,SLOT(on_percSB_valueChanged(double)));
    percSB->setValue((100*(newv - m_min))/(m_max - m_min));
    connect(percSB,SIGNAL(valueChanged(double)),this,SLOT(on_percSB_valueChanged(double)));
    emit dialogParamChanged();
}

void AbsPercWidget::on_percSB_valueChanged(double newv)
{
    disconnect(absSB,SIGNAL(valueChanged(double)),this,SLOT(on_absSB_valueChanged(double)));
    absSB->setValue((m_max - m_min)*0.01*newv + m_min);
    connect(absSB,SIGNAL(valueChanged(double)),this,SLOT(on_absSB_valueChanged(double)));
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

void AbsPercWidget::addWidgetToGridLayout( QGridLayout* lay,const int r )
{
    if (lay != NULL)
    {
        lay->addWidget(fieldDesc,r,0,Qt::AlignLeft);
        lay->addLayout(vlay,r,1,Qt::AlignTop);
    }
    MeshLabWidget::addWidgetToGridLayout(lay,r);
}
/******************************************/
// Point3fWidget Implementation
/******************************************/


//QHBoxLayout(NULL)
Point3fWidget::Point3fWidget(QWidget *p, RichPoint3f* rpf, QWidget *gla_curr): MeshLabWidget(p,rpf)
{
    //qDebug("Creating a Point3fWidget");
    paramName = rpf->name;
    //int row = gridLay->rowCount() - 1;
    descLab = new QLabel(rpf->pd->fieldDesc,this);
    descLab->setToolTip(rpf->pd->fieldDesc);
    //gridLay->addWidget(descLab,row,0);

    vlay = new QHBoxLayout();
    vlay->setSpacing(0);
    for(int i =0;i<3;++i)
    {
        coordSB[i]= new QLineEdit(this);
        QFont baseFont=coordSB[i]->font();
        if(baseFont.pixelSize() != -1) baseFont.setPixelSize(baseFont.pixelSize()*3/4);
        else baseFont.setPointSize(baseFont.pointSize()*3/4);
        coordSB[i]->setFont(baseFont);
        coordSB[i]->setMaximumWidth(coordSB[i]->sizeHint().width()/2);
        coordSB[i]->setValidator(new QDoubleValidator());
        coordSB[i]->setAlignment(Qt::AlignRight);
        coordSB[i]->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Preferred);
        vlay->addWidget(coordSB[i]);
        connect(coordSB[i],SIGNAL(textChanged(QString)),p,SIGNAL(parameterChanged()));
    }
    this->setValue(paramName,rp->val->getPoint3f());
    if(gla_curr) // if we have a connection to the current glarea we can setup the additional button for getting the current view direction.
    {
        getPoint3Button = new QPushButton("Get",this);
        getPoint3Button->setMaximumWidth(getPoint3Button->sizeHint().width()/2);

        getPoint3Button->setFlat(true);
        getPoint3Button->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
        //getPoint3Button->setMinimumWidth(getPoint3Button->sizeHint().width());
        //this->addWidget(getPoint3Button,0,Qt::AlignHCenter);
        vlay->addWidget(getPoint3Button);
        QStringList names;
        names << "View Dir.";
        names << "View Pos.";
        names << "Surf. Pos.";
        names << "Raster Camera Pos.";
		names << "Trackball Center";

        getPoint3Combo = new QComboBox(this);
        getPoint3Combo->addItems(names);
        //getPoint3Combo->setMinimumWidth(getPoint3Combo->sizeHint().width());
        //this->addWidget(getPoint3Combo,0,Qt::AlignHCenter);
        vlay->addWidget(getPoint3Combo);

        connect(getPoint3Button,SIGNAL(clicked()),this,SLOT(getPoint()));
        connect(getPoint3Combo,SIGNAL(currentIndexChanged(int)),this,SLOT(getPoint()));
        connect(gla_curr,SIGNAL(transmitViewDir(QString,Point3m)),this,SLOT(setValue(QString,Point3m)));
        connect(gla_curr,SIGNAL(transmitShot(QString,Shotm)),this,SLOT(setShotValue(QString,Shotm)));
        connect(gla_curr,SIGNAL(transmitSurfacePos(QString,Point3m)),this,SLOT(setValue(QString,Point3m)));
		connect(gla_curr,SIGNAL(transmitCameraPos(QString, Point3m)),this,SLOT(setValue(QString, Point3m)));
		connect(gla_curr,SIGNAL(transmitTrackballPos(QString, Point3m)),this,SLOT(setValue(QString, Point3m)));
        connect(this,SIGNAL(askViewDir(QString)),gla_curr,SLOT(sendViewDir(QString)));
        connect(this,SIGNAL(askViewPos(QString)),gla_curr,SLOT(sendViewerShot(QString)));
        connect(this,SIGNAL(askSurfacePos(QString)),gla_curr,SLOT(sendSurfacePos(QString)));
        connect(this,SIGNAL(askCameraPos(QString)),gla_curr,SLOT(sendRasterShot(QString)));
		connect(this,SIGNAL(askTrackballPos(QString)),gla_curr,SLOT(sendTrackballPos(QString)));
    }
    //gridLay->addLayout(lay,row,1,Qt::AlignTop);
}

void Point3fWidget::getPoint()
{
    int index = getPoint3Combo->currentIndex();
    //qDebug("Got signal %i",index);
    switch(index)
    {
		case 0: emit askViewDir(paramName);       break;
		case 1: emit askViewPos(paramName);       break;
		case 2: emit askSurfacePos(paramName);    break;
		case 3: emit askCameraPos(paramName);     break;
		case 4: emit askTrackballPos(paramName);  break;
		default : assert(0);
    }
}

Point3fWidget::~Point3fWidget() {
    //qDebug("Deallocating a point3fwidget");
    this->disconnect();
}

void Point3fWidget::setValue(QString name,Point3m newVal)
{
    //qDebug("setValue parametername: %s ", qUtf8Printable(name));
    if(name==paramName)
    {
        for(int i =0;i<3;++i)
            coordSB[i]->setText(QString::number(newVal[i],'g',4));
    }
}

void Point3fWidget::setShotValue(QString name,Shotf newValShot)
{
    Point3f p = newValShot.GetViewPoint();
    setValue(name,p);
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

void Point3fWidget::addWidgetToGridLayout( QGridLayout* lay,const int r )
{
    if (lay != NULL)
    {
        lay->addWidget(descLab,r,0);
        lay->addLayout(vlay,r,1);
    }
    MeshLabWidget::addWidgetToGridLayout(lay,r);
}
/******************************************/
// Matrix44fWidget Implementation
/******************************************/


//QHBoxLayout(NULL)
Matrix44fWidget::Matrix44fWidget(QWidget *p, RichMatrix44f* rpf,  QWidget *gla_curr): MeshLabWidget(p,rpf)
{
    valid = false;
    m.SetIdentity();
    paramName = rpf->name;
    //int row = gridLay->rowCount() - 1;

    descLab = new QLabel(rpf->pd->fieldDesc,this);
    descLab->setToolTip(rpf->pd->fieldDesc);
    //gridLay->addWidget(descLab,row,0,Qt::AlignTop);
    vlay = new QVBoxLayout();
    lay44 = new QGridLayout();



    for(int i =0;i<16;++i)
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
        lay44->addWidget(coordSB[i],i/4,i%4);
        connect(coordSB[i], SIGNAL(textChanged(const QString&)), this, SLOT(invalidateMatrix(const QString&)));
    }
    this->setValue(paramName,rp->val->getMatrix44f());

	QLabel* headerL = new QLabel("Matrix:", this);
	vlay->addWidget(headerL, 0, Qt::AlignTop);

    vlay->addLayout(lay44);

    QPushButton     * getMatrixButton = new QPushButton("Read from current layer");
    vlay->addWidget(getMatrixButton);

    QPushButton     * pasteMatrixButton = new QPushButton("Paste from clipboard");
    vlay->addWidget(pasteMatrixButton);

    //gridLay->addLayout(vlay,row,1,Qt::AlignTop);

    connect(gla_curr,SIGNAL(transmitMatrix(QString,Matrix44m)),this,SLOT(setValue(QString,Matrix44m)));
    connect(getMatrixButton,SIGNAL(clicked()),this,SLOT(getMatrix()));
    connect(pasteMatrixButton,SIGNAL(clicked()),this,SLOT(pasteMatrix()));
    connect(this,SIGNAL(askMeshMatrix(QString)),  gla_curr,SLOT(sendMeshMatrix(QString)));

}


Matrix44fWidget::~Matrix44fWidget() {}

void Matrix44fWidget::setValue(QString name,Matrix44m newVal)
{
    if(name==paramName)
    {
        for(int i =0;i<16;++i)
            coordSB[i]->setText(QString::number(newVal[i/4][i%4],'g',4));
        valid = true;
        m = newVal;
    }
}


vcg::Matrix44f Matrix44fWidget::getValue()
{
  if (!valid)
  {
    float val[16];
    for (unsigned int i = 0; i < 16; ++i)
      val[i] = coordSB[i]->text().toFloat();
    return Matrix44f(val);
  }
  return m;
}

void Matrix44fWidget::getMatrix()
{
    emit askMeshMatrix(QString("TransformMatrix"));
}

void Matrix44fWidget::pasteMatrix()
{
    QClipboard *clipboard = QApplication::clipboard();
    QString shotString = clipboard->text().trimmed();
    if (shotString.contains(' '))
    {
      QStringList list1 = shotString.split(" ");
      if (list1.size() != 16)
        return;
      valid = false;
      int id = 0;
      for (QStringList::iterator i = list1.begin(); i != list1.end(); ++i, ++id) {
        bool ok = true;
        (*i).toFloat(&ok);
        if (!ok) return;
      }
      id = 0;
      for (QStringList::iterator i = list1.begin(); i != list1.end(); ++i, ++id)
        coordSB[id]->setText(*i);
    }
    else
    {
      QByteArray value = QByteArray::fromBase64(shotString.toLocal8Bit());
      memcpy(m.V(), value.data(), sizeof(Matrix44m::ScalarType) * 16);
      int id = 0;
      for (int i = 0; i < 16; ++i, ++id)
        coordSB[id]->setText(QString::number(m.V()[i]));
    }
}

void Matrix44fWidget::collectWidgetValue()
{
  if (!valid)
  {
    vcg::Matrix44f  tempM;
    for (unsigned int i = 0; i < 16; ++i) tempM[i / 4][i % 4] = coordSB[i]->text().toFloat();
    rp->val->set(Matrix44fValue(tempM));
  }
  else
    rp->val->set(Matrix44fValue(m));
}

void Matrix44fWidget::resetWidgetValue()
{
  valid = false;
    vcg::Matrix44f  m; m.SetIdentity();
    for(unsigned int ii = 0; ii < 16;++ii)
        coordSB[ii]->setText(QString::number(rp->pd->defVal->getMatrix44f()[ii/4][ii%4],'g',3));
}

void Matrix44fWidget::setWidgetValue( const Value& nv )
{
  valid = true;
  m = nv.getMatrix44f();
    for(unsigned int ii = 0; ii < 16;++ii)
        coordSB[ii]->setText(QString::number(nv.getMatrix44f()[ii/4][ii%4],'g',3));
}

void Matrix44fWidget::addWidgetToGridLayout( QGridLayout* lay,const int r )
{
    if (lay != NULL)
    {
        lay->addWidget(descLab,r,0,Qt::AlignTop);
        lay->addLayout(vlay,r,1,Qt::AlignTop);
    }
    MeshLabWidget::addWidgetToGridLayout(lay,r);
}

void Matrix44fWidget::invalidateMatrix(const QString& /*s*/)
{
  valid = false;
}

/********************/
// ShotfWidget Implementation

ShotfWidget::ShotfWidget(QWidget *p, RichShotf* rpf, QWidget *gla_curr): MeshLabWidget(p,rpf)
{

    paramName = rpf->name;
    //int row = gridLay->rowCount() - 1;

    descLab = new QLabel(rpf->pd->fieldDesc,p);
    descLab->setToolTip(rpf->pd->fieldDesc);
    //gridLay->addWidget(descLab,row,0,Qt::AlignTop);

    hlay = new QHBoxLayout();


    this->setShotValue(paramName,rp->val->getShotf());
    if(gla_curr) // if we have a connection to the current glarea we can setup the additional button for getting the current view direction.
    {
        getShotButton = new QPushButton("Get shot",this);
        getShotButton->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Preferred);
        hlay->addWidget(getShotButton);

        QStringList names;
        names << "Current Trackball";
        names << "Current Mesh";
        names << "Current Raster";
        names << "From File";

        getShotCombo = new QComboBox(this);
        getShotCombo->addItems(names);
        hlay->addWidget(getShotCombo);
        connect(getShotCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(getShot()));
        connect(getShotButton,SIGNAL(clicked()),this,SLOT(getShot()));
        connect(gla_curr,SIGNAL(transmitShot(QString, Shotm)),this,SLOT(setShotValue(QString,Shotm)));
        connect(this,SIGNAL(askViewerShot(QString)),gla_curr,SLOT(sendViewerShot(QString)));
        connect(this,SIGNAL(askMeshShot(QString)),  gla_curr,SLOT(sendMeshShot(QString)));
        connect(this,SIGNAL(askRasterShot(QString)),gla_curr,SLOT(sendRasterShot(QString)));
    }
    //gridLay->addLayout(hlay,row,1,Qt::AlignTop);
}

void ShotfWidget::getShot()
{
    int index = getShotCombo->currentIndex();
    switch(index)  {
    case 0 : emit askViewerShot(paramName); break;
    case 1 : emit askMeshShot(paramName); break;
    case 2 : emit askRasterShot(paramName); break;
    case 3:
        {
            QString filename = QFileDialog::getOpenFileName(this, tr("Load xml camera"), "./", tr("Xml Files (*.xml)"));
            QFile qf(filename);
            QFileInfo qfInfo(filename);

            if( !qf.open(QIODevice::ReadOnly ) )
                return ;

            QDomDocument doc("XmlDocument");    //It represents the XML document
            if(!doc.setContent( &qf ))     return;
            qf.close();

            QString type = doc.doctype().name();

            //TextAlign file project
            //if(type == "RegProjectML")   loadShotFromTextAlignFile(doc);
            //View State file
            //else if(type == "ViewState") loadViewFromViewStateFile(doc);

            //qDebug("End file reading");


            // return true;
        }
    default : assert(0);
    }
}

ShotfWidget::~ShotfWidget() {}

void ShotfWidget::setShotValue(QString name,Shotm newVal)
{
    if(name==paramName)
    {
        curShot=newVal;
    }
}

vcg::Shotf ShotfWidget::getValue()
{
    return curShot;
}

void ShotfWidget::collectWidgetValue()
{
    rp->val->set(ShotfValue(curShot));
}

void ShotfWidget::resetWidgetValue()
{
    curShot = rp->pd->defVal->getShotf();
}

void ShotfWidget::setWidgetValue( const Value& nv )
{
    curShot = nv.getShotf();
}

void ShotfWidget::addWidgetToGridLayout( QGridLayout* lay,const int r )
{
    if (lay != NULL)
    {
        lay->addLayout(hlay,r,1);
        lay->addWidget(descLab,r,0);
    }
    MeshLabWidget::addWidgetToGridLayout(lay,r);
}
/********************/
// ComboWidget End Implementation

ComboWidget::ComboWidget(QWidget *p, RichParameter* rpar) :MeshLabWidget(p,rpar) {
}

void ComboWidget::Init(QWidget *p,int defaultEnum, QStringList values)
{
    enumLabel = new QLabel(this);
    enumLabel->setText(rp->pd->fieldDesc);
    enumCombo = new QComboBox(this);
    enumCombo->addItems(values);
    setIndex(defaultEnum);
    //int row = gridLay->rowCount() - 1;
    //gridLay->addWidget(enumLabel,row,0,Qt::AlignTop);
    //gridLay->addWidget(enumCombo,row,1,Qt::AlignTop);
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

void ComboWidget::addWidgetToGridLayout( QGridLayout* lay,const int r )
{
    if (lay != NULL)
    {
        lay->addWidget(enumLabel,r,0);
        lay->addWidget(enumCombo,r,1);
    }
    MeshLabWidget::addWidgetToGridLayout(lay,r);
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
    //defaultMeshIndex = -1;

    int currentmeshindex = -1;
    for(int i=0;i<md->meshList.size();++i)
    {
        QString shortName = md->meshList.at(i)->label();
        meshNames.push_back(shortName);
        /*  if(md->meshList.at(i) == rp->pd->defVal->getMesh())
        defaultMeshIndex = i;*/
        if(md->meshList.at(i) == rp->val->getMesh())
        {
            currentmeshindex = i;
            rpar->meshindex = currentmeshindex;
        }
    }

    //add a blank choice because there is no default available
    //if(defaultMeshIndex == -1)
    //{
    //    meshNames.push_back("");
    //    defaultMeshIndex = meshNames.size()-1;  //have the blank choice be shown
    //}
    Init(p,currentmeshindex,meshNames);
}

MeshModel * MeshWidget::getMesh()
{
    //test to make sure index is in bounds
    int index = enumCombo->currentIndex();
    if(index < md->meshList.size() && index > -1)
    {
        //RichMesh* rm = reinterpret_cast<RichMesh*>(rp);
        //rm->meshindex = index;
        return md->meshList.at(enumCombo->currentIndex());
    }
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
    //MeshDecoration* dec = reinterpret_cast<MeshDecoration*>(rp->pd);
    RichMesh* rm = reinterpret_cast<RichMesh*>(rp);
    rm->meshindex = enumCombo->currentIndex();
    rp->val->set(MeshValue(md->meshList.at(rm->meshindex)));
}

void MeshWidget::resetWidgetValue()
{
    int meshindex = -1;
    for(int i=0;i<md->meshList.size();++i)
    {
        if(md->meshList.at(i) == rp->pd->defVal->getMesh())
        {
            meshindex = i;
            //RichMesh* rm = reinterpret_cast<RichMesh*>(rp);
            //rm->meshindex = enumCombo->currentIndex();
        }

    }
    enumCombo->setCurrentIndex(meshindex);
}

void MeshWidget::setWidgetValue( const Value& nv )
{
    //WARNING!!!!! I HAVE TO THINK CAREFULLY ABOUT THIS FUNCTION!!!
    //assert(0);
    int meshindex = -1;
    for(int i=0;i<md->meshList.size();++i)
    {
        if(md->meshList.at(i) == nv.getMesh())
        {
            meshindex = i;
            //RichMesh* rm = reinterpret_cast<RichMesh*>(rp);
            //rm->meshindex = meshindex;
        }
    }
    enumCombo->setCurrentIndex(meshindex);
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
    int numbdecimaldigit = 4;
    minVal = reinterpret_cast<DynamicFloatDecoration*>(rdf->pd)->min;
    maxVal = reinterpret_cast<DynamicFloatDecoration*>(rdf->pd)->max;
    valueLE = new QLineEdit(this);
    valueLE->setAlignment(Qt::AlignRight);

    valueSlider = new QSlider(Qt::Horizontal,this);
    valueSlider->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    fieldDesc = new QLabel(rp->pd->fieldDesc,this);
    valueSlider->setMinimum(0);
    valueSlider->setMaximum(100);
    valueSlider->setValue(floatToInt(rp->val->getFloat()));
    const DynamicFloatDecoration* dfd = reinterpret_cast<const DynamicFloatDecoration*>(&(rp->pd));
    QFontMetrics fm(valueLE->font());
    QSize sz = fm.size(Qt::TextSingleLine,QString::number(0));
    valueLE->setValidator(new QDoubleValidator (dfd->min,dfd->max, numbdecimaldigit, valueLE));
    valueLE->setText(QString::number(rp->val->getFloat()));
    valueLE->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);

    //int row = gridLay->rowCount() - 1;
    //lay->addWidget(fieldDesc,row,0);

    hlay = new QHBoxLayout();
    hlay->addWidget(valueLE);
    hlay->addWidget(valueSlider);
    int maxlenghtplusdot = 8;//numbmaxvaluedigit + numbdecimaldigit + 1;
    valueLE->setMaxLength(maxlenghtplusdot);
    valueLE->setMaximumWidth(sz.width() * maxlenghtplusdot);


    //gridLay->addLayout(hlay,row,1);

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

void DynamicFloatWidget::addWidgetToGridLayout( QGridLayout* lay,const int r )
{
    if (lay != NULL)
    {
        lay->addWidget(fieldDesc,r,0);
        lay->addLayout(hlay,r,1);
    }
    MeshLabWidget::addWidgetToGridLayout(lay,r);
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
    :QWidget(p),rp(rpar) // this version of the line caused the very strange error of uncheckabe first bool widget
    //:rp(rpar)
{
    //setParent(p);
    if (rp!= NULL)
    {
        helpLab = new QLabel("<small>"+rpar->pd->tooltip +"</small>",this);
        helpLab->setTextFormat(Qt::RichText);
        helpLab->setWordWrap(true);
        helpLab->setVisible(false);
        helpLab->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
        helpLab->setMinimumWidth(250);
        helpLab->setMaximumWidth(QWIDGETSIZE_MAX);
        /*gridLay = qobject_cast<QGridLayout*>(p->layout());
        assert(gridLay != 0);
        row = gridLay->rowCount();*/
        //gridLay->addWidget(helpLab,row,3,1,1,Qt::AlignTop);
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

//void MeshLabWidget::setEqualSpaceForEachColumn()
//{
//	int singlewidth = gridLay->geometry().width() / gridLay->columnCount();
//	for (int ii = 0;ii < gridLay->columnCount();++ii)
//		gridLay->setColumnMinimumWidth(ii,singlewidth);
//}

void MeshLabWidget::addWidgetToGridLayout( QGridLayout* lay, const int r)
{
	if (lay != NULL)
	{
		lay->addWidget(helpLab, r, 2, 1, 1, Qt::AlignLeft);
	}
}

//connect(qcb,SIGNAL(stateChanged(int)),this,SIGNAL(parameterChanged()));
BoolWidget::BoolWidget(QWidget* p, RichBool* rb )
    :MeshLabWidget(p,rb)
{
    cb = new QCheckBox(rp->pd->fieldDesc,this);
    cb->setToolTip(rp->pd->tooltip);
    cb->setChecked(rp->val->getBool());

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

void BoolWidget::addWidgetToGridLayout(QGridLayout* lay,const int r)
{
    if (lay !=NULL)
        lay->addWidget(cb,r,0,1,2);
    MeshLabWidget::addWidgetToGridLayout(lay,r);
}

//connect(qle,SIGNAL(editingFinished()),this,SIGNAL(parameterChanged()));
LineEditWidget::LineEditWidget( QWidget* p,RichParameter* rpar )
    :MeshLabWidget(p,rpar)
{
    lab = new QLabel(rp->pd->fieldDesc,this);
    lned = new QLineEdit(this);
    //int row = gridLay->rowCount() -1;

    lab->setToolTip(rp->pd->tooltip);
    //gridLay->addWidget(lab,row,0);
    //gridLay->addWidget(lned,row,1);
    connect(lned,SIGNAL(editingFinished()),this,SLOT(changeChecker()));
    connect(this,SIGNAL(lineEditChanged()),p,SIGNAL(parameterChanged()));
    lned->setAlignment(Qt::AlignLeft);
}

LineEditWidget::~LineEditWidget()
{
    delete lned;
    delete lab;
}

void LineEditWidget::changeChecker()
{
    if(lned->text() != this->lastVal)
    {
        this->lastVal = lned->text();
        if(!this->lastVal.isEmpty())
            emit lineEditChanged();
    }
}

void LineEditWidget::addWidgetToGridLayout( QGridLayout* lay,const int r )
{
    if (lay !=NULL)
    {
        lay->addWidget(lab,r,0);
        lay->addWidget(lned,r,1);
    }
    MeshLabWidget::addWidgetToGridLayout(lay,r);
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


/******************************************/
// ColorWidget Implementation
/******************************************/


ColorWidget::ColorWidget(QWidget *p, RichColor* newColor)
    :MeshLabWidget(p,newColor),pickcol()
{
    colorLabel = new QLabel(this);
    descLabel = new QLabel(rp->pd->fieldDesc,this);
    colorButton = new QPushButton(this);
    colorButton->setAutoFillBackground(true);
    colorButton->setFlat(true);
    colorButton->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    //const QColor cl = rp->pd->defVal->getColor();
    //resetWidgetValue();
    initWidgetValue();
    //int row = gridLay->rowCount() - 1;
    //gridLay->addWidget(descLabel,row,0,Qt::AlignTop);

    vlay = new QHBoxLayout();
    QFontMetrics met(colorLabel->font());
    QColor black(Qt::black);
    QString blackname = "(" + black.name() + ")";
    QSize sz = met.size(Qt::TextSingleLine,blackname);
    colorLabel->setMaximumWidth(sz.width());
    colorLabel->setMinimumWidth(sz.width());
    vlay->addWidget(colorLabel,0,Qt::AlignRight);
    vlay->addWidget(colorButton);


    //gridLay->addLayout(lay,row,1,Qt::AlignTop);
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
    pickcol =QColorDialog::getColor(pickcol,this->parentWidget(),"Pick a Color",QColorDialog::DontUseNativeDialog|QColorDialog::ShowAlphaChannel);
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

void ColorWidget::addWidgetToGridLayout( QGridLayout* lay,const int r )
{
    if (lay != NULL)
    {
        lay->addWidget(descLabel,r,0);
        lay->addLayout(vlay,r,1);
    }
    MeshLabWidget::addWidgetToGridLayout(lay,r);
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

void RichParameterToQTableWidgetItemConstructor::visit( RichShotf& /*pd*/ )
{
    assert(0); ///
    lastCreated = new QTableWidgetItem(QString("TODO")/*,lst*/);
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
	if (rpar != NULL)
		fl = rpar->val->getFileName();
	filename = new QLineEdit(this);
    filename->setText(tr(""));
    browse = new QPushButton(this);
    descLab = new QLabel(rp->pd->fieldDesc,this);
    browse->setText("...");
    //const QColor cl = rp->pd->defVal->getColor();
    //resetWidgetValue();
    //int row = gridLay->rowCount() - 1;
    //gridLay->addWidget(descLab,row,0,Qt::AlignTop);
    hlay = new QHBoxLayout();
    hlay->addWidget(filename,2);
    hlay->addWidget(browse);

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

void IOFileWidget::addWidgetToGridLayout( QGridLayout* lay,const int r )
{
    if (lay != NULL)
    {
        lay->addWidget(descLab,r,0,Qt::AlignTop);
        lay->addLayout(hlay,r,1,Qt::AlignTop);
    }
    MeshLabWidget::addWidgetToGridLayout(lay,r);
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
	FileValue fileName(fl);
	rp->pd->defVal->set(fileName);
    emit dialogParamChanged();
}


OpenFileWidget::~OpenFileWidget()
{
}


SaveFileWidget::SaveFileWidget( QWidget* p,RichSaveFile* rpar )
    :IOFileWidget(p,rpar)
{
    filename->setText(rpar->val->getFileName());
	QString tmp = rp->val->getFileName();
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
	FileValue fileName(fl);
	rp->pd->defVal->set(fileName);
    emit dialogParamChanged();
}
