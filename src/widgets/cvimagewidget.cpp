#include "cvimagewidget.h"
#include <QContextMenuEvent>
#include <QMenu>
#include <QImageWriter>
#include <QFileDialog>
#include <QPainter>

#include <iostream>

#include <imagefilter/filterimage.h>

CVImageWidget::CVImageWidget(QWidget* parent): QWidget(parent), contextMenu(new QMenu)
{
	
	QAction* saveAction = new QAction(this);
	saveAction->setText(tr("Save Image"));
	saveAction->setIcon(QIcon(":/icons/disk.png"));
	contextMenu->addAction(saveAction);
	connect(saveAction, SIGNAL(triggered(bool)), this, SLOT(saveImage()));

	contextMenu->addSeparator();

}

CVImageWidget::~CVImageWidget()
{
	delete contextMenu;
}

void CVImageWidget::addZoomItems()
{
	QAction* actionZoom1 = new QAction(this);
	actionZoom1->setText(tr("Zoom %1").arg(1));
	actionZoom1->setIcon(QIcon(":/icons/zoom.png"));
	contextMenu->addAction(actionZoom1);
	connect(actionZoom1, &QAction::triggered, this, &CVImageWidget::setZoom1);

	QAction* actionZoom2 = new QAction(this);
	actionZoom2->setText(tr("Zoom %1").arg(2));
	actionZoom2->setIcon(QIcon(":/icons/zoom.png"));
	contextMenu->addAction(actionZoom2);
	connect(actionZoom2, &QAction::triggered, this, &CVImageWidget::setZoom2);

	QAction* actionZoom3 = new QAction(this);
	actionZoom3->setText(tr("Zoom %1").arg(3));
	actionZoom3->setIcon(QIcon(":/icons/zoom.png"));
	contextMenu->addAction(actionZoom3);
	connect(actionZoom3, &QAction::triggered, this, &CVImageWidget::setZoom3);

	QAction* actionZoom4 = new QAction(this);
	actionZoom4->setText(tr("Zoom %1").arg(4));
	actionZoom4->setIcon(QIcon(":/icons/zoom.png"));
	contextMenu->addAction(actionZoom4);
	connect(actionZoom4, &QAction::triggered, this, &CVImageWidget::setZoom4);

	QAction* actionZoom5 = new QAction(this);
	actionZoom5->setText(tr("Zoom %1").arg(5));
	actionZoom5->setIcon(QIcon(":/icons/zoom.png"));
	contextMenu->addAction(actionZoom5);
	connect(actionZoom5, &QAction::triggered, this, &CVImageWidget::setZoom5);
}


void CVImageWidget::contextMenuEvent(QContextMenuEvent* event)
{
	QWidget::contextMenuEvent(event);
	contextMenu->setVisible(true);
	contextMenu->exec(event->globalPos());
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
		scaleFactor = std::min(static_cast<double>(height())/cvImage.rows, static_cast<double>(width())/cvImage.cols);
	}
	else
	{
		setFixedSize(cvImage.cols, cvImage.rows);
		scaleFactor = 1.;
	}
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

	assert(outputImage.isContinuous());
	// Assign OpenCV's image buffer to the QImage. Note that the bytesPerLine parameter
	// (http://qt-project.org/doc/qt-4.8/qimage.html#QImage-6) is 3*width because each pixel
	// has three bytes.
	if(grayCvImage)
		qtImage = QImage(outputImage.data, outputImage.cols, outputImage.rows, outputImage.cols, QImage::Format_Grayscale8);
	else
		qtImage = QImage(outputImage.data, outputImage.cols, outputImage.rows, outputImage.cols*3, QImage::Format_RGB888);


	switch(scaleMethod)
	{
		case ScaleMethod::Factor:
			imageScale.setHeight(static_cast<int>(outputImage.rows * scaleFactor + 0.5));
			imageScale.setWidth (static_cast<int>(outputImage.cols * scaleFactor + 0.5));
			setFixedSize(imageScale);
			break;
		case ScaleMethod::Size:
			updateScaleFactor();
			break;
	}

	if(scaleFactor != 1)
		qtImage = qtImage.scaled(static_cast<int>(outputImage.cols*scaleFactor + 0.5)
		                       , static_cast<int>(outputImage.rows*scaleFactor + 0.5)
		                       , Qt::KeepAspectRatio
		                       , Qt::FastTransformation);

	update();
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
		qtImage.save(filename);
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


