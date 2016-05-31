#ifndef CSCAN_H
#define CSCAN_H

#include <vector>

class SLOImage;
class BScan;

class CScan
{
	SLOImage*           sloImage = nullptr;
	std::vector<BScan*> bscans;

public:

	const SLOImage* getSloImage() const { return sloImage; }

};

#endif // CSCAN_H
