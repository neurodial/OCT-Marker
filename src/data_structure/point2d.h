#pragma once


class Point2D
{
	double x = 0.;
	double y = 0.;
public:
	Point2D(double x, double y) : x(x), y(y) {}
	Point2D() = default;
	Point2D(const Point2D& other) = default;


	double getX() const { return x; }
	double getY() const { return y; }
};


