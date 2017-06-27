#ifndef FINDSUPPORTINGPOINTS_H
#define FINDSUPPORTINGPOINTS_H

#include<vector>
#include<list>

#include<data_structure/point2d.h>

class FindSupportingPoints
{
public:
	typedef std::list<Point2D>::iterator PtIt;
	typedef std::vector<Point2D>::const_iterator PtItSource;
	FindSupportingPoints(const std::vector<Point2D>& values);


	const std::list<Point2D>& getPoints() const                     { return destPoints; }

	std::vector<double> interpolated;


private:

	constexpr static const double tol = 0.2;

	void divideLocalMinMax(const PtItSource firstPoint, const PtItSource lastPoint);

	void divideOnDerivative(const std::vector<Point2D>& values);

	void divideOnPoint(const PtItSource firstPoint, const PtItSource dividePoint, const PtItSource lastPoint, PtIt insertPointBefore);
	void findSupportingPointsRecursiv(PtIt insertPointBefore, const PtItSource firstPoint, const PtItSource lastPoint);

	void updateInterpolated(); // PtIt first, PtIt last);
	std::list<Point2D> destPoints;

};

#endif // FINDSUPPORTINGPOINTS_H
