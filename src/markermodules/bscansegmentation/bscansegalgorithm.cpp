#include "bscansegalgorithm.h"

#include <opencv/cv.h>
#if CV_MAJOR_VERSION >= 3
	#include <opencv2/imgproc.hpp>
#endif
#include <cassert>
#include <limits>
#include <cmath>


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
		cv::Mat* levelSetData ;
		const cv::Mat&       img;
		const BScanSegmentationMarker::internalMatType paintVal0;
		const BScanSegmentationMarker::internalMatType paintVal1;
		const int    neededStrikes   ;
		const double negStrikesFactor;

		PartitionFromGrayValueWorker(const BScanSegmentationMarker::ThresholdDirectionData& data
		                           , const cv::Mat& image
		                           , cv::Mat& segMat
		                           , BScanSegmentationMarker::internalMatType paintVal0
		                           , BScanSegmentationMarker::internalMatType paintVal1)
		: levelSetData(&segMat)
		, img(image)
		, paintVal0(paintVal0)
		, paintVal1(paintVal1)
		, neededStrikes   (data.neededStrikes   )
		, negStrikesFactor(data.negStrikesFactor)
		{
		}

		inline void iterateRow(const uint8_t        grayValue
		                     , const uint8_t        breakValue
		                     , const std::size_t    startInner
		                     , const std::size_t    posOuter
		                     , const std::size_t    numInner
		                     , const std::ptrdiff_t itLineNum)
		{
			BScanSegmentationMarker::internalMatType* levelSetIt  = Operator::template startIt      <BScanSegmentationMarker::internalMatType>(*levelSetData, startInner, posOuter);
			const uint8_t*                            imgIt       = Operator::template startIt_const<uint8_t                                 >(img          , startInner, posOuter);

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

				*levelSetIt =  paintVal0;
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
				*levelSetIt = paintVal1;
				levelSetIt  = Operator::op(levelSetIt, itLineNum);
				imgIt       = Operator::op(imgIt     , itLineNum);
			}
		}

		void iterateAbsolute(const BScanSegmentationMarker::internalMatType grayValue)
		{
			const std::size_t    numInner   = Operator::numInner(levelSetData); // levelSetData->getSizeY();
			const std::size_t    numOuter   = Operator::numOuter(levelSetData); // levelSetData->getSizeX();
			const std::size_t    innerStart = Operator::posDirection?0:numInner-1;
			const std::ptrdiff_t itLineNum  = img.ptr<uint8_t>(1) - img.ptr<uint8_t>(0);

			for(size_t outerPos = 0; outerPos < numOuter; ++outerPos)
			{
				iterateRow(grayValue, std::numeric_limits<uint8_t>::max(), innerStart, outerPos, numInner, itLineNum);
			}
		}

		void iterateRelativ(double frac)
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

				iterateRow(grayValue, maxGrayValueCol, innerStart, outerPos, numInner, itLineNum);
			}
		}

		void initFromThresholdMethod(const BScanSegmentationMarker::ThresholdDirectionData& data)
		{
			switch(data.method)
			{
				case BScanSegmentationMarker::ThresholdMethod::Absolute:
					iterateAbsolute(data.absoluteValue);
					break;
				case BScanSegmentationMarker::ThresholdMethod::Relative:
					iterateRelativ(data.relativeFrac  );
					break;
			}
		}
	};



	void fillRow(BScanSegmentationMarker::internalMatType* colIt
	           , const std::size_t colSize
	           , const std::size_t rowSize
	           , BScanSegmentationMarker::internalMatType upperValue
	           , BScanSegmentationMarker::internalMatType lowerValue
	           , const std::size_t valueChangeOnRow)
	{
		const std::size_t rowCh = std::min(valueChangeOnRow, rowSize);

		for(std::size_t row = 0; row < rowCh; ++row)
		{
			*colIt = upperValue;
			colIt += colSize;
		}

		for(std::size_t row = rowCh; row < rowSize; ++row)
		{
			*colIt = lowerValue;
			colIt += colSize;
		}
	}

	void findUnemptyBroder(int maxNumCols
	                     , int rowAdd
	                     , int rowSize
	                     , int colSize
	                     , BScanSegmentationMarker::internalMatType* imgIt
	                     , BScanSegmentationMarker::internalMatType& upperValue
	                     , BScanSegmentationMarker::internalMatType& lowerValue
	                     , int& foundCol
	                     , int& foundRow
	)
	{
		for(int i = 0; i < maxNumCols; ++i)
		{
			BScanSegmentationMarker::internalMatType* colIt = imgIt;

			upperValue = *colIt;

			for(int j = 1; j < rowSize; ++j)
			{
				colIt += colSize;
				if(*colIt != upperValue)
				{
					lowerValue = *colIt;
					foundCol = i;
					foundRow = j;
					i = maxNumCols; // break outer for
					break;
				}
			}
			imgIt += rowAdd;
		}
	}
}

