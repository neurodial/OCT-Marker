#include "bscansegalgorithm.h"

#include <opencv/cv.h>
#include <cassert>
#include <limits>


#include <octdata/datastruct/bscan.h>


#include "bscansegmentation.h"

namespace
{
	struct FieldAccVertical
	{
		template<typename T>
		static constexpr T* startIt(cv::Mat& field, std::size_t inner, std::size_t outer) { return field.ptr<T>(static_cast<int>(inner)) + outer; }
		template<typename T>
		static constexpr const T* startIt_const(const cv::Mat& field, std::size_t inner, std::size_t outer) { return field.ptr<T>(static_cast<int>(inner)) + outer; }

		static std::size_t numInner(const cv::Mat* levelset) { return static_cast<std::size_t>(levelset->rows); }
		static std::size_t numOuter(const cv::Mat* levelset) { return static_cast<std::size_t>(levelset->cols); }
	};

	struct FieldAccHorizontal
	{
		template<typename T>
		static constexpr T* startIt(cv::Mat& field, std::size_t inner, std::size_t outer) { return field.ptr<T>(static_cast<int>(outer)) + inner; }
		template<typename T>
		static constexpr const T* startIt_const(const cv::Mat& field, std::size_t inner, std::size_t outer) { return field.ptr<T>(static_cast<int>(outer)) + inner; }

		static std::size_t numInner(const cv::Mat* levelset) { return static_cast<std::size_t>(levelset->cols); }
		static std::size_t numOuter(const cv::Mat* levelset) { return static_cast<std::size_t>(levelset->rows); }
	};

	struct OpDown : public FieldAccVertical
	{
		template<typename T>
		static constexpr T* op(T* p, std::ptrdiff_t val, int num = 1) { return p + val*num; }
		static constexpr const bool posDirection = true;
	};
	struct OpUp : public FieldAccVertical
	{
		template<typename T>
		static constexpr T* op(T* p, std::ptrdiff_t val, int num = 1) { return p - val*num; }
		static constexpr const bool posDirection = false;
	};

	struct OpRight : public FieldAccHorizontal
	{
		template<typename T>
		static constexpr T* op(T* p, std::ptrdiff_t, int num = 1) { return p + num; }
		static constexpr const bool posDirection = true;
	};
	struct OpLeft : public FieldAccHorizontal
	{
		template<typename T>
		static constexpr T* op(T* p, std::ptrdiff_t, int num = 1) { return p - num; }
		static constexpr const bool posDirection = false;
	};


	template<typename Operator>
	struct PartitionFromGrayValueWorker
	{
		inline static void iterateRow(cv::Mat* levelSetData
		                            , const cv::Mat&       img
		                            , const uint8_t        grayValue
		                            , const uint8_t        breakValue
		                            , const std::size_t    startInner
		                            , const std::size_t    posOuter
		                            , const std::size_t    numInner
		                            , const std::ptrdiff_t itLineNum
		                            , const int            neededStrikes
		                            , const double         negStrikesFactor)
		{
			BScanSegmentationMarker::internalMatType* levelSetIt  = Operator::template startIt      <BScanSegmentationMarker::internalMatType>(*levelSetData, startInner, posOuter);
			const uint8_t*                      imgIt       = Operator::template startIt_const<uint8_t>(img          , startInner, posOuter);

			int         negStri  = 0;
			int         strikes  = 0;
			std::size_t innerPos = 0;

			for(; innerPos < numInner; ++innerPos)
			{
				if(*imgIt >= grayValue)
				{
					if(strikes > neededStrikes || *imgIt == breakValue)
						break;
					++strikes;
				}
				else if(strikes > 0)
				{
					++negStri;
					if(negStri < strikes*negStrikesFactor)
						++strikes;
					else
					{
						strikes = 0;
						negStri = 0;
					}
				}

				*levelSetIt =  BScanSegmentationMarker::paintArea0Value;
				levelSetIt  = Operator::op(levelSetIt, itLineNum);
				imgIt       = Operator::op(imgIt     , itLineNum);
			}

			// go back to the begin of the founded shape
			assert(innerPos >= static_cast<std::size_t>(strikes));
			innerPos  -= strikes;
			levelSetIt = Operator::op(levelSetIt, itLineNum, -strikes);
			imgIt      = Operator::op(imgIt     , itLineNum, -strikes);

			for(; innerPos < numInner; ++innerPos)
			{
				*levelSetIt =  BScanSegmentationMarker::paintArea1Value;
				levelSetIt  = Operator::op(levelSetIt, itLineNum);
				imgIt       = Operator::op(imgIt     , itLineNum);
			}
		}

