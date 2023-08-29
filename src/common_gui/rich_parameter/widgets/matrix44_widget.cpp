/*****************************************************************************
 * MeshLab                                                           o o     *
 * Visual and Computer Graphics Library                            o     o   *
 *                                                                _   O  _   *
 * Copyright(C) 2004-2022                                           \/)\/    *
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

#include "matrix44_widget.h"

#include <QApplication>
#include <QClipboard>

Matrix44Widget::Matrix44Widget(
	QWidget*             p,
	const RichMatrix44&  param,
	const Matrix44Value& defaultValue,
	QWidget*             gla) :
		RichParameterWidget(p, param, defaultValue)
{
	valid = false;
	m.SetIdentity();
	paramName = param.name();

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
		coordSB[i]->setMinimumWidth(coordSB[i]->sizeHint().width() / 4);
		coordSB[i]->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
		coordSB[i]->setValidator(new QDoubleValidator(p));
		lay44->addWidget(coordSB[i], i / 4, i % 4);
		widgets.push_back(coordSB[i]);
		connect(
			coordSB[i],
			SIGNAL(textChanged(const QString&)),
			this,
			SLOT(invalidateMatrix(const QString&)));
		connect(coordSB[i], SIGNAL(editingFinished()), this, SLOT(setParameterChanged()));
	}
	this->setValue(paramName, param.value().getMatrix44());

	QLabel* headerL = new QLabel("Matrix:", this);
	vlay->addWidget(headerL, 0, Qt::AlignTop);
	widgets.push_back(headerL);

	vlay->addLayout(lay44);

	QPushButton* getMatrixButton = new QPushButton("Read from current layer");
	vlay->addWidget(getMatrixButton);
	widgets.push_back(getMatrixButton);

	QPushButton* pasteMatrixButton = new QPushButton("Paste from clipboard");
	vlay->addWidget(pasteMatrixButton);
	widgets.push_back(pasteMatrixButton);

	// gridLay->addLayout(vlay,row,1,Qt::AlignTop);

	connect(
		gla, SIGNAL(transmitMatrix(QString, Matrix44m)), this, SLOT(setValue(QString, Matrix44m)));
	connect(getMatrixButton, SIGNAL(clicked()), this, SLOT(getMatrix()));
	connect(pasteMatrixButton, SIGNAL(clicked()), this, SLOT(pasteMatrix()));
	connect(this, SIGNAL(askMeshMatrix(QString)), gla, SLOT(sendMeshMatrix(QString)));
}

Matrix44Widget::~Matrix44Widget()
{
}

void Matrix44Widget::addWidgetToGridLayout(QGridLayout* lay, const int r)
{
	if (lay != nullptr) {
		lay->addLayout(vlay, r, 1, Qt::AlignTop);
	}
	RichParameterWidget::addWidgetToGridLayout(lay, r);
}

std::shared_ptr<Value> Matrix44Widget::getWidgetValue() const
{
	if (!valid) {
		Matrix44m tempM;
		for (unsigned int i = 0; i < 16; ++i)
			tempM[i / 4][i % 4] = coordSB[i]->text().toFloat();
		return std::make_shared<Matrix44Value>(tempM);
	}
	else {
		return std::make_shared<Matrix44Value>(m);
	}
}

void Matrix44Widget::setWidgetValue(const Value& nv)
{
	valid = true;
	m     = nv.getMatrix44();
	for (unsigned int ii = 0; ii < 16; ++ii)
		coordSB[ii]->setText(QString::number(nv.getMatrix44()[ii / 4][ii % 4], 'g', 3));
}

Matrix44m Matrix44Widget::getValue()
{
	if (!valid) {
		Scalarm val[16];
		for (unsigned int i = 0; i < 16; ++i)
			val[i] = coordSB[i]->text().toFloat();
		return Matrix44m(val);
	}
	return m;
}

void Matrix44Widget::setValue(QString name, Matrix44m newVal)
{
	if (name == paramName) {
		for (int i = 0; i < 16; ++i)
			coordSB[i]->setText(QString::number(newVal[i / 4][i % 4], 'g', 4));
		valid = true;
		m     = newVal;
	}
}

void Matrix44Widget::getMatrix()
{
	emit askMeshMatrix(QString("TransformMatrix"));
}

void Matrix44Widget::pasteMatrix()
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

void Matrix44Widget::invalidateMatrix(const QString& /*s*/)
{
	valid = false;
}
