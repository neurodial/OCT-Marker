#include "definedintervalmarker.h"


DefinedIntervalMarker::DefinedIntervalMarker()
{
	IntervalMarker signalQuality("signalQuality", tr("signal quality").toStdString());

	signalQuality.addMarker(IntervalMarker::Marker("good"          , tr("good signal"             ).toStdString(),   0, 255,   0));
	signalQuality.addMarker(IntervalMarker::Marker("mid"           , tr("middle signal"           ).toStdString(), 255, 255,   0));
	signalQuality.addMarker(IntervalMarker::Marker("bad"           , tr("bad signal"              ).toStdString(), 255,   0,   0));

	intervallMarkerMap.emplace("signalQuality", signalQuality);




	IntervalMarker objectZones("objectZones", tr("object zones").toStdString());

	objectZones.addMarker(IntervalMarker::Marker("macula"        , tr("macula"                  ).toStdString(),   0, 255,   0));
	objectZones.addMarker(IntervalMarker::Marker("edge"          , tr("edge"                    ).toStdString(), 255, 255,   0));
	objectZones.addMarker(IntervalMarker::Marker("bloodVessel"   , tr("blood vesel"             ).toStdString(), 255,   0,   0));

	intervallMarkerMap.emplace("objectZones", objectZones);




	IntervalMarker qualityFailures("qualityFailures", tr("quality failures").toStdString());

	qualityFailures.addMarker(IntervalMarker::Marker("signalmissing" , tr("signal missing"          ).toStdString(), 255, 255,   0));
	qualityFailures.addMarker(IntervalMarker::Marker("ILM_upper"     , tr("ILM upper"               ).toStdString(), 255,   0,   0));
	qualityFailures.addMarker(IntervalMarker::Marker("ILM_lower"     , tr("ILM lower"               ).toStdString(), 240, 110,  90));
	qualityFailures.addMarker(IntervalMarker::Marker("BM_upper"      , tr("BM upper"                ).toStdString(), 255, 140,   0));
	qualityFailures.addMarker(IntervalMarker::Marker("BM_lower"      , tr("BM lower"                ).toStdString(), 255, 200,   0));
	qualityFailures.addMarker(IntervalMarker::Marker("SegILMmissing" , tr("missing ILM segmentation").toStdString(),   0, 205, 205));
	qualityFailures.addMarker(IntervalMarker::Marker("SegBMmissing"  , tr("missing BM segmentation" ).toStdString(),   0, 140, 140));
	qualityFailures.addMarker(IntervalMarker::Marker("MultiError"    , tr("multi error"             ).toStdString(), 138,  43, 226));

	intervallMarkerMap.emplace("qualityFailures", qualityFailures);


}


