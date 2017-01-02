#ifndef BSCANSEGLOCALOPNN_H
#define BSCANSEGLOCALOPNN_H

#include "bscanseglocalop.h"

#ifdef ML_SUPPORT

class CvANN_MLP;
class Callback;

namespace cv { class Mat; }

class BScanSegLocalOpNN : public BScanSegLocalOp
{
	static const int paintSizeWidthInput    =  5;
	static const int paintSizeWidthOutput   =  1;
	static const int paintSizeHeight        = 12;
	static const std::size_t maskSizeInput  = paintSizeWidthInput *paintSizeHeight*4;
	static const std::size_t maskSizeOutput = paintSizeWidthOutput*paintSizeHeight*4;

	CvANN_MLP* mlp           = nullptr;
	cv::Mat*   tranSampels   = nullptr;
	cv::Mat*   outputSampels = nullptr;

	bool applyNN(int x, int y);

	int getSubMaps(cv::Mat& image, cv::Mat& seg, int x, int y);
	int getSubMaps(const cv::Mat& image, const cv::Mat& seg, cv::Mat* imageOut, cv::Mat* segOut, int x, int y);
	int getSubMapSize(const cv::Mat& mat, int x, int y);

	template<typename T>
	void iterateBscanSeg(const cv::Mat& seg, T& op);


	// void learnBScanMats(const cv::Mat& image, cv::Mat& seg, Callback&);
public:
	BScanSegLocalOpNN(BScanSegmentation& parent);
	~BScanSegLocalOpNN();


	void drawMarkerPaint(QPainter& painter, const QPoint& centerDrawPoint, double factor) const override;

	bool endOnCoord(int /*x*/, int /*y*/)   override                { return false; }
	bool drawOnCoord(int x, int y)          override                { return applyNN(x, y); }
	bool startOnCoord(int /*x*/, int /*y*/) override                { return false; }

	int getOperatorHeight()const            override                { return paintSizeHeight; }
	int getOperatorWidth() const            override                { return paintSizeWidthInput ; }

	void loadNN(const QString& file);
	void saveNN(const QString& file) const;

	int numExampels() const;
	void addBscanExampels();
	void trainNN(BScanSegmentationMarker::NNTrainData& trainData);


	int getInputHeight ()                                     const { return paintSizeHeight     ; }
	int getInputWidth  ()                                     const { return paintSizeWidthInput ; }
	int getOutputHeight()                                     const { return paintSizeHeight     ; }
	int getOutputWidth ()                                     const { return paintSizeWidthOutput; }


    const cv::Mat& getLayerSizes() const;
};

#else
class BScanSegLocalOpNN : public BScanSegLocalOp
{
	BScanSegLocalOpNN(BScanSegmentation& parent) : BScanSegLocalOp(parent) {} // null implementation, private is right
};
#endif
#endif // BSCANSEGLOCALOPNN_H
