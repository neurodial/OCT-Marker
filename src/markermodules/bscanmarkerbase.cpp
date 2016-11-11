#include "bscanmarkerbase.h"


#include <octdata/datastruct/series.h>
#include <octdata/datastruct/bscan.h>

#include <QToolBar>


#include <manager/octmarkermanager.h>

std::size_t BscanMarkerBase::getActBScanNr() const
{
	return static_cast<std::size_t>(markerManager->getActBScan());
}

int BscanMarkerBase::getBScanWidth(std::size_t nr) const
{
	const OctData::Series* series = getSeries();
	if(series)
		return series->getBScan(nr)->getWidth();
	return 0;
}

int BscanMarkerBase::getBScanWidth() const
{
	return getBScanWidth(getActBScanNr());
}

int BscanMarkerBase::getBScanHight() const
{
	const OctData::Series* series = getSeries();
	if(series)
		return series->getBScan(getActBScanNr())->getHeight();
	return 0;
}

const OctData::Series* BscanMarkerBase::getSeries() const
{
	return markerManager->getSeries();
}

const OctData::BScan* BscanMarkerBase::getActBScan() const
{
	return getBScan(getActBScanNr());
}

const OctData::BScan * BscanMarkerBase::getBScan(std::size_t nr) const
{
	const OctData::Series* series = getSeries();
	if(series)
		return series->getBScan(nr);
	return nullptr;
}


void BscanMarkerBase::activate(bool b)
{
	isActivated = b;
	enabledToolbar(b);
}


void BscanMarkerBase::connectToolBar(QToolBar* toolbar)
{
	connect(this, &BscanMarkerBase::enabledToolbar, toolbar, &QToolBar::setEnabled);
	toolbar->setEnabled(isActivated);
}


bool BscanMarkerBase::setMarkerActive(bool active, BScanMarkerWidget*)
{
	bool result = (active != markerActive);
	markerActive = active;
	return result;
}

