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

#include "paintsegmentationtotikz.h"

#include<algos/orderdcontures2d.h>
#include<algos/douglaspeuckeralgorithm.h>


const QString PaintSegmentationToTikz::getTikzCode() const
{
	double aspectRatio = static_cast<double>(height)/static_cast<double>(width);
	double tikzFactorX = 1./static_cast<double>(width)             ;
	double tikzFactorY = 1./static_cast<double>(height)*aspectRatio;

	QString tikzCode = "\\definecolor{manualSegColor}{rgb}{0.000000,1.000000,0.000000}\n\n";

	OrderdContures2D oc2d(conture);
	const std::vector<ContureSegment>& contureSegments = oc2d.getSegments();
	for(const ContureSegment& segment : contureSegments)
	{
		tikzCode += "\\draw[manualSegColor] ";
		bool firstVal = true;
		std::size_t pos = 0;

		DouglasPeuckerAlgorithm dpa(segment.points, 1e-5);

		for(const Point2D& p : dpa.getPoints())
		{
			if(pos % 5 == 0)
				tikzCode += "\n";
			if(firstVal)
				firstVal = false;
			else
				tikzCode += " -- ";

			tikzCode += QString("(%1,%2)").arg(p.getX()*tikzFactorX).arg(aspectRatio-p.getY()*tikzFactorY);
			++pos;
		}

		if(segment.cirled)
			tikzCode += " -- cycle";
		tikzCode += ";\n\n";
	}


	return tikzCode;
}
