#pragma once

#include <QObject>

#include <vector>
#include <string>


class QString;

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
	
	OctData::OCT* octData = nullptr;
	QString actFilename;
	
	const OctData::Patient* actPatient = nullptr;
	const OctData::Study*   actStudy   = nullptr;
	const OctData::Series*  actSeries  = nullptr;
	
	OctDataManager();
public:
	enum class Fileformat { XML, Josn };
	
	virtual ~OctDataManager();
	
	static OctDataManager& getInstance()                            { static OctDataManager instance; return instance; }
	
	const QString& getLoadedFilename() const                        { return actFilename; }

	const OctData::Series* getSeries() const                        { return actSeries;   }

public slots:
	void openFile(const QString& filename);
	
	void chooseSeries  (const OctData::Series* seriesReq);
	
	
	virtual bool loadMarkers(QString filename, Fileformat format);
	virtual bool addMarkers (QString filename, Fileformat format);
	virtual void saveMarkers(QString filename, Fileformat format);
	

signals:
	void octFileChanged(const OctData::OCT*    );
	void patientChanged(const OctData::Patient*);
	void studyChanged  (const OctData::Study*  );
	void seriesChanged (const OctData::Series* );

};


