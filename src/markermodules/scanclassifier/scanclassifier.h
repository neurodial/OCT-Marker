#ifndef SCANCLASSIFIER_H
#define SCANCLASSIFIER_H

#include"classifiermarkerstate.h"
#include"classifiermarkerproxy.h"
#include"definedclassifiermarker.h"

#include "../bscanmarkerbase.h"

class ScanClassifier : public BscanMarkerBase
{
	class ScanClassifierStates
	{
		std::vector<ClassifierMarkerState> states;
	public:
		ScanClassifierStates(const DefinedClassifierMarker::ClassifierMarkerMap& classifierMap);
		~ScanClassifierStates();

		void reset();
	};

	Q_OBJECT
public:
	ScanClassifier(OctMarkerManager* markerManager);
	~ScanClassifier();

	virtual QWidget* getWidget   ()          override               { return widgetPtr2WGScanClassifier; }

	virtual void saveState(boost::property_tree::ptree& markerTree) override;
	virtual void loadState(boost::property_tree::ptree& markerTree) override;
	virtual void newSeriesLoaded(const OctData::Series*, boost::property_tree::ptree&) override;

	virtual bool hasChangedSinceLastSave() const override           { return stateChangedSinceLastSave; }

	std::vector<ClassifierMarkerProxy*>& getScanClassifierProxys()  { return scanClassifierProxys; }
	std::vector<ClassifierMarkerProxy*>& getBScanClassifierProxys() { return slideClassifierProxys; }


	virtual void setActBScan(std::size_t bscan) override;

private:

	void resetStates();

	bool     stateChangedSinceLastSave  = false;
	QWidget* widgetPtr2WGScanClassifier = nullptr;

	ScanClassifierStates scanClassifierStates;
	std::vector<ClassifierMarkerProxy*> scanClassifierProxys;

	std::vector<ScanClassifierStates> slidesClassifierStates;
	std::vector<ClassifierMarkerProxy*> slideClassifierProxys;
};

#endif // SCANCLASSIFIER_H
