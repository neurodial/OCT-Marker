#include "cvimagewidget.h"
#include <QContextMenuEvent>
#include <QMenu>
#include <QImageWriter>
#include <QFileDialog>
#include <QPainter>

#include <iostream>

#include <imagefilter/filterimage.h>
#include <helper/actionclasses.h>
#include <helper/actionclasses.h>

CVImageWidget::CVImageWidget(QWidget* parent): QWidget(parent), contextMenu(new QMenu)
{
	
	QAction* saveAction = new QAction(this);
	saveAction->setText(tr("Save Image"));
	saveAction->setIcon(QIcon(":/icons/disk.png"));
	contextMenu->addAction(saveAction);
	connect(saveAction, &QAction::triggered, this, &CVImageWidget::saveImage);

	QAction* saveBaseImageAction = new QAction(this);
	saveBaseImageAction->setText(tr("Save Base Image"));
	saveBaseImageAction->setIcon(QIcon(":/icons/disk.png"));
	contextMenu->addAction(saveBaseImageAction);
	connect(saveBaseImageAction, &QAction::triggered, this, &CVImageWidget::saveBaseImage);


	contextMenu->addSeparator();

}

CVImageWidget::~CVImageWidget()
{
	delete contextMenu;
}


void CVImageWidget::addZoomAction(int zoom)
{
	IntValueAction* actionZoom = new IntValueAction(zoom, this, false);
	actionZoom->setText(tr("Zoom %1").arg(zoom));
	actionZoom->setIcon(QIcon(":/icons/zoom.png"));
	contextMenu->addAction(actionZoom);
	connect(actionZoom, &IntValueAction::triggered, this, &CVImageWidget::setZoom);
}

void CVImageWidget::addZoomItems()
{
	addZoomAction(1);
	addZoomAction(2);
	addZoomAction(3);
	addZoomAction(4);
	addZoomAction(6);
	addZoomAction(8);
// 	addZoomAction(9);
// 	addZoomAction(12);
}


void CVImageWidget::contextMenuEvent(QContextMenuEvent* event)
{
// 	if(!event->isAccepted())
	{
		QWidget::contextMenuEvent(event);
		contextMenu->setVisible(true);
		contextMenu->exec(event->globalPos());
	}
}

void CVImageWidget::showImage(const cv::Mat& image)
{
	if(image.empty())
		cvImage = cv::Mat();
	else
	{
		// Convert the image to the RGB888 format
		switch(image.type())
		{
			case CV_8UC1:
				// cvtColor(image, cvImage, CV_GRAY2RGB);
				cvImage = image.clone();
				grayCvImage = true;
				break;
			case CV_8UC3:
				cvtColor(image, cvImage, CV_BGR2RGB);
				grayCvImage = false;
				break;
			case CV_32FC1:
			case CV_64FC1:
			{
				std::cout << "1 Channels: " << image.channels() << "\tgr: " << image.rows << " x " << image.cols << std::endl;

				switch(floatGrayTransform)
				{
					case FloatGrayTransform::Auto:
					{
						double min, max;
						cv::minMaxLoc(image, &min, &max);
						if(min == max)
							max = min+1;
						image.convertTo(cvImage, cv::DataType<uint8_t>::type, 255.0/(max-min), -255.0*min/(max-min));
						break;
					}
					case FloatGrayTransform::Fix:
						image.convertTo(cvImage, cv::DataType<uint8_t>::type, grayTransformA, grayTransformB);
						break;
					case FloatGrayTransform::ZeroToOne:
						image.convertTo(cvImage, cv::DataType<uint8_t>::type, 255.0, 0);
						break;
				}


				std::cout << "2 Channels: " << cvImage.channels() << "\tgr: " << cvImage.rows << " x " << cvImage.cols << std::endl;

				if(cvImage.channels() == 1)
					cv::cvtColor(cvImage, cvImage, CV_GRAY2BGR);


				std::cout << "3 Channels: " << cvImage.channels() << "\tgr: " << cvImage.rows << " x " << cvImage.cols << std::endl;

				break;
			}
			default:
				qDebug("unhandeld opencv image format %d", image.type());
				return;
		}

		// QImage needs the data to be stored continuously in memory
	}

	cvImage2qtImage();
}

