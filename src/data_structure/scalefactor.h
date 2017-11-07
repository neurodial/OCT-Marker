#pragma once

class ScaleFactor
{
	double x = 1;
	double y = 1;
	double viewFactor = 1;
public:
	ScaleFactor(double x, double y, double view) : x(x), y(y), viewFactor(view) {}
	ScaleFactor() = default;

	double getFactorX() const                                       { return x; }
	double getFactorY() const                                       { return y; }
	double getFactorView() const                                    { return viewFactor; }

	void setFactorX(double v)                                       { x = v; }
	void setFactorY(double v)                                       { y = v; }
	void setFactorView(double v)                                    { viewFactor = v; }


	bool isIdentical() const                                        { return x == 1.0 && y == 1.0; }
	bool isValid()     const                                        { return x  > 0.0 && y  > 0.0; }
};
