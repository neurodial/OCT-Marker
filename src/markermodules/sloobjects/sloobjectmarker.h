#ifndef SLOOBJECTMARKER_H
#define SLOOBJECTMARKER_H

#include "../slomarkerbase.h"

class SloObjectMarker : public SloMarkerBase
{
	Q_OBJECT
public:

	SloObjectMarker(OctMarkerManager* markerManager);


	QGraphicsScene* getGraphicsScene()                 override     { return graphicsScene; }


	virtual void saveState(boost::property_tree::ptree& markerTree)  override;
	virtual void loadState(boost::property_tree::ptree& markerTree)  override;

private:
	QGraphicsScene* graphicsScene = nullptr;
};

#endif // SLOOBJECTMARKER_H
