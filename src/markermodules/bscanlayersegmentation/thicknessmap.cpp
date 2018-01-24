#include "thicknessmap.h"

#include <map>

#include <octdata/datastruct/series.h>
#include <octdata/datastruct/bscan.h>
#include <octdata/datastruct/sloimage.h>

#include<data_structure/matrx.h>
#include<data_structure/point2d.h>

#include<algos/linebresenhamalgo.h>

#include<opencv/cv.hpp>

#include<limits>

using Segmentline         = OctData::Segmentationlines::Segmentline;
using SegmentlineDataType = OctData::Segmentationlines::SegmentlineDataType;



#include<iostream> // TODO
#include<opencv/highgui.h>

namespace
{

	class CreateThicknessMap
	{
		constexpr static const double maxDistance = 15;
		constexpr static const double lNorm = 1; // TODO L2 ist defekt


		struct SlideInfo
		{
			SlideInfo() = default;

			SlideInfo(double distance, double value)
			: distance(distance)
			, value   (value)
			{}

			bool operator<(const SlideInfo& info)                   const { return distance < info.distance; }
			operator bool()                                         const { return distance != std::numeric_limits<double>::infinity(); }


			double distance = std::numeric_limits<double>::infinity();
			double value    = 0;
		};

		class PixelInfo
		{
		public:
			enum class Status : uint8_t { FAR_AWAY, ACCEPTED, TRAIL, BRODER };

			double distance  = 0;
			double tempValue = 0;

// 			double weight    = 0;
// 			double value     = 0;

			bool initValue = false;

			Status status = Status::FAR_AWAY;

			SlideInfo val1;
			SlideInfo val2;

			bool setValueFinal()
			{
				SlideInfo info(distance, tempValue);
				status = Status::ACCEPTED;
				return updateValue(info);
			}

			double getMixValue() const
			{
// 				if(val1.distance == 0)
// 					return 0;

				if(!val1)
					return 0;
				if(!val2)
					return val1.value;

				double w1 = maxDistance - val1.distance;
				double w2 = maxDistance - val2.distance;

				w1 *= w1;
				w2 *= w2;

				const double sum = w1+w2;

// 				std::cout << val1.value << '\t' << val2.value << '\t';

				return (val1.value*w1 + val2.value*w2)/sum;
			}

		private:

			bool updateValue(const SlideInfo& info)
			{
				if(val2 < info)
					return false;

				val2 = info;
				if(val2 < val1)
					std::swap(val1, val2);
				return true;
			}
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

		void addTrail(std::size_t x, std::size_t y, double distance, double thickness, PixelInfo& info)
		{
			if(std::isnan(distance))
				return;

			switch(info.status)
			{
				case PixelInfo::Status::FAR_AWAY:
					info.status   = PixelInfo::Status::TRAIL;
					break;
				case PixelInfo::Status::TRAIL:
					if(distance >= info.distance)
						return;
					break;
				default:
					return;
			}

			info.distance  = distance;
			info.tempValue = thickness;

			trailMap.emplace(distance, PixtureElement(x, y));
		}


		template<int L>
		inline void calcSetTrailDistanceL(std::size_t x, std::size_t y, double distance, double value)
		{
			PixelInfo& newTrailInfo = (*pixelMap)(x, y);
			if(newTrailInfo.status != PixelInfo::Status::ACCEPTED && newTrailInfo.status != PixelInfo::Status::BRODER)
			{
				double trailDistance;
				double thicknessVal;
				std::tie(trailDistance, thicknessVal) = calcTrailDistanceL<L>(x, y, distance, value);
				if(trailDistance < maxDistance)
					addTrail(x, y, trailDistance, thicknessVal, newTrailInfo);
// 					newTrailInfo.setTrailDistance(trailDistance, trailmap, x, y);
			}
		}


		template<int L>
		inline std::tuple<double, double> calcTrailDistanceL(std::size_t x, std::size_t y, double distance, double value);



