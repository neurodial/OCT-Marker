#include "octdataitembase.h"

OctDataItemBase::OctDataItemBase()
: parentItem()
{

}

OctDataItemBase::~OctDataItemBase()
{
	for(OctDataItemBase* ele : childVec)
		delete ele;
}



OctDataItemBase* OctDataItemBase::child(int number)
{
	if(number < static_cast<int>(childVec.size()))
		return childVec[number];
	return nullptr;
}



int OctDataItemBase::indexOf(const OctDataItemBase* item) const
{
	size_t pos = std::find(childVec.begin(), childVec.end(), item) - childVec.begin();

	if(pos < childVec.size())
		return pos;
	else
		return -1;
}
