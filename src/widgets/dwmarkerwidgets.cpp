#include "dwmarkerwidgets.h"

#include <manager/octmarkermanager.h>
#include <markermodules/bscanmarkerbase.h>


DWMarkerWidgets::DWMarkerWidgets(OctMarkerManager* markerManager, QWidget* parent)
: QDockWidget(parent)
, markerManager(markerManager)
{
	setWindowTitle(tr("Marker widgets"));
	connect(markerManager, &OctMarkerManager::bscanMarkerChanged, this, &DWMarkerWidgets::markerChanged);

	markerChanged(markerManager->getActBscanMarker());
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

