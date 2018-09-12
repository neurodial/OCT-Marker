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

#ifndef DEFINEDINTERVALLMARKER_H
#define DEFINEDINTERVALLMARKER_H

#include <data_structure/intervalmarker.h>
#include <map>


#include <QObject>

class DefinedIntervalMarker : QObject
{
	Q_OBJECT
public:
	typedef std::map<std::string, IntervalMarker> IntervallMarkerMap;

	static DefinedIntervalMarker& getInstance()                     { static DefinedIntervalMarker instance; return instance; }

	const IntervallMarkerMap& getIntervallMarkerMap() const         { return intervallMarkerMap; }

private:


	IntervallMarkerMap intervallMarkerMap;

	DefinedIntervalMarker();
	void addMarker(IntervalMarker&& marker);
};

#endif // DEFINEDINTERVALLMARKER_H
