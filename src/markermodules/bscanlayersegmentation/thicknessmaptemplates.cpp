#include"thicknessmaptemplates.h"


ThicknessmapTemplates::ThicknessmapTemplates()
{
	configurations.emplace_back(tr("Full Retina"               ), OctData::Segmentationlines::SegmentlineType::ILM , OctData::Segmentationlines::SegmentlineType::BM  , 100, 500, UseColorMap::hsv   );
	configurations.emplace_back(tr("RNFL"                      ), OctData::Segmentationlines::SegmentlineType::ILM , OctData::Segmentationlines::SegmentlineType::RNFL, 0  , 100, UseColorMap::yellow);
	configurations.emplace_back(tr("Ganglion Cell Layer"       ), OctData::Segmentationlines::SegmentlineType::RNFL, OctData::Segmentationlines::SegmentlineType::GCL , 0  , 75 , UseColorMap::yellow);
	configurations.emplace_back(tr("Inner Plexiform Layer"     ), OctData::Segmentationlines::SegmentlineType::GCL , OctData::Segmentationlines::SegmentlineType::IPL , 0  , 75 , UseColorMap::yellow);
	configurations.emplace_back(tr("Inner Nuclear Layer"       ), OctData::Segmentationlines::SegmentlineType::IPL , OctData::Segmentationlines::SegmentlineType::INL , 0  , 75 , UseColorMap::yellow);
	configurations.emplace_back(tr("Outer Plexiform Layer"     ), OctData::Segmentationlines::SegmentlineType::INL , OctData::Segmentationlines::SegmentlineType::OPL , 0  , 60 , UseColorMap::yellow);
	configurations.emplace_back(tr("Outer Nuclear Layer"       ), OctData::Segmentationlines::SegmentlineType::OPL , OctData::Segmentationlines::SegmentlineType::ELM , 0  , 160, UseColorMap::yellow);
	configurations.emplace_back(tr("Retinal Pigment Epithelium"), OctData::Segmentationlines::SegmentlineType::RPE , OctData::Segmentationlines::SegmentlineType::BM  , 0  , 30 , UseColorMap::yellow);
	configurations.emplace_back(tr("Inner Retinal Layers"      ), OctData::Segmentationlines::SegmentlineType::ILM , OctData::Segmentationlines::SegmentlineType::ELM , 0  , 400, UseColorMap::yellow);
	configurations.emplace_back(tr("Photoreceptors"            ), OctData::Segmentationlines::SegmentlineType::ELM , OctData::Segmentationlines::SegmentlineType::BM  , 0  , 160, UseColorMap::yellow);
}
