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

#include "definedclassifiermarker.h"

DefinedClassifierMarker::DefinedClassifierMarker()
{
	// ------------------------

	ClassifierMarker scanArea("scanArea", tr("scan area").toStdString(), ClassifierMarker::ClassifierChoiceType::Single);

	scanArea.addMarker(ClassifierMarker::Marker("macular", tr("macular").toStdString()));
	scanArea.addMarker(ClassifierMarker::Marker("onh"    , tr("onh"    ).toStdString()));

	addMarker(volumeMarkers, std::move(scanArea));


	ClassifierMarker scanUsable("scanUsable", tr("scan usable").toStdString(), ClassifierMarker::ClassifierChoiceType::Single);

	scanUsable.addMarker(ClassifierMarker::Marker("usable"   , tr("usable"   ).toStdString()));
	scanUsable.addMarker(ClassifierMarker::Marker("ambiguous", tr("ambiguous").toStdString()));
	scanUsable.addMarker(ClassifierMarker::Marker("unusable" , tr("unusable" ).toStdString()));

	addMarker(volumeMarkers, std::move(scanUsable));

	// ------------------------

	ClassifierMarker scanAttributes("scanAttributes", "scan attributes", ClassifierMarker::ClassifierChoiceType::Multible);

	scanAttributes.addMarker(ClassifierMarker::Marker("attr1", "attribut 1"));
	scanAttributes.addMarker(ClassifierMarker::Marker("attr2", "attribut 2"));
	scanAttributes.addMarker(ClassifierMarker::Marker("attr3", "attribut 3"));
	scanAttributes.addMarker(ClassifierMarker::Marker("attr4", "attribut 4"));

	addMarker(volumeMarkers, std::move(scanAttributes));

	// ------------------------

	ClassifierMarker bscanAttributes("bscanAttributes", "b-scan attributes", ClassifierMarker::ClassifierChoiceType::Multible);

	bscanAttributes.addMarker(ClassifierMarker::Marker("attr1", "attribut A"));
	bscanAttributes.addMarker(ClassifierMarker::Marker("attr2", "attribut B"));
	bscanAttributes.addMarker(ClassifierMarker::Marker("attr3", "attribut C"));
	bscanAttributes.addMarker(ClassifierMarker::Marker("attr4", "attribut D"));

	addMarker(bscanMarkers, std::move(bscanAttributes));

	// ------------------------

	ClassifierMarker bscanAttributesSingle("bscanAttributesSingle", "b-scan attributes single", ClassifierMarker::ClassifierChoiceType::Single);

	bscanAttributesSingle.addMarker(ClassifierMarker::Marker("attr1", "attribut I"));
	bscanAttributesSingle.addMarker(ClassifierMarker::Marker("attr2", "attribut II"));
	bscanAttributesSingle.addMarker(ClassifierMarker::Marker("attr3", "attribut III"));
	bscanAttributesSingle.addMarker(ClassifierMarker::Marker("attr4", "attribut IV"));

	addMarker(bscanMarkers, std::move(bscanAttributesSingle));
}

void DefinedClassifierMarker::addMarker(ClassifierMarkerMap& map, ClassifierMarker&& marker)
{
// 	map.emplace(marker.getInternalName(), marker);
	map.emplace_back(marker);
}
