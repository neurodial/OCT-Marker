#include "bscanmarkerbase.h"


#include <octdata/datastruct/series.h>
#include <octdata/datastruct/bscan.h>

#include <QToolBar>

#include<opencv/cv.hpp>

#include <manager/octmarkermanager.h>

std::size_t BscanMarkerBase::getActBScanNr() const
{
	return static_cast<std::size_t>(markerManager->getActBScanNum());
}

int BscanMarkerBase::getBScanWidth(std::size_t nr) const
{
	const OctData::Series* series = getSeries();
	if(series)
	{
		const OctData::BScan* bscan = series->getBScan(nr);
		if(bscan)
			return bscan->getWidth();
	}
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
	{
		const OctData::BScan* bscan = series->getBScan(getActBScanNr());
		if(bscan)
			return bscan->getHeight();
	}
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

bool BscanMarkerBase::drawSLOOverlayImage(const cv::Mat& sloImage, cv::Mat& outSloImage, double alpha, const cv::Mat& sloOverlay) const
{
	if(alpha == -1.)
	{
		outSloImage = sloOverlay;
		return true;
	}

	if(sloOverlay.cols == sloImage.cols && sloOverlay.rows == sloImage.rows)
	{
// 		std::cout << sloImage.type() << " != " << CV_8U << " || " << thicknesMapImage->type() << " != " << CV_8UC4 << std::endl;
		if(sloImage.type() != CV_8U || sloOverlay.type() != CV_8UC4)
			return false;

		outSloImage.create(sloImage.size(), CV_8UC3);
		const std::size_t length = static_cast<std::size_t>(sloImage.cols*sloImage.rows);

		const uint8_t* thi  = sloOverlay .ptr<uint8_t>(0);
		const uint8_t* src  =    sloImage.ptr<uint8_t>(0);
		      uint8_t* dest = outSloImage.ptr<uint8_t>(0);

		for(std::size_t i = 0; i<length; ++i)
		{
			uint8_t alphaThicknes = thi[3];
			if(alphaThicknes > 0)
			{
				double blend = static_cast<double>(alphaThicknes)/255. * alpha;
				for(int k = 0; k<3; ++k)
					dest[k] = static_cast<uint8_t>(src[0]*(1-blend) + thi[k]*blend);
			}
			else
				for(int k = 0; k<3; ++k)
					dest[k] = src[0];

			thi  += 4;
			dest += 3;
			++src;
		}
		return true;
	}
	return false;
}
