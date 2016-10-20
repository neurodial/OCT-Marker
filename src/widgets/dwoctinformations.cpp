#include "dwoctinformations.h"

#include <octdata/datastruct/series.h>
#include <octdata/datastruct/bscan.h>
#include <octdata/datastruct/oct.h>

#include <manager/octdatamanager.h>

#include <QFormLayout>
#include <QLabel>


namespace
{
	void clearQLayout(QLayout* layout)
	{
		QLayoutItem* item;
		QLayout* sublayout;
		QWidget* widget;
		if(layout)
		{
			while(layout->count() > 0)
			{
				item = layout->takeAt(0);
				if((sublayout = item->layout()) != 0)
					clearQLayout(sublayout);
				else if((widget = item->widget()) != 0)
					{widget->hide(); delete widget;}
				else
					delete item;
			}
		}
	}
	
	void addInformation(QFormLayout* formlayout, const QString& labelText, const std::string& information)
	{
		if(!information.empty())
		{
			QLabel* label = new QLabel(QString::fromStdString(information));
			label->setTextInteractionFlags(Qt::TextSelectableByMouse);
			formlayout->addRow(labelText, label);
		}
	}
	void addInformation(QFormLayout* formlayout, const QString& labelText, const QString& information)
	{
		if(!information.isEmpty())
		{
			QLabel* label = new QLabel(information);
			label->setTextInteractionFlags(Qt::TextSelectableByMouse);
			formlayout->addRow(labelText, label);
		}
	}
}


DwOctInformations::DwOctInformations::DwOctInformations(QWidget* parent)
:QDockWidget(parent)
{
	setupUi(this);
	
	OctDataManager& dataManager = OctDataManager::getInstance();
	
	patientInformations = new QFormLayout;
	studyInformations   = new QFormLayout;
	seriesInformations  = new QFormLayout;
	
	groupBoxPatient->setLayout(patientInformations);
	groupBoxStudy  ->setLayout(studyInformations  );
	groupBoxSeries ->setLayout(seriesInformations );
		
	connect(&dataManager, &OctDataManager::patientChanged, this, &DwOctInformations::setPatient);
	connect(&dataManager, &OctDataManager::studyChanged  , this, &DwOctInformations::setStudy  );
	connect(&dataManager, &OctDataManager::seriesChanged , this, &DwOctInformations::setSeries );
}




void DwOctInformations::setPatient(const OctData::Patient* patient)
{
	clearQLayout(patientInformations);
	
	if(!patient)
		return;
	
	
	addInformation(patientInformations, tr("Surname"  ), patient->getSurname ());
	addInformation(patientInformations, tr("Forename" ), patient->getForename());
	addInformation(patientInformations, tr("Title"    ), patient->getTitle()   );
	addInformation(patientInformations, tr("ID"       ), patient->getId()      );
	
	if(!patient->getBirthdate().isEmpty())
		addInformation(patientInformations, tr("Birthdate"), patient->getBirthdate().str());
	
	QString sex;
	switch(patient->getSex())
	{
		case OctData::Patient::Sex::Female:
			sex = tr("Female");
			break;
		case OctData::Patient::Sex::Male:
			sex = tr("Male");
			break;
		case OctData::Patient::Sex::Unknown:
			break;
	}
	addInformation(patientInformations, tr("Sex"      ), sex);
	addInformation(patientInformations, tr("UID"      ), patient->getPatientUID());
}


void DwOctInformations::setStudy(const OctData::Study* study)
{
	clearQLayout(studyInformations);
	
	if(!study)
		return;
	
	addInformation(studyInformations, tr("Operator"), study->getStudyOperator());
	addInformation(studyInformations, tr("UID"     ), study->getStudyUID()     );
	if(!study->getStudyDate().isEmpty())
		addInformation(studyInformations, tr("Study date"), study->getStudyDate().str());
	
}


void DwOctInformations::setSeries(const OctData::Series* series)
{
	clearQLayout(seriesInformations);
	
	if(!series)
		return;
	
	addInformation(seriesInformations, tr("UID"     ), series->getSeriesUID()     );
	addInformation(seriesInformations, tr("Ref. UID"), series->getRefSeriesUID()  );
	
	QString scanPos;
	switch(series->getLaterality())
	{
		case OctData::Series::Laterality::OS:
			scanPos = tr("OS");
			break;
		case OctData::Series::Laterality::OD:
			scanPos = tr("OD");
			break;
		case OctData::Series::Laterality::undef:
			break;
	}
	addInformation(seriesInformations, tr("Laterality"     ), scanPos     );
	
}
