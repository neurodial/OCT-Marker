/*
 * Copyright (c) 2018 Kay Gawlik <kaydev@amarunet.de> <kay.gawlik@beuth-hochschule.de> <kay.gawlik@charite.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

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
	addZoomAction(4);
	addZoomAction(6);
	addZoomAction(9);
	addZoomAction(12);
	addZoomAction(25);
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

	if(aspectRatio < 1 && aspectRatio > 1e-4)
		scaleFactorX = 1/aspectRatio;
	else if(aspectRatio > 1)
		scaleFactorY = aspectRatio;

	scaleFactor.setFactorX(scaleFactorX);
	scaleFactor.setFactorY(scaleFactorY);
	scaleFactor.setFactor(scaleFactorConfig);

	scaledSize = QSize(static_cast<int>(qtImage.width ()*scaleFactor.getFactorX())
	                 , static_cast<int>(qtImage.height()*scaleFactor.getFactorY()));
}

double CVImageWidget::getFactorFitImage2Parent()
{
	QWidget* parent = parentWidget();
	if(!parent)
		return -1;

	if(cvImage.cols > 0 && cvImage.rows > 0)
	{
		QSize size = parent->size();
		int width  = size.width();
		int height = size.height();

		double scale1 = static_cast<double>(width )/cvImage.cols/scaleFactor.getPureFactorX();
		double scale2 = static_cast<double>(height)/cvImage.rows/scaleFactor.getPureFactorY();

		return std::min(scale1, scale2);
	}
	return -1;
}

void CVImageWidget::fitImage2ParentHeight()
{
	QWidget* parent = parentWidget();
	if(!parent)
		return;

	if(cvImage.rows > 0)
	{
		QSize size = parent->size();
		int height = size.height();

		setZoom(static_cast<double>(height)/cvImage.rows/scaleFactor.getPureFactorY());
	}
}

void CVImageWidget::fitImage2ParentWidth()
{
	QWidget* parent = parentWidget();
	if(!parent)
		return;

	if(cvImage.cols > 0)
	{
		QSize size = parent->size();
		int width  = size.width();

		setZoom(static_cast<double>(width)/cvImage.cols/scaleFactor.getPureFactorX());
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


	sizeChanged();
	update();
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


void CVImageWidget::drawScaled(const QImage& image, QPainter& painter, const QRect* /*rect*/, const ScaleFactor& factor)
{
// 	QRect sourceRect;
// 	if(!rect)
// 		sourceRect = image.rect();
// 	else
// 	{
// 		sourceRect = QRect(static_cast<int>((rect->x     ()  )/factor.getFactorX())
// 		                 , static_cast<int>((rect->y     ()  )/factor.getFactorY())
// 		                 , static_cast<int>((rect->width ()+0)/factor.getFactorX() + 0)
// 		                 , static_cast<int>((rect->height()+2)/factor.getFactorY() + 2));
// 	}
	QRect sourceRect = image.rect();

	QRect destRect(static_cast<int>(sourceRect.x     ()*factor.getFactorX())
	             , static_cast<int>(sourceRect.y     ()*factor.getFactorY())
	             , static_cast<int>(sourceRect.width ()*factor.getFactorX())
	             , static_cast<int>(sourceRect.height()*factor.getFactorY()));


	painter.drawImage(destRect, image, sourceRect);
}


void CVImageWidget::paintEvent(QPaintEvent* event)
{
	// Display the image
	QPainter painter(this);
	if(event)
		drawScaled(qtImage, painter, &event->rect(), scaleFactor);
	else
		drawScaled(qtImage, painter, nullptr, scaleFactor);

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


