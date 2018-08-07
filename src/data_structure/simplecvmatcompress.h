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
