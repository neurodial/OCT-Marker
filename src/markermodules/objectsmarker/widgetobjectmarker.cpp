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

#include "widgetobjectmarker.h"

#include<QVBoxLayout>
#include<QPushButton>


WidgetObjectMarker::WidgetObjectMarker(QWidget* parent)
: QWidget(parent)
{
	QVBoxLayout* layout = new QVBoxLayout(this);

	addObjectButton = new QPushButton(this);

	addObjectButton->setText(tr("add rect"));
	addObjectButton->setCheckable(true);

	connect(addObjectButton, &QAbstractButton::toggled, this, &WidgetObjectMarker::addObjectStatusChangedSlotPrivte);

	layout->addWidget(addObjectButton);

	layout->addStretch();

	setLayout(layout);
}

void WidgetObjectMarker::addObjectStatusChangedSlot(bool v)
{
	addObjectButton->setChecked(v);
}

void WidgetObjectMarker::addObjectStatusChangedSlotPrivte(bool v)
{
	addObjectStatusChangedSignal(v);
}

