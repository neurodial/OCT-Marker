#ifndef WGSLOIMAGE_H
#define WGSLOIMAGE_H

#include <QMainWindow>


class OctMarkerManager;
class SLOImageWidget;


class WgSloImage : public QMainWindow
{
	Q_OBJECT
	
	SLOImageWidget* imageWidget;
	OctMarkerManager& markerManager;

	void createMarkerToolbar();
public:
	explicit WgSloImage(OctMarkerManager& markerManager, QWidget* parent = nullptr);
	
	
	virtual void resizeEvent(QResizeEvent* event);

protected:
	virtual void wheelEvent       (QWheelEvent*);
};

#endif // WGSLOIMAGE_H
