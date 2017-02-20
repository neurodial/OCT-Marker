#ifndef SIMPLECVMATCOMPRESS_H
#define SIMPLECVMATCOMPRESS_H

#include "simplematcompress.h"

namespace cv { class Mat; }

class SimpleCvMatCompress : public SimpleMatCompress
{
public:
	void readFromMat(const cv::Mat& mat);

	void writeToMat(cv::Mat& mat) const;

	bool operator==(const cv::Mat& mat) const;
	bool operator!=(const cv::Mat& mat) const                       { return !(this->operator==(mat)); }

};

#endif // SIMPLECVMATCOMPRESS_H
