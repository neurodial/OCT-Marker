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

class SLOImageWidget : public CVImageWidget
{
	Q_OBJECT

	OctMarkerManager& markerManger;
	QGraphicsView* gv = nullptr;
	QGraphicsScene* scene = nullptr;
	
	// std::vector<QColor*> intervallColors;
	bool drawBScans = true;
	bool drawOnylActBScan = true;

	void createIntervallColors();
	void deleteIntervallColors();
public:
	SLOImageWidget(OctMarkerManager& markerManger);

    virtual ~SLOImageWidget();

	bool getShowBScans() const                                   { return drawBScans; }

	virtual void setImageSize(QSize size);

public slots:
	void showBScans(bool show);
	void showOnylActBScan(bool show);
	void showLabels(bool show);

protected:
	void paintEvent(QPaintEvent* event);

	void paintBScan    (QPainter& painter, const OctData::BScan& bscan, const OctData::ScaleFactor& factor, const OctData::CoordSLOpx& shift, const OctData::CoordTransform& transform, int bscanNr, bool paintMarker);
	void paintBScanLine(QPainter& painter, const OctData::BScan& bscan, const OctData::ScaleFactor& factor, const OctData::CoordSLOpx& shift, const OctData::CoordTransform& transform, int bscanNr, bool paintMarker);

private slots:
	void reladSLOImage();
	void bscanChanged(int);
};

#endif // SLOIMAGE_H

