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

#include "rich_parameter_list_frame.h"

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
	const RichParameterList& parameterList,
	const RichParameterList& defaultValuesList,
	QWidget*                 p,
	QWidget*                 gla) :
		QFrame(p), isHelpVisible(false), gla(gla)
{
	loadFrameContent(parameterList, defaultValuesList);
}

RichParameterListFrame::RichParameterListFrame(
	const RichParameterList& parameterList,
	QWidget*                 p,
	QWidget*                 gla) :
		QFrame(p), isHelpVisible(false), gla(gla)
{
	loadFrameContent(parameterList, parameterList);
}

RichParameterListFrame::RichParameterListFrame(
	const RichParameter& parameter,
	const RichParameter& defaultValue,
	QWidget*             p,
	QWidget*             gla) :
		QFrame(p), isHelpVisible(false), gla(gla)
{
	RichParameterList crpl;
	crpl.addParam(parameter);
	RichParameterList drpl;
	drpl.addParam(defaultValue);
	loadFrameContent(crpl, drpl);
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
		rpl.getParameterByName(p.first).setValue(*p.second->getWidgetValue(), true);
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

RichParameterList RichParameterListFrame::currentRichParameterList() const
{
	for (const auto& w : *this) {
		if (w.second->hasBeenChanged()) {
			rpl.getParameterByName(w.first).setValue(*w.second->getWidgetValue());
		}
	}
	return rpl;
}

RichParameterListFrame::iterator RichParameterListFrame::begin()
{
	return stdfieldwidgets.begin();
}

RichParameterListFrame::iterator RichParameterListFrame::end()
{
	return stdfieldwidgets.end();
}

RichParameterListFrame::const_iterator RichParameterListFrame::begin() const
{
	return stdfieldwidgets.begin();
}

RichParameterListFrame::const_iterator RichParameterListFrame::end() const
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
			RichParameterWidget* wd      = createWidgetFromRichParameter(this, *fpi, defrp.value());
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
	rpl = curParSet;
	rpl.setAllValuesAsDefault();
}

RichParameterWidget* RichParameterListFrame::createWidgetFromRichParameter(
	RichParameterListFrame* parent,
	const RichParameter&    pd,
	const Value&            defaultValue)
{
	RichParameterWidget* w = nullptr;
	if (pd.isOfType<RichPercentage>() && defaultValue.isOfType<FloatValue>()) {
		w = new PercentageWidget(
			parent, (const RichPercentage&) pd, (const FloatValue&) defaultValue);
	}
	else if (pd.isOfType<RichDynamicFloat>() && defaultValue.isOfType<FloatValue>()) {
		w = new DynamicFloatWidget(
			parent, (const RichDynamicFloat&) pd, (const FloatValue&) defaultValue);
	}
	else if (pd.isOfType<RichEnum>() && defaultValue.isOfType<IntValue>()) {
		w = new EnumWidget(parent, (const RichEnum&) pd, (const IntValue&) defaultValue);
	}
	else if (pd.isOfType<RichBool>() && defaultValue.isOfType<BoolValue>()) {
		w = new BoolWidget(parent, (const RichBool&) pd, (const BoolValue&) defaultValue);
	}
	else if (pd.isOfType<RichInt>() && defaultValue.isOfType<IntValue>()) {
		w = new IntWidget(parent, (const RichInt&) pd, (const IntValue&) defaultValue);
	}
	else if (pd.isOfType<RichFloat>() && defaultValue.isOfType<FloatValue>()) {
		w = new FloatWidget(parent, (const RichFloat&) pd, (const FloatValue&) defaultValue);
	}
	else if (pd.isOfType<RichString>() && defaultValue.isOfType<StringValue>()) {
		w = new StringWidget(parent, (const RichString&) pd, (const StringValue&) defaultValue);
	}
	else if (pd.isOfType<RichMatrix44>() && defaultValue.isOfType<Matrix44Value>()) {
		w = new Matrix44Widget(
			parent,
			(const RichMatrix44&) pd,
			(const Matrix44Value&) defaultValue,
			reinterpret_cast<RichParameterListFrame*>(parent)->gla);
	}
	else if (pd.isOfType<RichPosition>() && defaultValue.isOfType<Point3Value>()) {
		w = new PositionWidget(
			parent,
			(const RichPosition&) pd,
			(const Point3Value&) defaultValue,
			reinterpret_cast<RichParameterListFrame*>(parent)->gla);
	}
	else if (pd.isOfType<RichDirection>() && defaultValue.isOfType<Point3Value>()) {
		w = new DirectionWidget(
			parent,
			(const RichDirection&) pd,
			(const Point3Value&) defaultValue,
			reinterpret_cast<RichParameterListFrame*>(parent)->gla);
	}
	else if (pd.isOfType<RichShot>() && defaultValue.isOfType<ShotValue>()) {
		w = new ShotWidget(
			parent,
			(const RichShot&) pd,
			(const ShotValue&) defaultValue,
			reinterpret_cast<RichParameterListFrame*>(parent)->gla);
	}
	else if (pd.isOfType<RichColor>() && defaultValue.isOfType<ColorValue>()) {
		w = new ColorWidget(parent, (const RichColor&) pd, (const ColorValue&) defaultValue);
	}
	else if (pd.isOfType<RichFileOpen>() && defaultValue.isOfType<StringValue>()) {
		w = new OpenFileWidget(
			parent, (const RichFileOpen&) pd, (const StringValue&) defaultValue);
	}
	else if (pd.isOfType<RichFileSave>() && defaultValue.isOfType<StringValue>()) {
		w = new SaveFileWidget(
			parent, (const RichFileSave&) pd, (const StringValue&) defaultValue);
	}
	else if (pd.isOfType<RichMesh>() && defaultValue.isOfType<IntValue>()) {
		w = new MeshWidget(parent, (const RichMesh&) pd, (const IntValue&) defaultValue);
	}
	else {
		std::cerr << "RichParameter type not supported for widget creation.\n";
		assert(0);
		return nullptr;
	}
	connect(w, SIGNAL(parameterChanged()), parent, SIGNAL(parameterChanged()));
	return w;
}
