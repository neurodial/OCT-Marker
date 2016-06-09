#ifndef DWSLOIMAGE_H
#define DWSLOIMAGE_H

#include<QDockWidget>

class QMouseEvent;
class QWheelEvent;
class QResizeEvent;
namespace cv { class Mat; }

class SLOImageWidget;
class MarkerManager;

class DWSloImage : public QDockWidget
{
	Q_OBJECT

	SLOImageWidget* imageWidget;
	MarkerManager& markerManger;

public:
	DWSloImage(MarkerManager& markerManger);
	~DWSloImage();

	void setSLOImage(const cv::Mat& image);

    virtual void resizeEvent(QResizeEvent* event);

protected:
	virtual void wheelEvent       (QWheelEvent*);
    virtual void mouseReleaseEvent(QMouseEvent*);
};

#endif // DWSLOIMAGE_H
