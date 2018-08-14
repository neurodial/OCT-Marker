/*
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

#pragma once

#include<QEvent>


#include<widgets/numberpushbutton.h>
#include<octdata/datastruct/segmentationlines.h>

#include"bscanlayersegmentation.h"


class SegLineButton : public NumberPushButton
{
	Q_OBJECT;

	static QString getId(OctData::Segmentationlines::SegmentlineType type)
	{
		const auto keyListIt = std::find(BScanLayerSegmentation::keySeglines.begin(), BScanLayerSegmentation::keySeglines.end(), type);
		if(keyListIt != BScanLayerSegmentation::keySeglines.end())
			return QString::number(keyListIt - BScanLayerSegmentation::keySeglines.begin());
		return QString();
	}

	OctData::Segmentationlines::SegmentlineType type;
public:
	SegLineButton(OctData::Segmentationlines::SegmentlineType type, QWidget* parent = nullptr)
	: NumberPushButton(getId(type), parent)
	, type(type)
	{
		setText(OctData::Segmentationlines::getSegmentlineName(type));

		connect(this, &QPushButton::clicked, [this, type]() { clickSegLineButton(type);});
	}

protected:

	bool event(QEvent* e)
	{
		if(e->type() == QEvent::Enter)
			enterSegLineButton(type);
		if(e->type() == QEvent::Leave)
			leaveSegLineButton(type);

		return NumberPushButton::event(e); // Or whatever parent class you have.
	}

signals:
	void enterSegLineButton(OctData::Segmentationlines::SegmentlineType type);
	void leaveSegLineButton(OctData::Segmentationlines::SegmentlineType type);
	void clickSegLineButton(OctData::Segmentationlines::SegmentlineType type);
};

