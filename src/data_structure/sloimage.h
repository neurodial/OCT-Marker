#ifndef SLOIMAGE_H
#define SLOIMAGE_H

#include <string>

#include "coordslo.h"

namespace cv { class Mat; }

class SLOImage
{
	cv::Mat*    image    = nullptr;
	std::string filename;

	double scaleX                  = 0.;
	double scaleY                  = 0.;

	int    numAverage              = 0 ;
	int    imageQuality            = 0 ;

public:
	SLOImage();
	~SLOImage();

	SLOImage(const SLOImage& other)            = delete;
	SLOImage& operator=(const SLOImage& other) = delete;

	const cv::Mat* getImage()           const                   { return image                  ; }

	const std::string getFilename()     const                   { return filename               ; }

	double getScaleX()                  const                   { return scaleX                 ; }
	double getScaleY()                  const                   { return scaleY                 ; }

	int    getNumAverage()              const                   { return numAverage             ; }
	int    getImageQuality()            const                   { return imageQuality           ; }

};

#endif // SLOIMAGE_H
