#ifndef OBJECTSMARKER_H
#define OBJECTSMARKER_H

#include "../bscanmarkerbase.h"

#include <vector>
#include <QPoint>

class QAction;
class QToolBar;
class QActionGroup;
class QWidget;


class Objectsmarker : public BscanMarkerBase
{
public:

	Objectsmarker(OctMarkerManager* markerManager);

	virtual bool drawBScan() const                         override { return true;  }

	virtual RedrawRequest mouseMoveEvent   (QMouseEvent*, BScanMarkerWidget*) override;
	virtual RedrawRequest mousePressEvent  (QMouseEvent*, BScanMarkerWidget*) override;
	virtual RedrawRequest mouseReleaseEvent(QMouseEvent*, BScanMarkerWidget*) override;

	virtual bool toolTipEvent     (QEvent*     , BScanMarkerWidget*) override;
	virtual bool keyPressEvent    (QKeyEvent*  , BScanMarkerWidget*) override;
	virtual bool leaveWidgetEvent (QEvent*     , BScanMarkerWidget*) override;

	virtual void saveState(boost::property_tree::ptree& markerTree)  override;
	virtual void loadState(boost::property_tree::ptree& markerTree)  override;
};

#endif // OBJECTSMARKER_H
