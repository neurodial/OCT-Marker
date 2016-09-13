#ifndef WGSLOIMAGE_H
#define WGSLOIMAGE_H

#include <QMainWindow>


class BScanMarkerManager;
class SLOImageWidget;


class WgSloImage : public QMainWindow
{
	Q_OBJECT
	
	SLOImageWidget* imageWidget;
	BScanMarkerManager& markerManger;
public:
	explicit WgSloImage(BScanMarkerManager& markerManger);
	
	
	virtual void resizeEvent(QResizeEvent* event);

protected:
	virtual void wheelEvent       (QWheelEvent*);
};

#endif // WGSLOIMAGE_H
