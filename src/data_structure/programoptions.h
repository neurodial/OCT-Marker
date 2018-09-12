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

#ifndef PROGRAMOPTIONS_H
#define PROGRAMOPTIONS_H

#include <vector>

#include<QObject>

#include <helper/programoptionstype.h>

class ProgramOptions;
class QSettings;


class ProgramOptions : public QObject
{
	Q_OBJECT

	ProgramOptions();
	~ProgramOptions();

	static ProgramOptions& getInstance() { static ProgramOptions instance; return instance; }

	QSettings* settings = nullptr;
	QAction*   resetAction;
	bool saveOptions = true;

	struct ConfigList
	{
		std::map<QString, std::vector<Option*>> sortedConfig;
		std::vector<Option*>                    allConfig;
	};

public:
	friend class Option;

	static OptionBool   bscansShowSegmentationslines;
	static OptionColor  bscanSegmetationLineColor;
	static OptionInt    bscanSegmetationLineThicknes;
	static OptionInt    bscanAspectRatioType;
	static OptionBool   bscanAutoFitImage;

	static OptionBool   bscanShowExtraSegmentationslines;

	static OptionBool   fillEmptyPixelWhite;
	static OptionBool   registerBScans;
	static OptionBool   loadRotateSlo;

	static OptionBool   saveOctBinFlat;


	static OptionBool   holdOCTRawData;
	static OptionBool   readBScans;

	static OptionInt    e2eGrayTransform;

	static OptionBool   sloShowLabels;
	static OptionBool   sloShowGrid;
	static OptionBool   sloShowBScanMousePos;
	static OptionInt    sloShowsBScansPos;
	static OptionBool   sloShowOverlay ;
	static OptionDouble sloOverlayAlpha;
	static OptionBool   sloClipScanArea;
	
	static OptionString octDirectory;
	static OptionString loadOctdataAtStart;
	
	static OptionBool   autoSaveOctMarkers;
	static OptionInt    defaultFileformatOctMarkers;

	static OptionInt    bscanMarkerToolId;
	static OptionInt    sloMarkerToolId;

	
	static OptionDouble layerSegFindPointInsertTol  ;
	static OptionDouble layerSegFindPointRemoveTol  ;
	static OptionDouble layerSegFindPointMaxAbsError;
	static OptionInt    layerSegFindPointMaxPoints  ;

	static OptionColor  layerSegActiveLineColor;
	static OptionColor  layerSegPassivLineColor;
	static OptionInt    layerSegActiveLineSize;
	static OptionInt    layerSegPassivLineSize;
	static OptionInt    layerSegSplinePointSize;

	static OptionBool   layerSegThicknessmapBlend;
	static OptionBool   layerSegSloMapsAutoUpdate;
	static OptionBool   layerSegHighlightSegLine;


	static OptionInt    freeFormedSegmetationLineThickness;
	static OptionBool   freeFormedSegmetationShowArea;

	static OptionBool   intervallMarkSloMapAuteGenerate;
	
	static std::vector<Option*>& getAllOptions()                    { return getAllOptionsPrivate().allConfig; }
	
	static void readAllOptions();
	static void writeAllOptions();
	static void resetAllOptions();

	static QSettings& getSettings();
	
	static QAction* getResetAction();

	static void setIniFile(const QString& iniFilename);
	static void setOptionsPostfix(const QString& postfix);
	static void setSaveOptions(bool save);
	static bool getSaveOptions();

private slots:
	void resetAllOptionsPrivate()                                   { resetAllOptions(); }

private:
	static void registerOption(Option* option);
	static ConfigList& getAllOptionsPrivate();
};


#endif // PROGRAMOPTIONS_H
