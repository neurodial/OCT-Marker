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
	
	
public:
	explicit CVImageWidget(QWidget *parent = 0);
	virtual ~CVImageWidget();

	QSize sizeHint() const { return _qimage.size(); }
	QSize minimumSizeHint() const { return _qimage.size(); }

public slots:
	void showImage(const cv::Mat& image);

protected:
	void paintEvent(QPaintEvent* /*event*/)
	{
		// Display the image
		QPainter painter(this);
		painter.drawImage(QPoint(0,0), _qimage);
		painter.end();
	}

	QImage _qimage;
	cv::Mat _tmp;
	QMenu* contextMenu;
	
	
	QString translateFileFormat(const QString& format) const;
	int fileDialog(QString& filename);
	
public slots:
	virtual void saveImage();
};

#endif // CVIMAGEWIDGET_H
