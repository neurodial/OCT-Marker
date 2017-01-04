#include "bscanseglocalopnn.h"

#ifdef ML_SUPPORT

#include <QPainter>

#include <opencv/cv.h>
#include <opencv/ml.h>

#include <octdata/datastruct/bscan.h>

#include <helper/callback.h>


namespace
{
	void getRelOpSize(int& x1, int& x2, int& y1, int& y2, int x, int y)
	{
		x1 = (x+1)/2;
		x2 = x/2;
		y1 = (y+1)/2;
		y2 = y/2;
	}
}


BScanSegLocalOpNN::BScanSegLocalOpNN(BScanSegmentation& parent)
: BScanSegLocalOp(parent)
, mlp(new CvANN_MLP)
{
	calcMaskSizes();
	createNN();
}

BScanSegLocalOpNN::~BScanSegLocalOpNN()
{
	delete mlp;
	delete tranSampels  ;
	delete outputSampels;
}

void BScanSegLocalOpNN::createNN()
{
	if(maskSizeInput > 0 && maskSizeOutput > 0)
	{
		int numHiddenLayers = static_cast<int>(neuronsPerHiddenLayer.size());

		cv::Mat layers = cv::Mat(numHiddenLayers+2, 1, CV_32SC1);

		layers.row(0) = cv::Scalar(maskSizeInput);
		for(int i = 0; i < numHiddenLayers; ++i)
			layers.row(i+1) = cv::Scalar(neuronsPerHiddenLayer[static_cast<std::size_t>(i)]);
		layers.row(numHiddenLayers+1) = cv::Scalar(maskSizeOutput);

		mlp->create(layers, CvANN_MLP::SIGMOID_SYM, 1, 1);

		if(tranSampels)
			*tranSampels   = cv::Mat();
		if(outputSampels)
			*outputSampels = cv::Mat();
	}
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

	int dx0i, dx1i, dy0i, dy1i;
	getRelOpSize(dx0i, dx1i, dy0i, dy1i, paintSizeWidthInput, paintSizeHeightInput);
	int dx0o, dx1o, dy0o, dy1o;
	getRelOpSize(dx0o, dx1o, dy0o, dy1o, paintSizeWidthOutput, paintSizeHeightOutput);

	int x0i = std::max(x - dx0i , 0);
	int x1i = std::min(x + dx1i , cols-1);
	int y0i = std::max(y - dy0i , 0);
	int y1i = std::min(y + dy1i , rows-1);
	int x0o = std::max(x - dx0o , 0);
	int x1o = std::min(x + dx1o , cols-1);
	int y0o = std::max(y - dy0o , 0);
	int y1o = std::min(y + dy1o , rows-1);

	if(x0i>=x1i)
		return 0;
	if(x0o>=x1o)
		return 0;
	if(y0i>=y1i)
		return 0;
	if(y0o>=y1o)
		return 0;

	int height = y1i-y0i;
	int widthI = x1i-x0i;
	// int widthO = x1o-x0o;

	if(segOut)
		*segOut   = seg  (cv::Rect(x0o, y0o, x1o-x0o, y1o-y0o));
	if(imageOut)
		*imageOut = image(cv::Rect(x0i, y0i, x1i-x0i, y1i-y0i));

	return height*widthI;
}


int BScanSegLocalOpNN::getSubMapSize(const cv::Mat& mat, int x, int y)
{
	return getSubMaps(cv::Mat(), mat, nullptr, nullptr, x, y);
}