void CVImageWidget::updateScaleFactor()
{
	if(imageScale.width() > 0 && imageScale.height() > 0)
	{
		scaleFactorConfig = std::min(static_cast<double>(height())/cvImage.rows, static_cast<double>(width())/cvImage.cols);
	}
	else
	{
		setFixedSize(cvImage.cols, cvImage.rows);
		scaleFactorConfig = 1.;
	}
	updateScaleFactorXY();
}

void CVImageWidget::updateScaleFactorXY()
{
	double scaleFactorX = 1;
	double scaleFactorY = 1;

	if(useAspectRatio && aspectRatio > 1e-4)
	{
		if(aspectRatio < 1)
			scaleFactorX = 1/aspectRatio;
		if(aspectRatio > 1)
			scaleFactorY = aspectRatio;
	}

	scaleFactor.setFactorX(scaleFactorX);
	scaleFactor.setFactorY(scaleFactorY);
	scaleFactor.setFactor(scaleFactorConfig);
}



void CVImageWidget::fitImage2Width(int width)
{
	if(cvImage.cols > 0)
		setZoom(static_cast<double>(width)/cvImage.cols/scaleFactor.getPureFactorX());
}

void CVImageWidget::fitImage2Height(int heigth)
{
	if(cvImage.rows > 0)
		setZoom(static_cast<double>(heigth)/cvImage.rows/scaleFactor.getPureFactorY());
}

void CVImageWidget::fitImage(int width, int heigth)
{
	if(cvImage.cols > 0 && cvImage.rows > 0)
	{
		double scale1 = static_cast<double>(width )/cvImage.cols/scaleFactor.getPureFactorX();
		double scale2 = static_cast<double>(heigth)/cvImage.rows/scaleFactor.getPureFactorY();

		setZoom(std::min(scale1, scale2));
	}
}


void CVImageWidget::cvImage2qtImage(const cv::Mat& cvImage, QImage& qimage)
{
	if(cvImage.empty())
	{
		qimage = QImage();
		return;
	}

	assert(cvImage.isContinuous());
	// Assign OpenCV's image buffer to the QImage. Note that the bytesPerLine parameter
	// (http://qt-project.org/doc/qt-4.8/qimage.html#QImage-6) is 3*width because each pixel
	// has three bytes.
	if(cvImage.channels() == 1)
		qimage = QImage(cvImage.data, cvImage.cols, cvImage.rows, cvImage.cols, QImage::Format_Grayscale8);
	else
		qimage = QImage(cvImage.data, cvImage.cols, cvImage.rows, cvImage.cols*3, QImage::Format_RGB888);

}


void CVImageWidget::cvImage2qtImage()
{
	if(cvImage.empty())
	{
		qtImage = QImage();
		return;
	}

	if(imageFilter)
		imageFilter->applyFilter(cvImage, outputImage);
	else
		outputImage = cvImage;

	cvImage2qtImage(outputImage, qtImage);

	switch(scaleMethod)
	{
		case ScaleMethod::Factor:
			imageScale.setWidth (static_cast<int>(outputImage.cols * scaleFactor.getFactorX() + 0.5));
			imageScale.setHeight(static_cast<int>(outputImage.rows * scaleFactor.getFactorY() + 0.5));
			setFixedSize(imageScale);
			break;
		case ScaleMethod::Size:
			updateScaleFactor();
			break;
	}

	const Qt::AspectRatioMode aspectRadioMode = (aspectRatio == 1.0) ? Qt::KeepAspectRatio : Qt::IgnoreAspectRatio;

	if(!scaleFactor.isIdentical())
		qtImage = qtImage.scaled(static_cast<int>(outputImage.cols*scaleFactor.getFactorX() + 0.5)
		                       , static_cast<int>(outputImage.rows*scaleFactor.getFactorY() + 0.5)
		                       , aspectRadioMode
		                       , Qt::FastTransformation);

	update();
	sizeChanged();
}

int CVImageWidget::imageHight() const
{
	return cvImage.rows;
}

