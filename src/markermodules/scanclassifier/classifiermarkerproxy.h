/*
 * Copyright (c) 2018 Kay Gawlik <kaydev@amarunet.de> <kay.gawlik@beuth-hochschule.de> <kay.gawlik@charite.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef CLASSIFIERMARKERPROXY_H
#define CLASSIFIERMARKERPROXY_H

#include<QObject>
#include<QAction>
#include<vector>

class ClassifierMarker;
class ClassifierMarkerState;
class QActionGroup;

class ClassifierMarkerProxy : public QObject
{
	std::vector<QAction*> actions;
	const ClassifierMarker& marker;
	ClassifierMarkerState* actMarkerState = nullptr;

	std::vector<QAction*> markerActions;

	QActionGroup* actionGroup = nullptr;

	bool changes = false;

public:
	ClassifierMarkerProxy(const ClassifierMarker& marker);

	void setMarkerState(ClassifierMarkerState* marker);

	std::vector<QAction*> getMarkerActions()                        { return markerActions; }
	const ClassifierMarker& getClassifierMarker()             const { return marker; }

	void resetChanges()                                             { changes = false; }
	bool hasChanges()                                         const { return changes;  }

private slots:
	void markerStateChanged(int id, bool value);
	void updateActionStates();
};

#endif // CLASSIFIERMARKERPROXY_H
