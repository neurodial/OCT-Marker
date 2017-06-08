#ifndef BSCANLAYERSEGMENTATION_H
#define BSCANLAYERSEGMENTATION_H

#include<octdata/datastruct/segmentationlines.h>

#include "../bscanmarkerbase.h"

class BScanLayerSegmentation : public BscanMarkerBase
{
	Q_OBJECT

	struct SegPoint
	{
		SegPoint() : x(0), y(0) {}
		SegPoint(std::size_t x, double y) : x(x), y(y) {}

		std::size_t x;
		double      y;
	};
	std::vector<OctData::Segmentationlines> lines;

	void resetMarkers(const OctData::Series* series);

	SegPoint lastPoint;
	bool paintSegLine = false;

	SegPoint calcPoint(int x, int y, double scaleFactor, int bscanWidth);

	void setLinePoint2Point(const SegPoint& p1, const SegPoint& p2, OctData::Segmentationlines::Segmentline& segLine);

	QRect getWidgetPaintSize(const SegPoint& p1, const SegPoint& p2, double scaleFactor);

public:
	BScanLayerSegmentation(OctMarkerManager* markerManager);

	virtual void drawMarker(QPainter& painter, BScanMarkerWidget* widget, const QRect& /*drawrect*/) const;

	virtual RedrawRequest mouseMoveEvent   (QMouseEvent*, BScanMarkerWidget*);
	virtual RedrawRequest mousePressEvent  (QMouseEvent*, BScanMarkerWidget*);
	virtual RedrawRequest mouseReleaseEvent(QMouseEvent*, BScanMarkerWidget*);


	virtual void newSeriesLoaded(const OctData::Series* series, boost::property_tree::ptree& ptree);
};

#endif // BSCANLAYERSEGMENTATION_H
