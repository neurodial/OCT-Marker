#ifndef DOUGLASPEUCKERALGORITHM_H
#define DOUGLASPEUCKERALGORITHM_H

#include<vector>
#include<list>

#include<data_structure/point2d.h>

class DouglasPeuckerAlgorithm
{
	constexpr static const double tol = 0.5;

	void DouglasPeuckerAlgorithmRecursiv(std::list<Point2D>::iterator insertDpPointBefore, std::vector<Point2D>::const_iterator firstPoint, std::vector<Point2D>::const_iterator lastPoint);

	std::list<Point2D> destPoints;

public:
	DouglasPeuckerAlgorithm(const std::vector<Point2D>& values);


	const std::list<Point2D>& getPoints() const                     { return destPoints; }
};

#endif // DOUGLASPEUCKERALGORITHM_H
