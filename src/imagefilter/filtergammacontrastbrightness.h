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
