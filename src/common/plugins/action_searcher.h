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

#ifndef ACTION_SEARCHER_H
#define ACTION_SEARCHER_H

#include <map>
#include <vector>
#include <QString>
#include <QAction>

class ActionSearcher
{
public:
	ActionSearcher();

	void clear();
	void addAction(QAction* action, bool usePythonFilterNames = false);

	std::vector<QAction*> bestMatchingActions(QString inputString, int maxNumberActions) const;

private:
	const QRegExp sepexp = QRegExp("\\W+");
	const QRegExp ignexp = QRegExp(
		"\\b(an|the|of|it|as|in|by|and|or|for)\\b|\\b[a-z]\\b|'s\\b|\\.|<[^>]*>");

	// map that stores, for each string, all the actions that store that string in their titles
	std::map<QString, std::vector<QAction*>> titleActionsMap;

	// map that stores, for each string, all the actions that store that string in their info
	std::map<QString, std::vector<QAction*>> infoActionsMap;

	struct ActionComparator {
		bool operator()(QAction* a1, QAction* a2) {
			return a1->text() < a2->text();
		}
	};

	static void addSubStrings(QStringList& res);
};

#endif // ACTION_SEARCHER_H
