#include "bscanintervallmarker.h"


#include <QIcon>
#include <QColor>
#include <QPixmap>

#include <QToolBar>
#include <QActionGroup>
#include <QSignalMapper>

#include <QPainter>

#include <QAction>

#include <QMouseEvent>
#include <widgets/bscanmarkerwidget.h>

#include <manager/bscanmarkermanager.h>

#include <octdata/datastruct/series.h>
#include <octdata/datastruct/bscan.h>

namespace
{
	QIcon createColorIcon(const QColor& color)
	{
		QPixmap pixmap(15, 15);
		pixmap.fill(color);
		return QIcon(pixmap);
	}
}


BScanIntervallMarker::BScanIntervallMarker(BScanMarkerManager* markerManager)
: BscanMarkerBase(markerManager)
{
	connect(markerManager, &BScanMarkerManager::newSeriesShowed, this, &BScanIntervallMarker::newSeriesLoaded);
}



QToolBar* BScanIntervallMarker::createToolbar(QObject* parent)
{
	QActionGroup*  actionGroupMarker  = new QActionGroup (parent);
	QActionGroup*  actionGroupMethod  = new QActionGroup (parent);
	QSignalMapper* signalMapperMarker = new QSignalMapper(parent);
	QSignalMapper* signalMapperMethod = new QSignalMapper(parent);
	QToolBar*      toolBar            = new QToolBar(tr("Marker"));

	toolBar->setObjectName(tr("ToolBarIntervallMarker"));

	const IntervallMarker& intervallMarker = IntervallMarker::getInstance();

	markersActions.resize(IntervallMarker::Marker::getMaxInternalId() + 1); // +1 for undefined marker

	for(const IntervallMarker::Marker& marker : intervallMarker.getIntervallMarkerList())
	{
		QIcon icon = createColorIcon(QColor::fromRgb(marker.getRed(), marker.getGreen(), marker.getBlue()));

		QAction* markerAction = new QAction(parent);
		markerAction->setCheckable(true);
		markerAction->setText(QString::fromStdString(marker.getName()));
		markerAction->setIcon(icon);
		connect(markerAction, SIGNAL(triggered()), signalMapperMarker, SLOT(map()));
		signalMapperMarker->setMapping(markerAction, marker.getInternalId()) ;

		actionGroupMarker->addAction(markerAction);
		toolBar->addAction(markerAction);

		markersActions.at(marker.getInternalId()) = markerAction;
	}



	toolBar->addSeparator();


	paintMarkerAction = new QAction(parent);
	paintMarkerAction->setCheckable(true);
	paintMarkerAction->setText(tr("paint marker"));
	paintMarkerAction->setIcon(QIcon(":/icons/paintbrush.png"));
	paintMarkerAction->setShortcut(Qt::LeftArrow);
	connect(paintMarkerAction, SIGNAL(triggered()), signalMapperMethod, SLOT(map()));
	signalMapperMethod->setMapping(paintMarkerAction, static_cast<int>(Method::Paint));
	actionGroupMethod->addAction(paintMarkerAction);
	toolBar->addAction(paintMarkerAction);

	fillMarkerAction = new QAction(parent);
	fillMarkerAction->setCheckable(true);
	fillMarkerAction->setText(tr("fill marker"));
	fillMarkerAction->setIcon(QIcon(":/icons/paintcan.png"));
	fillMarkerAction->setShortcut(Qt::LeftArrow);
	connect(fillMarkerAction, SIGNAL(triggered()), signalMapperMethod, SLOT(map()));
	signalMapperMethod->setMapping(fillMarkerAction, static_cast<int>(Method::Fill));
	actionGroupMethod->addAction(fillMarkerAction);
	toolBar->addAction(fillMarkerAction);


	actionGroupMarker->setExclusive(true);
	actionGroupMethod->setExclusive(true);

	connect(signalMapperMarker, SIGNAL(mapped(int)), this, SLOT(chooseMarkerID(int)));
	connect(signalMapperMethod, SIGNAL(mapped(int)), this, SLOT(chooseMethodID(int)));

	return toolBar;
}


