#ifndef SLOIMAGEWIDGET_H
#define SLOIMAGEWIDGET_H

#include "cvimagewidget.h"
// #include <vector>
#include<data_structure/point2d.h>

namespace OctData
{
	class Series;
	class BScan;

	class CoordSLOpx;
	class ScaleFactor;
	class CoordTransform;
}

class QGraphicsScene;
class QGraphicsView;

// class QColor;
class QPainter;
class OctMarkerManager;
class SloMarkerBase;

class QWheelEvent;

class SLOImageWidget : public CVImageWidget
{
	Q_OBJECT

	struct Point {bool show = false; Point2DInt p; };

	Point markPos;

	OctMarkerManager& markerManger;
	QGraphicsView*  gv    = nullptr;
	QGraphicsScene* scene = nullptr;
	
	// std::vector<QColor*> intervallColors;
	bool drawBScans       = true;
	bool drawOnylActBScan = true;
	bool drawConvexHull   = true;
	bool singelBScanScan  = false;

	void createIntervallColors();
	void deleteIntervallColors();

	int getBScanNearPos(int x, int y, double tol);

	void updateGraphicsViewSize();
public:
	SLOImageWidget(QWidget* parent = 0);

    virtual ~SLOImageWidget();

	bool getShowBScans() const                                   { return drawBScans; }

	virtual void setImageSize(QSize size) override;

protected:
	virtual void wheelEvent       (QWheelEvent*);

public slots:
	void showBScans(bool show);
	void showOnylActBScan(bool show);

	void showPosOnBScan(const OctData::BScan* bscan, double t);

protected:
	void paintEvent(QPaintEvent* event) override;
	virtual void mousePressEvent(QMouseEvent*) override;

	void paintBScan      (QPainter& painter, const OctData::BScan& bscan, const OctData::ScaleFactor& factor, const OctData::CoordSLOpx& shift, const OctData::CoordTransform& transform, std::size_t bscanNr, bool paintMarker);
	void paintBScanLine  (QPainter& painter, const OctData::BScan& bscan, const OctData::ScaleFactor& factor, const OctData::CoordSLOpx& shift, const OctData::CoordTransform& transform, std::size_t bscanNr, bool paintMarker);
	void paintBScanCircle(QPainter& painter, const OctData::BScan& bscan, const OctData::ScaleFactor& factor, const OctData::CoordSLOpx& shift, const OctData::CoordTransform& transform, std::size_t bscanNr, bool paintMarker);

	void paintAnalyseGrid(QPainter& painter, const OctData::Series* series);
	void paintBScans     (QPainter& painter, const OctData::Series* series);
	void paintConvexHull (QPainter& painter, const OctData::Series* series);

private slots:
	void reladSLOImage();
	void bscanChanged(int);
	void sloMarkerChanged(SloMarkerBase* marker);
	void sloViewChanged  ();
};

#endif // SLOIMAGE_H

