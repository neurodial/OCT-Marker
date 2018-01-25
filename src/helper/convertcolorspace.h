#pragma once

#include<tuple>

/**
* @brief convert hsv color to rgb color
*
* @param h hue [0,360]
* @param s saturation [0,1]
* @param v value [0,1]
* @return std::tuple< double, double, double > rgb-value [0,1]
*/
std::tuple<double, double, double> hsv2rgb(double h, double s, double v)
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
			return std::make_tuple(v, t, p);
		case 1:
			return std::make_tuple(q, v, p);
		case 2:
			return std::make_tuple(p, v, t);
		case 3:
			return std::make_tuple(p, q, v);
		case 4:
			return std::make_tuple(t, p, v);
		case 5:
			return std::make_tuple(v, p, q);
	}

	return std::make_tuple(0, 0, 0);
}
