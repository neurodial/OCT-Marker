#ifndef CVIMAGEWIDGET_H
#define CVIMAGEWIDGET_H


#include <QWidget>
#include <QImage>

#include <opencv2/opencv.hpp>


// http://develnoter.blogspot.de/2012/05/integrating-opencv-in-qt-gui.html

class QMenu;
class QContextMenuEvent;
class CVImageWidget : public QWidget
{
	Q_OBJECT
	virtual void contextMenuEvent(QContextMenuEvent* event);

	enum class ScaleMethod { Factor, Size };
	
	double scaleFactor = 1.;
	bool grayCvImage;
	ScaleMethod scaleMethod = ScaleMethod::Factor;
	
public:
	explicit CVImageWidget(QWidget *parent = 0);
	virtual ~CVImageWidget();

	QSize sizeHint()        const                               { return qtImage.size(); }
	QSize minimumSizeHint() const                               { return qtImage.size(); }

	virtual void setImageSize(QSize size)                       { imageScale  = size  ; scaleMethod = ScaleMethod::Size  ; cvImage2qtImage(); }
	virtual void setScaleFactor(double factor)                  { scaleFactor = factor; scaleMethod = ScaleMethod::Factor; cvImage2qtImage(); }

	int  imageHight() const;
	int  imageWidth() const;

	int  scaledImageHight() const                               { return qtImage.height(); }
	int  scaledImageWidth() const                               { return qtImage.width() ; }

	double getImageScaleFactor()                                { return scaleFactor; }

	void addZoomItems();

public slots:
	void showImage(const cv::Mat& image);

protected:
	void paintEvent(QPaintEvent* event);

	QImage   qtImage;
	cv::Mat  cvImage;
	QMenu*   contextMenu;
	QAction* saveAction;
	QSize    imageScale;
	
	
	QString translateFileFormat(const QString& format) const;
	int fileDialog(QString& filename);

	void cvImage2qtImage();
	void updateScaleFactor();
	
public slots:
	virtual void saveImage();


	void setZoom1()                                              { scaleFactor = 1; cvImage2qtImage(); repaint(); }
	void setZoom2()                                              { scaleFactor = 2; cvImage2qtImage(); repaint(); }
	void setZoom3()                                              { scaleFactor = 3; cvImage2qtImage(); repaint(); }
};

#endif // CVIMAGEWIDGET_H
