#ifndef ACTIONCLASSES_H
#define ACTIONCLASSES_H

#include <QAction>

class IntValueAction : public QAction
{
	Q_OBJECT
	int valTrue;
public:
	IntValueAction(const int t, QObject* parent, bool checkable = true) : QAction(parent), valTrue(t)
	{
		setCheckable(checkable);
		if(checkable)
			connect(this, &QAction::toggled, this, &IntValueAction::onTriggered);
		else
			connect(this, &QAction::triggered, this, &IntValueAction::onTriggered);
	}
signals:
	void triggered(int v);

private slots:
	void onTriggered(bool v)
	{
		if(v || !isCheckable())
			emit(triggered(valTrue));
	}
public slots:
	void valueChanged(int v)
	{
		if(v == valTrue)
			setChecked(true);
		else
			setChecked(false);
	}
};

class SizetValueAction : public QAction
{
	Q_OBJECT
	std::size_t valTrue;
public:
	SizetValueAction(size_t t, QObject* parent) : QAction(parent), valTrue(t)
	{
		setCheckable(true);
		connect(this, &QAction::toggled, this, &SizetValueAction::onTriggered);
	}
signals:
	void triggered(std::size_t v);

private slots:
	void onTriggered(bool v)
	{
		if(v)
			emit(triggered(valTrue));
	}
public slots:
	void valueChanged(std::size_t v)
	{
		if(v == valTrue)
			setChecked(true);
		else
			setChecked(false);
	}
};

#endif // ACTIONCLASSES_H
