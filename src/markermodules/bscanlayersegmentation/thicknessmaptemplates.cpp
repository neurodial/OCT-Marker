#include"thicknessmaptemplates.h"


ThicknessmapTemplates::ThicknessmapTemplates()
{
	configurations.emplace_back("Retina"                    , OctData::Segmentationlines::SegmentlineType::ILM , OctData::Segmentationlines::SegmentlineType::BM  , 100, 725, UseColorMap::hsv   );
	configurations.emplace_back("RNFL"                      , OctData::Segmentationlines::SegmentlineType::RNFL, OctData::Segmentationlines::SegmentlineType::ILM , 0  , 100, UseColorMap::yellow);
	configurations.emplace_back("Ganglion Cell Layer"       , OctData::Segmentationlines::SegmentlineType::GCL , OctData::Segmentationlines::SegmentlineType::RNFL, 0  , 75 , UseColorMap::yellow);
	configurations.emplace_back("Inner Plexiform Layer"     , OctData::Segmentationlines::SegmentlineType::IPL , OctData::Segmentationlines::SegmentlineType::GCL , 0  , 75 , UseColorMap::yellow);
	configurations.emplace_back("Inner Nuclear Layer"       , OctData::Segmentationlines::SegmentlineType::INL , OctData::Segmentationlines::SegmentlineType::IPL , 0  , 75 , UseColorMap::yellow);
	configurations.emplace_back("Outer Plexiform Layer"     , OctData::Segmentationlines::SegmentlineType::OPL , OctData::Segmentationlines::SegmentlineType::INL , 0  , 60 , UseColorMap::yellow);
	configurations.emplace_back("Outer Nuclear Layer"       , OctData::Segmentationlines::SegmentlineType::ELM , OctData::Segmentationlines::SegmentlineType::OPL , 0  , 160, UseColorMap::yellow);
	configurations.emplace_back("Retinal Pigment Epithelium", OctData::Segmentationlines::SegmentlineType::BM  , OctData::Segmentationlines::SegmentlineType::RPE , 0  , 30 , UseColorMap::yellow);
	configurations.emplace_back("Inner Retinal Layers"      , OctData::Segmentationlines::SegmentlineType::ELM , OctData::Segmentationlines::SegmentlineType::ILM , 0  , 400, UseColorMap::yellow);
	configurations.emplace_back("Photoreceptors"            , OctData::Segmentationlines::SegmentlineType::BM  , OctData::Segmentationlines::SegmentlineType::ELM , 0  , 160, UseColorMap::yellow);
}
