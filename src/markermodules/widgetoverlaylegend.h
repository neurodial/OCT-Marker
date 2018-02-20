#pragma once

#include<QString>

class QWidget;


class WidgetOverlayLegend
{
	QWidget* widget = nullptr;
public:
	explicit WidgetOverlayLegend(QWidget* widget = nullptr) : widget(widget) {}
	virtual ~WidgetOverlayLegend() { }

	virtual QString getLatexCode() const { return QString(); }
	virtual QWidget* getWidget() { return widget; }
};
