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

#ifndef MESHLAB_MATRIX44_WIDGET_H
#define MESHLAB_MATRIX44_WIDGET_H

#include "rich_parameter_widget.h"

class Matrix44Widget : public RichParameterWidget
{
	Q_OBJECT

public:
	Matrix44Widget(
		QWidget*             p,
		const RichMatrix44&  param,
		const Matrix44Value& defaultValue,
		QWidget*             gla);
	~Matrix44Widget();

	void                   addWidgetToGridLayout(QGridLayout* lay, const int r);
	std::shared_ptr<Value> getWidgetValue() const;
	void                   setWidgetValue(const Value& nv);

	Matrix44m getValue();

public slots:
	void setValue(QString name, Matrix44m val);
	void getMatrix();
	void pasteMatrix();
	void invalidateMatrix(const QString& s);
signals:
	void askMeshMatrix(QString);

private:
	QString      paramName;
	QLineEdit*   coordSB[16];
	QPushButton* getPoint3Button;
	QGridLayout* lay44;
	QVBoxLayout* vlay;
	Matrix44m    m;
	bool         valid;
};

#endif // MESHLAB_MATRIX44_WIDGET_H
