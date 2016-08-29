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
//	std::vector<std::string> filenames;

	OctData::OCT* octData = nullptr;
	QString actFilename;
public:
	~MarkerDataManager();


public slots:
	void openFile(const QString& filename);

signals:
	void octFileChanged(OctData::OCT*    );
	void patientChanged(OctData::Patient*);
	void studyChanged  (OctData::Study*  );
	void seriesChanged (OctData::Series* );

};

#endif // MARKERDATAMANAGER_H
