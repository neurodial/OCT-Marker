#include"thicknessmaptemplates.h"

using SegmentlineType = OctData::Segmentationlines::SegmentlineType;

ThicknessmapTemplates::ThicknessmapTemplates()
{
	configurations.emplace_back(tr("Full Retina"               ), SegmentlineType::ILM , SegmentlineType::BM  , 100, 500, UseColorMap::hsv   );
	configurations.emplace_back(tr("Retinal Nerve Fiber Layer" ), SegmentlineType::ILM , SegmentlineType::RNFL, 0  , 100, UseColorMap::yellow);
	configurations.emplace_back(tr("Ganglion Cell Layer"       ), SegmentlineType::RNFL, SegmentlineType::GCL , 0  , 75 , UseColorMap::yellow);
	configurations.emplace_back(tr("Inner Plexiform Layer"     ), SegmentlineType::GCL , SegmentlineType::IPL , 0  , 75 , UseColorMap::yellow);
	configurations.emplace_back(tr("Inner Nuclear Layer"       ), SegmentlineType::IPL , SegmentlineType::INL , 0  , 75 , UseColorMap::yellow);
	configurations.emplace_back(tr("Outer Plexiform Layer"     ), SegmentlineType::INL , SegmentlineType::OPL , 0  , 60 , UseColorMap::yellow);
	configurations.emplace_back(tr("Outer Nuclear Layer"       ), SegmentlineType::OPL , SegmentlineType::ELM , 0  , 160, UseColorMap::yellow);
	configurations.emplace_back(tr("Retinal Pigment Epithelium"), SegmentlineType::RPE , SegmentlineType::BM  , 0  , 30 , UseColorMap::yellow);
	configurations.emplace_back(tr("Inner Retinal Layers"      ), SegmentlineType::ILM , SegmentlineType::ELM , 0  , 400, UseColorMap::yellow);
	configurations.emplace_back(tr("Photoreceptors"            ), SegmentlineType::ELM , SegmentlineType::BM  , 0  , 160, UseColorMap::yellow);
}
