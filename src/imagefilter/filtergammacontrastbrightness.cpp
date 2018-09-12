/*
 * Copyright (c) 2018 Kay Gawlik <kaydev@amarunet.de> <kay.gawlik@beuth-hochschule.de> <kay.gawlik@charite.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "filtergammacontrastbrightness.h"

#include <opencv/cv.h>
#include <cmath>

#include <iostream>

void FilterGammaContrastBrightness::applyFilter(const cv::Mat& in, cv::Mat& out) const
{
	if(in.type() != cv::DataType<uint8_t>::type || !in.isContinuous())
	{
		out = in;
		return;
	}

	if(&out != &in)
	{
		if(out.type() != cv::DataType<uint8_t>::type || out.rows != in.rows || out.cols != in.cols || out.channels() != in.channels())
			out.create(in.rows, in.cols, CV_MAKETYPE(cv::DataType<uint8_t>::type, in.channels()));
	}

	const uint8_t* dataIn  = in .ptr<uint8_t>();
	      uint8_t* dataOut = out.ptr<uint8_t>();

	for(std::size_t i = 0; i < in.rows*in.cols*in.channels(); ++i)
	{
		*dataOut = lut[*dataIn];
		++dataIn;
		++dataOut;
	}
}


void FilterGammaContrastBrightness::calcLut()
{
	const double gamma      = parameter.gamma     ;
	const double contrast   = parameter.contrast  ;
	const double brightness = parameter.brightness;

	for (int i = 0; i < sizeof(lut)/sizeof(lut[0]); i++)
	{
		const double gammaValue = std::pow((i / 255.0), gamma);
		lut[i] = cv::saturate_cast<uchar>((gammaValue*contrast + brightness)*255.0);
	}

	parameterChanged();
}
