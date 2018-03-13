#ifndef SCANCLASSIFIER_H
#define SCANCLASSIFIER_H

#include"classifiermarkerstate.h"
#include"classifiermarkerproxy.h"
#include"definedclassifiermarker.h"

#include "../bscanmarkerbase.h"

class ScanClassifier : public BscanMarkerBase
{
	Q_OBJECT

	class ClassifierStates
	{
		std::vector<ClassifierMarkerState> states;
	public:
		ClassifierStates(const DefinedClassifierMarker::ClassifierMarkerMap& classifierMap);
		~ClassifierStates();

		ClassifierMarkerState& getState(std::size_t id)                { return states.at(id); }
		std::size_t size()                                       const { return states.size(); }

		void reset();
	};

public:
	class ClassifierProxys
	{
	public:
		typedef std::vector<ClassifierMarkerProxy*> ProxyList;

		ClassifierProxys(const DefinedClassifierMarker::ClassifierMarkerMap& classifierMap);
		~ClassifierProxys();

		void setClassifierStates(ClassifierStates* states);

		ProxyList::iterator begin()                                    { return proxys.begin(); }
		ProxyList::iterator end()                                      { return proxys.end(); }
	private:
		ProxyList proxys;
	};

	ScanClassifier(OctMarkerManager* markerManager);
	~ScanClassifier();

	virtual QWidget* getWidget   ()          override               { return widgetPtr2WGScanClassifier; }

	virtual void saveState(boost::property_tree::ptree& markerTree) override;
	virtual void loadState(boost::property_tree::ptree& markerTree) override;
	virtual void newSeriesLoaded(const OctData::Series*, boost::property_tree::ptree&) override;

	virtual bool hasChangedSinceLastSave() const override           { return stateChangedSinceLastSave; }

	ClassifierProxys& getScanClassifierProxys()  { return scanClassifierProxys; }
	ClassifierProxys& getBScanClassifierProxys() { return slideClassifierProxys; }


	virtual void setActBScan(std::size_t bscan) override;

private:

	void resetStates();

	bool     stateChangedSinceLastSave  = false;
	QWidget* widgetPtr2WGScanClassifier = nullptr;

	ClassifierStates scanClassifierStates;
	ClassifierProxys scanClassifierProxys;

	std::vector<ClassifierStates> slidesClassifierStates;
	ClassifierProxys slideClassifierProxys;
};

#endif // SCANCLASSIFIER_H
