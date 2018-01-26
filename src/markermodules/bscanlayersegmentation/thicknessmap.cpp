#include "thicknessmap.h"

#include <map>

#include <octdata/datastruct/series.h>
#include <octdata/datastruct/bscan.h>
#include <octdata/datastruct/sloimage.h>

#include<helper/convertcolorspace.h>

#include<data_structure/matrx.h>
#include<data_structure/point2d.h>
#include<data_structure/programoptions.h>

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
		bool blendColor = true;

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
			enum class Status : uint8_t { FAR_AWAY, ACCEPTED, BRODER };

			double value    = 0;

			Status status = Status::FAR_AWAY;

			SlideInfo val1;
			SlideInfo val2;

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

			bool updateValue(const SlideInfo& info)
			{
				if(val2 < info)
					return false;

				val2 = info;
				if(val2 < val1)
					std::swap(val1, val2);
				return true;
			}

		private:

		};

		typedef Matrix<PixelInfo> PixelMap;
		typedef std::multimap<double, PixtureElement> TrailMap;


		PixelMap* pixelMap = nullptr;
		TrailMap  trailMap;




		SegmentlineDataType minValue =  std::numeric_limits<SegmentlineDataType>::infinity();
		SegmentlineDataType maxValue = -std::numeric_limits<SegmentlineDataType>::infinity();

		cv::Mat thicknessImage;
		bool thicknessImageCreated = false;

		OctData::ScaleFactor    factor   ;
		OctData::CoordSLOpx     shift    ;
		OctData::CoordTransform transform;

		inline void addTrail(std::size_t x, std::size_t y, double distance, double thickness, PixelInfo& info)
		{
			if(std::isnan(distance))
				return;

			info.value    = thickness;
			info.status   = PixelInfo::Status::ACCEPTED;

			if(info.updateValue(SlideInfo(distance, thickness)))
				trailMap.emplace(distance, PixtureElement(x, y));
		}


		inline void calcSetTrailDistanceL1(std::size_t x, std::size_t y, double distance, double value)
		{
			PixelInfo& newTrailInfo = (*pixelMap)(x, y);
			if(newTrailInfo.status == PixelInfo::Status::FAR_AWAY)
			{
				double trailDistance = distance + 1;
				if(trailDistance < maxDistance)
					addTrail(x, y, trailDistance, value, newTrailInfo);
			}
		}

		void calcActDistMap()
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
				const double thickness = info.value;

				if(aktY > 0                     ) calcSetTrailDistanceL1(aktX  , aktY-1, distance, thickness);
				if(aktY < pixelMap->getSizeY()-1) calcSetTrailDistanceL1(aktX  , aktY+1, distance, thickness);
				if(aktX > 0                     ) calcSetTrailDistanceL1(aktX-1, aktY  , distance, thickness);
				if(aktX < pixelMap->getSizeX()-1) calcSetTrailDistanceL1(aktX+1, aktY  , distance, thickness);

				trailMap.erase(aktIt);
			}

			for(PixelInfo& info : *pixelMap)
			{
				if(info.status != PixelInfo::Status::BRODER)
					info.status = PixelInfo::Status::FAR_AWAY;
			}
		}


		void createColorMap()
		{
// 			std::cout << "maxValue: " << maxValue << "\nminValue: " << minValue << std::endl;
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


			PixelInfo& info = (*pixelMap)(x, y);


			info.status    = PixelInfo::Status::BRODER;
			info.value     = thickness;
			info.updateValue(SlideInfo(0, thickness));
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
				info.status = PixelInfo::Status::BRODER;
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

		const OctData::Series* series                                 ;
		const std::vector<BScanLayerSegmentation::BScanSegData>& lines;
		OctData::Segmentationlines::SegmentlineType t1                ;
		OctData::Segmentationlines::SegmentlineType t2                ;

	public:
		CreateThicknessMap(const OctData::Series* series
		                 , const std::vector<BScanLayerSegmentation::BScanSegData>& lines
		                 , OctData::Segmentationlines::SegmentlineType t1
		                 , OctData::Segmentationlines::SegmentlineType t2)
		: series(series)
		, lines(lines)
		, t1(t1)
		, t2(t2)
		{}

		void createMap()
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


			if(blendColor)
			{
				InitValueSetter ivs(*this);
				addBScans(series, lines, t1, t2, ivs);
			}

			AddPixelValueSetter apvs(*this, blendColor);
			addBScans(series, lines, t1, t2, apvs);

			if(!blendColor)
				calcActDistMap();
		}

		~CreateThicknessMap()
		{
			delete pixelMap;
		}

		void setBlendColor(bool b)                                     { blendColor = b; }


		const cv::Mat& getThicknessMap() { if(!thicknessImageCreated) createColorMap(); return thicknessImage; }
	};




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

	thicknessMapCreator.setBlendColor(ProgramOptions::layerSegThicknessmapBlend());

	thicknessMapCreator.createMap();

	thicknessMapCreator.getThicknessMap().copyTo(*thicknessMap);

// 	imshow("Thicknessmap", thicknessMapCreator.getThicknessMap());
}

