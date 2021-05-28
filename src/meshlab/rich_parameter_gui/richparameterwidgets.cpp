/****************************************************************************
* MeshLab                                                           o o     *
* Visual and Computer Graphics Library                            o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004-2020                                           \/)\/    *
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

#include "richparameterwidgets.h"

#include <QColorDialog>
#include <QClipboard>
#include <QFileDialog>
#include <QApplication>
#include <common/ml_document/mesh_document.h>

/******************************************/
// MeshLabWidget Implementation
/******************************************/
RichParameterWidget::RichParameterWidget(QWidget* p, const RichParameter& rpar , const RichParameter& defaultParam):
	QWidget(p) , rp(rpar.clone()), defp(defaultParam.clone())
{
	if (rp!= NULL) {
		helpLab = new QLabel("<small>"+rpar.toolTip() +"</small>",this);
		helpLab->setTextFormat(Qt::RichText);
		helpLab->setWordWrap(true);
		helpLab->setVisible(false);
		helpLab->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
		helpLab->setMinimumWidth(250);
		helpLab->setMaximumWidth(QWIDGETSIZE_MAX);
	}
}

RichParameterWidget::~RichParameterWidget()
{
	delete rp;
	delete defp;
	delete helpLab;
}

void RichParameterWidget::resetValue()
{
	rp->setValue(defp->value());
	resetWidgetValue();
}

void RichParameterWidget::setValue(const Value& v)
{
	rp->setValue(v);
	resetWidgetValue();
}



const Value& RichParameterWidget::widgetValue()
{
	collectWidgetValue();
	return rp->value();
}

const RichParameter& RichParameterWidget::richParameter() const
{
	return *rp;
}

QString RichParameterWidget::parameterName() const
{
	return rp->name();
}


void RichParameterWidget::addWidgetToGridLayout( QGridLayout* lay, const int r)
{
	if (lay != NULL) {
		lay->addWidget(helpLab, r, 2, 1, 1, Qt::AlignLeft);
	}
}

/******************************************/
// BoolWidget Implementation
/******************************************/

BoolWidget::BoolWidget(QWidget* p, const RichBool& rb , const RichBool& rdef) :
	RichParameterWidget(p,rb, rdef)
{
	cb = new QCheckBox(rp->fieldDescription(),this);
	cb->setToolTip(rp->toolTip());
	cb->setChecked(rp->value().getBool());

	connect(cb,SIGNAL(stateChanged(int)),p,SIGNAL(parameterChanged()));
}

BoolWidget::~BoolWidget()
{
}

void BoolWidget::collectWidgetValue()
{
	rp->setValue(BoolValue(cb->isChecked()));
}

void BoolWidget::resetWidgetValue()
{
	cb->setChecked(rp->value().getBool());
}

void BoolWidget::setWidgetValue( const Value& nv )
{
	cb->setChecked(nv.getBool());
}

void BoolWidget::addWidgetToGridLayout(QGridLayout* lay,const int r)
{
	if (lay !=NULL)
		lay->addWidget(cb,r,0,1,2);
	RichParameterWidget::addWidgetToGridLayout(lay,r);
}

/******************************************/
// LineEditWidget Implementation
/******************************************/

LineEditWidget::LineEditWidget(QWidget* p, const RichParameter& rpar , const RichParameter& rdef) :
	RichParameterWidget(p,rpar, rdef)
{
	lab = new QLabel(rp->fieldDescription(),this);
	lned = new QLineEdit(this);

	lab->setToolTip(rp->toolTip());
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
	if(lned->text() != this->lastVal) {
		this->lastVal = lned->text();
		if(!this->lastVal.isEmpty())
			emit lineEditChanged();
	}
}

void LineEditWidget::addWidgetToGridLayout( QGridLayout* lay,const int r )
{
	if (lay !=NULL) {
		lay->addWidget(lab,r,0);
		lay->addWidget(lned,r,1);
	}
	RichParameterWidget::addWidgetToGridLayout(lay,r);
}

/******************************************/
// IntWidget Implementation
/******************************************/

