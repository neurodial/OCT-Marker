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
