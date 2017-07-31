#include "bscanseglocalopnn.h"

#ifdef ML_SUPPORT

#include <QPainter>

#include <opencv/cv.h>
#include <opencv/ml.h>

#include <octdata/datastruct/bscan.h>

#include <helper/callback.h>

#include <fann.h>
#include <fann_cpp.h>

#include <cmath>
#include <algorithm>

namespace
{
	void getRelOpSize(int& x1, int& x2, int& y1, int& y2, int x, int y)
	{
		x1 = (x+1)/2;
		x2 = x/2;
		y1 = (y+1)/2;
		y2 = y/2;
	}

	typedef struct user_context_type
	{
		FANN::callback_type user_callback; // Pointer to user callback function
		void *user_data; // Arbitrary data pointer passed to the callback
		FANN::neural_net *net; // This pointer for the neural network
	} user_context;

//         // Internal callback used to convert from pointers to class references
//         static int FANN_API internal_callback(struct fann *ann, struct fann_train_data *train,
//             unsigned int max_epochs, unsigned int epochs_between_reports, float desired_error, unsigned int epochs)
//         {
//             user_context *user_data = static_cast<user_context *>(fann_get_user_data(ann));
//             if (user_data != NULL)


	int nn_callback(FANN::neural_net &net
	              , FANN::training_data& /*train*/
	              , unsigned int max_epochs
	              , unsigned int /*epochs_between_reports*/
	              , float        /*desired_error*/
	              , unsigned int epochs
	              , void* user_data2)
	{
		std::cout << "Epochs     " << std::setw(8) << epochs << ". " << "Current Error: " << std::left << net.get_MSE() << std::right << std::endl;

		user_context* context = static_cast<user_context *>(user_data2);
		void* user_data = context->user_data;
		if(user_data)
		{
// 			BScanSegLocalOpNN* nn = reinterpret_cast<BScanSegLocalOpNN*>(user_data);
// 			if(nn->callCallback(static_cast<double>(epochs)/static_cast<double>(max_epochs)))
			Callback* callback = reinterpret_cast<Callback*>(user_data);
			QString text = QString("Current Error: %1").arg(net.get_MSE());
			bool continueTrain = callback->callback(static_cast<double>(epochs)/static_cast<double>(max_epochs), text.toUtf8().data());

			return continueTrain?0:-1;
		}
		return 0;
	}
}



BScanSegLocalOpNN::BScanSegLocalOpNN(BScanSegmentation& parent)
: BScanSegLocalOp(parent)
, nNet(new FANN::neural_net)
{
	calcMaskSizes();
	createNN();
}

BScanSegLocalOpNN::~BScanSegLocalOpNN()
{
// 	delete mlp;
	delete nNet;
	delete tranSampels  ;
	delete outputSampels;
}

void BScanSegLocalOpNN::createNN()
{
	if(maskSizeInput > 0 && maskSizeOutput > 0)
	{
		std::vector<unsigned int> layers;
		layers.push_back(static_cast<unsigned>(maskSizeInput));
		layers.insert(layers.end(), neuronsPerHiddenLayer.begin(), neuronsPerHiddenLayer.end());
		layers.push_back(static_cast<unsigned>(maskSizeOutput));

		nNet->create_standard_array(static_cast<unsigned>(layers.size()), layers.data());

		if(!tranSampels || !outputSampels
		 || tranSampels  ->rows*tranSampels  ->cols != maskSizeInput
		 || outputSampels->rows*outputSampels->cols != maskSizeOutput)
		{
			delete tranSampels;
			delete outputSampels;

			tranSampels   = nullptr;
			outputSampels = nullptr;
		}
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


namespace
{
	void convert2FloatRow(const cv::Mat& in, cv::Mat& out, double factor, double add)
	{
		cv::Mat tmp;
		in.copyTo(tmp);
		tmp.convertTo(tmp, cv::DataType<fann_type>::type, factor, add);
		out = tmp.reshape(0, 1);
	}

}

void BScanSegLocalOpNN::convertInputMat(const cv::Mat& in, cv::Mat& out) const
{
	convert2FloatRow(in, out, 1./255., 0);
}
void BScanSegLocalOpNN::convertOutputMat(const cv::Mat& in, cv::Mat& out, bool learnDirection) const
{
	if(learnDirection)
		convert2FloatRow(in, out, 1./BScanSegmentationMarker::paintArea1Value, 0);
	else
	{
		in.convertTo(out, cv::DataType<uint8_t>::type, BScanSegmentationMarker::paintArea1Value, 0);
		out.reshape(0, paintSizeHeightOutput);
	}
}

bool BScanSegLocalOpNN::applyNN(int x, int y)
{

	cv::Mat image, seg;
	getSubMaps(image, seg, x, y);

	if(image.rows*image.cols != maskSizeInput || seg.rows*seg.cols != maskSizeOutput)
		return false;

	cv::Mat imageFloat;
	convertInputMat(image, imageFloat);

	fann_type* output = nNet->run(imageFloat.ptr<fann_type>());
	cv::Mat segFloat(1, maskSizeOutput, cv::DataType<fann_type>::type, output);

	if(callbackInOutNeurons)
		callbackInOutNeurons->processedInOutNeurons(image, segFloat.reshape(0, paintSizeHeightOutput));


	segFloat.convertTo(segFloat, cv::DataType<uint8_t>::type, BScanSegmentationMarker::paintArea1Value, 0);
	segFloat.reshape(0, seg.rows).copyTo(seg);


	return true;
}


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
	nNet->create_from_file(file.toUtf8().data());
}


void BScanSegLocalOpNN::saveNN(const QString& file) const
{
	nNet->save(file.toUtf8().data());
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
				parent->convertInputMat (subImage, imageFloatLin);
				parent->convertOutputMat(subSeg  , segFloatLin  , true);

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

void BScanSegLocalOpNN::trainNN(BScanSegmentationMarker::NNTrainData& trainData, Callback& callback)
{
	if((tranSampels == nullptr) || (outputSampels == nullptr) || tranSampels->rows != outputSampels->rows) // TODO: Error Message
		return;


	FANN::training_data data;

	unsigned int num_data   = static_cast<unsigned>(tranSampels->rows);
	unsigned int num_input  = static_cast<unsigned>(tranSampels->cols);
	fann_type**  input      = new fann_type*[num_data];
// 	fann_type*   input      = tranSampels->ptr<fann_type>();
	unsigned int num_output = static_cast<unsigned>(outputSampels->cols);
	fann_type**  output     = new fann_type*[num_data];
// 	fann_type*   output     = outputSampels->ptr<fann_type>();

	fann_type* inIt  = tranSampels  ->ptr<fann_type>();
	fann_type* outIt = outputSampels->ptr<fann_type>();

	for(unsigned i = 0; i<num_data; ++i)
	{
		input [i] = inIt ;
		output[i] = outIt;
		inIt  += tranSampels->cols;
		outIt += outputSampels->cols;
	}

	data.set_train_data(num_data, num_input, input, num_output, output);

	callback.callback(0.);

	nNet->set_callback(nn_callback, &callback);
	nNet->train_on_data(data, trainData.maxIterations, 1, static_cast<float>(trainData.epsilon));

	nNet->set_callback(nn_callback, nullptr);
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
	if(tranSampels)
		return tranSampels->rows;
	return 0;
}

const std::vector<unsigned int> BScanSegLocalOpNN::getLayerSizes() const
{
	std::vector<unsigned int> layers(nNet->get_num_layers());
	nNet->get_layer_array(layers.data());

	return layers;
}




#endif


