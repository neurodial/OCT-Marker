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

#ifndef PAINTSEGMENTATIONTOTIKZ_H
#define PAINTSEGMENTATIONTOTIKZ_H

#include<vector>
#include<QString>

#include<data_structure/conture2d.h>

#include<limits>

#include"paintsegline.h"

class QPen;

class PaintSegmentationToTikz : public PaintSegLine
{
	int width;
	int height;

	std::vector<std::size_t> pointIndexMap;
	Conture2D conture;

	std::size_t getPointIndex(const Point2D& p)
	{
		Point2D::value_type x = p.getX();
		Point2D::value_type y = p.getY();

		if(x<0 || x>width || y<0 || y>height)
			throw("invalid index");

		std::size_t index = static_cast<std::size_t>(x*2 + y*width*4);

		if(pointIndexMap[index] == std::numeric_limits<std::size_t>::max())
			pointIndexMap[index] = conture.addPoint(p);

		return pointIndexMap[index];
	}


public:
	PaintSegmentationToTikz(int width, int height)
	:width (width )
	,height(height)
	{
		pointIndexMap.assign(static_cast<std::size_t>(width*height*4), std::numeric_limits<std::size_t>::max());
	}


	void setPen(QPen&) {}

	void paintLine(const Point2D& p1, const Point2D& p2) override
	{
		try
		{
			std::size_t pIndex1 = getPointIndex(p1);
			std::size_t pIndex2 = getPointIndex(p2);

			conture.addLine(pIndex1, pIndex2);
		}
		catch(const char*)
		{}
	}


	const QString getTikzCode() const;
};


#endif // PAINTSEGMENTATIONTOTIKZ_H
