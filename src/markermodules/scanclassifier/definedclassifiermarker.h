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

#ifndef DEFINEDCLASSIFIERMARKER_H
#define DEFINEDCLASSIFIERMARKER_H

#include<QObject>

#include"classifiermarker.h"

class DefinedClassifierMarker : QObject
{
	Q_OBJECT
public:
	typedef std::vector<ClassifierMarker> ClassifierMarkerMap;

	static DefinedClassifierMarker& getInstance()                   { static DefinedClassifierMarker instance; return instance; }

	const ClassifierMarkerMap& getVolumeMarkers()             const { return volumeMarkers; }
	const ClassifierMarkerMap& getBscanMarkers ()             const { return bscanMarkers ; }

private:
	DefinedClassifierMarker();

	ClassifierMarkerMap volumeMarkers;
	ClassifierMarkerMap bscanMarkers ;

	void addMarker(ClassifierMarkerMap& map, ClassifierMarker&& marker);
};

#endif // DEFINEDCLASSIFIERMARKER_H
