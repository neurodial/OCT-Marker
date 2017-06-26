#include "findsupportingpoints.h"

#include<iostream>
#include<cmath>

FindSupportingPoints::FindSupportingPoints(const std::vector<Point2D>& values)
{
	if(values.size() < 2)
		return;

	std::vector<Point2D>::const_iterator firstPoint = values.begin();
	std::vector<Point2D>::const_iterator lastPoint  = --(values.end());

	destPoints.push_back(*firstPoint);
// 	divideOnDerivative(values);
	divideLocalMinMax(firstPoint, lastPoint);
	destPoints.push_back(*lastPoint);

// 	findSupportingPointsRecursiv(++destPoints.begin(), firstPoint, lastPoint);
}

void FindSupportingPoints::divideOnPoint(const PtItSource firstPoint, const PtItSource dividePoint, const PtItSource lastPoint, PtIt insertPointBefore)
{
	PtIt newPointIt = destPoints.insert(insertPointBefore, *dividePoint);
	findSupportingPointsRecursiv(newPointIt, firstPoint, dividePoint);
	findSupportingPointsRecursiv(insertPointBefore, dividePoint, lastPoint);
}


namespace
{
	class MaxValuePt
	{
		FindSupportingPoints::PtItSource it;
		double dist;
	public:
		MaxValuePt(FindSupportingPoints::PtItSource it, double dist = 0) : it(it), dist(dist) {}
		bool updateDist(FindSupportingPoints::PtItSource upIt, double upDist)
		{
			if(dist < upDist)
			{
				dist = upDist;
				it   = upIt;
				return true;
			}
			return false;
		}
		double                           getDist() const               { return dist; }
		FindSupportingPoints::PtItSource getIt  () const               { return it  ; }
	};
}


void FindSupportingPoints::findSupportingPointsRecursiv(PtIt insertPointBefore, const PtItSource firstPoint, const PtItSource lastPoint)
{
	if(lastPoint == firstPoint)
		return;

	const double point1X = firstPoint->getX();
	const double point1Y = firstPoint->getY();
	const double point2Y = lastPoint ->getY();

	double length = lastPoint->getX() - point1X;

	MaxValuePt maxLineDist(firstPoint);
	MaxValuePt maxHeightPt(firstPoint, point1Y);
	MaxValuePt minHeightPt(firstPoint, point2Y);

	maxHeightPt.updateDist(lastPoint, point2Y);


	for(PtItSource it = firstPoint; it != lastPoint; ++it)
	{
		const double linePos   = (it->getX() - point1X)/length;
		const double lineValue = (point1Y)*(1. - linePos) + (point2Y)*linePos;

		const double dist = std::abs(lineValue - it->getY());
		maxLineDist.updateDist(it, dist);
	}

	if(maxLineDist.getDist() > tol)
		divideOnPoint(firstPoint, maxLineDist.getIt(), lastPoint, insertPointBefore);
}

namespace
{
	class DirectionHelper
	{
	public:
		enum class Direction { Up, None, Down };
		DirectionHelper(double value) : lastVal(value) {}

		bool isDirectionChangeAndUpdate(double val)
		{
			Direction direction = Direction::None;
			if(lastVal > val)
				direction = Direction::Down;
			else if(lastVal > val)
				direction = Direction::Up;

			bool result = (direction != lastDirection);
			lastVal = val;
			lastDirection = direction;

			return result;
		}

	private:
		double lastVal = 0;
		Direction lastDirection = Direction::None;

	};
}

void FindSupportingPoints::divideLocalMinMax(const PtItSource firstPoint, const PtItSource lastPoint)
{
	DirectionHelper dir(firstPoint->getY());
	PtItSource it = firstPoint+1;
	dir.isDirectionChangeAndUpdate(it->getY());

	++it;

	for(; it != lastPoint; ++it)
	{
		if(dir.isDirectionChangeAndUpdate(it->getY()))
		{
			destPoints.push_back(*(it-1));
		}
	}
}

namespace
{
	double derivative3(FindSupportingPoints::PtItSource p)
	{
		double t = -p->getY();
		++p;
		t += 2*p->getY();
		++p;
		++p;
		t -= 2*p->getY();
		++p;
		t += p->getY();

		return t*100;
	}
}


void FindSupportingPoints::divideOnDerivative(const std::vector<Point2D>& values)
{
	if(values.size() < 7)
		return;

	      PtItSource it         = values.begin();
	const PtItSource lastPoint  = values.end()-5;

	double ablValue = derivative3(it);

	for(; it != lastPoint; ++it)
	{
		const double abl = derivative3(it);
		if(std::abs(abl - ablValue) > 1)
		{
			destPoints.push_back(*(it+2));
		}
		ablValue = abl;
		std::cout << abl << std::endl;
	}

}

