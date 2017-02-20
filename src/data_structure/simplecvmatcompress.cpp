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

