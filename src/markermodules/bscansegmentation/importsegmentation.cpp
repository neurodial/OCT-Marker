#include "importsegmentation.h"

#include "bscansegmentation.h"
#include <data_structure/programoptions.h>
#include <data_structure/simplecvmatcompress.h>

#include <octdata/filereadoptions.h>
#include <octdata/octfileread.h>
#include <octdata/datastruct/oct.h>
#include <octdata/datastruct/bscan.h>


#include <opencv/cv.h>

namespace
{
	void combineMat(cv::Mat& dest, const cv::Mat& source)
	{
		const int rows = std::min(dest.rows, source.rows);
		const int cols = std::min(dest.cols, source.cols);

		for(int row = 0; row < rows; ++row)
		{
			      uint8_t* destIt   = dest  .ptr<uint8_t>(row);
			const uint8_t* sourceIt = source.ptr<uint8_t>(row);

			for(int col = 0; col < cols; ++col)
			{
				if(*sourceIt > 0)
					*destIt |= 2;
				++destIt;
				++sourceIt;
			}
		}
	}



}

bool ImportSegmentation::importOct(BScanSegmentation* markerManager, const std::string& filename)
{
	if(!markerManager)
		return false;



	OctData::FileReadOptions octOptions;
	octOptions.e2eGray             = static_cast<OctData::FileReadOptions::E2eGrayTransform>(ProgramOptions::e2eGrayTransform());
	octOptions.registerBScanns     = ProgramOptions::registerBScans();
	octOptions.fillEmptyPixelWhite = ProgramOptions::fillEmptyPixelWhite();
	octOptions.holdRawData         = ProgramOptions::holdOCTRawData();

	OctData::OCT oct = OctData::OctFileRead::openFile(filename, octOptions, nullptr);

	if(oct.size() == 0)
		return false;

	const OctData::Patient* actPatient = nullptr;
	const OctData::Study*   actStudy   = nullptr;
	const OctData::Series*  actSeries  = nullptr;

	actPatient = oct.begin()->second;
	if(actPatient->size() > 0)
	{
		actStudy = actPatient->begin()->second;

		if(actStudy->size() > 0)
		{
			actSeries = actStudy->begin()->second;
		}
	}

	if(!actSeries)
		return false;

	const std::size_t numBScans = actSeries->bscanCount();

	markerManager->saveActMatState();

	std::size_t count = 0;
	for(SimpleCvMatCompress* compressedMat : markerManager->segments)
	{
		const OctData::BScan* actBScan = actSeries->getBScan(count);
		++count;

		if(!compressedMat || !actBScan)
			continue;


		cv::Mat map;
		compressedMat->writeToMat(map);
		combineMat(map, actBScan->getImage());
		compressedMat->readFromMat(map);

		if(count >= numBScans)
			break;
	}

	markerManager->rejectMatChanges();
	return true;
}
