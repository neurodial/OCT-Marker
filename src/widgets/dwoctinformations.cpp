#include "dwoctinformations.h"

#include <octdata/datastruct/series.h>
#include <octdata/datastruct/bscan.h>
#include <octdata/datastruct/oct.h>

#include <manager/markerdatamanager.h>

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
			formlayout->addRow(labelText, new QLabel(QString::fromStdString(information)));
		}
	}
	void addInformation(QFormLayout* formlayout, const QString& labelText, const QString& information)
	{
		if(!information.isEmpty())
		{
			formlayout->addRow(labelText, new QLabel(information));
		}
	}
}


DwOctInformations::DwOctInformations::DwOctInformations(QWidget* parent)
:QDockWidget(parent)
{
	setupUi(this);
	
	MarkerDataManager& dataManager = MarkerDataManager::getInstance();
	
	patientInformations = new QFormLayout;
	studyInformations   = new QFormLayout;
	seriesInformations  = new QFormLayout;
	
	groupBoxPatient->setLayout(patientInformations);
	groupBoxStudy  ->setLayout(studyInformations  );
	groupBoxSeries ->setLayout(seriesInformations );
		
	connect(&dataManager, &MarkerDataManager::patientChanged, this, &DwOctInformations::setPatient);
	connect(&dataManager, &MarkerDataManager::studyChanged  , this, &DwOctInformations::setStudy  );
	connect(&dataManager, &MarkerDataManager::seriesChanged , this, &DwOctInformations::setSeries );
}




void DwOctInformations::setPatient(const OctData::Patient* patient)
{
	clearQLayout(patientInformations);
	
	if(!patient)
		return;
	
	
	addInformation(patientInformations, tr("Surname"  ), patient->getSurname ());
	addInformation(patientInformations, tr("Forename" ), patient->getForename());
	addInformation(patientInformations, tr("Title"    ), patient->getTitle());
	addInformation(patientInformations, tr("ID"       ), patient->getId());
	
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
	addInformation(patientInformations, tr("Sex      "), sex);
}


void DwOctInformations::setStudy(const OctData::Study* study)
{

}


void DwOctInformations::setSeries(const OctData::Series* series)
{

}
