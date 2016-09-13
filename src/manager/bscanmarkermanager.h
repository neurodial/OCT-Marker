#ifndef MARKERMANAGER_H
#define MARKERMANAGER_H

#include <QObject>
#include <boost/icl/interval_map.hpp>
#include <data_structure/intervallmarker.h>

namespace OctData { class Series; class OCT; }

class BScanMarkerManager : public QObject
{
public:
	typedef IntervallMarker::Marker Marker;
	typedef boost::icl::interval_map<int, Marker, boost::icl::partial_enricher> MarkerMap;

	enum class Method { Paint, Fill };
	enum class Fileformat { XML, Josn };

	BScanMarkerManager();
    virtual ~BScanMarkerManager();

	int getActBScan() const                                     { return actBScan; }

	const OctData::Series& getSeries() const                    { return *series;   }

	const MarkerMap& getMarkers() const                         { return markers.at(actBScan); }
	const MarkerMap& getMarkers(int bscan) const                { return markers.at(bscan); }

	void setMarker(int x1, int x2)                              { setMarker(x1, x2, aktMarker, actBScan); }
	void setMarker(int x1, int x2, const Marker& type)          { setMarker(x1, x2, type     , actBScan); }
	void setMarker(int x1, int x2, const Marker& type, int bscan);

	void fillMarker(int x)                                      { fillMarker(x, aktMarker); }
	void fillMarker(int x, const Marker& type);

	bool cscanLoaded() const;
	
	const QString& getFilename() const                          { return xmlFilename; }
	
	const Marker& getActMarker() const                          { return aktMarker; }

	Method getMarkerMethod() const                              { return markerMethod; }


private:
	int              actBScan = 0;
	OctData::Series* series = nullptr;
	OctData::OCT*    oct    = nullptr;
	Marker           aktMarker;
	bool             dataChanged = false;

	Method markerMethod = Method::Paint;

	std::vector<MarkerMap> markers;

	void initMarkerMap();
	void saveMarkerDefault();
	void loadMarkerDefault();
	
	QString xmlFilename;

public slots:
	virtual void chooseBScan(int bscan);
	virtual void inkrementBScan(int inkrement)                  { chooseBScan(actBScan + inkrement); }

	virtual void nextBScan()                                    { inkrementBScan(+1); }
	virtual void previousBScan()                                { inkrementBScan(-1); }

	virtual void chooseMarkerID(int id)                         { aktMarker = IntervallMarker::getInstance().getMarkerFromID(id); }
	virtual void chooseMethodID(int id)                         { markerMethod = static_cast<Method>(id); emit(markerMethodChanged(markerMethod)); }

	virtual void loadImage(QString filename);
	
	virtual bool loadMarkers(QString filename, Fileformat format);
	virtual bool addMarkers (QString filename, Fileformat format);
	virtual void saveMarkers(QString filename, Fileformat format);

signals:
	void bscanChanged(int bscan);
	void newCScanLoaded();

	void markerMethodChanged(Method);
private:
	Q_OBJECT
};

#endif // MARKERMANAGER_H
