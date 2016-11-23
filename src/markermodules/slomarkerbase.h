#ifndef SLOMARKERBASE_H
#define SLOMARKERBASE_H

#include <QObject>
#include <QIcon>

#include <boost/property_tree/ptree_fwd.hpp>


class OctMarkerManager;

class QToolBar;
class QGraphicsScene;

namespace OctData
{
	class Series;
	class CoordSLOpx;
};

class SloMarkerBase : public QObject
{
	Q_OBJECT
public:
	class RedrawRequest
	{
	public:
		bool redraw = false;
		QRect rect;
	};


	SloMarkerBase(OctMarkerManager* markerManager) : markerManager(markerManager) {}
	virtual ~SloMarkerBase()                                        {}

	virtual QGraphicsScene* getGraphicsScene()                      { return nullptr; }


	virtual void saveState(boost::property_tree::ptree&)            {}
	virtual void loadState(boost::property_tree::ptree&)            {}

	virtual void newSeriesLoaded(const OctData::Series*, boost::property_tree::ptree&)
	                                                                {}


	virtual const QString& getName()                                { return name; }
	virtual const QString& getMarkerId()                            { return id;   }
	virtual const QIcon&   getIcon()                                { return icon; }

protected:
	OctMarkerManager* const markerManager;

	void connectToolBar(QToolBar* toolbar);

	const OctData::Series* getSeries() const;

	QString name;
	QString id;
	QIcon  icon;
	bool isActivated  = false;
	bool markerActive = true;

};


#endif // SLOMARKERBASE_H
