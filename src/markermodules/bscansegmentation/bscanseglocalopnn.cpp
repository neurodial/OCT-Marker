#include "bscanseglocalopnn.h"

#ifdef ML_SUPPORT

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

	layers.row(0) = cv::Scalar(maskSizeInput);
	layers.row(1) = cv::Scalar(150);
	layers.row(2) = cv::Scalar(100);
	layers.row(3) = cv::Scalar(maskSizeOutput);

	mlp->create(layers, CvANN_MLP::SIGMOID_SYM, 1, 1);
}

BScanSegLocalOpNN::~BScanSegLocalOpNN()
{
	delete mlp;
	delete tranSampels  ;
	delete outputSampels;
}



int BScanSegLocalOpNN::getSubMaps(cv::Mat& image, cv::Mat& seg, int x, int y)
{
	const OctData::BScan* bscan = getActBScan();
	if(!bscan)
		return false;

	cv::Mat* map = getActMat();
	if(!map || map->empty())
		return false;

	return getSubMaps(bscan->getImage(), *map, &image, &seg, x, y);
}


int BScanSegLocalOpNN::getSubMaps(const cv::Mat& image, const cv::Mat& seg, cv::Mat* imageOut, cv::Mat* segOut, int x, int y)
{
	int rows = seg.rows;
	int cols = seg.cols;

	int x0i = std::max(x - paintSizeWidthInput , 0);
	int x1i = std::min(x + paintSizeWidthInput , cols-1);
	int x0o = std::max(x - paintSizeWidthOutput, 0);
	int x1o = std::min(x + paintSizeWidthOutput, cols-1);
	int y0  = std::max(y - paintSizeHeight     , 0);
	int y1  = std::min(y + paintSizeHeight     , rows-1);

	if(x0i>=x1i)
		return 0;
	if(x0o>=x1o)
		return 0;
	if(y0>=y1)
		return 0;

	int height = y1 -y0 ;
	int widthI = x1i-x0i;
	// int widthO = x1o-x0o;

	if(segOut)
		*segOut   = seg  (cv::Rect(x0o, y0, x1o-x0o, y1-y0));
	if(imageOut)
		*imageOut = image(cv::Rect(x0i, y0, x1i-x0i, y1-y0));

	return height*widthI;
}


int BScanSegLocalOpNN::getSubMapSize(const cv::Mat& mat, int x, int y)
{
	return getSubMaps(cv::Mat(), mat, nullptr, nullptr, x, y);
}


void BScanSegLocalOpNN::drawMarkerPaint(QPainter& painter, const QPoint& centerDrawPoint, double factor) const
{
	int sizeW    = static_cast<int>(paintSizeWidthInput *factor + 0.5);
	int sizeWerg = static_cast<int>(paintSizeWidthOutput*factor + 0.5);
	int sizeH    = static_cast<int>(paintSizeHeight*factor + 0.5);
	painter.drawRect(centerDrawPoint.x()-sizeW, centerDrawPoint.y()-sizeH, sizeW*2, sizeH*2);

	QPen pen(Qt::blue);
	painter.setPen(pen);
	painter.drawRect(centerDrawPoint.x()-sizeWerg, centerDrawPoint.y()-sizeH, sizeWerg*2, sizeH*2);
}



