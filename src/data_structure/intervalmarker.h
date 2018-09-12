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

#ifndef INTERVALLMARKER_H
#define INTERVALLMARKER_H

#include <string>
#include <cstdint>
#include <vector>

class IntervalMarker
{
public:
	class Marker
	{
		friend class IntervalMarker;

		static std::size_t markerCounter;

// 		size_t internalId;
		std::string internalName;
		std::string name;
		bool defined;
		struct
		{
			uint8_t red;
			uint8_t green;
			uint8_t blue ;
		} color;


	public:
		Marker();
		Marker(const std::string& internalName, const std::string& name, uint8_t red, uint8_t green, uint8_t blue);

		uint8_t getRed  () const                                { return color.red  ; }
		uint8_t getGreen() const                                { return color.green; }
		uint8_t getBlue () const                                { return color.blue ; }

		const std::string getInternalName() const               { return internalName; }
		const std::string getName()         const               { return name;         }

		bool operator==(const Marker& other) const              { return internalName == other.internalName; }
		bool operator!=(const Marker& other) const              { return internalName != other.internalName; }
//
		bool isDefined() const                                  { return defined; }

// 		std::size_t getInternalId() const                       { return internalId; }
		static std::size_t getMaxInternalId()                   { return markerCounter; }
	};

	typedef std::vector<Marker> IntervalMarkerList;

	IntervalMarker(const std::string& internalName, const std::string& viewName);


// 	static IntervalMarker& getInstance()                        { static IntervalMarker instance; return instance; }

	const IntervalMarkerList& getIntervalMarkerList() const     { return markerList;        }
	std::size_t size() const                                    { return markerList.size(); }

	const Marker& getMarkerFromString(const std::string&) const;
	const Marker& getMarkerFromID    (int id) const;

	const std::string& getViewName    ()                 const  { return viewName    ;}
	const std::string& getInternalName()                 const  { return internalName;}

	IntervalMarkerList::const_iterator begin()                const { return markerList.begin(); }
	IntervalMarkerList::const_iterator end()                  const { return markerList.end(); }

	void addMarker(const Marker& marker);
private:
	IntervalMarkerList markerList;
	std::string internalName;
	std::string viewName    ;
};

#endif // INTERVALLMARKER_H
