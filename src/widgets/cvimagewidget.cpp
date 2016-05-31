#include "cvimagewidget.h"
#include <QContextMenuEvent>
#include <QMenu>
#include <QImageWriter>
#include <QFileDialog>

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
	// Convert the image to the RGB888 format
	switch(image.type())
	{
		case CV_8UC1:
			cvtColor(image, _tmp, CV_GRAY2RGB);
			break;
		case CV_8UC3:
			cvtColor(image, _tmp, CV_BGR2RGB);
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
			image.convertTo(_tmp, CV_8UC3, 255.0/(max-min), -255.0*min/(max-min));
		//	image.convertTo(_tmp, CV_8U, 255.0/(max-min), -255.0*min/(max-min));
			
	
			if(_tmp.channels() == 1)
			{
				cv::cvtColor(_tmp, _tmp, CV_GRAY2BGR);
			//	cv::cvtColor(result, result, CV_8UC3, 3);
			}
			
			break;
		}
	}

	// QImage needs the data to be stored continuously in memory
	assert(_tmp.isContinuous());
	// Assign OpenCV's image buffer to the QImage. Note that the bytesPerLine parameter
	// (http://qt-project.org/doc/qt-4.8/qimage.html#QImage-6) is 3*width because each pixel
	// has three bytes.
	_qimage = QImage(_tmp.data, _tmp.cols, _tmp.rows, _tmp.cols*3, QImage::Format_RGB888);

	this->setFixedSize(image.cols, image.rows);

	repaint();
}


void CVImageWidget::saveImage()
{
	QString filename;
	if(fileDialog(filename))
	{
		_qimage.save(filename);
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
