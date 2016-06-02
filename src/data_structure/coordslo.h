#ifndef COORDSLO_H
#define COORDSLO_H


class ScaleFactor
{
	double x;
	double y;

public:
	ScaleFactor(double x, double y) : x(x), y(y)                {}
	ScaleFactor() : x(0), y(0)                                  {}

	double getX() const                                         { return x; }
	double getY() const                                         { return y; }

	ScaleFactor  operator* (double factor) const                { return ScaleFactor(x*factor, y*factor); }
	ScaleFactor& operator*=(double factor)                      { x *= factor; y *= factor; return *this; }
};

class CoordSLOpx
{
	int x;
	int y;

public:
	CoordSLOpx(double x, double y) : x(x), y(y)                 {}
	CoordSLOpx() : x(0), y(0)                                   {}

	int getX() const                                            { return x; }
	int getY() const                                            { return y; }
};


class CoordSLOmm
{
	double x;
	double y;

public:
	CoordSLOmm(double x, double y) : x(x), y(y)                 {}
	CoordSLOmm() : x(0), y(0)                                   {}

	double getX() const                                         { return x; }
	double getY() const                                         { return y; }

	CoordSLOpx operator*(const ScaleFactor& factor) const       { return CoordSLOpx(x/factor.getX(), y/factor.getY()); }
};




#endif // COORDSLO_H
