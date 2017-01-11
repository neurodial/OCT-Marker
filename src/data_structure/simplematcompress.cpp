#include "simplematcompress.h"

#include <opencv/cv.h>

void SimpleMatCompress::readMat(const cv::Mat& mat)
{
	rows = mat.rows;
	cols = mat.cols;
	segmentsChange.clear();

	sumSegments = 0;

	if(mat.empty())
		return;

	int     actSegmentLengt = 0;
	uint8_t actSegmentValue = *(mat.ptr<uint8_t>());

	int rowSize = mat.rows;
	int colSize = mat.cols;

	for(int row = 0; row < rowSize; ++row)
	{
		const uint8_t* dataPtr = mat.ptr<uint8_t>(row);
		for(int col = 0; col < colSize; ++col)
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
}

void SimpleMatCompress::writeMat(cv::Mat& mat) const
{
	mat = cv::Mat(rows, cols, cv::DataType<uint8_t>::type);
	uint8_t* dataPtr = mat.ptr<uint8_t>();

	for(const MatSegment& segment : segmentsChange)
	{
		uint8_t segmentValue = segment.value;
		for(int i=0; i<segment.length; ++i, ++dataPtr)
			*dataPtr = segmentValue;
	}
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
