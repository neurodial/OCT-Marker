#ifndef DOUGLASPEUCKERALGORITHM_H
#define DOUGLASPEUCKERALGORITHM_H

#include<vector>
#include<list>

#include<data_structure/point2d.h>

class DouglasPeuckerAlgorithm
{
	typedef std::list<Point2D>::iterator PtIt;
	typedef std::vector<Point2D>::const_iterator PtItSource;

	const double tol = 0.5;

	void divideOnPoint(const PtItSource firstPoint, const PtItSource dividePoint, const PtItSource lastPoint, PtIt insertPointBefore);
	void douglasPeuckerAlgorithmRecursiv(PtIt insertPointBefore, const PtItSource firstPoint, const PtItSource lastPoint);

	std::list<Point2D> destPoints;

public:
	DouglasPeuckerAlgorithm(const std::vector<Point2D>& values, double tol);


	const std::list<Point2D>& getPoints() const                     { return destPoints; }
};

#endif // DOUGLASPEUCKERALGORITHM_H
