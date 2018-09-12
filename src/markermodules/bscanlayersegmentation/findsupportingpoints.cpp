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

#include "findsupportingpoints.h"

#include<iostream>
#include<iterator>
#include<cmath>
#include "pchip.h"


FindSupportingPoints::FindSupportingPoints(const std::vector<double>& values)
{
	createRefPoints(values);

	std::size_t length = 10;

	if(refValues.size() >= 2)
		length = static_cast<std::size_t>((refValues.end()-1)->getX() + 1);

	interpolated.resize(length);
}


void FindSupportingPoints::calculateSupportingPoints()
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


	if(refValues.size() < 2)
		return;


	PtItSource firstPoint = refValues.begin();
	PtItSource lastPoint  = --(refValues.end());

	if(lastPoint->getX() < 0)
		return;

	destPoints.push_back(*firstPoint);
	divideLocalMinMax(firstPoint, lastPoint);
	destPoints.push_back(*lastPoint);



	fillPoints(CallFindSupportingPointsDerivatie(this));
	updateInterpolated();
	fillPoints(CallFindSupportingPointsRecursiv(this));

	removePoints();
}

template<typename InsertPointsFunc>
void FindSupportingPoints::fillPoints(InsertPointsFunc fun)
{
	PtIt actIt = destPoints.begin();
	++actIt;
	for(; actIt != destPoints.end(); ++actIt)
	{
		PtIt latestIt = actIt;
		--latestIt;
		PtItSource it1 = std::find_if(refValues.cbegin(), refValues.cend(), [latestIt] (const Point2D& p) { return p.getX() >= latestIt->getX(); } );
		PtItSource it2 = std::find_if(it1               , refValues.cend(), [actIt   ] (const Point2D& p) { return p.getX() >= actIt   ->getX(); } );
		if(it1 == refValues.end())
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

	struct ErrorSeglines
	{
		double maxError;
		double quadError;

		void calcError(FindSupportingPoints::PtIt it1, FindSupportingPoints::PtIt it2, const std::vector<Point2D>& values, const std::vector<double>& interpolated)
		{
			maxError  = 0;
			quadError = 0;

			FindSupportingPoints::PtItSource sourceIt1 = std::find_if(values.begin(), values.end(), [it1] (const Point2D& p) { return p.getX() >= it1->getX(); } );
			FindSupportingPoints::PtItSource sourceIt2 = std::find_if(sourceIt1     , values.end(), [it2] (const Point2D& p) { return p.getX() >= it2->getX(); } );
			if(sourceIt1 == values.end())
				return;

			double sumQuadError = 0;
			std::size_t summants = 0;
			while(sourceIt1 != sourceIt2)
			{
				std::size_t index = static_cast<std::size_t>(std::round(sourceIt1->getX()));
				if(index > interpolated.size())
					continue; // TODO: ungueltiger punkt

				const double error = std::abs(sourceIt1->getY() - interpolated[index]);
				if(error > maxError)
					maxError = error;
				sumQuadError += error * error;

				++summants;
				++sourceIt1;
			}
			quadError = sumQuadError/static_cast<double>(summants);
		}
	};
}


void FindSupportingPoints::findSupportingPointsRecursiv(PtIt insertPointBefore, const PtItSource firstPoint, const PtItSource lastPoint, std::size_t depth)
{
	if(lastPoint == firstPoint)
		return;

	if(lastPoint == firstPoint+1)
		return;

	if(depth == 15) // TODO
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

	if(maxLineDist.getDist() > conf.insertTol)
		divideOnPoint(firstPoint, maxLineDist.getIt(), lastPoint, insertPointBefore, depth + 1);
}


FindSupportingPoints::PtIt FindSupportingPoints::itPointsSave(PtIt it, int num)
{
	if(num>0)
	{
		for(int i = 0; i<num; ++i)
		{
			if(it != destPoints.end())
				++it;
			else
				break;
		}
	}
	else
	{
		for(int i = num; i<0; ++i)
		{
			--it;
			if(it == destPoints.begin())
				break;
		}
	}
	return it;
}



// -----------------
// Remove points
// -----------------

void FindSupportingPoints::setDirtySurrounding(PtIt pt)
{
	constexpr const std::size_t surroundingArea = 2;

	PtIt posIt = pt;
	for(std::size_t i = 0; i < surroundingArea; ++i)
		if(posIt != destPoints.begin())
		{
			--posIt;
			posIt->dirty = true;
		}

	PtIt negIt = pt;
	for(std::size_t i = 0; i <= surroundingArea; ++i)
		if(negIt != destPoints.end())
		{
			negIt->dirty = true;
			++negIt;
		}
}


void FindSupportingPoints::removePoints()
{
	if(destPoints.size() < 3)
		return;

	PtIt minIt = destPoints.end();
	bool pointRemoved;

	std::size_t removedPoints = 0;
	std::size_t minRemvePoints;
	if(destPoints.size() < conf.maxPoints || conf.maxPoints < 4)
		minRemvePoints = 0;
	else
		minRemvePoints = destPoints.size() - conf.maxPoints;

	const PtIt endPoint = --(destPoints.end());

	do
	{
		double minError = std::numeric_limits<double>::infinity();
		pointRemoved = false;
		updatePointsError();
		for(PtIt it = ++(destPoints.begin()); it != endPoint; ++it)
		{
// 			std::cout << it->error << std::endl;
			if(minError > it->error)
			{
				minError = it->error;
				minIt = it;
			}
		}

// 		std::cout << "minError: " << minError << " < " << conf.removeTol << std::endl;
		if(minError < conf.removeTol || removedPoints < minRemvePoints)
		{
			setDirtySurrounding(minIt);
			destPoints.erase(minIt);
			pointRemoved = true;
			++removedPoints;
		}
	} while(pointRemoved);

// 	std::cout << "Number of points: " << destPoints.size() << ", removed points: " << removedPoints << std::endl;
}

// -----------------
// Error calculation
// -----------------

void FindSupportingPoints::calcAndSetPointError(PtIt firstScope, PtIt point, PtIt lastScope)
{

// 	ErrorSeglines oldError;
	ErrorSeglines newError;

// 	oldError.calcError(firstScope, lastScope, refValues, interpolated);
	std::vector<double> newInterpolated = calcInterpolatedWithout(point, itPointsSave(firstScope, -1), itPointsSave(lastScope, 1));
	newError.calcError(firstScope, lastScope, refValues, newInterpolated);
	point->dirty = false;
	if(newError.maxError > conf.maxAbsError)
		point->error = 1000. + newError.quadError;
	else
		point->error = newError.quadError; // - oldError.maxError;

}


void FindSupportingPoints::updatePointsError()
{
	if(destPoints.size() <= 3)
	{
		for(DestPoint& point : destPoints)
			point.error = std::numeric_limits<double>::infinity();
		return;
	}

	updateInterpolated();

	PtIt first = destPoints.begin();
	PtIt act   = first;
	++act;
	PtIt last  = act;
	++last;
	++last;

	if(act->dirty)
		calcAndSetPointError(first, act, last);
	++act;
	++last;

	while(last != destPoints.end())
	{
		if(act->dirty)
			calcAndSetPointError(first, act, last);
		++first;
		++act;
		++last;
	}

	--last;
	if(act->dirty)
		calcAndSetPointError(first, act, last);

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

	if(it == lastPoint)
		return;

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

			return result;
		}

	private:
		double v1 = 0;
		double v2 = 0;
		constexpr static const double tol = 1e-3;
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
// 		std::cout << '\n' << it->getX() << " - " << value;
		if(dir.isPitchChangeAndUpdate(value))
		{
// 			std::cout << "\t*";
			if(ignoreCount > 2)
			{
				destPoints.insert(insertPointBefore, *(it+1));
				ignoreCount = 0;
			}
		}
		++ignoreCount;
	}

