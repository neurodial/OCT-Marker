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

#ifndef SIMPLECVMATCOMPRESS_H
#define SIMPLECVMATCOMPRESS_H

// #include<cpp_framework/matcompress/simplematcompress.h> // TODO: opencv-abhängigkeiten stören bei mex-datei
#include"simplematcompress.h"

namespace cv { class Mat; }

class SimpleCvMatCompress : public SimpleMatCompress
{
public:
	SimpleCvMatCompress() = default;
	SimpleCvMatCompress(int rows, int cols, uint8_t initValue) : SimpleMatCompress(rows, cols, initValue) {}

	void readFromMat(const cv::Mat& mat);

	void writeToMat(cv::Mat& mat) const;

	bool operator==(const cv::Mat& mat) const;
	bool operator!=(const cv::Mat& mat) const                       { return !(this->operator==(mat)); }


	bool operator==(const SimpleCvMatCompress& mat) const           { return SimpleMatCompress::operator==(mat); }
	bool operator!=(const SimpleCvMatCompress& mat) const           { return !(this->operator==(mat)); }

};

#endif // SIMPLECVMATCOMPRESS_H