int CVImageWidget::imageWidth() const
{
	return cvImage.cols;
}




void CVImageWidget::saveImage()
{
	QString filename;
	if(fileDialog(filename))
	{
		QPixmap pixmap(size());
		render(&pixmap);
		pixmap.save(filename);
	}
}

void CVImageWidget::saveBaseImage()
{
	QString filename;
	if(fileDialog(filename))
	{
		QImage imageTmp;
		cvImage2qtImage(outputImage, imageTmp);
		imageTmp.save(filename);
	}
}


void CVImageWidget::paintEvent(QPaintEvent* event)
{
	// Display the image
	QPainter painter(this);
	if(event)
		painter.drawImage(event->rect(), qtImage, event->rect());
	else
		painter.drawImage(QPoint(0,0), qtImage);
	painter.end();
}

void CVImageWidget::wheelEvent(QWheelEvent* wheelE)
{
	int deltaWheel = wheelE->delta();
	if(wheelE->modifiers() == Qt::ControlModifier)
	{
		QPoint pos = mapToParent(wheelE->pos());

		int x = wheelE->x();
		int y = wheelE->y();

		int px = pos.x();
		int py = pos.y();

		double oldScaleFactor = getScaleFactor();
		double changeScaleFactor;

		if(deltaWheel < 0)
			changeScaleFactor = 1/1.25;
		else
			changeScaleFactor =   1.25;

		double newScaleFactor = oldScaleFactor*changeScaleFactor;
		if(newScaleFactor > 0.5 || changeScaleFactor > 1) // avoid zoom out lower then factor 0.5
			setZoom(newScaleFactor);

		newScaleFactor = getScaleFactor();

		if(oldScaleFactor > 0)
		{
			double percentChanged = newScaleFactor/oldScaleFactor;
			int dx = static_cast<int>(x*percentChanged) - (px);
			int dy = static_cast<int>(y*percentChanged) - (py);
			needScrollTo(dx, dy);
		}

		wheelE->accept();
	}
}



int CVImageWidget::fileDialog(QString& filename)
{
	QFileDialog fd(this);
	// fd.selectFile(lineEditFile->text());
	fd.setWindowTitle(tr("Choose a filename to save under"));
	fd.setAcceptMode(QFileDialog::AcceptSave);
	
	QList<QByteArray> list = QImageWriter::supportedImageFormats();
	
	QStringList filters;
	const int listcount = list.count();
	for(int i=0; i<listcount; ++i)
	{
		filters << translateFileFormat(list[i]) + " (*."+list[i].toLower()+")";
	}
	
	filters.sort();
	fd.setNameFilters(filters);
	fd.setFileMode(QFileDialog::AnyFile);
	
	int result = fd.exec();
	
	if(result)
	{
		filename = fd.selectedFiles().first();
	}
	return result;
}


QString CVImageWidget::translateFileFormat(const QString& format) const
{
	QString formatUp = format.toUpper();
	if(formatUp == "BMP")
		return tr("BMP - Windows Bitmap");
	if(formatUp == "JPG" || formatUp == "JPEG")
		return tr("JPG - Joint Photographic Experts Group");
	if(formatUp == "PNG")
		return tr("PNG - Portable Network Graphics");
	if(formatUp == "PPM")
		return tr("PPM - Portable Pixmap");
	if(formatUp == "TIFF")
		return tr("TIFF - Tagged Image File Format");
	if(formatUp == "XBM")
		return tr("XBM - X11 Bitmap");
	if(formatUp == "XPM")
		return tr("XPM - X11 Pixmap");
	
	return tr("%1 File").arg(format);
}

void CVImageWidget::setImageFilter(const FilterImage* imageFilter)
{
	if(this->imageFilter)
		disconnect(this->imageFilter, &FilterImage::parameterChanged, this, &CVImageWidget::imageParameterChanged);

	this->imageFilter = imageFilter;
	if(imageFilter)
		connect(imageFilter, &FilterImage::parameterChanged, this, &CVImageWidget::imageParameterChanged);

	cvImage2qtImage();
}

void CVImageWidget::imageParameterChanged()
{
	cvImage2qtImage();
}


