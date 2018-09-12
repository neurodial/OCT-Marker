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
