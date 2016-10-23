#ifndef BSCANSEGMENTATION_H
#define BSCANSEGMENTATION_H


#include "bscanmarkerbase.h"

#include <boost/icl/interval_map.hpp>
#include <data_structure/intervallmarker.h>
#include <vector>

#include <QPoint>

class QAction;

class BScanMarkerManager;

namespace cv { class Mat; }


class BScanSegmentation : public BscanMarkerBase
{
	Q_OBJECT
	
	typedef uint8_t internalMatType;
	typedef std::vector<cv::Mat*> SegMats;
	
	static const internalMatType paintArea0Value = 0;
	static const internalMatType paintArea1Value = 1;
	
	bool inWidget = false;
	QPoint mousePoint;
	
	bool paint = false;
	int paintRadius = 10;
	internalMatType paintValue = 0;
	bool autoPaintValue = true;
	
	SegMats segments;
	
	void clearSegments();
	void drawSegmentLine(QPainter&, int factor);
	
	bool setOnCoord(int x, int y, int factor);
	internalMatType valueOnCoord(int x, int y, int factor);
public:
	BScanSegmentation(BScanMarkerManager* markerManager);
	virtual ~BScanSegmentation()                                    { clearSegments(); }

	QToolBar* createToolbar(QObject* parent) override;
	
	virtual void drawMarker(QPainter&, BScanMarkerWidget*) override;
	virtual bool drawBScan() const                         override { return true;  }
	
	virtual bool mouseMoveEvent   (QMouseEvent*, BScanMarkerWidget*) override;
	virtual bool mousePressEvent  (QMouseEvent*, BScanMarkerWidget*) override;
	virtual bool mouseReleaseEvent(QMouseEvent*, BScanMarkerWidget*) override;
	virtual bool keyPressEvent    (QKeyEvent*  , BScanMarkerWidget*) override;
	virtual bool leaveWidgetEvent (QEvent*     , BScanMarkerWidget*) override;
	
	
private slots:
	virtual void newSeriesLoaded(const OctData::Series* series);
	
	virtual void paintArea0Slot()                                   { paintValue  = paintArea0Value; autoPaintValue = false; }
	virtual void paintArea1Slot()                                   { paintValue  = paintArea1Value; autoPaintValue = false; }
	virtual void autoAddRemoveArea()                                { autoPaintValue = true; }
	virtual void setPaintRadius(int r)                              { paintRadius = r; }
	
	virtual void initFromSegmentline();
	
};

#endif // BSCANSEGMENTATION_H
