/*****************************************************************************
 * VCGLib                                                            o o     *
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

#include "richparameterlistframe.h"

#include <QClipboard>
#include <QColorDialog>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QToolBar>

using namespace vcg;

RichParameterListFrame::RichParameterListFrame(QWidget* parent) :
		QFrame(parent), isHelpVisible(false), gla(nullptr)
{
}

RichParameterListFrame::RichParameterListFrame(
	const RichParameterList& curParSet,
	const RichParameterList& defParSet,
	QWidget*                 p,
	QWidget*                 gla) :
		QFrame(p), isHelpVisible(false), gla(gla)
{
	loadFrameContent(curParSet, defParSet);
}

RichParameterListFrame::RichParameterListFrame(
	const RichParameterList& curParSet,
	QWidget*                 p,
	QWidget*                 gla) :
		QFrame(p), isHelpVisible(false), gla(gla)
{
	loadFrameContent(curParSet);
}

RichParameterListFrame::RichParameterListFrame(
	const RichParameter& curPar,
	const RichParameter& defPar,
	QWidget*             p,
	QWidget*             gla) :
		QFrame(p), isHelpVisible(false), gla(gla)
{
	loadFrameContent(curPar, defPar);
}

RichParameterListFrame::~RichParameterListFrame()
{
}

void RichParameterListFrame::initParams(
	const RichParameterList& curParSet,
	const RichParameterList& defParSet,
	QWidget*                 gla)
{
	if (gla != nullptr)
		this->gla = gla;
	loadFrameContent(curParSet, defParSet);
}

/**
 * @brief RichParameterListFrame::readValues
 * From GUI to RichParameterList
 * @param curParSet
 */
void RichParameterListFrame::writeValuesOnParameterList(RichParameterList& curParSet)
{
	assert(curParSet.size() == (unsigned int) stdfieldwidgets.size());
	for (auto& p : stdfieldwidgets) {
		curParSet.setValue(p.first, *(p.second)->getWidgetValue());
	}
}

void RichParameterListFrame::resetValues()
{
	for (auto& p : stdfieldwidgets) {
		p.second->resetWidgetToDefaultValue();
	}
}

void RichParameterListFrame::toggleHelp()
{
	isHelpVisible = !isHelpVisible;
	for (auto& p : stdfieldwidgets) {
		p.second->setHelpVisible(isHelpVisible);
	}
	adjustSize();
	updateGeometry();
}

unsigned int RichParameterListFrame::size() const
{
	return stdfieldwidgets.size();
}

RichParameterListFrame::iterator RichParameterListFrame::begin()
{
	return stdfieldwidgets.begin();
}

RichParameterListFrame::iterator RichParameterListFrame::end()
{
	return stdfieldwidgets.end();
}

void RichParameterListFrame::toggleAdvancedParameters()
{
	if (areAdvancedParametersVisible) {
		showHiddenFramePushButton->setText("▼");
		showHiddenFramePushButton->setToolTip("Show advanced parameters");
	}
	else {
		showHiddenFramePushButton->setText("▲");
		showHiddenFramePushButton->setToolTip("Hide advanced parameters");
	}
	areAdvancedParametersVisible = !areAdvancedParametersVisible;
	for (const QString& name : advancedParameters) {
		stdfieldwidgets[name]->setVisible(areAdvancedParametersVisible);
	}
}

