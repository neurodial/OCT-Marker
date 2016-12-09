#ifndef BSCANSEGLOCALOPNN_H
#define BSCANSEGLOCALOPNN_H

#include "bscanseglocalop.h"

#ifdef ML_SUPPORT

class CvANN_MLP;
class Callback;

class BScanSegLocalOpNN : public BScanSegLocalOp
{
	static const int paintSizeWidthInput    =  5;
	static const int paintSizeWidthOutput   =  1;
	static const int paintSizeHeight        = 12;
	static const std::size_t maskSizeInput  = paintSizeWidthInput *paintSizeHeight*4;
	static const std::size_t maskSizeOutput = paintSizeWidthOutput*paintSizeHeight*4;
	bool learningNN = false;

	CvANN_MLP*             mlp    = nullptr;

	bool applyNN(int x, int y);
	void learnNN(int x, int y);

	void learnNNSubMat(const cv::Mat& image, const cv::Mat& seg);

	int getSubMaps(cv::Mat& image, cv::Mat& seg, int x, int y);
	int getSubMaps(cv::Mat& image, cv::Mat& seg, int x, int y, std::size_t bscanNr);
	int getSubMaps(const cv::Mat& image, const cv::Mat& seg, cv::Mat& imageOut, cv::Mat& segOut, int x, int y);
	int getSubMapSize(const cv::Mat& mat, int x, int y);


	void learnBScanMats(const cv::Mat& image, cv::Mat& seg, Callback&);
public:
	BScanSegLocalOpNN(BScanSegmentation& parent);


	void drawMarkerPaint(QPainter& painter, const QPoint& centerDrawPoint, double factor) const override;

	bool endOnCoord(int /*x*/, int /*y*/)   override                { return false; }
	bool drawOnCoord(int x, int y)          override                { if(!learningNN) return applyNN(x, y); learnNN(x, y); return false; }
	bool startOnCoord(int /*x*/, int /*y*/) override                { return false; }

	int getOperatorHeight()const            override                { return paintSizeHeight; }
	int getOperatorWidth() const            override                { return paintSizeWidthInput ; }
	bool getLearningNN()   const                                    { return learningNN; }

	void setLearningNN(bool value)                                  { learningNN = value; }

	void learnBScan(Callback&);
	void learnBScans(std::size_t start, std::size_t end, Callback&);

	void loadNN(const QString& file);
	void saveNN(const QString& file) const;
};

#else
class BScanSegLocalOpNN : public BScanSegLocalOp
{
	BScanSegLocalOpNN(BScanSegmentation& parent) : BScanSegLocalOp(parent) {} // null implementation, private is right
};
#endif
#endif // BSCANSEGLOCALOPNN_H
