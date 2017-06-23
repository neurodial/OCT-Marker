#ifndef BSCANSEGLOCALOPNN_H
#define BSCANSEGLOCALOPNN_H

#include "bscanseglocalop.h"

#include <cstdint>
#include <vector>

#ifdef ML_SUPPORT

class Callback;

namespace cv { class Mat; }
namespace FANN { class neural_net; }

class BScanSegLocalOpNN : public BScanSegLocalOp
{
public:
	class CallbackInOutNeurons
	{
	public:
		virtual void processedInOutNeurons(const cv::Mat& in, const cv::Mat& out) const = 0;
	};
private:
	int paintSizeWidthInput   = 10;
	int paintSizeWidthOutput  =  1;
	int paintSizeHeightInput  = 24;
	int paintSizeHeightOutput = 18;
	int maskSizeInput         = 0;
	int maskSizeOutput        = 0;

	const CallbackInOutNeurons* callbackInOutNeurons = nullptr;


	FANN::neural_net* nNet   = nullptr;
	cv::Mat*   tranSampels   = nullptr;
	cv::Mat*   outputSampels = nullptr;

	std::vector<int> neuronsPerHiddenLayer = {50};

	bool applyNN(int x, int y);

	int getSubMaps(cv::Mat& image, cv::Mat& seg, int x, int y);
	int getSubMaps(const cv::Mat& image, const cv::Mat& seg, cv::Mat* imageOut, cv::Mat* segOut, int x, int y);
	int getSubMapSize(const cv::Mat& mat, int x, int y);

	template<typename T>
	void iterateBscanSeg(const cv::Mat& seg, T& op);

	void createNN();
	void calcMaskSizes();
	void setInputOutputSize(int widthIn, int heighIn, int widthOut, int heighOut);
	void setNeuronsPerHiddenLayer(const std::string& neuronsStr);


	void convertInputMat(const cv::Mat& in, cv::Mat& out)                       const;
	void convertOutputMat(const cv::Mat& in, cv::Mat& out, bool learnDirection) const;
public:

	BScanSegLocalOpNN(BScanSegmentation& parent);
	~BScanSegLocalOpNN();


	void drawMarkerPaint(QPainter& painter, const QPoint& centerDrawPoint, double factor) const override;

	bool endOnCoord(int /*x*/, int /*y*/)   override                { return false; }
	bool drawOnCoord(int x, int y)          override                { return applyNN(x, y); }
	bool startOnCoord(int /*x*/, int /*y*/) override                { return false; }

	int getOperatorHeight()const            override                { return paintSizeHeightInput/2+1; }
	int getOperatorWidth() const            override                { return paintSizeWidthInput /2+1; }

	void loadNN(const QString& file);
	void saveNN(const QString& file) const;

	int numExampels() const;
	void addBscanExampels();
	void trainNN(BScanSegmentationMarker::NNTrainData& trainData, Callback& callback);

	void setCallbackInOutNeurons(const CallbackInOutNeurons* callback)
	                                                                { callbackInOutNeurons = callback; }

	int getInputHeight ()                                     const { return paintSizeHeightInput ; }
	int getInputWidth  ()                                     const { return paintSizeWidthInput  ; }
	int getOutputHeight()                                     const { return paintSizeHeightOutput; }
	int getOutputWidth ()                                     const { return paintSizeWidthOutput ; }

	void setNNConfig(const std::string& neuronsPerHiddenLayer, int widthIn, int heighIn, int widthOut, int heighOut);

    const std::vector<unsigned int> getLayerSizes() const;
};

#else
class BScanSegLocalOpNN : public BScanSegLocalOp
{
	BScanSegLocalOpNN(BScanSegmentation& parent) : BScanSegLocalOp(parent) {} // null implementation, private scope is correct
};
#endif
#endif // BSCANSEGLOCALOPNN_H
