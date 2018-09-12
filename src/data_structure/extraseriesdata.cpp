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

#include "extraseriesdata.h"

#include <manager/octdatamanager.h>

#include <oct_cpp_framework/cvmat/treestructbin.h>
#include <oct_cpp_framework/cvmat/cvmattreestruct.h>
#include <oct_cpp_framework/cvmat/cvmattreestructextra.h>

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
