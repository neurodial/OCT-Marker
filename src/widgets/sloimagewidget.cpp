#include "sloimagewidget.h"

#include<fstream>

#include <QPainter>
#include <QResizeEvent>
#include<QMenu>
#include<QIcon>

#include <octdata/datastruct/series.h>
#include <octdata/datastruct/sloimage.h>
#include <octdata/datastruct/coordslo.h>
#include <octdata/datastruct/bscan.h>

#include <manager/octmarkermanager.h>
#include <manager/octdatamanager.h>

#include <markermodules/bscanmarkerbase.h>
#include <markermodules/slomarkerbase.h>

#include <data_structure/intervalmarker.h>
#include <data_structure/programoptions.h>
#include<data_structure/rect2d.h>

#include<helper/slocoordtranslator.h>

#include <QGraphicsView>
#include <QGraphicsTextItem>

#include <cmath>
#include <limits>
#include <QFileDialog>
#include <QFileInfo>

#include <QPdfWriter>
#include <markermodules/widgetoverlaylegend.h>


namespace
{

	class GraphicsView : public QGraphicsView
	{
		double scaleFactor = 100;
	public:
		explicit GraphicsView(QWidget* parent) : QGraphicsView(parent) {}

		void setScaleFactor(double factor) { scaleFactor = factor; }
	protected:
		virtual void resizeEvent(QResizeEvent *event) override
		{
			QGraphicsView::resizeEvent(event);

			setSceneRect    (0, 0, scaleFactor, scaleFactor);
			fitInView(QRectF(0, 0, scaleFactor, scaleFactor));
		}
	};
}


SLOImageWidget::SLOImageWidget(QWidget* parent)
: CVImageWidget(parent)
, markerManger(OctMarkerManager::getInstance())
{
	OctDataManager& octDataManager = OctDataManager::getInstance();
	connect(&octDataManager, &OctDataManager  ::seriesChanged     , this, &SLOImageWidget::reladSLOImage           );
	connect(&markerManger  , &OctMarkerManager::bscanChanged      , this, &SLOImageWidget::bscanChanged            );
	connect(&markerManger  , &OctMarkerManager::sloViewChanged    , this, &SLOImageWidget::sloViewChanged          );
	connect(&markerManger  , &OctMarkerManager::sloMarkerChanged  , this, &SLOImageWidget::sloMarkerChanged        );
	connect(&markerManger  , &OctMarkerManager::sloOverlayChanged , this, &SLOImageWidget::updateMarkerOverlayImage);
	connect(&markerManger  , &OctMarkerManager::bscanMarkerChanged, this, &SLOImageWidget::updateMarkerOverlayImage);

	connect(&ProgramOptions::sloShowsBScansPos, &OptionInt   ::valueChanged, this, &SLOImageWidget::setBScanVisibility);
	connect(&ProgramOptions::sloShowGrid      , &OptionBool  ::valueChanged, this, static_cast<void (SLOImageWidget::*)(void)>(&SLOImageWidget::update));
	connect(&ProgramOptions::sloShowOverlay   , &OptionBool  ::valueChanged, this, &SLOImageWidget::updateMarkerOverlayImage);
	connect(&ProgramOptions::sloOverlayAlpha  , &OptionDouble::valueChanged, this, &SLOImageWidget::updateMarkerOverlayImage);
	connect(&ProgramOptions::sloClipScanArea  , &OptionBool  ::valueChanged, this, &SLOImageWidget::reladSLOImage           );

	setBScanVisibility(ProgramOptions::sloShowsBScansPos());

	setMinimumSize(150,150);
	setFocusPolicy(Qt::StrongFocus);

	gv = new GraphicsView(this);
	gv->setStyleSheet("QGraphicsView { border-style: none; background: transparent;}" );
	gv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	gv->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	gv->setWindowFlags (Qt::FramelessWindowHint);
	gv->setCacheMode(QGraphicsView::CacheBackground);
	gv->setFocusPolicy(Qt::NoFocus);
	gv->setDragMode(QGraphicsView::NoDrag);

	// gv->show();
	// gv->setGeometry(rect());
	// gv->setVisible(ProgramOptions::sloShowLabels());
	sloMarkerChanged(markerManger.getActSloMarker());
	updateGraphicsViewSize();

	QAction* saveTexAction = new QAction(this);
	saveTexAction->setText(tr("save latex"));
	saveTexAction->setIcon(QIcon(":/icons/disk.png"));
	connect(saveTexAction, &QAction::triggered, this, &SLOImageWidget::saveLatexImageSlot);
	contextMenu->addAction(saveTexAction);
}

