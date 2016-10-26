#include "bscanintervalmarker.h"


#include <QIcon>
#include <QColor>
#include <QPixmap>

#include <QToolBar>
#include <QActionGroup>

#include <QPainter>

#include <QAction>

#include <QMouseEvent>
#include <widgets/bscanmarkerwidget.h>

#include <manager/bscanmarkermanager.h>
#include <helper/actionclasses.h>

#include <octdata/datastruct/series.h>
#include <octdata/datastruct/bscan.h>

#include "bscanintervalptree.h"

namespace
{
	QIcon createColorIcon(const QColor& color)
	{
		QPixmap pixmap(15, 15);
		pixmap.fill(color);
		return QIcon(pixmap);
	}
}


BScanIntervalMarker::BScanIntervalMarker(BScanMarkerManager* markerManager)
: BscanMarkerBase(markerManager)
{
	name = tr("Interval marker");
	id   = "IntervalMarker";
	icon = QIcon(":/icons/intervall_edit.png");
	
	// connect(markerManager, &BScanMarkerManager::newSeriesShowed, this, &BScanIntervalMarker::newSeriesLoaded);
}



QToolBar* BScanIntervalMarker::createToolbar(QObject* parent)
{
	QActionGroup*  actionGroupMarker  = new QActionGroup (parent);
	QActionGroup*  actionGroupMethod  = new QActionGroup (parent);
	QToolBar*      toolBar            = new QToolBar(tr("Intervall marker"));

	std::size_t actMarkerId = actMarker.getInternalId();
	toolBar->setObjectName("ToolBarIntervalMarker");

	const IntervalMarker& intervalMarker = IntervalMarker::getInstance();

	markersActions.resize(IntervalMarker::Marker::getMaxInternalId() + 1); // +1 for undefined marker


	for(const IntervalMarker::Marker& marker : intervalMarker.getIntervalMarkerList())
	{
		std::size_t markerId = marker.getInternalId();
		QIcon icon = createColorIcon(QColor::fromRgb(marker.getRed(), marker.getGreen(), marker.getBlue()));

		IntValueAction* markerAction = new IntValueAction(markerId, parent);
		// markerAction->setCheckable(true);
		markerAction->setText(QString::fromStdString(marker.getName()));
		markerAction->setIcon(icon);
		markerAction->setChecked(actMarkerId == markerId);
		connect(markerAction, &IntValueAction::triggered            , this        , &BScanIntervalMarker::chooseMarkerID);
		connect(this        , &BScanIntervalMarker::markerIdChanged, markerAction, &IntValueAction::valueChanged        );

		actionGroupMarker->addAction(markerAction);
		toolBar->addAction(markerAction);

		markersActions.at(markerId) = markerAction;
	}

	toolBar->addSeparator();


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


bool BScanIntervalMarker::mouseMoveEvent(QMouseEvent* event, BScanMarkerWidget*)
{
	mouseInWidget = true;
	// if(markerActiv)
	{
		mousePos = event->pos();
		return true;
	}
}

bool BScanIntervalMarker::mousePressEvent(QMouseEvent* event, BScanMarkerWidget*)
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

bool BScanIntervalMarker::mouseReleaseEvent(QMouseEvent* event, BScanMarkerWidget* widget)
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

void BScanIntervalMarker::setMarker(int x1, int x2)
{
	setMarker(x1, x2, actMarker, getActBScan());
}

void BScanIntervalMarker::setMarker(int x1, int x2, const Marker& type)
{
	setMarker(x1, x2, type     , getActBScan());
}

void BScanIntervalMarker::setMarker(int x1, int x2, const Marker& type, int bscan)
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


void BScanIntervalMarker::fillMarker(int x, const Marker& type)
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

void BScanIntervalMarker::drawMarker(QPainter& painter, BScanMarkerWidget* widget)
{
	double scaleFactor = widget->getImageScaleFactor();
	
	for(const MarkerMap::interval_mapping_type pair : getMarkers())
	{
		IntervalMarker::Marker marker = pair.second;
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

void BScanIntervalMarker::newSeriesLoaded(const OctData::Series* series, boost::property_tree::ptree& markerTree)
{
	if(!series)
		return;
	
	std::size_t numBscans = series->bscanCount();
	markers.clear();
	markers.resize(numBscans);

	for(std::size_t i = 0; i<numBscans; ++i)
	{
		int bscanWidth = series->getBScan(i)->getWidth();
		markers[i].set(std::make_pair(boost::icl::discrete_interval<int>::closed(0, bscanWidth), IntervalMarker::Marker()));
	}
	
	BScanIntervalPTree::parsePTree(markerTree, this);
}


void BScanIntervalMarker::saveState(boost::property_tree::ptree& markerTree)
{
	BScanIntervalPTree::fillPTree(markerTree, this);
}


void BScanIntervalMarker::loadState(boost::property_tree::ptree& markerTree)
{
	std::size_t numBscans = markers.size();
	markers.clear();
	markers.resize(numBscans);

	BScanIntervalPTree::parsePTree(markerTree, this);
}

