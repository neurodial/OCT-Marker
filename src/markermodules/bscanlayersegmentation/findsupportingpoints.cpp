#include "findsupportingpoints.h"

#include<iostream>
#include<algorithm>
#include<cmath>
#include "pchip.h"


FindSupportingPoints::FindSupportingPoints(const std::vector<Point2D>& values)
{
	class CallFindSupportingPointsRecursiv
	{
		FindSupportingPoints* m;
	public:
		CallFindSupportingPointsRecursiv(FindSupportingPoints* m) : m(m) {};
		void operator()(FindSupportingPoints::PtIt insertPointBefore, const FindSupportingPoints::PtItSource firstPoint, const FindSupportingPoints::PtItSource lastPoint) { m->findSupportingPointsRecursiv(insertPointBefore, firstPoint, lastPoint); }
	};

	class CallFindSupportingPointsDerivatie
	{
		FindSupportingPoints* m;
	public:
		CallFindSupportingPointsDerivatie(FindSupportingPoints* m) : m(m) {};
		void operator()(FindSupportingPoints::PtIt insertPointBefore, const FindSupportingPoints::PtItSource firstPoint, const FindSupportingPoints::PtItSource lastPoint) { m->divideOnDerivative(insertPointBefore, firstPoint, lastPoint); }
	};


	if(values.size() < 2)
		return;


	PtItSource firstPoint = values.begin();
	PtItSource lastPoint  = --(values.end());

	if(lastPoint->getX() < 0)
		return;

	interpolated.resize(static_cast<std::size_t>(lastPoint->getX()+1));

	destPoints.push_back(*firstPoint);
// 	divideOnDerivative(values);
	divideLocalMinMax(firstPoint, lastPoint);
	destPoints.push_back(*lastPoint);



	fillPoints(values, CallFindSupportingPointsDerivatie(this));
	updateInterpolated();
	fillPoints(values, CallFindSupportingPointsRecursiv(this));
	/*
	PtIt actIt = destPoints.begin();
	++actIt;
	for(; actIt != destPoints.end(); ++actIt)
	{
		PtIt latestIt = actIt;
		--latestIt;
		PtItSource it1 = std::find_if(values.begin(), values.end(), [latestIt] (const Point2D& p) { return p.getX() >= latestIt->getX(); } );
		PtItSource it2 = std::find_if(it1           , values.end(), [actIt   ] (const Point2D& p) { return p.getX() >= actIt   ->getX(); } );
		findSupportingPointsRecursiv(actIt, it1, it2);
	}
	*/

}

template<typename InsertPointsFunc>
void FindSupportingPoints::fillPoints(const std::vector<Point2D>& values, InsertPointsFunc fun)
{
	PtIt actIt = destPoints.begin();
	++actIt;
	for(; actIt != destPoints.end(); ++actIt)
	{
		PtIt latestIt = actIt;
		--latestIt;
		PtItSource it1 = std::find_if(values.begin(), values.end(), [latestIt] (const Point2D& p) { return p.getX() >= latestIt->getX(); } );
		PtItSource it2 = std::find_if(it1           , values.end(), [actIt   ] (const Point2D& p) { return p.getX() >= actIt   ->getX(); } );
		if(it1 == values.end())
			return;

		fun(actIt, it1, it2);
	}
}