SLOImageWidget::~SLOImageWidget()
{
	// deleteIntervallColors();
}

void SLOImageWidget::paintEvent(QPaintEvent* event)
{
	CVImageWidget::paintEvent(event);

	const OctData::Series* series = OctDataManager::getInstance().getSeries();
	if(!series)
		return;

	QPainter painter(this);
	if(drawConvexHull)
		paintConvexHull(painter, series);

	if(drawBScans)
		paintBScans(painter, series);

	if(ProgramOptions::sloShowGrid())
		paintAnalyseGrid(painter, series);

	if(markPos.show)
	{
		QPen pen(QColor(128, 0, 0));
		pen.setWidth(3);
		painter.setPen(pen);
		painter.drawLine(markPos.p.getX()-5, markPos.p.getY()-5, markPos.p.getX()+5, markPos.p.getY()+5);
		painter.drawLine(markPos.p.getX()-5, markPos.p.getY()+5, markPos.p.getX()+5, markPos.p.getY()-5);
	}

	painter.end();

}


void SLOImageWidget::paintConvexHull(QPainter& painter, const OctData::Series* series)
{
	if(!series)
		return;
	const OctData::Series::BScanSLOCoordList& hull = series->getConvexHull();


	if(hull.size() < 3)
		return;

	QPen pen(QColor(128, 255, 128));
	pen.setWidth(2);
	painter.setPen(pen);

	SloCoordTranslator sct(*series, getImageScaleFactor());
	sct.setClipShift(OctData::CoordSLOpx(clipX1, clipY1));

	for(std::size_t i = 1; i < hull.size(); ++i)
	{
		const OctData::CoordSLOpx start_px = sct(hull[i-1]);
		const OctData::CoordSLOpx   end_px = sct(hull[i  ]);

		painter.drawLine(start_px.getX(), start_px.getY(), end_px.getX(), end_px.getY());
	}
}


void SLOImageWidget::paintBScans(QPainter& painter, const OctData::Series* series)
{
	if(!series)
		return;

	QPen normalBscanPen;
	QPen activBscanPen;

	normalBscanPen.setWidth(2);
	normalBscanPen.setColor(QColor(0,0,0));


	activBscanPen.setWidth(2);
	activBscanPen.setColor(QColor(255,0,0));


	
	const OctData::Series::BScanList bscans = series->getBScans();
	std::size_t activBScan                  = static_cast<std::size_t>(markerManger.getActBScanNum());

	SloCoordTranslator coordTranslator(*series, getImageScaleFactor());
	coordTranslator.setClipShift(OctData::CoordSLOpx(clipX1, clipY1));

	// std::cout << cscan.getSloImage()->getShift() << " * " << (getImageScaleFactor()) << " = " << shift << std::endl;

	bool paintMarker = false;
	BscanMarkerBase* actMarker = markerManger.getActBscanMarker();
	if(actMarker)
		paintMarker = actMarker->drawingBScanOnSLO();

	if(singelBScanScan || drawOnylActBScan)
	{
		if(bscans.size() > activBScan)
		{
			const OctData::BScan* actBScan = bscans.at(static_cast<std::size_t>(activBScan));
			if(actBScan)
			{
				if(paintMarker)
					painter.setPen(normalBscanPen);
				else
					painter.setPen(activBscanPen);

				paintBScan(painter, *actBScan, coordTranslator, activBScan, paintMarker);
			}
		}
	}
	else
	{
		std::size_t bscanCounter = 0;
		const OctData::BScan* actBScan = nullptr;
		for(const OctData::BScan* bscan : bscans)
		{
			if(bscan)
			{
				if(bscanCounter == activBScan)
				{
					actBScan = bscan;
				}
				else
				{
					painter.setPen(normalBscanPen);
					paintBScan(painter, *bscan, coordTranslator, bscanCounter, paintMarker);
				}
			}
			++bscanCounter;
		}

		if(actBScan)
		{
			painter.setPen(activBscanPen);
			paintBScan(painter, *actBScan, coordTranslator, activBScan, paintMarker);
		}
	}
}

