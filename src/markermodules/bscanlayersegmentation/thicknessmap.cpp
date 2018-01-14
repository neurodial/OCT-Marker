#include "thicknessmap.h"

#include <octdata/datastruct/series.h>
#include <octdata/datastruct/bscan.h>
#include <octdata/datastruct/sloimage.h>

#include<opencv/cv.hpp>

#include<limits>

using Segmentline         = OctData::Segmentationlines::Segmentline;
using SegmentlineDataType = OctData::Segmentationlines::SegmentlineDataType;


namespace
{

	class CreateThicknessMap
	{
		SegmentlineDataType minValue =  std::numeric_limits<SegmentlineDataType>::infinity();
		SegmentlineDataType maxValue = -std::numeric_limits<SegmentlineDataType>::infinity();

		cv::Mat rawImage;
		cv::Mat mask;
		cv::Mat thickness;

		OctData::ScaleFactor    factor   ;
		OctData::CoordSLOpx     shift    ;
		OctData::CoordTransform transform;

		void createMap()
		{
			cv::Mat convertImage;

			const double scaleFactor = std::numeric_limits<uint8_t>::max()/(maxValue - minValue);
			const double shift       = minValue;
			rawImage.convertTo(convertImage, cv::DataType<uint8_t>::type, scaleFactor, shift);

			const double inpaintRadius = 3; // Radius of a circular neighborhood of each point inpainted that is considered by the algorithm.
			inpaint(convertImage, mask, thickness, inpaintRadius, cv::INPAINT_TELEA); // cv::INPAINT_NS
		}

		void addPixelValue(const OctData::CoordSLOpx& coord, SegmentlineDataType value1, SegmentlineDataType value2)
		{
			if(value1 < 0 || value1 > 10000
			|| value2 < 0 || value2 > 10000)
				return;

			double thickness = (value1 - value2); // TODO: ScaleFactor

			const int x = coord.getX();
			const int y = coord.getY();

			if(x<0 || y<0 || x>rawImage.cols || y>rawImage.rows)
				return;

			if(thickness < 0)
				thickness = -thickness;

			rawImage.at<SegmentlineDataType>(x,y) = thickness;
			mask.at<uint8_t>(x,y) = 0;


			if(thickness < minValue) minValue = thickness;
			if(thickness > maxValue) maxValue = thickness;
		}

		void addLineScan(const OctData::BScan& bscan, const Segmentline& segLine1, const Segmentline& segLine2)
		{
			const OctData::CoordSLOpx& start_px = (transform * bscan.getStart())*factor + shift;
			const OctData::CoordSLOpx&   end_px = (transform * bscan.getEnd()  )*factor + shift;

			const std::size_t bscanWidth = static_cast<std::size_t>(bscan.getWidth());

			if(bscanWidth < 2)
				return;

			for(std::size_t i=0; i<bscanWidth; ++i)
			{
				const double v = static_cast<double>(i)/static_cast<double>(bscanWidth-1);
				const OctData::CoordSLOpx actPos = start_px*v + end_px*(1-v);
				addPixelValue(actPos, segLine2[i], segLine1[i]);
			}
		}


		void addCircleScan(const OctData::BScan& bscan, const Segmentline& segLine1, const Segmentline& segLine2)
		{
		}


		void addBScan(const OctData::BScan& bscan, const Segmentline& segLine1, const Segmentline& segLine2)
		{
			const std::size_t bscanWidth = static_cast<std::size_t>(bscan.getWidth());
			if(bscanWidth != segLine1.size() || bscanWidth != segLine2.size())
				return;

			switch(bscan.getBScanType())
			{
				case OctData::BScan::BScanType::Line:
					addLineScan(bscan, segLine1, segLine2);
					break;
				case OctData::BScan::BScanType::Circle:
					addCircleScan(bscan, segLine1, segLine2);
					break;
			}
		}

	public:
		CreateThicknessMap(const OctData::Series* series
		                 , const std::vector<BScanLayerSegmentation::BScanSegData>& lines
		                 , OctData::Segmentationlines::SegmentlineType t1
		                 , OctData::Segmentationlines::SegmentlineType t2)
		{
			if(!series)
				return;

// 			const OctData::Series::BScanSLOCoordList& convexHull = series->getConvexHull();
			const OctData::SloImage& sloImage = series->getSloImage();

			cv::Mat sloImageMat = sloImage.getImage();
			if(sloImageMat.empty())
				return;

			rawImage.create(sloImageMat.size(), cv::DataType<double >::type);
			mask    .create(sloImageMat.size(), cv::DataType<uint8_t>::type);

			mask = 255;

			factor    = sloImage.getScaleFactor();
			shift     = sloImage.getShift()      ;
			transform = sloImage.getTransform()  ;

			if(lines.size() < series->bscanCount())
				return;

			std::size_t bscanNr = 0;
			for(const OctData::BScan* bscan : series->getBScans())
			{
				if(!bscan)
					continue;

				const BScanLayerSegmentation::BScanSegData& data = lines[bscanNr];
				const Segmentline& segLine1 = data.lines.getSegmentLine(t1);
				const Segmentline& segLine2 = data.lines.getSegmentLine(t2);

				addBScan(*bscan, segLine1, segLine2);

				++bscanNr;
			}


// 			imshow("mask", mask);
// 			imshow("raw", rawImage);
//
// 			cv::Mat convertImage;
// 			const double scaleFactor = std::numeric_limits<uint8_t>::max()/(maxValue - minValue);
// 			const double shift       = -minValue*scaleFactor;
// 			rawImage.convertTo(convertImage, cv::DataType<uint8_t>::type, scaleFactor, shift);
// 			imshow("convertImage", convertImage);

		}


		const cv::Mat& getThicknessMap() { if(thickness.empty()) createMap(); return thickness; }
	};


}




void ThicknessMap::createMap(const OctData::Series* series
                           , const std::vector<BScanLayerSegmentation::BScanSegData>& lines
                           , OctData::Segmentationlines::SegmentlineType t1
                           , OctData::Segmentationlines::SegmentlineType t2)
{
	CreateThicknessMap thicknessMap(series, lines, t1, t2);



	imshow("Thicknessmap", thicknessMap.getThicknessMap());

}
