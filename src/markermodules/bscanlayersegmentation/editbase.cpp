#include "editbase.h"


#include "bscanlayersegmentation.h"

int EditBase::getBScanHight() const
{
	return parent->getBScanHight();
}

int EditBase::getBScanWidth() const
{
	return parent->getBScanWidth();
}

void EditBase::requestFullUpdate()
{
	parent->requestFullUpdate();
}

