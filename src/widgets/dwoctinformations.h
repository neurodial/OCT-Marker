#ifndef DWOCTINFORMATIONS_H
#define DWOCTINFORMATIONS_H

#include<QDockWidget>

namespace OctData
{
	class Patient;
	class Study;
	class Series;
	class BScan;
}

#include <ui_octinformations.h>

class QFormLayout;
class BScanMarkerManager;
class QLabel;


class DwOctInformations : public QDockWidget, Ui::UiOctInformations
{
	Q_OBJECT
public:
	struct OctInfoField
	{
		QLabel* labelDesc = nullptr;
		QLabel* labelInfo = nullptr;
		bool showed = false;
	};


	explicit DwOctInformations(QWidget *parent = nullptr);
	virtual ~DwOctInformations() = default;
	
	void setBScanMarkerManager(BScanMarkerManager* manager);

public slots:
	void setPatient(const OctData::Patient* patient);
	void setStudy  (const OctData::Study  * study  );
	void setSeries (const OctData::Series * series );
	void setBScan  (const OctData::BScan  * bscan );

private:
	QFormLayout* patientInformations = nullptr;
	QFormLayout* studyInformations   = nullptr;
	QFormLayout* seriesInformations  = nullptr;
	QFormLayout* bscanInformations   = nullptr;

	BScanMarkerManager* markerManager = nullptr;


	// BScan informations
	OctInfoField bscanImageQuality;
	OctInfoField bscanNumAverage;
	OctInfoField bscanAcquisitionTime;
};

#endif // DWOCTINFORMATIONS_H
