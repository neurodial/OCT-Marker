#ifndef CSCAN_H
#define CSCAN_H

#include <vector>
#include <string>

class SLOImage;
class BScan;

namespace cv { class Mat; }

class CScan
{
	friend class OctXml;
	friend class E2ERead;
	friend class VOLRead;

	SLOImage*                               sloImage = nullptr;
	std::vector<BScan*>                     bscans;

	std::string                             filename;
	std::string                             path;

public:
	CScan();
	~CScan();

	const SLOImage* getSloImage() const                         { return sloImage; }

	void takeBScan(BScan* bscan);

	const BScan* getBScan(std::size_t pos) const;
	std::size_t  bscanCount() const                             { return bscans.size(); }


	const std::vector<BScan*> getBscans() const                 { return bscans; }



};

#endif // CSCAN_H
