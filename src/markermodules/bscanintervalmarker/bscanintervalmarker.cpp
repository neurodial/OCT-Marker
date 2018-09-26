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

#define _USE_MATH_DEFINES

#include "bscanintervalmarker.h"

#include <cmath>

// for mingw
#ifndef M_PI
	#define M_PI 3.1415926535897932384626433832795
#endif


#include <QIcon>
#include <QColor>
#include <QPixmap>

#include <QToolBar>
#include <QActionGroup>

#include <QPainter>
#include <QAction>
#include <QToolTip>

#include <QMouseEvent>
#include <QElapsedTimer>

#include<opencv/cv.hpp>

#include <widgets/bscanmarkerwidget.h>

#include <manager/octmarkermanager.h>
#include <helper/actionclasses.h>

#include <octdata/datastruct/series.h>
#include <octdata/datastruct/bscan.h>

#include "bscanintervalptree.h"
#include "definedintervalmarker.h"
#include "wgintervalmarker.h"
#include "importintervalmarker.h"
#include"slointervallmap.h"
#include <manager/octdatamanager.h>

#include<data_structure/programoptions.h>
#include<helper/signalblocker.h>

BScanIntervalMarker::BScanIntervalMarker(OctMarkerManager* markerManager)
: BscanMarkerBase(markerManager)
, widgetOverlayLegend(*this)
, sloOverlayImage(new cv::Mat)
{
	name = tr("Interval marker");
	id   = "IntervalMarker";
	icon = QIcon(":/icons/typicons_mod/intervall_mark.svg");


	createMarkerMethodActions();
	widgetPtr2WGIntevalMarker = new WGIntervalMarker(this); // Important: create after function createMarkerMethodActions

	for(const auto& obj : DefinedIntervalMarker::getInstance().getIntervallMarkerMap())
	{
		markersCollectionsData[obj.first].markerCollection = &(obj.second);
	}

	actCollection = markersCollectionsData.begin();
}

BScanIntervalMarker::~BScanIntervalMarker()
{
	delete widgetPtr2WGIntevalMarker;
}


void BScanIntervalMarker::createMarkerMethodActions()
{
	QActionGroup* actionGroupMethod  = new QActionGroup(this);
	actionGroupMethod->setExclusive(true);


	IntValueAction* paintMarkerAction = new IntValueAction(static_cast<int>(Method::Paint), actionGroupMethod);
	paintMarkerAction->setCheckable(true);
	paintMarkerAction->setText(tr("paint marker"));
	paintMarkerAction->setIcon(QIcon(":/icons/paintbrush.png"));
	paintMarkerAction->setChecked(markerMethod == Method::Paint);
	connect(paintMarkerAction, &IntValueAction::triggered               , this             , static_cast<void(BScanIntervalMarker::*)(int)>(&BScanIntervalMarker::chooseMethodID));
	connect(this             , &BScanIntervalMarker::markerMethodChanged, paintMarkerAction, &IntValueAction::valueChanged        );
	actionGroupMethod->addAction(paintMarkerAction);
	markerMethodActions.push_back(paintMarkerAction);

	IntValueAction* fillMarkerAction = new IntValueAction(static_cast<int>(Method::Fill), actionGroupMethod);
	fillMarkerAction->setCheckable(true);
	fillMarkerAction->setText(tr("fill marker"));
	fillMarkerAction->setIcon(QIcon(":/icons/paintcan.png"));
	fillMarkerAction->setChecked(markerMethod == Method::Fill);
	connect(fillMarkerAction, &IntValueAction::triggered               , this            , static_cast<void(BScanIntervalMarker::*)(int)>(&BScanIntervalMarker::chooseMethodID));
	connect(this            , &BScanIntervalMarker::markerMethodChanged, fillMarkerAction, &IntValueAction::valueChanged        );
	actionGroupMethod->addAction(fillMarkerAction);
	markerMethodActions.push_back(fillMarkerAction);

	connect(&ProgramOptions::intervallMarkSloMapAuteGenerate, &OptionBool::trueSignal, this, &BScanIntervalMarker::generateSloMap);
}




QToolBar* BScanIntervalMarker::createToolbar(QObject* /*parent*/)
{
	QToolBar* toolBar = new QToolBar(tr("Interval marker"));
	toolBar->setObjectName("ToolBarIntervalMarker");

	for(QAction* action : markerMethodActions)
		toolBar->addAction(action);

	connectToolBar(toolBar);
	return toolBar;
}