IntWidget::IntWidget( QWidget* p, const RichInt& rpar, const RichInt& rdef ) :
	LineEditWidget(p,rpar, rdef)
{
	lned->setText(QString::number(rp->value().getInt()));
}

void IntWidget::collectWidgetValue()
{
	rp->setValue(IntValue(lned->text().toInt()));
}

void IntWidget::resetWidgetValue()
{
	lned->setText(QString::number(rp->value().getInt()));
}

void IntWidget::setWidgetValue( const Value& nv )
{
	lned->setText(QString::number(nv.getInt()));
}

/******************************************/
// FloatWidget Implementation
/******************************************/

FloatWidget::FloatWidget(QWidget* p, const RichFloat& rpar , const RichFloat& rdef) :
	LineEditWidget(p,rpar, rdef)
{
	lned->setText(QString::number(rp->value().getFloat(),'g',3));
}

void FloatWidget::collectWidgetValue()
{
	rp->setValue(FloatValue(lned->text().toFloat()));
}

void FloatWidget::resetWidgetValue()
{
	lned->setText(QString::number(rp->value().getFloat(),'g',3));
}

void FloatWidget::setWidgetValue( const Value& nv )
{
	lned->setText(QString::number(nv.getFloat(),'g',3));
}

/******************************************/
// StringWidget Implementation
/******************************************/

StringWidget::StringWidget(QWidget* p, const RichString& rpar , const RichString& rdef) :
	LineEditWidget(p,rpar, rdef)
{
	lned->setText(rp->value().getString());
}

void StringWidget::collectWidgetValue()
{
	rp->setValue(StringValue(lned->text()));
}

void StringWidget::resetWidgetValue()
{
	lned->setText(rp->value().getString());
}

void StringWidget::setWidgetValue( const Value& nv )
{
	lned->setText(nv.getString());
}

/******************************************/
// ColorWidget Implementation
/******************************************/


ColorWidget::ColorWidget(QWidget *p, const RichColor& newColor, const RichColor& rdef)
	:RichParameterWidget(p,newColor, rdef),pickcol()
{
	colorLabel = new QLabel(this);
	descLabel = new QLabel(rp->fieldDescription(),this);
	colorButton = new QPushButton(this);
	colorButton->setAutoFillBackground(true);
	colorButton->setFlat(true);
	colorButton->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
	//const QColor cl = rp->pd->defvalue().getColor();
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
	pickcol = rp->value().getColor();
	connect(colorButton,SIGNAL(clicked()),this,SLOT(pickColor()));
	connect(this,SIGNAL(dialogParamChanged()),p,SIGNAL(parameterChanged()));
}

ColorWidget::~ColorWidget()
{
	delete colorButton;
	delete colorLabel;
	delete descLabel;
}

void ColorWidget::addWidgetToGridLayout( QGridLayout* lay,const int r )
{
	if (lay != NULL)
	{
		lay->addWidget(descLabel,r,0);
		lay->addLayout(vlay,r,1);
	}
	RichParameterWidget::addWidgetToGridLayout(lay,r);
}

void ColorWidget::collectWidgetValue()
{
	rp->setValue(ColorValue(pickcol));
}

void ColorWidget::resetWidgetValue()
{
	QColor cl = rp->value().getColor();
	pickcol = cl;
	updateColorInfo(cl);
}

void ColorWidget::setWidgetValue( const Value& nv )
{
	QColor cl = nv.getColor();
	pickcol = cl;
	updateColorInfo(cl);
}

void ColorWidget::initWidgetValue()
{
	QColor cl = rp->value().getColor();
	pickcol = cl;
	updateColorInfo(cl);
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
	pickcol = QColorDialog::getColor(pickcol,this->parentWidget(),"Pick a Color",QColorDialog::DontUseNativeDialog|QColorDialog::ShowAlphaChannel);
	if(pickcol.isValid())
	{
		collectWidgetValue();
		updateColorInfo(ColorValue(pickcol));
	}
	emit dialogParamChanged();
}

/******************************************/
// AbsPercWidget Implementation
/******************************************/


AbsPercWidget::AbsPercWidget(QWidget *p, const RichAbsPerc& rabs, const RichAbsPerc& rdef):
	RichParameterWidget(p,rabs, rdef)

