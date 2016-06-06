#ifndef INTERVALLMARKER_H
#define INTERVALLMARKER_H

#include <string>
#include <cstdint>
#include <vector>

class IntervallMarker
{
	IntervallMarker();
public:
	class Marker
	{
		const std::string name;
		struct
		{
			uint8_t red;
			uint8_t green;
			uint8_t blue ;
		} color;
	public:
		Marker(const std::string& name, uint8_t red, uint8_t green, uint8_t blue);

		uint8_t getRed  () const                                { return color.red  ; }
		uint8_t getGreen() const                                { return color.green; }
		uint8_t getBlue () const                                { return color.blue ; }

		const std::string getName() const                       { return name;        }
	};

	typedef std::vector<Marker> IntervallMarkerList;


	static IntervallMarker& getInstance()                       { static IntervallMarker instance; return instance; }

	const IntervallMarkerList& getIntervallMarkerList() const   { return markerList;        }
	std::size_t size() const                                    { return markerList.size(); }

private:
	IntervallMarkerList markerList;
};

#endif // INTERVALLMARKER_H
