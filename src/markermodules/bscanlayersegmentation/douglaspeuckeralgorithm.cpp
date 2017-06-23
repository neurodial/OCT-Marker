#include "douglaspeuckeralgorithm.h"

#include<iostream>
#include<cmath>

DouglasPeuckerAlgorithm::DouglasPeuckerAlgorithm(const std::vector<Point2D>& values)
{
	if(values.size() < 2)
		return;

	std::vector<Point2D>::const_iterator firstPoint = values.begin();
	std::vector<Point2D>::const_iterator lastPoint  = --(values.end());

	destPoints.push_back(*firstPoint);
	destPoints.push_back(*lastPoint);

	DouglasPeuckerAlgorithmRecursiv(++destPoints.begin(), firstPoint, lastPoint);
}

void DouglasPeuckerAlgorithm::DouglasPeuckerAlgorithmRecursiv(std::list<Point2D>::iterator insertDpPointBefore, std::vector<Point2D>::const_iterator firstPoint, std::vector<Point2D>::const_iterator lastPoint)
{
	if(lastPoint == firstPoint)
		return;

	double length = lastPoint->getX() - firstPoint->getX();

	double maxDist = 0;
	std::vector<Point2D>::const_iterator maxDistIt = firstPoint;

	for(std::vector<Point2D>::const_iterator it = firstPoint; it != lastPoint; ++it)
	{
		const double linePos   = (it->getX() - firstPoint->getX())/length;
		const double lineValue = (firstPoint->getY())*(1. - linePos) + (lastPoint->getY())*linePos;

		const double dist = std::abs(lineValue - it->getY());
		if(dist > maxDist)
		{
			maxDist = dist;
			maxDistIt = it;
		}
	}

	if(maxDist > tol)
	{
// 		std::cout << "maxDist: " << maxDist << std::endl;
		std::list<Point2D>::iterator newPointIt = destPoints.insert(insertDpPointBefore, *maxDistIt);
		DouglasPeuckerAlgorithmRecursiv(newPointIt, firstPoint, maxDistIt);
		DouglasPeuckerAlgorithmRecursiv(insertDpPointBefore, maxDistIt, lastPoint);
	}
}
