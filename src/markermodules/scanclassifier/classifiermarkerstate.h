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

#ifndef CLASSIFIERMARKERSTATE_H
#define CLASSIFIERMARKERSTATE_H

#include"classifiermarker.h"

#include <boost/property_tree/ptree_fwd.hpp>


class ClassifierMarkerState
{
	const ClassifierMarker* cassifierMarker;

	std::vector<bool> markerStatus;

public:
	explicit ClassifierMarkerState(const ClassifierMarker& marker);

	void reset();

	void setStatusId(std::size_t id, bool value)                    { markerStatus.at(id) = value; }
	bool getStatusId(std::size_t id)                          const { return markerStatus.at(id); }

	bool parsePTree(const boost::property_tree::ptree& ptree);
	void fillPTree (      boost::property_tree::ptree& ptree) const;
};

#endif // CLASSIFIERMARKERSTATE_H
