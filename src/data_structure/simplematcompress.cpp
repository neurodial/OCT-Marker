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

