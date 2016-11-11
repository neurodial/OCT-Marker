#ifndef CALLBACK_H
#define CALLBACK_H

#include <QProgressDialog>

class Callback
{
	Callback* parent  = nullptr;
	double taskFactor = 1;
	double tasksReady = 0;
	double lastFrac   = 0;

	Callback(Callback* p, double taskFactor)
	: parent(p)
	, taskFactor(taskFactor)
	, tasksReady(p->lastFrac)
	{ }
public:
	Callback() {}
	virtual ~Callback() {}

	bool callback(double frac)
	{
		lastFrac = frac;
		if(parent == nullptr)
			return handleCallback(frac);
		return parent->callback(frac*taskFactor + tasksReady);
	}

	Callback createSubTask(double frac) { return Callback(this, frac); }


protected:
	virtual bool handleCallback(double /*frac*/) { return true; }
};




class CallbackProgressDialog : public Callback
{
	QProgressDialog* progress;

public:

	CallbackProgressDialog(const QString& text, const QString& buttonText)
	: progress(new QProgressDialog(text, buttonText, 0, 100))
	{
		progress->setWindowModality(Qt::ApplicationModal);
		progress->show();
	}
	~CallbackProgressDialog()
	{
		delete progress;
	}

protected:
	virtual bool handleCallback(double frac)
	{
		progress->setValue(static_cast<int>(frac*100));
		return !progress->wasCanceled();
	}
};

#endif // CALLBACK_H
