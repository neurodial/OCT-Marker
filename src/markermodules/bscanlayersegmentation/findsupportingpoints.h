#ifndef FINDSUPPORTINGPOINTS_H
#define FINDSUPPORTINGPOINTS_H

#include<vector>
#include<list>

#include<data_structure/point2d.h>

class FindSupportingPoints
{
public:
	struct DestPoint
	{
		DestPoint() = default;
		DestPoint(const Point2D& p) : point(p) {}

		double getX() const { return point.getX(); }
		double getY() const { return point.getY(); }

		Point2D point;
		bool dirty = true;
		double error = 0;
	};
	typedef std::list<DestPoint>::iterator PtIt;
	typedef std::vector<Point2D>::const_iterator PtItSource;
	FindSupportingPoints(const std::vector<Point2D>& values);


	const std::vector<Point2D> getSupportingPoints() const;


private:
	constexpr static const double tol = 0.2;

	template<typename InsertPointsFunc>
	void fillPoints(const std::vector<Point2D>& values, InsertPointsFunc fun);

	void divideLocalMinMax(const PtItSource firstPoint, const PtItSource lastPoint);

// 	void divideOnDerivative(const std::vector<Point2D>& values);
	void divideOnPoint(const PtItSource firstPoint, const PtItSource dividePoint, const PtItSource lastPoint, PtIt insertPointBefore, std::size_t depth);

	void findSupportingPointsRecursiv(PtIt insertPointBefore, const PtItSource firstPoint, const PtItSource lastPoint, std::size_t depth = 0);
	void divideOnDerivative(PtIt insertPointBefore, const PtItSource firstPoint, const PtItSource lastPoint);

	void setDirtySurrounding(PtIt pt);

	std::vector<double> calcInterpolatedWithout(PtIt it); // PtIt first, PtIt last);
	void updateInterpolated(); // PtIt first, PtIt last);
	void removePoints(const std::vector<Point2D>& values);

	void calcAndSetPointError(PtIt firstScope, PtIt point, PtIt lastScope, const std::vector<Point2D>& values);
	void updatePointsError(const std::vector<Point2D>& values);

	std::list<DestPoint> destPoints;

	std::vector<double> interpolated;
};

#endif // FINDSUPPORTINGPOINTS_H
