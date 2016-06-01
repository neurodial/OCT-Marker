#include "bscan.h"


#include <opencv/cv.hpp>

BScan::BScan(const cv::Mat& img, BScan::Data data)
: image(new cv::Mat(img))
, data(data)
{

}

BScan::~BScan()
{
	delete image;
}