void BScanSegAlgorithm::initFromThresholdDirection(const cv::Mat& image, cv::Mat& segMat
                                                 , const BScanSegmentationMarker::ThresholdDirectionData& data
                                                 , BScanSegmentationMarker::internalMatType paintArea0Value
                                                 , BScanSegmentationMarker::internalMatType paintArea1Value)
{
	assert(image.cols == segMat.cols);
	assert(image.rows == segMat.rows);
	assert(image.ptr<uint8_t>(1) - image.ptr<uint8_t>(0) == segMat.ptr<uint8_t>(1) - segMat.ptr<uint8_t>(0)); // line distance

	switch(data.direction)
	{
		case BScanSegmentationMarker::ThresholdDirectionData::Direction::down:
		{
			PartitionFromGrayValueWorker<OpDown > worker(data, image, segMat, paintArea0Value, paintArea1Value);
			worker.initFromThresholdMethod(data);
			break;
		}
		case BScanSegmentationMarker::ThresholdDirectionData::Direction::up:
		{
			PartitionFromGrayValueWorker<OpUp   > worker(data, image, segMat, paintArea0Value, paintArea1Value);
			worker.initFromThresholdMethod(data);
			break;
		}
		case BScanSegmentationMarker::ThresholdDirectionData::Direction::right:
		{
			PartitionFromGrayValueWorker<OpRight> worker(data, image, segMat, paintArea0Value, paintArea1Value);
			worker.initFromThresholdMethod(data);
			break;
		}
		case BScanSegmentationMarker::ThresholdDirectionData::Direction::left:
		{
			PartitionFromGrayValueWorker<OpLeft > worker(data, image, segMat, paintArea0Value, paintArea1Value);
			worker.initFromThresholdMethod(data);
			break;
		}
	}
}

BScanSegmentationMarker::internalMatType BScanSegAlgorithm::getThresholdGrayValue(const cv::Mat& image, const BScanSegmentationMarker::ThresholdData& data)
{
	if(data.method == BScanSegmentationMarker::ThresholdMethod::Relative)
	{
		double minVal;
		double maxVal;
		cv::minMaxLoc(image, &minVal, &maxVal);
		return static_cast<BScanSegmentationMarker::internalMatType>((maxVal-minVal)*data.relativeFrac + minVal);
	}
	else
		return data.absoluteValue;
}


void BScanSegAlgorithm::initFromThreshold(const cv::Mat& image
                                        , cv::Mat& segMat
                                        , const BScanSegmentationMarker::ThresholdData& data
                                        , BScanSegmentationMarker::internalMatType paintArea0Value
                                        , BScanSegmentationMarker::internalMatType paintArea1Value)
{
	assert(image.cols == segMat.cols);
	assert(image.rows == segMat.rows);

	const BScanSegmentationMarker::internalMatType grayValue = getThresholdGrayValue(image, data);

	// accept only char type matrices
	CV_Assert(image.depth() == CV_8U);
	CV_Assert(image.channels() == 1);

	int nRows = image.rows;
	int nCols = image.cols;

	if(image.isContinuous())
	{
		nCols *= nRows;
		nRows = 1;
	}

	for(int i = 0;i < nRows; ++i)
	{
		const uint8_t* imgIt = image .ptr<uint8_t>(i);
		      uint8_t* segIt = segMat.ptr<uint8_t>(i);

		for(int j = 0;j < nCols; ++j)
		{
			*segIt = *imgIt<grayValue ? paintArea0Value : paintArea1Value;
			++imgIt;
			++segIt;
		}
	}

}

