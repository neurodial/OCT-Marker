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
	public:
		Marker();
		Marker(const std::string& internalName, const std::string& name);

		const std::string getInternalName()  const                     { return internalName; }
		const std::string getName()          const                     { return name;         }

		bool operator==(const Marker& other) const                     { return internalName == other.internalName; }
		bool operator!=(const Marker& other) const                     { return internalName != other.internalName; }

		bool operator==(const std::string& internalName) const         { return internalName == this->internalName; }

		static std::size_t getMaxInternalId()                          { return markerCounter; }
	};

	typedef std::vector<Marker> ClassifierMarkerList;

	ClassifierMarker(const std::string& internalName, const std::string& viewName, ClassifierChoiceType type);


// 	static IntervalMarker& getInstance()                        { static IntervalMarker instance; return instance; }

	const ClassifierMarkerList& getClassifierMarkerList()     const { return markerList;        }
	std::size_t size() const                                        { return markerList.size(); }

	const Marker* getMarkerFromString(const std::string&)     const;
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
