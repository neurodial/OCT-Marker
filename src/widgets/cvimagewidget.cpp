#include "cvimagewidget.h"
#include <QContextMenuEvent>
#include <QMenu>
#include <QImageWriter>
#include <QFileDialog>
#include <QPainter>

#include <iostream>

CVImageWidget::CVImageWidget(QWidget* parent): QWidget(parent), contextMenu(new QMenu)
{
	
	QAction* saveAction = new QAction(this);
	saveAction->setText(tr("Save Image"));
	saveAction->setIcon(QIcon(":/icons/disk.png"));
	contextMenu->addAction(saveAction);

	connect(saveAction, SIGNAL(triggered(bool)), this, SLOT(saveImage()));
}

CVImageWidget::~CVImageWidget()
{
	delete contextMenu;
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
				double min, max;
				cv::minMaxLoc(image, &min, &max);
			//	qDebug("min: %d\tmax: %d", min, max);
			//	std::cout << "min: " << min << "\tmax: " << max << std::endl;

				if(min == max)
					max = min+1;
		//		image.convertTo(cvImage, CV_8UC3, 255.0/(max-min), -255.0*min/(max-min));
				image.convertTo(cvImage, CV_8UC3, 255.0, 0);

				if(cvImage.channels() == 1)
					cv::cvtColor(cvImage, cvImage, CV_GRAY2BGR);

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

void CVImageWidget::cvImage2qtImage()
{
	if(cvImage.empty())
	{
		qtImage = QImage();
		return;
	}

		assert(cvImage.isContinuous());
	// Assign OpenCV's image buffer to the QImage. Note that the bytesPerLine parameter
	// (http://qt-project.org/doc/qt-4.8/qimage.html#QImage-6) is 3*width because each pixel
	// has three bytes.
	if(grayCvImage)
		qtImage = QImage(cvImage.data, cvImage.cols, cvImage.rows, cvImage.cols, QImage::Format_Grayscale8);
	else
		qtImage = QImage(cvImage.data, cvImage.cols, cvImage.rows, cvImage.cols*3, QImage::Format_RGB888);

	if(imageScale.width() > 0 && imageScale.height() > 0)
	{
		scaleFactor = std::min(static_cast<double>(height())/cvImage.rows, static_cast<double>(width())/cvImage.cols);
		// qtImage = qtImage.scaled(imageScale,  Qt::KeepAspectRatio);
		qtImage = qtImage.scaled(cvImage.cols*scaleFactor, cvImage.rows*scaleFactor,  Qt::KeepAspectRatio);
	}
	else
	{
		setFixedSize(cvImage.cols, cvImage.rows);
		scaleFactor = 1.;
	}

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

void CVImageWidget::paintEvent(QPaintEvent* /*event*/)
{
	// Display the image
	QPainter painter(this);
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
