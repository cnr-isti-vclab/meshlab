/*****************************************************************************
 * MeshLab                                                           o o     *
 * Visual and Computer Graphics Library                            o     o   *
 *                                                                _   O  _   *
 * Copyright(C) 2004-2021                                           \/)\/    *
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

#include <QApplication>
#include <QClipboard>
#include <QColorDialog>
#include <QFileDialog>
#include <common/ml_document/mesh_document.h>

/******************************************/
// MeshLabWidget Implementation
/******************************************/
RichParameterWidget::RichParameterWidget(
	QWidget*             p,
	const RichParameter& rpar,
	const RichParameter& defaultParam) :
		QWidget(p), parameter(rpar.clone()), defaultParameter(defaultParam.clone())
{
	if (parameter != nullptr) {
		descriptionLabel = new QLabel(parameter->fieldDescription(), this);
		descriptionLabel->setToolTip(parameter->toolTip());
		descriptionLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

		helpLabel = new QLabel("<small>" + rpar.toolTip() + "</small>", this);
		helpLabel->setTextFormat(Qt::RichText);
		helpLabel->setWordWrap(true);
		helpLabel->setVisible(false);
		helpLabel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
		helpLabel->setMinimumWidth(250);
	}
}

RichParameterWidget::~RichParameterWidget()
{
	delete parameter;
	delete defaultParameter;
}

void RichParameterWidget::resetValue()
{
	parameter->setValue(defaultParameter->value());
	resetWidgetValue();
}

void RichParameterWidget::setValue(const Value& v)
{
	parameter->setValue(v);
	resetWidgetValue();
}

void RichParameterWidget::setHelpVisible(bool b)
{
	helpLabel->setVisible(b);
}

const Value& RichParameterWidget::widgetValue()
{
	collectWidgetValue();
	return parameter->value();
}

const RichParameter& RichParameterWidget::richParameter() const
{
	return *parameter;
}

QString RichParameterWidget::parameterName() const
{
	return parameter->name();
}

void RichParameterWidget::addWidgetToGridLayout(QGridLayout* lay, const int r)
{
	if (lay != NULL) {
		lay->addWidget(descriptionLabel, r, 0);
		lay->addWidget(helpLabel, r, 2);
	}
}

/******************************************/
// BoolWidget Implementation
/******************************************/

BoolWidget::BoolWidget(QWidget* p, const RichBool& rb, const RichBool& rdef) :
		RichParameterWidget(p, rb, rdef)
{
	cb = new QCheckBox("", this);
	cb->setToolTip(parameter->toolTip());
	cb->setChecked(parameter->value().getBool());

	connect(cb, SIGNAL(stateChanged(int)), p, SIGNAL(parameterChanged()));
}

BoolWidget::~BoolWidget()
{
}

void BoolWidget::collectWidgetValue()
{
	parameter->setValue(BoolValue(cb->isChecked()));
}

void BoolWidget::resetWidgetValue()
{
	cb->setChecked(parameter->value().getBool());
}

void BoolWidget::setWidgetValue(const Value& nv)
{
	cb->setChecked(nv.getBool());
}

void BoolWidget::addWidgetToGridLayout(QGridLayout* lay, const int r)
{
	if (lay != NULL) {
		lay->addWidget(cb, r, 1);
	}
	RichParameterWidget::addWidgetToGridLayout(lay, r);
}

/******************************************/
// LineEditWidget Implementation
/******************************************/

LineEditWidget::LineEditWidget(QWidget* p, const RichParameter& rpar, const RichParameter& rdef) :
		RichParameterWidget(p, rpar, rdef)
{
	lned = new QLineEdit(this);

	connect(lned, SIGNAL(editingFinished()), this, SLOT(changeChecker()));
	connect(this, SIGNAL(lineEditChanged()), p, SIGNAL(parameterChanged()));
	lned->setAlignment(Qt::AlignLeft);
}

LineEditWidget::~LineEditWidget()
{
}

void LineEditWidget::changeChecker()
{
	if (lned->text() != this->lastVal) {
		this->lastVal = lned->text();
		if (!this->lastVal.isEmpty())
			emit lineEditChanged();
	}
}

void LineEditWidget::addWidgetToGridLayout(QGridLayout* lay, const int r)
{
	if (lay != NULL) {
		lay->addWidget(lned, r, 1);
	}
	RichParameterWidget::addWidgetToGridLayout(lay, r);
}

/******************************************/
// IntWidget Implementation
/******************************************/

IntWidget::IntWidget(QWidget* p, const RichInt& rpar, const RichInt& rdef) :
		LineEditWidget(p, rpar, rdef)
{
	lned->setText(QString::number(parameter->value().getInt()));
}

void IntWidget::collectWidgetValue()
{
	parameter->setValue(IntValue(lned->text().toInt()));
}

void IntWidget::resetWidgetValue()
{
	lned->setText(QString::number(parameter->value().getInt()));
}

void IntWidget::setWidgetValue(const Value& nv)
{
	lned->setText(QString::number(nv.getInt()));
}

/******************************************/
// FloatWidget Implementation
/******************************************/

