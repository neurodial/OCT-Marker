#ifndef OCTXML_H
#define OCTXML_H

#include <vector>
#include <string>

class AlgoBatch;

class OctXml
{
	struct OCTImage
	{
		std::string filename;
		std::vector<double> beginRetina;
		std::vector<double> endRetina;
	};
	
	std::vector<OCTImage> imagesData;
	
	std::string xmlPath;
	std::string xmlFilename;
	
public:

	OctXml(const std::string& filename);
	
	std::size_t getNumImages() const { return imagesData.size(); }
	
	void applyBatch(AlgoBatch& batch);
	
};

#endif // OCTXML_H
