/****************************************************************************
* VCGLib                                                            o o     *
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

#ifndef RICHPARAMETERLISTFRAME_H
#define RICHPARAMETERLISTFRAME_H

#include "richparameterwidgets.h"

#include <map>

#include<QCheckBox>
#include<QRadioButton>
#include<QSpinBox>
#include<QTableWidget>
#include<QComboBox>
#include<QGridLayout>
#include<QDockWidget>

/*---------------------------------*/

/**
 * This class is used to automatically create a frame from a set of parameters.
 * it is used mostly for creating the main dialog of the filters, but it is used also
 * in the creation of the additional saving options, post and pre opening processing
 * and for general parameter setting in edit plugins (e.g. look at the alignment parameters)
 */
class RichParameterListFrame : public QFrame
{
	Q_OBJECT
public:
	typedef std::map<QString, RichParameterWidget *>::iterator iterator;

	RichParameterListFrame(const RichParameterList& curParSet, const RichParameterList& defParSet, QWidget *p, QWidget *gla=0);
	RichParameterListFrame(const RichParameterList& curParSet, QWidget *p, QWidget *gla=0);
	RichParameterListFrame(const RichParameter& curPar, const RichParameter& defPar, QWidget *p, QWidget *gla=0);
	~RichParameterListFrame();

	// The curParSet that is passed must be 'compatible' with the RichParameterSet that have been used to create the frame.
	// This function updates the RichParameterSet used to create the frame AND fill also the passed <curParSet>
	void writeValuesOnParameterList(RichParameterList &curParSet);
	void resetValues();

	void toggleHelp();

	unsigned int size() const;

	iterator begin();
	iterator end();

signals:
	void parameterChanged();

public slots:
	void toggleAdvancedParameters();

private:
	void loadFrameContent(const RichParameterList& curParSet, const RichParameterList& defParSet);
	void loadFrameContent(const RichParameterList& curParSet);

	void loadFrameContent(const RichParameter& curPar, const RichParameter& defPar);

	static RichParameterWidget* createWidgetFromRichParameter(
			QWidget* parent,
			const RichParameter& pd,
			const RichParameter& def);

	std::map<QString, RichParameterWidget *> stdfieldwidgets;
	QVector<QLabel *> helpList;

	QWidget *gla; // used for having a link to the glarea that spawned the parameter asking.
	QFrame* hiddenFrame;
	QPushButton* showHiddenFramePushButton;
};

#endif //RICHPARAMETERLISTFRAME_H

