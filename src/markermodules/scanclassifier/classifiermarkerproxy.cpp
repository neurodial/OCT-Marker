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
// 		qDebug("marker id %d in marker %s set to %d", id, marker.getInternalName().c_str(), (int)value);
		try
		{
			if(actMarkerState->getStatusId(static_cast<std::size_t>(id)) != value)
			{
				actMarkerState->setStatusId(static_cast<std::size_t>(id), value);
				changes = true;
			}
		}
		catch(...) // TODO: error handling, when a exception is throw, than it is a programming bug
		{
			qErrnoWarning("illigal marker id %d in marker %s", id, marker.getInternalName().c_str());
		}
	}
}
