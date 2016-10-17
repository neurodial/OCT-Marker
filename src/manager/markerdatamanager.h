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
	
	MarkerDataManager() = default;
public:
	~MarkerDataManager();
	
	static MarkerDataManager& getInstance()                         { static MarkerDataManager instance; return instance; }


public slots:
	void openFile(const QString& filename);
	
	void chooseSeries  (OctData::Series* );

signals:
	void octFileChanged(OctData::OCT*    );
	void patientChanged(OctData::Patient*);
	void studyChanged  (OctData::Study*  );
	void seriesChanged (OctData::Series* );

};

#endif // MARKERDATAMANAGER_H
