#pragma once

class ScaleFactor
{
	double x      = 1;
	double y      = 1;
	double factor = 1;
public:
	ScaleFactor(double x, double y) : x(x), y(y) {}
	ScaleFactor() = default;

	double getFactorX() const                                       { return x*factor; }
	double getFactorY() const                                       { return y*factor; }

	double getPureFactorX() const                                   { return x; }
	double getPureFactorY() const                                   { return y; }

	void setFactorX(double v)                                       { x = v; }
	void setFactorY(double v)                                       { y = v; }
	void setFactor(double v)                                        { factor = v; }


	bool isIdentical() const                                        { return x == 1.0 && y == 1.0 && factor == 1.0; }
	bool isValid()     const                                        { return x  > 0.0 && y  > 0.0 && factor  > 0.0; }
};
