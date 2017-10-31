#include "objectsmarker.h"

Objectsmarker::Objectsmarker(OctMarkerManager* markerManager)
: BscanMarkerBase(markerManager)
{
	name = tr("Objects marker");
	id   = "ObjectsMarker";
	icon = QIcon(":/icons/typicons_mod/object_marker.svg");
}


bool Objectsmarker::keyPressEvent(QKeyEvent*, BScanMarkerWidget*)
{
}

bool Objectsmarker::leaveWidgetEvent(QEvent*, BScanMarkerWidget*)
{
}

void Objectsmarker::loadState(boost::property_tree::ptree& markerTree)
{
}

BscanMarkerBase::RedrawRequest Objectsmarker::mouseMoveEvent(QMouseEvent*, BScanMarkerWidget*)
{
}

BscanMarkerBase::RedrawRequest Objectsmarker::mousePressEvent(QMouseEvent*, BScanMarkerWidget*)
{
}

BscanMarkerBase::RedrawRequest Objectsmarker::mouseReleaseEvent(QMouseEvent*, BScanMarkerWidget*)
{
}

void Objectsmarker::saveState(boost::property_tree::ptree& markerTree)
{
}

bool Objectsmarker::toolTipEvent(QEvent*, BScanMarkerWidget*)
{
}
