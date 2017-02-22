#include "dwmarkerwidgets.h"

#include <manager/octmarkermanager.h>
#include <markermodules/bscanmarkerbase.h>


DWMarkerWidgets::DWMarkerWidgets(QWidget* parent)
: QDockWidget(parent)
{
	OctMarkerManager* markerManager = &OctMarkerManager::getInstance();
	setWindowTitle(tr("Marker widgets"));
	connect(markerManager, &OctMarkerManager::bscanMarkerChanged, this, &DWMarkerWidgets::markerChanged);

	markerChanged(markerManager->getActBscanMarker());
}

DWMarkerWidgets::~DWMarkerWidgets()
{
	// widgets are owned by the BscanMarker -> removed it from DockWidget scope
	QWidget* oldWidget = widget();
	if(oldWidget)
		oldWidget->setParent(nullptr);
	setWidget(nullptr);
}

void DWMarkerWidgets::markerChanged(BscanMarkerBase* marker)
{
	// widgets are owned by the BscanMarker -> removed it from DockWidget scope
	QWidget* oldWidget = widget();
	if(oldWidget)
		oldWidget->setParent(nullptr);

	if(marker)
	{
		QWidget* widget = marker->getWidget();
		setWidget(widget);
	}
	else
		setWidget(nullptr);
}

