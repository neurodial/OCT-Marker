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

#pragma once

#include<vector>
#include<data_structure/point2d.h>
#include<data_structure/conture2d.h>
#include<data_structure/conturesegment.h>



class OrderdContures2D
{
	const std::vector<Line2D  >& lines;
	const std::vector<LineNode>& points;
	std::vector<bool> handeldPoints;
	std::vector<bool> handeldLines;

	std::vector<ContureSegment> segments;

	std::size_t addLineToSegment(ContureSegment& segment, std::size_t lineIdx  , std::size_t nodeIdx);
	void testAndAddCreateSegment(std::size_t lineIndex, std::size_t nodeIdx);

	void handelLineSegment(const std::size_t nodeId);

	void handleOpenEnds();
	void handleOthers();
public:
	explicit OrderdContures2D(const Conture2D& conture);


	const std::vector<ContureSegment>& getSegments()          const { return segments; }

};