FloatWidget::FloatWidget(QWidget* p, const RichFloat& rpar, const RichFloat& rdef) :
		LineEditWidget(p, rpar, rdef)
{
	lned->setText(QString::number(parameter->value().getFloat(), 'g', 3));
}

void FloatWidget::collectWidgetValue()
{
	parameter->setValue(FloatValue(lned->text().toFloat()));
}

void FloatWidget::resetWidgetValue()
{
	lned->setText(QString::number(parameter->value().getFloat(), 'g', 3));
}

void FloatWidget::setWidgetValue(const Value& nv)
{
	lned->setText(QString::number(nv.getFloat(), 'g', 3));
}

/******************************************/
// StringWidget Implementation
/******************************************/

StringWidget::StringWidget(QWidget* p, const RichString& rpar, const RichString& rdef) :
		LineEditWidget(p, rpar, rdef)
{
	lned->setText(parameter->value().getString());
}

void StringWidget::collectWidgetValue()
{
	parameter->setValue(StringValue(lned->text()));
}

void StringWidget::resetWidgetValue()
{
	lned->setText(parameter->value().getString());
}

void StringWidget::setWidgetValue(const Value& nv)
{
	lned->setText(nv.getString());
}

/******************************************/
// ColorWidget Implementation
/******************************************/

ColorWidget::ColorWidget(QWidget* p, const RichColor& newColor, const RichColor& rdef) :
		RichParameterWidget(p, newColor, rdef), pickcol()
{
	colorLabel  = new QLabel(this);
	colorButton = new QPushButton(this);
	colorButton->setAutoFillBackground(true);
	colorButton->setFlat(true);
	colorButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	initWidgetValue();

	vlay = new QHBoxLayout();
	QFontMetrics met(colorLabel->font());
	QColor       black(Qt::black);
	QString      blackname = "(" + black.name() + ")";
	QSize        sz        = met.size(Qt::TextSingleLine, blackname);
	colorLabel->setMaximumWidth(sz.width());
	colorLabel->setMinimumWidth(sz.width());
	vlay->addWidget(colorLabel, 0, Qt::AlignRight);
	vlay->addWidget(colorButton);

	pickcol = parameter->value().getColor();
	connect(colorButton, SIGNAL(clicked()), this, SLOT(pickColor()));
	connect(this, SIGNAL(dialogParamChanged()), p, SIGNAL(parameterChanged()));
}

ColorWidget::~ColorWidget()
{
	delete colorButton;
	delete colorLabel;
}

void ColorWidget::addWidgetToGridLayout(QGridLayout* lay, const int r)
{
	if (lay != NULL) {
		lay->addLayout(vlay, r, 1);
	}
	RichParameterWidget::addWidgetToGridLayout(lay, r);
}

void ColorWidget::collectWidgetValue()
{
	parameter->setValue(ColorValue(pickcol));
}

void ColorWidget::resetWidgetValue()
{
	QColor cl = parameter->value().getColor();
	pickcol   = cl;
	updateColorInfo(cl);
}

void ColorWidget::setWidgetValue(const Value& nv)
{
	QColor cl = nv.getColor();
	pickcol   = cl;
	updateColorInfo(cl);
}

void ColorWidget::initWidgetValue()
{
	QColor cl = parameter->value().getColor();
	pickcol   = cl;
	updateColorInfo(cl);
}

void ColorWidget::updateColorInfo(const ColorValue& newColor)
{
	QColor col = newColor.getColor();
	colorLabel->setText("(" + col.name() + ")");
	QPalette palette(col);
	colorButton->setPalette(palette);
}

void ColorWidget::pickColor()
{
	pickcol = QColorDialog::getColor(
		pickcol,
		this->parentWidget(),
		"Pick a Color",
		QColorDialog::DontUseNativeDialog | QColorDialog::ShowAlphaChannel);
	if (pickcol.isValid()) {
		collectWidgetValue();
		updateColorInfo(ColorValue(pickcol));
	}
	emit dialogParamChanged();
}

/******************************************/
// AbsPercWidget Implementation
/******************************************/

AbsPercWidget::AbsPercWidget(QWidget* p, const RichAbsPerc& rabs, const RichAbsPerc& rdef) :
		RichParameterWidget(p, rabs, rdef)

