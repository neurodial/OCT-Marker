/*
 * Copyright (c) 2018 Kay Gawlik <kaydev@amarunet.de>
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

#include "douglaspeuckeralgorithm.h"

#include<iostream>
#include<cmath>
#include<algorithm>

DouglasPeuckerAlgorithm::DouglasPeuckerAlgorithm(const std::vector<Point2D>& values, double tol)
: tol(tol)
{
	if(values.size() < 2)
		return;

	std::vector<Point2D>::const_iterator firstPoint = values.begin();
	std::vector<Point2D>::const_iterator lastPoint  = --(values.end());

	destPoints.push_back(*firstPoint);
	destPoints.push_back(*lastPoint);

	douglasPeuckerAlgorithmRecursiv(++destPoints.begin(), firstPoint, lastPoint);
}

void DouglasPeuckerAlgorithm::divideOnPoint(const PtItSource firstPoint, const PtItSource dividePoint, const PtItSource lastPoint, PtIt insertPointBefore)
{
	PtIt newPointIt = destPoints.insert(insertPointBefore, *dividePoint);
	douglasPeuckerAlgorithmRecursiv(newPointIt, firstPoint, dividePoint);
	douglasPeuckerAlgorithmRecursiv(insertPointBefore, dividePoint, lastPoint);
}

namespace
{
	inline double calcDistance(const Point2D& p1, const Point2D& p2, const Point2D& geradenvektor, const Point2D& clickPos)
	{
		const Point2D kraftvektor = clickPos-p1;
		double alpha = (kraftvektor*geradenvektor) / geradenvektor.normquadrat();
		if(alpha>0 && alpha<1)
		{
			return (geradenvektor*alpha).euklidDist(kraftvektor);
		}
		return std::min(p1.euklidDist(clickPos), p2.euklidDist(clickPos));
	}
}


void DouglasPeuckerAlgorithm::douglasPeuckerAlgorithmRecursiv(PtIt insertPointBefore, const PtItSource firstPoint, const PtItSource lastPoint)
{
	if(lastPoint == firstPoint)
		return;

	const Point2D geradenvektor = *lastPoint - *firstPoint;
	double maxDist = 0;
	std::vector<Point2D>::const_iterator maxDistIt = firstPoint;

	for(std::vector<Point2D>::const_iterator it = firstPoint+1; it != lastPoint; ++it)
	{
		const double dist = calcDistance(*firstPoint, *lastPoint, geradenvektor, *it);

		if(dist > maxDist)
		{
			maxDist = dist;
			maxDistIt = it;
		}
	}

	if(maxDist > tol)
		divideOnPoint(firstPoint, maxDistIt, lastPoint, insertPointBefore);
}
