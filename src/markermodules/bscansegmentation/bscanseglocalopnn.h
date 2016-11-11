#ifndef BSCANSEGLOCALOPNN_H
#define BSCANSEGLOCALOPNN_H

#include "bscanseglocalop.h"

class CvANN_MLP;

class BScanSegLocalOpNN : public BScanSegLocalOp
{
	static const int paintSizeWidth   =  6;
	static const int paintSizeHeight  = 16;
	bool learningNN = true;

	CvANN_MLP*             mlp    = nullptr;

	bool applyNN(int x, int y);
	void learnNN(int x, int y);

	void learnNNSubMat(const cv::Mat& image, const cv::Mat& seg);

	bool getSubMaps(cv::Mat& image, cv::Mat& seg, int x, int y);
	bool getSubMaps(cv::Mat& image, cv::Mat& seg, int x, int y, std::size_t bscanNr);
	bool getSubMaps(const cv::Mat& image, const cv::Mat& seg, cv::Mat& imageOut, cv::Mat& segOut, int x, int y);


	void learnBScanMats(const cv::Mat& image, cv::Mat& seg);
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

	void learnBScan();
	void learnBScans(int start, int end);
};

#endif // BSCANSEGLOCALOPNN_H
