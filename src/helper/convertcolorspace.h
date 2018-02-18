#pragma once

#include<tuple>



inline void hsv2rgb(double h, double s, double v, double& r, double& g, double& b)
{
	const int hi = static_cast<int>(h/60);
	const double f = h/60 - static_cast<double>(hi);

	const double p = v*(1-s);
	const double q = v*(1-s*f);
	const double t = v*(1-s*(1-f));

	switch(hi)
	{
		case 0:
		case 6:
			r = v; g = t; b = p;
			break;
		case 1:
			r = q; g = v; b = p;
			break;
		case 2:
			r = p; g = v; b = t;
			break;
		case 3:
			r = p; g = q; b = v;
			break;
		case 4:
			r = t; g = p; b = v;
			break;
		case 5:
			r = v; g = p; b = q;
			break;
		default:
			r = 0; g = 0; b = 0;
			break;
	}
}

inline std::tuple<double, double, double> hsv2rgb(double h, double s, double v)
{
	double r, g, b;
	hsv2rgb(h, s, v, r, g, b);
	return std::make_tuple(r, g, b);
}
