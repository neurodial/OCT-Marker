#ifndef MARKERMANAGER_H
#define MARKERMANAGER_H

#include <QObject>
#include <vector>

namespace OctData { class Series; }

class BscanMarkerBase;

class BScanMarkerManager : public QObject
{
public:

//	enum class Method { Paint, Fill };
//	enum class Fileformat { XML, Josn };

	BScanMarkerManager();
    virtual ~BScanMarkerManager();

	int getActBScan() const                                     { return actBScan; }

	const OctData::Series* getSeries() const                    { return series;   }




//	bool cscanLoaded() const;
	
	// const QString& getFilename() const                          { return xmlFilename; }
	

//	Method getMarkerMethod() const                              { return markerMethod; }

	BscanMarkerBase* getActMarker()                                 { return actMarker; }
	
	const std::vector<BscanMarkerBase*>& getMarker() const          { return markerObj; }


private:
	int                    actBScan = 0;
	const OctData::Series* series   = nullptr;
	
	std::vector<BscanMarkerBase*> markerObj;
	BscanMarkerBase* actMarker = nullptr;
	
	// Marker           aktMarker;
// 	bool             dataChanged = false;
// 
// 	Method markerMethod = Method::Paint;
// 
// 	std::vector<MarkerMap> markers;

// 	void initMarkerMap();
// 	void saveMarkerDefault();
// 	void loadMarkerDefault();
	
//	QString xmlFilename; // TODO

public slots:
	virtual void chooseBScan(int bscan);
	virtual void inkrementBScan(int inkrement)                  { chooseBScan(actBScan + inkrement); }

	virtual void nextBScan()                                    { inkrementBScan(+1); }
	virtual void previousBScan()                                { inkrementBScan(-1); }

	// virtual void loadImage(QString filename);
	virtual void showSeries(const OctData::Series* series);
	
// 	virtual bool loadMarkers(QString filename, Fileformat format);
// 	virtual bool addMarkers (QString filename, Fileformat format);
// 	virtual void saveMarkers(QString filename, Fileformat format);
	
	virtual void setMarker(int id);

signals:
	void bscanChanged(int bscan);
// 	void newCScanLoaded();
// 
// 	void markerMethodChanged(Method);
	void newSeriesShowed(const OctData::Series* series);
private:
	Q_OBJECT
};

#endif // MARKERMANAGER_H