void RichParameterListFrame::loadFrameContent(
	const RichParameterList& curParSet,
	const RichParameterList& defParSet)
{
	if (layout())
		delete layout();
	QGridLayout* glay = new QGridLayout();

	// collect parameters per category
	std::map<QString, std::vector<const RichParameter*>> parameters;

	for (const RichParameter& fpi : curParSet) {
		parameters[fpi.category()].push_back(&fpi);
		if (fpi.isAdvanced()) {
			advancedParameters.insert(fpi.name());
		}
	}

	int i = 0;
	// parameters are organized into categories
	for (const auto& p : parameters) {
		// if not the default category, the category name must be printed in the dialog
		// before the list of parameter widgets
		if (!p.first.isEmpty()) {
			QString labltext = "<P><b>" + p.first + ":</b></P>";
			QLabel* l        = new QLabel(labltext, this);
			l->setAlignment(Qt::AlignRight);
			glay->addWidget(l, i++, 0);
		}
		// put the parameter widgets into the grid layout
		for (const RichParameter* fpi : p.second) {
			const RichParameter& defrp   = defParSet.getParameterByName(fpi->name());
			RichParameterWidget* wd      = createWidgetFromRichParameter(this, *fpi, defrp);
			stdfieldwidgets[fpi->name()] = wd;
			wd->addWidgetToGridLayout(glay, i++);
		}
	}
	if (advancedParameters.size() > 0) {
		showHiddenFramePushButton = new QPushButton("", this);
		showHiddenFramePushButton->setFlat(true);
		glay->addWidget(showHiddenFramePushButton, i++, 0, 1, 3);
		connect(
			showHiddenFramePushButton, SIGNAL(clicked()), this, SLOT(toggleAdvancedParameters()));
		// set first advanced params visible, and then toggle them in order to
		// initialize the button and then set the visibility correctly to all the elements
		areAdvancedParametersVisible = true;
		toggleAdvancedParameters();
	}

	QSpacerItem* spacer = new QSpacerItem(40, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
	glay->addItem(spacer, i++, 0);
	setLayout(glay);
}

/* creates widgets for the standard parameters */
void RichParameterListFrame::loadFrameContent(const RichParameterList& curParSet)
{
	loadFrameContent(curParSet, curParSet);
}

void RichParameterListFrame::loadFrameContent(
	const RichParameter& curPar,
	const RichParameter& defPar)
{
	RichParameterList crpl;
	crpl.addParam(curPar);
	RichParameterList drpl;
	drpl.addParam(defPar);
	loadFrameContent(crpl, drpl);
}

RichParameterWidget* RichParameterListFrame::createWidgetFromRichParameter(
	QWidget*             parent,
	const RichParameter& pd,
	const RichParameter& def)
{
	if (pd.isOfType<RichAbsPerc>() && def.isOfType<RichAbsPerc>()) {
		return new AbsPercWidget(parent, (const RichAbsPerc&) pd, (const FloatValue&) def.value());
	}
	else if (pd.isOfType<RichDynamicFloat>() && def.isOfType<RichDynamicFloat>()) {
		return new DynamicFloatWidget(
			parent, (const RichDynamicFloat&) pd, (const RichDynamicFloat&) def);
	}
	else if (pd.isOfType<RichEnum>() && def.isOfType<RichEnum>()) {
		return new EnumWidget(parent, (const RichEnum&) pd, (const IntValue&) def.value());
	}
	else if (pd.isOfType<RichBool>() && def.isOfType<RichBool>()) {
		return new BoolWidget(parent, (const RichBool&) pd, (const BoolValue&) def.value());
	}
	else if (pd.isOfType<RichInt>() && def.isOfType<RichInt>()) {
		return new IntWidget(parent, (const RichInt&) pd, (const RichInt&) def);
	}
	else if (pd.isOfType<RichFloat>() && def.isOfType<RichFloat>()) {
		return new FloatWidget(parent, (const RichFloat&) pd, (const RichFloat&) def);
	}
	else if (pd.isOfType<RichString>() && def.isOfType<RichString>()) {
		return new StringWidget(parent, (const RichString&) pd, (const RichString&) def);
	}
	else if (pd.isOfType<RichMatrix44f>() && def.isOfType<RichMatrix44f>()) {
		return new Matrix44Widget(
			parent,
			(const RichMatrix44f&) pd,
			(const RichMatrix44f&) def,
			reinterpret_cast<RichParameterListFrame*>(parent)->gla);
	}
	else if (pd.isOfType<RichPosition>() && def.isOfType<RichPosition>()) {
		return new PositionWidget(
			parent,
			(const RichPosition&) pd,
			(const RichPosition&) def,
			reinterpret_cast<RichParameterListFrame*>(parent)->gla);
	}
	else if (pd.isOfType<RichDirection>() && def.isOfType<RichDirection>()) {
		return new DirectionWidget(
			parent,
			(const RichDirection&) pd,
			(const RichDirection&) def,
			reinterpret_cast<RichParameterListFrame*>(parent)->gla);
	}
	else if (pd.isOfType<RichShotf>() && def.isOfType<RichShotf>()) {
		return new ShotWidget(
			parent,
			(const RichShotf&) pd,
			(const RichShotf&) def,
			reinterpret_cast<RichParameterListFrame*>(parent)->gla);
	}
	else if (pd.isOfType<RichColor>() && def.isOfType<RichColor>()) {
		return new ColorWidget(parent, (const RichColor&) pd, (const ColorValue&) def.value());
	}
	else if (pd.isOfType<RichOpenFile>() && def.isOfType<RichOpenFile>()) {
		return new OpenFileWidget(parent, (const RichOpenFile&) pd, (const RichOpenFile&) def);
	}
	else if (pd.isOfType<RichSaveFile>() && def.isOfType<RichSaveFile>()) {
		return new SaveFileWidget(parent, (const RichSaveFile&) pd, (const RichSaveFile&) def);
	}
	else if (pd.isOfType<RichMesh>() && def.isOfType<RichMesh>()) {
		return new MeshWidget(parent, (const RichMesh&) pd, (const IntValue&) def.value());
	}
	else {
		std::cerr << "RichParameter type not supported for widget creation.\n";
		assert(0);
		return nullptr;
	}
}
