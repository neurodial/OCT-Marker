#include "cscan.h"

#include "sloimage.h"
#include "bscan.h"


CScan::CScan()
: sloImage(new SLOImage)
{

}


CScan::~CScan()
{
	delete sloImage;

	for(BScan* bscan : bscans)
		delete bscan;

}

void CScan::takeBScan(BScan* bscan)
{
	bscans.push_back(bscan);
}

const BScan* CScan::getBScan(std::size_t pos) const
{
	if(pos >= bscans.size())
		return nullptr;
	return bscans[pos];
}
