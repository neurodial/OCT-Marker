#ifndef INTERVALLLEGEND_H
#define INTERVALLLEGEND_H

#include"../widgetoverlaylegend.h"

class BScanIntervalMarker;


class IntervallLegend : public WidgetOverlayLegend
{
	const BScanIntervalMarker& marker;
public:
	IntervallLegend(const BScanIntervalMarker& marker)
	: marker(marker)
	{}

	QString getLatexCode() const override;
};

#endif // INTERVALLLEGEND_H
