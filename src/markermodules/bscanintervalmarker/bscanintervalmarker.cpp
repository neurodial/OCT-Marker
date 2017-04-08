#include "bscanintervalmarker.h"


#include <QIcon>
#include <QColor>
#include <QPixmap>

#include <QToolBar>
#include <QActionGroup>

#include <QPainter>
#include <QAction>
#include <QToolTip>

#include <QMouseEvent>
#include <widgets/bscanmarkerwidget.h>

#include <manager/octmarkermanager.h>
#include <helper/actionclasses.h>

#include <octdata/datastruct/series.h>
#include <octdata/datastruct/bscan.h>

#include "bscanintervalptree.h"
#include "definedintervalmarker.h"
#include "wgintervalmarker.h"

// namespace
// {
// 	QIcon createColorIcon(const QColor& color)
// 	{
// 		QPixmap pixmap(15, 15);
// 		pixmap.fill(color);
// 		return QIcon(pixmap);
// 	}
// }
//

BScanIntervalMarker::BScanIntervalMarker(OctMarkerManager* markerManager)
: BscanMarkerBase(markerManager)
, widgetPtr2WGIntevalMarker(new WGIntervalMarker(this))
{
	name = tr("Interval marker");
	id   = "IntervalMarker";
	icon = QIcon(":/icons/intervall_edit.png");

// 	markersCollectionsData.resize(DefinedIntervalMarker::getInstance().getIntervallMarkerMap().size());
	
	// connect(markerManager, &BScanMarkerManager::newSeriesShowed, this, &BScanIntervalMarker::newSeriesLoaded);



	for(const auto& obj : DefinedIntervalMarker::getInstance().getIntervallMarkerMap())
	{
		markersCollectionsData[obj.first].markerCollection = &(obj.second);
	}

}

BScanIntervalMarker::~BScanIntervalMarker()
{
	delete widgetPtr2WGIntevalMarker;
}



/*
void BScanIntervalMarker::addMarkerCollection2Toolbar(const IntervalMarker& markers, QToolBar& markerToolbar, QActionGroup& actionGroupMarker, std::vector<QAction*>& actionList, QObject* parent)
{
	std::size_t actMarkerId = actMarker.getInternalId();

	for(const IntervalMarker::Marker& marker : markers.getIntervalMarkerList())
	{
		std::size_t markerId = marker.getInternalId();
		QIcon icon = createColorIcon(QColor::fromRgb(marker.getRed(), marker.getGreen(), marker.getBlue()));

		SizetValueAction* markerAction = new SizetValueAction(markerId, parent);
		// markerAction->setCheckable(true);
		markerAction->setText(QString::fromStdString(marker.getName()));
		markerAction->setIcon(icon);
		markerAction->setChecked(actMarkerId == markerId);
		connect(markerAction, &SizetValueAction::triggered         , this        , &BScanIntervalMarker::chooseMarkerID);
		connect(this        , &BScanIntervalMarker::markerIdChanged, markerAction, &SizetValueAction::valueChanged     );

		actionGroupMarker.addAction(markerAction);
		markerToolbar.addAction(markerAction);

		actionList.push_back(markerAction);
	}
}
*/



