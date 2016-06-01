#ifndef OCTXML_H
#define OCTXML_H

#include <vector>
#include <string>

class SLOImage;
class CScan;
class AlgoBatch;

class OctXml
{
public:

	static void readOctXml(const std::string& filename, CScan* cscan);
	
};

#endif // OCTXML_H
