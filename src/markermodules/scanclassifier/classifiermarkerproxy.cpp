#include "classifiermarkerproxy.h"

#include<helper/actionclasses.h>

#include"classifiermarker.h"
#include"classifiermarkerstate.h"

ClassifierMarkerProxy::ClassifierMarkerProxy(const ClassifierMarker& marker)
: marker(marker)
{
	int id = 0;
	for(const ClassifierMarker::Marker& item : marker)
	{
		IntValueAction* itemAction = new IntValueAction(id, this);
		itemAction->setText(item.getName().c_str());
		itemAction->setEnabled(false);
		connect(itemAction, &IntValueAction::triggeredBool, this, &ClassifierMarkerProxy::markerStateChanged);

		markerActions.push_back(itemAction);
		++id;
	}

	if(marker.getclassifierChoiceType() == ClassifierMarker::ClassifierChoiceType::Single)
	{
		actionGroup = new QActionGroup(this);
		for(QAction* action : markerActions)
			actionGroup->addAction(action);
	}
}


void ClassifierMarkerProxy::setMarkerState(ClassifierMarkerState* marker)
{
	bool activate = marker != nullptr;

	for(QAction* action : markerActions)
		action->setEnabled(activate);

	actMarkerState = marker;
	updateActionStates();
}

void ClassifierMarkerProxy::updateActionStates()
{
	if(actMarkerState)
	{
		std::size_t id = 0;
		for(QAction* action : markerActions)
		{
			action->setChecked(actMarkerState->getStatusId(id));
			++id;
		}
	}
	else
		for(QAction* action : markerActions)
			action->setChecked(false);
}


void ClassifierMarkerProxy::markerStateChanged(int id, bool value)
{
	if(actMarkerState)
	{
		qDebug("marker id %d in marker %s set to %d", id, marker.getInternalName().c_str(), (int)value);
		try
		{
			actMarkerState->setStatusId(static_cast<std::size_t>(id), value);
		}
		catch(...) // TODO: error handling, when a exception is throw, than it is a programming bug
		{
			qErrnoWarning("illigal marker id %d in marker %s", id, marker.getInternalName().c_str());
		}
	}
}
