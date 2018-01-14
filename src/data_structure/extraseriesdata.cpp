#include "extraseriesdata.h"

#include <manager/octdatamanager.h>

#include <cpp_framework/cvmat/treestructbin.h>
#include <cpp_framework/cvmat/cvmattreestruct.h>
#include <cpp_framework/cvmat/cvmattreestructextra.h>

#include <QFileInfo>

namespace
{
	bool loadExtraDataFromFile(ExtraSeriesData::ExtraBScanData& extraBScanData, const QString& contur2dFilename)
	{
		QFileInfo info(contur2dFilename);
		if(info.exists())
		{
			const CppFW::CVMatTree contTree = CppFW::CVMatTreeStructBin::readBin(contur2dFilename.toStdString());
			for(const CppFW::CVMatTree* bscan : contTree.getNodeList())
			{
				ExtraImageData imgData;
				if(!bscan)
					break;

				for(const CppFW::CVMatTree* segment : bscan->getDirNode("segments").getNodeList())
				{
					if(!segment)
						break;
					ContureSegment contSegment;

					contSegment.cirled = CppFW::CVMatTreeExtra::getCvScalar(segment, "circled", true);
					const cv::Mat& pointsMat = segment->getDirNode("points").getMat();
					contSegment.points.reserve(pointsMat.rows);

					for(int i = 0; i<pointsMat.rows; ++i)
						contSegment.points.push_back(Point2D(pointsMat.at<float>(i,0), pointsMat.at<float>(i,1)));

					imgData.addContureSegment(std::move(contSegment));
				}

			extraBScanData.push_back(std::move(imgData));
			}
			return true;
		}
		return false;
	}
}

bool ExtraSeriesData::loadExtraData(const OctData::Series& series, const bpt::ptree& ptree)
{
	try
	{
		extraBScanData.clear();

		// TODO: unschöne Übergangslösung
		const QString& filename = OctDataManager::getInstance().getLoadedFilename();
		QString contur2dFilename = filename + "_2d_cont.bin";
		if(loadExtraDataFromFile(extraBScanData, contur2dFilename))
			return true;

		extraBScanData.clear();

		QFileInfo info(filename);
		contur2dFilename = info.path() + "/" + info.completeBaseName() + "_2d_cont.bin";
		if(loadExtraDataFromFile(extraBScanData, contur2dFilename))
			return true;
	}
	catch(...)
	{

	}

	return false;
}