// 	std::cout << std::endl;
}



std::vector<double> FindSupportingPoints::calcInterpolatedWithout(PtIt it, PtIt first, PtIt last)
{
	std::vector<double> interpolatedResult;

	std::vector<Point2D> supportingPoints;
	supportingPoints.reserve(destPoints.size());

	std::transform(first, it, std::back_inserter(supportingPoints), [](const DestPoint& cls) { return cls.point; });
	if(it != last)
	{
		++it;
		std::transform(it, last, std::back_inserter(supportingPoints), [](const DestPoint& cls) { return cls.point; });
	}

	if(interpolated.size() > 100000)
		return interpolatedResult;

	PChip pchip(supportingPoints, interpolated.size());
	interpolatedResult = pchip.getValues();
	return interpolatedResult;
}


void FindSupportingPoints::updateInterpolated() // PtIt first, PtIt last)
{
	std::vector<Point2D> supportingPoints = getSupportingPoints();
	PChip pchip(supportingPoints, interpolated.size());
	interpolated = pchip.getValues();
}

std::vector<Point2D> FindSupportingPoints::getSupportingPoints() const
{
	std::vector<Point2D> supportingPoints;

	supportingPoints.reserve(destPoints.size());
	std::transform(destPoints.begin(), destPoints.end(), std::back_inserter(supportingPoints), [](const DestPoint& cls) { return cls.point; });

	return supportingPoints;
}



void FindSupportingPoints::createRefPoints(const std::vector<double>& values)
{
	for(std::size_t x = 0; x < values.size(); ++x)
	{
		double val = values[x];
		if(val < 1000 && val > 0)
		{
			refValues.push_back(Point2D(x, val));
		}
	}
}

