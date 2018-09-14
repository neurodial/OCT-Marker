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

#include"point2d.h"

#include<cmath>
#include<data_structure/scalefactor.h>

template<typename T>
double Point2DBase<T>::euklidDist(const Point2DBase<T>& p) const
{
	double dx = x - p.x;
	double dy = y - p.y;
	return std::sqrt(dx*dx + dy*dy);
}

template<typename T>
double Point2DBase<T>::euklidDist(const Point2DBase<T>& p, const ScaleFactor& scale) const
{
	double dx = (x - p.x)*scale.getFactorX();
	double dy = (y - p.y)*scale.getFactorY();
	return std::sqrt(dx*dx + dy*dy);
}


template<typename T>
double Point2DBase<T>::length() const
{
	return std::sqrt(x*x + y*y);
}


template double Point2DBase<double>::euklidDist(const Point2DBase<double>& p, const ScaleFactor& scale) const;
template double Point2DBase<int   >::euklidDist(const Point2DBase<int   >& p, const ScaleFactor& scale) const;

template double Point2DBase<double>::euklidDist(const Point2DBase<double>& p) const;
template double Point2DBase<int   >::euklidDist(const Point2DBase<int   >& p) const;

template double Point2DBase<double>::length() const;
template double Point2DBase<int   >::length() const;
