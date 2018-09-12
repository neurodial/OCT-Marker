/*
 * Copyright (c) 2018 Kay Gawlik <kaydev@amarunet.de> <kay.gawlik@beuth-hochschule.de> <kay.gawlik@charite.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

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
