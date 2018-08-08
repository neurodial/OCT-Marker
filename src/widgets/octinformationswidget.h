#ifndef DWOCTINFORMATIONS_H
#define DWOCTINFORMATIONS_H

#include<QWidget>

namespace OctData
{
	class Patient;
	class Study;
	class Series;
	class BScan;
}

#include <ui_octinformations.h>

class QFormLayout;
class OctMarkerManager;
class QLabel;


class OctInformationsWidget : public QWidget, Ui::UiOctInformations
{
	Q_OBJECT
public:
	struct OctInfoField
	{
		QLabel* labelDesc = nullptr;
		QLabel* labelInfo = nullptr;
		bool showed = false;
	};


	explicit OctInformationsWidget(QWidget *parent = nullptr);
	virtual ~OctInformationsWidget() = default;

private slots:
	void setOctFile(QString filename);
	void setPatient(const OctData::Patient* patient);
	void setStudy  (const OctData::Study  * study  );
	void setSeries (const OctData::Series * series );
	void setBScan  (const OctData::BScan  * bscan );

private:
	QFormLayout* octfileInformations = nullptr;
	QFormLayout* patientInformations = nullptr;
	QFormLayout* studyInformations   = nullptr;
	QFormLayout* seriesInformations  = nullptr;
	QFormLayout* bscanInformations   = nullptr;

	// BScan informations
	OctInfoField bscanImageQuality;
	OctInfoField bscanNumAverage;
	OctInfoField bscanAcquisitionTime;

	OctInfoField scaleFactorX;
	OctInfoField scaleFactorY;
	OctInfoField scaleFactorZ;

	OctInfoField bscanAngle;

	OctInfoField bscanWidthPx;
	OctInfoField bscanHeightPx;
};

#endif // DWOCTINFORMATIONS_H
