#ifndef CVIMAGEWIDGET_H
#define CVIMAGEWIDGET_H


#include <QWidget>
#include <QImage>
#include <QPainter>

#include <opencv2/opencv.hpp>


// http://develnoter.blogspot.de/2012/05/integrating-opencv-in-qt-gui.html

class QMenu;
class QContextMenuEvent;
class CVImageWidget : public QWidget
{
	Q_OBJECT
	virtual void contextMenuEvent(QContextMenuEvent* event);
	
	double scaleFactor = 1.;
	
public:
	explicit CVImageWidget(QWidget *parent = 0);
	virtual ~CVImageWidget();

	QSize sizeHint()        const                               { return qtImage.size(); }
	QSize minimumSizeHint() const                               { return qtImage.size(); }

	void setImageSize(QSize size)                               { imageScale = size; cvImage2qtImage(); }

	double getImageScaleFactor()                                { return scaleFactor; }

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
	
public slots:
	virtual void saveImage();
};

#endif // CVIMAGEWIDGET_H
