#ifndef BSCAN_H
#define BSCAN_H

#include <string>
#include <vector>

#include "coordslo.h"

namespace cv { class Mat; }

class BScan
{
public:
	typedef std::vector<double> SegmentLine;
	struct Data
	{
		std::string filename;

		ScaleFactor scaleFactor;

		int    numAverage              = 0 ;
		int    imageQuality            = 0 ;

		bool   positionWithinTolerance     ;
		bool   edi                         ;

		CoordSLOmm start;
		CoordSLOmm end;
		CoordSLOmm center;
	};

	// BScan();
	BScan(const cv::Mat& img, const BScan::Data& data);
	~BScan();

	BScan(const BScan& other)            = delete;
	BScan& operator=(const BScan& other) = delete;

	const cv::Mat& getImage()           const                   { return *image                      ; }
	const cv::Mat& getRawImage()        const                   { return *rawImage                   ; }
	
	void setRawImage(const cv::Mat& img);

	const std::string getFilename()     const                   { return data.filename               ; }

	int    getNumAverage()              const                   { return data.numAverage             ; }
	int    getImageQuality()            const                   { return data.imageQuality           ; }

	bool   getPositionWithinTolerance() const                   { return data.positionWithinTolerance; }
	bool   getEdi()                     const                   { return data.edi                    ; }

	const ScaleFactor& getScaleFactor() const                   { return data.scaleFactor            ; }
	const CoordSLOmm& getStart()        const                   { return data.start                  ; }
	const CoordSLOmm& getEnd()          const                   { return data.end                    ; }
	const CoordSLOmm& getCenter()       const                   { return data.center                 ; }

	std::size_t getNumSegmentLine() const                       { return seg.size()   ; }
	const SegmentLine& getSegmentLine(std::size_t index) const  { return seg.at(index); }

	void addSegLine(SegmentLine segLine)                        { seg.push_back(std::move(segLine)); }

	int   getWidth()                    const;
	int   getHeight()                   const;

private:
	cv::Mat*                                image    = nullptr;
	cv::Mat*                                rawImage = nullptr;
	Data                                    data;

	std::vector<SegmentLine>        seg;

};


#endif // BSCAN_H
