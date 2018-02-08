#pragma once

#include<cstdint>

#include<helper/convertcolorspace.h>

class ColormapHSV
{
	double minValue = 100;
	double maxValue = 500;
	double fadeIn  = 0.15;
	double fadeOut = 0.35;

	const double maxHue = 300;
	const double minHue = 0;
	const double fadeInHue = 0;
	const double fadeOutHue = 260;
public:


	double getMaxValue() const { return 750; }

	void getColor(double value, uint8_t& r, uint8_t& g, uint8_t& b)
	{
// 		if(value < minValue)
// 		{
// 			r = g = b = 0;
// 		}
// 		else if(value > maxValue)
// 		{
// 			r = g = b = 255;
// 		}
// 		else
		{
			double relValue   = 1.-(value-minValue)/(maxValue-minValue);
			double saturation = 1; // relValue>0.8?(0.8-relValue)*5:1;
			double value      = 1; // relValue<0.2?(relValue*5):1;
			double hue        = relValue*360;

			if(hue < fadeInHue)
			{
				saturation = 1+(hue-fadeInHue)/360./fadeIn;
				if(saturation < 0) saturation = 0;
			}

			if(hue > fadeOutHue)
			{
				value = 1-(hue-fadeOutHue)/360./fadeOut;
				if(value < 0) value = 0;
			}

			if(hue > maxHue)
				hue = maxHue;
			else if(hue < minHue)
				hue = 0;


			double rd, gd, bd;
			std::tie(rd, gd, bd) = hsv2rgb(hue, saturation, value);

			r = static_cast<uint8_t>(rd*255);
			g = static_cast<uint8_t>(gd*255);
			b = static_cast<uint8_t>(bd*255);
		}
	}

};
