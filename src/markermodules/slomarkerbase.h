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

#ifndef SLOMARKERBASE_H
#define SLOMARKERBASE_H

#include <QObject>
#include <QIcon>

#include <boost/property_tree/ptree_fwd.hpp>


class OctMarkerManager;

class QToolBar;
class QGraphicsScene;

namespace OctData
{
	class Series;
	class CoordSLOpx;
};

class SloMarkerBase : public QObject
{
	Q_OBJECT
public:
	class RedrawRequest
	{
	public:
		bool redraw = false;
		QRect rect;
	};


	SloMarkerBase(OctMarkerManager* markerManager) : markerManager(markerManager) {}
	virtual ~SloMarkerBase()                                        {}

	virtual QGraphicsScene* getGraphicsScene()                      { return nullptr; }
	virtual const QGraphicsScene* getGraphicsScene() const          { return nullptr; }


	virtual void saveState(boost::property_tree::ptree&)            {}
	virtual void loadState(boost::property_tree::ptree&)            {}

	virtual void newSeriesLoaded(const OctData::Series*, boost::property_tree::ptree&)
	                                                                {}

	virtual bool hasChangedSinceLastSave() const                    { return false; }

	virtual const QString& getName()                                { return name; }
	virtual const QString& getMarkerId()                            { return id;   }
	virtual const QIcon&   getIcon()                                { return icon; }

	virtual double getScaleFactor() const                           { return 1000;  }


	virtual void activate(bool);

protected:
	OctMarkerManager* const markerManager;

	void connectToolBar(QToolBar* toolbar);

	const OctData::Series* getSeries() const;

	QString name;
	QString id;
	QIcon  icon;
	bool isActivated  = false;
	bool markerActive = true;

};


#endif // SLOMARKERBASE_H
