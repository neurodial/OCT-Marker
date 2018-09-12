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

#pragma once

#include <QObject>

#include <QThread>

#include <vector>
#include <string>


#include <boost/property_tree/ptree_fwd.hpp>

#include <globaldefinitions.h>

#include <oct_cpp_framework/callback.h>


class QString;
class OctMarkerIO;
class SloBScanDistanceMap;

namespace OctData
{
	class OCT;
	class Patient;
	class Study;
	class Series;
}

class OctDataManagerThread;

class OctDataManager : public QObject
{
	friend class OctDataManagerThread;

	Q_OBJECT
public:
	
	virtual ~OctDataManager();
	
	static OctDataManager& getInstance()                            { static OctDataManager instance; return instance; }
	
	const QString& getLoadedFilename() const                        { return actFilename; }
	const OctData::Patient* getPatient() const                      { return actPatient ; }
	const OctData::Study  * getStudy  () const                      { return actStudy   ; }
	const OctData::Series * getSeries () const                      { return actSeries  ; }
	boost::property_tree::ptree* getMarkerTree(const OctData::Series* series)
	                                                                { return getMarkerTreeSeries(series); }
	
	void saveMarkersDefault();
	bool checkAndAskSaveBeforContinue();

private slots:
	void loadOctDataThreadProgress(double frac)                     { emit(loadFileProgress(frac)); }
	void loadOctDataThreadFinish();
	void clearSeriesCache();

public slots:
	void openFile(const QString& filename);
	
	void chooseSeries(const OctData::Series* seriesReq);


	const SloBScanDistanceMap* getSeriesSLODistanceMap() const;
	
	
	virtual bool loadMarkers(QString filename, OctMarkerFileformat format);
	virtual void saveMarkers(QString filename, OctMarkerFileformat format);
	
	void triggerSaveMarkersDefault();

	void saveOctScan(QString filename);

	void abortLoadingOctFile();

signals:
	void octFileChanged();
	void octFileChanged(QString filename);
	void octFileChanged(const OctData::OCT*    );
	void patientChanged(const OctData::Patient*);
	void studyChanged  (const OctData::Study*  );
	void seriesChanged (const OctData::Series* );

	void saveMarkerState(const OctData::Series*);
	void loadMarkerState(const OctData::Series*);

	void loadMarkerStateAll();

	void loadFileSignal(bool loading);
	void loadFileProgress(double frac);


private:
	
	boost::property_tree::ptree* const markerstree = nullptr;
	OctMarkerIO*                 const markerIO    = nullptr;

	OctData::OCT* octData         = nullptr;
	OctData::OCT* octData4Loading = nullptr; // is nullptr when no file is loading by task
	QString actFilename;
	
	const OctData::Patient* actPatient = nullptr;
	const OctData::Study*   actStudy   = nullptr;
	const OctData::Series*  actSeries  = nullptr;

	mutable SloBScanDistanceMap* seriesSLODistanceMap = nullptr;
	
	OctDataManagerThread* loadThread = nullptr;
	
	OctDataManager();
	
	boost::property_tree::ptree* getMarkerTreeSeries(const OctData::Series* series);
	boost::property_tree::ptree* getMarkerTreeSeries(const OctData::Patient* pat, const OctData::Study* study, const OctData::Series*  series);
};


class OctDataManagerThread : public QThread, public CppFW::Callback
{
	Q_OBJECT

	OctDataManager& octDataManager;

	bool breakLoading = false;
	bool loadSuccess  = true;
	bool loadError    = false;

	OctData::OCT* oct = nullptr;

	const QString filename;
	QString  error;

public:
	OctDataManagerThread(OctDataManager& dataManager, const QString& filename, OctData::OCT* oct) : octDataManager(dataManager), oct(oct), filename(filename) {}

	void breakLoad()                                                { breakLoading = true; }

	bool success()                                           const  { return loadSuccess; }
	const QString& getError()                                const  { return error; }
	const QString& getFilename()                             const  { return filename; }
	bool hasLoadError()                                      const  { return loadError; }

protected:
	void run();

	virtual bool callback(double frac) override
	{
		emit(stepCalulated(frac));
		return !breakLoading;
	}
signals:
	void stepCalulated(double);
};

