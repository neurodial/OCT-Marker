/*
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

#include "pchip.h"

#include<tuple>
#include<cmath>
#include<cassert>
#include<limits>
#include<iostream>

/*
 * References
 * [1] F. N. Fritsch and R. E. Carlson, Monotone Piecewise Cubic Interpolation,
 *     SIAM Journal on Numerical Analysis, 17 (1980), pp. 238-246.
 * [2] D. Kahaner, C. Moler, and S. Nash, Numerical Methods and Software,
 *     Prentice–Hall, Englewood Cliffs, NJ, 1989.
*/

namespace
{
	std::tuple<std::vector<double>, std::vector<double>> diff(const std::vector<Point2D>& points)
	{
		std::vector<double> resultX;
		std::vector<double> resultY;

		      std::vector<Point2D>::const_iterator it    = points.begin();
		const std::vector<Point2D>::const_iterator itEnd = points.end();

		Point2D lastPoint = *it;
		++it;

		while(it != itEnd)
		{
			resultX.push_back(it->getX() - lastPoint.getX());
			resultY.push_back(it->getY() - lastPoint.getY());
			lastPoint = *it;
			++it;
		}

		return std::make_tuple(resultX, resultY);
	}

	double pchipendpoint(double h1, double h2, double del1, double del2)
	{
	// Noncentered, shape-preserving, three-point formula.
		double d = ((2*h1+h2)*del1 - h1*del2)/(h1+h2);
		if(d*del1 < 0)
			d = 0;
		else if(del1*del2 <1 && std::abs(d) > std::abs(3*del1))
			d = 3*del1;
		return d;
	}

	std::vector<double> pchipslopes(const std::vector<double>& h, const std::vector<double>& delta)
	{
		const std::size_t n = h.size();
		std::vector<double> d;
		d.resize(n+1);

		for(std::size_t k = 1; k < n; ++k)
		{
			const double lastValue = delta[k-1];
			const double actValue  = delta[k];
			if(lastValue*actValue > 0.)
			{
				double w1 = 2*h[k]+  h[k-1];
				double w2 =   h[k]+2*h[k-1];
				d[k] = (w1+w2)/(w1/lastValue + w2/actValue);
			}
		}

		// Slopes at endpoints
		d[0  ] = pchipendpoint(h[0  ], h[1  ], delta[0  ], delta[1  ]);
		d[n  ] = pchipendpoint(h[n-1], h[n-2], delta[n-1], delta[n-2]);
		return d;
	}
}


PChip::PChip(const std::vector<Point2D>& points, std::size_t length)
{
// 	values.clear();
	values.assign(length, std::numeric_limits<double>::quiet_NaN());

	if(points.size() < 2 || length == 0)
		return;

	// First derivatives
	std::vector<double> h;
	std::vector<double> delta;
	std::tie(h, delta) = diff(points);

	for(std::size_t i = 0; i < h.size(); ++i)
		delta[i] /= h[i];

	std::vector<double> d = pchipslopes(h, delta);

	const std::size_t len = points.size();
	assert(len - 1 == delta.size());

	std::size_t nextPointIndex = 0;
	std::size_t actPointIndex = 0;
	double pointIntervalUntil = 0;
	double pointYValue = 0;
	double pointXValue = 0;

	double c;
	double b;
/*
	std::cout << "d.size()     : " << d.size() << std::endl;
	std::cout << "delta.size() : " << delta.size() << std::endl;
	std::cout << "points.size(): " << points.size() << std::endl;*/

	std::size_t firstPos = points[0].getX()>0?static_cast<std::size_t>(points[0].getX()):0;
	std::size_t lastPos  = static_cast<std::size_t>(points[points.size()-1].getX());

	if(lastPos > length)
		lastPos = length;

	for(std::size_t actPos = firstPos; actPos <= lastPos; ++actPos)
	{
		// Piecewise polynomial coefficients
		if(actPos >= static_cast<std::size_t>(pointIntervalUntil))
		{
			actPointIndex = nextPointIndex;
			++nextPointIndex;
			double hVal = h[actPointIndex];
			c = ( 3*delta[actPointIndex] - 2*d[actPointIndex] - d[actPointIndex+1])/hVal;
			b = (-2*delta[actPointIndex] +   d[actPointIndex] + d[actPointIndex+1])/(hVal*hVal);
			pointYValue        = points[actPointIndex].getY();
			pointXValue        = points[actPointIndex].getX();
			if(nextPointIndex < points.size()-1)
				pointIntervalUntil = points[nextPointIndex].getX();
			else
				pointIntervalUntil = static_cast<double>(length);
		}

		// Evaluate interpolant
		double s = static_cast<double>(actPos) - pointXValue;
		values[actPos] = pointYValue + s*(d[actPointIndex] + s*(c + s*b));
	}
}
