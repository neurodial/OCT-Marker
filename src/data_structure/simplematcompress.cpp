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

#include "simplematcompress.h"



SimpleMatCompress::SimpleMatCompress(int rows, int cols, uint8_t initValue)
: rows(rows)
, cols(cols)
{
	addSegment(rows*cols, initValue);
}

bool SimpleMatCompress::readFromMat(const uint8_t* mat, int rows, int cols)
{
	this->rows = rows;
	this->cols = cols;
	segmentsChange.clear();

	sumSegments = 0;

	if(mat == nullptr)
		return false;

	int     actSegmentLengt = 0;
	uint8_t actSegmentValue = *mat;

	const uint8_t* dataPtr = mat;
	for(int row = 0; row < rows; ++row)
	{
		for(int col = 0; col < cols; ++col)
		{
			if(actSegmentValue != *dataPtr)
			{
				addSegment(actSegmentLengt, actSegmentValue);
				actSegmentValue = *dataPtr;
				actSegmentLengt = 1;
			}
			else
				++actSegmentLengt;

			++dataPtr;
		}
	}
	addSegment(actSegmentLengt, actSegmentValue);

	assert(sumSegments == rows*cols);
	return true;
}

bool SimpleMatCompress::writeToMat(uint8_t* mat, int rows, int cols) const
{
	if(this->rows != rows || this->cols != cols || mat == nullptr)
		return false;

	for(const MatSegment& segment : segmentsChange)
	{
		uint8_t segmentValue = segment.value;
		for(int i=0; i<segment.length; ++i, ++mat)
			*mat = segmentValue;
	}
	return true;
}

inline void SimpleMatCompress::addSegment(int length, uint8_t value)
{
	segmentsChange.push_back(MatSegment(length, value));
	sumSegments += length;
}

bool SimpleMatCompress::isEmpty(uint8_t defaultValue) const
{
	if(segmentsChange.empty())
		return true;
	else if(segmentsChange.size() == 1)
		return segmentsChange[0].value == defaultValue;
	return false;
}

bool SimpleMatCompress::isEqual(const uint8_t* mat, int rows, int cols) const
{
	if(this->rows != rows || this->cols != cols || mat == nullptr)
		return false;

	for(const MatSegment& segment : segmentsChange)
	{
		uint8_t segmentValue = segment.value;
		for(int i=0; i<segment.length; ++i, ++mat)
			if(*mat != segmentValue)
				return false;
	}
	return true;
}

bool SimpleMatCompress::operator==(const SimpleMatCompress& other) const
{
	return rows           == other.rows
	    && cols           == other.cols
	    && segmentsChange == other.segmentsChange;
}

