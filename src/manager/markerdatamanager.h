#ifndef MARKERDATAMANAGER_H
#define MARKERDATAMANAGER_H

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


class MarkerDataManager : public QObject
{
	Q_OBJECT
	
	OctData::OCT* octData = nullptr;
	QString actFilename;
	
	const OctData::Patient* actPatient = nullptr;
	const OctData::Study*   actStudy   = nullptr;
	const OctData::Series*  actSeries  = nullptr;
	
	MarkerDataManager() = default;
public:
	virtual ~MarkerDataManager();
	
	static MarkerDataManager& getInstance()                         { static MarkerDataManager instance; return instance; }


public slots:
	void openFile(const QString& filename);
	
	void chooseSeries  (const OctData::Series* seriesReq);

signals:
	void octFileChanged(const OctData::OCT*    );
	void patientChanged(const OctData::Patient*);
	void studyChanged  (const OctData::Study*  );
	void seriesChanged (const OctData::Series* );

};

#endif // MARKERDATAMANAGER_H
