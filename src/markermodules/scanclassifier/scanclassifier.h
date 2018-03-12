#ifndef SCANCLASSIFIER_H
#define SCANCLASSIFIER_H

#include"classifiermarkerstate.h"
#include"classifiermarkerproxy.h"

#include "../bscanmarkerbase.h"

class ScanClassifier : public BscanMarkerBase
{
	Q_OBJECT
public:
	ScanClassifier(OctMarkerManager* markerManager);
	~ScanClassifier();

	virtual QWidget* getWidget   ()          override               { return widgetPtr2WGScanClassifier; }

	virtual void saveState(boost::property_tree::ptree& markerTree)  override;
	virtual void loadState(boost::property_tree::ptree& markerTree)  override;

	virtual bool hasChangedSinceLastSave() const override           { return stateChangedSinceLastSave; }

	std::vector<ClassifierMarkerProxy*>& getScanClassifierProxys()  { return scanClassifierProxys; }

private:

	bool     stateChangedSinceLastSave  = false;
	QWidget* widgetPtr2WGScanClassifier = nullptr;

	std::vector<ClassifierMarkerState*> scanClassifierStates;
	std::vector<ClassifierMarkerProxy*> scanClassifierProxys;
};

#endif // SCANCLASSIFIER_H
