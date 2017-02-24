#ifndef FILTERGAMMACONTRASTBRIGHTNESS_H
#define FILTERGAMMACONTRASTBRIGHTNESS_H

#include "filterimage.h"

#include <cstdint>

class FilterGammaContrastBrightness : public FilterImage
{
public:
	struct Parameter
	{
		double gamma      = 1.;
		double contrast   = 1.;
		double brightness = 1.;

		bool operator==(const Parameter& other) const
		{
			return gamma      == other.gamma
			    && contrast   == other.contrast
			    && brightness == other.brightness;
		}
		bool operator!=(const Parameter& other) const                  { return !operator==(other); }
	};

	virtual void applyFilter(const cv::Mat& in, cv::Mat& out) const override;

	void setParameter(const Parameter& para)                        { if(parameter != para) { parameter = para; calcLut(); } }

private:
	void calcLut();

	Parameter parameter;

	unsigned char lut[256];
};

#endif // FILTERGAMMACONTRASTBRIGHTNESS_H
