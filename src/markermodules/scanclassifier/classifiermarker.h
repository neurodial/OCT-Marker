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

#ifndef CLASSIFIERMARKER_H
#define CLASSIFIERMARKER_H

#include<string>
#include<vector>



class ClassifierMarker
{
public:
	enum class ClassifierChoiceType { Multible, Single }; // TODO

	class Marker
	{
		friend class ClassifierMarker;

		static std::size_t markerCounter;

		std::string internalName;
		std::string name;
		std::size_t id = 0;
	public:
		Marker(const std::string& internalName, const std::string& name);
		Marker();

		const std::string getInternalName()  const                     { return internalName; }
		const std::string getName()          const                     { return name;         }

		bool operator==(const Marker& other) const                     { return internalName == other.internalName; }
		bool operator!=(const Marker& other) const                     { return internalName != other.internalName; }

		bool operator==(const std::string& internalName) const         { return internalName == this->internalName; }

		static std::size_t getMaxInternalId()                          { return markerCounter; }
		std::size_t getId() const { return id; }
	};

	typedef std::vector<Marker> ClassifierMarkerList;

	ClassifierMarker(const std::string& internalName, const std::string& viewName, ClassifierChoiceType type);


// 	static IntervalMarker& getInstance()                        { static IntervalMarker instance; return instance; }

	const ClassifierMarkerList& getClassifierMarkerList()     const { return markerList;        }
	std::size_t size() const                                        { return markerList.size(); }

	const Marker* getMarkerFromString(const std::string&)     const;
	const Marker* getMarkerIDFromString(const std::string&)     const;
	const Marker& getMarkerFromID    (int id) const;

	const std::string& getViewName    ()                      const { return viewName    ;}
	const std::string& getInternalName()                      const { return internalName;}

	ClassifierMarkerList::const_iterator begin()              const { return markerList.begin(); }
	ClassifierMarkerList::const_iterator end()                const { return markerList.end(); }


	ClassifierChoiceType getclassifierChoiceType()            const { return classifierChoiceType; }

	void addMarker(const Marker& marker);
private:
	ClassifierMarkerList markerList     ;
	std::string          internalName   ;
	std::string          viewName       ;
	ClassifierChoiceType classifierChoiceType = ClassifierChoiceType::Single;
};

#endif // CLASSIFIERMARKER_H