void BScanSegAlgorithm::initFromSegline(const OctData::BScan& bscan, cv::Mat& segMat, OctData::Segmentationlines::SegmentlineType type)
{
	if(!segMat.empty())
	{
		const OctData::Segmentationlines::Segmentline& segline = bscan.getSegmentLine(type);
		BScanSegmentationMarker::internalMatType* colIt = segMat.ptr<BScanSegmentationMarker::internalMatType>();

		std::size_t colSize = static_cast<std::size_t>(segMat.cols);
		std::size_t rowSize = static_cast<std::size_t>(segMat.rows);

		for(double value : segline)
		{
			fillRow(colIt, colSize, rowSize, BScanSegmentationMarker::paintArea0Value, BScanSegmentationMarker::paintArea1Value, static_cast<std::size_t>(value));

			++colIt;
		}
	}

}



void BScanSegAlgorithm::openClose(cv::Mat& dest, cv::Mat* src)
{
	if(!src)
		src = &dest;

	int iterations = 1;
	cv::erode (*src, dest, cv::Mat(), cv::Point(-1, -1), iterations  , cv::BORDER_REFLECT_101, 1);
	cv::dilate(dest, dest, cv::Mat(), cv::Point(-1, -1), iterations*2, cv::BORDER_REFLECT_101, 1);
	cv::erode (dest, dest, cv::Mat(), cv::Point(-1, -1), iterations  , cv::BORDER_REFLECT_101, 1);
}


bool BScanSegAlgorithm::removeUnconectedAreas(cv::Mat& image)
{
	int rows = image.rows;
	int cols = image.cols;

	int posX = cols/2;
	int posY1 = 0;
	int posY2 = rows - 1;

	auto v1 = image.at<BScanSegmentationMarker::internalMatType>(cv::Point(posX, posY1));
	auto v2 = image.at<BScanSegmentationMarker::internalMatType>(cv::Point(posX, posY2));

	if(v1 == v2)
		return false;

	cv::floodFill(image, cv::Point(posX , posY1), 255); // save upper area
	cv::floodFill(image, cv::Point(posX , posY2), v1);  // convert v2 -> v1 : v1 areas in lower scope is included in lower area
	cv::floodFill(image, cv::Point(posX , posY2), 254); // save lower area
	cv::floodFill(image, cv::Point(posX , posY1), v2);  // convert v1 -> v2 : work on upper area
	cv::floodFill(image, cv::Point(posX , posY1), v1);  // retrieval upper area
	cv::floodFill(image, cv::Point(posX , posY2), v2);  // retrieval lower area
	return true;
}




bool BScanSegAlgorithm::extendLeftRightSpace(cv::Mat& image, int limit)
{
	if(!image.isContinuous())
		return false;

	if(limit < 0)
		limit = std::numeric_limits<int>::max();

	int rowSize = image.rows;
	int colSize = image.cols;

	int maxNumCols = std::min(colSize, limit);

	int foundCol = 0;
	int foundRow = 0;

	BScanSegmentationMarker::internalMatType upperValue;
	BScanSegmentationMarker::internalMatType lowerValue;

	BScanSegmentationMarker::internalMatType* imgIt = image.ptr<BScanSegmentationMarker::internalMatType>(0);
	findUnemptyBroder(maxNumCols, 1, rowSize, colSize, imgIt, upperValue, lowerValue, foundCol, foundRow);

	for(int i = 0; i < foundCol; ++i)
	{
		fillRow(imgIt, colSize, rowSize, upperValue, lowerValue, static_cast<std::size_t>(foundRow));
		++imgIt;
	}


	imgIt = image.ptr<BScanSegmentationMarker::internalMatType>(1)-1; // end of line 0
	findUnemptyBroder(maxNumCols, -1, rowSize, colSize, imgIt, upperValue, lowerValue, foundCol, foundRow);

	for(int i = 0; i < foundCol; ++i)
	{
		fillRow(imgIt, colSize, rowSize, upperValue, lowerValue, static_cast<std::size_t>(foundRow));
		--imgIt;
	}

	return true;
}