		static void iterateAbsolute(cv::Mat* levelSetData, const cv::Mat& img, const BScanSegmentationMarker::internalMatType grayValue, const int neededStrikes, const double negStrikesFactor)
		{
			const std::size_t    numInner   = Operator::numInner(levelSetData); // levelSetData->getSizeY();
			const std::size_t    numOuter   = Operator::numOuter(levelSetData); // levelSetData->getSizeX();
			const std::size_t    innerStart = Operator::posDirection?0:numInner-1;
			const std::ptrdiff_t itLineNum  = img.ptr<uint8_t>(1) - img.ptr<uint8_t>(0);

			for(size_t outerPos = 0; outerPos < numOuter; ++outerPos)
			{
				iterateRow(levelSetData, img, grayValue, std::numeric_limits<uint8_t>::max(), innerStart, outerPos, numInner, itLineNum, neededStrikes, negStrikesFactor);
			}
		}

		static void iterateRelativ(cv::Mat* levelSetData, const cv::Mat& img, double frac, const int neededStrikes, const double negStrikesFactor)
		{
			const std::size_t    numInner   = Operator::numInner(levelSetData);
			const std::size_t    numOuter   = Operator::numOuter(levelSetData);
			const std::size_t    innerStart = Operator::posDirection?0:numInner-1;
			const std::ptrdiff_t itLineNum  = img.ptr<uint8_t>(1) - img.ptr<uint8_t>(0);

			for(size_t outerPos = 0; outerPos < numOuter; ++outerPos)
			{
				const uint8_t* imgIt           = Operator::template startIt_const<uint8_t>(img, innerStart, outerPos);
				uint8_t        maxGrayValueCol = *imgIt;
				uint8_t        minGrayValueCol = *imgIt;

				for(size_t innerPos = 0; innerPos < numInner; ++innerPos)
				{
					if(maxGrayValueCol < *imgIt)
						maxGrayValueCol = *imgIt;
					else if(minGrayValueCol > *imgIt)
						minGrayValueCol = *imgIt;

					imgIt = Operator::op(imgIt, itLineNum);
				}
				const uint8_t grayValue = static_cast<uint8_t>((maxGrayValueCol-minGrayValueCol)*frac + minGrayValueCol);

				iterateRow(levelSetData, img, grayValue, maxGrayValueCol, innerStart, outerPos, numInner, itLineNum, neededStrikes, negStrikesFactor);
			}
		}

		static void initFromThresholdMethod(const cv::Mat& image, cv::Mat& segMat, const BScanSegmentationMarker::ThresholdData& data)
		{
			switch(data.method)
			{
				case BScanSegmentationMarker::ThresholdData::Method::Absolute:
					iterateAbsolute(&segMat, image, data.absoluteValue, data.neededStrikes, data.negStrikesFactor);
					break;
				case BScanSegmentationMarker::ThresholdData::Method::Relative:
					iterateRelativ(&segMat , image, data.relativeFrac , data.neededStrikes, data.negStrikesFactor);
					break;
			}
		}
	};
}

