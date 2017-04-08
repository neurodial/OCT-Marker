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

	void addMarker(const Marker& marker);
private:
	IntervalMarkerList markerList;
	std::string internalName;
	std::string viewName    ;
};

#endif // INTERVALLMARKER_H
