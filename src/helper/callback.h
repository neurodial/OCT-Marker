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

	bool callback(double frac, const char* extraText = nullptr)
	{
		lastFrac = frac;
		if(parent == nullptr)
			return handleCallback(frac, extraText);
		return parent->callback(frac*taskFactor + tasksReady, extraText);
	}

	Callback createSubTask(double frac) { return Callback(this, frac); }


protected:
	virtual bool handleCallback(double /*frac*/, const char* /*extraText*/) { return true; }
};




class CallbackProgressDialog : public Callback
{
	QProgressDialog* progress;
	const QString text;

public:

	CallbackProgressDialog(const QString& text, const QString& buttonText)
	: progress(new QProgressDialog(text, buttonText, 0, 100))
	, text(text)
	{
		progress->setWindowModality(Qt::ApplicationModal);
		progress->show();
	}
	~CallbackProgressDialog()
	{
		delete progress;
	}

protected:
	virtual bool handleCallback(double frac, const char* extraText = nullptr) override
	{
		if(extraText)
			progress->setLabelText(QString("%1; %2").arg(text).arg(extraText));
		else
			progress->setLabelText(text);

		progress->setValue(static_cast<int>(frac*100));
		bool canceled = progress->wasCanceled();
// 		qDebug("canceled: %d\n", (int)canceled);
		return !canceled;
	}
};

#endif // CALLBACK_H
