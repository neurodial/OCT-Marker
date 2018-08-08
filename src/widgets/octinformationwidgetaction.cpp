/*
 * Copyright (C) 2018  kay
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
 */

#include "octinformationwidgetaction.h"

#include<QMouseEvent>
#include<QIcon>

#include "octinformationswidget.h"


namespace
{
	class OctInformationsNotCloseWidgetAction : public OctInformationsWidget
	{
	public:
		OctInformationsNotCloseWidgetAction(QWidget* parent) : OctInformationsWidget(parent) {}
	protected:
		void mouseReleaseEvent(QMouseEvent* event) override
		{
			OctInformationsWidget::mouseReleaseEvent(event);
			event->setAccepted(true);
		}
	};
}


OctInformationWidgetAction::OctInformationWidgetAction(QWidget* parent)
: QWidgetAction(parent)
{
	setIcon(QIcon::fromTheme("document-properties",  QIcon(":/icons/tango/actions/document-properties.svgz")));
}


QWidget* OctInformationWidgetAction::createWidget(QWidget* parent)
{
	return new OctInformationsNotCloseWidgetAction(parent);
}
