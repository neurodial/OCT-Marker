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

#ifndef OCTMARKERIO_H
#define OCTMARKERIO_H

#include<string>

#include<boost/property_tree/ptree_fwd.hpp>

#include<globaldefinitions.h>

namespace boost{ namespace filesystem { class path; }}


class OctMarkerIO
{
	static OctMarkerFileformat getDefaultFileFormat();

	OctMarkerFileformat defaultLoadedFormat = OctMarkerFileformat::Json;
	std::string loadedDefaultFilename;
	
	boost::property_tree::ptree* markerstree = nullptr;

	bool saveMarkersPrivat(const std::string& markersFilename, OctMarkerFileformat format);
	
public:
	explicit OctMarkerIO(boost::property_tree::ptree* markerTree);
	
	
	static const char* getFileExtension(OctMarkerFileformat format);
	static OctMarkerFileformat getFormatFromExtension(const std::string            & filename);
	static OctMarkerFileformat getFormatFromExtension(const boost::filesystem::path& markersPath);
	static std::string addMarkerExtension(const std::string& file, OctMarkerFileformat format);
	
	static OctMarkerFileformat int2Fileformat(int formatId);
	static int fileformat2Int(OctMarkerFileformat format);
	
	
	bool saveDefaultMarker(const std::string& octFilename);
	bool loadDefaultMarker(const std::string& octFilename);
	
	bool loadMarkers(const std::string&             markersFilename, OctMarkerFileformat format);
	bool loadMarkers(const boost::filesystem::path& markersPath    , OctMarkerFileformat format);
	bool saveMarkers(const std::string&             markersFilename, OctMarkerFileformat format);
	
	bool saveMarkersSeries(const std::string& markersFilename);
	bool addMarkersSeries (const std::string& markersFilename);
};

#endif // OCTMARKERIO_H
