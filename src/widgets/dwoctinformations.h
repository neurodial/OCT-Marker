#ifndef DWOCTINFORMATIONS_H
#define DWOCTINFORMATIONS_H

#include<QDockWidget>

namespace OctData
{
	class Patient;
	class Study;
	class Series;
}

class DwOctInformations : public QDockWidget
{
	Q_OBJECT

public slots:
	void setPatient(OctData::Patient* patient);
	void setStudy  (OctData::Study  * study  );
	void setSeries (OctData::Series * series );
};

#endif // DWOCTINFORMATIONS_H
