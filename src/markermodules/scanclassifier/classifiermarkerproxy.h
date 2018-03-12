#ifndef CLASSIFIERMARKERPROXY_H
#define CLASSIFIERMARKERPROXY_H

#include<QObject>
#include<QAction>
#include<vector>

class ClassifierMarker;
class ClassifierMarkerState;
class QActionGroup;

class ClassifierMarkerProxy : public QObject
{
	std::vector<QAction*> actions;
	const ClassifierMarker& marker;
	ClassifierMarkerState* actMarkerState = nullptr;

	std::vector<QAction*> markerActions;

	QActionGroup* actionGroup = nullptr;

public:
	ClassifierMarkerProxy(const ClassifierMarker& marker);

	void setMarkerState(ClassifierMarkerState* marker);

	std::vector<QAction*> getMarkerActions()                        { return markerActions; }
	const ClassifierMarker& getClassifierMarker()             const { return marker; }

private slots:
	void markerStateChanged(int id, bool value);
	void updateActionStates();
};

#endif // CLASSIFIERMARKERPROXY_H
