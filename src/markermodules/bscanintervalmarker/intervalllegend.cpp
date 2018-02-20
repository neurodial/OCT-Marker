#include "intervalllegend.h"


#include"bscanintervalmarker.h"

QString IntervallLegend::getLatexCode() const
{
	QString out;
	const IntervalMarker* intervallMarkers = marker.getMarkersList();
	if(intervallMarkers)
	{
		out = "\\begin{tikzpicture}";
		out += "\n\t\\tikzstyle{legenditem}=[circle, draw, thick]";


		std::string oldName;
		for(const IntervalMarker::Marker& m : *intervallMarkers)
		{
			if(!m.isDefined())
				continue;

			std::string name = m.getInternalName();

			QString pos;
			if(!oldName.empty())
				pos = QString("below of = %1").arg(oldName.c_str());

			QString colorName = QString("color%1").arg(name.c_str());
			out += QString("\n\n\t\\definecolor{%1}{rgb}{%2,%3,%4};").arg(colorName)
			                                                    .arg(static_cast<double>(m.getRed  ())/255.)
			                                                    .arg(static_cast<double>(m.getGreen())/255.)
			                                                    .arg(static_cast<double>(m.getBlue ())/255.);

			out += QString("\n\t\\node[legenditem, fill=%1] (%2) [%3] {};").arg(colorName).arg(name.c_str()).arg(pos);
			out += QString("\n\t\\node [right of = %1, right, node distance = 1em] {%2};").arg(name.c_str()).arg(m.getName().c_str());

			oldName = name;
		}

		out += "\n\\end{tikzpicture}";
	}

	return out;
}