BscanMarkerBase::RedrawRequest  BScanIntervalMarker::mouseMoveEvent(QMouseEvent* event, BScanMarkerWidget* widget)
{
	RedrawRequest result;
	if(markerActive)
	{
		result.redraw = true;

		if(markerActiv)
			result.rect = getWidgetPaintSize(event->pos(), mousePos, widget->getImageScaleFactor(), &clickPos);
		else
			result.rect = getWidgetPaintSize(event->pos(), mousePos, widget->getImageScaleFactor());
	}

	mouseInWidget = true;
	mousePos = event->pos();

	return result;
}

BscanMarkerBase::RedrawRequest BScanIntervalMarker::mousePressEvent(QMouseEvent* event, BScanMarkerWidget*)
{
	RedrawRequest result;
	result.redraw = false;

	if(event->button() == Qt::LeftButton)
	{
		clickPos = event->pos();
		mousePos = event->pos();
		markerActiv = true;
	}
	else
		markerActiv = false;
	return result;
}

BscanMarkerBase::RedrawRequest BScanIntervalMarker::mouseReleaseEvent(QMouseEvent* event, BScanMarkerWidget* widget)
{
	const ScaleFactor& scaleFactor = widget->getImageScaleFactor();
	RedrawRequest result;
	result.redraw = true;

	const double scaleFactorX = scaleFactor.getFactorX();

	if(markerActiv)
	{
		switch(getMarkerMethod())
		{
			case Method::Paint:
				if(clickPos.x() != event->x() && markerActiv)
				{
					// std::cout << __FUNCTION__ << ": " << clickPos << " - " << event->x() << std::endl;
					setMarker(static_cast<int>(clickPos.x()/scaleFactorX + 0.5), static_cast<int>(event->x()/scaleFactorX + 0.5));
				}
				break;
			case Method::Fill:
				if(markerActiv)
					fillMarker(static_cast<int>(clickPos.x()/scaleFactorX + 0.5));
				break;
		}
		result.rect = getWidgetPaintSize(event->pos(), mousePos, scaleFactor, &clickPos);
		markerActiv = false;
	}
	else
		result.rect = getWidgetPaintSize(event->pos(), mousePos, scaleFactor);

	return result;
}

void BScanIntervalMarker::setMarker(int x1, int x2)
{
	setMarker(x1, x2, actMarker, getActBScanNr(), actCollection);
}

void BScanIntervalMarker::setMarker(int x1, int x2, const Marker& type)
{
	setMarker(x1, x2, type     , getActBScanNr(), actCollection);
}

void BScanIntervalMarker::setMarker(int x1, int x2, const Marker& type, std::size_t bscan)
{
	setMarker(x1, x2, type, bscan, actCollection);
}

void BScanIntervalMarker::setMarker(int x1, int x2, const Marker& type, std::size_t bscan, BScanIntervalMarker::MarkerCollectionWork& w)
{
	setMarker(x1, x2, type, bscan, w.actCollection);
}


void BScanIntervalMarker::setMarker(int x1, int x2, const Marker& type, std::size_t bscan, MarkersCollectionsDataList::iterator& collection)
{
	if(collection == markersCollectionsData.end())
		return;

	if(bscan >= collection->second.markers.size())
		return;

	if(x1 == x2)
		return;

	if(x2 < x1)
		std::swap(x1, x2);

	int maxWidth = getBScanWidth();

	if(x2 < 0 || x1 > maxWidth)
		return;

	if(x1 < 0)
		x1 = 0;
	if(x2 > maxWidth)
		x2 = maxWidth;

	collection->second.markers[bscan].set(std::make_pair(boost::icl::discrete_interval<int>::closed(x1, x2), type));
	stateChangedSinceLastSave = true;
	stateChangedInActBScan    = true;
	sloViewHasChanged();
}



void BScanIntervalMarker::fillMarker(int x, const Marker& type)
{
	std::size_t bscan = getActBScanNr();

	if(bscan >= getMarkerMapSize())
		return;

	MarkerMap& map = actCollection->second.markers[bscan];
	MarkerMap::const_iterator it = map.find(x);

	if(it->second == type)
		return;

	boost::icl::discrete_interval<int> intervall = it->first;

	if(intervall.upper() > 0)
	{
		map.set(std::make_pair(boost::icl::discrete_interval<int>::closed(intervall.lower(), intervall.upper()), type));
		stateChangedSinceLastSave = true;
		stateChangedInActBScan    = true;
		requestFullUpdate();
// 		sloViewHasChanged(); // handled by requestFullUpdate
	}
}

