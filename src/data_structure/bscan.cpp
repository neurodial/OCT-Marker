#include "bscan.h"


#include <opencv/cv.hpp>

BScan::BScan(const cv::Mat& img, const BScan::Data& data)
: image(new cv::Mat(img))
, data(data)
{

}

BScan::~BScan()
{
	delete image;
}