{
	m_min = rabs.min;
	m_max = rabs.max;

	fieldDesc = new QLabel(rp->fieldDescription() + " (abs and %)",this);
	fieldDesc->setToolTip(rp->toolTip());
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
	float initVal = rp->value().getAbsPerc();
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

void AbsPercWidget::setValue(float val, float minV, float maxV)
{
	assert(absSB);
	absSB->setValue(val);
	m_min=minV;
	m_max=maxV;
}

void AbsPercWidget::collectWidgetValue()
{
	rp->setValue(AbsPercValue(float(absSB->value())));
}

void AbsPercWidget::resetWidgetValue()
{
	//const AbsPercDecoration* absd = reinterpret_cast<const AbsPercDecoration*>(&(rp->pd));
	RichAbsPerc* ap = reinterpret_cast<RichAbsPerc*>(rp);
	setValue(rp->value().getAbsPerc(),ap->min,ap->max);
}

void AbsPercWidget::setWidgetValue( const Value& nv )
{
	//const AbsPercDecoration* absd = reinterpret_cast<const AbsPercDecoration*>(&(rp->pd));
	RichAbsPerc* ap = reinterpret_cast<RichAbsPerc*>(rp);
	setValue(nv.getAbsPerc(),ap->min,ap->max);
}

void AbsPercWidget::addWidgetToGridLayout( QGridLayout* lay,const int r )
{
	if (lay != NULL)
	{
		lay->addWidget(fieldDesc,r,0,Qt::AlignLeft);
		lay->addLayout(vlay,r,1,Qt::AlignTop);
	}
	RichParameterWidget::addWidgetToGridLayout(lay,r);
}

/******************************************/
// Point3fWidget Implementation
/******************************************/

Point3fWidget::Point3fWidget(QWidget *p, const RichPoint3f& rpf, const RichPoint3f& rdef, QWidget *gla_curr):
	RichParameterWidget(p,rpf, rdef)
{
	//qDebug("Creating a Point3fWidget");
	paramName = rpf.name();
	//int row = gridLay->rowCount() - 1;
	descLab = new QLabel(rpf.fieldDescription(),this);
	descLab->setToolTip(rpf.fieldDescription());
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
	this->setValue(paramName,rp->value().getPoint3f());
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

Point3fWidget::~Point3fWidget()
{
	//qDebug("Deallocating a point3fwidget");
	this->disconnect();
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

void Point3fWidget::setValue(QString name,Point3m newVal)
{
	//qDebug("setValue parametername: %s ", qUtf8Printable(name));
	if(name==paramName)
	{
		for(int i =0;i<3;++i)
			coordSB[i]->setText(QString::number(newVal[i],'g',4));
	}
}

void Point3fWidget::setShotValue(QString name, Shotm newValShot)
{
	vcg::Point3f p = newValShot.GetViewPoint();
	setValue(name,p);
}

vcg::Point3f Point3fWidget::getValue()
{
	return vcg::Point3f(coordSB[0]->text().toFloat(),coordSB[1]->text().toFloat(),coordSB[2]->text().toFloat());
}

void Point3fWidget::collectWidgetValue()
{
	rp->setValue(Point3fValue(vcg::Point3f(coordSB[0]->text().toFloat(),coordSB[1]->text().toFloat(),coordSB[2]->text().toFloat())));
}

void Point3fWidget::resetWidgetValue()
{
	for(unsigned int ii = 0; ii < 3;++ii)
		coordSB[ii]->setText(QString::number(rp->value().getPoint3f()[ii],'g',3));
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
	RichParameterWidget::addWidgetToGridLayout(lay,r);
}

/******************************************/
// Matrix44fWidget Implementation
/******************************************/

Matrix44fWidget::Matrix44fWidget(QWidget *p, const RichMatrix44f& rpf, const RichMatrix44f& rdef, QWidget *gla_curr):
	RichParameterWidget(p,rpf, rdef)
{
	valid = false;
	m.SetIdentity();
	paramName = rpf.name();
	//int row = gridLay->rowCount() - 1;

	descLab = new QLabel(rpf.fieldDescription(),this);
	descLab->setToolTip(rpf.fieldDescription());
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
	this->setValue(paramName,rp->value().getMatrix44f());

	QLabel* headerL = new QLabel("Matrix:", this);
	vlay->addWidget(headerL, 0, Qt::AlignTop);

	vlay->addLayout(lay44);

	QPushButton* getMatrixButton = new QPushButton("Read from current layer");
	vlay->addWidget(getMatrixButton);

	QPushButton* pasteMatrixButton = new QPushButton("Paste from clipboard");
	vlay->addWidget(pasteMatrixButton);

	//gridLay->addLayout(vlay,row,1,Qt::AlignTop);

	connect(gla_curr,SIGNAL(transmitMatrix(QString,Matrix44m)),this,SLOT(setValue(QString,Matrix44m)));
	connect(getMatrixButton,SIGNAL(clicked()),this,SLOT(getMatrix()));
	connect(pasteMatrixButton,SIGNAL(clicked()),this,SLOT(pasteMatrix()));
	connect(this,SIGNAL(askMeshMatrix(QString)),  gla_curr,SLOT(sendMeshMatrix(QString)));

}

Matrix44fWidget::~Matrix44fWidget()
{
}

void Matrix44fWidget::setValue(QString name, Matrix44m newVal)
{
	if(name==paramName)
	{
		for(int i =0;i<16;++i)
			coordSB[i]->setText(QString::number(newVal[i/4][i%4],'g',4));
		valid = true;
		m = newVal;
	}
}


Matrix44m Matrix44fWidget::getValue()
{
	if (!valid) {
		Scalarm val[16];
		for (unsigned int i = 0; i < 16; ++i)
			val[i] = coordSB[i]->text().toFloat();
		return Matrix44m(val);
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
	if (shotString.contains(' ')) {
		QStringList list1 = shotString.split(" ");
		if (list1.size() != 16)
			return;
		valid = false;
		int id = 0;
		for (QStringList::iterator i = list1.begin(); i != list1.end(); ++i, ++id) {
			bool ok = true;
			(*i).toFloat(&ok);
			if (!ok)
				return;
		}
		id = 0;
		for (QStringList::iterator i = list1.begin(); i != list1.end(); ++i, ++id)
			coordSB[id]->setText(*i);
	}
	else {
		QByteArray value = QByteArray::fromBase64(shotString.toLocal8Bit());
		memcpy(m.V(), value.data(), sizeof(Matrix44m::ScalarType) * 16);
		int id = 0;
		for (int i = 0; i < 16; ++i, ++id)
			coordSB[id]->setText(QString::number(m.V()[i]));
	}
}

void Matrix44fWidget::collectWidgetValue()
{
	if (!valid) {
		Matrix44m  tempM;
		for (unsigned int i = 0; i < 16; ++i) tempM[i / 4][i % 4] = coordSB[i]->text().toFloat();
		rp->setValue(Matrix44fValue(tempM));
	}
	else
		rp->setValue(Matrix44fValue(m));
}

void Matrix44fWidget::resetWidgetValue()
{
	valid = false;
	vcg::Matrix44f  m; m.SetIdentity();
	for(unsigned int ii = 0; ii < 16;++ii)
		coordSB[ii]->setText(QString::number(rp->value().getMatrix44f()[ii/4][ii%4],'g',3));
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
	RichParameterWidget::addWidgetToGridLayout(lay,r);
}

void Matrix44fWidget::invalidateMatrix(const QString& /*s*/)
{
	valid = false;
}

/******************************************/
// ShotfWidget Implementation
/******************************************/

ShotfWidget::ShotfWidget(QWidget *p, const RichShotf& rpf, const RichShotf& rdef, QWidget *gla_curr):
	RichParameterWidget(p,rpf, rdef)
{

	paramName = rpf.name();
	//int row = gridLay->rowCount() - 1;

	descLab = new QLabel(rpf.fieldDescription(),p);
	descLab->setToolTip(rpf.fieldDescription());
	//gridLay->addWidget(descLab,row,0,Qt::AlignTop);

	hlay = new QHBoxLayout();


	this->setShotValue(paramName,rp->value().getShotf());
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

		}
		break;
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

Shotm ShotfWidget::getValue()
{
	return curShot;
}

void ShotfWidget::collectWidgetValue()
{
	rp->setValue(ShotfValue(curShot));
}

void ShotfWidget::resetWidgetValue()
{
	curShot = rp->value().getShotf();
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
	RichParameterWidget::addWidgetToGridLayout(lay,r);
}

/******************************************/
// DynamicFloatWidget Implementation
/******************************************/

DynamicFloatWidget::DynamicFloatWidget(QWidget *p, const RichDynamicFloat& rdf, const RichDynamicFloat& rdef):
	RichParameterWidget(p,rdf, rdef)
{
	int numbdecimaldigit = 4;
	minVal = rdf.min;
	maxVal = rdf.max;
	valueLE = new QLineEdit(this);
	valueLE->setAlignment(Qt::AlignRight);

	valueSlider = new QSlider(Qt::Horizontal,this);
	valueSlider->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
	fieldDesc = new QLabel(rp->fieldDescription(),this);
	valueSlider->setMinimum(0);
	valueSlider->setMaximum(100);
	valueSlider->setValue(floatToInt(rp->value().getFloat()));
	RichDynamicFloat* dfd = reinterpret_cast<RichDynamicFloat*>(rp);
	//const DynamicFloatDecoration* dfd = reinterpret_cast<const DynamicFloatDecoration*>(&(rp->pd));
	QFontMetrics fm(valueLE->font());
	QSize sz = fm.size(Qt::TextSingleLine,QString::number(0));
	valueLE->setValidator(new QDoubleValidator (dfd->min,dfd->max, numbdecimaldigit, valueLE));
	valueLE->setText(QString::number(rp->value().getFloat()));
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
	rp->setValue(DynamicFloatValue(valueLE->text().toFloat()));
}

void DynamicFloatWidget::resetWidgetValue()
{
	valueLE->setText(QString::number(rp->value().getFloat()));
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
	RichParameterWidget::addWidgetToGridLayout(lay,r);
}

/******************************************/
// ComboWidget Implementation
/******************************************/

ComboWidget::ComboWidget(QWidget *p, const RichParameter& rpar, const RichParameter& rdef) :
	RichParameterWidget(p,rpar, rdef) {
}

void ComboWidget::Init(QWidget *p,int defaultEnum, QStringList values)
{
	enumLabel = new QLabel(this);
	enumLabel->setText(rp->fieldDescription());
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
	RichParameterWidget::addWidgetToGridLayout(lay,r);
}

/******************************************/
//EnumWidget Implementation
/******************************************/

EnumWidget::EnumWidget(QWidget *p, const RichEnum& rpar, const RichEnum& rdef) :
	ComboWidget(p,rpar, rdef)
{
	//you MUST call it!!!!
	Init(p,rpar.value().getEnum(),rpar.enumvalues);
	//assert(enumCombo != NULL);
}

int EnumWidget::getSize()
{
	return enumCombo->count();
}

void EnumWidget::collectWidgetValue()
{
	rp->setValue(EnumValue(enumCombo->currentIndex()));
}

void EnumWidget::resetWidgetValue()
{
	//lned->setText(QString::number(rp->value().getFloat(),'g',3));
	enumCombo->setCurrentIndex(rp->value().getEnum());
}

void EnumWidget::setWidgetValue( const Value& nv )
{
	enumCombo->setCurrentIndex(nv.getEnum());
}

/******************************************/
//MeshWidget Implementation
/******************************************/

MeshWidget::MeshWidget(QWidget *p, const RichMesh& rpar, const RichMesh& rdef) :
	ComboWidget(p,rpar, rdef)
{
	md=((RichMesh*)rp)->meshdoc;

	QStringList meshNames;

	//make the default mesh Index be 0
	//defaultMeshIndex = -1;

	int currentmeshindex = -1;
	unsigned int i = 0;
	for(MeshModel* mm : md->meshList) {
		QString shortName = mm->label();
		meshNames.push_back(shortName);
		if((unsigned int) mm->id() == rp->value().getMeshId()) {
			currentmeshindex = i;
		}
		++i;
	}

	Init(p,currentmeshindex,meshNames);
}

MeshModel * MeshWidget::getMesh()
{
	//test to make sure index is in bounds
	unsigned int index = enumCombo->currentIndex();
	if(index < md->meshList.size() && index > -1) {
		auto it = md->meshList.begin();
		std::advance(it, index);
		return *it;
	}
	else return nullptr;
}

void MeshWidget::setMesh(MeshModel * newMesh)
{
	unsigned int i = 0;
	for(MeshModel* mm : md->meshList) {
		if(mm == newMesh)
			setIndex(i);
		i++;
	}
}

void MeshWidget::collectWidgetValue()
{
	auto it = md->meshList.begin();
	std::advance(it, enumCombo->currentIndex());
	rp->setValue(MeshValue((*it)->id()));
}

void MeshWidget::resetWidgetValue()
{
	int meshindex = -1;
	unsigned int i = 0;
	for(MeshModel* mm : md->meshList) {
		if(mm->id() == rp->value().getMeshId()) {
			meshindex = i;
		}
		++i;
	}
	enumCombo->setCurrentIndex(meshindex);
}

void MeshWidget::setWidgetValue( const Value& nv )
{
	int meshindex = -1;
	unsigned int i = 0;
	for(MeshModel* mm : md->meshList) {
		if(mm->id() == nv.getMeshId()) {
			meshindex = i;
		}
		++i;
	}
	enumCombo->setCurrentIndex(meshindex);
}

/******************************************/
//IOFileWidget Implementation
/******************************************/

IOFileWidget::IOFileWidget(QWidget* p, const RichParameter& rpar , const RichParameter& rdef) :
	RichParameterWidget(p,rpar, rdef)
{
	filename = new QLineEdit(this);
	filename->setText(tr(""));
	browse = new QPushButton(this);
	descLab = new QLabel(rp->fieldDescription(),this);
	browse->setText("...");
	//const QColor cl = rp->pd->defvalue().getColor();
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
	rp->setValue(FileValue(filename->text()));
}

void IOFileWidget::resetWidgetValue()
{
	QString fle = rp->value().getFileName();
	updateFileName(fle);
}

void IOFileWidget::setWidgetValue(const Value& nv)
{
	QString fle = nv.getFileName();
	updateFileName(fle);
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
	RichParameterWidget::addWidgetToGridLayout(lay,r);
}

/******************************************/
//SaveFileWidget Implementation
/******************************************/

SaveFileWidget::SaveFileWidget(QWidget* p, const RichSaveFile& rpar , const RichSaveFile& rdef) :
	IOFileWidget(p,rpar, rdef)
{
	filename->setText(rp->value().getFileName());
	QString tmp = rp->value().getFileName();
}

SaveFileWidget::~SaveFileWidget()
{
}

void SaveFileWidget::selectFile()
{
	//SaveFileDecoration* dec = reinterpret_cast<SaveFileDecoration*>(rp->pd);
	RichSaveFile* dec = reinterpret_cast<RichSaveFile*>(rp);
	QString ext;
	QString fl = QFileDialog::getSaveFileName(this,tr("Save"),rp->value().getFileName(),dec->ext);
	collectWidgetValue();
	updateFileName(fl);
	FileValue fileName(fl);
	rp->setValue(fileName);
	emit dialogParamChanged();
}

/******************************************/
//OpenFileWidget Implementation
/******************************************/

OpenFileWidget::OpenFileWidget(QWidget *p, const RichOpenFile& rdf, const RichOpenFile& rdef) :
	IOFileWidget(p,rdf, rdef)
{
}

void OpenFileWidget::selectFile()
{
	//OpenFileDecoration* dec = reinterpret_cast<OpenFileDecoration*>(rp->pd);
	RichOpenFile* dec = reinterpret_cast<RichOpenFile*>(rp);
	QString ext;
	QString fl = QFileDialog::getOpenFileName(this,tr("Open"),rp->value().getFileName(), dec->exts.join(" "));
	collectWidgetValue();
	updateFileName(fl);
	FileValue fileName(fl);
	rp->setValue(fileName);
	emit dialogParamChanged();
}


OpenFileWidget::~OpenFileWidget()
{
}
