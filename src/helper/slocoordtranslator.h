#pragma once

#include <octdata/datastruct/coordslo.h>
#include <octdata/datastruct/series.h>
#include <octdata/datastruct/sloimage.h>

#include <data_structure/scalefactor.h>

class SloCoordTranslator
{
	OctData::ScaleFactor divide(const OctData::ScaleFactor& osf, const ScaleFactor& isf)
	{
		return OctData::ScaleFactor(osf.getX()/isf.getFactorX(), osf.getY()/isf.getFactorY(), 1);
	}

	OctData::CoordSLOpx mult(const OctData::CoordSLOpx& osf, const ScaleFactor& isf)
	{
		return OctData::CoordSLOpx(osf.getXf()*isf.getFactorX(), osf.getYf()*isf.getFactorY());
	}

	const OctData::SloImage&       sloImage  ;
	const OctData::ScaleFactor     factor    ;
	const OctData::CoordSLOpx      shift     ;
	const OctData::CoordTransform& cTransform;

public:
	SloCoordTranslator(const OctData::Series& series, const ScaleFactor scaleFactor = ScaleFactor())
	: sloImage  (series.getSloImage())
	, factor    (divide(sloImage.getScaleFactor(), scaleFactor))
	, shift     (mult  (sloImage.getShift()      , scaleFactor))
	, cTransform(sloImage.getTransform()                )
	{}

	OctData::CoordSLOpx operator()(const OctData::CoordSLOmm& in) const
	                                                               { return (cTransform * in)*factor + shift; }
};