void SLOImageWidget::paintBScan(QPainter& painter, const OctData::BScan& bscan, const SloCoordTranslator& coordTranslator, std::size_t bscanNr, bool paintMarker)
{
	switch(bscan.getBScanType())
	{
		case OctData::BScan::BScanType::Line:
			paintBScanLine(painter, bscan, coordTranslator, bscanNr, paintMarker);
			break;
		case OctData::BScan::BScanType::Circle:
			paintBScanCircle(painter, bscan, coordTranslator, bscanNr, paintMarker);
			break;
		case OctData::BScan::BScanType::Unknown:
			break;
	}
}


void SLOImageWidget::paintBScanLine(QPainter& painter, const OctData::BScan& bscan, const SloCoordTranslator& coordTranslator, std::size_t bscanNr, bool paintMarker)
{
	const OctData::CoordSLOpx start_px = coordTranslator(bscan.getStart());
	const OctData::CoordSLOpx   end_px = coordTranslator(bscan.getEnd()  );

	painter.drawLine(start_px.getX(), start_px.getY(), end_px.getX(), end_px.getY());

	if(paintMarker)
	{
		BscanMarkerBase* actMarker = markerManger.getActBscanMarker();
		if(actMarker)
			actMarker->drawBScanSLOLine(painter, bscanNr, start_px, end_px, this);
	}
}

void SLOImageWidget::paintBScanCircle(QPainter& painter, const OctData::BScan& bscan, const SloCoordTranslator& coordTranslator, std::size_t bscanNr, bool paintMarker)
{
	const OctData::CoordSLOpx  start_px = coordTranslator(bscan.getStart() );
	const OctData::CoordSLOpx center_px = coordTranslator(bscan.getCenter());

	double radius = center_px.abs(start_px);

	painter.drawEllipse(QPointF(center_px.getXf(), center_px.getYf()), radius, radius);

	if(paintMarker)
	{
		BscanMarkerBase* actMarker = markerManger.getActBscanMarker();
		if(actMarker)
			actMarker->drawBScanSLOCircle(painter, bscanNr, start_px, center_px, bscan.getClockwiseRot(), this);
	}
}

void SLOImageWidget::paintAnalyseGrid(QPainter& painter, const OctData::Series* series)
{
	if(!series)
		return;

	QPen pen;
	pen.setWidth(2);
	pen.setColor(QColor(125, 125, 255));

	painter.setPen(pen);

	const OctData::AnalyseGrid& grid = series->getAnalyseGrid();

	const std::vector<double>& diameters = grid.getDiametersMM();
	if(diameters.size() == 0)
		return;

	SloCoordTranslator sct(*series, getImageScaleFactor());
	sct.setClipShift(OctData::CoordSLOpx(clipX1, clipY1));

	const OctData::CoordSLOpx centerPx = sct(grid.getCenter());

	for(double d : diameters)
	{
		double r = d/2.;
		OctData::CoordSLOpx radius = sct.transformWithoutShift(OctData::CoordSLOmm(r, r));
		painter.drawEllipse(QPointF(centerPx.getXf(), centerPx.getYf()), radius.getXf(), radius.getYf());
	}
}

