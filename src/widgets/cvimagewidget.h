#ifndef CVIMAGEWIDGET_H
#define CVIMAGEWIDGET_H


#include <QWidget>
#include <QImage>

#include <opencv2/opencv.hpp>


// http://develnoter.blogspot.de/2012/05/integrating-opencv-in-qt-gui.html

class QMenu;
class QContextMenuEvent;

class FilterImage;

class CVImageWidget : public QWidget
{
	Q_OBJECT

	enum class ScaleMethod { Factor, Size };
	
	double scaleFactor = 1.;
	bool grayCvImage;
	ScaleMethod scaleMethod = ScaleMethod::Factor;

	const FilterImage* imageFilter = nullptr;
	
	void addZoomAction(int zoom);

public:
	enum class FloatGrayTransform { Auto, Fix, ZeroToOne };

	explicit CVImageWidget(QWidget *parent = 0);
	virtual ~CVImageWidget();

	QSize sizeHint()        const                      override { return qtImage.size(); }
	QSize minimumSizeHint() const                      override { return qtImage.size()/100; }

	virtual void setImageSize(QSize size)                       { imageScale  = size  ; scaleMethod = ScaleMethod::Size  ; cvImage2qtImage(); }
	virtual void setScaleFactor(double factor)                  { scaleFactor = factor; scaleMethod = ScaleMethod::Factor; cvImage2qtImage(); }

	int  imageHight() const;
	int  imageWidth() const;

	int  scaledImageHeight() const                              { return qtImage.height(); }
	int  scaledImageWidth() const                               { return qtImage.width() ; }

	double getImageScaleFactor()                          const { return scaleFactor; }

	void addZoomItems();

	void setFloatGrayTransform(FloatGrayTransform transform)    { floatGrayTransform = transform; }
	void setGrayTransformValueA(double val)                     { grayTransformA = val; }
	void setGrayTransformValueB(double val)                     { grayTransformB = val; }

	void setImageFilter(const FilterImage* imageFilter);

protected:
	virtual void paintEvent(QPaintEvent* event) override;
	virtual void wheelEvent(QWheelEvent* event) override;
	virtual void contextMenuEvent(QContextMenuEvent* event) override;

	QImage   qtImage;
	cv::Mat  cvImage;
	cv::Mat  outputImage;

	QMenu*   contextMenu;
	QAction* saveAction;
	QSize    imageScale;
	
	
	QString translateFileFormat(const QString& format) const;
	int fileDialog(QString& filename);

	void cvImage2qtImage();
	void updateScaleFactor();
	
public slots:
	virtual void saveImage();
	virtual void showImage(const cv::Mat& image);

	void setZoom(double factor)                                  { if(scaleFactor != factor && factor <= 8 && factor > 0) { scaleFactor = factor; cvImage2qtImage(); zoomChanged(factor); } }

	void fitImage2Width (int width );
	void fitImage2Height(int heigth);
	void fitImage(int width, int height);

	void zoom_in()                                               { setZoom(scaleFactor+0.5); }
	void zoom_out()                                              { setZoom(scaleFactor-0.5); }

private slots:
	void imageParameterChanged();

signals:
	void zoomChanged(double);
	void needScrollTo(int x, int y);
	void sizeChanged();

private:
	FloatGrayTransform floatGrayTransform = FloatGrayTransform::ZeroToOne;
	double grayTransformA = 255;
	double grayTransformB = 0;
};

#endif // CVIMAGEWIDGET_H
