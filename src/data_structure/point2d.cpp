#include"point2d.h"

#include<cmath>

template<typename T>
double Point2DBase<T>::euklidDist(const Point2DBase<T>& p) const
{
	double dx = x - p.x;
	double dy = y - p.y;
	return std::sqrt(dx*dx + dy*dy);
}


template double Point2DBase<double>::euklidDist(const Point2DBase<double>& p) const;
template double Point2DBase<int   >::euklidDist(const Point2DBase<int   >& p) const;

