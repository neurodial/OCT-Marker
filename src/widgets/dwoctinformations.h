#ifndef DWOCTINFORMATIONS_H
#define DWOCTINFORMATIONS_H

#include<QDockWidget>

namespace OctData
{
	class Patient;
	class Study;
	class Series;
}

#include <ui_octinformations.h>

class QFormLayout;


class DwOctInformations : public QDockWidget, Ui::UiOctInformations
{
	Q_OBJECT
	
	QFormLayout* patientInformations = nullptr;
	QFormLayout* studyInformations   = nullptr;
	QFormLayout* seriesInformations  = nullptr;
	
public:
	explicit DwOctInformations(QWidget *parent = nullptr);
	virtual ~DwOctInformations() = default;
	

public slots:
	void setPatient(const OctData::Patient* patient);
	void setStudy  (const OctData::Study  * study  );
	void setSeries (const OctData::Series * series );
};

#endif // DWOCTINFORMATIONS_H
