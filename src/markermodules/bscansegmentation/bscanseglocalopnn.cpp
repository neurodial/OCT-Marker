#include "bscanseglocalopnn.h"

#include <QPainter>

#include <opencv/cv.h>
#include <opencv/ml.h>

#include <octdata/datastruct/bscan.h>

#include <helper/callback.h>


BScanSegLocalOpNN::BScanSegLocalOpNN(BScanSegmentation& parent)
: BScanSegLocalOp(parent)
, mlp(new CvANN_MLP)
{
	cv::Mat layers = cv::Mat(4, 1, CV_32SC1);

	layers.row(0) = cv::Scalar(maskSize);
	layers.row(1) = cv::Scalar(150);
	layers.row(2) = cv::Scalar(100);
	layers.row(3) = cv::Scalar(maskSize);

	mlp->create(layers, CvANN_MLP::SIGMOID_SYM, 1, 1);
}

int BScanSegLocalOpNN::getSubMaps(cv::Mat& image, cv::Mat& seg, int x, int y)
{
	const OctData::BScan* bscan = getActBScan();
	if(!bscan)
		return false;

	cv::Mat* map = getActMat();
	if(!map || map->empty())
		return false;

	return getSubMaps(bscan->getImage(), *map, image, seg, x, y);
}


int BScanSegLocalOpNN::getSubMaps(cv::Mat& image, cv::Mat& seg, int x, int y, std::size_t bscanNr)
{
	const OctData::BScan* bscan = getBScan(bscanNr);
	if(!bscan)
		return false;

	cv::Mat* map = getSegMat(bscanNr);
	if(!map || map->empty())
		return false;

	return getSubMaps(bscan->getImage(), *map, image, seg, x, y);
}

int BScanSegLocalOpNN::getSubMaps(const cv::Mat& image, const cv::Mat& seg, cv::Mat& imageOut, cv::Mat& segOut, int x, int y)
{
	int rows = seg.rows;
	int cols = seg.cols;

	int x0 = std::max(x - paintSizeWidth , 0);
	int x1 = std::min(x + paintSizeWidth , cols-1);
	int y0 = std::max(y - paintSizeHeight, 0);
	int y1 = std::min(y + paintSizeHeight, rows-1);

	if(x0>=x1)
		return 0;
	if(y0>=y1)
		return 0;

	int height = y1-y0;
	int width  = x1-x0;

	segOut   = seg(cv::Rect(x0, y0, x1-x0, y1-y0));
	imageOut = image(cv::Rect(x0, y0, x1-x0, y1-y0));

	return height*width;
}

int BScanSegLocalOpNN::getSubMapSize(const cv::Mat& mat, int x, int y)
{
	int rows = mat.rows;
	int cols = mat.cols;

	int x0 = std::max(x - paintSizeWidth , 0);
	int x1 = std::min(x + paintSizeWidth , cols-1);
	int y0 = std::max(y - paintSizeHeight, 0);
	int y1 = std::min(y + paintSizeHeight, rows-1);

	if(x0>=x1)
		return 0;
	if(y0>=y1)
		return 0;

	int height = y1-y0;
	int width  = x1-x0;

	return height*width;
}




void BScanSegLocalOpNN::drawMarkerPaint(QPainter& painter, const QPoint& centerDrawPoint, double factor) const
{
	int sizeW = static_cast<int>(paintSizeWidth *factor + 0.5);
	int sizeH = static_cast<int>(paintSizeHeight*factor + 0.5);
	painter.drawRect(centerDrawPoint.x()-sizeW, centerDrawPoint.y()-sizeH, sizeW*2, sizeH*2);
}