QToolBar* BScanIntervalMarker::createToolbar(QObject* parent)
{
	QActionGroup*  actionGroupMarker  = new QActionGroup (parent);
	QActionGroup*  actionGroupMethod  = new QActionGroup (parent);
	QToolBar*      toolBar            = new QToolBar(tr("Interval marker"));

	toolBar->setObjectName("ToolBarIntervalMarker");

// 	const IntervalMarker& intervalMarker = IntervalMarker::getInstance();

// 	markersActions.resize(IntervalMarker::Marker::getMaxInternalId() + 1); // +1 for undefined marker

	/*
	for(const auto& obj : DefinedIntervalMarker::getInstance().getIntervallMarkerMap())
	{
		std::vector<QAction*> markerActionList;
		addMarkerCollection2Toolbar(obj.second, *toolBar, *actionGroupMarker, markerActionList, parent);
		toolBar->addSeparator();
		markersCollectionsData[obj.first].markersActions = markerActionList;
	}
*/



	IntValueAction* paintMarkerAction = new IntValueAction(static_cast<int>(Method::Paint), parent);
	paintMarkerAction->setCheckable(true);
	paintMarkerAction->setText(tr("paint marker"));
	paintMarkerAction->setIcon(QIcon(":/icons/paintbrush.png"));
	paintMarkerAction->setChecked(markerMethod == Method::Paint);
	connect(paintMarkerAction, &IntValueAction::triggered                , this             , static_cast<void(BScanIntervalMarker::*)(int)>(&BScanIntervalMarker::chooseMethodID));
	connect(this             , &BScanIntervalMarker::markerMethodChanged, paintMarkerAction, &IntValueAction::valueChanged        );
	actionGroupMethod->addAction(paintMarkerAction);
	toolBar->addAction(paintMarkerAction);

	IntValueAction* fillMarkerAction = new IntValueAction(static_cast<int>(Method::Fill), parent);
	fillMarkerAction->setCheckable(true);
	fillMarkerAction->setText(tr("fill marker"));
	fillMarkerAction->setIcon(QIcon(":/icons/paintcan.png"));
	fillMarkerAction->setChecked(markerMethod == Method::Fill);
	connect(fillMarkerAction, &IntValueAction::triggered                , this            , static_cast<void(BScanIntervalMarker::*)(int)>(&BScanIntervalMarker::chooseMethodID));
	connect(this            , &BScanIntervalMarker::markerMethodChanged, fillMarkerAction, &IntValueAction::valueChanged        );
	actionGroupMethod->addAction(fillMarkerAction);
	toolBar->addAction(fillMarkerAction);


	actionGroupMarker->setExclusive(true);
	actionGroupMethod->setExclusive(true);

	
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
	double scaleFactor = widget->getImageScaleFactor();
	RedrawRequest result;
	result.redraw = true;

	if(markerActiv)
	{
		switch(getMarkerMethod())
		{
			case Method::Paint:
				if(clickPos.x() != event->x() && markerActiv)
				{
					// std::cout << __FUNCTION__ << ": " << clickPos << " - " << event->x() << std::endl;
					setMarker(static_cast<int>(clickPos.x()/scaleFactor + 0.5), static_cast<int>(event->x()/scaleFactor + 0.5));
				}
				break;
			case Method::Fill:
				if(markerActiv)
					fillMarker(static_cast<int>(clickPos.x()/scaleFactor + 0.5));
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
	setMarker(x1, x2, actMarker, getActBScanNr());
}

void BScanIntervalMarker::setMarker(int x1, int x2, const Marker& type)
{
	setMarker(x1, x2, type     , getActBScanNr());
}

void BScanIntervalMarker::setMarker(int x1, int x2, const Marker& type, std::size_t bscan)
{
	if(!actCollection || bscan >= getMarkerMapSize(actCollection))
		return;

// 	printf("BScanMarkerManager::setMarker(%d, %d, %d)", x1, x2, type);
// 	std::cout << std::endl;

	if(x2 < x1)
		std::swap(x1, x2);

	int maxWidth = getBScanWidth();

	if(x2 < 0 || x1 > maxWidth)
		return;

	if(x1 < 0)
		x1 = 0;
	if(x2 >= maxWidth)
		x2 = maxWidth - 1;

	actCollection->markers[bscan].set(std::make_pair(boost::icl::discrete_interval<int>::closed(x1, x2), type));
	dataChanged = true;
}


void BScanIntervalMarker::fillMarker(int x, const Marker& type)
{
	std::size_t bscan = getActBScanNr();

	if(!actCollection || bscan >= getMarkerMapSize(actCollection))
		return;

	MarkerMap& map = actCollection->markers[bscan];
	MarkerMap::const_iterator it = map.find(x);

	boost::icl::discrete_interval<int> intervall = it->first;

	if(intervall.upper() > 0)
	{
		map.set(std::make_pair(boost::icl::discrete_interval<int>::closed(intervall.lower(), intervall.upper()), type));
		dataChanged = true;
	}
}

bool BScanIntervalMarker::toolTipEvent(QEvent* event, BScanMarkerWidget* widget)
{
	if(!event || !widget)
		return false;

	if(event->type() == QEvent::ToolTip)
	{
		double scaleFactor = widget->getImageScaleFactor();
		QHelpEvent* helpEvent = static_cast<QHelpEvent*>(event);
		helpEvent->pos();

		const MarkerMap& markerMap = getMarkers();
		MarkerMap::const_iterator it = markerMap.find(static_cast<int>(helpEvent->pos().x()/scaleFactor));
		const Marker& marker = it->second;

		QToolTip::showText(helpEvent->globalPos(), QString::fromStdString(marker.getName()));
		return true;
	}
	return false;
}


bool BScanIntervalMarker::keyPressEvent(QKeyEvent* e ,BScanMarkerWidget*)
{
	switch(e->key())
	{
		case Qt::Key_Escape:
			markerActiv = false;
			return true;
		case Qt::Key_1:
			chooseMarkerID(0);
			return true;
		case Qt::Key_2:
			chooseMarkerID(1);
			return true;
		case Qt::Key_3:
			chooseMarkerID(2);
			return true;
		case Qt::Key_4:
			chooseMarkerID(3);
			return true;
		case Qt::Key_9:
			chooseMethodID(Method::Paint);
			return true;
		case Qt::Key_0:
			chooseMethodID(Method::Fill);
			return true;
	}
	return false;
}

bool BScanIntervalMarker::leaveWidgetEvent(QEvent* ,BScanMarkerWidget*)
{
	mouseInWidget = false;
	return true;
}

void BScanIntervalMarker::drawMarker(QPainter& painter, BScanMarkerWidget* widget, const QRect&) const
{
	double scaleFactor = widget->getImageScaleFactor();
	
	for(const MarkerMap::interval_mapping_type pair : getMarkers())
	{
		IntervalMarker::Marker marker = pair.second;
		if(marker.isDefined())
		{
			boost::icl::discrete_interval<int> itv  = pair.first;
			painter.fillRect(static_cast<int>(itv.lower()*scaleFactor + 0.5)
			               , 0
			               , static_cast<int>((itv.upper()-itv.lower())*scaleFactor + 0.5)
			               , widget->height()
			               , QColor(marker.getRed()
			               , marker.getGreen()
			               , marker.getBlue()
			               , 60));
		}
	}
	

	if(mouseInWidget && markerActive && getMarkerMethod() == Method::Paint)
	{
		painter.drawLine(mousePos.x(), 0, mousePos.x(), widget->height());

		if(markerActiv)
		{
			const IntervalMarker::Marker& marker = getActMarker();
			if(mousePos.x() != clickPos.x())
			{
				painter.drawLine(clickPos.x(), 0, clickPos.x(), widget->height());
				QPen pen;
				pen.setColor(QColor(marker.getRed(), marker.getGreen(), marker.getBlue(), 255));
				pen.setWidth(5);
				painter.setPen(pen);
				painter.drawLine(mousePos, clickPos);
			}
		}
	}
}

void BScanIntervalMarker::drawBScanSLOLine(QPainter& painter, int bscanNr, const OctData::CoordSLOpx& start_px, const OctData::CoordSLOpx& end_px, SLOImageWidget*) const
{
	if(!actCollection || bscanNr < 0 || bscanNr >= static_cast<int>(getMarkerMapSize(actCollection)))
		return;

	double bscanWidth = getBScanWidth();
	QPen pen;
	pen.setWidth(3);

	for(const MarkerMap::interval_mapping_type pair : getMarkers(static_cast<std::size_t>(bscanNr)))
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


void BScanIntervalMarker::resetMarkers(const OctData::Series* series)
{
	if(!series)
		return;

	const std::size_t numBscans = series->bscanCount();

	for(MarkersCollectionsDataList::value_type obj : markersCollectionsData)
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
}


void BScanIntervalMarker::saveState(boost::property_tree::ptree& markerTree)
{
	BScanIntervalPTree::fillPTree(markerTree, this);
}


void BScanIntervalMarker::loadState(boost::property_tree::ptree& markerTree)
{
	resetMarkers(getSeries());
	BScanIntervalPTree::parsePTree(markerTree, this);
}


QRect BScanIntervalMarker::getWidgetPaintSize(const QPoint& p1, const QPoint& p2, double factor, const QPoint* p3)
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
	QRect rect = QRect(minX-broder, 0, maxX-minX+2*broder, static_cast<int>(getBScanHight()*factor+0.5)); // old and new pos

	return rect;
}

bool BScanIntervalMarker::setMarkerCollection(const std::string& internalName)
{
	qDebug("setMarkerCollection(const std::string& internalName): %s", internalName.c_str());
	MarkersCollectionsDataList::iterator it = markersCollectionsData.find(internalName);
	if(it != markersCollectionsData.end())
	{
		actCollection = &(it->second);
		return true;
	}
	return false;
}


void BScanIntervalMarker::chooseMarkerID(int id)
{
	qDebug("chooseMarkerID(int id): %d", id);
	if(actCollection)
	{
		const IntervalMarker* markerCollection = actCollection->markerCollection;
		if(!markerCollection)
			return;

		actMarker = markerCollection->getMarkerFromID(id);
		markerIdChanged(id);
	}
}

