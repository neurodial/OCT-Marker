#ifndef DWSLOIMAGE_H
#define DWSLOIMAGE_H

#include<QDockWidget>

class WgSloImage;
class QMouseEvent;
class QWheelEvent;
class QResizeEvent;
namespace cv { class Mat; }

class MarkerManager;

class DWSloImage : public QDockWidget
{
	Q_OBJECT
	
	WgSloImage* sloWidget;

public:
	DWSloImage(MarkerManager& markerManger);
	~DWSloImage();

	// void setSLOImage(const cv::Mat& image);

	// virtual void resizeEvent(QResizeEvent* event);
};

#endif // DWSLOIMAGE_H