		std::tuple<double, double> calcTrailDistanceL2(std::size_t x, std::size_t y)
		{
			double valueX1 = std::numeric_limits<double>::infinity();
			double valueX2 = std::numeric_limits<double>::infinity();
			double valueY1 = std::numeric_limits<double>::infinity();
			double valueY2 = std::numeric_limits<double>::infinity();

			double thicknessX1;
			double thicknessX2;
			double thicknessY1;
			double thicknessY2;

			if(x>0)
			{
				const PixelInfo& x1 = (*pixelMap)(x-1, y);
				if(x1.status == PixelInfo::Status::ACCEPTED)
				{
					valueX1     = x1.distance;
					thicknessX1 = x1.tempValue;
				}
			}
			if(x<pixelMap->getSizeX()-1)
			{
				const PixelInfo& x2 = (*pixelMap)(x+1, y);
				if(x2.status == PixelInfo::Status::ACCEPTED)
				{
					valueX2     = x2.distance;
					thicknessX2 = x2.tempValue;
				}
			}

			if(y>0)
			{
				const PixelInfo& y1 = (*pixelMap)(x, y-1);
				if(y1.status == PixelInfo::Status::ACCEPTED)
				{
					valueY1     = y1.distance;
					thicknessY1 = y1.tempValue;
				}
			}
			if(y<pixelMap->getSizeY()-1)
			{
				const PixelInfo& y2 = (*pixelMap)(x, y+1);
				if(y2.status == PixelInfo::Status::ACCEPTED)
				{
					valueY2     = y2.distance;
					thicknessY2 = y2.tempValue;
				}
			}

			double valueX; // = std::min(valueX1, valueX2);
			double valueY; // = std::min(valueY1, valueY2);
			double thicknessX;
			double thicknessY;

			if(valueX1 < valueX2)
			{
				valueX     = valueX1;
				thicknessX = thicknessX1;
			}
			else
			{
				valueX     = valueX2;
				thicknessX = thicknessX2;
			}

			if(valueY1 < valueY2)
			{
				valueY     = valueY1;
				thicknessY = thicknessY1;
			}
			else
			{
				valueY     = valueY2;
				thicknessY = thicknessY2;
			}

			if(valueX == std::numeric_limits<double>::infinity())
				return std::make_tuple(valueY + 1, thicknessY);
			if(valueY == std::numeric_limits<double>::infinity())
				return std::make_tuple(valueX + 1, thicknessX);

			double d = (valueX+valueY)*(valueX+valueY)/4 - (valueX*valueX + valueY*valueY -1)/2.;

			if(d>0)
			{
				double v1 = (valueX+valueY)/2 + std::sqrt(d);
				double thicknessVal;
				if(valueX < valueY) thicknessVal = thicknessX;
				else                thicknessVal = thicknessY;
				return std::make_tuple(v1, thicknessVal);
			}
			else
			{
				std::cout << "x: " << x << "\ty: " << y <<"\tD: " << d << "\tvalueX: " << valueX << "\tvalueY: " << valueY << std::endl;

				return std::make_tuple(std::numeric_limits<double>::quiet_NaN(), 0.0);
			}
		}

// 		double weigthFormular(double distance)
// 		{
// 			double actWeight = 1;
// 			if(distance > 0)
// 				actWeight = 1/(distance*distance);
// 			return actWeight;
// 		}
//

		double weigthFormular(double distance)
		{
			double actWeight = 0;
			if(distance > 0)
				actWeight = maxDistance-distance;
			return actWeight;
		}

		template<int L>
		void createDistValues()
		{
			double distance    = 0;

			while(trailMap.size() > 0 && distance < maxDistance)
			{
				auto aktIt = trailMap.begin();

				PixtureElement& aktEle = aktIt->second;
				distance = aktIt->first;

				const std::size_t aktX = aktEle.getX();
				const std::size_t aktY = aktEle.getY();


				PixelInfo& info = (*pixelMap)(aktX, aktY);
				if(info.setValueFinal())
				{
					const double thickness = info.tempValue;

					if(aktY > 0)
						calcSetTrailDistanceL<L>(aktX  , aktY-1, distance, thickness);
					if(aktY < pixelMap->getSizeY()-1)
						calcSetTrailDistanceL<L>(aktX  , aktY+1, distance, thickness);
					if(aktX > 0)
						calcSetTrailDistanceL<L>(aktX-1, aktY  , distance, thickness);
					if(aktX < pixelMap->getSizeX()-1)
						calcSetTrailDistanceL<L>(aktX+1, aktY  , distance, thickness);
				}

// 					if(!info.initValue)
// 					{
// 						const double actWeight = weigthFormular(distance);
//
// 						info.value  += thickness*actWeight;
// 						info.weight += actWeight;
// 					}

				trailMap.erase(aktIt);
			}

			for(PixelInfo& info : *pixelMap)
			{
				if(info.status != PixelInfo::Status::BRODER)
					info.status = PixelInfo::Status::FAR_AWAY;
			}
		}

		void calcActDistMap()
		{
			if(lNorm == 1)
				createDistValues<1>();
			else
				createDistValues<2>();
		}