bool BScanIntervallMarker::mouseMoveEvent(QMouseEvent* event, BScanMarkerWidget*)
{
	mouseInWidget = true;
	// if(markerActiv)
	{
		mousePos = event->pos();
		return true;
	}
}

bool BScanIntervallMarker::mousePressEvent(QMouseEvent* event, BScanMarkerWidget*)
{
	if(event->button() == Qt::LeftButton)
	{
		clickPos = event->pos();
		mousePos = event->pos();
		markerActiv = true;
	}
	else
		markerActiv = false;
	return false;
}

bool BScanIntervallMarker::mouseReleaseEvent(QMouseEvent* event, BScanMarkerWidget* widget)
{
	double scaleFactor = widget->getImageScaleFactor();

	switch(getMarkerMethod())
	{
		case Method::Paint:
			if(clickPos.x() != event->x() && markerActiv)
			{
				// std::cout << __FUNCTION__ << ": " << clickPos << " - " << event->x() << std::endl;
				setMarker(clickPos.x()/scaleFactor, event->x()/scaleFactor);
			}
			break;
		case Method::Fill:
			if(markerActiv)
				fillMarker(clickPos.x()/scaleFactor);
			break;
	}
	
	markerActiv = false;
	return true;
}

void BScanIntervallMarker::setMarker(int x1, int x2)
{
	setMarker(x1, x2, aktMarker, getActBScan());
}

void BScanIntervallMarker::setMarker(int x1, int x2, const Marker& type)
{
	setMarker(x1, x2, type     , getActBScan());
}

void BScanIntervallMarker::setMarker(int x1, int x2, const Marker& type, int bscan)
{
	if(bscan<0 || bscan >= static_cast<int>(markers.size()))
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

	markers.at(bscan).set(std::make_pair(boost::icl::discrete_interval<int>::closed(x1, x2), type));
	dataChanged = true;
}


void BScanIntervallMarker::fillMarker(int x, const Marker& type)
{
	int bscan = getActBScan();


	if(bscan<0 || bscan >= static_cast<int>(markers.size()))
		return;

	MarkerMap& map = markers.at(bscan);
	MarkerMap::const_iterator it = map.find(x);

	boost::icl::discrete_interval<int> intervall = it->first;

	if(intervall.upper() > 0)
	{
		markers.at(bscan).set(std::make_pair(boost::icl::discrete_interval<int>::closed(intervall.lower(), intervall.upper()), type));
		dataChanged = true;
	}

}

bool BScanIntervallMarker::keyPressEvent(QKeyEvent* e ,BScanMarkerWidget*)
{
	switch(e->key())
	{
		case Qt::Key_Escape:
			markerActiv = false;
			return true;
			break;
	}
	return false;
}

bool BScanIntervallMarker::leaveWidgetEvent(QEvent* ,BScanMarkerWidget*)
{
	mouseInWidget = false;
	return true;
}

void BScanIntervallMarker::drawMarker(QPainter& painter, BScanMarkerWidget* widget)
{
	double scaleFactor = widget->getImageScaleFactor();
	
	for(const MarkerMap::interval_mapping_type pair : getMarkers())
	{
		IntervallMarker::Marker marker = pair.second;
		if(marker.isDefined())
		{
			boost::icl::discrete_interval<int> itv  = pair.first;
			painter.fillRect(itv.lower()*scaleFactor, 0, (itv.upper()-itv.lower())*scaleFactor, widget->height(), QColor(marker.getRed(), marker.getGreen(), marker.getBlue(),  60));
		}
	}
	

	if(mouseInWidget && getMarkerMethod() == Method::Paint)
	{
		painter.drawLine(mousePos.x(), 0, mousePos.x(), widget->height());

		if(markerActiv)
		{
			const IntervallMarker::Marker& marker = getActMarker();
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

void BScanIntervallMarker::newSeriesLoaded(const OctData::Series* series)
{
	if(!series)
		return;
	
	std::size_t numBscans = series->bscanCount();
	markers.clear();
	markers.resize(numBscans);

	for(std::size_t i = 0; i<numBscans; ++i)
	{
		int bscanWidth = series->getBScan(i)->getWidth();
		markers[i].set(std::make_pair(boost::icl::discrete_interval<int>::closed(0, bscanWidth), IntervallMarker::Marker()));
	}
}
