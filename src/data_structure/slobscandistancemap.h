#ifndef SLOBSCANDISTANCEMAP_H
#define SLOBSCANDISTANCEMAP_H

#include<data_structure/matrx.h>
#include<limits>
#include<vector>

#include "point2d.h"


namespace OctData { class Series; }

class SloBScanDistanceMap
{
public:
	class InfoBScanDist
	{
	public:
		double      distance = std::numeric_limits<double>::infinity();
		std::size_t bscan    = std::numeric_limits<std::size_t>::max();
		std::size_t ascan    = std::numeric_limits<std::size_t>::max();
	};
	class PixelInfo
	{
	public:
		InfoBScanDist bscan1;
		InfoBScanDist bscan2;
		bool init = false;
	};

	typedef Matrix<PixelInfo> PreCalcDataMatrix;


	SloBScanDistanceMap();
	~SloBScanDistanceMap();

	void createData(const OctData::Series* series);


	const PreCalcDataMatrix* getDataMatrix() const { return preCalcDataMatrix; }

private:
	PreCalcDataMatrix* preCalcDataMatrix = nullptr;

};

#endif // SLOBSCANDISTANCEMAP_H
