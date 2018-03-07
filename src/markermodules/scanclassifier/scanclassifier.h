#ifndef SCANCLASSIFIER_H
#define SCANCLASSIFIER_H


#include "../bscanmarkerbase.h"


class ScanClassifier : public BscanMarkerBase
{
public:

	ScanClassifier(OctMarkerManager* markerManager);
	~ScanClassifier();

	virtual QWidget* getWidget   ()          override               { return widgetPtr2WGScanClassifier; }

	virtual void saveState(boost::property_tree::ptree& markerTree)  override;
	virtual void loadState(boost::property_tree::ptree& markerTree)  override;

	virtual bool hasChangedSinceLastSave() const override           { return stateChangedSinceLastSave; }

private:

	bool     stateChangedSinceLastSave  = false;
	QWidget* widgetPtr2WGScanClassifier = nullptr;
};

#endif // SCANCLASSIFIER_H
