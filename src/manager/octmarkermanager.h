#ifndef MARKERMANAGER_H
#define MARKERMANAGER_H

#include <QObject>
#include <vector>

// #include <boost/property_tree/ptree_fwd.hpp>

namespace OctData
{
	class Series;
	class BScan;
}

class BscanMarkerBase;
class SloMarkerBase;

class OctMarkerManager : public QObject
{
public:
	OctMarkerManager();
	virtual ~OctMarkerManager();

	int getActBScan() const                                         { return actBScan; }
	const OctData::Series* getSeries() const                        { return series;   }


	BscanMarkerBase* getActBscanMarker()                            { return actBscanMarker; }
	int getActBscanMarkerId() const                                 { return actBscanMarkerId; }
	const std::vector<BscanMarkerBase*>& getBscanMarker() const     { return bscanMarkerObj; }

	SloMarkerBase* getActSloMarker()                                { return actSloMarker; }
	int getActSloMarkerId() const                                   { return actSloMarkerId; }
	const std::vector<SloMarkerBase*>& getSloMarker() const         { return sloMarkerObj; }

private:
	int                    actBScan = 0;
	const OctData::Series* series   = nullptr;
	
	std::vector<BscanMarkerBase*> bscanMarkerObj;
	BscanMarkerBase* actBscanMarker = nullptr;
	int actBscanMarkerId = -1;

	std::vector<SloMarkerBase*> sloMarkerObj;
	SloMarkerBase* actSloMarker = nullptr;
	int actSloMarkerId = -1;


private slots:
	virtual void saveMarkerStateSlot(const OctData::Series* series);
	virtual void loadMarkerStateSlot(const OctData::Series* series);
	virtual void reloadMarkerStateSlot()                            { loadMarkerStateSlot(series); }

	virtual void udateFromMarkerModul();

public slots:
	virtual void chooseBScan(int bscan);
	virtual void inkrementBScan(int inkrement)                      { chooseBScan(actBScan + inkrement); }

	virtual void nextBScan()                                        { inkrementBScan(+1); }
	virtual void previousBScan()                                    { inkrementBScan(-1); }

	virtual void showSeries(const OctData::Series* series);
	
	virtual void setMarker(int id);

signals:
	void bscanChanged   (int bscan);
	void newSeriesShowed(const OctData::Series* series);
	void newBScanShowed (const OctData::BScan * series);
	void markerChanged  (BscanMarkerBase* marker);

private:
	Q_OBJECT
};

#endif // MARKERMANAGER_H
