#include "dwmarkerwidgets.h"

#include <manager/bscanmarkermanager.h>
#include <markermodules/bscanmarkerbase.h>


DWMarkerWidgets::DWMarkerWidgets(BScanMarkerManager* markerManager, QWidget* parent)
: QDockWidget(parent)
, markerManager(markerManager)
{
	setWindowTitle(tr("Marker widgets"));
	connect(markerManager, &BScanMarkerManager::markerChanged, this, &DWMarkerWidgets::markerChanged);
}

DWMarkerWidgets::~DWMarkerWidgets()
{
}

void DWMarkerWidgets::markerChanged(BscanMarkerBase* marker)
{
	if(marker)
	{
		QWidget* widget = marker->getWidget();
		setWidget(widget);
	}
	else
		setWidget(nullptr);
}

