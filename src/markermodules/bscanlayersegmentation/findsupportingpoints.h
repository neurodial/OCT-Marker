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

		double getX()                                            const { return point.getX(); }
		double getY()                                            const { return point.getY(); }

		Point2D point;
		bool    dirty = true;
		double  error = 0;
	};

	struct Config
	{
		double insertTol   = 0.2;
		double removeTol   = 0.4;
		double maxAbsError = 0.4;
		std::size_t maxPoints = 40;
	};

	typedef std::vector<Point2D> PtSource;

	typedef std::list<DestPoint>::iterator PtIt;
	typedef PtSource::const_iterator PtItSource;

	typedef std::vector<Point2D>::const_iterator SupportPtItIn;


// 	FindSupportingPoints(const std::vector<Point2D>& values);
	FindSupportingPoints(const std::vector<double >& values);
	FindSupportingPoints(const std::vector<double >& values, SupportPtItIn it1, SupportPtItIn it2);

	void removePoints();

	void calculateSupportingPoints();

	std::vector<Point2D> getSupportingPoints() const;


	void setConfig(const Config& config)                            { conf = config; }

private:
// 	constexpr static const double tol = 0.2;

	template<typename InsertPointsFunc>
	void fillPoints(InsertPointsFunc fun);

	void divideLocalMinMax(const PtItSource firstPoint, const PtItSource lastPoint);

// 	void divideOnDerivative(const std::vector<Point2D>& values);
	void divideOnPoint(const PtItSource firstPoint, const PtItSource dividePoint, const PtItSource lastPoint, PtIt insertPointBefore, std::size_t depth);

	void findSupportingPointsRecursiv(PtIt insertPointBefore, const PtItSource firstPoint, const PtItSource lastPoint, std::size_t depth = 0);
	void divideOnDerivative          (PtIt insertPointBefore, const PtItSource firstPoint, const PtItSource lastPoint);

	void setDirtySurrounding(PtIt pt);

	std::vector<double> calcInterpolatedWithout(PtIt it, PtIt first, PtIt last);
	void updateInterpolated(); // PtIt first, PtIt last);

	void calcAndSetPointError(PtIt firstScope, PtIt point, PtIt lastScope);
	void updatePointsError();

	void createRefPoints(const std::vector<double>& values);


	std::list<DestPoint> destPoints;
	PtSource refValues;
	std::vector<double> interpolated;

	PtIt itPointsSave(PtIt it, int num = 1);


	Config conf;
};

#endif // FINDSUPPORTINGPOINTS_H
