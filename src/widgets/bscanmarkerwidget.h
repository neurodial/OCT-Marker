#ifndef BSCANMARKERWIDGET_H
#define BSCANMARKERWIDGET_H

#include "cvimagewidget.h"

#include <QPoint>

namespace OctData { class BScan; }

class QWheelEvent;
class QMouseEvent;
class QGraphicsScene;
class QGraphicsView;

class OctMarkerManager;
class BscanMarkerBase;

class ContureSegment;

class BScanMarkerWidget : public CVImageWidget
{
	Q_OBJECT

	OctMarkerManager& markerManger;
	QGraphicsView*  gv    = nullptr;
	QGraphicsScene* scene = nullptr;

	
	QAction*                                saveRawImageAction = nullptr;
	QAction*                                saveRawMatAction   = nullptr;
	QAction*                                saveRawBinAction   = nullptr;
	QAction*                                saveImageBinAction = nullptr;
// 	const OctData::BScan*                   actBscan           = nullptr;

	bool controlUsed = false;
	
	int fdSaveRaw(QString& filename);
	void updateRawExport();
	bool existsRaw(const OctData::BScan* bscan) const;
	bool rawSaveableAsImage() const;

	bool checkControlUsed(QMouseEvent* event);
	bool checkControlUsed(QKeyEvent  * event);
	bool checkControlUsed(bool modPressed);

	void paintConture(QPainter& painter, const std::vector<ContureSegment>& contours);


	void transformCoordWidget2Img(int xWidget, int yWidget, int& xImg, int& yImg)
	{
		const ScaleFactor& factor = getImageScaleFactor();
		yImg = static_cast<int>(yWidget/factor.getFactorX() + 0.5);
		xImg = static_cast<int>(xWidget/factor.getFactorY() + 0.5);
	}

public:
	BScanMarkerWidget();

	virtual ~BScanMarkerWidget();

	static void paintSegmentationLine(QPainter& segPainter, int bScanHeight, const std::vector<double>& segLine, const ScaleFactor& factor);

protected:
	virtual void paintEvent(QPaintEvent* event) override;
	virtual void contextMenuEvent(QContextMenuEvent* event) override;


    bool event(QEvent *event) override;

	virtual void mouseMoveEvent   (QMouseEvent*) override;
	virtual void mousePressEvent  (QMouseEvent*) override;
	virtual void mouseReleaseEvent(QMouseEvent*) override;

	virtual void keyPressEvent    (QKeyEvent*  ) override;
	virtual void keyReleaseEvent  (QKeyEvent*  ) override;
	virtual void leaveEvent       (QEvent*     ) override;
	virtual void wheelEvent       (QWheelEvent*) override;

private slots:
	void imageChanged();
	void cscanLoaded();

	void viewOptionsChangedSlot();
	void updateGraphicsViewSize();

	void markersMethodChanged(BscanMarkerBase* marker);
	

public slots:
	virtual void saveRawImage();
	virtual void saveRawMat  ();
	virtual void saveRawBin  ();
	virtual void saveImageBin();

	virtual void showImage(const cv::Mat& image) override;
	
signals:
	void bscanChangeInkrement(int delta);
	void mousePosInImage(int x, int y);
	void mouseLeaveImage();
	void mousePosOnBScan(double t);

};

#endif // BSCANMARKERWIDGET_H