void SLOImageWidget::showPosOnBScan(double t)
{
	if(!ProgramOptions::sloShowBScanMousePos())
		return;

	Rect2DInt rect(markPos.p);
	const OctData::BScan* actBScan = markerManger.getActBScan();
	if(actBScan)
	{
		if(t<0 || t>1 || !actBScan)
		{
			markPos.show = false;
			rect.addBroder(10);
			update(rect.toQRect());
			return;
		}
	}

	const OctData::Series* series            = OctDataManager::getInstance().getSeries();
	if(series)
	{
		SloCoordTranslator sct(*series, getImageScaleFactor());
		sct.setClipShift(OctData::CoordSLOpx(clipX1, clipY1));
		OctData::CoordSLOmm point = actBScan->getStart()*(1-t) + actBScan->getEnd()*(t); // TODO falsche Richtung?
		const OctData::CoordSLOpx markPx = sct(point);

		markPos.show = true;
		markPos.p = Point2DInt(markPx.getX(), markPx.getY());
		rect += markPos.p;
	}
	rect.addBroder(10);
	update(rect.toQRect());
}


void SLOImageWidget::clipAndShowImage(const cv::Mat& img)
{
	if(ProgramOptions::sloClipScanArea())
	{
		const OctData::Series* series = OctDataManager::getInstance().getSeries();
		if(series)
		{
			OctData::CoordSLOmm p1 = series->getLeftUpperCoord();
			OctData::CoordSLOmm p2 = series->getRightLowerCoord();
			SloCoordTranslator sct(*series, ScaleFactor());

			OctData::CoordSLOpx p1px = sct(p1);
			OctData::CoordSLOpx p2px = sct(p2);

			clipX1 = p1px.getX();
			clipY1 = p1px.getY();
			int clipX2 = p2px.getX();
			int clipY2 = p2px.getY();

			if(clipX1 < 0) clipX1 = 0;
			if(clipY1 < 0) clipY1 = 0;
			if(clipX2 >= img.cols) clipX2 = img.cols - 1;
			if(clipY2 >= img.rows) clipY2 = img.rows - 1;

			cv::Rect roi;
			roi.x = clipX1;
			roi.y = clipY1;
			roi.width  = clipX2 - clipX1;
			roi.height = clipY2 - clipY1;

			if(roi.width > 10 && roi.height > 10)
			{
				showImage(img(roi));
				return;
			}
		}
	}


	clipX1 = 0;
	clipY1 = 0;

	showImage(img);
}



void SLOImageWidget::updateMarkerOverlayImage()
{
	const OctData::Series* series = OctDataManager::getInstance().getSeries();
	if(!series)
		return;

	const OctData::SloImage& sloImage = series->getSloImage();
	const cv::Mat& sloPixture = sloImage.getImage();
	bool showPureSloImage = true;

	if(ProgramOptions::sloShowOverlay())
	{
		BscanMarkerBase* actMarker = markerManger.getActBscanMarker();
		if(actMarker)
		{
			cv::Mat outImage;
			bool overlayCreated = actMarker->drawSLOOverlayImage(sloPixture, outImage, ProgramOptions::sloOverlayAlpha());
			if(overlayCreated && !outImage.empty())
			{
				showPureSloImage = false;
				clipAndShowImage(outImage);
			}
		}
	}

	if(showPureSloImage)
		clipAndShowImage(sloPixture);

	singelBScanScan = (series->bscanCount() == 1);
}


void SLOImageWidget::reladSLOImage()
{
	updateMarkerOverlayImage();


	updateGraphicsViewSize();
// 	gv->setSceneRect(0, 0, imageWidth(), imageHight());
}

void SLOImageWidget::bscanChanged(int /*bscan*/)
{
	update();
}

void SLOImageWidget::showBScans(bool show)
{
	drawBScans = show;
	update();
}

void SLOImageWidget::showOnylActBScan(bool show)
{
	drawOnylActBScan = show;
	update();
}


void SLOImageWidget::setImageSize(QSize size)
{
	CVImageWidget::setImageSize(size);
	updateGraphicsViewSize();
}

void SLOImageWidget::updateGraphicsViewSize()
{
	gv->setGeometry(0, 0, scaledImageWidth(), scaledImageHeight());
}

void SLOImageWidget::resizeEvent(QResizeEvent* event)
{
	setImageSize(event->size());
}