bool BScanSegLocalOpNN::applyNN(int x, int y)
{
	cv::Mat image, seg;
	getSubMaps(image, seg, x, y);

	if(image.rows*image.cols != maskSize || seg.rows*seg.cols != maskSize)
		return false;

	cv::Mat imageFloat, segFloat;
	image.copyTo(imageFloat);
	// imageFloat.convertTo(imageFloat, cv::DataType<float>::type);
	imageFloat.convertTo(imageFloat, cv::DataType<float>::type, 1./128., -1);

	cv::Mat imageFloatLin = imageFloat.reshape(0, 1);
	// segFloat  .reshape(0, 1);

	mlp->predict(imageFloatLin, segFloat);


	double min1, max1, min2, max2;
	cv::minMaxLoc(segFloat, &min1, &max1);
	cv::minMaxLoc(imageFloatLin, &min2, &max2);
	// std::cerr << "ApplyNN: " << min1 << "\t" << max1 << "\t" << min2 << "\t" << max2 << "\n";

	segFloat.convertTo(segFloat, cv::DataType<uint8_t>::type, BScanSegmentationMarker::paintArea1Value, 0.5*BScanSegmentationMarker::paintArea1Value);
	segFloat.reshape(0, seg.rows).copyTo(seg);

	return true;
}

void BScanSegLocalOpNN::learnNN(int x, int y)
{
	cv::Mat image, seg;
	getSubMaps(image, seg, x, y);

	learnNNSubMat(image, seg);
}

namespace
{
	void convert2FloatRow(const cv::Mat& in, cv::Mat& out, double factor, double add)
	{
		cv::Mat tmp;
		in.copyTo(tmp);
		tmp.convertTo(tmp, cv::DataType<float>::type, factor, add);
		out = tmp.reshape(0, 1);
	}
}

void BScanSegLocalOpNN::learnNNSubMat(const cv::Mat& image, const cv::Mat& seg)
{
	CvTermCriteria criteria;
	criteria.max_iter = 1;
	criteria.epsilon = 0.00001f;
	criteria.type = CV_TERMCRIT_ITER | CV_TERMCRIT_EPS;

	CvANN_MLP_TrainParams params;
	params.train_method = CvANN_MLP_TrainParams::BACKPROP;
	params.bp_dw_scale = 0.005f;
	params.bp_moment_scale = 0.05f;
	params.term_crit = criteria;

	if(image.rows*image.cols != maskSize || seg.rows*seg.cols != maskSize)
		return;

	cv::Mat imageFloatLin, segFloatLin;
	convert2FloatRow(image, imageFloatLin, 1./255.                                    , -0.5);
	convert2FloatRow(seg  , segFloatLin  , 1./BScanSegmentationMarker::paintArea1Value, -0.5);
// 	image     .copyTo(imageFloat);
// 	seg       .copyTo(segFloat  );
// 	imageFloat.convertTo(imageFloat, cv::DataType<float>::type, 1./255., -0.5);
// 	segFloat  .convertTo(segFloat  , cv::DataType<float>::type, 1./BScanSegmentationMarker::paintArea1Value, -0.5);
//
// 	cv::Mat imageFloatLin = imageFloat.reshape(0, 1);
// 	cv::Mat segFloatLin   = segFloat  .reshape(0, 1);

	double min1, max1, min2, max2;
	cv::minMaxLoc(segFloatLin, &min1, &max1);
	cv::minMaxLoc(imageFloatLin, &min2, &max2);
	// std::cerr << "LearnNN (" << x << " : " << y <<"): " << min1 << "\t" << max1 << "\t" << min2 << "\t" << max2 << "\n";

	static bool init = true;

	if(init)
		mlp->train(imageFloatLin, segFloatLin, cv::Mat(), cv::Mat(), params, CvANN_MLP::NO_OUTPUT_SCALE | CvANN_MLP::NO_INPUT_SCALE);
	else
		mlp->train(imageFloatLin, segFloatLin, cv::Mat(), cv::Mat(), params, CvANN_MLP::UPDATE_WEIGHTS | CvANN_MLP::NO_OUTPUT_SCALE | CvANN_MLP::NO_INPUT_SCALE);

	init = false;
}

