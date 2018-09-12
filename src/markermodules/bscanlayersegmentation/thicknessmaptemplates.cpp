/*
 * Copyright (c) 2018 Kay Gawlik <kaydev@amarunet.de> <kay.gawlik@beuth-hochschule.de> <kay.gawlik@charite.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include"thicknessmaptemplates.h"

using SegmentlineType = OctData::Segmentationlines::SegmentlineType;

ThicknessmapTemplates::ThicknessmapTemplates()
{
	configurations.emplace_back(tr("Full Retina"                 ), SegmentlineType::ILM , SegmentlineType::BM  , 100, 500, UseColorMap::hsv   );
	configurations.emplace_back(tr("Retinal Nerve Fiber Layer"   ), SegmentlineType::ILM , SegmentlineType::RNFL,   0, 100, UseColorMap::yellow);
	configurations.emplace_back(tr("Ganglion Cell Layer"         ), SegmentlineType::RNFL, SegmentlineType::GCL ,   0,  75, UseColorMap::yellow);
	configurations.emplace_back(tr("combined GCL and IPL (GCIPL)"), SegmentlineType::RNFL, SegmentlineType::IPL ,  10, 150, UseColorMap::hsv   ); // GCIPL – combined Ganglion cell and inner plexiform layer
	configurations.emplace_back(tr("Inner Plexiform Layer"       ), SegmentlineType::GCL , SegmentlineType::IPL ,   0,  75, UseColorMap::yellow);
	configurations.emplace_back(tr("Inner Nuclear Layer"         ), SegmentlineType::IPL , SegmentlineType::INL ,   0,  75, UseColorMap::yellow);
	configurations.emplace_back(tr("Outer Plexiform Layer"       ), SegmentlineType::INL , SegmentlineType::OPL ,   0,  60, UseColorMap::yellow);
	configurations.emplace_back(tr("Outer Nuclear Layer"         ), SegmentlineType::OPL , SegmentlineType::ELM ,   0, 160, UseColorMap::yellow);
	configurations.emplace_back(tr("Retinal Pigment Epithelium"  ), SegmentlineType::RPE , SegmentlineType::BM  ,   0,  30, UseColorMap::yellow);
	configurations.emplace_back(tr("Inner Retinal Layers"        ), SegmentlineType::ILM , SegmentlineType::ELM ,   0, 400, UseColorMap::yellow);
	configurations.emplace_back(tr("Photoreceptors"              ), SegmentlineType::ELM , SegmentlineType::BM  ,   0, 160, UseColorMap::yellow);
}