bool BScanSegLocalOpNN::applyNN(int x, int y)
{
	cv::Mat image, seg;
	getSubMaps(image, seg, x, y);

	if(image.rows*image.cols != maskSizeInput || seg.rows*seg.cols != maskSizeOutput)
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

/*
	CvTermCriteria criteria;
	criteria.max_iter = 200;
	criteria.epsilon = 0.000001f;
	criteria.type = CV_TERMCRIT_ITER | CV_TERMCRIT_EPS;

	CvANN_MLP_TrainParams params;
	params.train_method = CvANN_MLP_TrainParams::BACKPROP;
	params.bp_dw_scale = 0.005f;
	params.bp_moment_scale = 0.05f;
	params.term_crit = criteria;

*/

void BScanSegLocalOpNN::loadNN(const QString& file)
{
	mlp->load(file.toStdString().c_str(), "mlp");
}


void BScanSegLocalOpNN::saveNN(const QString& file) const
{
	cv::FileStorage fs(file.toStdString(), cv::FileStorage::WRITE); // or xml
	mlp->write(*fs, "mlp"); // don't think too much about the deref, it casts to a FileNode
}


template<typename T>
void BScanSegLocalOpNN::iterateBscanSeg(const cv::Mat& seg, T& op)
{
	int mapHeight = seg.rows-1; // -1 for p01
	int mapWidth  = seg.cols-1; // -1 for p10

	const int learnAbs = paintSizeHeight - 4;

	int startH = 0;
	int endH   = mapHeight;
	int startW = 0;
	int endW   = mapWidth;

	for(int h = startH; h < endH; ++h)
	{
		const uint8_t* p00 = seg.ptr<uint8_t>(h);
		const uint8_t* p01 = seg.ptr<uint8_t>(h+1);

		p00 += startW;
		p01 += startW;

		for(int w = startW; w < endW; ++w)
		{
			if(*p00 != *p01)
			{
				for(int h2 = -learnAbs; h2 <= learnAbs; ++h2)
				{
					op.op(seg, w, h+h2);
				}
			}
			++p00;
			++p01;
		}
	}
}


void BScanSegLocalOpNN::addBscanExampels()
{
	class SumOp
	{
		int transampelsSize = 0;
		BScanSegLocalOpNN* parent;
	public:
		SumOp(BScanSegLocalOpNN* parent) : parent(parent) {}
		void op(const cv::Mat& seg, int x, int y) { if(parent->getSubMapSize(seg, x, y) == maskSizeInput) ++transampelsSize; }
		int  getSize() const { return transampelsSize; }
	};

	class AddExapmelsOp
	{
		BScanSegLocalOpNN* parent;
		const cv::Mat& img;
		int actTrainSampel;

		cv::Mat& tranSampels  ;
		cv::Mat& outputSampels;

		cv::Mat subImage, subSeg;
	public:
		AddExapmelsOp(BScanSegLocalOpNN* parent
		            , const cv::Mat& img
		            , int startTrainSampel
		            , cv::Mat& tranSampels
		            , cv::Mat& outputSampels)
		: parent(parent)
		, img(img)
		, actTrainSampel(startTrainSampel)
		, tranSampels(tranSampels)
		, outputSampels(outputSampels)
		{}

		void op(const cv::Mat& seg, int x, int y)
		{
			if(parent->getSubMaps(img, seg, &subImage, &subSeg, x, y) == maskSizeInput)
			{
				cv::Mat imageFloatLin, segFloatLin;
				convert2FloatRow(subImage, imageFloatLin, 1./255.                                    , -0.5);
				convert2FloatRow(subSeg  , segFloatLin  , 1./BScanSegmentationMarker::paintArea1Value, -0.5);

				cv::Rect actRecI = cv::Rect(0, actTrainSampel, maskSizeInput , 1);
				cv::Rect actRecO = cv::Rect(0, actTrainSampel, maskSizeOutput, 1);
				imageFloatLin.copyTo(tranSampels  (actRecI));
				segFloatLin  .copyTo(outputSampels(actRecO));
				++actTrainSampel;
			}
		}
	};

	const OctData::BScan* bscan = getActBScan();
	if(!bscan)
		return;

	const cv::Mat* seg = getActMat();
	const cv::Mat& img = bscan->getImage();

	SumOp sumOp(this);
	iterateBscanSeg(*seg, sumOp);
	int transampelsSize  = sumOp.getSize();
	int startTrainSampel = 0;

	if((tranSampels == nullptr) || (outputSampels == nullptr) || tranSampels->rows != outputSampels->rows)
	{
		delete tranSampels;
		delete outputSampels;
		tranSampels   = new cv::Mat(transampelsSize, maskSizeInput , cv::DataType<float>::type);
		outputSampels = new cv::Mat(transampelsSize, maskSizeOutput, cv::DataType<float>::type);
	}
	else
	{
		int oldSize = tranSampels->rows;
		int newSize = oldSize + transampelsSize;
		startTrainSampel = oldSize;
		tranSampels  ->resize(static_cast<std::size_t>(newSize));
		outputSampels->resize(static_cast<std::size_t>(newSize));
	}

	AddExapmelsOp addExapmelsOp(this, img, startTrainSampel, *tranSampels, *outputSampels);
	iterateBscanSeg(*seg, addExapmelsOp);
}

void BScanSegLocalOpNN::trainNN(BScanSegmentationMarker::NNTrainData& trainData)
{
	if((tranSampels == nullptr) || (outputSampels == nullptr) || tranSampels->rows != outputSampels->rows) // TODO: Error Message
		return;

	CvTermCriteria criteria;
	criteria.max_iter = trainData.maxIterations;
	criteria.epsilon  = trainData.epsilon;
	criteria.type = CV_TERMCRIT_ITER | CV_TERMCRIT_EPS;

	CvANN_MLP_TrainParams params;
	params.term_crit       = criteria;
	switch(trainData.trainMethod)
	{
		case BScanSegmentationMarker::NNTrainData::TrainMethod::Backpropergation:
			params.train_method    = CvANN_MLP_TrainParams::BACKPROP;
			params.bp_dw_scale     = trainData.learnRate;
			params.bp_moment_scale = trainData.momentScale;
			break;
		case BScanSegmentationMarker::NNTrainData::TrainMethod::RPROP:
			params.train_method    = CvANN_MLP_TrainParams::RPROP;
			params.rp_dw0          = trainData.delta0  ;
			params.rp_dw_plus      = trainData.nuePlus ;
			params.rp_dw_minus     = trainData.nueMinus;
			params.rp_dw_min       = trainData.deltaMin;
			params.rp_dw_max       = trainData.deltaMax;
			break;
	}

	mlp->train(*tranSampels, *outputSampels, cv::Mat(), cv::Mat(), params, CvANN_MLP::NO_OUTPUT_SCALE | CvANN_MLP::NO_INPUT_SCALE);
}



int BScanSegLocalOpNN::numExampels() const
{
	return tranSampels->rows;
}



#endif