{
	m_min = rabs.min;
	m_max = rabs.max;

	descriptionLabel->setText(descriptionLabel->text() + " (abs and %)");

	absSB  = new QDoubleSpinBox(this);
	percSB = new QDoubleSpinBox(this);

	absSB->setMinimum(m_min - (m_max - m_min));
	absSB->setMaximum(m_max * 2);
	absSB->setAlignment(Qt::AlignRight);

	int decimals = 7 - ceil(log10(fabs(m_max - m_min)));
	absSB->setDecimals(decimals);
	absSB->setSingleStep((m_max - m_min) / 100.0);
	float initVal = parameter->value().getFloat();
	absSB->setValue(initVal);
	absSB->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

	percSB->setMinimum(-200);
	percSB->setMaximum(200);
	percSB->setAlignment(Qt::AlignRight);
	percSB->setSingleStep(0.5);
	percSB->setValue((100 * (initVal - m_min)) / (m_max - m_min));
	percSB->setDecimals(3);
	percSB->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	QLabel* absLab  = new QLabel("<i> <small> world unit</small></i>");
	QLabel* percLab = new QLabel(
		"<i> <small> perc on" + QString("(%1 .. %2)").arg(m_min).arg(m_max) + "</small></i>");

	vlay = new QGridLayout();
	vlay->addWidget(absLab, 0, 0, Qt::AlignHCenter);
	vlay->addWidget(percLab, 0, 1, Qt::AlignHCenter);

	vlay->addWidget(absSB, 1, 0, Qt::AlignTop);
	vlay->addWidget(percSB, 1, 1, Qt::AlignTop);

	connect(absSB, SIGNAL(valueChanged(double)), this, SLOT(on_absSB_valueChanged(double)));
	connect(percSB, SIGNAL(valueChanged(double)), this, SLOT(on_percSB_valueChanged(double)));
	connect(this, SIGNAL(dialogParamChanged()), p, SIGNAL(parameterChanged()));
}

AbsPercWidget::~AbsPercWidget()
{
	delete absSB;
	delete percSB;
}

void AbsPercWidget::on_absSB_valueChanged(double newv)
{
	disconnect(percSB, SIGNAL(valueChanged(double)), this, SLOT(on_percSB_valueChanged(double)));
	percSB->setValue((100 * (newv - m_min)) / (m_max - m_min));
	connect(percSB, SIGNAL(valueChanged(double)), this, SLOT(on_percSB_valueChanged(double)));
	emit dialogParamChanged();
}

void AbsPercWidget::on_percSB_valueChanged(double newv)
{
	disconnect(absSB, SIGNAL(valueChanged(double)), this, SLOT(on_absSB_valueChanged(double)));
	absSB->setValue((m_max - m_min) * 0.01 * newv + m_min);
	connect(absSB, SIGNAL(valueChanged(double)), this, SLOT(on_absSB_valueChanged(double)));
	emit dialogParamChanged();
}

void AbsPercWidget::setValue(float val, float minV, float maxV)
{
	absSB->setValue(val);
	m_min = minV;
	m_max = maxV;
}

void AbsPercWidget::collectWidgetValue()
{
	parameter->setValue(FloatValue(absSB->value()));
}

void AbsPercWidget::resetWidgetValue()
{
	RichAbsPerc* ap = reinterpret_cast<RichAbsPerc*>(parameter);
	setValue(parameter->value().getFloat(), ap->min, ap->max);
}

void AbsPercWidget::setWidgetValue(const Value& nv)
{
	RichAbsPerc* ap = reinterpret_cast<RichAbsPerc*>(parameter);
	setValue(nv.getFloat(), ap->min, ap->max);
}

void AbsPercWidget::addWidgetToGridLayout(QGridLayout* lay, const int r)
{
	if (lay != NULL) {
		lay->addLayout(vlay, r, 1, Qt::AlignTop);
	}
	RichParameterWidget::addWidgetToGridLayout(lay, r);
}

/******************************************/
// PositionWidget Implementation
/******************************************/

PositionWidget::PositionWidget(
	QWidget*            p,
	const RichPosition& rpf,
	const RichPosition& rdef,
	QWidget*            gla_curr) :
		RichParameterWidget(p, rpf, rdef)
{
	paramName = rpf.name();

	vlay = new QHBoxLayout();
	vlay->setSpacing(0);
	for (int i = 0; i < 3; ++i) {
		coordSB[i]     = new QLineEdit(this);
		QFont baseFont = coordSB[i]->font();
		if (baseFont.pixelSize() != -1)
			baseFont.setPixelSize(baseFont.pixelSize() * 3 / 4);
		else
			baseFont.setPointSize(baseFont.pointSize() * 3 / 4);
		coordSB[i]->setFont(baseFont);
		coordSB[i]->setMinimumWidth(coordSB[i]->sizeHint().width() / 4);
		coordSB[i]->setValidator(new QDoubleValidator());
		coordSB[i]->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
		vlay->addWidget(coordSB[i]);
		connect(coordSB[i], SIGNAL(textChanged(QString)), p, SIGNAL(parameterChanged()));
	}
	this->setValue(paramName, parameter->value().getPoint3f());
	// if we have a connection to the current glarea we can setup the additional
	// button for getting the current view direction.
	if (gla_curr) {
		QStringList names;
		names << "View Pos.";
		names << "Surf. Pos.";
		names << "Raster Camera Pos.";
		names << "Trackball Center";

		getPoint3Combo = new QComboBox(this);
		getPoint3Combo->addItems(names);
		getPoint3Combo->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		vlay->addWidget(getPoint3Combo);

		connect(
			gla_curr,
			SIGNAL(transmitSurfacePos(QString, Point3m)),
			this,
			SLOT(setValue(QString, Point3m)));
		connect(
			gla_curr,
			SIGNAL(transmitCameraPos(QString, Point3m)),
			this,
			SLOT(setValue(QString, Point3m)));
		connect(
			gla_curr,
			SIGNAL(transmitShot(QString, Shotm)),
			this,
			SLOT(setShotValue(QString, Shotm)));
		connect(
			gla_curr,
			SIGNAL(transmitTrackballPos(QString, Point3m)),
			this,
			SLOT(setValue(QString, Point3m)));
		connect(this, SIGNAL(askViewPos(QString)), gla_curr, SLOT(sendViewerShot(QString)));
		connect(this, SIGNAL(askSurfacePos(QString)), gla_curr, SLOT(sendSurfacePos(QString)));
		connect(this, SIGNAL(askCameraPos(QString)), gla_curr, SLOT(sendRasterShot(QString)));
		connect(this, SIGNAL(askTrackballPos(QString)), gla_curr, SLOT(sendTrackballPos(QString)));

		getPoint3Button = new QPushButton("Get", this);
		getPoint3Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		connect(getPoint3Button, SIGNAL(clicked()), this, SLOT(getPoint()));

		vlay->addWidget(getPoint3Button);
	}
}

