#pragma once

#include <QObject>



class SignalBlocker
{
public:
	SignalBlocker(QObject* obj) : obj(obj)                          { obj->blockSignals(true ); }
	~SignalBlocker()                                                { obj->blockSignals(false); }

private:
	QObject* obj;
};
