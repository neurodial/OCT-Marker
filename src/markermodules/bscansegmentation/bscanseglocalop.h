#ifndef BSCANSEGLOCALOP_H
#define BSCANSEGLOCALOP_H

#include <QObject>

#include "configdata.h"


class QPainter;
class QPoint;
class BScanSegmentation;
class ScaleFactor;

namespace cv { class Mat; }
namespace OctData { class BScan; }

class BScanSegLocalOp
{
public:
	BScanSegLocalOp(BScanSegmentation& parent) : segmentation(parent) {}
	virtual ~BScanSegLocalOp() {}

	virtual void drawMarkerPaint(QPainter& painter, const QPoint& centerDrawPoint, const ScaleFactor& factor) const = 0;
	virtual bool drawMarker() const                                 { return true; }

	virtual bool endOnCoord(int x, int y)   = 0;
	virtual bool drawOnCoord(int x, int y)  = 0;
	virtual bool startOnCoord(int x, int y) = 0;

	virtual int getOperatorHeight()const = 0;
	virtual int getOperatorWidth() const = 0;

protected:
	BScanSegmentation& segmentation;

	cv::Mat* getActMat();
	const OctData::BScan* getActBScan();

	std::size_t getBScanNr();

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


	void drawMarkerPaint(QPainter& painter, const QPoint& centerDrawPoint, const ScaleFactor& factor) const override;
	bool drawMarker() const                 override                { return localPaintData.paintMethod != BScanSegmentationMarker::PaintData::PaintMethod::Pen; }

	bool endOnCoord(int /*x*/, int /*y*/)   override                { return false; }
	bool drawOnCoord(int x, int y)          override;
	bool startOnCoord(int x, int y)         override;

	int getOperatorHeight()const            override                { return paintSize; }
	int getOperatorWidth() const            override                { return paintSize; }


	void setOperatorSize(int size);
	void setPaintData(const BScanSegmentationMarker::PaintData& data);

	const BScanSegmentationMarker::PaintData& getPaintData() const  { return localPaintData; }
	QIcon getPaintColorIcon(BScanSegmentationMarker::PaintData::PaintColor color) const;
};




class BScanSegLocalOpThresholdDirection : public BScanSegLocalOp
{
	int  paintSizeWidth   =  4;
	int  paintSizeHeight  = 16;
	bool applyOnMouseMove = true;

	BScanSegmentationMarker::ThresholdDirectionData localThresholdData;

	bool applyThreshold(int x, int y);
public:
	BScanSegLocalOpThresholdDirection(BScanSegmentation& parent) : BScanSegLocalOp(parent) {}


	void drawMarkerPaint(QPainter& painter, const QPoint& centerDrawPoint, const ScaleFactor& factor) const override;

	bool endOnCoord(int x, int y)           override                { return applyThreshold(x, y); }
	bool drawOnCoord(int x, int y)          override                { if(applyOnMouseMove) return applyThreshold(x, y); return false; }
	bool startOnCoord(int /*x*/, int /*y*/) override                { return false; }

	int getOperatorHeight()const            override                { return paintSizeHeight; }
	int getOperatorWidth() const            override                { return paintSizeWidth ; }
	bool getApplyOnMouseMove() const                                { return applyOnMouseMove; }


	void setOperatorSizeWidth (int size);
	void setOperatorSizeHeight(int size);
	void setThresholdData(const BScanSegmentationMarker::ThresholdDirectionData& data);
	void setApplyOnMouseMove(bool value)                            { applyOnMouseMove = value; }
};


class BScanSegLocalOpThreshold : public BScanSegLocalOp
{
	int  paintSizeWidth   = 10;
	int  paintSizeHeight  = 10;
	bool applyOnMouseMove = true;

	BScanSegmentationMarker::ThresholdData localThresholdData;

	bool applyThreshold(int x, int y);
public:
	BScanSegLocalOpThreshold(BScanSegmentation& parent) : BScanSegLocalOp(parent) {}


	void drawMarkerPaint(QPainter& painter, const QPoint& centerDrawPoint, const ScaleFactor& factor) const override;

	bool endOnCoord(int x, int y)           override                { return applyThreshold(x, y); }
	bool drawOnCoord(int x, int y)          override                { if(applyOnMouseMove) return applyThreshold(x, y); return false; }
	bool startOnCoord(int /*x*/, int /*y*/) override                { return false; }

	int getOperatorHeight()const            override                { return paintSizeHeight; }
	int getOperatorWidth() const            override                { return paintSizeWidth ; }
	bool getApplyOnMouseMove() const                                { return applyOnMouseMove; }


	void setOperatorSizeWidth (int size);
	void setOperatorSizeHeight(int size);
	void setThresholdData(const BScanSegmentationMarker::ThresholdData& data);
	void setApplyOnMouseMove(bool value)                            { applyOnMouseMove = value; }

	const BScanSegmentationMarker::ThresholdData& getLocalThresholdData() const { return localThresholdData; }
	int getOperatorSizeWidth () const                               { return paintSizeWidth ; }
	int getOperatorSizeHeight() const                               { return paintSizeHeight; }
};



class BScanSegLocalOpOperation : public BScanSegLocalOp
{
	BScanSegmentationMarker::Operation localOperation = BScanSegmentationMarker::Operation::Dilate;
	int paintSizeWidth  = 10;
	int paintSizeHeight = 10;
public:
	BScanSegLocalOpOperation(BScanSegmentation& parent) : BScanSegLocalOp(parent) {}


	void drawMarkerPaint(QPainter& painter, const QPoint& centerDrawPoint, const ScaleFactor& factor) const override;

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
