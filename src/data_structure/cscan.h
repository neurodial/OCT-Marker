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

	SLOImage*           sloImage = nullptr;
	std::vector<BScan*> bscans;

	std::string         filename;
	std::string         path;

public:
	CScan();
	~CScan();

	const SLOImage* getSloImage() const { return sloImage; }

	void takeBScan(BScan* bscan);

	const BScan* getBScan(std::size_t pos) const;
	std::size_t  bscanCount() const { return bscans.size(); }

};

#endif // CSCAN_H
