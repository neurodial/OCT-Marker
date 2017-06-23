#include"point2d.h"

#include<cmath>

double Point2D::euklidDist(const Point2D& p) const
{
	double dx = x - p.x;
	double dy = y - p.y;
	return std::sqrt(dx*dx + dy*dy);
}

