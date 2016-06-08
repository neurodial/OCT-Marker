#ifndef MARKERMANAGER_H
#define MARKERMANAGER_H

#include <QObject>
#include <boost/icl/interval_map.hpp>

class CScan;

class MarkerManager : public QObject
{
	Q_OBJECT
public:
	typedef boost::icl::interval_map<int, int, boost::icl::partial_enricher> MarkerMap;

	enum class Method { Paint, Fill };

	MarkerManager();
    virtual ~MarkerManager();

	int getActBScan() const                                     { return actBScan; }

	const CScan& getCScan()                                     { return *cscan;   }

	const MarkerMap& getMarkers() const                         { return markers.at(actBScan); }
	const MarkerMap& getMarkers(int bscan) const                { return markers.at(bscan); }
	void setMarker(int x1, int x2, int type = -2)               { setMarker(x1, x2, type, actBScan); }
	void setMarker(int x1, int x2, int type, int bscan);

	void fillMarker(int x, int type = -2);

	bool cscanLoaded() const;
	
	const QString& getFilename() const                          { return xmlFilename; }
	
	int getActMarkerId() const                                  { return markerId; }

	Method getMarkerMethod() const                              { return markerMethod; }


private:
	int actBScan = 0;
	CScan* cscan = nullptr;
	int markerId = 0;

	Method markerMethod = Method::Paint;

	std::vector<MarkerMap> markers;

	void initMarkerMap();
	
	QString xmlFilename;

public slots:
	virtual void chooseBScan(int bscan);
	virtual void inkrementBScan(int inkrement)                  { chooseBScan(actBScan + inkrement); }

	virtual void nextBScan()                                    { inkrementBScan(+1); }
	virtual void previousBScan()                                { inkrementBScan(-1); }

	virtual void chooseMarkerID(int id)                         { markerId = id; }
	virtual void chooseMethodID(int id)                         { markerMethod = static_cast<Method>(id); emit(markerMethodChanged(markerMethod)); }

	virtual void loadOCTXml(QString filename);
	
	virtual void loadMarkersXml(QString filename);
	virtual void saveMarkersXml(QString filename);

signals:
	void bscanChanged(int bscan);
	void newCScanLoaded();

	void markerMethodChanged(Method);
};

#endif // MARKERMANAGER_H
