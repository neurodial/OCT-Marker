#ifndef OCTFILEELEMENT_H
#define OCTFILEELEMENT_H

#include <model/octdataitembase.h>

namespace OctData
{
	class OCT;
	class Patient;
	class Study;
	class Series;
}


class OctFileElement : public OctDataItemBase
{
	QString filename;

	OctData::OCT* octData = nullptr;
public:

};

#endif // OCTFILEELEMENT_H
