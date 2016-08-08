#ifndef SLOIMAGE_H
#define SLOIMAGE_H

#include <string>

#include "coordslo.h"

namespace cv { class Mat; }

class SLOImage
{
	cv::Mat*    image    = nullptr;
	std::string filename;

	ScaleFactor scaleFactor;
	CoordSLOpx  shift;

	int    numAverage              = 0 ;
	int    imageQuality            = 0 ;

public:
	SLOImage();
	~SLOImage();

	SLOImage(const SLOImage& other)            = delete;
	SLOImage& operator=(const SLOImage& other) = delete;

	const cv::Mat& getImage()                   const           { return *image                 ; }
	void           setImage(const cv::Mat& image);

	const std::string& getFilename()             const          { return filename               ; }
	void               setFilename(const std::string& s)        {        filename = s           ; }

	const ScaleFactor& getScaleFactor()         const           { return scaleFactor            ; }
	const CoordSLOpx&  getShift()               const           { return shift                  ; }
	void               setScaleFactor(const ScaleFactor& f)     { scaleFactor = f               ; }
	void               setShift      (const CoordSLOpx&  s)     { shift       = s               ; }

	int    getNumAverage()                      const           { return numAverage             ; }
	int    getImageQuality()                    const           { return imageQuality           ; }

};

#endif // SLOIMAGE_H
