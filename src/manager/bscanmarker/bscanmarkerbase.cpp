#include "bscanmarkerbase.h"


#include <octdata/datastruct/series.h>
#include <octdata/datastruct/bscan.h>


#include <manager/bscanmarkermanager.h>

int BscanMarkerBase::getActBScan() const
{
	return markerManager->getActBScan();
}

int BscanMarkerBase::getBScanWidth() const
{
	const OctData::Series* series = getSeries();
	if(series)
		return series->getBScan(getActBScan())->getWidth();
	return 0;
}

const OctData::Series* BscanMarkerBase::getSeries() const
{
	return markerManager->getSeries();
}