PositionWidget::~PositionWidget()
{
	this->disconnect();
}

void PositionWidget::getPoint()
{
	int index = getPoint3Combo->currentIndex();
	switch (index) {
	case 0: emit askViewPos(paramName); break;
	case 1: emit askSurfacePos(paramName); break;
	case 2: emit askCameraPos(paramName); break;
	case 3: emit askTrackballPos(paramName); break;
	default: assert(0);
	}
}

void PositionWidget::setValue(QString name, Point3m newVal)
{
	if (name == paramName) {
		for (int i = 0; i < 3; ++i)
			coordSB[i]->setText(QString::number(newVal[i], 'g', 4));
	}
}

void PositionWidget::setShotValue(QString name, Shotm newValShot)
{
	vcg::Point3f p = newValShot.GetViewPoint();
	setValue(name, p);
}

vcg::Point3f PositionWidget::getValue()
{
	return vcg::Point3f(
		coordSB[0]->text().toFloat(), coordSB[1]->text().toFloat(), coordSB[2]->text().toFloat());
}

void PositionWidget::collectWidgetValue()
{
	parameter->setValue(Point3fValue(vcg::Point3f(
		coordSB[0]->text().toFloat(), coordSB[1]->text().toFloat(), coordSB[2]->text().toFloat())));
}

void PositionWidget::resetWidgetValue()
{
	for (unsigned int ii = 0; ii < 3; ++ii)
		coordSB[ii]->setText(QString::number(parameter->value().getPoint3f()[ii], 'g', 3));
}

void PositionWidget::setWidgetValue(const Value& nv)
{
	for (unsigned int ii = 0; ii < 3; ++ii)
		coordSB[ii]->setText(QString::number(nv.getPoint3f()[ii], 'g', 3));
}

void PositionWidget::addWidgetToGridLayout(QGridLayout* lay, const int r)
{
	if (lay != NULL) {
		lay->addLayout(vlay, r, 1);
	}
	RichParameterWidget::addWidgetToGridLayout(lay, r);
}

/******************************************/
// DirectionWidget Implementation
/******************************************/

DirectionWidget::DirectionWidget(
	QWidget*             p,
	const RichDirection& rpf,
	const RichDirection& rdef,
	QWidget*             gla_curr) :
		RichParameterWidget(p, rpf, rdef)
{
	paramName = rpf.name();

	vlay = new QHBoxLayout();
	vlay->setSpacing(0);
	for (int i = 0; i < 3; ++i) {
		coordSB[i]     = new QLineEdit(this);
		QFont baseFont = coordSB[i]->font();
		if (baseFont.pixelSize() != -1)
			baseFont.setPixelSize(baseFont.pixelSize() * 3 / 4);
		else
			baseFont.setPointSize(baseFont.pointSize() * 3 / 4);
		coordSB[i]->setFont(baseFont);
		coordSB[i]->setMinimumWidth(coordSB[i]->sizeHint().width() / 4);
		coordSB[i]->setValidator(new QDoubleValidator());
		coordSB[i]->setAlignment(Qt::AlignRight);
		coordSB[i]->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
		vlay->addWidget(coordSB[i]);
		connect(coordSB[i], SIGNAL(textChanged(QString)), p, SIGNAL(parameterChanged()));
	}
	this->setValue(paramName, parameter->value().getPoint3f());
	// if we have a connection to the current glarea we can setup the additional
	// button for getting the current view direction.
	if (gla_curr) {
		QStringList names;
		names << "View Dir.";
		names << "Raster Camera Dir.";

		getPoint3Combo = new QComboBox(this);
		getPoint3Combo->addItems(names);
		getPoint3Combo->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		vlay->addWidget(getPoint3Combo);

		connect(
			gla_curr,
			SIGNAL(transmitViewDir(QString, Point3m)),
			this,
			SLOT(setValue(QString, Point3m)));
		connect(
			gla_curr,
			SIGNAL(transmitShot(QString, Shotm)),
			this,
			SLOT(setShotValue(QString, Shotm)));
		connect(this, SIGNAL(askViewDir(QString)), gla_curr, SLOT(sendViewDir(QString)));
		connect(this, SIGNAL(askCameraDir(QString)), gla_curr, SLOT(sendRasterShot(QString)));

		getPoint3Button = new QPushButton("Get", this);
		getPoint3Button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		connect(getPoint3Button, SIGNAL(clicked()), this, SLOT(getPoint()));

		vlay->addWidget(getPoint3Button);
	}
}

