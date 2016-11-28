#ifndef SLOOBJECTMARKER_H
#define SLOOBJECTMARKER_H

#include "../slomarkerbase.h"

#include <map>
#include <string>

class RectItem;

class SloObjectMarker : public SloMarkerBase
{
	Q_OBJECT
public:
	typedef std::map<std::string, RectItem*> RectItems;
	typedef RectItems::value_type RectItemsTypes;

	explicit SloObjectMarker(OctMarkerManager* markerManager);
	virtual ~SloObjectMarker();


	              QGraphicsScene* getGraphicsScene()       override { return graphicsScene; }
	virtual const QGraphicsScene* getGraphicsScene() const override { return graphicsScene; }

	virtual void newSeriesLoaded(const OctData::Series*, boost::property_tree::ptree& ptree) override
	                                                                { loadState(ptree); }


	virtual void saveState(boost::property_tree::ptree& markerTree)  override;
	virtual void loadState(boost::property_tree::ptree& markerTree)  override;

	      RectItem* getRectItem(const std::string& id);
	const RectItem* getRectItem(const std::string& id) const;

	const RectItems& getRectItems() const                           { return rectItems; }

private:
	QGraphicsScene* graphicsScene = nullptr;
	// RectItem*       sloMarker     = nullptr;

	RectItems rectItems;


	void resetScene();
};

#endif // SLOOBJECTMARKER_H
