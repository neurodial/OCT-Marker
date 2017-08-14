#pragma once

#include<ostream>

template<typename T>
class Point2DBase
{
	T x = 0.;
	T y = 0.;
public:
	Point2DBase(T x, T y) : x(x), y(y) {}
	Point2DBase() = default;
	Point2DBase(const Point2DBase<T>& other) = default;


	T getX() const { return x; }
	T getY() const { return y; }

	void setX(T v) { x = v; }
	void setY(T v) { y = v; }

	double euklidDist(const Point2DBase<T>& p) const;

	void print(std::ostream& stream) const { stream << '(' << x << "; " << y << ')'; }
};


template<typename T>
inline std::ostream& operator<<(std::ostream& stream, const Point2DBase<T>& p) { p.print(stream); return stream; }

typedef Point2DBase<double> Point2D;
typedef Point2DBase<int> Point2DInt;