void BScanSegLocalOpNN::drawMarkerPaint(QPainter& painter, const QPoint& centerDrawPoint, double factor) const
{

	int dx0i, dx1i, dy0i, dy1i;
	getRelOpSize(dx0i, dx1i, dy0i, dy1i, paintSizeWidthInput, paintSizeHeightInput);
	int dx0o, dx1o, dy0o, dy1o;
	getRelOpSize(dx0o, dx1o, dy0o, dy1o, paintSizeWidthOutput, paintSizeHeightOutput);


	int sizeW      = static_cast<int>(paintSizeWidthInput  *factor + 0.5);
	int sizeWerg   = static_cast<int>(paintSizeWidthOutput *factor + 0.5);
	int sizeH      = static_cast<int>(paintSizeHeightInput *factor + 0.5);
	int sizeHerg   = static_cast<int>(paintSizeHeightOutput*factor + 0.5);
	int startX0In  = static_cast<int>(centerDrawPoint.x()-dx0i*factor + 0.5);
	int startY0In  = static_cast<int>(centerDrawPoint.y()-dy0i*factor + 0.5);
	int startX0Out = static_cast<int>(centerDrawPoint.x()-dx0o*factor + 0.5);
	int startY0Out = static_cast<int>(centerDrawPoint.y()-dy0o*factor + 0.5);
	painter.drawRect(startX0In, startY0In, sizeW, sizeH);

	QPen pen(Qt::blue);
	painter.setPen(pen);
	painter.drawRect(startX0Out, startY0Out, sizeWerg, sizeHerg);
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

void BScanSegLocalOpNN::setInputOutputSize(int widthIn, int heighIn, int widthOut, int heighOut)
{
	paintSizeWidthInput   = widthIn;
	paintSizeWidthOutput  = widthOut;
	paintSizeHeightInput  = heighIn;
	paintSizeHeightOutput = heighOut;

	calcMaskSizes();
}

void BScanSegLocalOpNN::calcMaskSizes()
{
	maskSizeInput  = paintSizeWidthInput *paintSizeHeightInput;
	maskSizeOutput = paintSizeWidthOutput*paintSizeHeightOutput;
}



void BScanSegLocalOpNN::loadNN(const QString& file)
{
	mlp->load(file.toStdString().c_str(), "mlp");

	cv::Mat sizeInOut;
	cv::FileStorage storage(file.toStdString(), cv::FileStorage::READ);
	storage["sizeInOut"] >> sizeInOut;
	storage.release();

	if(sizeInOut.rows != 4 || sizeInOut.cols != 1 || sizeInOut.type() != cv::DataType<int>::type)
		setInputOutputSize(10, 24, 2, 24);
	else
		setInputOutputSize(sizeInOut.at<int>(0)
		                 , sizeInOut.at<int>(1)
		                 , sizeInOut.at<int>(2)
		                 , sizeInOut.at<int>(3));

}


void BScanSegLocalOpNN::saveNN(const QString& file) const
{
	cv::FileStorage fs(file.toStdString(), cv::FileStorage::WRITE); // or xml

	cv::Mat sizeInOut = cv::Mat(4, 1, cv::DataType<int>::type);
	sizeInOut.row(0) = paintSizeWidthInput  ;
	sizeInOut.row(1) = paintSizeHeightInput ;
	sizeInOut.row(2) = paintSizeWidthOutput ;
	sizeInOut.row(3) = paintSizeHeightOutput;
	fs << "sizeInOut" << sizeInOut;

	mlp->write(*fs, "mlp"); // don't think too much about the deref, it casts to a FileNode

	fs.release();
}


template<typename T>
void BScanSegLocalOpNN::iterateBscanSeg(const cv::Mat& seg, T& op)
{
	int mapHeight = seg.rows-1; // -1 for p01
	int mapWidth  = seg.cols-1; // -1 for p10

	const int learnAbs = paintSizeHeightInput - 4;

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
		BScanSegLocalOpNN* parent;
		int transampelsSize = 0;
		int maskSizeInput;
	public:
		SumOp(BScanSegLocalOpNN* parent, int maskSizeInput) : parent(parent), maskSizeInput(maskSizeInput) {}
		void op(const cv::Mat& seg, int x, int y) { if(parent->getSubMapSize(seg, x, y) == maskSizeInput) ++transampelsSize; }
		int  getSize() const { return transampelsSize; }
	};

	class AddExapmelsOp
	{
		BScanSegLocalOpNN* parent;
		const cv::Mat& img;
		int actTrainSampel;
		int maskSizeInput;
		int maskSizeOutput;

		cv::Mat& tranSampels  ;
		cv::Mat& outputSampels;

		cv::Mat subImage, subSeg;
	public:
		AddExapmelsOp(BScanSegLocalOpNN* parent
		            , const cv::Mat& img
		            , int startTrainSampel
		            , cv::Mat& tranSampels
		            , cv::Mat& outputSampels
		            , int maskSizeInput
		            , int maskSizeOutput)
		: parent(parent)
		, img(img)
		, actTrainSampel(startTrainSampel)
		, maskSizeInput(maskSizeInput)
		, maskSizeOutput(maskSizeOutput)
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

	SumOp sumOp(this, maskSizeInput);
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

	AddExapmelsOp addExapmelsOp(this, img, startTrainSampel, *tranSampels, *outputSampels, maskSizeInput, maskSizeOutput);
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

void BScanSegLocalOpNN::setNeuronsPerHiddenLayer(const std::string& neuronsStr)
{
	neuronsPerHiddenLayer.clear();
	std::istringstream is(neuronsStr);
	int n;
	while(is >> n)
	{
		if(n>0)
			neuronsPerHiddenLayer.push_back(n);
	}
}

void BScanSegLocalOpNN::setNNConfig(const std::string& neuronsPerHiddenLayer, int widthIn, int heighIn, int widthOut, int heighOut)
{
	setNeuronsPerHiddenLayer(neuronsPerHiddenLayer);
	setInputOutputSize(widthIn, heighIn, widthOut, heighOut);
	createNN();
}



int BScanSegLocalOpNN::numExampels() const
{
	return tranSampels->rows;
}

const cv::Mat& BScanSegLocalOpNN::getLayerSizes() const
{
	static cv::Mat layers;
	const CvMat* layersC = mlp->get_layer_sizes();

	if(layersC)
		layers = cv::Mat(layersC);
	else
		layers = cv::Mat();

	return layers;
}




#endif


