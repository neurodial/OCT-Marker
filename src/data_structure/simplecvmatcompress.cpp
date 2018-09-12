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

#include "simplecvmatcompress.h"

#include <opencv/cv.h>

void SimpleCvMatCompress::readFromMat(const cv::Mat& mat)
{
	// TODO: check datatype
	SimpleMatCompress::readFromMat(mat.ptr<uint8_t>(), mat.rows, mat.cols);
}


void SimpleCvMatCompress::writeToMat(cv::Mat& mat) const
{
	mat.create(getRows(), getCols(), cv::DataType<uint8_t>::type);
	// TODO: check datatype
	SimpleMatCompress::writeToMat(mat.ptr<uint8_t>(), mat.rows, mat.cols);
}


bool SimpleCvMatCompress::operator==(const cv::Mat& mat) const
{
	return SimpleMatCompress::isEqual(mat.ptr<uint8_t>(), mat.rows, mat.cols);
}

