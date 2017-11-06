#ifndef OBJECTSMARKER_H
#define OBJECTSMARKER_H

#include "../bscanmarkerbase.h"

#include<vector>
#include<QPoint>
#include<QList>

class QAction;
class QToolBar;
class QActionGroup;
class QWidget;

class ObjectsmarkerScene;
class RectItem;
class QGraphicsItem;


class Objectsmarker : public BscanMarkerBase
{
	Q_OBJECT
public:
// 	typedef std::map<std::string, RectItem*> RectItems;
// 	typedef RectItems::value_type RectItemsTypes;


	Objectsmarker(OctMarkerManager* markerManager);

	virtual bool drawBScan() const                         override { return true;  }

	virtual bool keyPressEvent    (QKeyEvent*  , BScanMarkerWidget*) override;

	virtual void saveState(boost::property_tree::ptree& markerTree)  override;
	virtual void loadState(boost::property_tree::ptree& markerTree)  override;


	virtual       QGraphicsScene* getGraphicsScene()       override;
	virtual const QGraphicsScene* getGraphicsScene() const override;

// 	virtual void drawMarker(QPainter&, BScanMarkerWidget*, const QRect& drawrect) const override;


// 	virtual RedrawRequest mouseMoveEvent   (QMouseEvent* event, BScanMarkerWidget* markerWidget) override;
// 	virtual RedrawRequest mousePressEvent  (QMouseEvent* event, BScanMarkerWidget* markerWidget) override;
// 	virtual RedrawRequest mouseReleaseEvent(QMouseEvent* event, BScanMarkerWidget* markerWidget) override;
// 	virtual void contextMenuEvent (QContextMenuEvent* /*event*/) {}


	virtual void setActBScan(std::size_t bscan)  override;
	virtual void newSeriesLoaded(const OctData::Series* series, boost::property_tree::ptree& markerTree) override;

private:
	void resetMarkerObjects(const OctData::Series* series);
	void removeAllItems();


	ObjectsmarkerScene* graphicsScene = nullptr;


	void removeItems(const QList<QGraphicsItem*>& items);
// 	RectItems rectItems;

	std::vector<std::vector<RectItem*>> itemsList;
	mutable std::size_t actBScanSceneNr = 0;

};

#endif // OBJECTSMARKER_H