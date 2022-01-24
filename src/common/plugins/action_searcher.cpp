/*****************************************************************************
 * MeshLab                                                           o o     *
 * A versatile mesh processing toolbox                             o     o   *
 *                                                                _   O  _   *
 * Copyright(C) 2005-2022                                           \/)\/    *
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

#include "action_searcher.h"

#include <cmath>
#include <QRegExp>

#include "interfaces/filter_plugin.h"

ActionSearcher::ActionSearcher()
{
}

void ActionSearcher::clear()
{
	titleActionsMap.clear();
}

/**
 * @brief Adds the given action to the ActionSearcher, allowing queries to it.
 * @param action
 */
void ActionSearcher::addAction(QAction *action, bool usePythonFilterNames)
{
	if (action != nullptr) {

		// add title to the action map
		QString title = action->text();
		title = title.toLower();
		title.remove(ignexp);
		QStringList res = title.split(sepexp, Qt::SkipEmptyParts);
		res.removeDuplicates();
		addSubStrings(res);
		for (const QString& str : qAsConst(res)) {
			titleActionsMap[str].push_back(action);
		}
		if (usePythonFilterNames) {
			// if the action is a filter, we should add also the python name to the search
			QObject* parent = action->parent();
			FilterPlugin* fp = qobject_cast<FilterPlugin*>(parent);
			if (fp) {
				QString title = fp->pythonFilterName(action);
				title.replace("_", " ");
				title.remove(ignexp);
				QStringList res = title.split(sepexp, Qt::SkipEmptyParts);
				res.removeDuplicates();
				addSubStrings(res);
				for (const QString& str : qAsConst(res)) {
					titleActionsMap[str].push_back(action);
				}
			}
		}

		// add info to the action map
		QString info = action->toolTip();
		info = info.toLower();
		info.remove(ignexp);
		res = info.split(sepexp, Qt::SkipEmptyParts);
		res.removeDuplicates();
		addSubStrings(res);
		for (const QString& str : qAsConst(res)) {
			infoActionsMap[str].push_back(action);
		}
	}
}

/**
 * @brief Performs a query using the inputString and returns an array containing the best matching
 * actions matching to the input string.
 *
 * The array will have maximum size equal to maxNumberactions.
 * @param inputString: query string
 * @param maxNumberActions: maximum size of the output array
 * @return array containing the best matching actions
 */
std::vector<QAction *> ActionSearcher::bestMatchingActions(QString inputString, int maxNumberActions) const
{
	std::vector<QAction *> res;

	// clean the input string
	inputString = inputString.toLower();
	inputString.replace("_", " "); // to allow python search
	inputString.remove(ignexp);

	// split the input string
	QStringList inputList = inputString.split(sepexp, Qt::SkipEmptyParts);
	inputList.removeDuplicates();

	const float bonuspertitleword = 1.0f / std::pow(10,inputList.size());

	// store the ranking for each action
	std::map<QAction*, float> actionRanking;

	// for each input string
	for (const QString& str : qAsConst(inputList)) {
		auto it = titleActionsMap.find(str);
		// if matches in a title of an action
		if (it != titleActionsMap.end()) {
			for (QAction* act : it->second) { // for each matching action, increment its ranking
				actionRanking[act] += bonuspertitleword;
			}
		}
		it = infoActionsMap.find(str);
		// if matches in a info of an action
		if (it != infoActionsMap.end()) {
			for (QAction* act : it->second) { // for each matching action, increment its ranking
				actionRanking[act]++;
			}
		}
 	}

	std::map<float, std::vector<QAction*>> rankingMap; // flipped map of actionRanking
	// populate the flipped map
	for (const auto& p : actionRanking) {
		// for each ranking, push another action havint that ranking
		rankingMap[p.second].push_back(p.first);
	}
	// at the end, the map will store the actions stored by ranking

	int count = 0; // used to stop the number of inserted actions in the resulting vector

	// reverse iteration: start from the biggest ranking
	for (auto it = rankingMap.rbegin(); it != rankingMap.rend() && count < maxNumberActions; ++it) {
		auto v1 = it->second;
		// need to sort properly actions having the same ranking, since they would be sorted using
		// memory addresses, that are not deterministic: we use action titles
		std::sort(v1.begin(), v1.end(), ActionComparator());
		// insert the sorted actions to the result vector
		res.insert(res.end(), v1.begin(), v1.end());
		count += it->second.size();
	}
	// if at the end the number exceeded the maximum number, we truncate the array
	if (count >= maxNumberActions) {
		res.resize(maxNumberActions);
	}
	return res;
}

void ActionSearcher::addSubStrings(QStringList &res)
{
	QStringList resWithPrefix;
	foreach(QString str, res)
	{
		QString strPref = str;
		resWithPrefix.push_back(strPref);
		for(int i=0;i<str.length()-3;++i)
		{
			strPref.chop(1);
			resWithPrefix.push_back(strPref);
		}
	}
	resWithPrefix.removeDuplicates();
	res = resWithPrefix;
}
