#ifndef DWSLOIMAGE_H
#define DWSLOIMAGE_H

#include<QDockWidget>

class QResizeEvent;
namespace cv { class Mat; }

class SLOImageWidget;
class DWSloImage : public QDockWidget
{
	Q_OBJECT

	SLOImageWidget* imageWidget;

public:
	DWSloImage();
	~DWSloImage();

	void setSLOImage(const cv::Mat& image);

    virtual void resizeEvent(QResizeEvent* event);
};

#endif // DWSLOIMAGE_H
