#ifndef BSCANSEGLOCALOP_H
#define BSCANSEGLOCALOP_H

#include <QObject>

#include "configdata.h"


class QPainter;
class QPoint;
class BScanSegmentation;

namespace cv { class Mat; }
namespace OctData { class BScan; }

class BScanSegLocalOp
{
public:
	BScanSegLocalOp(BScanSegmentation& parent) : segmentation(parent) {}
	virtual ~BScanSegLocalOp() {}

	virtual void drawMarkerPaint(QPainter& painter, const QPoint& centerDrawPoint, int factor) const = 0;

	virtual bool endOnCoord(int x, int y)   = 0;
	virtual bool drawOnCoord(int x, int y)  = 0;
	virtual bool startOnCoord(int x, int y) = 0;

	virtual int getOperatorHeight()const = 0;
	virtual int getOperatorWidth() const = 0;

protected:
	BScanSegmentation& segmentation;

	cv::Mat* getActMat();
	const OctData::BScan* getActBScan();

	BScanSegmentationMarker::internalMatType valueOnCoord(int x, int y);

	void updateCursor();

	static const int minOperatorSize = 1;
	static const int maxOperatorSize = 100;

	int validOperatorSize(int size)
	{
		if(size < minOperatorSize)
			return minOperatorSize;
		if(size > maxOperatorSize)
			return maxOperatorSize;
		return size;
	}
};


class BScanSegLocalOpPaint : public BScanSegLocalOp
{
	BScanSegmentationMarker::PaintData localPaintData;
	BScanSegmentationMarker::internalMatType paintValue = BScanSegmentationMarker::markermatInitialValue;

	int paintSize = 10;
public:
	BScanSegLocalOpPaint(BScanSegmentation& parent) : BScanSegLocalOp(parent) {}


	void drawMarkerPaint(QPainter& painter, const QPoint& centerDrawPoint, int factor) const override;

	bool endOnCoord(int /*x*/, int /*y*/)   override                { return false; }
	bool drawOnCoord(int x, int y)          override;
	bool startOnCoord(int x, int y)         override;

	int getOperatorHeight()const            override                { return paintSize; }
	int getOperatorWidth() const            override                { return paintSize; }


	void setOperatorSize(int size);
	void setPaintData(const BScanSegmentationMarker::PaintData& data);

	const BScanSegmentationMarker::PaintData& getPaintData() const  { return localPaintData; }
};




class BScanSegLocalOpThreshold : public BScanSegLocalOp
{
	int paintSizeWidth  =  8;
	int paintSizeHeight = 16;

	BScanSegmentationMarker::ThresholdData localThresholdData;
public:
	BScanSegLocalOpThreshold(BScanSegmentation& parent) : BScanSegLocalOp(parent) {}


	void drawMarkerPaint(QPainter& painter, const QPoint& centerDrawPoint, int factor) const override;

	bool endOnCoord(int x, int y)           override;
	bool drawOnCoord(int /*x*/, int /*y*/)  override                { return false; }
	bool startOnCoord(int /*x*/, int /*y*/) override                { return false; }

	int getOperatorHeight()const            override                { return paintSizeHeight; }
	int getOperatorWidth() const            override                { return paintSizeWidth ; }


	void setOperatorSizeWidth (int size);
	void setOperatorSizeHeight(int size);
	void setThresholdData(const BScanSegmentationMarker::ThresholdData& data);
};




class BScanSegLocalOpOperation : public BScanSegLocalOp
{
	BScanSegmentationMarker::Operation     localOperation;
	int paintSizeWidth  = 10;
	int paintSizeHeight = 10;
public:
	BScanSegLocalOpOperation(BScanSegmentation& parent) : BScanSegLocalOp(parent) {}


	void drawMarkerPaint(QPainter& painter, const QPoint& centerDrawPoint, int factor) const override;

	bool endOnCoord(int x, int y)            override;
	bool drawOnCoord(int /*x*/, int /*y*/)   override               { return false; }
	bool startOnCoord(int /*x*/, int /*y*/)  override               { return false; }

	int getOperatorHeight()const            override                { return paintSizeHeight; }
	int getOperatorWidth() const            override                { return paintSizeWidth ; }


	void setOperatorSizeWidth (int size);
	void setOperatorSizeHeight(int size);
	void setOperationData(const BScanSegmentationMarker::Operation& data);
	const BScanSegmentationMarker::Operation& getOperationData() const { return localOperation; }
};
#endif // BSCANSEGLOCALOP_H