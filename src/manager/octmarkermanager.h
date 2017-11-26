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
class ExtraSeriesData;
class ExtraImageData;
class QPaintEvent;

class BScanMarkerWidget;

class OctMarkerManager : public QObject
{
public:
	static OctMarkerManager& getInstance()                          { static OctMarkerManager instance; return instance; }

	int getActBScanNum() const                                      { return actBScan; }
	const OctData::Series* getSeries() const                        { return series;   }
	const OctData::BScan * getActBScan () const;


	BscanMarkerBase* getActBscanMarker()                            { return actBscanMarker; }
	int getActBscanMarkerId() const                                 { return actBscanMarkerId; }
	const std::vector<BscanMarkerBase*>& getBscanMarker() const     { return bscanMarkerObj; }

	SloMarkerBase* getActSloMarker()                                { return actSloMarker; }
	int getActSloMarkerId() const                                   { return actSloMarkerId; }
	const std::vector<SloMarkerBase*>& getSloMarker() const         { return sloMarkerObj; }

	bool hasChangedSinceLastSave() const                            { if(stateChangedSinceLastSave) return true; return hasActMarkerChanged(); }
	void resetChangedSinceLastSaveState()                           { stateChangedSinceLastSave = false; }

	const ExtraImageData* getExtraImageData() const;

// 	void paintMarker(QPaintEvent* event, const BScanMarkerWidget*) const;

private:
	OctMarkerManager();
	virtual ~OctMarkerManager();

	int                    actBScan = 0;
	const OctData::Series* series   = nullptr;
	
	std::vector<BscanMarkerBase*> bscanMarkerObj;
	BscanMarkerBase* actBscanMarker = nullptr;
	int actBscanMarkerId = -1;

	std::vector<SloMarkerBase*> sloMarkerObj;
	SloMarkerBase* actSloMarker = nullptr;
	int actSloMarkerId = -1;
	bool stateChangedSinceLastSave = false;

	bool hasActMarkerChanged() const;

	ExtraSeriesData* extraSeriesData = nullptr;


private slots:
	virtual void saveMarkerStateSlot(const OctData::Series* series);
	virtual void loadMarkerStateSlot(const OctData::Series* series);
	virtual void reloadMarkerStateSlot()                            { loadMarkerStateSlot(series); }

	virtual void udateFromMarkerModul();

	void handleSloRedrawAfterMarkerChange();

public slots:
	virtual void chooseBScan(int bscan);
	virtual void inkrementBScan(int inkrement)                      { chooseBScan(actBScan + inkrement); }

	virtual void nextBScan()                                        { inkrementBScan(+1); }
	virtual void previousBScan()                                    { inkrementBScan(-1); }

	virtual void showSeries(const OctData::Series* series);
	
	virtual void setBscanMarker(int id);
	virtual void setBscanMarkerTextID(QString id);
	virtual void setSloMarker  (int id);

signals:
	void bscanChanged      (int bscan);
	void sloViewChanged    ();
	void newSeriesShowed   (const OctData::Series* series);
	void newBScanShowed    (const OctData::BScan * series);
	void bscanMarkerChanged(BscanMarkerBase* marker);
	void sloMarkerChanged  (SloMarkerBase  * marker);

private:
	Q_OBJECT
};

#endif // MARKERMANAGER_H
