#include "octxml.h"

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





