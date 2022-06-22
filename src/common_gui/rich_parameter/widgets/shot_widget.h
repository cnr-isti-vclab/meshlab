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

#ifndef MESHLAB_SHOT_WIDGET_H
#define MESHLAB_SHOT_WIDGET_H

#include "rich_parameter_widget.h"

class ShotWidget : public RichParameterWidget
{
	Q_OBJECT

public:
	ShotWidget(QWidget* p, const RichShot& param, const ShotValue& defaultValue, QWidget* gla);
	~ShotWidget();

	void                   addWidgetToGridLayout(QGridLayout* lay, const int r);
	std::shared_ptr<Value> getWidgetValue() const;
	void                   setWidgetValue(const Value& nv);

	Shotm getValue();

public slots:
	void getShot();
	void setShotValue(QString name, Shotm val);
signals:
	void askRasterShot(QString);
	void askMeshShot(QString);
	void askViewerShot(QString);

private:
	QString      paramName;
	Shotm        curShot;
	QLineEdit*   shotLE;
	QPushButton* getShotButton;
	QComboBox*   getShotCombo;
	QHBoxLayout* hlay;
};

#endif // MESHLAB_SHOT_WIDGET_H
