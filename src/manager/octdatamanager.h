#pragma once

#include <QObject>

#include <vector>
#include <string>


#include <boost/property_tree/ptree_fwd.hpp>

#include <globaldefinitons.h>


class QString;
class OctMarkerIO;

namespace OctData
{
	class OCT;
	class Patient;
	class Study;
	class Series;
}


class OctDataManager : public QObject
{
	Q_OBJECT
public:
	
	virtual ~OctDataManager();
	
	static OctDataManager& getInstance()                            { static OctDataManager instance; return instance; }
	
	const QString& getLoadedFilename() const                        { return actFilename; }
	const OctData::Series* getSeries() const                        { return actSeries;   }
	boost::property_tree::ptree* getMarkerTree(const OctData::Series* series)
	                                                                { return getMarkerTreeSeries(series); }
	
	void saveMarkersDefault();

public slots:
	void openFile(const QString& filename);
	
	void chooseSeries(const OctData::Series* seriesReq);
	
	
	virtual bool loadMarkers(QString filename, OctMarkerFileformat format);
	// virtual bool addMarkers (QString filename, Fileformat format);
	virtual void saveMarkers(QString filename, OctMarkerFileformat format);
	


signals:
	void octFileChanged(const OctData::OCT*    );
	void patientChanged(const OctData::Patient*);
	void studyChanged  (const OctData::Study*  );
	void seriesChanged (const OctData::Series* );

	void saveMarkerState(const OctData::Series*);
	void loadMarkerState(const OctData::Series*);

private:
	
	boost::property_tree::ptree* const markerstree = nullptr;
	OctMarkerIO*                 const markerIO    = nullptr;

	OctData::OCT* octData = nullptr;
	QString actFilename;
	
	const OctData::Patient* actPatient = nullptr;
	const OctData::Study*   actStudy   = nullptr;
	const OctData::Series*  actSeries  = nullptr;
	
	
	
	OctDataManager();
	
	boost::property_tree::ptree* getMarkerTreeSeries(const OctData::Series* series);
	boost::property_tree::ptree* getMarkerTreeSeries(const OctData::Patient* pat, const OctData::Study* study, const OctData::Series*  series);
};


