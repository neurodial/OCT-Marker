#ifndef INTERVALLMARKER_H
#define INTERVALLMARKER_H

#include <string>
#include <cstdint>
#include <vector>

class IntervalMarker
{
	IntervalMarker();
public:
	class Marker
	{
		friend class IntervalMarker;

		static std::size_t markerCounter;

		size_t internalId;
		std::string name;
		bool defined;
		struct
		{
			uint8_t red;
			uint8_t green;
			uint8_t blue ;
		} color;

		Marker(const std::string& name, uint8_t red, uint8_t green, uint8_t blue);

	public:
		Marker();

		uint8_t getRed  () const                                { return color.red  ; }
		uint8_t getGreen() const                                { return color.green; }
		uint8_t getBlue () const                                { return color.blue ; }

		const std::string getName() const                       { return name;        }

		bool operator==(const Marker& other) const              { return internalId == other.internalId; }
		bool operator!=(const Marker& other) const              { return internalId != other.internalId; }

		bool isDefined() const                                  { return defined; }

		std::size_t getInternalId() const                       { return internalId; }
		static std::size_t getMaxInternalId()                   { return markerCounter; }
	};

	typedef std::vector<Marker> IntervalMarkerList;


	static IntervalMarker& getInstance()                        { static IntervalMarker instance; return instance; }

	const IntervalMarkerList& getIntervalMarkerList() const     { return markerList;        }
	std::size_t size() const                                    { return markerList.size(); }

	const Marker& getMarkerFromString(const std::string&) const;
	const Marker& getMarkerFromID    (int id) const;
private:
	IntervalMarkerList markerList;
};

#endif // INTERVALLMARKER_H
