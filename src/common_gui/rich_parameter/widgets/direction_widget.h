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

#ifndef MESHLAB_DIRECTION_WIDGET_H
#define MESHLAB_DIRECTION_WIDGET_H

#include "rich_parameter_widget.h"

class DirectionWidget : public RichParameterWidget
{
	Q_OBJECT
public:
	DirectionWidget(QWidget* p, const RichDirection& rpf, const RichDirection& rdef, QWidget* gla);
	~DirectionWidget();

	void                   addWidgetToGridLayout(QGridLayout* lay, const int r);
	std::shared_ptr<Value> getWidgetValue() const;
	void                   collectWidgetValue();
	void                   resetWidgetValue();
	void                   setWidgetValue(const Value& nv);

	vcg::Point3f getValue();

public slots:
	void getPoint();
	void setValue(QString name, Point3m val);
	void setShotValue(QString name, Shotm val);

signals:
	void askViewDir(QString);
	void askCameraDir(QString);

private:
	QString      paramName;
	QLineEdit*   coordSB[3];
	QComboBox*   getPoint3Combo;
	QPushButton* getPoint3Button;
	QHBoxLayout* vlay;
};

#endif // MESHLAB_DIRECTION_WIDGET_H
