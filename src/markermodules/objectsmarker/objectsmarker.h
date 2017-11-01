#ifndef OBJECTSMARKER_H
#define OBJECTSMARKER_H

#include "../bscanmarkerbase.h"

#include <vector>
#include <QPoint>

class QAction;
class QToolBar;
class QActionGroup;
class QWidget;

class QGraphicsScene;
class RectItem;


class Objectsmarker : public BscanMarkerBase
{
	Q_OBJECT
public:
	typedef std::map<std::string, RectItem*> RectItems;
	typedef RectItems::value_type RectItemsTypes;


	Objectsmarker(OctMarkerManager* markerManager);

	virtual bool drawBScan() const                         override { return true;  }

	virtual bool keyPressEvent    (QKeyEvent*  , BScanMarkerWidget*) override;

	virtual void saveState(boost::property_tree::ptree& markerTree)  override;
	virtual void loadState(boost::property_tree::ptree& markerTree)  override;


	virtual       QGraphicsScene* getGraphicsScene()       override { return graphicsScene; }
	virtual const QGraphicsScene* getGraphicsScene() const override { return graphicsScene; }


private:
	QGraphicsScene* graphicsScene = nullptr;

	RectItems rectItems;
};

#endif // OBJECTSMARKER_H
