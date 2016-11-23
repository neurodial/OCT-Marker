#ifndef BSCANSEGLOCALOPNN_H
#define BSCANSEGLOCALOPNN_H

#include "bscanseglocalop.h"

class CvANN_MLP;
class Callback;

class BScanSegLocalOpNN : public BScanSegLocalOp
{
	static const int paintSizeWidth   =  5;
	static const int paintSizeHeight  = 12;
	static const std::size_t maskSize = paintSizeWidth*paintSizeHeight*4;
	bool learningNN = true;

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
	int getOperatorWidth() const            override                { return paintSizeWidth ; }
	bool getLearningNN()   const                                    { return learningNN; }

	void setLearningNN(bool value)                                  { learningNN = value; }

	void learnBScan(Callback&);
	void learnBScans(std::size_t start, std::size_t end, Callback&);
};

#endif // BSCANSEGLOCALOPNN_H