void SLOImageWidget::sloMarkerChanged(SloMarkerBase* marker)
{
	if(marker)
	{
		scene = marker->getGraphicsScene();
		GraphicsView* gvConvert = dynamic_cast<GraphicsView*>(gv);
		if(gvConvert)
			gvConvert->setScaleFactor(marker->getScaleFactor());
	}
	else
	{
		scene = nullptr;
	}
	gv->setScene(scene);
	gv->setVisible(scene != nullptr);
	updateGraphicsViewSize();
}

void SLOImageWidget::sloViewChanged()
{
	if(singelBScanScan || drawOnylActBScan)
		update();
}


namespace
{
	double calcAbsBScanLine(const OctData::BScan& bscan, const SloCoordTranslator& transform, const OctData::CoordSLOpx& clickPos)
	{
		const OctData::CoordSLOpx start_px = transform(bscan.getStart());
		const OctData::CoordSLOpx   end_px = transform(bscan.getEnd()  );


		const OctData::CoordSLOpx geradenvektor = end_px  -start_px;
		const OctData::CoordSLOpx kraftvektor   = clickPos-start_px;
		double alpha = (kraftvektor*geradenvektor) / geradenvektor.normquadrat();
		if(alpha>0 && alpha<1)
		{
			return (geradenvektor*alpha).abs(kraftvektor);
		}
		return std::numeric_limits<double>::infinity();
	}

	double calcAbsBScanCircle(const OctData::BScan& bscan, const SloCoordTranslator& transform, const OctData::CoordSLOpx& clickPos)
	{
		const OctData::CoordSLOpx  start_px = transform(bscan.getStart() );
		const OctData::CoordSLOpx center_px = transform(bscan.getCenter());

		double radiusCircle = center_px.abs(start_px);
		double distCenter   = center_px.abs(clickPos);

		return std::abs(distCenter - radiusCircle);
	}


	double calcAbsBScan(const OctData::BScan& bscan, const SloCoordTranslator& transform, const OctData::CoordSLOpx& clickPos)
	{
		if(bscan.getCenter())
			return calcAbsBScanCircle(bscan, transform, clickPos);
		else
			return calcAbsBScanLine(bscan, transform, clickPos);
	}
}



int SLOImageWidget::getBScanNearPos(int x, int y, double tol)
{
	const OctData::Series* series            = OctDataManager::getInstance().getSeries();
	if(!series)
		return -1;
	const OctData::Series::BScanList bscans  = series->getBScans();

	SloCoordTranslator transform(*series, getImageScaleFactor());
	transform.setClipShift(OctData::CoordSLOpx(clipX1, clipY1));
	const OctData::CoordSLOpx clickPos(x,y);

	int    nearstScan = 0;
	double nearstDist = std::numeric_limits<double>::infinity();

	int bscanCounter = 0;
	for(const OctData::BScan* bscan : bscans)
	{
		if(bscan)
		{
			double dist = calcAbsBScan(*bscan, transform, clickPos);
			if(dist < nearstDist)
			{
				nearstScan = bscanCounter;
				nearstDist = dist;
			}
		}
		++bscanCounter;
	}

	if(nearstDist < tol)
		return nearstScan;
	return -1;
}

void SLOImageWidget::mousePressEvent(QMouseEvent* e)
{
	int bscan = getBScanNearPos(e->x(), e->y(), 5);
	if(bscan >= 0)
	{
		e->accept();
		OctMarkerManager::getInstance().chooseBScan(bscan);
	}
}



void SLOImageWidget::wheelEvent(QWheelEvent* wheelE)
{
	int deltaWheel = wheelE->delta();
	if(deltaWheel < 0)
		markerManger.previousBScan();
	else
		markerManger.nextBScan();
	wheelE->accept();
}


void SLOImageWidget::setBScanVisibility(int opt)
{
	switch(opt)
	{
		case 0:
			drawBScans       = false;
			drawOnylActBScan = false;
			drawConvexHull   = false;
			break;
		case 1:
			drawBScans       = true;
			drawOnylActBScan = true;
			drawConvexHull   = true;
			break;
		case 2:
			drawBScans       = true;
			drawOnylActBScan = false;
			drawConvexHull   = false;
			break;
	}
	update();
}