DirectionWidget::~DirectionWidget()
{
	this->disconnect();
}

void DirectionWidget::getPoint()
{
	int index = getPoint3Combo->currentIndex();
	switch (index) {
	case 0: emit askViewDir(paramName); break;
	case 1: emit askCameraDir(paramName); break;

	default: assert(0);
	}
}

void DirectionWidget::setValue(QString name, Point3m newVal)
{
	if (name == paramName) {
		for (int i = 0; i < 3; ++i)
			coordSB[i]->setText(QString::number(newVal[i], 'g', 4));
	}
}

void DirectionWidget::setShotValue(QString name, Shotm newValShot)
{
	vcg::Point3f p = newValShot.GetViewDir();
	setValue(name, p);
}

vcg::Point3f DirectionWidget::getValue()
{
	return vcg::Point3f(
		coordSB[0]->text().toFloat(), coordSB[1]->text().toFloat(), coordSB[2]->text().toFloat());
}

void DirectionWidget::collectWidgetValue()
{
	parameter->setValue(Point3fValue(vcg::Point3f(
		coordSB[0]->text().toFloat(), coordSB[1]->text().toFloat(), coordSB[2]->text().toFloat())));
}

void DirectionWidget::resetWidgetValue()
{
	for (unsigned int ii = 0; ii < 3; ++ii)
		coordSB[ii]->setText(QString::number(parameter->value().getPoint3f()[ii], 'g', 3));
}

void DirectionWidget::setWidgetValue(const Value& nv)
{
	for (unsigned int ii = 0; ii < 3; ++ii)
		coordSB[ii]->setText(QString::number(nv.getPoint3f()[ii], 'g', 3));
}

void DirectionWidget::addWidgetToGridLayout(QGridLayout* lay, const int r)
{
	if (lay != NULL) {
		lay->addLayout(vlay, r, 1);
	}
	RichParameterWidget::addWidgetToGridLayout(lay, r);
}

/******************************************/
// Matrix44fWidget Implementation
/******************************************/

Matrix44fWidget::Matrix44fWidget(
	QWidget*             p,
	const RichMatrix44f& rpf,
	const RichMatrix44f& rdef,
	QWidget*             gla_curr) :
		RichParameterWidget(p, rpf, rdef)
{
	valid = false;
	m.SetIdentity();
	paramName = rpf.name();

	vlay  = new QVBoxLayout();
	lay44 = new QGridLayout();

	for (int i = 0; i < 16; ++i) {
		coordSB[i]     = new QLineEdit(p);
		QFont baseFont = coordSB[i]->font();
		if (baseFont.pixelSize() != -1)
			baseFont.setPixelSize(baseFont.pixelSize() * 3 / 4);
		else
			baseFont.setPointSize(baseFont.pointSize() * 3 / 4);
		coordSB[i]->setFont(baseFont);
		coordSB[i]->setMinimumWidth(coordSB[i]->sizeHint().width()/4);
		coordSB[i]->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);
		coordSB[i]->setValidator(new QDoubleValidator(p));
		lay44->addWidget(coordSB[i], i / 4, i % 4);
		connect(
			coordSB[i],
			SIGNAL(textChanged(const QString&)),
			this,
			SLOT(invalidateMatrix(const QString&)));
	}
	this->setValue(paramName, parameter->value().getMatrix44f());

	QLabel* headerL = new QLabel("Matrix:", this);
	vlay->addWidget(headerL, 0, Qt::AlignTop);

	vlay->addLayout(lay44);

	QPushButton* getMatrixButton = new QPushButton("Read from current layer");
	vlay->addWidget(getMatrixButton);

	QPushButton* pasteMatrixButton = new QPushButton("Paste from clipboard");
	vlay->addWidget(pasteMatrixButton);

	// gridLay->addLayout(vlay,row,1,Qt::AlignTop);

	connect(
		gla_curr,
		SIGNAL(transmitMatrix(QString, Matrix44m)),
		this,
		SLOT(setValue(QString, Matrix44m)));
	connect(getMatrixButton, SIGNAL(clicked()), this, SLOT(getMatrix()));
	connect(pasteMatrixButton, SIGNAL(clicked()), this, SLOT(pasteMatrix()));
	connect(this, SIGNAL(askMeshMatrix(QString)), gla_curr, SLOT(sendMeshMatrix(QString)));
}

Matrix44fWidget::~Matrix44fWidget()
{
}