bool BScanIntervalMarker::toolTipEvent(QEvent* event, BScanMarkerWidget* widget)
{
	if(!event || !widget)
		return false;

	if(event->type() == QEvent::ToolTip)
	{
		const ScaleFactor& scaleFactor = widget->getImageScaleFactor();
		QHelpEvent* helpEvent = static_cast<QHelpEvent*>(event);
		helpEvent->pos();

		const MarkerMap& markerMap = getMarkers();
		MarkerMap::const_iterator it = markerMap.find(static_cast<int>(helpEvent->pos().x()/scaleFactor.getFactorX()));
		const Marker& marker = it->second;

		if(marker.isDefined())
			QToolTip::showText(helpEvent->globalPos(), QString::fromStdString(marker.getName()));
		else
			QToolTip::hideText();

		return true;
	}
	return false;
}


bool BScanIntervalMarker::keyPressEvent(QKeyEvent* e ,BScanMarkerWidget*)
{
	int key = e->key();
	if(key >= Qt::Key_0 && key <= Qt::Key_9)
	{
		return chooseMarkerID(key-Qt::Key_0);
	}
	else
	{
		switch(key)
		{
			case Qt::Key_Escape:
				markerActiv = false;
				return true;
			case Qt::Key_P:
				chooseMethodID(Method::Paint);
				return true;
			case Qt::Key_F:
				chooseMethodID(Method::Fill);
				return true;
			case Qt::Key_T:
				generateSloMap();
				return true;
		}
	}
	return false;
}


void BScanIntervalMarker::generateSloMap()
{
	QElapsedTimer timer;
	timer.start();

	OctDataManager& manager = OctDataManager::getInstance();
	const SloBScanDistanceMap* distMap = manager.getSeriesSLODistanceMap();
	if(distMap && actCollectionValid())
	{
		SloIntervallMap tm;

		tm.createMap(*distMap, actCollection->second.markers, getSeries());
// 					tm.createMap(*distMap, lines, OctData::Segmentationlines::SegmentlineType::ILM, OctData::Segmentationlines::SegmentlineType::BM, factor, *thicknessmapColor);
		*sloOverlayImage = tm.getSloMap();
		requestSloOverlayUpdate();

		std::cout << "Creating slomap took " << timer.elapsed() << " milliseconds" << std::endl;
	}
}


bool BScanIntervalMarker::leaveWidgetEvent(QEvent* ,BScanMarkerWidget*)
{
	mouseInWidget = false;
	return true;
}

void BScanIntervalMarker::drawMarker(QPainter& painter, BScanMarkerWidget* widget, const QRect&) const
{
	const double scaleFactorX = widget->getImageScaleFactor().getFactorX();
	
	const MarkerMap& markerMap = getMarkers();
	for(const MarkerMap::interval_mapping_type pair : markerMap)
	{
		IntervalMarker::Marker marker = pair.second;
		if(marker.isDefined())
		{
			boost::icl::discrete_interval<int> itv  = pair.first;
			painter.fillRect(static_cast<int>(itv.lower()*scaleFactorX + 0.5)
			               , 0
			               , static_cast<int>((itv.upper()-itv.lower())*scaleFactorX + 0.5)
			               , widget->height()
			               , QColor(marker.getRed()
			                      , marker.getGreen()
			                      , marker.getBlue()
			                      , transparency));
		}
	}
	

	if(mouseInWidget && markerActive && getMarkerMethod() == Method::Paint)
	{
		class QPainterRestorer
		{
			QPainter& painter;
			bool restored = false;
		public:
			QPainterRestorer(QPainter& painter) : painter(painter) { painter.save(); }
			~QPainterRestorer() { if(!restored) painter.restore(); }
			void restore() { if(!restored) painter.restore(); restored = true; }
		};

		QPainterRestorer painterRestorer(painter);
		painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
		painter.setPen(QColor(0xff, 0xff, 0xff));

		painter.drawLine(mousePos.x(), 0, mousePos.x(), widget->height());

		if(markerActiv)
		{
			const IntervalMarker::Marker& marker = getActMarker();
			if(mousePos.x() != clickPos.x())
			{
				painter.drawLine(clickPos.x(), 0, clickPos.x(), widget->height());
				painterRestorer.restore();

				QPen pen;
				pen.setColor(QColor(marker.getRed(), marker.getGreen(), marker.getBlue(), 255));
				pen.setWidth(5);
				painter.setPen(pen);
				painter.drawLine(mousePos, clickPos);
			}
		}
	}
}

