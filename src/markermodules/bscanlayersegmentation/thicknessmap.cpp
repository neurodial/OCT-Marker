#include "thicknessmap.h"

#include <map>

#include <octdata/datastruct/series.h>
#include <octdata/datastruct/bscan.h>
#include <octdata/datastruct/sloimage.h>

#include<data_structure/matrx.h>
#include<data_structure/point2d.h>

#include<opencv/cv.hpp>

#include<limits>

using Segmentline         = OctData::Segmentationlines::Segmentline;
using SegmentlineDataType = OctData::Segmentationlines::SegmentlineDataType;



#include<iostream> // TODO


namespace
{

	class CreateThicknessMap
	{

		struct PixelInfo
		{
			enum class Status : uint8_t { FAR_AWAY, ACCEPTED, TRAIL };

			double distance = 0;
			double value    = 0;
			Status status = Status::FAR_AWAY;

// 			bool setInitialDistance(double dis, TrailMap& map, const VoxelElement& pix);
// 			void setTrailDistance(double dis, TrailMap& map, const VoxelElement& pos);
//
// 			void print(std::ostream& stream) const;
// 			static void printStatus(std::ostream& stream, Status st);
		};

		typedef Matrix<PixelInfo> PixelMap;
		typedef std::multimap<double, PixtureElement> TrailMap;


		PixelMap* pixelMap = nullptr;
		TrailMap  trailMap;



		SegmentlineDataType minValue =  std::numeric_limits<SegmentlineDataType>::infinity();
		SegmentlineDataType maxValue = -std::numeric_limits<SegmentlineDataType>::infinity();

// 		cv::Mat rawImage;
// 		cv::Mat mask;
		cv::Mat thicknessImage;
		bool thicknessImageCreated = false;

		OctData::ScaleFactor    factor   ;
		OctData::CoordSLOpx     shift    ;
		OctData::CoordTransform transform;

		void addTrail(std::size_t x, std::size_t y, double distance, double thickness)
		{
			if(x >= pixelMap->getSizeX() || y >= pixelMap->getSizeY())
				return;

			PixelInfo& info = (*pixelMap)(x, y);
			if(info.status != PixelInfo::Status::FAR_AWAY)
				return;

			info.status   = PixelInfo::Status::TRAIL;
			info.distance = distance;
			info.value    = thickness;

			trailMap.emplace(distance, PixtureElement(x, y));
		}

		void createMap()
		{

			double distance    = 0;
			double maxDistance = 20;

			while(trailMap.size() > 0 && distance < maxDistance)
			{
				auto aktIt = trailMap.begin();

				PixtureElement& aktEle = aktIt->second;
				distance = aktIt->first;

				const std::size_t aktX = aktEle.getX();
				const std::size_t aktY = aktEle.getY();


				PixelInfo& info = (*pixelMap)(aktX, aktY);
				info.status = PixelInfo::Status::ACCEPTED;

				double nextDistance = distance + 1;
				double thickness    = info.value;
				addTrail(aktX  , aktY-1, nextDistance, thickness);
				addTrail(aktX  , aktY+1, nextDistance, thickness);
				addTrail(aktX-1, aktY  , nextDistance, thickness);
				addTrail(aktX+1, aktY  , nextDistance, thickness);

				trailMap.erase(aktIt);
			}


			for(std::size_t y = 0; y < pixelMap->getSizeY(); ++y)
			{
				uint8_t* imgIt = thicknessImage.ptr<uint8_t>(static_cast<int>(y));
				for(std::size_t x = 0; x < pixelMap->getSizeX(); ++x)
				{
					PixelInfo& info = (*pixelMap)(y, x);
					if(info.status == PixelInfo::Status::ACCEPTED)
					{
						double value = info.value;
						*imgIt = static_cast<uint8_t>((value-minValue)/(maxValue-minValue)*255);
					}
					else
						*imgIt = 0;
					++imgIt;
				}
			}

			thicknessImageCreated = true;
		}

		void addPixelValue(const OctData::CoordSLOpx& coord, SegmentlineDataType value1, SegmentlineDataType value2)
		{
			if(value1 < 0 || value1 > 10000
			|| value2 < 0 || value2 > 10000)
				return;

			double thickness = (value1 - value2); // TODO: ScaleFactor

			const std::size_t x = static_cast<std::size_t>(coord.getX());
			const std::size_t y = static_cast<std::size_t>(coord.getY());

			if(x >= pixelMap->getSizeX() || y >= pixelMap->getSizeY())
				return;

			if(thickness < 0)
				thickness = -thickness;

			PixelInfo& info = (*pixelMap)(x, y);
			info.status   = PixelInfo::Status::ACCEPTED;
			info.distance = 0;
			info.value    = thickness;
			trailMap.emplace(0, PixtureElement(x, y));

// 			std::cout << thickness << std::endl;

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
			const OctData::CoordSLOpx& start_px = (transform * bscan.getStart ())*factor + shift;
			const OctData::CoordSLOpx&   end_px = (transform * bscan.getCenter())*factor + shift;

			const std::size_t bscanWidth = static_cast<std::size_t>(bscan.getWidth());

			if(bscanWidth < 2)
				return;

			double mX = end_px.getXf();
			double mY = end_px.getYf();
			double sX = start_px.getXf() - end_px.getXf();
			double sY = start_px.getYf() - end_px.getYf();
			double r = sqrt(sX*sX + sY*sY);

			for(std::size_t i=0; i<bscanWidth; ++i)
			{
				const double v     = static_cast<double>(i)/static_cast<double>(bscanWidth-1);
				const double angle = v*2.*M_PI;

				const double x = cos(angle)*r + mX;
				const double y = sin(angle)*r + mY;
				const OctData::CoordSLOpx actPos(x, y);
				addPixelValue(actPos, segLine2[i], segLine1[i]);
			}

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

			pixelMap = new PixelMap(sloImageMat.cols, sloImageMat.rows);
			thicknessImage.create(sloImageMat.size(), cv::DataType<uint8_t>::type);

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
		}

		~CreateThicknessMap()
		{
			delete pixelMap;
		}


		const cv::Mat& getThicknessMap() { if(!thicknessImageCreated) createMap(); return thicknessImage; }
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
