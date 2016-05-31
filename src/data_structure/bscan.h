#ifndef BSCAN_H
#define BSCAN_H

#include <string>

#include "coordslo.h"

namespace cv { class Mat; }

class BScan
{
	cv::Mat*    image    = nullptr;
	std::string filename;

	double scaleX                  = 0.;
	double scaleY                  = 0.;

	int    numAverage              = 0 ;
	int    imageQuality            = 0 ;

	bool   positionWithinTolerance     ;
	bool   edi                         ;

	CoordSLO start;
	CoordSLO end;

public:
	BScan();
	~BScan();

	BScan(const BScan& other)            = delete;
	BScan& operator=(const BScan& other) = delete;

	const cv::Mat* getImage()           const                   { return image                  ; }

	const std::string getFilename()     const                   { return filename               ; }

	double getScaleX()                  const                   { return scaleX                 ; }
	double getScaleY()                  const                   { return scaleY                 ; }

	int    getNumAverage()              const                   { return numAverage             ; }
	int    getImageQuality()            const                   { return imageQuality           ; }

	bool   getPositionWithinTolerance() const                   { return positionWithinTolerance; }
	bool   getEdi()                     const                   { return edi                    ; }

	const CoordSLO& getStart()          const                   { return start                  ; }
	const CoordSLO& getEnd()            const                   { return end                    ; }
};


#endif // BSCAN_H