void BScanIntervalMarker::drawBScanSLOLine(QPainter& painter, std::size_t bscanNr, const OctData::CoordSLOpx& start_px, const OctData::CoordSLOpx& end_px, SLOImageWidget*) const
{
	if(bscanNr >= getMarkerMapSize())
		return;

	double bscanWidth = getBScanWidth(bscanNr);
	QPen pen;
	pen.setWidth(3);

	for(const MarkerMap::interval_mapping_type pair : getMarkers(bscanNr))
	{
		IntervalMarker::Marker marker = pair.second;
		if(marker.isDefined())
		{
			boost::icl::discrete_interval<int> itv  = pair.first;

			double f1 = static_cast<double>(itv.lower())/bscanWidth;
			double f2 = static_cast<double>(itv.upper())/bscanWidth;

			const OctData::CoordSLOpx p1 = start_px*(1.-f1) + end_px*f1;
			const OctData::CoordSLOpx p2 = start_px*(1.-f2) + end_px*f2;

			// pen.setColor(*(intervallColors.at(markerQ)));
			pen.setColor(QColor(marker.getRed(), marker.getGreen(), marker.getBlue(), 255));
			painter.setPen(pen);
			painter.drawLine(QPointF(p1.getXf(), p1.getYf()), QPointF(p2.getXf(), p2.getYf()));
		}
	}
}

void BScanIntervalMarker::drawBScanSLOCircle(QPainter& painter, std::size_t bscanNr, const OctData::CoordSLOpx& start_px, const OctData::CoordSLOpx& center_px, bool clockwise, SLOImageWidget*) const
{
	if(bscanNr >= getMarkerMapSize())
		return;

	double bscanWidth = getBScanWidth(bscanNr);
	QPen pen;
	pen.setWidth(3);


	double radius = start_px.abs(center_px);
	double ratio  = start_px.getXf() - center_px.getXf();
	double nullAngle = acos( ratio/radius )/M_PI/2;

	const int rotationFactor = clockwise?-1:1;

	for(const MarkerMap::interval_mapping_type pair : getMarkers(bscanNr))
	{
		IntervalMarker::Marker marker = pair.second;
		if(marker.isDefined())
		{
			boost::icl::discrete_interval<int> itv  = pair.first;

			double f1 = static_cast<double>(itv.lower())/bscanWidth;
			double f2 = static_cast<double>(itv.upper())/bscanWidth;

			pen.setColor(QColor(marker.getRed(), marker.getGreen(), marker.getBlue(), 255));
			painter.setPen(pen);
			QRectF rectangle(center_px.getX()-radius, center_px.getY()-radius, radius*2, radius*2);
			int startAngle = static_cast<int>((f1+nullAngle)*360*16)*rotationFactor;
			int spanAngle  = static_cast<int>((f2-f1       )*360*16)*rotationFactor;

			painter.drawArc(rectangle, startAngle, spanAngle);
		}
	}
}



void BScanIntervalMarker::resetMarkers(const OctData::Series* series)
{
	if(!series)
		return;

	const std::size_t numBscans = series->bscanCount();

	for(MarkersCollectionsDataList::value_type& obj : markersCollectionsData)
	{
		std::vector<MarkerMap>& markers = obj.second.markers;
		markers.clear();
		markers.resize(numBscans);

		for(std::size_t i = 0; i<numBscans; ++i)
		{
			int bscanWidth = series->getBScan(i)->getWidth();
			markers[i].set(std::make_pair(boost::icl::discrete_interval<int>::closed(0, bscanWidth), IntervalMarker::Marker()));
		}
	}

}


void BScanIntervalMarker::newSeriesLoaded(const OctData::Series* series, boost::property_tree::ptree& markerTree)
{
	if(!series)
		return;
	
	resetMarkers(series);
	BScanIntervalPTree::parsePTree(markerTree, this);
	stateChangedSinceLastSave = false;
	stateChangedInActBScan    = false;
}


void BScanIntervalMarker::saveState(boost::property_tree::ptree& markerTree)
{
	BScanIntervalPTree::fillPTree(markerTree, this);
	stateChangedSinceLastSave = false;
}


void BScanIntervalMarker::loadState(boost::property_tree::ptree& markerTree)
{
	SignalBlocker sb(this);
	resetMarkers(getSeries());
	BScanIntervalPTree::parsePTree(markerTree, this);
	stateChangedSinceLastSave = false;
	stateChangedInActBScan    = true;
}


