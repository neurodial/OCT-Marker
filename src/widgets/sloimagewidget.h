#ifndef SLOIMAGEWIDGET_H
#define SLOIMAGEWIDGET_H

#include "cvimagewidget.h"
// #include <vector>

class BScan;
class ScaleFactor;
// class QColor;
class QPainter;
class MarkerManager;

class SLOImageWidget : public CVImageWidget
{
	Q_OBJECT

	MarkerManager& markerManger;
	
	// std::vector<QColor*> intervallColors;

	void createIntervallColors();
	void deleteIntervallColors();
public:
	SLOImageWidget(MarkerManager& markerManger);

    virtual ~SLOImageWidget();

protected:
	void paintEvent(QPaintEvent* event);

	void paintBScan    (QPainter& painter, const BScan& bscan, const ScaleFactor& factor, int bscanNr, bool paintMarker);
	void paintBScanLine(QPainter& painter, const BScan& bscan, const ScaleFactor& factor, int bscanNr, bool paintMarker);

private slots:
	void reladSLOImage();
	void bscanChanged(int);
};

#endif // SLOIMAGE_H

