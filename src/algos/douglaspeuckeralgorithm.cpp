#include "douglaspeuckeralgorithm.h"

#include<iostream>
#include<cmath>

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


void DouglasPeuckerAlgorithm::douglasPeuckerAlgorithmRecursiv(PtIt insertPointBefore, const PtItSource firstPoint, const PtItSource lastPoint)
{
	if(lastPoint == firstPoint)
		return;

	const double point1X = firstPoint->getX();
	const double point1Y = firstPoint->getY();
	const double point2Y = lastPoint->getY();
	double length = lastPoint->getX() - point1X;

	double maxDist = 0;
	std::vector<Point2D>::const_iterator maxDistIt = firstPoint;

	for(std::vector<Point2D>::const_iterator it = firstPoint; it != lastPoint; ++it)
	{
		const double linePos   = (it->getX() - point1X)/length;
		const double lineValue = (point1Y)*(1. - linePos) + (point2Y)*linePos;

		const double dist = std::abs(lineValue - it->getY());
		if(dist > maxDist)
		{
			maxDist = dist;
			maxDistIt = it;
		}
	}

	if(maxDist > tol)
		divideOnPoint(firstPoint, maxDistIt, lastPoint, insertPointBefore);
}
