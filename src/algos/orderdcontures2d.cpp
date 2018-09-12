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

#include "orderdcontures2d.h"


std::size_t OrderdContures2D::addLineToSegment(ContureSegment& segment, std::size_t lineIdx, std::size_t nodeIdx)
{
	const Line2D& line = lines[lineIdx];

	if(line.getNodeAid() != nodeIdx)
	{
		segment.points.push_back(points[line.getNodeAid()].getPoint());
		return line.getNodeAid();
	}
	if(line.getNodeBid() != nodeIdx)
	{
		segment.points.push_back(points[line.getNodeBid()].getPoint());
		return line.getNodeBid();
	}
	return -1; // TODO error
}


void OrderdContures2D::testAndAddCreateSegment(std::size_t lineIndex, std::size_t nodeIdx)
{
	if(handeldLines[lineIndex])
		return;

	ContureSegment segment;
	segment.cirled = true;

	handeldPoints[nodeIdx] = true;

	while(!handeldLines[lineIndex])
	{
		nodeIdx = addLineToSegment(segment, lineIndex, nodeIdx);
		handeldPoints[nodeIdx ] = true;
		handeldLines[lineIndex] = true;

		const std::vector<std::size_t>& lineIndices = points[nodeIdx].getLineIndices();
		if(lineIndices.size() < 2)
		{
			segment.cirled = false;
			break;
		}

		if(lineIndices[0] != lineIndex)
			lineIndex = lineIndices[0];
		else
			lineIndex = lineIndices[1];
	}

	segments.push_back(std::move(segment));
}

void OrderdContures2D::handelLineSegment(std::size_t nodeIdx)
{
	if(!handeldPoints[nodeIdx])
	{
		const std::vector<std::size_t>& lineIndices = points[nodeIdx].getLineIndices();
		for(std::size_t lineIdx : lineIndices)
			testAndAddCreateSegment(lineIdx, nodeIdx);
	}
}

void OrderdContures2D::handleOpenEnds()
{
	std::size_t nodeIdx = 0;
	for(const LineNode& node : points)
	{
		if(node.getLineIndices().size() == 1)
			handelLineSegment(nodeIdx);
		++nodeIdx;
	}
}

void OrderdContures2D::handleOthers()
{
	std::size_t nodeIdx = 0;
	for(bool b : handeldPoints)
	{
		if(!b)
			handelLineSegment(nodeIdx);
		++nodeIdx;
	}
}




OrderdContures2D::OrderdContures2D(const Conture2D& conture)
: lines (conture.getLines ())
, points(conture.getPoints())
{
	handeldPoints.resize(points.size());
	handeldLines .resize(lines .size());

	handleOpenEnds();
	handleOthers();
}
