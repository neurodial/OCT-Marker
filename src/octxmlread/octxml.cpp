#include "octxml.h"

#include <data_structure/cscan.h>
#include <data_structure/sloimage.h>
#include <data_structure/bscan.h>

#include <iostream>
#include <fstream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

#include <boost/filesystem.hpp>

namespace bfs = boost::filesystem;

#include <opencv2/opencv.hpp>

using boost::property_tree::ptree;
using boost::property_tree::xml_parser::read_xml;



namespace
{
	std::string getFilename(ptree& imageNode)
	{
		boost::optional<ptree&> urlNode = imageNode.get_child_optional("ImageData.ExamURL");

		if(urlNode)
		{
			std::string urlFull = urlNode.get().get_value<std::string>();
			std::size_t filePos = urlFull.rfind('\\') + 1;
			return urlFull.substr(filePos);
		}
		std::cerr << "Image URL not found in XML\n";
		return std::string();
	}

	void fillSLOImage(ptree& imageNode, SLOImage* sloImage, const std::string& xmlPath)
	{
		sloImage->setFilename(getFilename(imageNode));

		std::string filepath = xmlPath + '/' + sloImage->getFilename();
		cv::Mat image = cv::imread(filepath, true);
		sloImage->setImage(image);
	}

	void fillBScann(ptree& imageNode, CScan& cscann, const std::string& xmlPath)
	{
		BScan::Data bscanData;


		std::string filename = getFilename(imageNode);
		std::string filepath = xmlPath + '/' + filename;
		cv::Mat image = cv::imread(filepath, true);


		cscann.takeBScan(new BScan(image, bscanData));
	}

}


void OctXml::readOctXml(const std::string& filename, CScan* cscan)
{
	if(!cscan)
		return;

	bfs::path xmlfile(filename);
	std::cout << xmlfile.branch_path() << std::endl;

	std::string xmlPath     = xmlfile.branch_path().generic_string();
	std::string xmlFilename = xmlfile.filename().generic_string();


	// Create an empty property tree object
	ptree pt;

	// Load the XML file into the property tree. If reading fails
	// (cannot open file, parse error), an exception is thrown.
	read_xml(filename, pt);


	const char* numImageNodeStr = "HEDX.BODY.Patient.Study.Series.NumImages";

	boost::optional<ptree&> numImagesNode = pt.get_child(numImageNodeStr);
	if(!numImagesNode)
	{
		std::cerr << "no Node " << numImageNodeStr << std::endl;
		throw "";
	}
	std::size_t numImages = numImagesNode.get().get_value<std::size_t>();


	for(std::pair<const std::string, ptree>& imageNode : pt.get_child("HEDX.BODY.Patient.Study.Series"))
	{
		if(imageNode.first != "Image")
			continue;

		// std::cout << t.first << std::endl;

		boost::optional<ptree&> type = imageNode.second.get_child_optional("ImageType.Type");

		if(!type)
		{
			std::cerr << __FILE__ << ":" << __LINE__ << ": image type not found\n";
			continue;
		}

		std::string typeStr = type.get().get_value<std::string>();

		if(typeStr == "LOCALIZER")
			fillSLOImage(imageNode.second, cscan->sloImage, xmlPath);

		if(typeStr == "OCT")
			fillBScann(imageNode.second, *cscan, xmlPath);
			/*
		{
			boost::optional<ptree&> imageNumberNode = imageNode.second.get_child_optional("ImageNumber");
			if(!imageNumberNode)
			{
				std::cerr << "ImageNumber not found\n";
				continue;
			}

			int ImageNumber = imageNumberNode.get().get_value<int>();

			boost::optional<ptree&> urlNode = imageNode.second.get_child_optional("ImageData.ExamURL");

			if(urlNode)
			{
				std::string urlFull = urlNode.get().get_value<std::string>();
				std::size_t filePos = urlFull.rfind('\\') + 1;
				std::string urlShort = urlFull.substr(filePos);

				// imagesData.at(ImageNumber).filename = urlShort;
			}
			else
				std::cerr << "Image URL not found in XML\n";

//			for(auto p : t.second)
//				std::cout << '\t' << p.first << " : " << std::endl;
		}
		*/

		// std::cout << t.second.get_child("ID").get_value<std::string>().data();
	}
}





/*
OctXml::OctXml(const std::string& filename)
{
	
	bfs::path xmlfile(filename);
	std::cout << xmlfile.branch_path() << std::endl;
	xmlPath     = xmlfile.branch_path().generic_string();
	xmlFilename = xmlfile.filename().generic_string();

	// Create an empty property tree object
	ptree pt;

	// Load the XML file into the property tree. If reading fails
	// (cannot open file, parse error), an exception is thrown.
	read_xml(filename, pt);


	const char* numImageNodeStr = "HEDX.BODY.Patient.Study.Series.NumImages";

	boost::optional<ptree&> numImagesNode = pt.get_child(numImageNodeStr);
	if(!numImagesNode)
	{
		std::cerr << "no Node " << numImageNodeStr << std::endl;
		throw "";
	}
	imagesData.resize(numImagesNode.get().get_value<std::size_t>());
	
	
	for(std::pair<const std::string, ptree>& t : pt.get_child("HEDX.BODY.Patient.Study.Series"))
	{
		if(t.first != "Image")
			continue;

		// std::cout << t.first << std::endl;

		boost::optional<ptree&> type = t.second.get_child_optional("ImageType.Type");

	//	if(type)
	//		std::cout << "type: " << type.get().get_value<std::string>() << std::endl;

		if(type && type.get().get_value<std::string>() == "OCT")
		{
			boost::optional<ptree&> imageNumberNode = t.second.get_child_optional("ImageNumber");
			if(!imageNumberNode)
			{
				std::cerr << "ImageNumber not found\n";
				continue;
			}
			
			int ImageNumber = imageNumberNode.get().get_value<int>();

			boost::optional<ptree&> urlNode = t.second.get_child_optional("ImageData.ExamURL");

			if(urlNode)
			{
				std::string urlFull = urlNode.get().get_value<std::string>();
				std::size_t filePos = urlFull.rfind('\\') + 1;
				std::string urlShort = urlFull.substr(filePos);
				
				imagesData.at(ImageNumber).filename = urlShort;
			}
			else
				std::cerr << "Image URL not found in XML\n";

//			for(auto p : t.second)
//				std::cout << '\t' << p.first << " : " << std::endl;
		}

		// std::cout << t.second.get_child("ID").get_value<std::string>().data();
	}
}
*/




