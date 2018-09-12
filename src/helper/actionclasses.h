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

#ifndef ACTIONCLASSES_H
#define ACTIONCLASSES_H

#include <QAction>

class IntValueAction : public QAction
{
	Q_OBJECT
	int valTrue;
public:
	IntValueAction(const int t, QObject* parent, bool checkable = true) : QAction(parent), valTrue(t)
	{
		setCheckable(checkable);
		if(checkable)
			connect(this, &QAction::toggled, this, &IntValueAction::onTriggered);
		else
			connect(this, &QAction::triggered, this, &IntValueAction::onTriggered);
	}
signals:
	void triggered(int v);
	void triggeredBool(int v, bool bv);

private slots:
	void onTriggered(bool v)
	{
		if(v || !isCheckable())
			emit(triggered(valTrue));
		emit(triggeredBool(valTrue, v));
	}
public slots:
	void valueChanged(int v)
	{
		blockSignals(true);
		if(v == valTrue)
			setChecked(true);
		else
			setChecked(false);
		blockSignals(false);
	}
};

class SizetValueAction : public QAction
{
	Q_OBJECT
	std::size_t valTrue;
public:
	SizetValueAction(size_t t, QObject* parent) : QAction(parent), valTrue(t)
	{
		setCheckable(true);
		connect(this, &QAction::toggled, this, &SizetValueAction::onTriggered);
	}
signals:
	void triggered(std::size_t v);

private slots:
	void onTriggered(bool v)
	{
		if(v)
			emit(triggered(valTrue));
	}
public slots:
	void valueChanged(std::size_t v)
	{
		if(v == valTrue)
			setChecked(true);
		else
			setChecked(false);
	}
};

#endif // ACTIONCLASSES_H
