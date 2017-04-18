#ifndef SLOIMAGEWIDGET_H
#define SLOIMAGEWIDGET_H

#include "cvimagewidget.h"
// #include <vector>

namespace OctData
{
	class CoordSLOpx;
	class BScan;
	class ScaleFactor;
	class CoordTransform;
}

class QGraphicsScene;
class QGraphicsView;

// class QColor;
class QPainter;
class OctMarkerManager;
class SloMarkerBase;

class SLOImageWidget : public CVImageWidget
{
	Q_OBJECT

	OctMarkerManager& markerManger;
	QGraphicsView*  gv    = nullptr;
	QGraphicsScene* scene = nullptr;
	
	// std::vector<QColor*> intervallColors;
	bool drawBScans = true;
	bool drawOnylActBScan = true;
	bool singelBScanScan  = false;

	void createIntervallColors();
	void deleteIntervallColors();

	int getBScanNearPos(int x, int y, double tol);

	void updateGraphicsViewSize();
public:
	SLOImageWidget();

    virtual ~SLOImageWidget();

	bool getShowBScans() const                                   { return drawBScans; }

	virtual void setImageSize(QSize size) override;

public slots:
	void showBScans(bool show);
	void showOnylActBScan(bool show);

protected:
	void paintEvent(QPaintEvent* event) override;
	virtual void mousePressEvent(QMouseEvent*) override;

	void paintBScan      (QPainter& painter, const OctData::BScan& bscan, const OctData::ScaleFactor& factor, const OctData::CoordSLOpx& shift, const OctData::CoordTransform& transform, std::size_t bscanNr, bool paintMarker);
	void paintBScanLine  (QPainter& painter, const OctData::BScan& bscan, const OctData::ScaleFactor& factor, const OctData::CoordSLOpx& shift, const OctData::CoordTransform& transform, std::size_t bscanNr, bool paintMarker);
	void paintBScanCircle(QPainter& painter, const OctData::BScan& bscan, const OctData::ScaleFactor& factor, const OctData::CoordSLOpx& shift, const OctData::CoordTransform& transform, std::size_t bscanNr, bool paintMarker);

private slots:
	void reladSLOImage();
	void bscanChanged(int);
	void sloMarkerChanged(SloMarkerBase* marker);
	void sloViewChanged  ();
};

#endif // SLOIMAGE_H

