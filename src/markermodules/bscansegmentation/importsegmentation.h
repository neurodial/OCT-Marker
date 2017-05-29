#ifndef IMPORTSEGMENTATION_H
#define IMPORTSEGMENTATION_H

#include<string>

class BScanSegmentation;

class ImportSegmentation
{
public:
	static bool importOct(BScanSegmentation* markerManager, const std::string& filename);
};

#endif // IMPORTSEGMENTATION_H
