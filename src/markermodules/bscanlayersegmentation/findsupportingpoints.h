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
// 	typedef void (FindSupportingPoints::*insertPointsFunc)(PtIt, const PtItSource, const PtItSource);

	constexpr static const double tol = 0.2;

	template<typename InsertPointsFunc>
	void fillPoints(const std::vector<Point2D>& values, InsertPointsFunc fun);

	void divideLocalMinMax(const PtItSource firstPoint, const PtItSource lastPoint);

// 	void divideOnDerivative(const std::vector<Point2D>& values);
	void divideOnPoint(const PtItSource firstPoint, const PtItSource dividePoint, const PtItSource lastPoint, PtIt insertPointBefore, std::size_t depth);

	void findSupportingPointsRecursiv(PtIt insertPointBefore, const PtItSource firstPoint, const PtItSource lastPoint, std::size_t depth = 0);
	void divideOnDerivative(PtIt insertPointBefore, const PtItSource firstPoint, const PtItSource lastPoint);

	void updateInterpolated(); // PtIt first, PtIt last);
	std::list<Point2D> destPoints;

};

#endif // FINDSUPPORTINGPOINTS_H
