#include "thicknessmap.h"

#include <map>

#include <octdata/datastruct/series.h>
#include <octdata/datastruct/bscan.h>
#include <octdata/datastruct/sloimage.h>

#include<helper/convertcolorspace.h>

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
		constexpr static const double maxDistance = 25;
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
				if(!val1)
					return 0;
				if(val1.value <= 0)
					return 0;
				if(!val2 || val2.value <= 0)
					return val1.value;


				const double l = val1.distance + val2.distance;
				if(l == 0)
					return val1.value;

				const double w1 = val2.distance/l;
				const double w2 = val1.distance/l;

				const double result = val1.value*w1 + val2.value*w2;

				return result;
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

			if(!info.initValue)
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
			std::cout << "maxValue: " << maxValue << "\nminValue: " << minValue << std::endl;
			for(std::size_t y = 0; y < pixelMap->getSizeY(); ++y)
			{
				uint8_t* imgIt = thicknessImage.ptr<uint8_t>(static_cast<int>(y));
				for(std::size_t x = 0; x < pixelMap->getSizeX(); ++x)
				{
					PixelInfo& info = (*pixelMap)(x, y);

					double mixThickness = info.getMixValue();
					if(mixThickness >= maxValue)
					{
						imgIt[0] = 255;
						imgIt[1] = 255;
						imgIt[2] = 255;
						imgIt[3] = 255;
					}
// 					else if(mixThickness < minValue)
					else if(mixThickness == 0)
					{
						imgIt[0] = 0;
						imgIt[1] = 0;
						imgIt[2] = 0;
						imgIt[3] = 0;
					}
					else
					{
						double rd, gd, bd;
						double hue = mixThickness*360/maxValue;
						std::tie(rd, gd, bd) = hsv2rgb(hue, 1, 1);

						imgIt[0] = static_cast<uint8_t>(rd*255);
						imgIt[1] = static_cast<uint8_t>(gd*255);
						imgIt[2] = static_cast<uint8_t>(bd*255);
						imgIt[3] = 255;
					}
// 						*imgIt = static_cast<uint8_t>((mixThickness-minValue)/(maxValue-minValue)*255);

					imgIt += 4;
				}
			}

			thicknessImageCreated = true;
		}

		void addPixelValue(const OctData::CoordSLOpx& coord, SegmentlineDataType value1, SegmentlineDataType value2)
		{
			double thickness = (value1 - value2); // TODO: ScaleFactor

			if(value1 < 0 || value1 > 10000
			|| value2 < 0 || value2 > 10000)
				thickness = -100;
			else
			{
				if(thickness < minValue) minValue = thickness;
				if(thickness > maxValue) maxValue = thickness;
			}


			const std::size_t x = static_cast<std::size_t>(coord.getX());
			const std::size_t y = static_cast<std::size_t>(coord.getY());

			if(x >= pixelMap->getSizeX() || y >= pixelMap->getSizeY())
				return;

// 			if(thickness < 0)
// 				thickness = -thickness;

			PixelInfo& info = (*pixelMap)(x, y);
// 			if(info.status == PixelInfo::Status::BRODER)
// 				return;

			info.status    = PixelInfo::Status::ACCEPTED;
			info.distance  = 0;
			info.tempValue = thickness;
			info.initValue = true;
			trailMap.emplace(0, PixtureElement(x, y));

// 			std::cout << thickness << std::endl;

		}

		class AddPixelValueSetter
		{
			CreateThicknessMap& ctm;
		public:
			AddPixelValueSetter(CreateThicknessMap& ctm, bool calcDistMap) : ctm(ctm), calcDistMap(calcDistMap) {}

			void operator()(const OctData::CoordSLOpx& coord, SegmentlineDataType value1, SegmentlineDataType value2)
			{
				ctm.addPixelValue(coord, value1, value2);
			}

			const bool calcDistMap;

		};

		class InitValueSetter
		{
			CreateThicknessMap& ctm;
		public:
			InitValueSetter(CreateThicknessMap& ctm) : ctm(ctm) {}

			void operator()(const OctData::CoordSLOpx& coord, SegmentlineDataType value1, SegmentlineDataType value2)
			{
				const std::size_t x = static_cast<std::size_t>(coord.getX());
				const std::size_t y = static_cast<std::size_t>(coord.getY());

				if(x >= ctm.pixelMap->getSizeX() || y >= ctm.pixelMap->getSizeY())
					return;

				PixelInfo& info = (*ctm.pixelMap)(x, y);
				info.initValue = true;
			}
			constexpr static const bool calcDistMap = false;
		};


		OctData::CoordSLOpx transformCoord(const OctData::CoordSLOmm& coord)
		{
			return (transform*coord)*factor + shift;
		}

		template<typename Setter>
		void addLineScan(const OctData::BScan& bscan, const Segmentline& segLine1, const Segmentline& segLine2, Setter& pixelSetter)
		{
			const OctData::CoordSLOpx& start_px = transformCoord(bscan.getStart());
			const OctData::CoordSLOpx&   end_px = transformCoord(bscan.getEnd()  );

			const std::size_t bscanWidth = static_cast<std::size_t>(bscan.getWidth());

			if(bscanWidth < 2)
				return;

			for(std::size_t i=0; i<bscanWidth; ++i)
			{
				const double v = static_cast<double>(i)/static_cast<double>(bscanWidth-1);
				const OctData::CoordSLOpx actPos = start_px*(1-v) + end_px*v;
				pixelSetter(actPos, segLine2[i], segLine1[i]);
			}

			if(pixelSetter.calcDistMap)
				calcActDistMap();
		}


		template<typename Setter>
		void addCircleScan(const OctData::BScan& bscan, const Segmentline& segLine1, const Segmentline& segLine2, Setter& pixelSetter)
		{
			const OctData::CoordSLOpx& start_px  = transformCoord(bscan.getStart ());
			const OctData::CoordSLOpx& center_px = transformCoord(bscan.getCenter());

			const std::size_t bscanWidth = static_cast<std::size_t>(bscan.getWidth());

			if(bscanWidth < 2)
				return;

			bool clockwise = bscan.getClockwiseRot();

			double radius = start_px.abs(center_px);
			double ratio  = start_px.getXf() - center_px.getXf();
			double nullAngle = acos( ratio/radius )/M_PI/2;

			const int rotationFactor = clockwise?1:-1;

			double mX = center_px.getXf();
			double mY = center_px.getYf();

			for(std::size_t i=0; i<bscanWidth; ++i)
			{
				const double v     = static_cast<double>(i)/static_cast<double>(bscanWidth-1);
				const double angle = (v+nullAngle)*2.*M_PI*rotationFactor;

				const double x = cos(angle)*radius + mX;
				const double y = sin(angle)*radius + mY;
				const OctData::CoordSLOpx actPos(x, y);
				pixelSetter(actPos, segLine2[i], segLine1[i]);
			}

			if(pixelSetter.calcDistMap)
				calcActDistMap();
		}


		template<typename Setter>
		void addBScan(const OctData::BScan& bscan, const Segmentline& segLine1, const Segmentline& segLine2, Setter& pixelSetter)
		{
			const std::size_t bscanWidth = static_cast<std::size_t>(bscan.getWidth());
			if(bscanWidth != segLine1.size() || bscanWidth != segLine2.size())
				return;

			switch(bscan.getBScanType())
			{
				case OctData::BScan::BScanType::Line:
					addLineScan(bscan, segLine1, segLine2, pixelSetter);
					break;
				case OctData::BScan::BScanType::Circle:
					addCircleScan(bscan, segLine1, segLine2, pixelSetter);
					break;
			}
		}


		template<typename Setter>
		void addBScans(const OctData::Series* series
		             , const std::vector<BScanLayerSegmentation::BScanSegData>& lines
		             , OctData::Segmentationlines::SegmentlineType t1
		             , OctData::Segmentationlines::SegmentlineType t2
		             , Setter& pixelSetter)
		{
			std::size_t bscanNr = 0;
			for(const OctData::BScan* bscan : series->getBScans())
			{
				if(!bscan)
					continue;

				const BScanLayerSegmentation::BScanSegData& data = lines[bscanNr];
				const Segmentline& segLine1 = data.lines.getSegmentLine(t1);
				const Segmentline& segLine2 = data.lines.getSegmentLine(t2);

				addBScan(*bscan, segLine1, segLine2, pixelSetter);

				++bscanNr;
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
			thicknessImage.create(sloImageMat.size(), CV_8UC4);

			factor    = sloImage.getScaleFactor();
			shift     = sloImage.getShift()      ;
			transform = sloImage.getTransform()  ;

// 			addBroder(convexHull);

			if(lines.size() < series->bscanCount())
				return;

			bool blend = true;

			if(blend)
			{
				InitValueSetter ivs(*this);
				addBScans(series, lines, t1, t2, ivs);
			}

			AddPixelValueSetter apvs(*this, blend);
			addBScans(series, lines, t1, t2, apvs);

			if(!blend)
				calcActDistMap();
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

ThicknessMap::ThicknessMap()
: thicknessMap(new cv::Mat)
{
}



ThicknessMap::~ThicknessMap()
{
	delete thicknessMap;
}



void ThicknessMap::createMap(const OctData::Series* series
                           , const std::vector<BScanLayerSegmentation::BScanSegData>& lines
                           , OctData::Segmentationlines::SegmentlineType t1
                           , OctData::Segmentationlines::SegmentlineType t2)
{
	CreateThicknessMap thicknessMapCreator(series, lines, t1, t2);

	thicknessMapCreator.getThicknessMap().copyTo(*thicknessMap);

// 	imshow("Thicknessmap", thicknessMapCreator.getThicknessMap());
}

