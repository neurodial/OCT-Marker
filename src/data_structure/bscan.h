#ifndef BSCAN_H
#define BSCAN_H

#include <string>

#include "coordslo.h"

namespace cv { class Mat; }

class BScan
{
public:
	struct Data
	{
		std::string filename;

		double scaleX                  = 0.;
		double scaleY                  = 0.;

		int    numAverage              = 0 ;
		int    imageQuality            = 0 ;

		bool   positionWithinTolerance     ;
		bool   edi                         ;

		CoordSLO start;
		CoordSLO end;
	};

	cv::Mat*    image    = nullptr;
	Data        data;

public:
	// BScan();
	BScan(const cv::Mat& img, BScan::Data data);
	~BScan();

	BScan(const BScan& other)            = delete;
	BScan& operator=(const BScan& other) = delete;

	const cv::Mat& getImage()           const                   { return *image                      ; }

	const std::string getFilename()     const                   { return data.filename               ; }

	double getScaleX()                  const                   { return data.scaleX                 ; }
	double getScaleY()                  const                   { return data.scaleY                 ; }

	int    getNumAverage()              const                   { return data.numAverage             ; }
	int    getImageQuality()            const                   { return data.imageQuality           ; }

	bool   getPositionWithinTolerance() const                   { return data.positionWithinTolerance; }
	bool   getEdi()                     const                   { return data.edi                    ; }

	const CoordSLO& getStart()          const                   { return data.start                  ; }
	const CoordSLO& getEnd()            const                   { return data.end                    ; }
};


#endif // BSCAN_H
