#include "sloimage.h"

#include <opencv/cv.hpp>


SLOImage::SLOImage()
: image(new cv::Mat)
{

}


SLOImage::~SLOImage()
{
	delete image;
}


void SLOImage::setImage(const cv::Mat& image)
{
	*(this->image) = image;
}
