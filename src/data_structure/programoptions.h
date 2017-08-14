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
	
	static OptionBool sloShowBscans;
	static OptionBool sloShowOnylActBScan;
	static OptionBool sloShowLabels;
	static OptionBool sloShowGrid;
	
	static OptionString octDirectory;
	static OptionString loadOctdataAtStart;
	
	static OptionBool autoSaveOctMarkers;
	static OptionInt  defaultFileformatOctMarkers;

	static OptionInt  bscanMarkerToolId;
	static OptionInt    sloMarkerToolId;

	
	
	static std::vector<Option*>& getAllOptions()                    { return getAllOptionsPrivate(); }
	
	static void readAllOptions();
	static void writeAllOptions();
	static void resetAllOptions();

	static QSettings& getSettings();
	

	static void setIniFile(const QString& iniFilename);
private:
	static void registerOption(Option* option);
	static std::vector<Option*>& getAllOptionsPrivate();
};


#endif // PROGRAMOPTIONS_H
