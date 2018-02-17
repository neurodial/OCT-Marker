#ifndef WGSLOIMAGE_H
#define WGSLOIMAGE_H

#include <QMainWindow>


class OctMarkerManager;
class SloWithLegendWidget;
class SLOImageWidget;


class QGridLayout;


class WgSloImage : public QMainWindow
{
	Q_OBJECT
	
	OctMarkerManager& markerManager;
	SloWithLegendWidget* imageWidget;
	void createMarkerToolbar();
public:
	explicit WgSloImage(QWidget* parent = nullptr);
	~WgSloImage();
	
	SLOImageWidget* getImageWidget();

protected:
	virtual void wheelEvent       (QWheelEvent*);

};

#endif // WGSLOIMAGE_H
