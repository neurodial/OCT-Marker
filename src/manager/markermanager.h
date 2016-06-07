#ifndef MARKERMANAGER_H
#define MARKERMANAGER_H

#include <QObject>
#include <boost/icl/interval_map.hpp>

class CScan;

class MarkerManager : public QObject
{
	Q_OBJECT
public:
	typedef boost::icl::interval_map<int, int> MarkerMap;

	MarkerManager();
    virtual ~MarkerManager();

	int getActBScan() const                                     { return actBScan; }

	const CScan& getCScan()                                     { return *cscan;   }

	const MarkerMap& getMarkers() const                         { return markers.at(actBScan); }
	void setMarker(int x1, int x2, int type = -2);

	bool cscanLoaded() const;
	
	const QString& getFilename() const                          { return xmlFilename; }

	int markerId = -1;

private:
	int actBScan = 0;
	CScan* cscan = nullptr;

	std::vector<MarkerMap> markers;
	
	QString xmlFilename;

public slots:
	virtual void chooseBScan(int bscan);
	virtual void inkrementBScan(int inkrement)                  { chooseBScan(actBScan + inkrement); }

	virtual void nextBScan()                                    { inkrementBScan(+1); }
	virtual void previousBScan()                                { inkrementBScan(-1); }

	virtual void chooseMarkerID(int id)                         { markerId = id; }

	virtual void loadOCTXml(QString filename);

signals:
	void bscanChanged(int bscan);
	void newCScanLoaded();
};

#endif // MARKERMANAGER_H
