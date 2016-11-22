#include "dwmarkerwidgets.h"

#include <manager/octmarkermanager.h>
#include <markermodules/bscanmarkerbase.h>


DWMarkerWidgets::DWMarkerWidgets(OctMarkerManager* markerManager, QWidget* parent)
: QDockWidget(parent)
, markerManager(markerManager)
{
	setWindowTitle(tr("Marker widgets"));
	connect(markerManager, &OctMarkerManager::markerChanged, this, &DWMarkerWidgets::markerChanged);

	markerChanged(markerManager->getActMarker());
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