void FindSupportingPoints::divideOnPoint(const PtItSource firstPoint, const PtItSource dividePoint, const PtItSource lastPoint, PtIt insertPointBefore, std::size_t depth)
{
	if(firstPoint == dividePoint || lastPoint == dividePoint)
		return;

	PtIt newPointIt = destPoints.insert(insertPointBefore, *dividePoint);
	updateInterpolated();

	findSupportingPointsRecursiv(newPointIt       , firstPoint , dividePoint, depth);
	findSupportingPointsRecursiv(insertPointBefore, dividePoint, lastPoint  , depth);
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


void FindSupportingPoints::findSupportingPointsRecursiv(PtIt insertPointBefore, const PtItSource firstPoint, const PtItSource lastPoint, std::size_t depth)
{
	if(lastPoint == firstPoint)
		return;

	if(lastPoint == firstPoint+1)
		return;

	if(depth == 1)
		return;

// 	const double point1X = firstPoint->getX();
// 	const double point1Y = firstPoint->getY();
// 	const double point2Y = lastPoint ->getY();
//
// 	double length = lastPoint->getX() - point1X;
	PtItSource it = firstPoint;
	++it;

	MaxValuePt maxLineDist(it);
	for(; it != lastPoint; ++it)
	{
// 		const double linePos   = (it->getX() - point1X)/length;
// 		const double lineValue = (point1Y)*(1. - linePos) + (point2Y)*linePos;

// 		const double dist = std::abs(lineValue - it->getX());
		std::size_t pos = static_cast<std::size_t>(it->getX());
		double v1 = it->getY();
		double v2 = interpolated[pos];
		const double dist = std::abs(v1 - v2);

		maxLineDist.updateDist(it, dist);
	}

	if(maxLineDist.getDist() > tol)
		divideOnPoint(firstPoint, maxLineDist.getIt(), lastPoint, insertPointBefore, depth + 1);
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
			if(lastVal > val + tol)
				direction = Direction::Down;
			else if(lastVal < val - tol)
				direction = Direction::Up;

			bool result = (direction != lastDirection);
			lastVal = val;
			lastDirection = direction;

			return result;
		}

	private:
		double lastVal = 0;
		constexpr static const double tol = 0; // 1e-2;
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
	double derivative2(FindSupportingPoints::PtItSource p)
	{
		double res = p->getY();
		++p;
		res -= 2*p->getY();
		++p;
		res += p->getY();
		return res;
	}
}


namespace
{
	class PitchHelper
	{
	public:
// 		enum class Direction { Up, None, Down };
		PitchHelper() {}

		bool isPitchChangeAndUpdate(double val)
		{
			double expectVal = 2*v2 - v1;
			bool result = std::abs(expectVal - val) > tol ? true : false;

			v1 = v2;
			v2 = val;

// 			std::cout << val << " - " << expectVal << " - " << std::abs(expectVal - val) << std::endl;

			return result;
		}

	private:
		double v1 = 0;
		double v2 = 0;
		constexpr static const double tol = 1e-2;
// 		Direction lastDirection = Direction::None;

	};
}

void FindSupportingPoints::divideOnDerivative(PtIt insertPointBefore, const PtItSource firstPoint, const PtItSource lastPoint)
{
	if(lastPoint - firstPoint < 4)
		return;

	int ignoreCount = 0;

	const PtItSource endPoint = lastPoint-2;
	PitchHelper dir;

	PtItSource it = firstPoint;

	for(; it != endPoint; ++it)
	{
		double value = derivative2(it);
		if(dir.isPitchChangeAndUpdate(value))
		{
// 			std::cout << "-- ";
			if(ignoreCount > 2)
			{
				if(value > 0)
					destPoints.insert(insertPointBefore, *(it+1));
				else
					destPoints.insert(insertPointBefore, *(it-1));

			}
			ignoreCount = 0;
		}
		++ignoreCount;
// 		std::cout << value << std::endl;
	}
}

/*
void FindSupportingPoints::divideOnDerivative(PtIt insertPointBefore, const PtItSource firstPoint, const PtItSource lastPoint)
{
	if(lastPoint - firstPoint < 4)
		return;

	int ignoreCount = 0;

	const PtItSource endPoint = lastPoint-2;
	DirectionHelper dir(derivative2(firstPoint));

	PtItSource it = firstPoint+1;

	for(; it != endPoint; ++it)
	{
		double value = derivative2(it);
		if(dir.isDirectionChangeAndUpdate(value))
		{
			std::cout << "-- ";
			if(ignoreCount > 2)
			{
				if(value > 0)
					destPoints.insert(insertPointBefore, *(it+1));
				else
					destPoints.insert(insertPointBefore, *(it-1));

			}
			ignoreCount = 0;
		}
		++ignoreCount;
		std::cout << value << std::endl;
	}
}
*/

/*
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
*/


void FindSupportingPoints::updateInterpolated() // PtIt first, PtIt last)
{
	std::vector<Point2D> supportingPoints;
// 	std::copy(first, last,  std::back_inserter(supportingPoints));
	std::copy(destPoints.begin(), destPoints.end(),  std::back_inserter(supportingPoints));

	PChip pchip(supportingPoints, interpolated.size());

	interpolated = pchip.getValues();
}

