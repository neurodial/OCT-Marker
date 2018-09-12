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

#include "bscanmarkerbase.h"


#include <octdata/datastruct/series.h>
#include <octdata/datastruct/bscan.h>

#include <QToolBar>

#include<opencv/cv.hpp>

#include <manager/octmarkermanager.h>
#include<data_structure/markercommand.h>

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


namespace
{
	template<int channels>
	bool drawSLOOverlayImage4Datatype(const cv::Mat& sloImage, cv::Mat& outSloImage, double alpha, const cv::Mat& sloOverlay)
	{
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
					dest[k] = static_cast<uint8_t>(src[k % channels]*(1-blend) + thi[k]*blend);
			}
			else
				for(int k = 0; k<3; ++k)
					dest[k] = src[k % channels];

			thi  += 4;
			dest += 3;
			src += channels;
		}
		return true;
	}
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
		if(sloImage.depth() != cv::DataType<uint8_t>::type || sloOverlay.type() != CV_8UC4)
		{
			qDebug("%d != %d || %d != %d", sloImage.depth(), cv::DataType<uint8_t>::type, sloOverlay.type(), CV_8UC4);
			return false;
		}
		switch(sloImage.channels())
		{
			case 1:
				drawSLOOverlayImage4Datatype<1>(sloImage, outSloImage, alpha, sloOverlay);
				return true;
			case 3:
				drawSLOOverlayImage4Datatype<3>(sloImage, outSloImage, alpha, sloOverlay);
				return true;
			default:
				qDebug("Unsupported number of channels: %d", sloImage.channels());
				return false;
		}
	}
	return false;
}


// Undo redo functions

void BscanMarkerBase::addUndoCommand(MarkerCommand* command)
{
	clearRedo();
	undoList.push_back(command);

	undoRedoChanged();
}

void BscanMarkerBase::callRedoStep()
{
	if(redoList.size() == 0)
		return;

	MarkerCommand* command = redoList.back();
	if(!checkBScan(command))
		return;

	if(!command->redo())
		return;

	undoList.push_back(command);
	redoList.pop_back();

	undoRedoChanged();
}

void BscanMarkerBase::callUndoStep()
{
	if(undoList.size() == 0)
		return;

	MarkerCommand* command = undoList.back();
	if(!checkBScan(command))
		return;

	if(!command->undo())
		return;

	redoList.push_back(command);
	undoList.pop_back();

	undoRedoChanged();
}

void BscanMarkerBase::clearUndoRedo()
{
	clearRedo();
	for(MarkerCommand* command : undoList)
		delete command;
	undoList.clear();

	undoRedoChanged();
}

void BscanMarkerBase::clearRedo()
{
	for(MarkerCommand* command : redoList)
		delete command;
	redoList.clear();
}

bool BscanMarkerBase::checkBScan(MarkerCommand* command)
{
	int bscan = command->getBScan();
	if(bscan == -1)
		return true;
	if(bscan != static_cast<int>(getActBScanNr()))
	{
		requestChangeBscan(bscan);
		return false;
	}
	return true;
}