void SLOImageWidget::saveLatexImageSlot()
{
	OctDataManager& manager = OctDataManager::getInstance();
	const OctData::Series* series = manager.getSeries();
	if(!series)
		return;

	const int seriesId = series->getInternalId();
	const QString& loadedFilename = manager.getLoadedFilename();
	const QFileInfo fileinfo(loadedFilename);
	QString filename = QFileDialog::getSaveFileName(this, tr("Choose a base filename to save latex images"), fileinfo.baseName() + "_" + QString::number(seriesId) + ".tex", "*.tex");
	if(!filename.isEmpty())
		saveLatexImage(filename);
}

void SLOImageWidget::saveLatexImage(const QString& filename) const
{
	OctDataManager& manager = OctDataManager::getInstance();
	const OctData::Series* series = manager.getSeries();
	if(!series)
		return;

	std::ofstream stream(filename.toStdString());
	if(!stream.good())
		return;

	QFileInfo fileinfo(filename);
	std::string cleanFilename = fileinfo.fileName().toStdString();
	std::string path = fileinfo.path().toStdString();
	std::transform(cleanFilename.begin(), cleanFilename.end(), cleanFilename.begin(), [](char c) -> char { return c=='.'?'_':c; });
	std::string imageFilename = cleanFilename + "_base.jpg";
	std::string imageOverlayFilename = cleanFilename + "_overlay.png";
	std::string imageOverlayLegendFilename = cleanFilename + "_legend.pdf";

	cv::imwrite(path + "/" + imageFilename, series->getSloImage().getImage(), {CV_IMWRITE_JPEG_QUALITY, 75});

	bool overlayCreated = false;
	QWidget* overlayLegendWidget = nullptr;
	QString overlayLatexString;
	BscanMarkerBase* actMarker = markerManger.getActBscanMarker();
	if(actMarker)
	{
		cv::Mat outImage;
		overlayCreated = actMarker->drawSLOOverlayImage(cvImage, outImage, -1);
		if(overlayCreated && !outImage.empty())
		{
			cvtColor(outImage, outImage, CV_BGRA2RGBA);
			cv::imwrite(path + "/" + imageOverlayFilename, outImage);

			WidgetOverlayLegend* overlayLegend = actMarker->getSloLegendWidget();

			if(overlayLegend)
			{
				overlayLatexString  = overlayLegend->getLatexCode();
				if(overlayLatexString.isEmpty())
					overlayLegendWidget = overlayLegend->getWidget();
			}

			if(overlayLegendWidget)
			{
				QPdfWriter generator(QString::fromStdString(path + "/" + imageOverlayLegendFilename));
				generator.setTitle(tr("overlay legend"));;
				generator.setPageSize(QPageSize(overlayLegendWidget->size()));
				generator.setResolution(100);

				QPainter painter;
				QFont font;
				font.setPointSizeF(font.pointSizeF()*2);
				painter.begin(&generator);
				painter.setFont(font);
				overlayLegendWidget->render(&painter, QPoint(), QRegion(), DrawChildren);
// 				const QPoint &targetOffset = QPoint(), const QRegion &sourceRegion = QRegion(), RenderFlags renderFlags = RenderFlags( DrawWindowBackground | DrawChildren ))
				painter.end();
			}
		}
	}


	double height = static_cast<double>(cvImage.cols);
	double width  = static_cast<double>(cvImage.rows);

	double imgWidth  = height/30.;
	double imgHeight = width /30.;

	double aspectRatio = height/width;
	double tikzFactorX = 1./width             ;
	double tikzFactorY = 1./height*aspectRatio;
	const double scale = std::min(imgWidth, imgHeight);


	stream << "\\documentclass{standalone}\n\\usepackage{tikz}\n\n";
	stream << "\n\\begin{document}";

	if(!overlayLatexString.isEmpty())
		stream << "\n\t\\begin{minipage}[c]{.66\\linewidth}";

	stream << "\n\n\t\\begin{tikzpicture}";
	stream << "\n\t\t\\def\\hulllinewidth{1.75mm}";
	stream << "\n\t\t\\def\\actbscanlinewidth{1.25mm}\n\n";

	stream << "\n\t\t\\definecolor{convexHullColor}{rgb}{0.5,1.0,0.5}";
	stream << "\n\t\t\\definecolor{actBScanColor}{rgb}{0.5,0.75,1.0}";



	OctData::CoordSLOmm p1 = series->getLeftUpperCoord();
	OctData::CoordSLOmm p2 = series->getRightLowerCoord();
	SloCoordTranslator sct(*series, ScaleFactor(scale*tikzFactorX, scale*tikzFactorY));
	OctData::CoordSLOpx p1px = sct(p1);
	OctData::CoordSLOpx p2px = sct(p2);
	stream << "\n\n% clip to scan area\n";
	if(!ProgramOptions::sloClipScanArea())
		stream << "%";
	stream << "\t\t\\clip (" << p1px.getXf() << ", " << (imgHeight - p1px.getYf()) << ") rectangle (" << p2px.getXf() << ", " << (imgHeight - p2px.getYf()) << ");";

	stream << "\n\n\t\t\\node[anchor=south west, inner sep=0, scale=1] (Bild) at (0,0) {\\includegraphics[width=" << imgWidth << "cm,height=" << imgHeight << "cm]{" << imageFilename << "}};";
	if(overlayCreated)
		stream << "\n\t\t\\node[anchor=south west, inner sep=0, scale=1, opacity=" << ProgramOptions::sloOverlayAlpha() << "] (Overlay) at (0,0) {\\includegraphics[width=" << imgWidth << "cm,height=" << imgHeight << "cm]{" << imageOverlayFilename << "}};";

	if(overlayLegendWidget)
		stream << "\n\t\t\\node[anchor=south west,inner sep=0,scale=1] (legend) at (" << imgWidth*1.04 << "cm,0) {\\includegraphics[height=" << imgHeight << "cm]{" << imageOverlayLegendFilename << "}};";


	stream << "\n\n\t\t\\begin{scope}[xscale=" << scale << ",yscale=" << scale << "]";

	const OctData::Series::BScanSLOCoordList& hull = series->getConvexHull();
	if(hull.size() >= 3)
	{
		const ScaleFactor tikzScale(tikzFactorX, tikzFactorY);
		const SloCoordTranslator transform(*series, tikzScale);

		stream << "\n\t\t\t\\draw[convexHullColor, line width=\\hulllinewidth] ";
		bool firstVal = true;
		std::size_t pos = 0;

		for(std::size_t i = 0; i < hull.size(); ++i)
		{
			if(firstVal)
				firstVal = false;
			else
			{
				if(pos % 5 == 0)
					stream << "\n\t\t\t";
				stream << " -- ";
			}

			const OctData::CoordSLOpx pointPx = transform(hull[i]);
			stream << "(" << (pointPx.getXf()) << ", " << (aspectRatio - pointPx.getYf()) << ")";
			++pos;
		}

		stream <<  " -- cycle;";

		const OctData::BScan* bscan = markerManger.getActBScan();
		if(bscan)
		{
			if(bscan->getBScanType() == OctData::BScan::BScanType::Line)
			{
				stream << "\n\t\t\t\\draw[actBScanColor, line width=\\actbscanlinewidth, ->] ";
				const OctData::CoordSLOpx pointPxStart = transform(bscan->getStart());
				const OctData::CoordSLOpx pointPxEnd   = transform(bscan->getEnd()  );
				stream << "(" << (pointPxStart.getXf()) << ", " << (aspectRatio - pointPxStart.getYf()) << ") -- ";
				stream << "(" << (pointPxEnd  .getXf()) << ", " << (aspectRatio - pointPxEnd  .getYf()) << ");";
			}
		}
	}



	stream << "\n\t\t\\end{scope}";
	stream << "\n\t\\end{tikzpicture}";

	if(!overlayLatexString.isEmpty())
	{
		stream << "\n\\end{minipage}";

		stream << "\n\\begin{minipage}[c]{.32\\linewidth}";
		stream << "\n" << overlayLatexString.toStdString();
		stream << "\n\\end{minipage}";
	}

	stream << "\n\\end{document}\n";
}
