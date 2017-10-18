#ifndef PROGRAMOPTIONS_H
#define PROGRAMOPTIONS_H


#include <helper/programoptionstype.h>

#include <vector>

class ProgramOptions;
class QSettings;


class ProgramOptions
{
	ProgramOptions();
	~ProgramOptions();

	static ProgramOptions& getInstance() { static ProgramOptions instance; return instance; }

	QSettings* settings = nullptr;

	struct ConfigList
	{
		std::map<QString, std::vector<Option*>> sortedConfig;
		std::vector<Option*>                    allConfig;
	};

public:
	friend class Option;

	static OptionBool  bscansShowSegmentationslines;
	static OptionColor bscanSegmetationLineColor;
	static OptionInt   bscanSegmetationLineThicknes;
	
	static OptionBool fillEmptyPixelWhite;
	static OptionBool registerBScans;
	static OptionBool loadRotateSlo;

	static OptionBool holdOCTRawData;
	static OptionBool readBScans;

	static OptionInt  e2eGrayTransform;
	
	static OptionBool sloShowLabels;
	static OptionBool sloShowGrid;
	static OptionBool sloShowBScanMousePos;
	static OptionInt  sloShowsBScansPos;
	
	static OptionString octDirectory;
	static OptionString loadOctdataAtStart;
	
	static OptionBool autoSaveOctMarkers;
	static OptionInt  defaultFileformatOctMarkers;

	static OptionInt  bscanMarkerToolId;
	static OptionInt    sloMarkerToolId;

	
	static OptionDouble layerSegFindPointInsertTol  ;
	static OptionDouble layerSegFindPointRemoveTol  ;
	static OptionDouble layerSegFindPointMaxAbsError;

	static OptionColor  layerSegActiveLineColor;
	static OptionColor  layerSegPassivLineColor;
	static OptionInt    layerSegActiveLineSize;
	static OptionInt    layerSegPassivLineSize;
	static OptionInt    layerSegSplinePointSize;

	
	static std::vector<Option*>& getAllOptions()                    { return getAllOptionsPrivate().allConfig; }
	
	static void readAllOptions();
	static void writeAllOptions();
	static void resetAllOptions();

	static QSettings& getSettings();
	

	static void setIniFile(const QString& iniFilename);
private:
	static void registerOption(Option* option);
	static ConfigList& getAllOptionsPrivate();
};


#endif // PROGRAMOPTIONS_H
