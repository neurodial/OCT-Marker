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

#ifndef THICKNESSMAPLEGEND_H
#define THICKNESSMAPLEGEND_H

#include<QWidget>
#include<vector>


class QLabel;
class Colormap;

class ThicknessmapLegend : public QWidget
{
	QPixmap legend;

	int broderH = 5;
	const int broder         = 5;
	const int legendBarWidth = 20;
	int labelsMaxWidth  = 10;
	int labelsMaxHeight = 10;
	const int distanceBarLabel = 10;

	int labelXpos = 0;

	int value2HeightPos(double value, int height);

	class BarLabel
	{
		QLabel* label = nullptr;
		double value;
		int width = 0;
		int height = 0;
	public:
		BarLabel() = default;
		BarLabel(double value, QWidget* parent, int xpos);
		BarLabel(BarLabel&& other);
		~BarLabel();

		BarLabel(const BarLabel& other) = delete;
		BarLabel& operator=(const BarLabel& other) = delete;

		BarLabel& operator=(BarLabel&& other);

		void setHPos(int);
		void setValue(double v);

		void setVisible(bool s);
		bool isVisible() const;

		const QLabel* getLabel()                                 const { return label; }
		double getValue()                                        const { return value; }
		int getWidth()                                           const { return width; }
		int getHeight()                                          const { return height; }
	};

	const Colormap* colormap = nullptr;

	void adjustLabel(BarLabel& label, int height);
	void calcLabelsSize(int& maxHeight, int& maxWidth);
	void updateLabelsWidth();

	void updateLegend(const QSize& wgSize);
	void updateLegend();
	void updateLegendLabels();

	void updateInsertLabel(std::size_t nr, double value);

	void updateLabelXpos();

public:
	ThicknessmapLegend(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
	~ThicknessmapLegend();

	ThicknessmapLegend(const ThicknessmapLegend& other) = delete;
	ThicknessmapLegend& operator=(const ThicknessmapLegend& other) = delete;

	void setColormap(const Colormap* map)                           { colormap = map; updateLegend(); }

	virtual QSize minimumSizeHint() const;

protected:
	virtual void paintEvent(QPaintEvent* event);
	virtual void resizeEvent(QResizeEvent* event);


	std::vector<BarLabel> thicknessLabels;
};

#endif // THICKNESSMAPLEGEND_H
