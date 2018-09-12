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

#include "definedintervalmarker.h"

DefinedIntervalMarker::DefinedIntervalMarker()
{
/*
	IntervalMarker segmentationQuality("segmentationQuality", tr("segmentation quality").toStdString());

	segmentationQuality.addMarker(IntervalMarker::Marker("good"          , tr("good"             ).toStdString(),   0, 255,   0));
// 	segmentationQuality.addMarker(IntervalMarker::Marker("mid"           , tr("middle"           ).toStdString(), 255, 255,   0));
// 	segmentationQuality.addMarker(IntervalMarker::Marker("bad"           , tr("bad"              ).toStdString(), 255,   0,   0));

	intervallMarkerMap.emplace("segmentationQuality", segmentationQuality);
*/


	IntervalMarker objectZones("objectZones", tr("object zones").toStdString());

	objectZones.addMarker(IntervalMarker::Marker("edge"          , tr("edge"                    ).toStdString(), 255, 255,   0));
	objectZones.addMarker(IntervalMarker::Marker("bloodVessel"   , tr("blood vesel"             ).toStdString(), 255,   0,   0));

	addMarker(std::move(objectZones));



	IntervalMarker qualityFailures("qualityFailures", tr("quality failures").toStdString());

	qualityFailures.addMarker(IntervalMarker::Marker("signalmissing" , tr("signal missing"          ).toStdString(), 255, 255,   0));
	qualityFailures.addMarker(IntervalMarker::Marker("ILM_upper"     , tr("ILM upper"               ).toStdString(), 255,   0,   0));
	qualityFailures.addMarker(IntervalMarker::Marker("ILM_lower"     , tr("ILM lower"               ).toStdString(), 240, 110,  90));
	qualityFailures.addMarker(IntervalMarker::Marker("BM_upper"      , tr("BM upper"                ).toStdString(), 255, 140,   0));
	qualityFailures.addMarker(IntervalMarker::Marker("BM_lower"      , tr("BM lower"                ).toStdString(), 255, 200,   0));
	qualityFailures.addMarker(IntervalMarker::Marker("MultiError"    , tr("multi error"             ).toStdString(), 138,  43, 226));

	addMarker(std::move(qualityFailures));



	IntervalMarker segmentationFailures("segmentationFailures", tr("segmentation failures").toStdString());

	segmentationFailures.addMarker(IntervalMarker::Marker("ILM_upper"     , tr("ILM upper"               ).toStdString(), 255,   0,   0));
	segmentationFailures.addMarker(IntervalMarker::Marker("BM_lower"      , tr("BM lower"                ).toStdString(), 255, 200,   0));
	segmentationFailures.addMarker(IntervalMarker::Marker("SegILMmissing" , tr("missing ILM segmentation").toStdString(),   0, 205, 205));
	segmentationFailures.addMarker(IntervalMarker::Marker("SegBMmissing"  , tr("missing BM segmentation" ).toStdString(),   0, 140, 140));
	segmentationFailures.addMarker(IntervalMarker::Marker("MultiError"    , tr("multi error"             ).toStdString(), 138,  43, 226));

	addMarker(std::move(segmentationFailures));



	IntervalMarker qualityGrading("qualityGrading", tr("quality grading").toStdString());

	qualityGrading.addMarker(IntervalMarker::Marker("signalmissing"   , tr("signal missing"          ).toStdString(), 255, 255,   0));
	qualityGrading.addMarker(IntervalMarker::Marker("cut_upper"       , tr("cut upper"               ).toStdString(), 255,   0,   0));
	qualityGrading.addMarker(IntervalMarker::Marker("cut_lower"       , tr("cut lower"               ).toStdString(),   0, 128,   0));
	qualityGrading.addMarker(IntervalMarker::Marker("high_noise"      , tr("high noise"              ).toStdString(), 255, 140,   0));
	qualityGrading.addMarker(IntervalMarker::Marker("bad_illumination", tr("bad_illumination"        ).toStdString(),   0, 255, 255));
	qualityGrading.addMarker(IntervalMarker::Marker("MultiError"      , tr("multi error"             ).toStdString(), 138,  43, 226));

	addMarker(std::move(qualityGrading));
}


void DefinedIntervalMarker::addMarker(IntervalMarker&& marker)
{
	intervallMarkerMap.emplace(marker.getInternalName(), marker);
}

