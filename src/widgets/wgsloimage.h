#ifndef WGSLOIMAGE_H
#define WGSLOIMAGE_H

#include <QMainWindow>


class MarkerManager;
class SLOImageWidget;


class WgSloImage : public QMainWindow
{
	SLOImageWidget* imageWidget;
	MarkerManager& markerManger;
	
public:
    explicit WgSloImage(MarkerManager& markerManger);
	
	
	virtual void resizeEvent(QResizeEvent* event);

protected:
	virtual void wheelEvent       (QWheelEvent*);
};

#endif // WGSLOIMAGE_H