		void createMap()
		{
// 			std::cout << "maxValue: " << maxValue << "\nminValue: " << minValue << std::endl;
			for(std::size_t y = 0; y < pixelMap->getSizeY(); ++y)
			{
				uint8_t* imgIt = thicknessImage.ptr<uint8_t>(static_cast<int>(y));
				for(std::size_t x = 0; x < pixelMap->getSizeX(); ++x)
				{
					PixelInfo& info = (*pixelMap)(x, y);

					double mixThickness = info.getMixValue();
					if(mixThickness < minValue)
						*imgIt = 0;
					else
						*imgIt = static_cast<uint8_t>((mixThickness-minValue)/(maxValue-minValue)*255);
// 					std::cout << mixThickness << "\t" << (mixThickness-minValue)/(maxValue-minValue)*255 << std::endl;
// 					*imgIt = static_cast<uint8_t>(mixThickness*255);


// 					if(info.weight > 0)
// 					{
// 						double value = info.value/info.weight;
// 						*imgIt = static_cast<uint8_t>((value-minValue)/(maxValue-minValue)*255);
// 					}
// 					else
// 						*imgIt = 0;
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
// 			if(info.status == PixelInfo::Status::BRODER)
// 				return;

			info.status    = PixelInfo::Status::ACCEPTED;
			info.distance  = 0;
			info.tempValue = thickness;
// 			info.value     = thickness;
// 			info.weight    = 1;
			info.initValue = true;
			trailMap.emplace(0, PixtureElement(x, y));

// 			std::cout << thickness << std::endl;

			if(thickness < minValue) minValue = thickness;
			if(thickness > maxValue) maxValue = thickness;
		}

		OctData::CoordSLOpx transformCoord(const OctData::CoordSLOmm& coord)
		{
			return (transform*coord)*factor + shift;
		}

		void addLineScan(const OctData::BScan& bscan, const Segmentline& segLine1, const Segmentline& segLine2)
		{
			const OctData::CoordSLOpx& start_px = transformCoord(bscan.getStart());
			const OctData::CoordSLOpx&   end_px = transformCoord(bscan.getEnd()  );

			const std::size_t bscanWidth = static_cast<std::size_t>(bscan.getWidth());

			if(bscanWidth < 2)
				return;

			for(std::size_t i=0; i<bscanWidth; ++i)
			{
				const double v = static_cast<double>(i)/static_cast<double>(bscanWidth-1);
				const OctData::CoordSLOpx actPos = start_px*v + end_px*(1-v);
				addPixelValue(actPos, segLine2[i], segLine1[i]);
			}

			calcActDistMap();
		}


		void addCircleScan(const OctData::BScan& bscan, const Segmentline& segLine1, const Segmentline& segLine2)
		{
			const OctData::CoordSLOpx& start_px = transformCoord(bscan.getStart ());
			const OctData::CoordSLOpx&   end_px = transformCoord(bscan.getCenter());

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

			calcActDistMap();
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

		void addBroder(const OctData::Series::BScanSLOCoordList& broderPoints)
		{
			class DrawBroder
			{
				PixelMap& pixelMap;
			public:
				DrawBroder(PixelMap& map) : pixelMap(map) {}

				void plot(int x, int y)
				{
					std::cout << x << ", " << y << std::endl;
					if(x<0 && y<0)
						return;

					const std::size_t xs = static_cast<std::size_t>(x);
					const std::size_t ys = static_cast<std::size_t>(y);
					if(xs<pixelMap.getSizeX() && ys<pixelMap.getSizeY())
					{
						std::cout << xs << ", " << ys << std::endl;
						pixelMap(xs, ys).status = PixelInfo::Status::BRODER;
					}
				}
			};

			if(broderPoints.size() < 2)
				return;

			if(!pixelMap)
				return;

			DrawBroder db(*pixelMap);
			LineBresenhamAlgo<DrawBroder> lineAlgo(db);

			OctData::CoordSLOpx lastPoint = transformCoord(*(broderPoints.end()-1));
			for(const OctData::CoordSLOmm& actPoint : broderPoints)
			{
				OctData::CoordSLOpx actPointPx = transformCoord(actPoint);

				std::cout << "von : " << actPointPx.getX() << " , " << actPointPx.getY() << std::endl;
				std::cout << "nach: " << lastPoint .getX() << " , " << lastPoint .getY() << std::endl;

				lineAlgo.plotLine(actPointPx.getX(), actPointPx.getY()
				                , lastPoint .getX(), lastPoint .getY());

				lastPoint = actPointPx;
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

			const OctData::Series::BScanSLOCoordList& convexHull = series->getConvexHull();
			const OctData::SloImage& sloImage = series->getSloImage();

			cv::Mat sloImageMat = sloImage.getImage();
			if(sloImageMat.empty())
				return;

			pixelMap = new PixelMap(sloImageMat.cols, sloImageMat.rows);
			thicknessImage.create(sloImageMat.size(), cv::DataType<uint8_t>::type);

			factor    = sloImage.getScaleFactor();
			shift     = sloImage.getShift()      ;
			transform = sloImage.getTransform()  ;

			addBroder(convexHull);

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


	template<>
	inline std::tuple<double, double> CreateThicknessMap::calcTrailDistanceL<1>(std::size_t /*x*/, std::size_t /*y*/, double distance, double thickness)
	{
		return std::make_tuple(distance+1, thickness);
	}

	template<>
	inline std::tuple<double, double> CreateThicknessMap::calcTrailDistanceL<2>(std::size_t x, std::size_t y, double /*distance*/, double /*thickness*/)
	{
		return calcTrailDistanceL2(x, y);
	}


}




void ThicknessMap::createMap(const OctData::Series* series
                           , const std::vector<BScanLayerSegmentation::BScanSegData>& lines
                           , OctData::Segmentationlines::SegmentlineType t1
                           , OctData::Segmentationlines::SegmentlineType t2)
{
	CreateThicknessMap thicknessMap(series, lines, t1, t2);

	imshow("Thicknessmap", thicknessMap.getThicknessMap());
}