QRect BScanIntervalMarker::getWidgetPaintSize(const QPoint& p1, const QPoint& p2, const ScaleFactor& factor, const QPoint* p3)
{
	const int broder = 5;
	int x1 = p1.x();
	int x2 = p2.x();

	int minX = std::min(x1, x2);
	int maxX = std::max(x1, x2);

	if(p3)
	{
		int x3 = p3->x();
		if(minX > x3)
			minX = x3;
		if(maxX < x3)
			maxX = x3;
	}

	// QRect rect = QRect(p1.x(), 0, p2.x(), getBScanHight()*factor).normalized(); // old and new pos
	QRect rect = QRect(minX-broder, 0, maxX-minX+2*broder, static_cast<int>(getBScanHight()*factor.getFactorY()+0.5)); // old and new pos

	return rect;
}

bool BScanIntervalMarker::setMarkerCollection(const std::string& internalName)
{
	MarkersCollectionsDataList::iterator it = markersCollectionsData.find(internalName);
	if(it != markersCollectionsData.end())
	{
		actCollection = it;
		autoGenerateSloMap();
		markerCollectionChanged(internalName);
		requestFullUpdate();
		return true;
	}
	return false;
}

BScanIntervalMarker::MarkerCollectionWork BScanIntervalMarker::getMarkerCollection(const std::string& internalName)
{
	return MarkerCollectionWork(markersCollectionsData.find(internalName));
}



bool BScanIntervalMarker::chooseMarkerID(int id)
{
	if(actCollectionValid())
	{
		const IntervalMarker* markerCollection = actCollection->second.markerCollection;
		if(!markerCollection)
			return false;
		if(markerCollection->size() <= static_cast<std::size_t>(id) || id < 0)
			return false;

		try
		{
			actMarker = markerCollection->getMarkerFromID(id);
			markerIdChanged(id);
			return true;
		}
		catch(std::out_of_range& e)
		{
			std::cerr << "Exception in BScanIntervalMarker::chooseMarkerID(int id)\n" << e.what() << '\n';
		}
	}
	return false;
}

const BScanIntervalMarker::MarkerMap& BScanIntervalMarker::getMarkers(std::size_t bscan) const
{
	if(actCollectionValid())
	{
		const std::vector<MarkerMap>& markerMaps = actCollection->second.markers;
		if(markerMaps.size() > bscan)
			return actCollection->second.markers[bscan];
	}
	return nullMarkerMap;
}


const BScanIntervalMarker::MarkerMap& BScanIntervalMarker::getMarkers(const std::string& collection, std::size_t bscan) const
{
	MarkersCollectionsDataList::const_iterator it = markersCollectionsData.find(collection);
	if(it != markersCollectionsData.end())
	{
		const MarkersCollectionData* foundCollection = &(it->second);
		if(foundCollection && bscan < foundCollection->markers.size())
			return foundCollection->markers[bscan];
	}
	return nullMarkerMap;
}


bool BScanIntervalMarker::importMarkerFromBin(const std::string& filename)
{
	if(!actCollectionValid())
		return false;

	const IntervalMarker* markerCollection = actCollection->second.markerCollection;
	if(!markerCollection)
		return false;

	bool result = ImportIntervalMarker::importBin(this, filename);
	if(result)
		requestFullUpdate();

	return result;
}

void BScanIntervalMarker::exportMarkerToBin(const std::string& filename)
{
	ImportIntervalMarker::exportBin(this, filename);
}



std::size_t BScanIntervalMarker::getMaxBscanWidth() const
{
	const std::size_t numBscans = getNumBScans();
	std::size_t maxBscanWidth = 0;

	for(std::size_t bscanNum = 0; bscanNum < numBscans; ++bscanNum)
	{
		const OctData::BScan* bscan = getBScan(bscanNum);
		if(bscan)
		{
			const std::size_t bscanWidth = static_cast<std::size_t>(bscan->getWidth());
			if(maxBscanWidth < bscanWidth)
				maxBscanWidth = bscanWidth;
		}
	}
	return maxBscanWidth;
}

bool BScanIntervalMarker::drawSLOOverlayImage(const cv::Mat& sloImage, cv::Mat& outSloImage, double alpha) const
{
	if(sloOverlayImage)
		return BscanMarkerBase::drawSLOOverlayImage(sloImage, outSloImage, alpha, *sloOverlayImage);
	return false;
}

void BScanIntervalMarker::setActBScan(std::size_t bscan)
{
	BscanMarkerBase::setActBScan(bscan);
	if(stateChangedInActBScan)
		autoGenerateSloMap();
	stateChangedInActBScan = false;
}

void BScanIntervalMarker::autoGenerateSloMap()
{
	if(ProgramOptions::intervallMarkSloMapAuteGenerate())
		generateSloMap();
}