void BScanSegAlgorithm::initFromThreshold(const cv::Mat& image, cv::Mat& segMat, const BScanSegmentationMarker::ThresholdData& data)
{
	assert(image.cols == segMat.cols);
	assert(image.rows == segMat.rows);
	assert(image.ptr<uint8_t>(1) - image.ptr<uint8_t>(0) == segMat.ptr<uint8_t>(1) - segMat.ptr<uint8_t>(0)); // line distance

	switch(data.direction)
	{
		case BScanSegmentationMarker::ThresholdData::Direction::down:
			PartitionFromGrayValueWorker<OpDown >::initFromThresholdMethod(image, segMat, data);
			break;
		case BScanSegmentationMarker::ThresholdData::Direction::up:
			PartitionFromGrayValueWorker<OpUp   >::initFromThresholdMethod(image, segMat, data);
			break;
		case BScanSegmentationMarker::ThresholdData::Direction::right:
			PartitionFromGrayValueWorker<OpRight>::initFromThresholdMethod(image, segMat, data);
			break;
		case BScanSegmentationMarker::ThresholdData::Direction::left:
			PartitionFromGrayValueWorker<OpLeft >::initFromThresholdMethod(image, segMat, data);
			break;
	}
}

void BScanSegAlgorithm::initFromSegline(const OctData::BScan& bscan, cv::Mat& segMat)
{
	if(!segMat.empty())
	{
		const OctData::BScan::Segmentline& segline = bscan.getSegmentLine(OctData::BScan::SegmentlineType::ILM);
		BScanSegmentationMarker::internalMatType* colIt = segMat.ptr<BScanSegmentationMarker::internalMatType>();

		std::size_t colSize = static_cast<std::size_t>(segMat.cols);
		std::size_t rowSize = static_cast<std::size_t>(segMat.rows);

		for(double value : segline)
		{
			const std::size_t rowCh = std::min(static_cast<std::size_t>(value), rowSize);
			BScanSegmentationMarker::internalMatType* rowIt = colIt;

			for(std::size_t row = 0; row < rowCh; ++row)
			{
				*rowIt =  1;
				rowIt += colSize;
			}

			for(std::size_t row = rowCh; row < rowSize; ++row)
			{
				*rowIt = 0;
				rowIt += colSize;
			}

			++colIt;
		}
	}

}


/*
void BScanSegmentation::initFromSegmentline()
{
	const OctData::Series* series = getSeries();

	SegMats::iterator segMatIt = segments.begin();

	for(const OctData::BScan* bscan : series->getBScans())
	{
		const OctData::BScan::Segmentline& segline = bscan->getSegmentLine(OctData::BScan::SegmentlineType::ILM);
		cv::Mat* mat = *segMatIt;
		if(mat && !mat->empty())
		{
			internalMatType* colIt = mat->ptr<internalMatType>();
			std::size_t colSize = static_cast<std::size_t>(mat->cols);
			std::size_t rowSize = static_cast<std::size_t>(mat->rows);
			for(double value : segline)
			{
				const std::size_t rowCh = std::min(static_cast<std::size_t>(value), rowSize);
				internalMatType* rowIt = colIt;

				for(std::size_t row = 0; row < rowCh; ++row)
				{
					*rowIt =  1;
					rowIt += colSize;
				}

				for(std::size_t row = rowCh; row < rowSize; ++row)
				{
					*rowIt = 0;
					rowIt += colSize;
				}

				++colIt;
			}
		}
		++segMatIt;
	}
	requestUpdate();
}
*/


void BScanSegAlgorithm::openClose(cv::Mat& dest, cv::Mat* src)
{
	if(!src)
		src = &dest;

	int iterations = 1;
	cv::erode (*src, *src, cv::Mat(), cv::Point(-1, -1), iterations  , cv::BORDER_REFLECT_101, 1);
	cv::dilate(*src, *src, cv::Mat(), cv::Point(-1, -1), iterations*2, cv::BORDER_REFLECT_101, 1);
	cv::erode (*src, dest, cv::Mat(), cv::Point(-1, -1), iterations  , cv::BORDER_REFLECT_101, 1);
}