void Matrix44fWidget::setValue(QString name, Matrix44m newVal)
{
	if (name == paramName) {
		for (int i = 0; i < 16; ++i)
			coordSB[i]->setText(QString::number(newVal[i / 4][i % 4], 'g', 4));
		valid = true;
		m     = newVal;
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
	QClipboard* clipboard  = QApplication::clipboard();
	QString     shotString = clipboard->text().trimmed();
	if (shotString.contains(' ')) {
		QStringList list1 = shotString.split(" ");
		if (list1.size() != 16)
			return;
		valid  = false;
		int id = 0;
		for (QStringList::iterator i = list1.begin(); i != list1.end(); ++i, ++id) {
			bool ok = true;
			(*i).toFloat(&ok);
			if (!ok)
				return;
		}
		id = 0;
		for (QStringList::iterator i = list1.begin(); i != list1.end(); ++i, ++id) {
			coordSB[id]->setText(*i);
		}
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
		Matrix44m tempM;
		for (unsigned int i = 0; i < 16; ++i)
			tempM[i / 4][i % 4] = coordSB[i]->text().toFloat();
		parameter->setValue(Matrix44fValue(tempM));
	}
	else {
		parameter->setValue(Matrix44fValue(m));
	}
}

void Matrix44fWidget::resetWidgetValue()
{
	valid = false;
	vcg::Matrix44f m;
	m.SetIdentity();
	for (unsigned int ii = 0; ii < 16; ++ii) {
		coordSB[ii]->setText(
			QString::number(parameter->value().getMatrix44f()[ii / 4][ii % 4], 'g', 3));
	}
}

void Matrix44fWidget::setWidgetValue(const Value& nv)
{
	valid = true;
	m     = nv.getMatrix44f();
	for (unsigned int ii = 0; ii < 16; ++ii)
		coordSB[ii]->setText(QString::number(nv.getMatrix44f()[ii / 4][ii % 4], 'g', 3));
}

void Matrix44fWidget::addWidgetToGridLayout(QGridLayout* lay, const int r)
{
	if (lay != NULL) {
		lay->addLayout(vlay, r, 1, Qt::AlignTop);
	}
	RichParameterWidget::addWidgetToGridLayout(lay, r);
}

void Matrix44fWidget::invalidateMatrix(const QString& /*s*/)
{
	valid = false;
}

/******************************************/
// ShotfWidget Implementation
/******************************************/

ShotfWidget::ShotfWidget(
	QWidget*         p,
	const RichShotf& rpf,
	const RichShotf& rdef,
	QWidget*         gla_curr) :
		RichParameterWidget(p, rpf, rdef)
{
	paramName = rpf.name();

	hlay = new QHBoxLayout();

	this->setShotValue(paramName, parameter->value().getShotf());
	// if we have a connection to the current glarea we can setup the additional
	// button for getting the current view direction.
	if (gla_curr) {
		getShotButton = new QPushButton("Get shot", this);
		getShotButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
		hlay->addWidget(getShotButton);

		QStringList names;
		names << "Current Trackball";
		names << "Current Mesh";
		names << "Current Raster";
		names << "From File";

		getShotCombo = new QComboBox(this);
		getShotCombo->addItems(names);
		hlay->addWidget(getShotCombo);
		connect(getShotCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(getShot()));
		connect(getShotButton, SIGNAL(clicked()), this, SLOT(getShot()));
		connect(
			gla_curr,
			SIGNAL(transmitShot(QString, Shotm)),
			this,
			SLOT(setShotValue(QString, Shotm)));
		connect(this, SIGNAL(askViewerShot(QString)), gla_curr, SLOT(sendViewerShot(QString)));
		connect(this, SIGNAL(askMeshShot(QString)), gla_curr, SLOT(sendMeshShot(QString)));
		connect(this, SIGNAL(askRasterShot(QString)), gla_curr, SLOT(sendRasterShot(QString)));
	}
}

void ShotfWidget::getShot()
{
	int index = getShotCombo->currentIndex();
	switch (index) {
	case 0: emit askViewerShot(paramName); break;
	case 1: emit askMeshShot(paramName); break;
	case 2: emit askRasterShot(paramName); break;
	case 3: {
		QString filename = QFileDialog::getOpenFileName(
			this, tr("Load xml camera"), "./", tr("Xml Files (*.xml)"));
		QFile     qf(filename);
		QFileInfo qfInfo(filename);

		if (!qf.open(QIODevice::ReadOnly))
			return;

		QDomDocument doc("XmlDocument"); // It represents the XML document
		if (!doc.setContent(&qf))
			return;
		qf.close();

		QString type = doc.doctype().name();

	} break;
	default: assert(0);
	}
}

ShotfWidget::~ShotfWidget()
{
}

void ShotfWidget::setShotValue(QString name, Shotm newVal)
{
	if (name == paramName) {
		curShot = newVal;
	}
}

Shotm ShotfWidget::getValue()
{
	return curShot;
}

void ShotfWidget::collectWidgetValue()
{
	parameter->setValue(ShotfValue(curShot));
}

void ShotfWidget::resetWidgetValue()
{
	curShot = parameter->value().getShotf();
}

void ShotfWidget::setWidgetValue(const Value& nv)
{
	curShot = nv.getShotf();
}

void ShotfWidget::addWidgetToGridLayout(QGridLayout* lay, const int r)
{
	if (lay != NULL) {
		lay->addLayout(hlay, r, 1);
	}
	RichParameterWidget::addWidgetToGridLayout(lay, r);
}

/******************************************/
// DynamicFloatWidget Implementation
/******************************************/

DynamicFloatWidget::DynamicFloatWidget(
	QWidget*                p,
	const RichDynamicFloat& rdf,
	const RichDynamicFloat& rdef) :
		RichParameterWidget(p, rdf, rdef)
{
	int numbdecimaldigit = 4;
	minVal               = rdf.min;
	maxVal               = rdf.max;
	valueLE              = new QLineEdit(this);
	valueLE->setAlignment(Qt::AlignRight);

	valueSlider = new QSlider(Qt::Horizontal, this);
	valueSlider->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	valueSlider->setMinimum(0);
	valueSlider->setMaximum(100);
	valueSlider->setValue(floatToInt(parameter->value().getFloat()));
	RichDynamicFloat* dfd = reinterpret_cast<RichDynamicFloat*>(parameter);
	QFontMetrics fm(valueLE->font());
	QSize        sz = fm.size(Qt::TextSingleLine, QString::number(0));
	valueLE->setValidator(new QDoubleValidator(dfd->min, dfd->max, numbdecimaldigit, valueLE));
	valueLE->setText(QString::number(parameter->value().getFloat()));
	valueLE->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

	hlay = new QHBoxLayout();
	hlay->addWidget(valueLE);
	hlay->addWidget(valueSlider);
	int maxlenghtplusdot = 8; // numbmaxvaluedigit + numbdecimaldigit + 1;
	valueLE->setMaxLength(maxlenghtplusdot);
	valueLE->setMaximumWidth(sz.width() * maxlenghtplusdot);

	connect(valueLE, SIGNAL(textChanged(const QString&)), this, SLOT(setValue()));
	connect(valueSlider, SIGNAL(valueChanged(int)), this, SLOT(setValue(int)));
	connect(this, SIGNAL(dialogParamChanged()), p, SIGNAL(parameterChanged()));
}

DynamicFloatWidget::~DynamicFloatWidget()
{
}

float DynamicFloatWidget::getValue()
{
	return float(valueLE->text().toDouble());
}

void DynamicFloatWidget::setValue(float newVal)
{
	if (QString::number(intToFloat(newVal)) != valueLE->text())
		valueLE->setText(QString::number(intToFloat(newVal)));
}

void DynamicFloatWidget::setValue(int newVal)
{
	if (floatToInt(float(valueLE->text().toDouble())) != newVal) {
		valueLE->setText(QString::number(intToFloat(newVal)));
	}
}

void DynamicFloatWidget::setValue()
{
	float newValLE = float(valueLE->text().toDouble());
	valueSlider->setValue(floatToInt(newValLE));
	emit dialogParamChanged();
}

float DynamicFloatWidget::intToFloat(int val)
{
	return minVal + float(val) / 100.0f * (maxVal - minVal);
}
int DynamicFloatWidget::floatToInt(float val)
{
	return int(100.0f * (val - minVal) / (maxVal - minVal));
}

void DynamicFloatWidget::collectWidgetValue()
{
	parameter->setValue(FloatValue(valueLE->text().toFloat()));
}

void DynamicFloatWidget::resetWidgetValue()
{
	valueLE->setText(QString::number(parameter->value().getFloat()));
}

void DynamicFloatWidget::setWidgetValue(const Value& nv)
{
	valueLE->setText(QString::number(nv.getFloat()));
}

void DynamicFloatWidget::addWidgetToGridLayout(QGridLayout* lay, const int r)
{
	if (lay != NULL) {
		lay->addLayout(hlay, r, 1);
	}
	RichParameterWidget::addWidgetToGridLayout(lay, r);
}

/******************************************/
// ComboWidget Implementation
/******************************************/

ComboWidget::ComboWidget(QWidget* p, const RichParameter& rpar, const RichParameter& rdef) :
		RichParameterWidget(p, rpar, rdef)
{
}

void ComboWidget::init(QWidget* p, int defaultEnum, QStringList values)
{
	enumCombo = new QComboBox(this);
	enumCombo->addItems(values);
	setIndex(defaultEnum);
	connect(enumCombo, SIGNAL(activated(int)), this, SIGNAL(dialogParamChanged()));
	connect(this, SIGNAL(dialogParamChanged()), p, SIGNAL(parameterChanged()));
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
}

void ComboWidget::addWidgetToGridLayout(QGridLayout* lay, const int r)
{
	if (lay != NULL) {
		lay->addWidget(enumCombo, r, 1);
	}
	RichParameterWidget::addWidgetToGridLayout(lay, r);
}

/******************************************/
// EnumWidget Implementation
/******************************************/

EnumWidget::EnumWidget(QWidget* p, const RichEnum& rpar, const RichEnum& rdef) :
		ComboWidget(p, rpar, rdef)
{
	init(p, rpar.value().getInt(), rpar.enumvalues);
}

int EnumWidget::getSize()
{
	return enumCombo->count();
}

void EnumWidget::collectWidgetValue()
{
	parameter->setValue(IntValue(enumCombo->currentIndex()));
}

void EnumWidget::resetWidgetValue()
{
	enumCombo->setCurrentIndex(parameter->value().getInt());
}

void EnumWidget::setWidgetValue(const Value& nv)
{
	enumCombo->setCurrentIndex(nv.getInt());
}

/******************************************/
// MeshWidget Implementation
/******************************************/

MeshWidget::MeshWidget(QWidget* p, const RichMesh& rpar, const RichMesh& rdef) :
		ComboWidget(p, rpar, rdef)
{
	md = ((RichMesh*) parameter)->meshdoc;

	QStringList meshNames;

	// make the default mesh Index be 0
	// defaultMeshIndex = -1;
	int          currentmeshindex = -1;
	unsigned int i                = 0;
	for (const MeshModel& mm : md->meshIterator()) {
		QString shortName = mm.label();
		meshNames.push_back(shortName);
		if (mm.id() == (unsigned int) parameter->value().getInt()) {
			currentmeshindex = i;
		}
		++i;
	}

	init(p, currentmeshindex, meshNames);
}

void MeshWidget::collectWidgetValue()
{
	auto it = md->meshBegin();
	std::advance(it, enumCombo->currentIndex());
	parameter->setValue(IntValue((*it).id()));
}

void MeshWidget::resetWidgetValue()
{
	int          meshindex = -1;
	unsigned int i         = 0;
	for (const MeshModel& mm : md->meshIterator()) {
		if (mm.id() == (unsigned int) parameter->value().getInt()) {
			meshindex = i;
		}
		++i;
	}
	enumCombo->setCurrentIndex(meshindex);
}

void MeshWidget::setWidgetValue(const Value& nv)
{
	int          meshindex = -1;
	unsigned int i         = 0;
	for (const MeshModel& mm : md->meshIterator()) {
		if (mm.id() == (unsigned int) nv.getInt()) {
			meshindex = i;
		}
		++i;
	}
	enumCombo->setCurrentIndex(meshindex);
}

/******************************************/
// IOFileWidget Implementation
/******************************************/

IOFileWidget::IOFileWidget(QWidget* p, const RichParameter& rpar, const RichParameter& rdef) :
		RichParameterWidget(p, rpar, rdef)
{
	filename = new QLineEdit(this);
	filename->setText(tr(""));
	browse  = new QPushButton(this);
	browse->setText("...");
	hlay = new QHBoxLayout();
	hlay->addWidget(filename, 2);
	hlay->addWidget(browse);

	connect(browse, SIGNAL(clicked()), this, SLOT(selectFile()));
	connect(this, SIGNAL(dialogParamChanged()), p, SIGNAL(parameterChanged()));
}

IOFileWidget::~IOFileWidget()
{
	delete filename;
	delete browse;
}

void IOFileWidget::collectWidgetValue()
{
	parameter->setValue(StringValue(filename->text()));
}

void IOFileWidget::resetWidgetValue()
{
	QString fle = parameter->value().getString();
	updateFileName(fle);
}

void IOFileWidget::setWidgetValue(const Value& nv)
{
	QString fle = nv.getString();
	updateFileName(fle);
}

void IOFileWidget::updateFileName(const StringValue& file)
{
	filename->setText(file.getString());
}

void IOFileWidget::addWidgetToGridLayout(QGridLayout* lay, const int r)
{
	if (lay != NULL) {
		lay->addLayout(hlay, r, 1, Qt::AlignTop);
	}
	RichParameterWidget::addWidgetToGridLayout(lay, r);
}

/******************************************/
// SaveFileWidget Implementation
/******************************************/

SaveFileWidget::SaveFileWidget(QWidget* p, const RichSaveFile& rpar, const RichSaveFile& rdef) :
		IOFileWidget(p, rpar, rdef)
{
	filename->setText(parameter->value().getString());
	QString tmp = parameter->value().getString();
}

SaveFileWidget::~SaveFileWidget()
{
}

void SaveFileWidget::selectFile()
{
	RichSaveFile* dec = reinterpret_cast<RichSaveFile*>(parameter);
	QString       ext;
	QString       fl =
		QFileDialog::getSaveFileName(this, tr("Save"), parameter->value().getString(), dec->ext);
	collectWidgetValue();
	updateFileName(fl);
	StringValue fileName(fl);
	parameter->setValue(fileName);
	emit dialogParamChanged();
}

/******************************************/
// OpenFileWidget Implementation
/******************************************/

OpenFileWidget::OpenFileWidget(QWidget* p, const RichOpenFile& rdf, const RichOpenFile& rdef) :
		IOFileWidget(p, rdf, rdef)
{
}

void OpenFileWidget::selectFile()
{
	RichOpenFile* dec = reinterpret_cast<RichOpenFile*>(parameter);
	QString       ext;
	QString       path = QDir::homePath();
	if (!parameter->value().getString().isEmpty())
		path = parameter->value().getString();
	QString fl = QFileDialog::getOpenFileName(this, tr("Open"), path, dec->exts.join(";;"));
	collectWidgetValue();
	updateFileName(fl);
	StringValue fileName(fl);
	parameter->setValue(fileName);
	emit dialogParamChanged();
}

OpenFileWidget::~OpenFileWidget()
{
}