void BScanSegLocalOpNN::learnBScanMats(const cv::Mat& image, cv::Mat& seg, Callback& callback)
{
	int mapHeight = seg.rows-1; // -1 for p01
	int mapWidth  = seg.cols-1; // -1 for p10

	const int learnAbs = paintSizeHeight - 4;

	int startH = 0;
	int endH   = mapHeight;
	int startW = 0;
	int endW   = mapWidth;

	cv::Mat subImage, subSeg;

	double length = endH - startH;

	int transampelsSize = 0;

	for(int h = startH; h < endH; ++h)
	{
		uint8_t* p00 = seg.ptr<uint8_t>(h);
		uint8_t* p01 = seg.ptr<uint8_t>(h+1);

		p00 += startW;
		p01 += startW;

		for(int w = startW; w < endW; ++w)
		{
			if(*p00 != *p01)
			{
				for(int h2 = -learnAbs; h2 <= learnAbs; ++h2)
				{
					if(getSubMapSize(seg, w, h+h2) == maskSize)
						++transampelsSize;
				}
			}
			++p00;
			++p01;
		}
	}

	cv::Mat tranSampels, outputSampels;
	tranSampels  .create(transampelsSize, maskSize, cv::DataType<float>::type);
	outputSampels.create(transampelsSize, maskSize, cv::DataType<float>::type);

	int actTrainSampel = 0;
	for(int h = startH; h < endH; ++h)
	{
		if(!callback.callback(static_cast<double>(h-startH)/length))
			break;

		uint8_t* p00 = seg.ptr<uint8_t>(h);
		uint8_t* p01 = seg.ptr<uint8_t>(h+1);

		p00 += startW;
		p01 += startW;

		for(int w = startW; w < endW; ++w)
		{
			if(*p00 != *p01)
			{
				for(int h2 = -learnAbs; h2 <= learnAbs; ++h2)
				{
					if(getSubMaps(image, seg, subImage, subSeg, w, h+h2) == maskSize)
					{
						cv::Mat imageFloatLin, segFloatLin;
						convert2FloatRow(subImage, imageFloatLin, 1./255.                                    , -0.5);
						convert2FloatRow(subSeg  , segFloatLin  , 1./BScanSegmentationMarker::paintArea1Value, -0.5);

						cv::Rect actRec = cv::Rect(0, actTrainSampel, maskSize, 1);
						imageFloatLin.copyTo(tranSampels  (actRec));
						segFloatLin  .copyTo(outputSampels(actRec));
						++actTrainSampel;
					}
				}
			}
			++p00;
			++p01;
		}
	}



	CvTermCriteria criteria;
	criteria.max_iter = 100;
	criteria.epsilon = 0.000001f;
	criteria.type = CV_TERMCRIT_ITER | CV_TERMCRIT_EPS;

	CvANN_MLP_TrainParams params;
	params.train_method = CvANN_MLP_TrainParams::BACKPROP;
	params.bp_dw_scale = 0.05f;
	params.bp_moment_scale = 0.15f;
	params.term_crit = criteria;

	mlp->train(tranSampels, outputSampels, cv::Mat(), cv::Mat(), params, CvANN_MLP::NO_OUTPUT_SCALE | CvANN_MLP::NO_INPUT_SCALE);
}



void BScanSegLocalOpNN::learnBScan(Callback& callback)
{
	cv::Mat* map = getActMat();
	if(!map || map->empty())
		return;

	const OctData::BScan* bscan = getActBScan();
	if(!bscan)
		return;

	learnBScanMats(bscan->getImage(), *map, callback);
}


void BScanSegLocalOpNN::learnBScans(std::size_t start, std::size_t end, Callback& callback)
{
	double fracFactor = 1./static_cast<double>(end - start);

	for(std::size_t scan = start; scan < end; ++scan)
	{
		if(!callback.callback(static_cast<double>(scan-start)*fracFactor))
			break;

		cv::Mat* map = getSegMat(scan);
		if(!map || map->empty())
			continue;

		const OctData::BScan* bscan = getBScan(scan);
		if(!bscan)
			continue;

		Callback subCallback = callback.createSubTask(fracFactor);
		learnBScanMats(bscan->getImage(), *map, subCallback);
	}
}

